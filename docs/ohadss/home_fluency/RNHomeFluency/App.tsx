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

import React from 'react';
import { View, Image, StyleSheet, TouchableOpacity } from 'react-native';
import { NavigationContainer } from '@react-navigation/native';
import { createBottomTabNavigator } from '@react-navigation/bottom-tabs';
import HomeController from './src/main/controller/HomeController';
import { initialWindowMetrics, SafeAreaProvider, SafeAreaView } from 'react-native-safe-area-context';

const Tab = createBottomTabNavigator();

const CustomTabBarButton = ({ children, onPress, isHome }) => {
  return (
    <TouchableOpacity
      onPress={isHome ? onPress : () => { }}
      activeOpacity={isHome ? 0.2 : 1}
      style={{ flex: 1 }}
    >
      {children}
    </TouchableOpacity>
  );
};

const App = () => {
  return (
    <SafeAreaProvider initialMetrics={initialWindowMetrics}>
      <SafeAreaView style={{ flex: 1 }}>
        <NavigationContainer>
          <Tab.Navigator
            screenOptions={({ route }) => ({
              tabBarIcon: ({ focused, color, size }) => {
                let iconName;
                if (route.name === '首页') {
                  iconName = require('./assets/front_page.png');
                  return <Image source={iconName} style={styles.tabIconHome} />;
                } else if (route.name === '分类') {
                  iconName = require('./assets/ic_public_app.png');
                } else if (route.name === '发现') {
                  iconName = require('./assets/ic_public_fast.png');
                } else if (route.name === '购物袋') {
                  iconName = require('./assets/ic_public_appstore.png');
                } else if (route.name === '我的') {
                  iconName = require('./assets/ic_public_contacts.png');
                }

                return <Image source={iconName} style={styles.tabIcon} />;
              },
              tabBarLabelStyle: {
                color: route.name === '首页' ? '#0a59f7' : '#c6c7c9',
              },
              tabBarButton: (props) => (
                <CustomTabBarButton
                  {...props}
                  isHome={route.name === '首页'}
                />
              ),
            })}
          >
            <Tab.Screen
              name="首页"
              component={HomeController}
              options={{ headerShown: false }}
            />
            <Tab.Screen name="分类" component={() => null} />
            <Tab.Screen name="发现" component={() => null} />
            <Tab.Screen name="购物袋" component={() => null} />
            <Tab.Screen name="我的" component={() => null} />
          </Tab.Navigator>
        </NavigationContainer>
      </SafeAreaView>
    </SafeAreaProvider>
  );
};

const styles = StyleSheet.create({
  tabIconHome: {
    width: 20,
    height: 20,
  },
  tabIcon: {
    width: 20,
    height: 20,
    opacity: 0.3,
  },
});

export default App;
