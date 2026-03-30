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
import libaki from "libunittest.so"

export default function jsNullTest() {
  describe('JSNullTest',()=> {
    it('001_passingNullReturnNull',0, ()=> {
        let value = libaki.passingNullReturnNull(null);
        expect(value).assertNull();
    })
    it('002_passingNullToJs',0, ()=> {
        libaki.passingNullToJs( (value)=> {
            expect(value).assertNull();
        })
    })
    it('003_PassingJsNullToObjectPtrConvertToNullptr',0, ()=> {
        let res = libaki.PassingJsNullToObjectPtrConvertToNullptr(null);
        expect(res).assertTrue();
    })
    it('004_ReturnNullptrToObjectPtrConvertToJsNull',0, ()=> {
        let value = libaki.ReturnNullptrToObjectPtrConvertToJsNull();
        expect(value).assertNull();
    })
    it('005_PassingJsNullToSharedPtrConvertToNullptr',0, ()=> {
      let res = libaki.PassingJsNullToSharedPtrConvertToNullptr(null);
      expect(res).assertTrue();
    })
    it('006_ReturnNullptrToSharedPtrConvertToJsNull',0, ()=> {
      let value = libaki.ReturnNullptrToSharedPtrConvertToJsNull();
      expect(value).assertNull();
    })
    it('007_PassingJsNullToReferenceConvertToNullptr',0, ()=> {
      let res = libaki.PassingJsNullToReferenceConvertToNullptr(null);
      expect(res).assertTrue();
    })
  })
}