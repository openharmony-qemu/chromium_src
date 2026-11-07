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

import libmap_for_object, {JsonObjectHandle ,Student} from 'libaki_map.so'
import bundleManager from '@ohos.bundle.bundleManager';

@Entry
@Component
struct Index {
  @State message: string = 'Hello Map'

  build() {
    Row() {
      Column() {
        Text(this.message)
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
          .onClick(() => {
            let testInt = new Map([['Monday', 99]]);
            let result = libmap_for_object.passingAkiReturnMap();
            console.log('[AKI] passingAkiReturnMap:', JSON.stringify(result));
			      result = libmap_for_object.passingAkiReturnMapInt(testInt);
            console.log('[AKI] passingAkiReturnMapInt:', JSON.stringify(result));
			      result = libmap_for_object.passingAkiReturnMapFloat(testInt);
            console.log('[AKI] passingAkiReturnMapFloat:', JSON.stringify(result));
            let testBoolean = new Map([['Monday', false]]);
			      let resultb = libmap_for_object.passingAkiReturnMapBool(testBoolean);
            console.log('[AKI] passingAkiReturnMapBool:', JSON.stringify(resultb));
            let testStr = new Map([['Monday', 'abc']]);
            let results = libmap_for_object.passingAkiReturnMapString(testStr);
            console.log('[AKI] passingAkiReturnMapString:', JSON.stringify(results));
            let testStr1 = new Map([['Monday', 'abcd']]);
            let resultc = libmap_for_object.passingAkiReturnMapCharPointer(testStr1);
            console.log('[AKI] passingAkiReturnMapCharPointer:', JSON.stringify(resultc));
            let resulta = libmap_for_object.passingAkiValueReturnMapBuffer();
            console.log('[AKI] passingAkiValueReturnMapBuffer:', JSON.stringify(resulta));
            resulta = libmap_for_object.passingAkiReturnMapBuffer(resulta);
            console.log('[AKI] passingAkiReturnMapBuffer:', JSON.stringify(resulta));
            let resulto = libmap_for_object.passingAkiValueReturnMapObject();
            console.log('[AKI] passingAkiValueReturnMapObject:', JSON.stringify(resulto));
            resulto = libmap_for_object.passingAkiReturnMapObject(resulto);
            console.log('[AKI] passingAkiReturnMapObject:', JSON.stringify(resulto));

            let jsonObjectHandle = new libmap_for_object.JsonObjectHandle();
            let result1 = jsonObjectHandle.passingAkiReturnMap();
            console.log('[AKI] passingAkiReturnMap:', JSON.stringify(result1))
            let resultString = jsonObjectHandle.passingAkiReturnCharBuff();
            console.log('[AKI] passingAkiReturnCharBuff:', JSON.stringify(resultString));
            result1 = jsonObjectHandle.passingAkiReturnMapInt(result1);
            console.log('[AKI] passingAkiReturnMapInt:', JSON.stringify(result1));
            result1 = jsonObjectHandle.passingAkiReturnMapFloat(result1);
            console.log('[AKI] passingAkiReturnMapFloat:', JSON.stringify(result1));
            resultb = jsonObjectHandle.passingAkiReturnMapBool(resultb);
            console.log('[AKI] passingAkiReturnMapBool:', JSON.stringify(resultb));
            results = jsonObjectHandle.passingAkiReturnMapString(testStr);
            console.log('[AKI] passingAkiReturnMapString:', JSON.stringify(results));
            resultc = jsonObjectHandle.passingAkiReturnMapCharPointer(testStr1);
            console.log('[AKI] passingAkiReturnMapCharPointer:', JSON.stringify(resultc));
            resulta = jsonObjectHandle.passingAkiValueReturnMapBuffer();
            console.log('[AKI] passingAkiValueReturnMapBuffer:', JSON.stringify(resulta));
            resulta = jsonObjectHandle.passingAkiReturnMapBuffer(resulta);
            console.log('[AKI] passingAkiReturnMapBuffer:', JSON.stringify(resulta));
            resulto = jsonObjectHandle.passingAkiValueReturnMapObject();
            console.log('[AKI] passingAkiValueReturnMapObject:', JSON.stringify(resulto));
            resulto = jsonObjectHandle.passingAkiReturnMapObject(resulto);
            console.log('[AKI] passingAkiReturnMapObject:', JSON.stringify(resulto));

            let resultclass = jsonObjectHandle.passingAkiReturnMapClassNormal();
            let student : Student = resultclass['Monday'];
            console.log('[AKI]',"Student name:" + student.GetName());
            console.log('[AKI]',"Student job:" + student.GetJob());
            console.log('[AKI]',"Student Score:" + student.Getscore());

            let resultv = libmap_for_object.getAge(student);
            console.log('[AKI]',"Student Age:" + resultv);
          })
      }
      .width('100%')
    }
    .height('100%')
  }
}