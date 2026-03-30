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
import HashMap from '@ohos.util.HashMap';
import HashSet from '@ohos.util.HashSet';

export default function jsonObjectTest() {
  describe('JsonObjectTest', ()=> {
    it('001_passingVoidReturnMapLong', 0, ()=> {
      let result = libaki.passingVoidReturnMapLong();
      expect(JSON.stringify(result)).assertEqual('{"Monday":1,"Tuesday":2,"Wednesday":3}');
    })
    it('002_passingMapLongReturnMapLong', 0, ()=> {
      let result = libaki.passingMapLongReturnMapLong({
        'Monday': 1,
        'Tuesday': 2,
        'Wednesday': 3});
      expect(JSON.stringify(result)).assertEqual('{"Monday":1,"Thursday":4,"Tuesday":2,"Wednesday":3}');
    })
    it('003_JsonObjectHandle.passingVoidReturnMapLong', 0, ()=> {
      let  handle = new libaki.JsonObjectHandle();
      let result = handle.passingVoidReturnMapLong();
      expect(JSON.stringify(result)).assertEqual('{"Monday":1,"Tuesday":2,"Wednesday":3}');
    })
    it('004_JsonObjectHandle.passingMapLongReturnMapLong', 0, ()=> {
      let handle = new libaki.JsonObjectHandle();
      let result = handle.passingMapLongReturnMapLong({
        'Monday': 1,
        'Tuesday': 2,
        'Wednesday': 3});
      expect(JSON.stringify(result)).assertEqual('{"Monday":1,"Thursday":4,"Tuesday":2,"Wednesday":3}');
    })
    it('005_passingMapLongString', 0, ()=> {
      let result = libaki.passingVoidReturnMapLongString();
      expect(JSON.stringify(result)).assertEqual('{"1":"Monday","2":"Tuesday","3":"Wednesday"}');
      result = libaki.passingMapLongStringReturnMapLongString(result);
      expect(JSON.stringify(result)).assertEqual('{"1":"Monday","2":"Tuesday","3":"Wednesday","4":"Thursday"}');
      result = libaki.passingMapAkiLongStringReturnMapLongString(result);
      expect(JSON.stringify(result)).assertEqual('{"1":"Monday","2":"Tuesday","3":"Wednesday","4":"Thursday","6":"Saturday"}');
      result = libaki.passingMapAkiStringReturnMapAkiString(result);
      expect(JSON.stringify(result)).assertEqual('{"0":"value0","1":"Monday","2":"Tuesday","3":"Wednesday","4":"Thursday","6":"Saturday"}');
    })
    it('006_passingJsMapLongString', 0, ()=> {
      let jsMapArg: Map<number, string> = new Map([
        [1, "value1"],
        [3, "value3"],
        [2, "value2"]]);
      jsMapArg[6] = "value6";
      jsMapArg = libaki.passingMapAkiLongStringReturnMapLongString(jsMapArg);
      expect(JSON.stringify(jsMapArg)).assertEqual('{"1":"value1","2":"value2","3":"value3","6":"Saturday"}');
      jsMapArg = libaki.passingMapAkiStringReturnMapAkiString(jsMapArg);
      expect(JSON.stringify(jsMapArg)).assertEqual('{"0":"value0","1":"value1","2":"value2","3":"value3","6":"Saturday"}');
    })
    it('007_passingJsMapStringString', 0, ()=> {
      let jsMapStrArg: Map<string, string> = new Map([
        ["1", "value1"],
        ["3", "value3"],
        ["2", "value2"]]);
      jsMapStrArg.set("7", "value7");
      jsMapStrArg = libaki.passingMapAkiStringStringReturnMapStringString(jsMapStrArg);
      expect(JSON.stringify(jsMapStrArg)).assertEqual('{"0":"value0","1":"value1","2":"value2","3":"value3","7":"value7"}');
    })
    it('008_passingHashMapStringLong', 0, ()=> {
      let result = libaki.passingVoidReturnHashMapLong();
      expect(JSON.stringify(result)).assertEqual('{"Wednesday":3,"Tuesday":2,"Monday":1}');
      result = libaki.passingMapLongReturnHashMapLong(result);
      expect(JSON.stringify(result)).assertEqual('{"Monday":1,"Thursday":4,"Tuesday":2,"Wednesday":3}');
    })
    it('009_passingHashMapLongString', 0, ()=> {
      let result = libaki.passingVoidReturnHashMapLongString();
      expect(JSON.stringify(result)).assertEqual('{"1":"Monday","2":"Tuesday","3":"Wednesday"}');
      result = libaki.passingMapLongStringReturnHashMapLongString(result);
      expect(JSON.stringify(result)).assertEqual('{"1":"Monday","2":"Tuesday","3":"Wednesday","4":"Thursday"}');
      result = libaki.passingMapAkiLongStringReturnHashMapLongString(result);
      expect(JSON.stringify(result)).assertEqual('{"1":"Monday","2":"Tuesday","3":"Wednesday","4":"Thursday","5":"Friday"}');
      result = libaki.passingMapAkiStringReturnHashMapAkiString(result);
      expect(JSON.stringify(result)).assertEqual('{"0":"value0","1":"Monday","2":"Tuesday","3":"Wednesday","4":"Thursday","5":"Friday"}');
    })
    it('010_passingJsHashMapLongString', 0, ()=> {
      let jshashMapArg: HashMap<number, string> = new HashMap;
      jshashMapArg.set(1, "value1");
      jshashMapArg.set(3, "value3");
      jshashMapArg.set(2, "value2");
      jshashMapArg = libaki.passingMapAkiLongStringReturnHashMapLongString(jshashMapArg);
      expect(JSON.stringify(jshashMapArg)).assertEqual('{"1":"value1","2":"value2","3":"value3","5":"Friday"}');
      jshashMapArg = libaki.passingMapAkiStringReturnHashMapAkiString(jshashMapArg);
      expect(JSON.stringify(jshashMapArg)).assertEqual('{"0":"value0","1":"value1","2":"value2","3":"value3","5":"Friday"}');
    })
    it('011_passingJsHashMapStringString', 0, ()=> {
      let jshashMapStrArg: HashMap<string, string> = new HashMap;
      jshashMapStrArg.set("1", "value1");
      jshashMapStrArg.set("3", "value3");
      jshashMapStrArg = libaki.passingMapAkiStringReturnHashMapStringString(jshashMapStrArg);
      expect(JSON.stringify(jshashMapStrArg)).assertEqual('{"0":"value0","1":"value1","3":"value3"}');
    })
    it('012_passingHashSetLong', 0, ()=> {
      let result = libaki.passingVoidReturnHashSetLong();
      expect(JSON.stringify(result)).assertEqual('[3,2,1]');
      result = libaki.passingSetLongReturnHashSetLong(result);
      expect(JSON.stringify(result)).assertEqual('[4,1,2,3]');
      result = libaki.passingSetAkiLongReturnHashSetLong(result);
      expect(JSON.stringify(result)).assertEqual('[5,4,1,2,3]');
      result = libaki.passingSetAkiLongReturnHashSetAkiLong(result);
      expect(JSON.stringify(result)).assertEqual('[3,0,1,4,2,5]');
    })
    it('013_passingHashSetString', 0, ()=> {
      let result = libaki.passingVoidReturnHashSetString();
      expect(JSON.stringify(result)).assertEqual('["Wednesday","Tuesday","Monday"]');
      result = libaki.passingSetStringReturnHashSetString(result);
      expect(JSON.stringify(result)).assertEqual('["Monday","Thursday","Tuesday","Wednesday"]');
      result = libaki.passingSetAkiStringReturnHashSetString(result);
      expect(JSON.stringify(result)).assertEqual('["value8","Monday","Thursday","Tuesday","Wednesday"]');
      result = libaki.passingSetAkiStringReturnHashSetAkiString(result);
      expect(JSON.stringify(result)).assertEqual('["Friday","Wednesday","Tuesday","Thursday","Monday","value8"]');
    })
    it('014_passingJsHashSetLong', 0, ()=> {
      let jsHashSetArg:HashSet<number> = new HashSet();
      jsHashSetArg.add(2);
      jsHashSetArg.add(5);
      jsHashSetArg.add(3);
      jsHashSetArg = libaki.passingSetLongReturnHashSetLong(jsHashSetArg);
      expect(JSON.stringify(jsHashSetArg)).assertEqual('[4,5,3,2]');
    })
    it('015_passingJsHashSetString', 0, ()=> {
      let jsHashSetAkiArg:HashSet<string> = new HashSet();
      jsHashSetAkiArg.add("value2");
      jsHashSetAkiArg.add("value5");
      jsHashSetAkiArg.add("value3");
      jsHashSetAkiArg = libaki.passingSetAkiStringReturnHashSetAkiString(jsHashSetAkiArg);
      expect(JSON.stringify(jsHashSetAkiArg)).assertEqual('["value5","value3","Friday","value2"]');
    })
  })
}