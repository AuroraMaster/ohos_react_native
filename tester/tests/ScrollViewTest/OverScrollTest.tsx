/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {View, ScrollView, Text, StyleSheet} from 'react-native';
import {TestSuite, TestCase} from '@rnoh/testerino';
import React, {useState} from 'react';

export function OverScrollTest() {
  return (
    <TestSuite name="overScrollModeTest">
      <TestCase
        tags={['C_API']}
        modal
        itShould="overScrollMode set to 'auto', text is smaller than scrollview => the scrollview can not scroll">
        <View
          style={{
            backgroundColor: 'pink',
            width: '100%',
            justifyContent: 'center',
          }}>
          <View
            style={{
              height: 400,
              marginTop: 50,
              marginBottom: 50,
              backgroundColor:'green'
            }}>
            <ScrollView overScrollMode={'auto'}>
              <Text style={{fontSize:15, backgroundColor:'yellow'}}>
                property overScrollMode set to 'auto'
              </Text>
              <Text style={{fontSize:15, backgroundColor:'yellow', marginTop:5}}>
                property bounces property not set
              </Text>
              <Text style={{fontSize:15, backgroundColor:'yellow', marginTop:5}}>
                the scrollview can not scroll
              </Text>
              <Text style={{fontSize:15, backgroundColor:'yellow', marginTop:5}}>
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
              </Text>
            </ScrollView>
          </View>
        </View>
      </TestCase>
      <TestCase
        tags={['C_API']}
        modal
        itShould="overScrollMode set to 'auto', text is larger than scrollview => the scrollview can overscroll and bounces when it reaches the end of the content">
        <View
          style={{
            backgroundColor: 'pink',
            width: '100%',
            justifyContent: 'center',
          }}>
          <View
            style={{
              height: 400,
              marginTop: 50,
              marginBottom: 50,
              backgroundColor:'green'
            }}>
            <ScrollView overScrollMode={'auto'}>
              <Text style={{fontSize:15, backgroundColor:'yellow'}}>
                property overScrollMode set to 'auto'
              </Text>
              <Text style={{fontSize:15, backgroundColor:'yellow', marginTop:5}}>
                property bounces property not set
              </Text>
              <Text style={{fontSize:15, backgroundColor:'yellow', marginTop:5}}>
                the scrollview can scroll and over scroll and bounces when it reaches the end of the content
              </Text>
              <Text style={{fontSize:15, backgroundColor:'yellow'}}>
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
              </Text>
            </ScrollView>
          </View>
        </View>
      </TestCase>
      <TestCase
        tags={['C_API']}
        modal
        itShould="overScrollMode set to 'never', text is smaller than scrollview => the scrollview can not scroll">
        <View
          style={{
            backgroundColor: 'pink',
            width: '100%',
            justifyContent: 'center',
          }}>
          <View
            style={{
              height: 400,
              marginTop: 50,
              marginBottom: 50,
              backgroundColor:'green'
            }}>
            <ScrollView overScrollMode={'never'}>
              <Text style={{fontSize:15, backgroundColor:'yellow'}}>
                property overScrollMode set to 'never'
              </Text>
              <Text style={{fontSize:15, backgroundColor:'yellow', marginTop:5}}>
                property bounces property not set
              </Text>
              <Text style={{fontSize:15, backgroundColor:'yellow', marginTop:5}}>
                the scrollview can not scroll
              </Text>
              <Text style={{fontSize:15, backgroundColor:'yellow'}}>
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
              </Text>
            </ScrollView>
          </View>
        </View>
      </TestCase>
      <TestCase
        tags={['C_API']}
        modal
        itShould="overScrollMode set to 'never', text is larger than scrollview => the scrollview can scroll but can not overscroll">
        <View
          style={{
            backgroundColor: 'pink',
            width: '100%',
            justifyContent: 'center',
          }}>
          <View
            style={{
              height: 400,
              marginTop: 50,
              marginBottom: 50,
              backgroundColor:'green'
            }}>
            <ScrollView overScrollMode={'never'}>
              <Text style={{fontSize:15, backgroundColor:'yellow'}}>
                android property overScrollMode set to 'never'
              </Text>
              <Text style={{fontSize:15, backgroundColor:'yellow', marginTop:5}}>
                ios property bounces property not set
              </Text>
              <Text style={{fontSize:15, backgroundColor:'yellow', marginTop:5}}>
                the scrollview can scroll but can not overscroll
              </Text>
              <Text style={{fontSize:15, backgroundColor:'yellow'}}>
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
              </Text>
            </ScrollView>
          </View>
        </View>
      </TestCase>
      <TestCase
        tags={['C_API']}
        modal
        itShould="overScrollMode set to 'always', text is smaller than scrollview => the scrollview can overscroll and bounces when it reaches the end of the content">
        <View
          style={{
            backgroundColor: 'pink',
            width: '100%',
            justifyContent: 'center',
          }}>
          <View
            style={{
              height: 400,
              marginTop: 50,
              marginBottom: 50,
              backgroundColor:'green'
            }}>
            <ScrollView overScrollMode={'always'}>
              <Text style={{fontSize:15, backgroundColor:'yellow'}}>
                property overScrollMode set to 'always'
              </Text>
              <Text style={{fontSize:15, backgroundColor:'yellow', marginTop:5}}>
                property bounces property not set
              </Text>
              <Text style={{fontSize:15, backgroundColor:'yellow', marginTop:5}}>
                the scrollview can overscroll and bounces when it reaches the end of the content
              </Text>
              <Text style={{fontSize:15, backgroundColor:'yellow'}}>
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
              </Text>
            </ScrollView>
          </View>
        </View>
      </TestCase>
      <TestCase
        tags={['C_API']}
        modal
        itShould="overScrollMode set to 'always', text is larger than scrollview => the scrollview can overscroll and bounces when it reaches the end of the content">
        <View
          style={{
            backgroundColor: 'pink',
            width: '100%',
            justifyContent: 'center',
          }}>
          <View
            style={{
              height: 400,
              marginTop: 50,
              marginBottom: 50,
              backgroundColor:'green'
            }}>
            <ScrollView overScrollMode={'always'}>
              <Text style={{fontSize:15, backgroundColor:'yellow'}}>
                android property overScrollMode set to 'always'
              </Text>
              <Text style={{fontSize:15, backgroundColor:'yellow', marginTop:5}}>
                ios property bounces property not set
              </Text>
              <Text style={{fontSize:15, backgroundColor:'yellow', marginTop:5}}>
                the scrollview can overscroll and bounces when it reaches the end of the content
              </Text>
              <Text style={{fontSize:15, backgroundColor:'yellow'}}>
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
              </Text>
            </ScrollView>
          </View>
        </View>
      </TestCase>
      <TestCase
        tags={['C_API']}
        modal
        itShould="bounce set to false, text is smaller than scrollview => the scrollview can not scroll">
        <View
          style={{
            backgroundColor: 'pink',
            width: '100%',
            justifyContent: 'center',
          }}>
          <View
            style={{
              height: 400,
              marginTop: 50,
              marginBottom: 50,
              backgroundColor:'green'
            }}>
            <ScrollView bounces={false}>
              <Text style={{fontSize:15, backgroundColor:'yellow'}}>
                property overScrollMode not set
              </Text>
              <Text style={{fontSize:15, backgroundColor:'yellow', marginTop:5}}>
                property bounces property set to false
              </Text>
              <Text style={{fontSize:15, backgroundColor:'yellow', marginTop:5}}>
                the scrollview can not scroll
              </Text>
              <Text style={{fontSize:15, backgroundColor:'yellow'}}>
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
              </Text>
            </ScrollView>
          </View>
        </View>
      </TestCase>
      <TestCase
        tags={['C_API']}
        modal
        itShould="bounce set to false, text is larger than scrollview => the scrollview can scroll but can not overscroll">
        <View
          style={{
            backgroundColor: 'pink',
            width: '100%',
            justifyContent: 'center',
          }}>
          <View
            style={{
              height: 400,
              marginTop: 50,
              marginBottom: 50,
              backgroundColor:'green'
            }}>
            <ScrollView bounces={false}>
              <Text style={{fontSize:15, backgroundColor:'yellow'}}>
                android property overScrollMode set to 'always'
              </Text>
              <Text style={{fontSize:15, backgroundColor:'yellow', marginTop:5}}>
                ios property bounces property not set
              </Text>
              <Text style={{fontSize:15, backgroundColor:'yellow', marginTop:5}}>
                the scrollview can scroll but can not overscroll
              </Text>
              <Text style={{fontSize:15, backgroundColor:'yellow'}}>
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
              </Text>
            </ScrollView>
          </View>
        </View>
      </TestCase>
      <TestCase
        tags={['C_API']}
        modal
        itShould="bounce set to true, alwaysBounceVertical set to false, text is smaller than scrollview => the scrollview can not scroll">
        <View
          style={{
            backgroundColor: 'pink',
            width: '100%',
            justifyContent: 'center',
          }}>
          <View
            style={{
              height: 400,
              marginTop: 50,
              marginBottom: 50,
              backgroundColor:'green'
            }}>
            <ScrollView bounces={true} alwaysBounceVertical={false}>
              <Text style={{fontSize:15, backgroundColor:'yellow'}}>
                property overScrollMode not set
              </Text>
              <Text style={{fontSize:15, backgroundColor:'yellow', marginTop:5}}>
                property bounces property set to false
              </Text>
              <Text style={{fontSize:15, backgroundColor:'yellow', marginTop:5}}>
                the scrollview can not scroll
              </Text>
              <Text style={{fontSize:15, backgroundColor:'yellow'}}>
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
              </Text>
            </ScrollView>
          </View>
        </View>
      </TestCase>
      <TestCase
        tags={['C_API']}
        modal
        itShould="bounce set to true, alwaysBounceVertical set to false, text is larger than scrollview => the scrollview can overscroll and bounces when it reaches the end of the content">
        <View
          style={{
            backgroundColor: 'pink',
            width: '100%',
            justifyContent: 'center',
          }}>
          <View
            style={{
              height: 400,
              marginTop: 50,
              marginBottom: 50,
              backgroundColor:'green'
            }}>
            <ScrollView bounces={true} alwaysBounceVertical={false}>
              <Text style={{fontSize:15, backgroundColor:'yellow'}}>
                android property overScrollMode set to 'always'
              </Text>
              <Text style={{fontSize:15, backgroundColor:'yellow', marginTop:5}}>
                ios property bounces property not set
              </Text>
              <Text style={{fontSize:15, backgroundColor:'yellow', marginTop:5}}>
              the scrollview can overscroll and bounces when it reaches the end of the content
              </Text>
              <Text style={{fontSize:15, backgroundColor:'yellow'}}>
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
              </Text>
            </ScrollView>
          </View>
        </View>
      </TestCase>
      <TestCase
        tags={['C_API']}
        modal
        itShould="bounce set to true, alwaysBounceVertical set to true, text is smaller than scrollview => the scrollview can overscroll and bounces when it reaches the end of the content">
        <View
          style={{
            backgroundColor: 'pink',
            width: '100%',
            justifyContent: 'center',
          }}>
          <View
            style={{
              height: 400,
              marginTop: 50,
              marginBottom: 50,
              backgroundColor:'green'
            }}>
            <ScrollView bounces={true} alwaysBounceVertical={true}>
              <Text style={{fontSize:15, backgroundColor:'yellow'}}>
                property overScrollMode not set
              </Text>
              <Text style={{fontSize:15, backgroundColor:'yellow', marginTop:5}}>
                property bounces property set to false
              </Text>
              <Text style={{fontSize:15, backgroundColor:'yellow', marginTop:5}}>
                the scrollview can overscroll and bounces when it reaches the end of the content
              </Text>
              <Text style={{fontSize:15, backgroundColor:'yellow'}}>
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
              </Text>
            </ScrollView>
          </View>
        </View>
      </TestCase>
      <TestCase
        tags={['C_API']}
        modal
        itShould="bounce set to true, alwaysBounceVertical set to true, text is larger than scrollview => the scrollview can overscroll and bounces when it reaches the end of the content">
        <View
          style={{
            backgroundColor: 'pink',
            width: '100%',
            justifyContent: 'center',
          }}>
          <View
            style={{
              height: 400,
              marginTop: 50,
              marginBottom: 50,
              backgroundColor:'green'
            }}>
            <ScrollView bounces={true} alwaysBounceVertical={true}>
              <Text style={{fontSize:15, backgroundColor:'yellow'}}>
                android property overScrollMode set to 'always'
              </Text>
              <Text style={{fontSize:15, backgroundColor:'yellow', marginTop:5}}>
                ios property bounces property not set
              </Text>
              <Text style={{fontSize:15, backgroundColor:'yellow', marginTop:5}}>
                the scrollview can overscroll and bounces when it reaches the end of the content
              </Text>
              <Text style={{fontSize:15, backgroundColor:'yellow'}}>
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
                content size is smaller than scrollview
              </Text>
            </ScrollView>
          </View>
        </View>
      </TestCase>
    </TestSuite>
  )
}