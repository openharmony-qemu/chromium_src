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

import AlphabetMicroDots from '../src/Alphabet/AlphabetMicroDots';
import CONF from '../src/Alphabet/Conf';

const mockRectVal = {
    left: 300,
    right: 320,
    top: 100,
    bottom: 400,
    width: 20,
    height: 300,
    x: 300,
    y: 100,
    toJSON() {
        return {
            left: 300,
            right: 320,
            top: 100,
            bottom: 400,
            width: 20,
            height: 300,
            x: 300,
            y: 100,
        };
    },
};

describe('AlphabetMicroDots', () => {
    describe('Basic Structure', () => {
        test('AlphabetMicroDots instance', () => {
            const alphabetMicroDots = new AlphabetMicroDots();
            expect(typeof alphabetMicroDots.adjustAlphabet).toBe('function');
            expect(typeof alphabetMicroDots.showItem).toBe('function');
            expect(typeof alphabetMicroDots.hideItem).toBe('function');
            expect(typeof alphabetMicroDots.reset).toBe('function');
            expect(typeof alphabetMicroDots.resetHiddenElements).toBe('function');
            expect(typeof alphabetMicroDots.resetDottedElements).toBe('function');
            expect(typeof alphabetMicroDots.getTextNode).toBe('function');
            expect(typeof alphabetMicroDots.calculateNumOfLettersToRemove).toBe('function');
            expect(typeof alphabetMicroDots.distributeInteger).toBe('function');
            expect(typeof alphabetMicroDots.solveStringCut).toBe('function');
        });
    });
    describe('constructor() function', () => {
        test('constructor function should init distributeWeightType', () => {
            const alphabetMicroDots1 = new AlphabetMicroDots();
            expect(alphabetMicroDots1.distributeWeightType).toBe(CONF.MICRO_DOTS.DISTRIBUTE_WEIGHT_TYPE_DEFAULT);

            const alphabetMicroDots2 = new AlphabetMicroDots('start');
            expect(alphabetMicroDots2.distributeWeightType).toBe('start');

            const alphabetMicroDots3 = new AlphabetMicroDots('middle');
            expect(alphabetMicroDots3.distributeWeightType).toBe(CONF.MICRO_DOTS.DISTRIBUTE_WEIGHT_TYPE_DEFAULT);
        });
    });
    describe('showItem() function', () => {
        test('showItem function change item element to its default display', () => {
            const alphabetMicroDots = new AlphabetMicroDots();
            alphabetMicroDots.alphabetItemDefaultDisplayStyle = 'block';
            const item = document.createElement('div');
            alphabetMicroDots.showItem(item, false);
            expect(item.style.display).toBe('block');
            alphabetMicroDots.showItem(item, true);
            expect(item.innerText).toBe(CONF.MICRO_DOTS.MICRO_DISPLAY_TEXT);
            alphabetMicroDots.hideItem(item);
            expect(item.style.display).toBe('none');
        });
    });
    describe('reset() function', () => {
        test('reset function will return alphabet element to its original', () => {
            const alphabetMicroDots = new AlphabetMicroDots();
            const alphabet = document.createElement('div');
            const dottedElement = document.createElement('div');
            const hiddenElement = document.createElement('div');

            const originalText = 'C';

            dottedElement.innerText = originalText;

            alphabet.appendChild(dottedElement);
            alphabet.appendChild(hiddenElement);

            alphabetMicroDots.hiddenElements.push(hiddenElement);
            alphabetMicroDots.dottedElements.push(dottedElement);

            alphabetMicroDots.showItem(dottedElement, true);

            expect(dottedElement.dataset.originTag).toBe(originalText);
            expect(dottedElement.innerText).toBe(CONF.MICRO_DOTS.MICRO_DISPLAY_TEXT);

            alphabetMicroDots.reset();

            expect(alphabetMicroDots.hiddenElements.length).toBe(0);
            expect(alphabetMicroDots.dottedElements.length).toBe(0);
            expect(dottedElement.innerText).toBe(originalText);
        });
    });
    describe('distributeInteger() function', () => {
        test('buckets equal 0 will return empty array', () => {
            const alphabetMicroDots = new AlphabetMicroDots();
            const result = alphabetMicroDots.distributeInteger(5, 0);
            expect(result.length).toBe(0);
        });
        test('total less than buckets will fill from first', () => {
            const alphabetMicroDots = new AlphabetMicroDots();
            const result = alphabetMicroDots.distributeInteger(3, 5);
            expect(result.length).toBe(5);
            expect(result[0]).toBe(1);
            expect(result[1]).toBe(1);
            expect(result[2]).toBe(1);
            expect(result[3]).toBe(0);
            expect(result[4]).toBe(0);
        });
        test('distributeWeightType will affect result', () => {
            const alphabetMicroDots = new AlphabetMicroDots();
            const result = alphabetMicroDots.distributeInteger(10, 4);
            expect(result.length).toBe(4);
            expect(result[0]).toBe(2);
            expect(result[1]).toBe(4);
            expect(result[2]).toBe(2);
            expect(result[3]).toBe(2);
            alphabetMicroDots.distributeWeightType = 'start';
            const resultStart = alphabetMicroDots.distributeInteger(10, 4);
            expect(resultStart[0]).toBe(3);
            expect(resultStart[1]).toBe(3);
            expect(resultStart[2]).toBe(2);
            expect(resultStart[3]).toBe(2);
            alphabetMicroDots.distributeWeightType = 'end';
            const resultEnd = alphabetMicroDots.distributeInteger(10, 4);
            expect(resultEnd[0]).toBe(2);
            expect(resultEnd[1]).toBe(2);
            expect(resultEnd[2]).toBe(3);
            expect(resultEnd[3]).toBe(3);
        });
    });
    describe('solveStringCut() function', () => {
        test('length short than 2 will not process', () => {
            const alphabetMicroDots = new AlphabetMicroDots();
            const result = alphabetMicroDots.solveStringCut('A', 1);
            expect(result.result).toBe('A');
        });
        test('remove count equal 0 will not process', () => {
            const alphabetMicroDots = new AlphabetMicroDots();
            const result = alphabetMicroDots.solveStringCut('ABCDEFG', 0);
            expect(result.result).toBe('ABCDEFG');
        });
        test('solveStringCut should return a relative ideal cut result', () => {
            const alphabetMicroDots = new AlphabetMicroDots();
            const result1 = alphabetMicroDots.solveStringCut('ABCDEFGHIJKLMN', 1);
            expect(result1.result).toBe('ABCDEF|IJKLMN');
            const result2 = alphabetMicroDots.solveStringCut('ABCDEFGHIJKLMN', 2);
            expect(result2.result).toBe('ABC|FGHI|LMN');
            const result3 = alphabetMicroDots.solveStringCut('ABCDEFGHIJKLMN', 3);
            expect(result3.result).toBe('AB|EF|IJ|MN');
            const result4 = alphabetMicroDots.solveStringCut('ABCDEFGHIJKLMN', 4);
            expect(result4.result).toBe('A|D|GH|K|N');
        });
    });
    describe('getTextNode() function', () => {
        test('getTextNode should return the first text node element or itself', () => {
            const alphabetMicroDots = new AlphabetMicroDots();
            const element1 = document.createElement('div');
            expect(alphabetMicroDots.getTextNode(element1)).toBe(element1);
            const span = document.createElement('span');
            span.innerText = 'test';
            element1.appendChild(span);
            expect(alphabetMicroDots.getTextNode(element1) === element1).toBe(false);
        });
    });
    describe('adjustAlphabet() function', () => {
        test('adjustAlphabet should change the dom display behave by cut result', () => {
            const alphabetMicroDots = new AlphabetMicroDots();
            const alphabetStr = 'ABCDEFGHIJKLMN';
            const alphabetDom = document.createElement('div');
            const childElements = [];

            for (let i = 0; i < alphabetStr.length; i++) {
                let item = document.createElement('div');
                item.innerHTML = alphabetStr[i];
                alphabetDom.appendChild(item);
                childElements.push(item);
            }

            const result = alphabetMicroDots.solveStringCut(alphabetStr, 2);
            const toKeep = result.result;

            alphabetMicroDots.adjustAlphabet(toKeep, alphabetDom);

            expect(childElements[0].innerHTML).toBe('A');
            expect(childElements[0].style.display).toBe('block');
            expect(childElements[3].innerHTML).toBe('.');
            expect(childElements[4].style.display).toBe('none');
            expect(childElements[9].innerHTML).toBe('.');
        });
    });
    describe('calculateNumOfLettersToRemove() function', () => {
        test('will not remove if visibleHeight not less then scrollHeight', () => {
            const alphabetMicroDots = new AlphabetMicroDots();
            Object.defineProperty(window, 'innerHeight', {
                writable: true,
                configurable: true,
                value: 600
            });
            const alphabetDom = document.createElement('div');
            jest.spyOn(alphabetDom, 'getBoundingClientRect').mockReturnValue(mockRectVal);
            Object.defineProperty(alphabetDom, 'scrollHeight', {
                writable: true,
                configurable: true,
                value: 400
            });
            Object.defineProperty(alphabetDom, 'clientHeight', {
                writable: true,
                configurable: true,
                value: 400
            });
            const result = alphabetMicroDots.calculateNumOfLettersToRemove(alphabetDom);
            expect(result.removeCount).toBe(0);
        });
        test('alphabetMicroDots will calculate remove number', () => {
            const alphabetMicroDots = new AlphabetMicroDots();
            Object.defineProperty(window, 'innerHeight', {
                writable: true,
                configurable: true,
                value: 300
            });
            const alphabetDom = document.createElement('div');
            jest.spyOn(alphabetDom, 'getBoundingClientRect').mockReturnValue(mockRectVal);
            Object.defineProperty(alphabetDom, 'scrollHeight', {
                writable: true,
                configurable: true,
                value: 400
            });
            Object.defineProperty(alphabetDom, 'clientHeight', {
                writable: true,
                configurable: true,
                value: 400
            });

            const alphabetStr = 'ABCDEFGHIJKLMNOPQRST';
            const childElements = [];

            for (let i = 0; i < alphabetStr.length; i++) {
                let item = document.createElement('div');
                item.style.height = '20px';
                item.innerHTML = alphabetStr[i];
                alphabetDom.appendChild(item);
                childElements.push(item);
            }

            const result = alphabetMicroDots.calculateNumOfLettersToRemove(alphabetDom);
            expect(result.removeCount).toBe(15);
        });
    });
});