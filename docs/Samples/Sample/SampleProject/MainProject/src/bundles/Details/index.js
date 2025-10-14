/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 *
 * @format
 */

import { AppRegistry, View, Text } from 'react-native';
import AppDetails from './DetailsMainPage';
import React from 'react';

// 使用setWrapperComponentProvider来包装根组件。
// children: 渲染RN侧组件；<Text> 鸿蒙侧传递过来的数据内容。
AppRegistry.setWrapperComponentProvider(appParams => {
    return function ({ children, ...otherProps }) {
        return <View style={otherProps.initialProps.styles.container}>
            <View style={{ 'borderRadius': 10, 'backgroundColor': 'blue', height: 100, justifyContent: 'center' }}>
                <Text style={otherProps.initialProps.styles.apiFontSize}>{otherProps.initialProps.stringParam}</Text>
            </View>
            <View>{children}</View>
        </View>
    }
});

AppRegistry.registerComponent('Details', () => AppDetails);