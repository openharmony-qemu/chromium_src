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

import napi_call_function_exception from 'libnapi_call_function_exception.so';

class TestClass {
  TestException(isException: object) {
    console.log('TestException start');
    if (isException) {
      mightThrowAnError()
    }
    console.log('TestException end');
  }
}

@Entry
@Component
struct Index {
  @State message1: string = 'call exception'
  @State message2: string = 'callMethod exception'

  build() {
    Row() {
      Column() {
        Text(this.message1)
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
          .onClick(() => {
            napi_call_function_exception.CallException(() => {
              console.info("napi_call_function CallException");
              mightThrowAnError();
            })
          })
        Text(this.message2)
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
          .onClick(() => {
            console.info("napi_call_function CallMethodException");
            let test = new TestClass();
            napi_call_function_exception.CallMethodException(test);
          })
      }
      .width('100%')
    }
    .height('100%')
  }
}

// 使用自定义异常
function mightThrowAnError(): void {
  throw new MyCustomError('Cannot read property length of undefined');
}

class MyCustomError extends Error {
  constructor(message: string) {
    super(message);
    this.name = 'TypeError';
  }
}
