/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

export class Student {
    constructor(name : string, age : number);
    GetName : () => string;
    GetJob : () => string;
    Getscore : () => string;
}

export class JsonObjectHandle{
    passingAkiReturnMap: () => Map<string,number>;
    passingAkiValueReturnMapBuffer: () => Map<string,Array<string>>;
    passingAkiValueReturnMapObject: () => Map<string,Object>;
    passingAkiReturnMapClassNormal: ()=> Map<string,Student>;
    passingAkiReturnCharBuff: ()=> Map<string,string>;
    passingAkiReturnMapInt: (map:Map<string,number>) => Map<string,number>; 	 
    passingAkiReturnMapFloat: (map:Map<string,number>) => Map<string,number>;
    passingAkiReturnMapBool: (map:Map<string,boolean>) => Map<string,boolean>;
    passingAkiReturnMapString: (map:Map<string,string>) => Map<string,string>; 	
    passingAkiReturnMapCharPointer: (map:Map<string,string>) => Map<string,string>;
    passingAkiReturnMapBuffer: (map:Map<string,Array<string>>) => Map<string,Array<string>>;
    passingAkiReturnMapObject: (map:Map<string,Object>) => Map<string,Object>;
}

export const passingAkiReturnMap: () => Map<string,number>;
export const passingAkiValueReturnMapBuffer: () => Map<string,Array<string>>;
export const passingAkiValueReturnMapObject: () => Map<string,Object>;
export const passingAkiReturnMapInt: (map:Map<string,number>) => Map<string,number>; 
export const passingAkiReturnMapFloat: (map:Map<string,number>) => Map<string,number>; 
export const passingAkiReturnMapBool: (map:Map<string,boolean>) => Map<string,boolean>; 
export const passingAkiReturnMapString: (map:Map<string,string>) => Map<string,string>; 
export const passingAkiReturnMapCharPointer: (map:Map<string,string>) => Map<string,string>; 
export const passingAkiReturnMapBuffer: (map:Map<string,Array<string>>) => Map<string,Array<string>>;
export const passingAkiReturnMapObject: (map:Map<string,Object>) => Map<string,Object>;
export const getAge: (bean: object) => number;
