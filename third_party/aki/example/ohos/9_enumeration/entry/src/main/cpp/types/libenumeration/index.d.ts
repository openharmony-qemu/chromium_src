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

export enum TypeFlags {
      NONE,
      NUM,
      STRING
}

export enum ClassEnumFrom100 {
      NONE,
      NUM,
      STRING,
      BUTT
}

export enum ClassEnumFrom200 {
      NONE,
      NUM,
      STRING,
      BUTT
}

export const Foo: (flag: TypeFlags) => TypeFlags;
export const Foofrom100: (flag: ClassEnumFrom100) => ClassEnumFrom100;
export const Passingfrom100: (flag: ClassEnumFrom100) => ClassEnumFrom100;
export const Foofrom200: (flag: ClassEnumFrom200) => ClassEnumFrom200;
export const Passingfrom200: (flag: ClassEnumFrom200) => ClassEnumFrom200;