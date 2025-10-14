/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React, {useEffect, useState} from 'react';
import {Text, View} from 'react-native';

export const AppParamsContext = React.createContext(undefined);

const EnvCtx = React.createContext({isConnectedToInternet: false});

async function checkIsInternetAvailable() {
  try {
    await fetch('https://www.google.com', {
      method: 'HEAD',
    });
    return true;
  } catch (error) {
    return false;
  }
}

export const Environment = ({children}: {children: any}) => {
  const [internetAvailability, setInternetAvailability] = useState<
    'CHECKING' | 'AVAILABLE' | 'NOT_AVAILABLE'
  >('CHECKING');

  useEffect(() => {
    (async () => {
      setInternetAvailability(
        (await checkIsInternetAvailable()) ? 'AVAILABLE' : 'NOT_AVAILABLE',
      );
    })();
  }, []);

  if (internetAvailability === 'CHECKING') {
    return (
      <View
        style={{
          justifyContent: 'center',
          alignItems: 'center',
          height: '100%',
        }}>
        <Text>Preparing testing environment...</Text>
      </View>
    );
  }

  return (
    <EnvCtx.Provider
      value={{isConnectedToInternet: internetAvailability === 'AVAILABLE'}}>
      {children}
    </EnvCtx.Provider>
  );
};

export function useEnvironment() {
  return {env: React.useContext(EnvCtx)!};
}
