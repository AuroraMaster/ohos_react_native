/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {
    Text,
    StyleSheet,
    View,
    ScrollView,
    TouchableWithoutFeedback,
    Button,
    Pressable,
    Image,
} from 'react-native';
import React, { useState } from 'react';

const accessibilityRoleList = [
    { type: 'button', expect: '提示当前内容是一个按钮', value: 'button' },
    { type: 'togglebutton', expect: '提示当前内容是一个状态按钮', value: 'togglebutton' },
    { type: 'search', expect: '提示当前内容是一个编辑框', value: 'search' },
    { type: 'searchbox', expect: '提示当前内容是一个编辑框', value: 'searchbox' },
    { type: 'img', expect: '提示当前内容是一个图片', value: 'img' },
    { type: 'image', expect: '提示当前内容是一个图片', value: 'image' },
    { type: 'adjustable', expect: '提示当前内容是一个进度条', value: 'adjustable' },
    { type: 'imagebutton', expect: '提示当前内容是一个按钮', value: 'imagebutton' },
    { type: 'checkbox', expect: '提示当前内容是一个复选框', value: 'checkbox' },
    { type: 'progressbar', expect: '提示当前内容是一个进度条', value: 'progressbar' },
    { type: 'radio', expect: '提示当前内容是一个单选框', value: 'radio' },
    { type: 'scrollbar', expect: '提示当前内容是一个垂直滚动条', value: 'scrollbar' },
    { type: 'switch', expect: '提示当前内容是一个状态按钮', value: 'switch' },
    { type: 'list', expect: '提示当前内容是一个列表', value: 'list' },
    { type: 'slider', expect: '提示当前内容是一个滑动条', value: 'slider' },
]

export const TouchableWithoutFeedbackAccessibleExample = () => {
    const [accessibilityIgnoresInvertColors, setAccessibilityIgnoresInvertColors] = useState(false);
    const [changeValue, setChangeValue] = useState(true);
    const [messageOne, setMessageOne] = useState('');
    const accessibilityLiveRegions = ['none', 'polite', 'assertive'];
    const [accessibilityLiveRegion, setAccessibilityLiveRegion] = useState('none');
    const [bg, setBg] = useState('#FFFFFF');
    const [aria_hidden, setAria_hidden] = useState(false);
    const aria_lives = ['off', 'polite', 'assertive', 'rude'];
    const [aria_live, setAria_live] = useState('none');
    const [messageTwo, setMessageTwo] = useState('');
    const [aria_modal, setAria_modal] = useState(false);
    const [display, setDisplay] = useState("none");
    return (
        <ScrollView>
            <View>
                <View>
                    <Text style={{ padding: 3 }}>属性值:aria-busy</Text>
                    <Text style={{ padding: 3 }}>预期效果:选中下方黑色区域,屏幕朗读“busy”;选中下方天蓝色区域,屏幕朗读“天蓝色区域” </Text>
                    <TouchableWithoutFeedback
                        accessible={true}
                        accessibilityLabel="黑色区域"
                        aria-busy={true}
                    >
                        <View style={{ flex: 1, height: 40, backgroundColor: 'black' }} />
                    </TouchableWithoutFeedback>
                    <TouchableWithoutFeedback
                        accessible={true}
                        accessibilityLabel="天蓝色区域"
                        aria-busy={false}
                    >
                        <View style={{ flex: 1, height: 40, backgroundColor: 'skyblue' }} />
                    </TouchableWithoutFeedback>
                </View>
            </View>

            <View>
                <View>
                    <Text style={{ padding: 3 }}>属性值:aria-checked</Text>
                    <Text style={{ padding: 3 }}>预期效果:选中下方黑色区域,屏幕朗读“未选中黑色区域,单机双击即可选中”;
                        选中下方天蓝色区域,屏幕朗读“已选中天蓝色区域,单机双击即可取消选中”;
                        选中下方绿色区域,屏幕朗读“mixed”</Text>
                    <TouchableWithoutFeedback
                        accessible={true}
                        accessibilityLabel="黑色区域"
                        aria-checked={false}
                        role='checkbox'
                    >
                        <View style={{ flex: 1, height: 40, backgroundColor: 'black' }} />
                    </TouchableWithoutFeedback>
                    <TouchableWithoutFeedback
                        accessible={true}
                        accessibilityLabel="天蓝色区域"
                        aria-checked={true}
                        role='checkbox'
                    >
                        <View style={{ flex: 1, height: 40, backgroundColor: 'skyblue' }} />
                    </TouchableWithoutFeedback>
                    <TouchableWithoutFeedback
                        accessible={true}
                        accessibilityLabel="绿色区域"
                        aria-checked='mixed'
                        role='checkbox'
                    >
                        <View style={{ flex: 1, height: 40, backgroundColor: 'green' }} />
                    </TouchableWithoutFeedback>
                </View>
            </View>

            <View>
                <View>
                    <Text style={{ padding: 3 }}>属性值:aria-disabled</Text>
                    <Text style={{ padding: 3 }}>预期效果:选中下方黑色区域,无屏幕朗读“黑色区域,不可点击”；选中下方天蓝色区域,屏幕朗读“天蓝色区域” </Text>
                    <TouchableWithoutFeedback
                        accessible={true}
                        accessibilityLabel="黑色区域"
                        aria-disabled={true}
                    >
                        <View style={{ flex: 1, height: 40, backgroundColor: 'black' }} />
                    </TouchableWithoutFeedback>
                    <TouchableWithoutFeedback
                        accessible={true}
                        accessibilityLabel="天蓝色区域"
                        aria-disabled={false}
                    >
                        <View style={{ flex: 1, height: 40, backgroundColor: 'skyblue' }} />
                    </TouchableWithoutFeedback>
                </View>
            </View>

            <View>
                <View>
                    <Text style={{ padding: 3 }}>属性值:aria-expanded</Text>
                    <Text style={{ padding: 3 }}>预期效果:选中下方黑色区域,屏幕朗读“expanded”;选中下方天蓝色区域,屏幕朗读“alive” </Text>
                    <TouchableWithoutFeedback
                        accessible={true}
                        accessibilityLabel="黑色区域"
                        aria-expanded={true}
                    >
                        <View style={{ flex: 1, height: 40, backgroundColor: 'black' }} />
                    </TouchableWithoutFeedback>
                    <TouchableWithoutFeedback
                        accessible={true}
                        accessibilityLabel="天蓝色区域"
                        aria-expanded={false}
                    >
                        <View style={{ flex: 1, height: 40, backgroundColor: 'skyblue' }} />
                    </TouchableWithoutFeedback>
                </View>
            </View>

            <View>
                <Text>属性值：accessibilityIgnoresInvertColors</Text>
                <View>
                    <Text>预期效果:</Text>
                    <Text>当accessibilityIgnoresInvertColors值为false，开启反色功能后，颜色将不会受到反色功能的影响，保持原来的颜色</Text>
                    <Text>当accessibilityIgnoresInvertColors值为true，开启反色功能后，颜色都会受到反色功能的影响，发生颜色反转</Text>
                </View>
                <View>
                    <Text>实际效果:</Text>
                    <Button
                        title={'反转屏幕颜色: ' + accessibilityIgnoresInvertColors.toString()}
                        onPress={() => setAccessibilityIgnoresInvertColors(!accessibilityIgnoresInvertColors)}
                    />
                    <TouchableWithoutFeedback
                        style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, backgroundColor: 'red' }}
                        accessible={true}
                        accessibilityIgnoresInvertColors={false}>
                        <Text>text</Text>
                    </TouchableWithoutFeedback>
                </View>
            </View>

            <View>
                <Text style={styles.title}>属性值：accessibilityElementsHidden</Text>
                <View>
                    <Text style={{ padding: 3 }}>预期效果：视图2会被隐藏</Text>
                </View>
                <View>
                    <Text>实际效果：</Text>
                    <View accessible={true} style={styles.accessibilityContainer}>
                        <TouchableWithoutFeedback
                            style={[styles.accessibilityLayout, { backgroundColor: 'green' }]}>
                            <Text>First layout</Text>
                        </TouchableWithoutFeedback>
                        <TouchableWithoutFeedback
                            style={[styles.accessibilityLayout, { backgroundColor: 'yellow' }]}
                            accessibilityElementsHidden={true}>
                            <Text>Hidden Layout</Text>
                        </TouchableWithoutFeedback>
                    </View>
                </View>
            </View>

            <View>
                <Text>属性值：{'accessible={changeValue}'}</Text>
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
                    <TouchableWithoutFeedback
                        style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5 }}
                        accessible={changeValue}
                    >
                        <View style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}>
                            <Text style={{ padding: 10 }}>text one</Text>
                            <Text style={{ padding: 10 }}>text two</Text>
                            <Text style={{ padding: 10 }}>text three</Text>
                        </View>
                    </TouchableWithoutFeedback>
                </View>
            </View>

            <View>
                <Text>属性值：{"accessibilityLabel='Tap me!'"}</Text>
                <View>
                    <Text>预期结果：设置accessibilityLabel标签后，点击会读出选中元素的无障碍标签Tap me,不会读文本内容：text one</Text>
                </View>
                <View>
                    <Text>实际结果：</Text>
                    <TouchableWithoutFeedback
                        style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5 }}
                        accessible={true}
                        accessibilityLabel='Tap me!'
                    >
                        <Text style={{ padding: 10, borderColor: '#527FE4', borderWidth: 5, }}>text one</Text>
                    </TouchableWithoutFeedback>
                </View>
            </View>

            <View>
                <Text>属性值： accessibilityLiveRegion</Text>
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
                                key={value}
                                onPress={() => {
                                    setAccessibilityLiveRegion(value);
                                    setMessageOne('')
                                }}
                            />
                        ))}
                    </View>
                    <TouchableWithoutFeedback
                        style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}
                        accessible={true}
                        accessibilityLiveRegion={accessibilityLiveRegion}
                    >
                        <View style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}>
                            <Text>{messageOne}</Text>
                        </View>
                    </TouchableWithoutFeedback>
                    <Button title="点击" onPress={() => { setMessageOne('新消息已发送'); }} />
                </View>
            </View>

            <View>
                <Text style={styles.title}>属性值：accessibilityHint</Text>
                <View>
                    <Text style={{ padding: 3 }}>预期效果：accessibilityHint帮助用户理解操作可能导致什么后果，让屏幕阅读器显示“这个容器是蓝色边框”和提示“one and two”</Text>
                </View>
                <View>
                    <Text>实际结果：</Text>
                    <TouchableWithoutFeedback
                        style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}
                        accessible={true}
                        aria-label="这个容器是蓝色边框"
                        accessibilityHint="one and two"
                    >
                        <View style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}>
                            <Text style={{ padding: 10 }}>text one</Text>
                            <Text style={{ padding: 10 }}>text two</Text>
                        </View>
                    </TouchableWithoutFeedback>
                </View>
            </View>

            <View>
                {accessibilityRoleList.map((item, index) => (
                    <View key={index} style={{ paddingBottom: 10 }}>
                        <Text>属性值：{item.type}</Text>
                        <Text>预期结果：{item.expect}</Text>
                        <Text>实际结果：</Text>
                        <TouchableWithoutFeedback
                            accessible={true}>
                            <Text style={styles.text1}>{item.value}</Text>
                        </TouchableWithoutFeedback>
                    </View>
                ))}
            </View>

            <View>
                <View>
                    <Text>13.1属性值：accessibilityState={"{{ 'disabled': true }}"}</Text>
                    <Text>预期效果:点击文本内容后，提示“text one 不可点击”</Text>
                    <Text>实际效果:</Text>
                    <TouchableWithoutFeedback
                        style={{
                            flex: 1,
                            borderWidth: 5,
                            borderColor: '#527FE4'
                        }}
                        accessible={true}
                        accessibilityState={{ 'disabled': true }}
                    >
                        <View style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}>
                            <Text>text one</Text>
                        </View>
                    </TouchableWithoutFeedback>
                </View>
                <View style={{ marginTop: 10 }}>
                    <Text>13.2属性值：accessibilityState={"{{ 'selected': true }}"}</Text>
                    <Text>预期效果:点击文本内容后，提示“已选中 text one”</Text>
                    <Text>实际效果:</Text>
                    <TouchableWithoutFeedback
                        style={{
                            flex: 1,
                            borderWidth: 5,
                            borderColor: '#527FE4'
                        }}
                        accessible={true}
                        accessibilityState={{ 'selected': true }}
                    >
                        <View style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}>
                            <Text>text one</Text>
                        </View>
                    </TouchableWithoutFeedback>
                </View>
                <View style={{ marginTop: 10 }}>
                    <Text>13.3属性值：accessibilityState={"{{ 'checked': true }}"}</Text>
                    <Text>预期效果:点击文本内容后，提示“已选中 text one”</Text>
                    <Text>实际效果:</Text>
                    <TouchableWithoutFeedback
                        style={{
                            flex: 1,
                            borderWidth: 5,
                            borderColor: '#527FE4'
                        }}
                        accessible={true}
                        accessibilityState={{ 'checked': true }}
                    >
                        <View style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}>
                            <Text>text one</Text>
                        </View>
                    </TouchableWithoutFeedback>
                </View>
                <View style={{ marginTop: 10 }}>
                    <Text>13.4属性值：accessibilityState={"{{ 'busy': true }}"}</Text>
                    <Text>预期效果:点击文本内容后提示，busy</Text>
                    <Text>实际效果:</Text>
                    <TouchableWithoutFeedback
                        style={{
                            flex: 1,
                            borderWidth: 5,
                            borderColor: '#527FE4'
                        }}
                        accessible={true}
                        accessibilityState={{ 'busy': true }}
                    >
                        <View style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}>
                            <Text>text one</Text>
                        </View>
                    </TouchableWithoutFeedback>
                </View>
                <View style={{ marginTop: 10 }}>
                    <Text>13.5属性值：accessibilityState={"{{ 'expanded': true }}"}</Text>
                    <Text>预期效果:点击文本内容过后提示，“已展开 text one”</Text>
                    <Text>实际效果:</Text>
                    <TouchableWithoutFeedback
                        style={{
                            flex: 1,
                            borderWidth: 5,
                            borderColor: '#527FE4'
                        }}
                        accessible={true}
                        accessibilityState={{ 'expanded': true }}
                    >
                        <View style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}>
                            <Text>text one</Text>
                        </View>
                    </TouchableWithoutFeedback>
                </View>
            </View>

            <View>
                <Text>3.属性值：{"accessibilityActions={[{name: 'activate',label: 'activate'}]} "} </Text>
                <Text>4.属性值：onAccessibilityAction</Text>
                <View>
                    <Text>预期结果：双击文本内容后，背景变为粉红色</Text>
                </View>
                <View>
                    <Text>实际结果：</Text>
                    <TouchableWithoutFeedback
                        style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}
                        accessible={true}
                        accessibilityActions={[{ name: 'activate', label: 'activate' }]}
                        onAccessibilityAction={event => {
                            if (event.nativeEvent.actionName === 'activate') {
                                setBg('pink');
                            }
                        }}
                    >
                        <Text style={{ padding: 10, borderWidth: 5, borderColor: '#527FE4', backgroundColor: bg }}>text one</Text>
                    </TouchableWithoutFeedback>
                </View>
            </View>

            <View>
                <Text>属性值：aria-hidden </Text>
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
                    <TouchableWithoutFeedback
                        style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}
                        accessible={true}
                        aria-hidden={aria_hidden}
                    >
                        <View style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}>
                            <Text style={{ padding: 10 }}>text one</Text>
                            <Text style={{ padding: 10 }}>text one</Text>
                        </View>
                    </TouchableWithoutFeedback>
                </View>
            </View>

            <View>
                <Text>属性值：{"aria-label='这是在一个“安全”的可视区域内渲染内容的组件'"} </Text>
                <View>
                    <Text>预期结果：点击文本内容提示：这是在一个“安全”的可视区域内渲染内容的组件</Text>
                </View>
                <View>
                    <Text>实际结果：</Text>
                    <TouchableWithoutFeedback
                        style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}
                        accessible={true}
                        aria-label='这是在一个“安全”的可视区域内渲染内容的组件'
                    >
                        <View style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}>
                            <Text style={{ padding: 10 }}>text one</Text>
                            <Text style={{ padding: 10 }}>text two</Text>
                        </View>
                    </TouchableWithoutFeedback>
                </View>
            </View>

            <View>
                <Text>属性值： aria-live</Text>
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
                                // active={value === aria_live}
                                // label={value}
                                key={value}
                                onPress={() => {
                                    setAria_live(value);
                                    setMessageTwo('')
                                }}
                                // style={{ height: 30, width: 10 }}
                            />
                        ))}
                    </View>
                    <TouchableWithoutFeedback
                        style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}
                        accessible={true}
                        // aria-live={aria_live}
                    >
                        <Text>{messageTwo}</Text>
                    </TouchableWithoutFeedback>
                    <Button title="按钮" onPress={() => { setMessageTwo('内容更新啦！'); }} />
                </View>
            </View>

            <View>
                <Text>属性值：aria-modal</Text>
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
                        // label={'aria_modal: ' + aria_modal.toString()}
                    />
                    <View>
                        <View
                            accessible={true}
                            style={{ height: 50, borderColor: 'pink', borderWidth: 5, justifyContent: 'center' }}>
                            <Text style={{ fontSize: 16 }}>text one</Text>
                        </View>
                        <TouchableWithoutFeedback
                            style={{ borderColor: '#527FE4', borderWidth: 5, }}
                            accessible={true}
                            aria-modal={aria_modal}
                            // display={display}
                        >
                            <View aria-hidden={true}>
                                <Text style={{ fontSize: 16, padding: 5 }}>text two</Text>
                                <Text style={{ fontSize: 16, padding: 5 }}>text three</Text>
                            </View>
                        </TouchableWithoutFeedback>
                    </View>
                    {/* <Button title='关闭' onPress={() => { setDisplay("none") }}>关闭</Button> */}
                    <Pressable style={{ borderWidth: 1, height: 30, width: 120, borderRadius: 5, backgroundColor: 'skyblue', justifyContent: 'center', alignItems: 'center' }}
                        onPress={() => {
                            setDisplay("black")
                        }}>
                        <Text>打开</Text>
                    </Pressable>
                </View>
            </View>

            <View>
                <Text>属性值：aria-selected</Text>
                <View>
                    <Text>预期结果：点击text one,读出“已选中text one”，点击text two读出“text two”</Text>
                </View>
                <View>
                    <Text>实际结果：</Text>
                    <View style={{ flexDirection: 'row' }}>
                        <TouchableWithoutFeedback
                            style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, flexDirection: 'row', width: 80, marginRight: 10 }}
                            accessible={true}
                            aria-selected={true}
                        >
                            <Text style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}>text one</Text>
                        </TouchableWithoutFeedback>
                        <TouchableWithoutFeedback
                            style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, flexDirection: 'row', width: 80, }}
                            accessible={true}
                            aria-selected={false}
                        >
                            <Text style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}>text two</Text>
                        </TouchableWithoutFeedback>
                    </View>
                </View>
            </View>

            <View>
                <Text>属性值：{"accessibilityActions={[{name: 'activate',label: 'activate'}]} "} </Text>
                <Text>属性值：onAccessibilityAction</Text>
                <View>
                    <Text>预期结果：双击文本内容后，背景变为粉红色</Text>
                </View>
                <View>
                    <Text>实际结果：</Text>
                    <TouchableWithoutFeedback
                        style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, backgroundColor: bg }}
                        accessible={true}
                        accessibilityActions={[{ name: 'activate', label: 'activate' }]}
                        onAccessibilityAction={event => {
                            if (event.nativeEvent.actionName === 'activate') {
                                setBg('pink');
                            }
                        }}
                    >
                        <View style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}>
                            <Text style={{ padding: 10 }}>text one</Text>
                            <Text style={{ padding: 10 }}>text two</Text>
                        </View>
                    </TouchableWithoutFeedback>
                </View>
            </View>

            <View>
                <Text>属性值：accessibilityValue</Text>
                <View>
                    <Text>预期结果:提示当前进度为50%</Text>
                </View>
                <View>
                    <Text>实际结果：</Text>
                    <TouchableWithoutFeedback
                        style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}
                        accessible={true}
                        accessibilityValue={{ text: '当前进度为50%' }}>
                        <View style={{ height: 30, width: 135, backgroundColor: 'green' }}></View>
                    </TouchableWithoutFeedback>
                </View>
            </View>

            <View>
                <Text>属性值：{"aria-valuemax='100'"} </Text>
                <View>
                    <Text>预期结果：读出aria-valuemin的值，100 </Text>
                </View>
                <View>
                    <Text>实际结果：</Text>
                    <TouchableWithoutFeedback
                        style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}
                        accessible={true}
                        aria-valuemax={0}
                    >
                        <View style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}>
                            <Text style={{ padding: 10 }} >text one</Text>
                            <Text style={{ padding: 10 }} >text two</Text>
                        </View>
                    </TouchableWithoutFeedback>
                </View>
            </View>

            <View>
                <Text>属性值：{"aria-valuemax='100'"} </Text>
                <View>
                    <Text>预期结果：读出aria-valuemin的值，100 </Text>
                </View>
                <View>
                    <Text>实际结果：</Text>
                    <TouchableWithoutFeedback
                        style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}
                        accessible={true}
                        aria-valuemax={0}
                    >
                        <View style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}>
                            <Text style={{ padding: 10 }} >text one</Text>
                            <Text style={{ padding: 10 }} >text two</Text>
                        </View>
                    </TouchableWithoutFeedback>
                </View>
            </View>

            <View>
                <Text>属性值：{"aria-valuenow='40'"} </Text>
                <View>
                    <Text>预期结果：读出aria-valuenow的值，40 </Text>
                </View>
                <View>
                    <Text>实际结果：</Text>
                    <TouchableWithoutFeedback
                        style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}
                        accessible={true}
                    >
                        <View style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}>
                            <Text style={{ padding: 10 }} >text one</Text>
                            <Text style={{ padding: 10 }} >text two</Text>
                        </View>
                    </TouchableWithoutFeedback>
                </View>
            </View>

            <View>
                <Text>属性值：{"aria-valuetext='20'"} </Text>
                <View>
                    <Text>预期结果：读出aria-valuetext的值，20 </Text>
                </View>
                <View>

                    <Text>实际结果：</Text>
                    <TouchableWithoutFeedback
                        style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}
                        accessible={true}
                        aria-valuetext='20'
                    >
                        <View style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}>
                            <Text style={{ padding: 10 }} >text one</Text>
                            <Text style={{ padding: 10 }} >text two</Text>
                        </View>
                    </TouchableWithoutFeedback>
                </View>
            </View>

            <View>
                <Text style={styles.title}>属性值：importantForAccessibility</Text>
                <View>
                    <Text style={{ padding: 3 }}>预期效果：当可访问性为true时，渲染“First Layout”视图并忽略“Ignored Layout”</Text>
                </View>
                <View>
                    <Text>实际效果：</Text>
                    <ScrollView accessible={true} style={styles.accessibilityContainer}>
                        <TouchableWithoutFeedback
                            style={[styles.accessibilityLayout, { backgroundColor: 'green' }]}
                            importantForAccessibility="yes">
                            <View style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}>
                                <Text>First layout</Text>
                            </View>
                        </TouchableWithoutFeedback>
                        <TouchableWithoutFeedback
                            style={[styles.accessibilityLayout, { backgroundColor: 'yellow' }]}
                            importantForAccessibility="no-hide-descendants">
                            <View style={{ flex: 1, borderColor: '#527FE4', borderWidth: 5, }}>
                                <Text>Ignored Layout</Text>
                            </View>
                        </TouchableWithoutFeedback>
                    </ScrollView>
                </View>
            </View>

        </ScrollView>
    );
}

const styles = StyleSheet.create({
    scrollViewProps: {
        width: 300,
        marginBottom: 30,
        borderWidth: 1,
        borderColor: 'black',
        borderStyle: 'solid',
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
    text1: {
        color: 'white',
        fontSize: 24,
        lineHeight: 40,
        fontWeight: 'bold',
        textAlign: 'center',
        backgroundColor: 'skyblue',
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
    accessibilityLayout: {
        width: '100%',
        height: 100,
        backgroundColor: 'lightblue',
    },
});