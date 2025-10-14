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

interface DeviceState {
    deviceRange: DeviceRange;
}

type DeviceRange = 'SM' | 'MD' | 'LG' | 'UNKNOWN';
const RANGE_SM = { min: 320, max: 600 };
const RANGE_MD = { min: 600, max: 840 };
const RANGE_LG = { min: 840 };
export function getDeviceRange(width: number): DeviceRange {
    if (width >= RANGE_SM.min && width < RANGE_SM.max) {
        return 'SM';
    } else if (width >= RANGE_MD.min && width < RANGE_MD.max) {
        return 'MD';
    } else if (width >= RANGE_LG.min) {
        return 'LG';
    } else {
        return 'UNKNOWN';
    }
};

export { type DeviceState };