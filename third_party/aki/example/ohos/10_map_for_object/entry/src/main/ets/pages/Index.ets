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
 
import hilog from '@ohos.hilog';
import libmap_for_object from 'libmap_for_object.so'
import HashMap from '@ohos.util.HashMap';
import HashSet from '@ohos.util.HashSet';

@Entry
@Component
struct Index {
  @State message: string = 'Hello World'

  build() {
    Row() {
      Column() {
        Text(this.message)
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
          .onClick(() => {
            // map
            let result = libmap_for_object.passingVoidReturnMapLong();
            console.log('[AKI]', JSON.stringify(result));
            result = libmap_for_object.passingMapLongReturnMapLong(result);
            console.log('[AKI]', JSON.stringify(result));

            let jsonObjectHandle = new libmap_for_object.JsonObjectHandle();
            result = jsonObjectHandle.passingVoidReturnMapLong();
            console.log('[AKI]', JSON.stringify(result));
            result = jsonObjectHandle.passingMapLongReturnMapLong(result);
            console.log('[AKI]', JSON.stringify(result));
            let jsMap:Map<string, number> = new Map(Object.entries(result));
            jsMap.set('Friday', 5);
            console.log(`[AKI] get key(Tuesday) value: ${jsMap.get("Tuesday")}`);
            jsMap.forEach((value, key) => {
              console.log(`[AKI] Key: ${key}, Value: ${value}`);
            });

            let longResult = libmap_for_object.passingVoidReturnMapLongString();
            console.log('[AKI]', JSON.stringify(longResult));
            longResult = libmap_for_object.passingMapLongStringReturnMapLongString(longResult);
            console.log('[AKI]', JSON.stringify(longResult));
            longResult = libmap_for_object.passingMapAkiLongStringReturnMapLongString(longResult);
            console.log('[AKI]', JSON.stringify(longResult));
            longResult = libmap_for_object.passingMapAkiStringReturnMapAkiString(longResult);
            console.log('[AKI]', JSON.stringify(longResult));
            let jsLongMap: Map<number, string> = new Map(
              (Object.entries(longResult) as Array<[string, string]>).map((entry): [number, string] => {
                const key = parseInt(entry[0], 10);
                const value = entry[1];
                return [key, value];
              })
            );
            jsLongMap.set(7, "Sunday");
            console.log(`[AKI] get key(5) value: ${jsLongMap.get(5)}`);
            jsLongMap.forEach((value, key) => {
              console.log(`[AKI] Key: ${key}, Value: ${value}`);
            });

            let jsMapArg: Map<number, string> = new Map([
              [1, "value1"],
              [3, "value3"],
              [2, "value2"]
            ]);
            jsMapArg[6] = "value6";
            jsMapArg = libmap_for_object.passingMapAkiLongStringReturnMapLongString(jsMapArg);
            console.log('[AKI]', JSON.stringify(jsMapArg));
            jsMapArg = libmap_for_object.passingMapAkiStringReturnMapAkiString(jsMapArg);
            console.log('[AKI]', JSON.stringify(jsMapArg));

            let jsMapStrArg: Map<string, string> = new Map([
              ["1", "value1"],
              ["3", "value3"],
              ["2", "value2"]
            ]);
            jsMapStrArg.set("7", "value7");
            jsMapStrArg = libmap_for_object.passingMapAkiStringStringReturnMapStringString(jsMapStrArg);
            console.log('[AKI]', JSON.stringify(jsMapStrArg));

            // HashMap
            let hashResult = libmap_for_object.passingVoidReturnHashMapLong();
            console.log('[AKI]', JSON.stringify(hashResult));
            hashResult = libmap_for_object.passingMapLongReturnHashMapLong(hashResult);
            console.log('[AKI]', JSON.stringify(hashResult));
            let jsHashMap:HashMap<string, number> = new HashMap();
            for(const entry of Object.entries(hashResult) as Array<[string, number]>) {
              const keyStr = entry[0];
              const value = entry[1];
              jsHashMap.set(keyStr, value);
            }
            jsHashMap.set('Friday', 5);
            console.log(`[AKI] HashMap get key(Tuesday) value: ${jsHashMap.get("Tuesday")}`);
            jsHashMap.forEach((value, key) => {
              console.log(`[AKI] HashMap Key: ${key}, Value: ${value}`);
            });

            let longHashResult = libmap_for_object.passingVoidReturnHashMapLongString();
            console.log('[AKI]', JSON.stringify(longHashResult));
            longHashResult = libmap_for_object.passingMapLongStringReturnHashMapLongString(longHashResult);
            console.log('[AKI]', JSON.stringify(longHashResult));
            longHashResult = libmap_for_object.passingMapAkiLongStringReturnHashMapLongString(longHashResult);
            console.log('[AKI]', JSON.stringify(longHashResult));
            longHashResult = libmap_for_object.passingMapAkiStringReturnHashMapAkiString(longHashResult);
            console.log('[AKI]', JSON.stringify(longHashResult));
            let jsLongHashMap:HashMap<number, string> = new HashMap();
            for(const entry of Object.entries(longHashResult) as Array<[string, string]>) {
              const keyStr = entry[0];
              const value = entry[1];
              const key = parseInt(keyStr, 10);
              jsLongHashMap.set(key, value);
            }
            jsLongHashMap.set(6, "Saturday");
            console.log(`[AKI] hashMap get key(5) value: ${jsLongHashMap.get(5)}`);
            jsLongHashMap.forEach((value, key) => {
              console.log(`[AKI] hashMap Key: ${key}, Value: ${value}`);
            });

            let jshashMapArg: HashMap<number, string> = new HashMap;
            jshashMapArg.set(1, "value1");
            jshashMapArg.set(3, "value3");
            jshashMapArg.set(2, "value2");
            jshashMapArg = libmap_for_object.passingMapAkiLongStringReturnHashMapLongString(jshashMapArg);
            console.log('[AKI]', JSON.stringify(jshashMapArg));
            jshashMapArg = libmap_for_object.passingMapAkiStringReturnHashMapAkiString(jshashMapArg);
            console.log('[AKI]', JSON.stringify(jshashMapArg));

            let jshashMapStrArg: HashMap<string, string> = new HashMap;
            jshashMapStrArg.set("1", "value1");
            jshashMapStrArg.set("3", "value3");
            jshashMapStrArg = libmap_for_object.passingMapAkiStringReturnHashMapStringString(jshashMapStrArg);
            console.log('[AKI]', JSON.stringify(jshashMapStrArg));

            // HashSet
            let hashSet = libmap_for_object.passingVoidReturnHashSetLong();
            console.log('[AKI]', JSON.stringify(hashSet));
            hashSet = libmap_for_object.passingSetLongReturnHashSetLong(hashSet);
            console.log('[AKI]', JSON.stringify(hashSet));
            hashSet = libmap_for_object.passingSetAkiLongReturnHashSetLong(hashSet);
            console.log('[AKI]', JSON.stringify(hashSet));
            hashSet = libmap_for_object.passingSetAkiLongReturnHashSetAkiLong(hashSet);
            console.log('[AKI]', JSON.stringify(hashSet));
            let jsHashSet:HashSet<number> = new HashSet();
            for (const value of hashSet) {
              jsHashSet.add(value);
            }
            jsHashSet.add(7);
            console.log(`[AKI] HashSet has(${jsHashSet.has(2)}) value(2)`);
            jsHashSet.forEach((value) => {
              console.log(`[AKI] HashSet value: ${value}`);
            });

            let hashSetString = libmap_for_object.passingVoidReturnHashSetString();
            console.log('[AKI]', JSON.stringify(hashSetString));
            hashSetString = libmap_for_object.passingSetStringReturnHashSetString(hashSetString);
            console.log('[AKI]', JSON.stringify(hashSetString));
            hashSetString = libmap_for_object.passingSetAkiStringReturnHashSetString(hashSetString);
            console.log('[AKI]', JSON.stringify(hashSetString));
            hashSetString = libmap_for_object.passingSetAkiStringReturnHashSetAkiString(hashSetString);
            console.log('[AKI]', JSON.stringify(hashSetString));
            let jsStringHashSet:HashSet<string> = new HashSet();
            for (const value of hashSetString) {
              jsStringHashSet.add(value);
            }
            jsStringHashSet.add("Saturday");
            console.log(`[AKI] HashSet has(${jsStringHashSet.has("Tuesday")}) value(Tuesday)`);
            jsStringHashSet.forEach((value) => {
              console.log(`[AKI] HashSet value: ${value}`);
            });

            let jsHashSetArg:HashSet<number> = new HashSet();
            jsHashSetArg.add(2);
            jsHashSetArg.add(5);
            jsHashSetArg.add(3);
            jsHashSetArg = libmap_for_object.passingSetLongReturnHashSetLong(jsHashSetArg);
            console.log('[AKI]', JSON.stringify(jsHashSetArg));

            let jsHashSetAkiArg:HashSet<string> = new HashSet();
            jsHashSetAkiArg.add("value2");
            jsHashSetAkiArg.add("value5");
            jsHashSetAkiArg.add("value3");
            jsHashSetAkiArg = libmap_for_object.passingSetAkiStringReturnHashSetAkiString(jsHashSetAkiArg);
            console.log('[AKI]', JSON.stringify(jsHashSetAkiArg));
          })
      }
      .width('100%')
    }
    .height('100%')
  }
}
