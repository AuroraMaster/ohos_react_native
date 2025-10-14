/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { AppRegistry, Text, View, StyleSheet } from 'react-native';
import AppGoods from './src/bundles/Goods/GoodsMainPage';
import AppHome from './src/bundles/HomePage/HomePage';

import React from 'react';
import AppDetails from './src/bundles/Details/DetailsMainPage';
import AppTest from './src/bundles/test/testPage';
import SandBox from './src/bundles/Sandbox/examples'

// 使用setWrapperComponentProvider来包装根组件。
// children: 渲染RN侧组件；<Text> 鸿蒙侧传递过来的数据内容。
AppRegistry.setWrapperComponentProvider(appParams => {
    return function ({ children, ...otherProps }) {
        if (Object.keys(otherProps.initialProps).includes('styles') &&
            Object.keys(otherProps.initialProps).includes('stringParam')) {
            return <View style={otherProps.initialProps.styles.container}>
                <View style={{ 'borderRadius': 10, 'backgroundColor': '#FAE6B1', height: 100, justifyContent: 'center', alignItems: 'center' }}>
                    <Text style={otherProps.initialProps.styles.apiFontSize}>{otherProps.initialProps.stringParam}</Text>
                </View>
                <View>{children}</View>
            </View>
        } else {
            return children
        }
    }
});

AppRegistry.registerComponent('HomePage', () => AppHome);
AppRegistry.registerComponent('Goods', () => AppGoods);
AppRegistry.registerComponent('Details', () => AppDetails);
AppRegistry.registerComponent('AppTest', () => AppTest);
AppRegistry.registerComponent('SandBox', () => SandBox);