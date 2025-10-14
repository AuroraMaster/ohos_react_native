/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "RNOH/PackageProvider.h"
#include "generated/RNOHGeneratedPackage.h"
#include "RNCVideoPackage.h"
#include "GestureHandlerPackage.h"
#include "SafeAreaViewPackage.h"
#include "FlashListPackage.h"

using namespace rnoh;

std::vector<std::shared_ptr<Package>> PackageProvider::getPackages(Package::Context ctx)
{
    return {
        std::make_shared<RNOHGeneratedPackage>(ctx),
        std::make_shared<RNCVideoPackage>(ctx),
        std::make_shared<GestureHandlerPackage>(ctx),
        std::make_shared<SafeAreaViewPackage>(ctx),
        std::make_shared<FlashListPackage>(ctx),
    };
}