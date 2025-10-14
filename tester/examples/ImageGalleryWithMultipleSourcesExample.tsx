/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import React, {useEffect, useState} from 'react';
import {
  Image,
  ScrollView,
  StyleSheet,
  Text,
  TextInput,
  View,
} from 'react-native';
const REMOTE_IMAGE_URL =
  'https://images.pexels.com/photos/7125778/pexels-photo-7125778.jpeg?auto=compress&cs=tinysrgb&w=1900&h=1900&dpr=2';
const NUMBER_OF_IMAGES = 90;
const MEMORY_CONSUMING_SCENARIO = false;

// run example and it will break, sometimes after scrolling the page
export function ImageGalleryWithMultipleSourcesExample() {
  const [numberOfComponents, setNumberOfComponents] =
    useState(NUMBER_OF_IMAGES);
  const [preloadingImagesOn, setPreloadingImagesOn] = useState(
    MEMORY_CONSUMING_SCENARIO,
  );

  useEffect(() => {
    if (!preloadingImagesOn) {
      return;
    }
    const promisesArray: Promise<boolean>[] = [];
    for (let i = 0; i < numberOfComponents; i++) {
      const promise = Image.prefetch(REMOTE_IMAGE_URL + '&v=' + i);
      promisesArray.push(promise);
    }
    Promise.all(promisesArray)
      .then(res => console.log('Successfully loaded all images', res))
      .catch(err => console.log('Failed to load all images', err))
      .finally(() => {
        setPreloadingImagesOn(false);
      });
  }, []);
  if (preloadingImagesOn) {
    return (
      <View>
        <Text style={styles.textInput}>
          Preloading images on. Wait ~1 minute for finishing image preload
        </Text>
      </View>
    );
  }
  return (
    <ScrollView style={{flex: 1}}>
      <View>
        <TextInput
          style={styles.textInput}
          value={numberOfComponents.toString()}
          onChangeText={value => {
            setNumberOfComponents(parseInt(value) || 0);
          }}
        />
        <View style={styles.gallery}>
          {new Array(numberOfComponents).fill(0).map((_, idx) => {
            return (
              <Image
                key={idx}
                source={{
                  uri: REMOTE_IMAGE_URL + '&v=' + idx,
                }}
                style={styles.image}
              />
            );
          })}
          <Image source={{uri: REMOTE_IMAGE_URL}} style={styles.image} />
          <Image source={{uri: REMOTE_IMAGE_URL}} style={styles.image} />
          <Image source={{uri: REMOTE_IMAGE_URL}} style={styles.image} />
        </View>
      </View>
    </ScrollView>
  );
}
const styles = StyleSheet.create({
  image: {
    width: 60,
    height: 60,
    aspectRatio: 1,
  },
  textInput: {
    backgroundColor: 'white',
    width: '100%',
    height: 36,
    color: 'black',
  },
  gallery: {
    width: '100%',
    flexWrap: 'wrap',
    flexDirection: 'row',
  },
});
