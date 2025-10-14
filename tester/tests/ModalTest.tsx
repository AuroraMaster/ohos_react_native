/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {TestSuite} from '@rnoh/testerino';
import React, {useState} from 'react';
import {
  Modal,
  StyleSheet,
  Text,
  View,
  ModalProps,
  TextInput,
  FlatList,
  Pressable,
} from 'react-native';
import {Button, TestCase} from '../components';
import {PALETTE} from '../components/palette';

export function ModalTest() {
  return (
    <TestSuite name="Modal">
      <TestCase.Example itShould="show modal">
        <ModalExample />
      </TestCase.Example>
      <TestCase.Manual
        itShould="trigger onShow event after modal is shown"
        initialState={false}
        arrange={({setState}) => (
          <ModalExample
            onShow={() => {
              setState(true);
            }}
          />
        )}
        assert={({state, expect}) => {
          expect(state).to.be.true;
        }}
      />
      <TestCase.Example itShould="show orientation change history when Modal is open">
        <ModalExampleOrientation />
      </TestCase.Example>
      <TestCase.Manual
        itShould="show modal with slide animation"
        initialState={false}
        arrange={({setState}) => (
          <ModalExample
            animationType="slide"
            onShow={() => {
              setState(true);
            }}
          />
        )}
        assert={({state, expect}) => {
          expect(state).to.be.true;
        }}
      />
      <TestCase.Manual
        itShould="show modal with fade animation"
        initialState={false}
        arrange={({setState}) => (
          <ModalExample
            animationType="fade"
            onShow={() => {
              setState(true);
            }}
          />
        )}
        assert={({state, expect}) => {
          expect(state).to.be.true;
        }}
      />
      <TestCase.Example itShould="soft keyboard should cover modal">
        <ModalExample withTextInput />
      </TestCase.Example>
      <TestCase.Example itShould="allow to display one modal on top of another">
        <NestedModalinModal />
      </TestCase.Example>
      <TestCase.Example itShould="display modal from flatlist item">
        <NestedModalInFlatlist />
      </TestCase.Example>
      <TestCase.Example itShould="long-pressing to close the Modal will not block the next touch action.">
        <CloseModalByLongPress />
      </TestCase.Example>
    </TestSuite>
  );
}

const ModalExampleOrientation = () => {
  const [orientationHistory, setOrientationHistory] = useState<string[]>([]);
  const [modalVisible, setModalVisible] = useState(false);
  return (
    <View>
      <Text style={[styles.textStyle, {color: 'black'}]}>
        {modalVisible ? 'shown' : 'hidden'}
      </Text>
      <Modal
        transparent={true}
        visible={modalVisible}
        onRequestClose={() => {
          setModalVisible(!modalVisible);
        }}
        onOrientationChange={event => {
          const orientation = event.nativeEvent.orientation;
          setOrientationHistory(prev => [...prev, `${orientation}`]);
        }}>
        <View style={styles.centeredView}>
          <View style={styles.modalView}>
            <Text style={{marginVertical: 20}}>
              Orientation history: {JSON.stringify(orientationHistory)}
            </Text>
            <Button
              label="Hide Modal"
              onPress={() => {
                setModalVisible(false);
                setOrientationHistory([]);
              }}
            />
          </View>
        </View>
      </Modal>
      <Button label="Show Modal" onPress={() => setModalVisible(true)} />
    </View>
  );
};

const ModalExample = (props: ModalProps & {withTextInput?: boolean}) => {
  const [modalVisible, setModalVisible] = useState(false);
  return (
    <View>
      <Text style={[styles.textStyle, {color: 'black'}]}>
        {modalVisible ? 'shown' : 'hidden'}
      </Text>
      <Modal
        {...props}
        transparent={true}
        visible={modalVisible}
        onRequestClose={() => {
          setModalVisible(!modalVisible);
        }}>
        <View style={styles.centeredView}>
          <View style={styles.modalView}>
            <Text style={styles.modalText}>Hello World!</Text>
            {props.withTextInput && <TextInput style={styles.textInput} />}
            <Button label="Hide Modal" onPress={() => setModalVisible(false)} />
          </View>
        </View>
      </Modal>
      <Button label="Show Modal" onPress={() => setModalVisible(true)} />
    </View>
  );
};

const NestedModalinModal = () => {
  const [firstModalVisible, setFirstModalVisible] = useState(false);
  const [secondModalVisible, setSecondModalVisible] = useState(false);

  return (
    <View>
      <Text style={[styles.textStyle, {color: 'black'}]}>
        First: {firstModalVisible ? 'shown' : 'hidden'}
      </Text>
      <Text style={[styles.textStyle, {color: 'black'}]}>
        Second: {secondModalVisible ? 'shown' : 'hidden'}
      </Text>

      <Modal
        animationType="fade"
        transparent={true}
        visible={firstModalVisible}
        onRequestClose={() => {
          setFirstModalVisible(!firstModalVisible);
        }}>
        <View style={styles.centeredView}>
          <View style={[styles.modalView, {padding: 60}]}>
            <Text style={styles.modalText}>Hello World from first Modal!</Text>
            <Modal
              animationType="slide"
              transparent={true}
              visible={secondModalVisible}
              onRequestClose={() => {
                setSecondModalVisible(!secondModalVisible);
              }}>
              <View style={styles.centeredView}>
                <View
                  style={[styles.modalView, {backgroundColor: 'lightblue'}]}>
                  <Text style={styles.modalText}>
                    Hello World from second Modal!
                  </Text>
                  <Button
                    label="Hide Second Modal"
                    onPress={() => setSecondModalVisible(false)}
                  />
                </View>
              </View>
            </Modal>
            <Button
              label="Hide Modal"
              onPress={() => setFirstModalVisible(false)}
            />
            <Button
              label="Open another Modal"
              onPress={() => setSecondModalVisible(true)}
            />
          </View>
        </View>
      </Modal>
      <Button label="Show Modal" onPress={() => setFirstModalVisible(true)} />
    </View>
  );
};

const DATA = [
  {
    id: 'bd7acbea-c1b1-46c2-aed5-3ad53abb28ba',
    title: 'First Item',
  },
  {
    id: '3ac68afc-c605-48d3-a4f8-fbd91aa97f63',
    title: 'Second Item',
  },
  {
    id: '58694a0f-3da1-471f-bd96-145571e29d72',
    title: 'Third Item',
  },
];

type ItemProps = {title: string};

const Item = ({title}: ItemProps) => {
  const [modalVisible, setModalVisible] = useState(false);

  return (
    <View>
      <Modal
        animationType="slide"
        transparent={true}
        visible={modalVisible}
        onRequestClose={() => {
          setModalVisible(!modalVisible);
        }}>
        <View style={styles.centeredView}>
          <View style={styles.modalView}>
            <Text style={styles.modalText}>{title}</Text>
            <Button label="Hide Modal" onPress={() => setModalVisible(false)} />
          </View>
        </View>
      </Modal>
      <View style={{flexDirection: 'row', justifyContent: 'space-between'}}>
        <Text>{title}</Text>
        <Button label="Show Modal" onPress={() => setModalVisible(true)} />
      </View>
    </View>
  );
};

const NestedModalInFlatlist = () => {
  return (
    <>
      <FlatList
        data={DATA}
        renderItem={({item}) => <Item title={item.title} />}
        keyExtractor={item => item.id}
      />
    </>
  );
};

const CloseModalByLongPress = () => {
  const [modalVisible, setModalVisible] = useState(false);
  return (
    <>
      <Modal
        visible={modalVisible}
        animationType="slide"
        transparent={true}
        onRequestClose={() => {
          setModalVisible(false);
        }}>
        <View style={styles.centeredView}>
          <View style={styles.modalView}>
            <Text style={styles.modalText}>
              After the Modal is closed, click "Show Modal" again.
            </Text>
            <Pressable
              style={({pressed}) => [
                styles.longPressButton,
                {
                  backgroundColor: pressed
                    ? PALETTE.REACT_CYAN_DARK
                    : PALETTE.REACT_CYAN_LIGHT,
                },
              ]}
              onLongPress={() => setModalVisible(false)}>
              <Text>Long-pressing to close</Text>
            </Pressable>
          </View>
        </View>
      </Modal>
      <Button label="Show Modal" onPress={() => setModalVisible(true)} />
    </>
  );
};

const styles = StyleSheet.create({
  centeredView: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    marginTop: 22,
  },
  modalView: {
    margin: 20,
    backgroundColor: 'white',
    borderRadius: 20,
    padding: 35,
    alignItems: 'center',
    shadowColor: '#000',
    shadowOffset: {
      width: 0,
      height: 2,
    },
    shadowOpacity: 0.25,
    shadowRadius: 4,
    elevation: 5,
  },
  button: {
    borderRadius: 20,
    padding: 10,
    elevation: 2,
  },
  buttonOpen: {
    backgroundColor: '#F194FF',
  },
  buttonClose: {
    backgroundColor: '#2196F3',
  },
  textStyle: {
    color: 'white',
    fontWeight: 'bold',
    textAlign: 'center',
    height: 20,
  },
  modalText: {
    marginBottom: 15,
    textAlign: 'center',
    height: 20,
  },
  textInput: {
    width: 150,
    height: 40,
    backgroundColor: 'rgb(200,200,200)',
    borderRadius: 12,
    marginVertical: 4,
  },
  longPressButton: {
    paddingVertical: 6,
    paddingHorizontal: 12,
    borderWidth: 2,
    borderColor: PALETTE.REACT_CYAN_DARK,
  },
});
