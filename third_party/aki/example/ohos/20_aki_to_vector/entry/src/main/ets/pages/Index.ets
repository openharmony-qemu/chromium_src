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
import libaddon, {VectorTestObject, Student } from 'libaki_to_vector.so'

@Entry
@Component
struct Index {
  @State message: string = 'Hello World Vector'

  build() {
    Row() {
      Column() {
        Text(this.message)
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
          .onClick(() => {

            let handle = new libaddon.VectorTestObject();
            let result = handle.passingAkiReturnVector();
            console.log('AKI passingAkiReturnVector: ' + JSON.stringify(result));

            let arrayInt = [1,2,3];
            let resultInt = handle.passingAkiReturnVectorInt(arrayInt);
            console.log('AKI passingAkiReturnVectorInt: ' + JSON.stringify(resultInt));

            let arrayBoolean = [false, false, false];
            let resultBool = handle.passingAkiReturnVectorBool(arrayBoolean);
            console.log('AKI passingAkiReturnVectorBool:', resultBool);

            let arrayFloat = [3.14, 3.14, 3.14];
            let resultFloat = handle.passingAkiReturnVectorFloat(arrayFloat);
            console.log('AKI passingAkiReturnVectorFloat:', resultFloat);

            let arrayString = ['abc', 'abc', 'abc'];
            let resultString = handle.passingAkiReturnVectorString(arrayString);
            console.log('AKI passingAkiReturnVectorString:', resultString);

            let CharPointer = [1, 2, 3];
            let resultCharPointer = handle.passingAkiReturnVectorCharPointer(CharPointer);
            console.log('AKI passingAkiReturnVectorCharPointer:', resultCharPointer);

            let result1 = libaddon.passingAkiReturnVectorConstCharPointer();
            console.log('AKI passingAkiReturnVectorConstCharPointer:', result1);

            let result2 = [1, 2, 3];
            let result3 = handle.passingAkiReturnMapCharBuffer(result2);
            console.log('AKI passingAkiReturnMapCharBuffer:', result3);

            let ArrayBuf = [1, 2, 3];
            let resultBuf = handle.passingAkiReturnVectorArrayBuffer(ArrayBuf);
            console.log('AKI passingAkiReturnVectorArrayBuffer:', resultBuf);

            let resultObj = handle.passingAkiValueReturnVectorObject();
            console.log('AKI passingAkiValueReturnVectorObject:', JSON.stringify(resultObj));

            let resultObj1 = handle.passingAkiReturnVectorObject(resultObj);
            console.log('AKI passingAkiReturnVectorObject:', JSON.stringify(resultObj1));

            let arrayInt1 = [1,2,3];
            let resultInt1 = libaddon.passingAkiReturnVectorInt(arrayInt1);
            console.log('AKI passingAkiReturnVectorInt:', resultInt1);

            let arrayBoolean1 = [false, false, false];
            let resultBool1 = libaddon.passingAkiReturnVectorBool(arrayBoolean1);
            console.log('AKI passingAkiReturnVectorBool:', resultBool1);

            let arrayFloat1 = [3.14, 3.14, 3.14];
            let resultFloat1 = libaddon.passingAkiReturnVectorFloat(arrayFloat1);
            console.log('AKI passingAkiReturnVectorFloat:', resultFloat1);

            let arrayString1 = ['abc', 'abc', 'abc'];
            let resultString1 = libaddon.passingAkiReturnVectorString(arrayString1);
            console.log('AKI passingAkiReturnVectorString:', resultString1);

            let CharPointer1 = [1, 2, 3];
            let resultPointer1 = libaddon.passingAkiReturnVectorCharPointer(CharPointer1);
            console.log('AKI passingAkiReturnVectorCharPointer:', resultPointer1);

            let result4 = libaddon.passingAkiReturnVectorConstCharPointer();
            console.log('AKI passingAkiReturnVectorConstCharPointer:', result4);

            let result5 = [1, 2, 3];
            let result6 = libaddon.passingAkiReturnMapCharBuffer(result5);
            console.log('AKI passingAkiReturnMapCharBuffer:', result6);

            let ArrayBuf1 = [1, 2, 3];
            let resultBuf1 = libaddon.passingAkiReturnVectorArrayBuffer(ArrayBuf1);
            console.log('AKI passingAkiReturnVectorArrayBuffer:', resultBuf1);

            let resultObj2 = handle.passingAkiValueReturnVectorObject();
            console.log('AKI passingAkiValueReturnVectorObject:', JSON.stringify(resultObj2));

            let resultObj3 = handle.passingAkiReturnVectorObject(resultObj2);
            console.log('AKI passingAkiReturnVectorObject:', JSON.stringify(resultObj3));

            let arrayStudents:Array<Student> = libaddon.passingAkiReturnClassNormal();
            let student : Student = arrayStudents[0];
            console.log('[AKI]',"Student name:" + student.GetName());
            console.log('[AKI]',"Student job:" + student.GetJob());
            console.log('[AKI]',"Student Score:" + student.Getscore());

          })
      }
      .width('100%')
    }
    .height('100%')
  }
}
