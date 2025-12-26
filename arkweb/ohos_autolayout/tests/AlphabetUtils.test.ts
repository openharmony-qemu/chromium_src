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

import Utils from '../src/Alphabet/Utils';

const sourceString = 'ABCDEFG\nHIJKLMN\nOPQ\s\tRST\nUVW\s\tXYZ ';

describe('Alphabet utils', () => {
    describe('Basic Structure', () =>{
        test('Alphabet utils should have all required methods', () => {
            expect(typeof Utils.debounce).toBe('function');
            expect(typeof Utils.cleanText).toBe('function');
            expect(typeof Utils.isIncreasingAlphabet).toBe('function');
            expect(typeof Utils.isAlphabetLetter).toBe('function');
        });
    });
    describe('debounce() method', () =>{
        test('debounce method should return function, and called input function only once within the interval argument', () => {
            const consoleInspect = jest.spyOn(console, 'log');
            const intervalMicroSeconds = 500;
            const logMsg = 'debounced function called';

            jest.useFakeTimers();

            const SourceObj = class {
                logMsg = '';
                constructor(msg: string) {
                    this.logMsg = msg;
                }
                toDebouncedFunction(msg: string, context: object, args: string) {
                    console.log(msg);
                    console.log(args);
                    console.log(this === context ? 'success' : fail);
                }
                test() {
                    const debouncedFunction = Utils.debounce(this.toDebouncedFunction.bind(this), intervalMicroSeconds);
                    debouncedFunction(this.logMsg, this, 'first');
                    debouncedFunction(this.logMsg, this, 'second');
                    debouncedFunction(this.logMsg, this, 'third');
                    return debouncedFunction;
                }
            }

            const sourceObj = new SourceObj(logMsg);
            const testRet = sourceObj.test();

            expect(typeof testRet).toBe('function');

            setTimeout(() => {
                expect(consoleInspect).toHaveBeenCalledWith(logMsg);
                expect(consoleInspect).toHaveBeenCalledWith('success');
                expect(consoleInspect).toHaveBeenCalledWith('third');
                expect(consoleInspect).toHaveBeenCalledTimes(3);
            }, 1000);

            jest.runAllTimers();
        });
    });
    describe('cleanText() method', () =>{
        test('cleanText method should return string by remove \n\t\s ', () => {
            const cleanResult = Utils.cleanText(sourceString);
            expect(cleanResult).toBe('ABCDEFGHIJKLMNOPQRSTUVWXYZ');
        });
    });
    describe('isIncreasingAlphabet() method', () =>{
        test('isIncreasingAlphabet judge if a string is increasing at least some letters', () => {
            expect(Utils.isIncreasingAlphabet('ABCDEFGHIJKLMN', 10)).toBe(true);
            expect(Utils.isIncreasingAlphabet('ABCDEFGHIJKLMN', 14)).toBe(true);
            expect(Utils.isIncreasingAlphabet('ABCDEFGHIJKLMN', 15)).toBe(false);
            expect(Utils.isIncreasingAlphabet('ABCDE1FGHIJKLMN', 10)).toBe(true);
            expect(Utils.isIncreasingAlphabet('ABCDEFGHIJ2KLMN', 10)).toBe(true);
            expect(Utils.isIncreasingAlphabet('1ABCDEFGHIJKLMN', 10)).toBe(true);
            expect(Utils.isIncreasingAlphabet('ABCDEXFGHIJYKLMN', 10)).toBe(false);
            expect(Utils.isIncreasingAlphabet('ABCDEXFGHIJYKLMN', 5)).toBe(false);
        });
    });
    describe('isAlphabetLetter() method', () =>{
        test('isAlphabetLetter judge a letter is from A-Z or z-z', () => {
            expect(Utils.isAlphabetLetter('a')).toBe(true);
            expect(Utils.isAlphabetLetter('z')).toBe(true);
            expect(Utils.isAlphabetLetter('o')).toBe(true);
            expect(Utils.isAlphabetLetter('A')).toBe(true);
            expect(Utils.isAlphabetLetter('Z')).toBe(true);
            expect(Utils.isAlphabetLetter('O')).toBe(true);
            expect(Utils.isAlphabetLetter('1')).toBe(false);
            expect(Utils.isAlphabetLetter('+')).toBe(false);
        });
    });
});