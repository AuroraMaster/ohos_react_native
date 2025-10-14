/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React from 'react';
import { View, Text, Button, StyleSheet } from 'react-native';
import { NavigationTurboModule } from 'react-native-harmony-navigation-package';

// 自定义导航头
const CustomHeader = ({ title }) => {
  return (
    <View style={styles.header}>
      <Text style={styles.headerTitle}>{title}</Text>
    </View>
  );
};

// React Native页面B
const PageB = () => {
  const handleNavigationPush = () => {
    NavigationTurboModule.navigatePush("NativePageC");
  };

  const handleNavgationPop = () => {
    NavigationTurboModule.navigatePop();
  };

  return (
    <View style={styles.container}>
      <CustomHeader title="这是RN页面B" />
      <View style={styles.buttonContainer}>
        <Button
          title="跳转到原生页面C"
          onPress={handleNavigationPush}
          style={styles.button}
        />
        <View style={styles.buttonSpacing} />
        <Button
          title="回到上一个页面"
          onPress={handleNavgationPop}
          style={styles.button}
        />
      </View>
    </View>
  );
};

const App = () => {
  return <PageB />;
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#f5f5f5',
  },
  header: {
    flexDirection: 'row',
    alignItems: 'center',
    padding: 10,
    backgroundColor: '#fff',
    borderBottomWidth: 1,
    borderBottomColor: '#ddd',
  },
  headerTitle: {
    fontSize: 18,
    marginLeft: 10,
  },
  title: {
    fontSize: 24,
    marginBottom: 20,
  },
  buttonContainer: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    marginTop: 20,
  },
  buttonSpacing: {
    height: 20,
  },
  button: {
    width: 200,
    marginVertical: 10,
  },
});

export default App;
