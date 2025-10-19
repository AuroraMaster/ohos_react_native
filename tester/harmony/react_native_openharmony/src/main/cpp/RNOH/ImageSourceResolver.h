/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once

#include <ReactCommon/RuntimeExecutor.h>
#include <react/renderer/imagemanager/primitives.h>
#include "RNInstance.h"
#include "RNOH/ArkTSMessageHub.h"
#include "RNOH/Assert.h"
#include "RNOH/RNInstance.h"
#include "RNOH/TaskExecutor/TaskExecutor.h"
#include "RNOHCorePackage/TurboModules/ImageLoaderTurboModule.h"
#include <regex>
#include <filesystem>

namespace rnoh {

constexpr int MAX_REMOTE_DISK_CACHE_CAP = 128;

class ImageSourceResolver : public ArkTSMessageHub::Observer {
public:
    using Shared = std::shared_ptr<ImageSourceResolver>;

    ImageSourceResolver(ArkTSMessageHub::Shared const &subject, std::string cacheDir)
        : ArkTSMessageHub::Observer(subject), m_cacheDir(std::move(cacheDir)) {
        try {
            for (const auto &entry : std::filesystem::directory_iterator(m_cacheDir)) {
                if (entry.is_regular_file()) {
                    std::string filename = entry.path().filename().string();
                    filename.erase(std::remove(filename.begin(), filename.end(), '/'), filename.end());
                    auto cachedKey = getDiskCacheKey(filename);
                    if (cachedKey.empty()) {
                        continue;
                    }
                    updateImageDiskCacheMap(cachedKey);
                }
            }
        } catch (const std::exception &e) {
            LOG(ERROR) << "Error reading directory: " << e.what();
        }
    }

    class ImageSourceUpdateListener {
    public:
        std::string observedUri;

        ImageSourceUpdateListener(ImageSourceResolver::Shared const &imageSourceResolver)
            : m_imageSourceResolver(imageSourceResolver){};

        ~ImageSourceUpdateListener() { m_imageSourceResolver->removeListener(this); }

        virtual void onImageSourceCacheUpdate() = 0;

    private:
        ImageSourceResolver::Shared const &m_imageSourceResolver;
    };

    //@thread_safe
    std::string resolveImageSource(ImageSourceUpdateListener &listener,
                                   facebook::react::LayoutMetrics const &layoutMetrics,
                                   facebook::react::ImageSources const &newSourcesCandidates) {
        auto imageCandidate = getBestSourceForSize(layoutMetrics.frame.size.width, layoutMetrics.frame.size.height,
                                                   layoutMetrics.pointScaleFactor, newSourcesCandidates);

        const std::string &imageUri = imageCandidate.uri;
        if (imageCandidate.type != facebook::react::ImageSource::Type::Remote) {
            return imageUri;
        }

        {
            // Subscribe to get information about prefetched URIs.
            std::lock_guard<std::mutex> lock(m_uriListenersMapMutex);
            if (uriListenersMap.find(imageUri) == uriListenersMap.end()) {
                removeListener(&listener);
                addListenerForURI(imageUri, &listener);
            }
        }

        {
            std::shared_lock sharedLock(m_remoteImageSourceMapSharedMutex);
            auto it = remoteImageSourceMap.find(imageUri);
            if (it != remoteImageSourceMap.end()) {
                return it->second;
            }
        }

        std::string resolvedFileUri;
        {
            std::unique_lock uniqueLock(m_remoteImageSourceMapSharedMutex);
            // Check again to prevent other threads from writing
            auto it = remoteImageSourceMap.find(imageUri);
            if (it == remoteImageSourceMap.end()) {
                resolvedFileUri = getPrefetchedImageFileUri(imageUri);
                if (!resolvedFileUri.empty()) {
                    remoteImageSourceMap.emplace(imageUri, resolvedFileUri);
                    return resolvedFileUri;
                }
            }
        }
        return imageUri;
    }

    // Based on Android MultiSourceHelper class, see:
    // https://github.com/facebook/react-native/blob/v0.72.5/packages/react-native/ReactAndroid/src/main/java/com/facebook/react/views/imagehelper/MultiSourceHelper.java
    facebook::react::ImageSource getBestSourceForSize(double width, double height, double pointScaleFactor,
                                                      facebook::react::ImageSources const &sources) {
        RNOH_ASSERT_MSG(sources.size() > 0, "ImageSources vector should not be empty");

        if (sources.size() == 1) {
            return sources[0];
        }

        auto bestSourceIndex = 0;
        auto targetImagePixels = width * height * pointScaleFactor;
        auto bestPixelsFit = std::numeric_limits<double>::max();

        for (auto i = 0; i < sources.size(); i++) {
            auto &source = sources[i];
            auto imagePixels = source.size.width * source.size.height;
            auto pixelsFit = std::abs(1.0 - imagePixels / targetImagePixels);

            if (pixelsFit < bestPixelsFit) {
                bestPixelsFit = pixelsFit;
                bestSourceIndex = i;
            }
        }

        return sources[bestSourceIndex];
    }

    void addListenerForURI(const std::string &uri, ImageSourceUpdateListener *listener) {
        listener->observedUri = uri;
        auto it = uriListenersMap.find(uri);
        if (it == uriListenersMap.end()) {
            uriListenersMap.emplace(uri, std::initializer_list<ImageSourceUpdateListener *>{listener});
            return;
        }
        if (std::find(it->second.begin(), it->second.end(), listener) != it->second.end()) {
            return;
        }
        it->second.push_back(listener);
    }

    void removeListenerForURI(const std::string &uri, ImageSourceUpdateListener *listener) {
        auto it = uriListenersMap.find(uri);
        if (it == uriListenersMap.end()) {
            return;
        }
        auto &listeners = it->second;
        auto listenerPos = std::find(listeners.begin(), listeners.end(), listener);
        if (listenerPos != listeners.end()) {
            listeners.erase(listenerPos);
            if (listeners.empty()) {
                uriListenersMap.erase(uri);
            }
        }
    }

    void removeListener(ImageSourceUpdateListener *listener) { removeListenerForURI(listener->observedUri, listener); }

protected:
    virtual void onMessageReceived(const ArkTSMessage &message) override {
        if (message.name == "UPDATE_IMAGE_SOURCE_MAP") {
            auto remoteUri = message.payload["remoteUri"].asString();
            auto fileUri = message.payload["fileUri"].asString();
            remoteImageSourceMap.insert_or_assign(remoteUri, fileUri);
            auto it = uriListenersMap.find(remoteUri);
            if (it == uriListenersMap.end()) {
                return;
            }
            auto &listeners = it->second;
            for (auto listener : listeners) {
                listener->onImageSourceCacheUpdate();
                removeListenerForURI(remoteUri, listener);
            }
        }
    }

private:
    std::mutex m_uriListenersMapMutex;
    std::shared_mutex m_remoteImageSourceMapSharedMutex;
    std::mutex m_imageDiskCacheMapMutex;
    std::unordered_map<std::string, std::vector<ImageSourceUpdateListener *>> uriListenersMap;
    std::unordered_map<std::string, std::string> remoteImageSourceMap;
    std::unordered_map<std::string, bool> imageDiskCacheMap;
    std::string m_cacheDir;

    void updateImageDiskCacheMap(const std::string &cacheKey) {
        // Disk cache updates are performed only in the main thread
        std::lock_guard lock(m_imageDiskCacheMapMutex);
        imageDiskCacheMap[cacheKey] = true;
        // Check if memoryCache size exceeds maxSize, and if so, remove the oldest entry
        if (imageDiskCacheMap.size() > MAX_REMOTE_DISK_CACHE_CAP) {
            imageDiskCacheMap.erase(imageDiskCacheMap.begin());
        }
    }

    std::string getPrefetchedImageFileUri(const std::string &imageUri) {
        if (imageUri.empty()) {
            return {};
        }
        const auto diskCacheKey = getDiskCacheKey(imageUri);
        const auto imageLocation = getFilePath(diskCacheKey);
        std::lock_guard lock(m_imageDiskCacheMapMutex);
        if (auto it = imageDiskCacheMap.find(diskCacheKey); it != imageDiskCacheMap.end()) {
            if (std::filesystem::exists(imageLocation)) {
                return "file://" + imageLocation;
            } else {
                imageDiskCacheMap.erase(diskCacheKey);
            }
        }
        return imageUri;
    }

    std::string getFilePath(const std::string &key) { return m_cacheDir + '/' + key; }

    std::string getDiskCacheKey(const std::string &imageUri) {
        static const std::regex imageUriReg("[^a-zA-Z0-9 \\-]");
        return std::regex_replace(imageUri, imageUriReg, "");
    }
};

} // namespace rnoh