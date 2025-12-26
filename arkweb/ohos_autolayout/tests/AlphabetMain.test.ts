/*
  Copyright (c) 2025 Huawei Device Co., Ltd.
  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

import AlphabetMain from '../src/Alphabet/Main';

describe('Alphabet Main', () => {
    describe('Basic Structure', () =>{
        test('Alphabet main should have all required methods ', () => {
            expect(typeof AlphabetMain.start).toBe('function');
        });
    });
    describe('start() method', () =>{
        test('start method should judge input config and return void', () => {
            const consoleInspect = jest.spyOn(console, 'log');
            const startResult = AlphabetMain.start('invalid config');

            expect(consoleInspect).toHaveBeenCalledWith('New AlphabetAdapter');
            expect(startResult).toBe(undefined);

            AlphabetMain.start('invalid config');
            expect(consoleInspect).toHaveBeenCalledWith('Reset AlphabetAdapter');
        })
    });
});