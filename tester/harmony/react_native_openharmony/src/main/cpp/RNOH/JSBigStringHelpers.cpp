/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#include "JSBigStringHelpers.h"

using facebook::react::JSBigFileString;
using facebook::react::JSBigString;

namespace rnoh::JSBigStringHelpers {

class JSBigStdVecString final : public JSBigString {
public:
    explicit JSBigStdVecString(std::vector<uint8_t> buffer, bool isAscii = false)
        : m_isAscii(isAscii), m_buffer(std::move(buffer))
    {
    }

    bool isAscii() const override
    {
        return m_isAscii;
    }

    const char *c_str() const override
    {
        return reinterpret_cast<char const *>(m_buffer.data());
    }

    size_t size() const override
    {
        return m_buffer.size();
    }

private:
    bool m_isAscii;
    std::vector<uint8_t> m_buffer;
};

std::unique_ptr<JSBigString const> fromBuffer(std::vector<uint8_t> buffer)
{
    try {
        return std::make_unique<JSBigStdVecString>(std::move(buffer));
    } catch (...) {
        return nullptr;
    }
}

std::unique_ptr<JSBigString const> fromRawFilePath(
    std::string const &rawFilePath,
    NativeResourceManager *nativeResourceManager)
{
    std::unique_ptr<RawFile, decltype(&OH_ResourceManager_CloseRawFile)>
        bundleRawFile(OH_ResourceManager_OpenRawFile(nativeResourceManager,
            rawFilePath.c_str()),
            OH_ResourceManager_CloseRawFile);
    if (!bundleRawFile) {
        throw std::runtime_error("Failed to open raw file: " + rawFilePath);
    }
    RawFileDescriptor bundleRawFileDescriptor{};
    auto success = OH_ResourceManager_GetRawFileDescriptorData(
        bundleRawFile.get(), &bundleRawFileDescriptor);
    if (!success) {
        throw std::runtime_error("Failed to get raw file descriptor data: " + rawFilePath);
    }

    auto result = std::make_unique<JSBigFileString>(bundleRawFileDescriptor.fd,
        bundleRawFileDescriptor.length,
        bundleRawFileDescriptor.start);

    OH_ResourceManager_ReleaseRawFileDescriptorData(&bundleRawFileDescriptor);

    return result;
}

std::unique_ptr<JSBigString const> fromFilePath(std::string const &filePath)
{
    return JSBigFileString::fromPath(filePath);
}
} // namespace rnoh::JSBigStringHelpers