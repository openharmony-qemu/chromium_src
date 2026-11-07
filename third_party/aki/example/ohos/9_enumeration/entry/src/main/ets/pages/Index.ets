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
import libAddon from 'libenumeration.so'

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
            console.log('AKI libAddon.TypeFlags.NONE = ' + libAddon.TypeFlags.NONE);
            console.log('AKI libAddon.TypeFlags.NUM = ' + libAddon.TypeFlags.NUM);
            console.log('AKI libAddon.TypeFlags.Passing() = ' + libAddon.Foo(libAddon.TypeFlags.STRING));

            console.log('AKI libAddon.ClassEnumFrom100.NONE = ' + libAddon.ClassEnumFrom100.NONE);
            console.log('AKI libAddon.ClassEnumFrom100.NUM = ' + libAddon.ClassEnumFrom100.NUM);
            console.log('AKI libAddon.ClassEnumFrom100.BUTT = ' + libAddon.ClassEnumFrom100.BUTT);

            console.log('AKI libAddon.ClassEnumFrom200.NONE = ' + libAddon.ClassEnumFrom200.NONE);
            console.log('AKI libAddon.ClassEnumFrom200.NUM = ' + libAddon.ClassEnumFrom200.NUM);
            console.log('AKI libAddon.ClassEnumFrom200.BUTT = ' + libAddon.ClassEnumFrom200.BUTT);

            console.log('AKI libAddon.ClassEnumFrom100.Foofrom100() = ' + libAddon.Foofrom100(libAddon.ClassEnumFrom100.NUM));
            console.log('AKI libAddon.ClassEnumFrom100.Passingfrom100() = ' + libAddon.Passingfrom100(libAddon.ClassEnumFrom100.NUM));
            console.log('AKI libAddon.ClassEnumFrom200.Foofrom200() = ' + libAddon.Foofrom200(libAddon.ClassEnumFrom200.NUM));
            console.log('AKI libAddon.ClassEnumFrom200.Passingfrom200() = ' + libAddon.Passingfrom200(libAddon.ClassEnumFrom200.NUM));
          })
      }
      .width('100%')
    }
    .height('100%')
  }
}
