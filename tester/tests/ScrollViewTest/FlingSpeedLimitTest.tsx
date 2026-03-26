/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React, { useState, useRef } from 'react';
import { View, Text, StyleSheet, FlatList, TouchableOpacity, TextInput } from 'react-native';
import { TestSuite, TestCase } from '@rnoh/testerino';

const frictionToRate = (f: number) => 1.0 - (f / 250.0);

const DATA = Array.from({ length: 100 }, (_, i) => ({
  id: String(i + 1),
  title: `Item ${i + 1}`,
}));

export function FlingSpeedLimitTest() {
  const [decelerationRate, setDecelerationRate] = useState<number>(frictionToRate(0.75));
  const [flingSpeedLimit, setFlingSpeedLimit] = useState<number>(9000);
  const [manualFriction, setManualFriction] = useState('0.75');
  const [manualSpeed, setManualSpeed] = useState('9000');
  const flatListRef = useRef<FlatList>(null);

  const handleManualSubmit = () => {
    const friction = parseFloat(manualFriction);
    const speed = parseFloat(manualSpeed);
    if (!isNaN(friction) && friction > 0) {
      setDecelerationRate(frictionToRate(friction));
    }
    if (!isNaN(speed) && speed > 0) {
      setFlingSpeedLimit(speed);
    }
  };

  const scrollToTop = () => {
    flatListRef.current?.scrollToOffset({ offset: 0, animated: true });
  };

  return (
    <TestSuite name="flingSpeedLimit & decelerationRate">
      <TestCase
        modal
        itShould="support dynamic flingSpeedLimit and decelerationRate adjustments">
        <View style={styles.container}>
          <View style={styles.header}>
            <View style={styles.manualInputRow}>
              <Text style={styles.label}>Friction:</Text>
              <TextInput
                style={styles.textInput}
                value={manualFriction}
                onChangeText={setManualFriction}
                keyboardType="numeric"
              />
              <Text style={styles.label}>v0 Limit:</Text>
              <TextInput
                style={styles.textInput}
                value={manualSpeed}
                onChangeText={setManualSpeed}
                keyboardType="numeric"
              />
              <TouchableOpacity style={styles.submitButton} onPress={handleManualSubmit}>
                <Text style={styles.submitButtonText}>应用</Text>
              </TouchableOpacity>
            </View>

            <Text style={styles.configText}>
              Current: Rate: {decelerationRate.toFixed(4)} | Limit: {flingSpeedLimit}
            </Text>
          </View>

          <FlatList
            ref={flatListRef}
            data={DATA}
            renderItem={({ item }) => (
              <View style={styles.itemContainer}>
                <Text style={styles.itemTitle}>{item.title}</Text>
              </View>
            )}
            keyExtractor={(item) => item.id}
            decelerationRate={decelerationRate}
            // @ts-ignore
            flingSpeedLimit={flingSpeedLimit}
            style={styles.list}
          />

          <TouchableOpacity style={styles.scrollToTopButton} onPress={scrollToTop}>
            <Text style={styles.submitButtonText}>置顶</Text>
          </TouchableOpacity>
        </View>
      </TestCase>
    </TestSuite>
  );
}

const styles = StyleSheet.create({
  container: {
    height: 500,
    backgroundColor: '#F5F5F5',
  },
  header: {
    padding: 10,
    backgroundColor: '#E5E5E5',
  },
  manualInputRow: {
    flexDirection: 'row',
    alignItems: 'center',
    marginBottom: 8,
  },
  label: {
    fontSize: 12,
    marginRight: 4,
  },
  textInput: {
    borderWidth: 1,
    borderColor: '#CCC',
    backgroundColor: '#FFF',
    padding: 4,
    marginRight: 8,
    width: 60,
    fontSize: 12,
  },
  submitButton: {
    backgroundColor: '#007AFF',
    paddingHorizontal: 12,
    paddingVertical: 6,
    borderRadius: 4,
  },
  submitButtonText: {
    color: '#FFF',
    fontSize: 12,
  },
  configText: {
    fontSize: 12,
    color: '#333',
  },
  list: {
    flex: 1,
  },
  itemContainer: {
    padding: 15,
    borderBottomWidth: 1,
    borderBottomColor: '#DDD',
    backgroundColor: '#FFF',
  },
  itemTitle: {
    fontSize: 14,
  },
  scrollToTopButton: {
    position: 'absolute',
    bottom: 20,
    right: 20,
    backgroundColor: '#007AFF',
    padding: 10,
    borderRadius: 20,
  },
});

