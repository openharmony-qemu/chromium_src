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
import libaki, {TestStudent}from "libunittest.so"

export default function mapTest() {
  describe('MapTest', ()=> {
    it('001_passingAkiVoidReturnMap', 0, ()=> {
      let result = libaki.passingAkiReturnMap();
      expect(JSON.stringify(result)).assertEqual('{"Monday":99}');
    })
    it('002_passingAkiReturnMapInt', 0, ()=> {
      let result = libaki.passingAkiReturnMapInt({'Monday': 88});
      expect(JSON.stringify(result)).assertEqual('{"Monday":88,"Thursday":99}');
    })
    it('003_passingAkiReturnMapFloat', 0, ()=> {
      let result = libaki.passingAkiReturnMapFloat({'Monday': 88.8});
      expect(JSON.stringify(result)).assertEqual('{"Monday":88.8,"Thursday":3.1500000953674316}');
    })
    it('004_passingAkiReturnMapBool', 0, ()=> {
      let result = libaki.passingAkiReturnMapBool({'Monday':false});
      expect(JSON.stringify(result)).assertEqual('{"Monday":false,"Thursday":true}');
    })
    it('005_passingAkiReturnMapString', 0, ()=> {
      let result = libaki.passingAkiReturnMapString({'Monday':'hello map'});
      expect(JSON.stringify(result)).assertEqual('{"Monday":"hello map","Thursday":"234"}');
    })
    it('006_passingAkiReturnMapCharPointer', 0, ()=> {
      let result = libaki.passingAkiReturnMapCharPointer({'Monday':'hello map'});
      expect(JSON.stringify(result)).assertEqual('{"Monday":"hello map","Thursday":"1234"}');
    })
    it('007_passingAkiReturnMapCharBuffer', 0, ()=> {
      let result = libaki.passingAkiReturnMapCharBuffer({'Monday':'hello'});
      expect(JSON.stringify(result)).assertEqual('{"Monday":"hello","Thursday":"123456789"}');
    })
    it('008_passingAkiReturnMapBuffer', 0, ()=> {
      let result1 = libaki.passingAkiValueReturnMapBuffer();
      let result = libaki.passingAkiReturnMapBuffer(result1);
      expect(JSON.stringify(result)).assertEqual('{"Monday":["Element1","Element2","Element3"],"Thursday":["Element1","Element2","Element3"]}');
    })
    it('009_passingAkiReturnMapObject', 0, ()=> {
      let result1 = libaki.passingAkiValueReturnMapObject();
      let result = libaki.passingAkiReturnMapObject(result1);
      expect(JSON.stringify(result)).assertEqual('{"Monday":{"name":"Mark","age":25},"Thursday":{"name":"Jack","age":15}}');
    })
    it('010_passingAkiReturnMapObject', 0, ()=> {
      let handle = new libaki.MapTestObject();
      let result1 = handle.passingAkiReturnMapClass();
      let student : TestStudent = result1['Monday'];
      let resultv = libaki.getAge(student);
      expect(resultv.toString()).assertEqual("25");
    })
    it('011_MapTestObject.passingAkiReturnMap', 0, ()=> {
      let handle = new libaki.MapTestObject();
      let result = handle.passingAkiReturnMap();
      expect(JSON.stringify(result)).assertEqual('{"Monday":99}');
    })
    it('012_MapTestObject.passingAkiReturnMapInt', 0, ()=> {
      let handle = new libaki.MapTestObject();
      let result = handle.passingAkiReturnMapInt({'Monday': 88});
      expect(JSON.stringify(result)).assertEqual('{"Monday":88,"Thursday":99}');
    })
    it('013_MapTestObject.passingAkiReturnMapFloat', 0, ()=> {
      let handle = new libaki.MapTestObject();
      let result = handle.passingAkiReturnMapFloat({'Monday': 88.8});
      expect(JSON.stringify(result)).assertEqual('{"Monday":88.8,"Thursday":3.1500000953674316}');
    })
    it('014_MapTestObject.passingAkiReturnMapBool', 0, ()=> {
      let handle = new libaki.MapTestObject();
      let result = handle.passingAkiReturnMapBool({'Monday':false});
      expect(JSON.stringify(result)).assertEqual('{"Monday":false,"Thursday":true}');
    })
    it('015_MapTestObject.passingAkiReturnMapString', 0, ()=> {
      let handle = new libaki.MapTestObject();
      let result = handle.passingAkiReturnMapString({'Monday':'hello map'});
      expect(JSON.stringify(result)).assertEqual('{"Monday":"hello map","Thursday":"123"}');
    })
    it('016_MapTestObject.passingAkiReturnMapCharPointer', 0, ()=> {
      let handle = new libaki.MapTestObject();
      let result = handle.passingAkiReturnMapCharPointer({'Monday':'hello map'});
      expect(JSON.stringify(result)).assertEqual('{"Monday":"hello map","Thursday":"1234"}');
    })
    it('017_MapTestObject.passingAkiReturnMapCharBuffer', 0, ()=> {
      let handle = new libaki.MapTestObject();
      let result = handle.passingAkiReturnMapCharBuffer({'Monday':'hello'});
      expect(JSON.stringify(result)).assertEqual('{"Monday":"hello","Thursday":"123456789"}');
    })
    it('018_MapTestObject.passingAkiReturnMapBuffer', 0, ()=> {
      let handle = new libaki.MapTestObject();
      let result1 = handle.passingAkiValueReturnMapBuffer();
      let result = handle.passingAkiReturnMapBuffer(result1);
      expect(JSON.stringify(result)).assertEqual('{"Monday":["Element1","Element2","Element3"],"Thursday":["Element1","Element2","Element3"]}');
    })
    it('019_MapTestObject.passingAkiReturnMapObject', 0, ()=> {
      let handle = new libaki.MapTestObject();
      let result1 = handle.passingAkiValueReturnMapObject();
      let result = handle.passingAkiReturnMapObject(result1);
      expect(JSON.stringify(result)).assertEqual('{"Monday":{"name":"Mark","age":25},"Thursday":{"name":"Jack","age":15}}');
    })
    it('020_MapTestObject.passingAkiReturnMapClass', 0, ()=> {
      let handle = new libaki.MapTestObject();
      let result1 = handle.passingAkiReturnMapClass();
      let student : TestStudent = result1['Monday'];
      expect(student.GetName().toString()).assertEqual("Mark");
      expect(student.GetJob().toString()).assertEqual("student");
      expect(student.Getscore().toString()).assertEqual("123");
    })
  })
}