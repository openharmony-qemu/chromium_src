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
 
import aki from 'libhello.so';

@Entry
@Component
struct Index {
  @State message: string = 'Hello Optional';

  build() {
    Row() {
      Column() {
        Text(this.message)
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
          .onClick(() => {
              console.log("ReturnOptionalInt: " + aki.ReturnOptionalInt());
              console.log("ReturnOptionalIntEmpty: " + aki.ReturnOptionalIntEmpty());

              aki.GetOptionalString("hello aki");
              aki.GetOptionalString(undefined);

              console.log("ReturnOptionalString: " + aki.ReturnOptionalString(true));
              console.log("ReturnOptionalString: " + aki.ReturnOptionalString(false));

              aki.GetOptionalObject(undefined);
              let human = new aki.Human("Ben");
              aki.GetOptionalObject(human);

              let obj = aki.ReturnOptionalObject(true);
              console.log("obj is undefined: " + obj);
              obj = aki.ReturnOptionalObject(false);
              console.log("obj is human: " + obj?.GetName());

              aki.GetOptionalAkiValue("js string by aki");
              aki.GetOptionalAkiValue(undefined);

              let value =  aki.ReturnOtionalAkiValue(true);
              console.log("aki value is undefined : " + value);
              value = aki.ReturnOtionalAkiValue(false);
              console.log("aki value is string : " + value);


          })
      }
      .width('100%')
    }
    .height('100%')
  }
}
