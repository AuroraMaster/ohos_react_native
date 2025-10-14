/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {useCallback, useState} from 'react';
import {
  Platform,
  RefreshControl,
  ScrollView,
  StyleSheet,
  Text,
  TextInput,
  TouchableOpacity,
  View,
  Image,
  TouchableWithoutFeedback,
  Pressable,
  Button,
} from 'react-native';

const accessibilityRoleList = [
  { type: 'adjustable', expect: '提示当前内容是一个进度条', value: '元素具有可调整的特性' },
  { type: 'alert', expect: '提示当前内容是一个alert', value: '警告' },
  { type: 'button', expect: '提示当前内容是一个按钮', value: 'button' },
  { type: 'checkbox', expect: '提示当前内容是一个复选框', value: 'checkbox' },
  { type: 'combobox', expect: '提示当前内容是一个combobox', value: 'combobox' },
  { type: 'header', expect: '提示当前内容是一个标题', value: '内容区域的头部' },
  { type: 'image', expect: '提示当前内容是一个image', value: '图片' },
  { type: 'imagebutton', expect: '提示当前内容是一个button、image', value: '元素应被视为按钮并且也是图像时使用' },
  { type: 'link', expect: '提示当前内容是一个link', value: '链接' },
  { type: 'menu', expect: '提示当前内容是一个menu', value: '菜单' },
  { type: 'menubar', expect: '提示当前内容是一个menubar', value: '菜单栏' },
  { type: 'menuitem', expect: '提示当前内容是一个menuitem', value: '菜单项' },
  { type: 'progressbar', expect: '提示当前内容是一个progressbar', value: '进度条' },
  { type: 'radio', expect: '提示当前内容是一个单选按钮', value: 'radio' },
  { type: 'radiogroup', expect: '提示当前内容是一个radiogroup', value: '表示一组单选按钮' },
  { type: 'scrollbar', expect: '提示当前内容是一个scrollbar', value: '滚动条' },
  { type: 'search', expect: '提示当前内容是一个编辑框', value: '用作搜索框的文本框' },
  { type: 'spinbutton', expect: '提示当前内容是一个spinbutton', value: '表示打开选项列表的按钮' },
  { type: 'summary', expect: '提示当前内容是一个summary', value: '提供当前的简要总结信息的元素' },
  { type: 'switch', expect: '提示当前内容是一个关闭开关', value: '表示可以打开和关闭的开关' },
  { type: 'tab', expect: '提示当前内容是一个tab', value: 'tab标签' },
  { type: 'tablist', expect: '提示当前内容是一个tablist', value: '选项卡的列表' },
  { type: 'timer', expect: '提示当前内容是一个timer', value: '定时器' },
  { type: 'togglebutton', expect: '提示当前内容是一个关闭开关', value: '切换按钮' },
  { type: 'toolbar', expect: '提示当前内容是一个toolbar', value: '工具栏' },
]

const roleList = [
  { type: 'alert', expect: '提示alert', value: '警告' },
  { type: 'button', expect: '提示当前内容是一个按钮', value: '按钮' },
  { type: 'checkbox', expect: '提示当前内容是一个复选框', value: '复选框' },
  { type: 'heading', expect: '提示当前内容是一个标题', value: '标题' },
  { type: 'menu', expect: '提示当前内容是一个menu', value: '菜单' },
  { type: 'menubar', expect: '提示当前内容是一个menubar', value: '菜单栏' },
  { type: 'menuitem', expect: '提示当前内容是一个menuitem', value: '菜单项' },
  { type: 'progressbar', expect: '提示当前内容是一个progressbar', value: '进度条' },
  { type: 'radio', expect: '提示当前内容是一个单选按钮', value: '单选' },
  { type: 'radiogroup', expect: '提示当前内容是一个radiogroup', value: '单选按钮组' },
  { type: 'slider', expect: '提示当前内容是一个进度条', value: '	滑动条' },
  { type: 'spinbutton', expect: '提示当前内容是一个spinbutton', value: '微调' },
  { type: 'tab', expect: '提示当前内容是一个tab', value: 'tab标签' },
  { type: 'tablist', expect: '提示当前内容是一个tablist', value: '提示文本' },
  { type: 'timer', expect: '提示当前内容是一个timer', value: '计数' },
  { type: 'toolbar', expect: '提示当前内容是一个toolbar', value: '工具栏' },
]

export const ScrollViewAccessibleExample = () => {
  const [changeValue, setChangeValue] = useState(true);
  const [bg, setBg] = useState('#FFFFFF');
  const [messageOne, setMessageOne] = useState('');
  const accessibilityLiveRegions = ['none', 'polite', 'assertive'];
  const [accessibilityLiveRegion, setAccessibilityLiveRegion] = useState('none');
  const [aria_checked, setAria_checked] = useState(false);
  const [aria_disabled, setAria_disabled] = useState(false);
  const [aria_expanded, setAria_expanded] = useState(false);
  const [aria_hidden, setAria_hidden] = useState(false);
  const [accessibilityIgnoresInvertColors, setAccessibilityIgnoresInvertColors] = useState(false);
  const [onAccessibilityTap, setOnAccessibilityTap] = useState('ready');
  const [messageTwo, setMessageTwo] = useState('');
  const aria_lives = ['off', 'polite', 'assertive','rude'];
  const [aria_live, setAria_live] = useState('none');
  const [onMagicTap, setOnMagicTap] = useState('ready');
  const [onAccessibilityEscape, setOnAccessibilityEscape] = useState('ready');
  const [aria_modal, setAria_modal] = useState(false);
  const [display, setDisplay] = useState("none");
  return (
    <ScrollView style={{flex:1}}>
      <View style={{ flex: 1, alignItems: 'center' }}>
        <View style={styles.scrollViewProps}>
          <Text>1.属性值：{'accessible={changeValue}'}</Text>
          <View>
            <Text>预期结果：设置为true时表示当前视图是一个“无障碍元素”（accessibility element）。无障碍元素会将其所有子组件视为一整个可以选中的组件,即无法单独选中'text one'和'text two'，而只能选中整个父视图</Text>
            <Text>当changeValue值为true，无法单独选中'text one'和'text two'</Text>
            <Text>当changeValue值为false，可以单独选中'text one'和'text two'</Text>
          </View>
          <View>
            <Text>实际结果：</Text>
            <Pressable
              style={{ backgroundColor: '#AFEEEE', borderRadius: 5, }}
              onPress={() => {
                setChangeValue(!changeValue)
              }}>
              <Text style={{ fontSize: 16, textAlign: 'center', paddingTop: 10, paddingBottom: 10 }}>按压切换changeValue：{JSON.stringify(changeValue)}</Text>
            </Pressable>
            <ScrollView
              style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5 }}
              accessible={changeValue}
            >
              <Text style={{ padding: 10 }}>text one</Text>
              <Text style={{ padding: 10 }}>text two</Text>
              <Text style={{ padding: 10 }}>text three</Text>
            </ScrollView>
          </View>
        </View>


        <View style={styles.scrollViewProps}>
          <Text>2.属性值：{"accessibilityLabel='Tap me!'"}</Text>
          <View>
            <Text>预期结果：设置accessibilityLabel标签后，点击会读出选中元素的无障碍标签Tap me,不会读文本内容：text one，text two</Text>
          </View>
          <View>
            <Text>实际结果：</Text>
            <ScrollView
              style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5 }}
              accessible={true}
              accessibilityLabel='Tap me!'
            >
              <Text style={{ padding: 10 }}>text one</Text>
              <Text style={{ padding: 10 }}>text two</Text>
              <Text style={{ padding: 10 }}>text three</Text>
            </ScrollView>
          </View>
        </View>


        <View style={styles.scrollViewProps}>
          <Text>3.属性值：{"accessibilityActions={[{name: 'activate',label: 'activate'}]} "} </Text>
          <Text>4.属性值：onAccessibilityAction</Text>
          <View>
            <Text>预期结果：双击文本内容后，背景变为粉红色</Text>
          </View>
          <View>
            <Text>实际结果：</Text>
            <ScrollView
              style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, backgroundColor: bg }}
              accessible={true}
              accessibilityActions={[{ name: 'activate', label: 'activate' }]}
              onAccessibilityAction={event => {
                if (event.nativeEvent.actionName === 'activate') {
                  setBg('pink');
                }
              }}
            >
              <Text style={{ padding: 10 }}>text one</Text>
              <Text style={{ padding: 10 }}>text two</Text>
            </ScrollView>
          </View>
        </View>


        <View style={styles.scrollViewProps}>
          <Text>5.属性值： accessibilityLiveRegion</Text>
          <View>
            <Text>预期结果：</Text>
            <Text>当属性值accessibilityLiveRegion为none，点击发送后不会读出文本内容，</Text>
            <Text>当属性值accessibilityLiveRegion为polite，点击发送后会读出文本内容，</Text>
            <Text>当属性值accessibilityLiveRegion为assertive，点击发送后如果在语音回话会立即打断当前的语音会话，提醒用户当前视图的变化读出文本内容</Text>
          </View>
          <View>
            <Text>实际结果：</Text>
            <View>
              {accessibilityLiveRegions.map(value => (
                <Button
                  title={value}
                  active={value === accessibilityLiveRegion}
                  label={value}
                  key={value}
                  onPress={() => {
                    setAccessibilityLiveRegion(value);
                    setMessageOne('')
                  }}
                  style={{ height: 30, width: 10 }}
                />
              ))}
            </View>
            <ScrollView
              style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}
              accessible={true}
              accessibilityLiveRegion={accessibilityLiveRegion}
            >
              <Text>{messageOne}</Text>
            </ScrollView>
            <Button title="点击" onPress={() => { setMessageOne('新消息已发送'); }} />
          </View>
        </View>


        <View style={styles.scrollViewProps}>
          <Text>6.1属性值：{'aria-busy={true} '} </Text>
          <View>
            <Text>预期结果：当aria-busy值为true时，提示busy</Text>
          </View>
          <View>
            <Text>实际结果：</Text>
            <ScrollView
              style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}
              accessible={true}
              aria-busy={true}
            >
              <Text style={{ padding: 10 }}>text one</Text>
              <Text style={{ padding: 10 }}>text two</Text>
            </ScrollView>
          </View>
          <Text>6.2属性值：{'aria-busy={false} '}</Text>
          <View>
            <Text>预期结果：当aria-busy值为false时，读出文本内容text one, text two </Text>
          </View>
          <View>
            <Text>实际结果：</Text>
            <ScrollView
              style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}
              accessible={true}
              aria-busy={false}
            >
              <Text style={{ padding: 10 }}>text one</Text>
              <Text style={{ padding: 10 }}>text two</Text>
            </ScrollView>
          </View>
        </View>


        <View style={styles.scrollViewProps}>
          <Text>7.属性值：aria-checked </Text>
          <View>
            <Text>预期结果：</Text>
            <Text>1.当aria-checked为false时，提示元素未被选择</Text>
            <Text>2.当aria-checked为true时，提示元素被选择</Text>
          </View>
          <View>
            <Text>实际结果：</Text>
            <Button
              title={'aria_checked: ' + aria_checked.toString()}
              onPress={() => setAria_checked(!aria_checked)}
              label={'aria_checked: ' + aria_checked.toString()}
            />
            <ScrollView
              style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}
              accessible={true}
              aria-checked={aria_checked}
            >
              <Text style={{ padding: 10 }}>text one</Text>
            </ScrollView>
          </View>
        </View>


        <View style={styles.scrollViewProps}>
          <Text>8.属性值：aria-disabled </Text>
          <View>
            <Text>预期结果：</Text>
            <Text>1.当aria-disabled为false时，表示清除非激活状态</Text>
            <Text>2.当aria-disabled为true时，表示当前是非激活状态，提示元素不可点击</Text>
          </View>
          <View>
            <Text>实际结果：</Text>
            <Button
              title={'aria_disabled: ' + aria_disabled.toString()}
              onPress={() => setAria_disabled(!aria_disabled)}
              label={'aria_disabled: ' + aria_disabled.toString()}
            />
            <ScrollView
              style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}
              accessible={true}
              aria-disabled={aria_disabled}
            >
              <Text style={{ padding: 10 }}>text one</Text>
            </ScrollView>
          </View>
        </View>          


        <View style={styles.scrollViewProps}>
          <Text>9.属性值：aria-expanded </Text>
          <View>
            <Text>预期结果：</Text>
            <Text>1.当aria-expanded为false时，表示元素不是展开</Text>
            <Text>2.当aria-expanded为true时，表示元素是展开的</Text>
          </View>
          <View>
            <Text>实际结果：</Text>
            <Button
              title={'aria_expanded: ' + aria_expanded.toString()}
              onPress={() => setAria_expanded(!aria_expanded)}
              label={'aria_expanded: ' + aria_expanded.toString()}
            />
            <ScrollView
              style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}
              accessible={true}
              aria-expanded={aria_expanded}
            >
              <Text style={{ padding: 10 }}>text one</Text>
              <Text style={{ padding: 10 }}>text two</Text>
            </ScrollView>
          </View>
        </View>


        <View style={styles.scrollViewProps}>
          <Text>10.属性值：aria-hidden </Text>
          <View>
            <Text>预期结果：</Text>
            <Text>1.当aria-hidden为false时，表示元素可见，可点击文本内容</Text>
            <Text>2.当aria-hidden为true时，表示元素隐藏(不可见)，不可点击到文本</Text>
          </View>
          <View>
            <Text>实际结果：</Text>
            <Button
              title={'aria_hidden: ' + aria_hidden.toString()}
              onPress={() => setAria_hidden(!aria_hidden)}
              label={'aria_hidden: ' + aria_hidden.toString()}
            />
            <ScrollView
              style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}
              accessible={true}
              aria-hidden={aria_hidden}
            >
              <Text style={{ padding: 10 }}>text one</Text>
              <Text style={{ padding: 10 }}>text one</Text>
            </ScrollView>
          </View>
        </View>


        <View style={styles.scrollViewProps}>
          <Text>11.属性值：{"aria-label='这是在一个“安全”的可视区域内渲染内容的组件'"} </Text>
          <View>
            <Text>预期结果：点击文本内容提示：这是在一个“安全”的可视区域内渲染内容的组件</Text>
          </View>
          <View>
            <Text>实际结果：</Text>
            <ScrollView
              style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}
              accessible={true}
              aria-label='这是在一个“安全”的可视区域内渲染内容的组件'
            >
              <Text style={{ padding: 10 }}>text one</Text>
              <Text style={{ padding: 10 }}>text two</Text>
            </ScrollView>
          </View>
        </View>


        <View style={styles.scrollViewProps}>
          <Text>12.属性值：accessibilityLanguage</Text>
          <View>
            <Text>预期效果:使用中文的辅助功能打开时，点击这个视图，可以听到 "Hello" 的声音提示</Text>
          </View>
          <View>
            <Text>实际效果:</Text>
            <ScrollView
              style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}
              accessible={true}
              accessibilityLabel="Hello"
              accessibilityLanguage="zh">
            </ScrollView>
          </View>
        </View>


        <View style={styles.scrollViewProps}>
          <Text>13.属性值：accessibilityIgnoresInvertColors</Text>
          <View>
            <Text>预期效果:</Text>
            <Text>1.当accessibilityIgnoresInvertColors值为false，开启反色功能后，颜色将不会受到反色功能的影响，保持原来的颜色</Text>
            <Text>1.当accessibilityIgnoresInvertColors值为true，开启反色功能后，颜色都会受到反色功能的影响，发生颜色反转</Text>
          </View>
          <View>
            <Text>实际效果:</Text>
            <Button
              title={'反转屏幕颜色: ' + accessibilityIgnoresInvertColors.toString()}
              onPress={() => setAccessibilityIgnoresInvertColors(!accessibilityIgnoresInvertColors)}
              label={'accessibilityIgnoresInvertColors: ' + setAccessibilityIgnoresInvertColors.toString()}
            />
            <ScrollView
              style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, backgroundColor: 'red' }}
              accessible={true}
              accessibilityIgnoresInvertColors={false}>
              <Text>text</Text>
            </ScrollView>
          </View>
        </View>


        <View style={styles.scrollViewProps}>
          <Text>14.属性值：onAccessibilityTap</Text>
          <View>
            <Text>预期结果：当双击下方文字，log文本内容由ready变为run done</Text>
          </View>
          <View>
            <Text>实际结果：</Text>
            <ScrollView
              style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}
              accessible={true}
              accessibilityLabel="press me"
              onAccessibilityTap={() => {
                setOnAccessibilityTap('run done')
              }}
            >
              <Text style={{ fontSize: 16 }}>press me</Text>
            </ScrollView>
            <View style={{ marginTop: 10, borderColor: '#f0f0f0', backgroundColor: '#f9f9f9', }}>
              <Text>log文本：</Text>
              <Text style={{ padding: 10 }}>{onAccessibilityTap}</Text>
            </View>
          </View>
        </View>
        

        <View style={styles.scrollViewProps}>
          <Text>15.属性值：{"aria-valuetext='20'"} </Text>
          <View>
            <Text>预期结果：读出aria-valuetext的值，20 </Text>
          </View>
          <View>

            <Text>实际结果：</Text>
            <ScrollView
              style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}
              accessible={true}
              aria-valuetext='20'
            >
              <Text style={{ padding: 10 }} >text one</Text>
              <Text style={{ padding: 10 }} >text two</Text>
            </ScrollView>
          </View>
        </View>


        <View style={styles.scrollViewProps}>
          {accessibilityRoleList.map((item, index) => (
            <View key={index} style={{ paddingBottom: 10 }}>
              <Text>16.{index + 1}属性值：accessibilityRole-{item.type}</Text>
              <Text>预期结果：{item.expect}</Text>
              <Text>实际结果：</Text>
              <ScrollView
                style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}
                accessible={true}
                accessibilityRole={item.type}>
                <Text style={{ padding: 10 }} >{item.value}</Text>
              </ScrollView>
            </View>
          ))}
        </View>        


        <View style={styles.scrollViewProps}>
          <Text>17.属性值：accessibilityValue</Text>
          <View>
            <Text>预期结果:提示当前进度为50%</Text>
          </View>
          <View>
            <Text>实际结果：</Text>
            <ScrollView
              style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}
              accessible={true}
              accessibilityValue={{ text: '当前进度为50%' }}>
              <View style={{ height: 30, width: 135, backgroundColor: 'green' }}></View>
            </ScrollView>
          </View>
        </View> 


        <View style={styles.scrollViewProps}>
          <Text>18.属性值： aria-live</Text>
          <View>
            <Text>预期结果：</Text>
            <Text>当属性值aria-live为off，点击按钮后不会读出文本内容，</Text>
            <Text>当属性值aria-live为polite，点击按钮后在用户闲时宣布，读出文本内容，</Text>
            <Text>当属性值aria-live为assertive，点击发送后尽快对用户宣布，提醒用户当前视图的变化读出文本内容</Text>
            <Text>当属性值aria-live为rude，点击按钮后即时提醒用户，必要的时候甚至中断用户，提醒用户当前视图的变化读出文本内容</Text>
            <View>
              <Text>实际结果：</Text>
            </View>
            <View>
              {aria_lives.map(value => (
                <Button
                  title={value}
                  active={value === aria_live}
                  label={value}
                  key={value}
                  onPress={() => {
                    setAria_live(value);
                    setMessageTwo('')
                  }}
                  style={{ height: 30, width: 10 }}
                />
              ))}
            </View>
            <ScrollView
              style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}
              accessible={true}
              aria-live={aria_live}
            >
              <Text>{messageTwo}</Text>
            </ScrollView>
            <Button title="按钮" onPress={() => { setMessageTwo('内容更新啦！'); }} />
          </View>
        </View> 

        
        <View style={styles.scrollViewProps}>
          <View>
            <Text>19.1属性值：accessibilityState={"{{ 'disabled': true }}"}</Text>
            <Text>预期效果:点击文本内容后，提示“text one 不可点击”</Text>
            <Text>实际效果:</Text>
            <ScrollView
              style={{
                flex: 1,
                borderWidth: 5,
                borderColor: '#527FE4'
              }}
              accessible={true}
              accessibilityState={{ 'disabled': true }}
            >
              <Text>text one</Text>
            </ScrollView>
          </View>
          <View style={{ marginTop: 10 }}>
            <Text>19.2属性值：accessibilityState={"{{ 'selected': true }}"}</Text>
            <Text>预期效果:点击文本内容后，提示“已选中 text one”</Text>
            <Text>实际效果:</Text>
            <ScrollView
              style={{
                flex: 1,
                borderWidth: 5,
                borderColor: '#527FE4'
              }}
              accessible={true}
              accessibilityState={{ 'selected': true }}
            >
              <Text>text one</Text>
            </ScrollView>
          </View>
          <View style={{ marginTop: 10 }}>
            <Text>19.3属性值：accessibilityState={"{{ 'checked': true }}"}</Text>
            <Text>预期效果:点击文本内容后，提示“已选中 text one”</Text>
            <Text>实际效果:</Text>
            <ScrollView
              style={{
                flex: 1,
                borderWidth: 5,
                borderColor: '#527FE4'
              }}
              accessible={true}
              accessibilityState={{ 'checked': true }}
            >
              <Text>text one</Text>
            </ScrollView>
          </View>
          <View style={{ marginTop: 10 }}>
            <Text>19.4属性值：accessibilityState={"{{ 'busy': true }}"}</Text>
            <Text>预期效果:点击文本内容后提示，busy</Text>
            <Text>实际效果:</Text>
            <ScrollView
              style={{
                flex: 1,
                borderWidth: 5,
                borderColor: '#527FE4'
              }}
              accessible={true}
              accessibilityState={{ 'busy': true }}
            >
              <Text>text one</Text>
            </ScrollView>
          </View>
          <View style={{ marginTop: 10 }}>
            <Text>19.5属性值：accessibilityState={"{{ 'expanded': true }}"}</Text>
            <Text>预期效果:点击文本内容过后提示，“已展开 text one”</Text>
            <Text>实际效果:</Text>
            <ScrollView
              style={{
                flex: 1,
                borderWidth: 5,
                borderColor: '#527FE4'
              }}
              accessible={true}
              accessibilityState={{ 'expanded': true }}
            >
              <Text>text one</Text>
            </ScrollView>
          </View>
        </View>


        <View style={styles.scrollViewProps}>
          <Text>20.属性值：aria-selected</Text>
          <View>
            <Text>预期结果：点击text one,读出“已选中text one”，点击text two读出“text two”</Text>
          </View>
          <View>
            <Text>实际结果：</Text>
            <View style={{ flexDirection: 'row' }}>
              <ScrollView
                style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, flexDirection: 'row', width: 80, marginRight: 10 }}
                accessible={true}
                aria-selected={true}
              >
                <View style={{ width: 20, height: 20, borderRadius: 10, backgroundColor: 'red', marginRight: 10 }}></View>
                <Text>text one</Text>
              </ScrollView>
              <ScrollView
                style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, flexDirection: 'row', width: 80, }}
                accessible={true}
                aria-selected={false}
              >
                <View style={{ width: 20, height: 20, borderRadius: 10, backgroundColor: 'grey', marginRight: 10 }}></View>
                <Text>text two</Text>
              </ScrollView>
            </View>
          </View>
        </View>


        <View style={styles.scrollViewProps}>
          <Text>21.属性值：onMagicTap</Text>
          <View>
            <Text>预期效果:当双指双击下方文字，log文本内容由ready变为run done</Text>
          </View>
          <View>
            <Text>实际效果:</Text>
            <ScrollView
              style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}
              accessible={true}
              onMagicTap={() => {
                setOnMagicTap('run done!')
              }}
            >
              <Text style={{ fontSize: 16 }}>press me</Text>
            </ScrollView>
            <View style={{ marginTop: 10, borderColor: '#f0f0f0', backgroundColor: '#f9f9f9', }}>
              <Text>log文本：</Text>
              <Text style={{ padding: 10 }}>{onMagicTap}</Text>
            </View>
          </View>
        </View>


        <View style={styles.scrollViewProps}>
          {roleList.map((item, index) => (
            <View key={index} style={{ paddingBottom: 10 }}>
              <Text>22.{index}属性值：role-{item.type}</Text>
              <Text>预期结果：{item.expect}</Text>
              <Text>实际结果：</Text>
              <ScrollView
                style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}
                accessible={true}
                role={item.type}>
                <Text style={{ padding: 10 }} >{item.value}</Text>
              </ScrollView>
            </View>
          ))}
        </View>


        <View style={styles.scrollViewProps}>
          <Text style={styles.title}>23.属性值：accessibilityHint</Text>
          <View>
            <Text style={{ padding: 3 }}>预期效果：accessibilityHint帮助用户理解操作可能导致什么后果，让屏幕阅读器显示“这个容器是蓝色边框”和提示“one and two”</Text>
          </View>
          <View>
            <Text>实际结果：</Text>
            <ScrollView
              style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}
              accessible={true}
              aria-label="这个容器是蓝色边框"
              accessibilityHint="one and two"
            >
              <Text style={{ padding: 10 }}>text one</Text>
              <Text style={{ padding: 10 }}>text two</Text>
            </ScrollView>
          </View>
        </View>


        <View style={styles.scrollViewProps}>
          <Text style={styles.title}>24.属性值：accessibilityElementsHidden</Text>
          <View>
            <Text style={{ padding: 3 }}>预期效果：视图2会被隐藏</Text>
          </View>
          <View>
            <Text>实际效果：</Text>
            <View accessible={true} style={styles.accessibilityContainer}>
              <ScrollView
                style={[styles.accessibilityLayout, { backgroundColor: 'green' }]}>
                <Text>First layout</Text>
              </ScrollView>
              <ScrollView
                style={[styles.accessibilityLayout, { backgroundColor: 'yellow' }]}
                accessibilityElementsHidden={true}>
                <Text>Hidden Layout</Text>
              </ScrollView>
            </View>
          </View>
        </View>


        <View style={styles.scrollViewProps}>
          <Text style={styles.title}>25.属性值：importantForAccessibility</Text>
          <View>
            <Text style={{ padding: 3 }}>预期效果：当可访问性为true时，渲染“First Layout”视图并忽略“Ignored Layout”</Text>
          </View>
          <View>
            <Text>实际效果：</Text>
            <ScrollView accessible={true} style={styles.accessibilityContainer}>
              <ScrollView
                style={[styles.accessibilityLayout, { backgroundColor: 'green' }]}
                importantForAccessibility="yes">
                <Text>First layout</Text>
              </ScrollView>
              <ScrollView
                style={[styles.accessibilityLayout, { backgroundColor: 'yellow' }]}
                importantForAccessibility="no-hide-descendants">
                <Text>Ignored Layout</Text>
              </ScrollView>
            </ScrollView>
          </View>
        </View>


        <View style={styles.scrollViewProps}>
          <Text style={styles.title}>26.属性值：aria-labelledby</Text>
          <View>
            <Text style={{ padding: 3 }}>预期结果：点击文本“text one”，读出“aria-labelledby”</Text>
          </View>
          <View>
            <Text>实际结果：</Text>
            <Text nativeID="formLabel">aria-labelledby</Text>
            <ScrollView
              style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}
              accessible={true}
              aria-labelledby={'formLabel'}
            >
              <Text>text one</Text>
            </ScrollView>
          </View>
        </View>


        <View style={styles.scrollViewProps}>
          <Text style={styles.title}>27.属性值：accessibilityLabelledBy</Text>
          <View>
            <Text style={{ padding: 3 }}>预期结果：点击文本“text one”，读出“accessibilityLabelledBy”</Text>
          </View>
          <View>
            <Text>实际结果：</Text>
            <Text nativeID="formLabel">accessibilityLabelledBy</Text>
            <ScrollView
              style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}
              accessible={true}
              accessibilityLabelledBy={'formLabel'}
            >
              <Text>text one</Text>
            </ScrollView>
          </View>
        </View>


        <View style={styles.scrollViewProps}>
          <Text>28.属性值：{"aria-valuemax='100'"} </Text>
          <View>
            <Text>预期结果：读出aria-valuemin的值，100 </Text>
          </View>
          <View>
            <Text>实际结果：</Text>
            <ScrollView
              style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}
              accessible={true}
              aria-valuemax={0}
            >
              <Text style={{ padding: 10 }} >text one</Text>
              <Text style={{ padding: 10 }} >text two</Text>
            </ScrollView>
          </View>
        </View>


        <View style={styles.scrollViewProps}>
          <Text>29.属性值：{"aria-valuemin='0'"} </Text>
          <View>
            <Text>预期结果：读出aria-valuemin的值，0 </Text>
          </View>
          <View>
            <Text>实际结果：</Text>
            <ScrollView
              style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}
              accessible={true}
              aria-valuemin={0}
            >
              <Text style={{ padding: 10 }} >text one</Text>
              <Text style={{ padding: 10 }} >text two</Text>
            </ScrollView>
          </View>
        </View>


        <View style={styles.scrollViewProps}>
          <Text>30.属性值：{"aria-valuenow='40'"} </Text>
          <View>
            <Text>预期结果：读出aria-valuenow的值，40 </Text>
          </View>
          <View>
            <Text>实际结果：</Text>
            <ScrollView
              style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}
              accessible={true}
              aria-valuenow='40'
            >
              <Text style={{ padding: 10 }} >text one</Text>
              <Text style={{ padding: 10 }} >text two</Text>
            </ScrollView>
          </View>
        </View>


        <View style={styles.scrollViewProps}>
          <Text>31.属性值：onAccessibilityEscape</Text>
          <View>
            <Text>预期效果:log文本内容由ready变为run done</Text>
          </View>
          <View>
            <Text>实际效果:</Text>
            <ScrollView
              accessible={true}
              style={{ width: '100%', height: 100, backgroundColor: 'gray' }}
              onAccessibilityEscape={() => {
                console.log('onAccessibilityEscape called!');
                setOnAccessibilityEscape('run done')
              }}>
              <View
                style={{
                  width: 100,
                  height: 100,
                  backgroundColor: 'red',
                }}
              />
            </ScrollView>
            <View style={{ marginTop: 10, borderColor: '#f0f0f0', backgroundColor: '#f9f9f9', }}>
              <Text>log文本：</Text>
              <Text style={{ padding: 10 }}>{onAccessibilityEscape}</Text>
            </View>
          </View>
        </View>


        <View style={styles.scrollViewProps}>
          <Text>32.属性值：aria-modal</Text>
          <View>
            <Text>预期结果：</Text>
            <Text>1.当aria-modal为false时，点击“打开”按钮后，仍然可以访问text one</Text>
            <Text>2.当aria-modal为true时，点击“打开”按钮后，只能访问text two，text three，不能访问其他元素，直到点击“关闭”按钮 </Text>
          </View>
          <View>
            <Text>实际结果：</Text>
            <Button
              title={'aria_modal: ' + aria_modal.toString()}
              onPress={() => setAria_modal(!aria_modal)}
              label={'aria_modal: ' + aria_modal.toString()}
            />
            <View>
              <View
                accessible={true}
                style={{ height: 50, borderColor: 'pink', borderWidth: 5, justifyContent: 'center' }}>
                <Text style={{ fontSize: 16 }}>text one</Text>
              </View>
              <ScrollView
                style={{ borderColor: '#527FE4', borderWidth: 5, }}
                accessible={true}
                aria-modal={aria_modal}
                display={display}
              >
                <View aria-hidden={true}>
                  <Text style={{ fontSize: 16, padding: 5 }}>text two</Text>
                  <Text style={{ fontSize: 16, padding: 5 }}>text three</Text>
                </View>
                <Button title='关闭' onPress={() => { setDisplay("none") }}>关闭</Button>
              </ScrollView>
            </View>
            <Pressable style={{ borderWidth: 1, height: 30, width: 120, borderRadius: 5, backgroundColor: 'skyblue', justifyContent: 'center', alignItems: 'center' }}
              onPress={() => {
                setDisplay("black")
              }}>
              <Text>打开</Text>
            </Pressable>
          </View>
        </View>

      </View>
    </ScrollView>
  );
}

const styles = StyleSheet.create({
  scrollViewProps:{
    width:300,
    marginBottom:30,
    borderWidth:1,
    borderColor:'black',
    borderStyle:'solid',
  },
  scrollView: {
    backgroundColor: '#eeeeee',
    height: 300,
  },
  horizontalScrollView: {
    height: 106,
  },
  text: {
    fontSize: 16,
    fontWeight: 'bold',
    margin: 5,
  },
  activeButton: {
    backgroundColor: 'rgba(100,215,255,.3)',
  },
  button: {
    margin: 5,
    padding: 5,
    alignItems: 'center',
    backgroundColor: '#cccccc',
    borderRadius: 3,
  },
  row: {
    flexDirection: 'row',
    justifyContent: 'space-around',
  },
  item: {
    margin: 5,
    padding: 5,
    backgroundColor: '#cccccc',
    borderRadius: 3,
    minWidth: 96,
  },
  containerStyle: {
    backgroundColor: '#aae3b6',
  },
  rowTitle: {
    flex: 1,
    fontWeight: 'bold',
    alignSelf: 'center',
  },
  textInput: {
    height: 40,
    borderColor: 'gray',
    borderWidth: 1,
  },
  box: {
    backgroundColor: '#aaccff',
    borderWidth: 1,
    borderColor: '#7799cc',
    padding: 10,
    margin: 5,
  },
  boxPassedThrough: {
    borderColor: '#99bbee',
  },
  pointerEventBoxNone: {
    pointerEvents: 'box-none',
  },
  pointerEventBoxOnly: {
    pointerEvents: 'box-only',
  },
  pointerEventNone: {
    pointerEvents: 'none',
  },
  pointerEventAuto: {
    pointerEvents: 'auto',
  },
  logText: {
    fontSize: 9,
  },
  logBox: {
    padding: 20,
    margin: 10,
    borderWidth: 0.5,
    borderColor: '#f0f0f0',
    backgroundColor: '#f9f9f9',
  },
  text: {
    fontSize: 10,
    color: '#5577cc',
  },
  textPassedThrough: {
    color: '#88aadd',
  },
  wrapperView: {
    height: 300,
    width: '60%',
  },
  accessibilityContainer: {
    width: '100%',
    backgroundColor: 'gray',
  },
  title: {
    fontSize: 18
  },
});

