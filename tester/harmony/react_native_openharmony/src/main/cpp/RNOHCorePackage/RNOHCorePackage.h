/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

#pragma once
#include <react/renderer/components/image/ImageComponentDescriptor.h>
#include <react/renderer/components/modal/ModalHostViewComponentDescriptor.h>
#include <react/renderer/components/rncore/ComponentDescriptors.h>
#include <react/renderer/components/scrollview/ScrollViewComponentDescriptor.h>
#include <react/renderer/components/text/ParagraphComponentDescriptor.h>
#include <react/renderer/components/text/RawTextComponentDescriptor.h>
#include <react/renderer/components/text/TextComponentDescriptor.h>
#include <react/renderer/components/textinput/TextInputComponentDescriptor.h>
#include <react/renderer/components/view/ViewComponentDescriptor.h>
#include "RNOH/ArkTSTurboModule.h"
#include "RNOH/Package.h"
#include "RNOHCorePackage/ComponentBinders/ActivityIndicatorComponentJSIBinder.h"
#include "RNOHCorePackage/ComponentBinders/ActivityIndicatorComponentNapiBinder.h"
#include "RNOHCorePackage/ComponentBinders/ImageComponentJSIBinder.h"
#include "RNOHCorePackage/ComponentBinders/ImageComponentNapiBinder.h"
#include "RNOHCorePackage/ComponentBinders/ModalHostViewJSIBinder.h"
#include "RNOHCorePackage/ComponentBinders/ModalHostViewNapiBinder.h"
#include "RNOHCorePackage/ComponentBinders/PullToRefreshViewJSIBinder.h"
#include "RNOHCorePackage/ComponentBinders/PullToRefreshViewNapiBinder.h"
#include "RNOHCorePackage/ComponentBinders/ScrollViewComponentJSIBinder.h"
#include "RNOHCorePackage/ComponentBinders/ScrollViewComponentNapiBinder.h"
#include "RNOHCorePackage/ComponentBinders/SwitchComponentJSIBinder.h"
#include "RNOHCorePackage/ComponentBinders/SwitchComponentNapiBinder.h"
#include "RNOHCorePackage/ComponentBinders/TextComponentJSIBinder.h"
#include "RNOHCorePackage/ComponentBinders/TextComponentNapiBinder.h"
#include "RNOHCorePackage/ComponentBinders/TextInputComponentJSIBinder.h"
#include "RNOHCorePackage/ComponentBinders/TextInputComponentNapiBinder.h"
#include "RNOHCorePackage/ComponentBinders/ViewComponentJSIBinder.h"
#include "RNOHCorePackage/ComponentBinders/ViewComponentNapiBinder.h"
#include "RNOHCorePackage/ComponentInstances/ActivityIndicatorComponentInstance.h"
#include "RNOHCorePackage/ComponentInstances/ImageComponentInstance.h"
#include "RNOHCorePackage/ComponentInstances/ModalHostViewComponentInstance.h"
#include "RNOHCorePackage/ComponentInstances/PullToRefreshViewComponentInstance.h"
#include "RNOHCorePackage/ComponentInstances/ScrollViewComponentInstance.h"
#include "RNOHCorePackage/ComponentInstances/SwitchComponentInstance.h"
#include "RNOHCorePackage/ComponentInstances/TextComponentInstance.h"
#include "RNOHCorePackage/ComponentInstances/TextInputComponentInstance.h"
#include "RNOHCorePackage/ComponentInstances/ViewComponentInstance.h"
#include "RNOHCorePackage/ComponentInstances/CustomNodeComponentInstance.h"
#include "RNOHCorePackage/EventEmitRequestHandlers/ImageEventEmitRequestHandler.h"
#include "RNOHCorePackage/EventEmitRequestHandlers/ModalEventEmitRequestHandler.h"
#include "RNOHCorePackage/EventEmitRequestHandlers/PullToRefreshViewEventEmitRequestHandler.h"
#include "RNOHCorePackage/EventEmitRequestHandlers/ScrollEventEmitRequestHandler.h"
#include "RNOHCorePackage/EventEmitRequestHandlers/SwitchEventEmitRequestHandler.h"
#include "RNOHCorePackage/EventEmitRequestHandlers/TextInputEventEmitRequestHandler.h"
#include "RNOHCorePackage/EventEmitRequestHandlers/TouchEventEmitRequestHandler.h"
#include "RNOHCorePackage/EventEmitRequestHandlers/ViewEventEmitRequestHandler.h"
#include "RNOHCorePackage/GlobalBinders/BlobCollectorJSIBinder.h"
#include "RNOHCorePackage/TurboModules/AccessibilityInfoTurboModule.h"
#include "RNOHCorePackage/TurboModules/AlertManagerTurboModule.h"
#include "RNOHCorePackage/TurboModules/Animated/NativeAnimatedTurboModule.h"
#include "RNOHCorePackage/TurboModules/AppStateTurboModule.h"
#include "RNOHCorePackage/TurboModules/AppearanceTurboModule.h"
#include "RNOHCorePackage/TurboModules/BlobTurboModule.h"
#include "RNOHCorePackage/TurboModules/DevLoadingViewTurboModule.h"
#include "RNOHCorePackage/TurboModules/DevMenuTurboModule.h"
#include "RNOHCorePackage/TurboModules/DevSettingsTurboModule.h"
#include "RNOHCorePackage/TurboModules/DeviceEventManagerTurboModule.h"
#include "RNOHCorePackage/TurboModules/DeviceInfoTurboModule.h"
#include "RNOHCorePackage/TurboModules/ExceptionsManagerTurboModule.h"
#include "RNOHCorePackage/TurboModules/FileReaderTurboModule.h"
#include "RNOHCorePackage/TurboModules/I18nManagerTurboModule.h"
#include "RNOHCorePackage/TurboModules/ImageLoaderTurboModule.h"
#include "RNOHCorePackage/TurboModules/LinkingManagerTurboModule.h"
#include "RNOHCorePackage/TurboModules/LogBoxTurboModule.h"
#include "RNOHCorePackage/TurboModules/NetworkingTurboModule.h"
#include "RNOHCorePackage/TurboModules/PlatformColorTurboModule.h"
#include "RNOHCorePackage/TurboModules/PlatformConstantsTurboModule.h"
#include "RNOHCorePackage/TurboModules/SafeAreaTurboModule.h"
#include "RNOHCorePackage/TurboModules/ShareTurboModule.h"
#include "RNOHCorePackage/TurboModules/SourceCodeTurboModule.h"
#include "RNOHCorePackage/TurboModules/StatusBarTurboModule.h"
#include "RNOHCorePackage/TurboModules/TimingTurboModule.h"
#include "RNOHCorePackage/TurboModules/ToastAndroidTurboModule.h"
#include "RNOHCorePackage/TurboModules/VibrationTurboModule.h"
#include "RNOHCorePackage/TurboModules/WebSocketTurboModule.h"

namespace rnoh {

class RNOHCoreTurboModuleFactoryDelegate : public TurboModuleFactoryDelegate {
 public:
  SharedTurboModule createTurboModule(Context ctx, const std::string& name)
      const override {
    if (name == "AccessibilityInfo") {
      return std::make_shared<AccessibilityInfoTurboModule>(ctx, name);
    } else if (name == "AlertManager") {
      return std::make_shared<AlertManagerTurboModule>(ctx, name);
    } else if (name == "Appearance") {
      return std::make_shared<AppearanceTurboModule>(ctx, name);
    } else if (name == "AppState") {
      return std::make_shared<AppStateTurboModule>(ctx, name);
    } else if (name == "BlobModule") {
      return std::make_shared<BlobTurboModule>(ctx, name);
    } else if (name == "DeviceEventManager") {
      return std::make_shared<DeviceEventManagerTurboModule>(ctx, name);
    } else if (name == "DeviceInfo") {
      return std::make_shared<DeviceInfoTurboModule>(ctx, name);
    } else if (name == "ExceptionsManager") {
      return std::make_shared<ExceptionsManagerTurboModule>(ctx, name);
    } else if (name == "FileReaderModule") {
      return std::make_shared<FileReaderTurboModule>(ctx, name);
    } else if (name == "ImageLoader") {
      return std::make_shared<ImageLoaderTurboModule>(ctx, name);
    } else if (name == "KeyboardObserver") {
      return std::make_shared<ArkTSTurboModule>(ctx, name);
    } else if (name == "LinkingManager") {
      return std::make_shared<LinkingManagerTurboModule>(ctx, name);
    } else if (name == "NativeAnimatedTurboModule") {
      return std::make_shared<NativeAnimatedTurboModule>(ctx, name);
    } else if (name == "Networking") {
      return std::make_shared<NetworkingTurboModule>(ctx, name);
    } else if (name == "PlatformColor") {
      return std::make_shared<PlatformColorTurboModule>(ctx, name);
    } else if (name == "PlatformConstants") {
      return std::make_shared<PlatformConstantsTurboModule>(ctx, name);
    } else if (name == "SourceCode") {
      return std::make_shared<SourceCodeTurboModule>(ctx, name);
    } else if (name == "StatusBarManager") {
      return std::make_shared<StatusBarTurboModule>(ctx, name);
    } else if (name == "Timing") {
      return std::make_shared<TimingTurboModule>(ctx, name);
    } else if (name == "WebSocketModule") {
      return std::make_shared<WebSocketTurboModule>(ctx, name);
    } else if (name == "SafeAreaTurboModule") {
      return std::make_shared<SafeAreaTurboModule>(ctx, name);
    } else if (name == "I18nManager") {
      return std::make_shared<I18nManagerTurboModule>(ctx, name);
    } else if (name == "DevSettings") {
      return std::make_shared<DevSettingsTurboModule>(ctx, name);
    } else if (name == "DevMenu") {
      return std::make_shared<DevMenuTurboModule>(ctx, name);
    } else if (name == "LogBox") {
      return std::make_shared<LogBoxTurboModule>(ctx, name);
    } else if (name == "DevLoadingView") {
      return std::make_shared<DevLoadingViewTurboModule>(ctx, name);
    } else if (name == "Vibration") {
      return std::make_shared<VibrationTurboModule>(ctx, name);
    } else if (name == "ToastAndroid") {
      return std::make_shared<ToastAndroidTurboModule>(ctx, name);
    } else if (name == "ShareModule") {
      return std::make_shared<ShareTurboModule>(ctx, name);
    }
    return nullptr;
  };
};

class RNOHCorePackage : public Package {
 public:
  RNOHCorePackage(Package::Context ctx) : Package(ctx){};

  std::unique_ptr<TurboModuleFactoryDelegate> createTurboModuleFactoryDelegate()
      override {
    return std::make_unique<RNOHCoreTurboModuleFactoryDelegate>();
  }

  ComponentInstance::Shared createComponentInstance(
      const ComponentInstance::Context& ctx) override {
    if (ctx.componentName == "RootView") {
      return std::make_shared<ViewComponentInstance>(std::move(ctx));
    }
    if (ctx.componentName == "View") {
      return std::make_shared<CustomNodeComponentInstance>(std::move(ctx));
    }
    if (ctx.componentName == "Paragraph") {
      return std::make_shared<TextComponentInstance>(std::move(ctx));
    }
    if (ctx.componentName == "TextInput") {
      return std::make_shared<TextInputComponentInstance>(std::move(ctx));
    }
    if (ctx.componentName == "ScrollView") {
      return std::make_shared<ScrollViewComponentInstance>(std::move(ctx));
    }
    if (ctx.componentName == "Image") {
      return std::make_shared<ImageComponentInstance>(std::move(ctx));
    }
    if (ctx.componentName == "ActivityIndicatorView") {
      return std::make_shared<ActivityIndicatorComponentInstance>(
          std::move(ctx));
    }
    if (ctx.componentName == "ModalHostView") {
      return std::make_shared<ModalHostViewComponentInstance>(std::move(ctx));
    }
    if (ctx.componentName == "Switch") {
      return std::make_shared<SwitchComponentInstance>(std::move(ctx));
    }
    if (ctx.componentName == "PullToRefreshView") {
      return std::make_shared<PullToRefreshViewComponentInstance>(
          std::move(ctx));
    }
    return nullptr;
  }

  std::vector<facebook::react::ComponentDescriptorProvider>
  createComponentDescriptorProviders() override {
    return {
        facebook::react::concreteComponentDescriptorProvider<
            facebook::react::ViewComponentDescriptor>(),
        facebook::react::concreteComponentDescriptorProvider<
            facebook::react::ImageComponentDescriptor>(),
        facebook::react::concreteComponentDescriptorProvider<
            facebook::react::TextComponentDescriptor>(),
        facebook::react::concreteComponentDescriptorProvider<
            facebook::react::RawTextComponentDescriptor>(),
        facebook::react::concreteComponentDescriptorProvider<
            facebook::react::ParagraphComponentDescriptor>(),
        facebook::react::concreteComponentDescriptorProvider<
            facebook::react::TextInputComponentDescriptor>(),
        facebook::react::concreteComponentDescriptorProvider<
            facebook::react::ScrollViewComponentDescriptor>(),
        facebook::react::concreteComponentDescriptorProvider<
            facebook::react::PullToRefreshViewComponentDescriptor>(),
        facebook::react::concreteComponentDescriptorProvider<
            facebook::react::ModalHostViewComponentDescriptor>(),
        facebook::react::concreteComponentDescriptorProvider<
            facebook::react::SwitchComponentDescriptor>(),
        facebook::react::concreteComponentDescriptorProvider<
            facebook::react::ActivityIndicatorViewComponentDescriptor>()};
  }

  ComponentJSIBinderByString createComponentJSIBinderByName() override {
    return {
        {"RCTView", std::make_shared<ViewComponentJSIBinder>()},
        {"RCTImageView", std::make_shared<ImageComponentJSIBinder>()},
        {"RCTVirtualText", std::make_shared<ViewComponentJSIBinder>()},
        {"Paragraph", std::make_shared<TextComponentJSIBinder>()},
        {"RCTSinglelineTextInputView",
         std::make_shared<TextInputComponentJSIBinder>()},
        {"RCTScrollView", std::make_shared<ScrollViewComponentJSIBinder>()},
        {"RCTMultilineTextInputView",
         std::make_shared<TextInputComponentJSIBinder>()},
        {"RCTScrollContentView", std::make_shared<ViewComponentJSIBinder>()},
        {"RCTModalHostView", std::make_shared<ModalHostViewJSIBinder>()},
        {"RCTSwitch", std::make_shared<SwitchComponentJSIBinder>()},
        {"RCTRefreshControl", std::make_shared<PullToRefreshViewJSIBinder>()},
        {"RCTActivityIndicatorView",
         std::make_shared<ActivityIndicatorComponentJSIBinder>()}};
  };

  ComponentNapiBinderByString createComponentNapiBinderByName() override {
    return {
        {"RootView", std::make_shared<ViewComponentNapiBinder>()},
        {"View", std::make_shared<ViewComponentNapiBinder>()},
        {"Image", std::make_shared<ImageComponentNapiBinder>()},
        {"Paragraph",
         std::make_shared<TextComponentNapiBinder>(
             this->m_ctx.shadowViewRegistry)},
        {"ScrollView", std::make_shared<ScrollViewComponentNapiBinder>()},
        {"TextInput", std::make_shared<TextInputComponentNapiBinder>()},
        {"ModalHostView", std::make_shared<ModalHostViewNapiBinder>()},
        {"Switch", std::make_shared<SwitchComponentNapiBinder>()},
        {"PullToRefreshView", std::make_shared<PullToRefreshViewNapiBinder>()},
        {"ActivityIndicatorView",
         std::make_shared<ActivityIndicatorComponentNapiBinder>()}};
  };

  EventEmitRequestHandlers createEventEmitRequestHandlers() override {
    return {
        std::make_shared<TouchEventEmitRequestHandler>(),
        std::make_shared<TextInputEventEmitRequestHandler>(),
        std::make_shared<ScrollEventEmitRequestHandler>(),
        std::make_shared<ModalEventEmitRequestHandler>(),
        std::make_shared<SwitchEventEmitRequestHandler>(),
        std::make_shared<PullToRefreshViewEventEmitRequestHandler>(),
        std::make_shared<ImageEventEmitRequestHandler>(),
        std::make_shared<ViewEventEmitRequestHandler>()};
  }

  GlobalJSIBinders createGlobalJSIBinders() override {
    return {std::make_shared<rnoh::BlobCollectorJSIBinder>(
        GlobalJSIBinder::Context{})};
  }
};

} // namespace rnoh
