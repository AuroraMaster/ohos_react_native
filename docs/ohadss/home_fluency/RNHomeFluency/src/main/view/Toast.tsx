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

import React, { useState, useEffect } from 'react';
import { Modal, Text, StyleSheet, Animated, View } from 'react-native';

interface CustomToastProps {
  message: string;
  visible: boolean
}
const CustomToast = (props: CustomToastProps) => {
  return (
    <Modal
      visible={props.visible}
      transparent={true}
      animationType='fade'
    >
      <View style={styles.container}>
        <Animated.View style={[styles.toast]}>
          <Text style={styles.message}>{props.message}</Text>
        </Animated.View>
      </View>
    </Modal>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'flex-end',
    alignItems: 'center',
    backgroundColor: 'rgba(0, 0, 0, 0)',
    paddingBottom: 108,
    shadowColor: '#000',
    shadowOffset: { width: 0, height: 4 },
    shadowOpacity: 0.1,
    shadowRadius: 6,
    elevation: 3,
  },
  toast: {
    backgroundColor: 'rgba(255, 255, 255, 1)',
    width: 244,
    height: 35,
    display: 'flex',
    justifyContent: 'center',
    alignItems: 'center',
    borderRadius: 18,
    paddingHorizontal: 16,
    paddingVertical: 8,
  },
  message: {
    color: 'rgba(0, 0, 0, 0.8)',
    fontSize: 14,
  },
});

export default CustomToast;