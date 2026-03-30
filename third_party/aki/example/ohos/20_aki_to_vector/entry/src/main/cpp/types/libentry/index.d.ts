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

export class VectorTestObject{
    passingAkiReturnVector:() => Array<number>;
    passingAkiReturnVectorInt:(array:Array<number>) => Array<number>;
    passingAkiReturnVectorBool:(array:Array<Boolean>) => Array<Boolean>;
    passingAkiReturnVectorFloat:(array:Array<number>) => Array<number>;
    passingAkiReturnVectorString:(array:Array<String>) => Array<String>;
    passingAkiReturnMapCharBuffer: (array:Array<Number>) => Array<Number>;
    passingAkiReturnVectorCharPointer: (array:Array<Number>) => Array<Number>;
    passingAkiReturnVectorConstCharPointer:() => Array<String>;
    passingAkiReturnVectorArrayBuffer: (array:Array<Number>) => Array<Number>;
    passingAkiValueReturnVectorObject:() => Array<Object>;
    passingAkiReturnVectorObject:(array:Array<Object>) => Array<Object>;
}

export const passingAkiReturnVector:() => Array<number>;

export const passingAkiReturnVectorInt:(array:Array<number>) => Array<number>;

export const passingAkiReturnVectorBool:(array:Array<Boolean>) => Array<Boolean>;

export const passingAkiReturnVectorFloat:(array:Array<number>) => Array<number>;

export const passingAkiReturnVectorString:(array:Array<String>) => Array<String>;

export const passingAkiReturnVectorConstCharPointer:() => Array<String>;

export const passingAkiReturnVectorCharPointer: (array:Array<Number>) => Array<Number>;

export const passingAkiReturnMapCharBuffer: (array:Array<Number>) => Array<Number>;

export const passingAkiReturnVectorArrayBuffer: (array:Array<Number>) => Array<Number>;

export const passingAkiValueReturnVectorObject:() => Array<Object>;

export const passingAkiReturnVectorObject:(array:Array<Object>) => Array<Object>;

export const passingAkiReturnClassNormal:() => Array<Student>;

export const getAge: (bean: object) => number;

export const getName: (bean: object) => string;

export class Student {
    constructor(name : string, age : number);
    GetName : () => string;
    GetJob : () => string;
    Getscore : () => string;
}

