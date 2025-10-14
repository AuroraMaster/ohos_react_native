/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import axios from 'axios';
const PAGE_SIZE = 20;

const URL = "https://devecostudio-drcn.op.hicloud.com/solution/v1/getSceneMockData?scene={scene}&fileName={fileName}&pageNum={pageNum}&pageSize={pageSize}";

export interface CardData {
    thumbnails: string;
    source: string;
    width: number;
    height: number;
    type: string;
    title: string;
    title_en: string;
    user_image: string;
    vip_sign: string;
    nick_name: string;
    collections_count: string;
    url: string;
    index: number;
}

export interface Item {
    icon: string;
    width: number;
    height: number;
    name: string;
    name_en: string;
    url: string;
    index: number;
}


export class NetworkUtil {
    static async getCarData(scene: string, filename: string, pageNum: number, pageSize: number, isRefresh: boolean): Promise<CardData[]> {
        pageNum = isRefresh ? Math.floor(Math.random() * 25) + 1 : pageNum;
        let carDataURL = URL.replace("{scene}", scene)
            .replace("{fileName}", filename)
            .replace("{pageNum}", pageNum + "")
            .replace("{pageSize}", pageSize + "")
        const response = await axios.get(carDataURL);
        const newData: CardData[] = response.data.data;
        return newData;
    }

    static async getFunctionData(scene: string, filename: string, pageNum: number, pageSize: number): Promise<Item[]> {
        let functionDataURL = URL.replace("{scene}", scene)
            .replace("{fileName}", filename)
            .replace("{pageNum}", pageNum + "")
            .replace("{pageSize}", pageSize + "")
        const response = await axios.get(functionDataURL);
        const newData: Item[] = response.data.data;
        return newData;
    }
    static async testNetwork(errCallback: () => void) {
        return axios.get(URL, {
            params: { pageNum: 1, PAGE_SIZE: 10 },
            // });
            timeout: 3000, // 设置超时时间为3秒
        }).then(res => {
        }).catch(err => {
            if (err.message === 'Network Error') {
                errCallback()
            }
        })
    }
}

