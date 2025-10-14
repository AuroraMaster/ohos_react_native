/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import {PlatformColor, View, Text, Platform} from 'react-native';
import {TestSuite} from '@rnoh/testerino';
import {TestCase} from '../components';

const SYSTEM_COLORS = [
  'ohos_id_color_foreground',
  'ohos_id_color_foreground_contrary',
  'ohos_id_color_foreground_contrary_disable',
  'ohos_id_color_background',
  'ohos_id_color_sub_background',
  'ohos_id_color_emphasize',
  'ohos_id_color_emphasize_contrary',
  'ohos_id_color_warning',
  'ohos_id_color_alert',
  'ohos_id_color_handup',
  'ohos_id_color_connected',
  'ohos_id_color_component_activated',
  'ohos_id_color_component_normal',
  'ohos_id_color_list_separator',
  'ohos_id_color_subheading_separator',
  'ohos_id_color_click_effect',
  'ohos_id_color_hover',
  'ohos_id_color_focused_content_primary',
  'ohos_id_color_focused_content_second',
  'ohos_id_color_focused_content_tertiary',
  'ohos_id_color_focused_outline',
  'ohos_id_color_focused_bg',
];

const ICONS_COLORS = [
  'ohos_id_color_primary',
  'ohos_id_color_primary_contrary',
  'ohos_id_color_secondary',
  'ohos_id_color_tertiary',
  'ohos_id_color_fourth',
  'ohos_id_color_activated',
  'ohos_id_color_activated_start',
  'ohos_id_color_activated_end',
];

const TEXT_COLORS = [
  'ohos_id_color_text_primary',
  'ohos_id_color_text_primary_contrary',
  'ohos_id_color_text_secondary',
  'ohos_id_color_text_secondary_contrary',
  'ohos_id_color_text_tertiary',
  'ohos_id_color_text_tertiary_contrary',
  'ohos_id_color_text_primary_activated',
  'ohos_id_color_text_secondary_activated',
  'ohos_id_color_text_search_url',
  'ohos_id_color_text_hint',
  'ohos_id_color_text_hint_contrary',
  'ohos_id_color_text_hyperlink',
  'ohos_id_color_text_hyperlink_contrary',
  'ohos_id_color_text_highlight_bg',
  'ohos_id_color_text_highlight_bg_contrary',
];

const SYSTEM_PALETTES = [
  'ohos_id_color_palette1',
  'ohos_id_color_palette2',
  'ohos_id_color_palette3',
  'ohos_id_color_palette4',
  'ohos_id_color_palette5',
  'ohos_id_color_palette6',
  'ohos_id_color_palette7',
  'ohos_id_color_palette8',
  'ohos_id_color_palette9',
  'ohos_id_color_palette10',
  'ohos_id_color_palette11',
  'ohos_id_color_palette_aux1',
  'ohos_id_color_palette_aux2',
  'ohos_id_color_palette_aux3',
  'ohos_id_color_palette_aux4',
  'ohos_id_color_palette_aux5',
  'ohos_id_color_palette_aux6',
  'ohos_id_color_palette_aux7',
  'ohos_id_color_palette_aux8',
  'ohos_id_color_palette_aux9',
  'ohos_id_color_palette_aux10',
  'ohos_id_color_palette_aux11',
  'ohos_id_color_special1',
  'ohos_id_color_special2',
  'ohos_id_color_special3',
  'ohos_id_color_special4',
  'ohos_id_color_special5',
  'ohos_id_color_special6',
  'ohos_id_color_special7',
  'ohos_id_color_special8',
  'ohos_id_color_special9',
  'ohos_id_color_special10',
  'ohos_id_color_special11',
];

export function PlatformColorTest() {
  return (
    <TestSuite name="PlatformColor">
      <TestCase.Example
        itShould="display system colors from system resources"
        skip={{
          android: 'Works only for Open Harmony system colors',
          harmony: false,
        }}>
        <DisplayColors title="System Colors" colors={SYSTEM_COLORS} />
      </TestCase.Example>
      <TestCase.Example
        itShould="display system icons colors from system resources"
        skip={{
          android: 'Works only for Open Harmony system colors',
          harmony: false,
        }}>
        <DisplayColors title="Icons Colors" colors={ICONS_COLORS} />
      </TestCase.Example>
      <TestCase.Example
        itShould="display system text colors from system resources"
        skip={{
          android: 'Works only for Open Harmony system colors',
          harmony: false,
        }}>
        <DisplayColors title="Text Colors" colors={TEXT_COLORS} />
      </TestCase.Example>
      <TestCase.Example
        itShould="display system palette colors from system resources"
        skip={{
          android: 'Works only for Open Harmony system colors',
          harmony: false,
        }}>
        <DisplayColors title="System Palette Colors" colors={SYSTEM_PALETTES} />
      </TestCase.Example>
      <TestCase.Example
        itShould="fallback to second color and display green box"
        skip={Platform.select({
          android: 'Works only for Open Harmony system colors',
          ios: 'Works only for Open Harmony system colors',
        })}>
        <View
          style={{
            backgroundColor: PlatformColor(
              'color_id_that_doesnt_exist',
              'ohos_id_color_connected',
            ),
            height: 50,
          }}
        />
      </TestCase.Example>
    </TestSuite>
  );
}

function DisplayColors({title, colors}: {title: string; colors: string[]}) {
  return (
    <View>
      <View style={{padding: 16, borderBottomWidth: 1}}>
        <Text style={{textAlign: 'center', color: '#020403'}}>{title}</Text>
      </View>
      {colors.map(colorId => (
        <View
          key={colorId}
          style={{
            flexDirection: 'row',
            justifyContent: 'space-between',
            alignItems: 'center',
            borderBottomWidth: 1,
          }}>
          <View style={{flex: 1}}>
            <Text style={{color: '#020403'}}>{colorId}</Text>
          </View>
          <View
            style={{
              backgroundColor: PlatformColor(colorId),
              height: 50,
              width: 100,
            }}
          />
        </View>
      ))}
    </View>
  );
}
