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
import HashMap from '@ohos.util.HashMap';
import HashSet from '@ohos.util.HashSet';

export const passingVoidReturnMapLong: () => Map<string,number>;

export const passingMapLongReturnMapLong: (map:Map<string,number>) => Map<string,number>; 

export const passingVoidReturnMapLongString: () => Map<number,string>;

export const passingMapLongStringReturnMapLongString: (map:Map<number,string>) => Map<number,string>;

export const passingMapAkiLongStringReturnMapLongString: (map:Map<number, string>) => Map<number, string>;

export const passingMapAkiStringStringReturnMapStringString: (map:Map<string, string>) => Map<string, string>;

export const passingMapAkiStringReturnMapAkiString: (map:Map<number,string>) => Map<number,string>;

export const passingVoidReturnHashMapLong: () => HashMap<string,number>;

export const passingMapLongReturnHashMapLong: (hashMap:HashMap<string,number>) => HashMap<string,number>;

export const passingVoidReturnHashMapLongString: () => HashMap<number,string>;

export const passingMapLongStringReturnHashMapLongString: (hashMap:HashMap<number,string>) => HashMap<number,string>;

export const passingMapAkiLongStringReturnHashMapLongString: (hashMap:HashMap<number, string>) => HashMap<number, string>;

export const passingMapAkiStringReturnHashMapStringString: (hashMap:HashMap<string, string>) => HashMap<string, string>;

export const passingMapAkiStringReturnHashMapAkiString: (hashMap:HashMap<number,string>) => HashMap<number,string>;

export const passingVoidReturnHashSetLong: () => HashSet<number>;

export const passingSetLongReturnHashSetLong: (hashSet:HashSet<number>) => HashSet<number>;

export const passingSetAkiLongReturnHashSetLong: (hashSet:HashSet<number>) => HashSet<number>;

export const passingSetAkiLongReturnHashSetAkiLong: (hashSet:HashSet<number>) => HashSet<number>;

export const passingVoidReturnHashSetString: () => HashSet<string>;

export const passingSetStringReturnHashSetString: (hashSet:HashSet<string>) => HashSet<string>;

export const passingSetAkiStringReturnHashSetString: (hashSet:HashSet<string>) => HashSet<string>;

export const passingSetAkiStringReturnHashSetAkiString: (hashSet:HashSet<string>) => HashSet<string>;

export class JsonObjectHandle{

     passingVoidReturnMapLong: () => Map<string,number>;

     passingMapLongReturnMapLong: (map:Map<string,number>) => Map<string,number>; 
     
}