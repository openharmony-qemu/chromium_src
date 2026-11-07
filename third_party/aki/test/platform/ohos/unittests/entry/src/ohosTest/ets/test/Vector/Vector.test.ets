/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from '@ohos/hypium'
import libaki, { TestStudent } from 'libunittest.so'

export default function vectorTest() {
  describe('VectorTest', () => {
    it('001_libaki.PassingAkiReturnVector', 0, () => {
      let result = libaki.passingAkiReturnVector();
      expect(result.toString()).assertEqual([42].toString());
    })

    it('002_VectorTestObject.passingAkiReturnVector', 0, () => {
      let handle = new libaki.VectorTestObject();
      let result = handle.passingAkiReturnVector();
      expect(result.toString()).assertEqual([42].toString());
    })

    it('003_libaki.passingAkiReturnVectorInt', 0, () => {
      let array = [1, 2, 3];
      let result = libaki.passingAkiReturnVectorInt(array);
      expect(result.toString()).assertEqual([1, 2, 3, 100].toString());
    })

    it('004_VectorTestObject.passingAkiReturnVectorInt', 0, () => {
      let array = [1, 2, 3];
      let handle = new libaki.VectorTestObject();
      let result = handle.passingAkiReturnVectorInt(array);
      expect(result.toString()).assertEqual([1, 2, 3, 100].toString());
    })

    it('005_VectorTestObject.passingAkiReturnVectorBool', 0, () => {
      let arrayBoolean = [false, false, false];
      let handle = new libaki.VectorTestObject();
      let result = handle.passingAkiReturnVectorBool(arrayBoolean);
      expect(result.toString()).assertEqual([false, false, false, false].toString());
      })

    it('006_libaki.passingAkiReturnVectorBool', 0, () => {
      let arrayBoolean = [false, false, false];
      let result = libaki.passingAkiReturnVectorBool(arrayBoolean);
      expect(result.toString()).assertEqual([false, false, false, false].toString());
    })

    it('007_VectorTestObject.passingAkiReturnVectorFloat', 0, () => {
      let array = [3.14, 3.14, 3.14];
      let handle = new libaki.VectorTestObject();
      let result = handle.passingAkiReturnVectorFloat(array);
      expect(result.toString()).assertEqual([3.14, 3.14, 3.14, 3.1500000953674316].toString());
    })

    it('008_libaki.passingAkiReturnVectorFloat', 0, () => {
      let array = [3.14, 3.14, 3.14];
      let result = libaki.passingAkiReturnVectorFloat(array);
      expect(result.toString()).assertEqual([3.14, 3.14, 3.14, 3.1500000953674316].toString());
    })

    it('009_VectorTestObject.passingAkiReturnVectorString', 0, () => {
      let array = ['a', 'b', 'c'];
      let handle = new libaki.VectorTestObject();
      let result = handle.passingAkiReturnVectorString(array);
      expect(result.toString()).assertEqual(['a', 'b', 'c', '234'].toString());
    })

    it('010_libaki.passingAkiReturnVectorString', 0, () => {
      let array = ['a', 'b', 'c'];
      let result = libaki.passingAkiReturnVectorString(array);
      expect(result.toString()).assertEqual(['a', 'b', 'c', '234'].toString());
    })

    it('011_VectorTestObject.passingAkiReturnVectorCharBuffer', 0, () => {
      let array = [1, 2, 3];
      let handle = new libaki.VectorTestObject();
      let result = handle.passingAkiReturnVectorCharBuffer(array);
      expect(result.toString()).assertEqual([1, 2, 3, "123456789"].toString());
    })

    it('012_libaki.passingAkiReturnVectorCharBuffer', 0, () => {
      let array = [1, 2, 3];
      let result = libaki.passingAkiReturnVectorCharBuffer(array);
      expect(result.toString()).assertEqual([1, 2, 3, "123456789"].toString());
    })

    it('013_libaki.passingAkiReturnVectorConstCharPointer', 0, () => {
      let array = ['a', 'b', 'c'];
      let result = libaki.passingAkiReturnVectorString(array);
      expect(result.toString()).assertEqual(['a', 'b', 'c', '234'].toString());
    })

    it('014_VectorTestObject.passingAkiReturnVectorConstCharPointer', 0, () => {
      let handle = new libaki.VectorTestObject();
      let result = handle.passingAkiReturnVectorConstCharPointer();
      expect(result.toString()).assertEqual(["12356789"].toString());
    })

    it('015_VectorTestObject.passingAkiReturnVectorCharPointer', 0, () => {
      let array = [1, 2 , 3];
      let handle = new libaki.VectorTestObject();
      let result = handle.passingAkiReturnVectorCharPointer(array);
      expect(result.toString()).assertEqual([1, 2 , 3, "d"].toString());
    })

    it('016_libaki.passingAkiReturnVectorCharPointer', 0, () => {
      let array = [1, 2 , 3];
      let result = libaki.passingAkiReturnVectorCharPointer(array);
      expect(result.toString()).assertEqual([1, 2 , 3, "d"].toString());
    })

    it('017_VectorTestObject.passingAkiReturnVectorArrayBuffer', 0, () => {
      let array = [1, 2, 3];
      let handle = new libaki.VectorTestObject();
      let result = handle.passingAkiReturnVectorArrayBuffer(array);
      expect(result.toString()).assertEqual([1, 2, 3, "Element1", "Element2", "Element3"].toString());
    })

    it('018_libaki.passingAkiReturnVectorArrayBuffer', 0, () => {
      let array = [1, 2, 3];
      let result = libaki.passingAkiReturnVectorArrayBuffer(array);
      expect(result.toString()).assertEqual([1, 2, 3, "Element1", "Element2", "Element3"].toString());
    })

    it('019_VectorTestObject.passingAkiReturnVectorObject', 0, () => {
      let handle = new libaki.VectorTestObject()
      let result = handle.passingAkiValueReturnVectorObject();
      let result1 = handle.passingAkiReturnVectorObject(result);
      expect(JSON.stringify(result1)).assertEqual('[{"name":"zhangsan","age":20},{"name":"lisi","age":22}]');
    })

    it('020_VectorTestObject.passingAkiReturnClassNormal', 0, ()=> {
      let result1 = libaki.passingAkiReturnClassNormal();
      let student : TestStudent = result1[0];
      expect(student.GetName().toString()).assertEqual("Mark");
      expect(student.GetJob().toString()).assertEqual("student");
      expect(student.Getscore().toString()).assertEqual("123");
    })

    })
}
