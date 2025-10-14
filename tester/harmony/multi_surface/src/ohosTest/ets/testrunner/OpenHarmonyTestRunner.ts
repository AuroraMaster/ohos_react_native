/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import hilog from '@ohos.hilog';
import TestRunner from '@ohos.application.testRunner';
import AbilityDelegatorRegistry from '@ohos.app.ability.abilityDelegatorRegistry';

var abilityDelegator = undefined
var abilityDelegatorArguments = undefined

async function onAbilityCreateCallback() {
    hilog.info(0x0000, 'testTag', '%{public}s', 'onAbilityCreateCallback');
}

async function addAbilityMonitorCallback(err: any) {
    hilog.info(0x0000, 'testTag', 'addAbilityMonitorCallback : %{public}s', JSON.stringify(err) ?? '');
}

export default class OpenHarmonyTestRunner implements TestRunner {
    constructor() {
    }

    onPrepare() {
        hilog.info(0x0000, 'testTag', '%{public}s', 'OpenHarmonyTestRunner OnPrepare ');
    }

    async onRun() {
        hilog.info(0x0000, 'testTag', '%{public}s', 'OpenHarmonyTestRunner onRun run');
        abilityDelegatorArguments = AbilityDelegatorRegistry.getArguments()
        abilityDelegator = AbilityDelegatorRegistry.getAbilityDelegator()
        const bundleName = abilityDelegatorArguments.bundleName;
        const testAbilityName = 'TestAbility';
        let lMonitor = {
            abilityName: testAbilityName,
            onAbilityCreate: onAbilityCreateCallback,
        };
        abilityDelegator.addAbilityMonitor(lMonitor, addAbilityMonitorCallback)
        const want = {
            bundleName: bundleName,
            abilityName: testAbilityName
        };
        abilityDelegator = AbilityDelegatorRegistry.getAbilityDelegator();
        abilityDelegator.startAbility(want, (err : any, data : any) => {
            hilog.info(0x0000, 'testTag', 'startAbility : err : %{public}s', JSON.stringify(err) ?? '');
            hilog.info(0x0000, 'testTag', 'startAbility : data : %{public}s',JSON.stringify(data) ?? '');
        })
        hilog.info(0x0000, 'testTag', '%{public}s', 'OpenHarmonyTestRunner onRun end');
    }
}