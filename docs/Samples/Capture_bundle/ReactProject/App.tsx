/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React, { useState } from 'react';
import { View, Text, Button, Alert, StyleSheet } from 'react-native'

const App: React.FC = () => {
  const [errorMessage, setErrorMessage] = useState('');

  const handleReferenceError = () => {
    try {
      const erro = new ReferenceError('Some error happened');
      throw error;
    } catch (e) {
      console.error('Caught an error:', e);

      setErrorMessage(e.message);
    }
  };

  const handleStandardError = () => {
    try {
      const error = new Error('Some error happened');
      throw error;
    } catch (e) {
      console.error('Caught an error:', e);

      setErrorMessage(e.message);
    }
  };

  const handleFrozenError = () => {
    try {
      const error = Object.freeze(new Error('Some error happened'));
      throw error;
    } catch (e) {
      console.error('Caught an error:', e);

      setErrorMessage(e.message);
    }
  };

  const handleWarningError = () => {
    try {
      const warning = new Error('This is a warning message') as any;
      warning.type = 'warn';
      throw warning;
    } catch (e) {
      Alert.alert('Warning', e.message);
      setErrorMessage('Warning displayed:' + e.message);
    }
  };

  return (
    <View style={styles.container}>
      <Text style={styles.title}>Press a Button to trigger an error or warning:</Text>
      <View style={styles.buttonContainer}>
        <Button title="ReferenceError" onPress={handleReferenceError} />
        <Button title="Standard Error" onPress={handleStandardError} />
        <Button title="Frozen Error" onPress={handleFrozenError} />
        <Button title="Warning" onPress={handleWarningError} />
      </View>
      {
        errorMessage && <Text style={styles.errorMessage}>{errorMessage}</Text>
      }
    </View>
  );

};

const styles =
  StyleSheet.create({
    container: {
      flex: 1,
      justifyContent: 'center',
      alignItems: 'center',
      padding: 20,
    },
    title: {
      fontSize: 18,
      marginBottom: 20,
    },
    buttonContainer: {
      width: '80%',
      justifyContent: 'space-around',
      flexDirection: 'row',
      flexWrap: 'wrap',
    },
    errorMessage: {
      color: 'red',
      marginTop: 20,
    },
  });

export default App;