/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { UITurboModule } from '@rnoh/react-native-openharmony/ts';
import promptAction from '@ohos.promptAction';
import axios, { AxiosResponse, AxiosError } from '@ohos/axios';

export interface Fruit {
  name: string,
  color?: string
}

export interface VoidStruct {};

export interface EnglishNovelModel {
  content: string,
  tts: string,
  note: string,
  dateline: string,
  img: string
}

export interface ResultModel {
  code: number,
  result: EnglishNovelModel,
  msg: string
}

export class SampleTurboModule2 extends UITurboModule {
  getObject(obj: VoidStruct) {
    this.showToast(JSON.stringify(obj));
  }

  getRequest(): Promise<ResultModel> {
    return new Promise((resolve, reject) => {
      let urlStr = 'https://api.oioweb.cn/api/common/OneDayEnglish';
      axios.get<string, AxiosResponse<ResultModel>, null>(urlStr)
        .then((response: AxiosResponse<ResultModel>) => {
          let data: ResultModel = response.data;
          let novel: EnglishNovelModel = data.result;
          this.showToast(novel.note);
          resolve(data);
        })
        .catch((error: AxiosError) => {
          console.error('SampleTurboModule2.getRequest error', JSON.stringify(error));
          this.showToast(JSON.stringify(error));
          reject(error);
        });
    })
  }

  checkPwd(param: VoidStruct, success: (res: VoidStruct) => void, failure: (res: VoidStruct) => void) {
    console.log(JSON.stringify(param));
    const result: VoidStruct = {};
    const flag: boolean = true;
    if (flag) {
      this.showToast('checkPwd success!');
      success(result);
    } else {
      this.showToast('checkPwd failure xx');
      failure(result);
    }
  }

  eatFruit(fruit: Fruit) {
    this.showToast('正在吃水果，' + fruit.name + "很好吃。颜色呢？" + (fruit.color ?? "透明的"));
  }

  callFunction() {
    console.log("调用callFunction");
  }

  test() {
    this.showToast('调用 SampleTurboModule2 的 Test');
  }

  private showToast(msg: string) {
    promptAction.showToast({
      message: msg,
      duration: 2000
    });
  }
}