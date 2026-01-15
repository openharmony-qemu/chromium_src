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

import AlphabetAdapter from '../src/Alphabet/AlphabetAdapter';
import AlphabetButtonGroup from '../src/Alphabet/AlphabetButtonGroup';
import CONF from '../src/Alphabet/Conf';

interface AlphabetConfig {
    alphabetHeightWidthMinRatio: number,
    alphabetIdentificationMinSize: number,
}

const wholeAlphabetStr = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ';
const invalidJSONStr: string = 'invalid json';
const configJSON: AlphabetConfig = {
    alphabetHeightWidthMinRatio: 9,
    alphabetIdentificationMinSize: 15,
};
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

describe('AlphabetAdapter', () => {
    beforeAll(() => {
        Object.defineProperty(HTMLElement.prototype, 'checkVisibility', {
            configurable: true,
            value: jest.fn(() => true),
        })
    });
    describe('Basic Structure', () => {
        test('AlphabetAdapter instance', () => {
            const alphabetAdapter = new AlphabetAdapter(invalidJSONStr);
            expect(typeof alphabetAdapter.likelyAnAlphabet).toBe('function');
            expect(typeof alphabetAdapter.traverseElements).toBe('function');
            expect(typeof alphabetAdapter.scrollBoxAdapt).toBe('function');
            expect(typeof alphabetAdapter.updateAlphabetButtonGroupVersion).toBe('function');
            expect(typeof alphabetAdapter.update).toBe('function');
            expect(typeof alphabetAdapter.reset).toBe('function');
            expect(typeof alphabetAdapter.observeAction).toBe('function');
            expect(typeof alphabetAdapter.observeDocumentChange).toBe('function');
            expect(typeof alphabetAdapter.observeResize).toBe('function');
            expect(typeof alphabetAdapter.run).toBe('function');
            expect(typeof alphabetAdapter.setConfig).toBe('function');
        });
    });
    describe('constructor() method', () => {
        test('constructor method should execute run when loading is over', () => {
            const consoleInspect = jest.spyOn(console, 'log');
            Object.defineProperty(document, 'readyState', {
                get() {
                    return 'loading';
                },
                configurable: true,
            });

            const alphabetAdapter = new AlphabetAdapter(invalidJSONStr);
            expect(consoleInspect).toHaveBeenCalledWith('document on loading');

            window.document.dispatchEvent(new Event('DOMContentLoaded', {
                bubbles: true,
                cancelable: true,
            }));

            jest.useFakeTimers();
            setTimeout(() => {
                expect(consoleInspect).toHaveBeenCalledWith('Alphabet run');
            });
            jest.runAllTimers();
        });
    });
    describe('setConfig() method', () => {
        test('setConfig method should take config when config is valid', () => {
            const alphabetAdapter1 = new AlphabetAdapter(invalidJSONStr);
            expect(alphabetAdapter1.heightWidthMinRatio).toBe(CONF.BASE.HEIGHT_WIDTH_MIN_RATIO_DEFAULT);
            expect(alphabetAdapter1.identificationMinSize).toBe(CONF.BASE.IDENTIFICATION_MIN_SIZE);

            const alphabetAdapter2 = new AlphabetAdapter(JSON.stringify(configJSON));
            expect(alphabetAdapter2.heightWidthMinRatio).toBe(configJSON.alphabetHeightWidthMinRatio);
            expect(alphabetAdapter2.identificationMinSize).toBe(configJSON.alphabetIdentificationMinSize);

            const configJSON3: AlphabetConfig = {
                alphabetHeightWidthMinRatio: -5,
                alphabetIdentificationMinSize: -26
            };

            const alphabetAdapter3 = new AlphabetAdapter(JSON.stringify(configJSON3));
            expect(alphabetAdapter3.heightWidthMinRatio).toBe(CONF.BASE.HEIGHT_WIDTH_MIN_RATIO_DEFAULT);
            expect(alphabetAdapter3.identificationMinSize).toBe(CONF.BASE.IDENTIFICATION_MIN_SIZE);

            const configJSON4: AlphabetConfig = {
                alphabetHeightWidthMinRatio: 31,
                alphabetIdentificationMinSize: 28
            };
            const alphabetAdapter4 = new AlphabetAdapter(JSON.stringify(configJSON4));
            expect(alphabetAdapter4.heightWidthMinRatio).toBe(CONF.BASE.HEIGHT_WIDTH_MIN_RATIO_CONFIG_MAX);
            expect(alphabetAdapter4.identificationMinSize).toBe(CONF.BASE.IDENTIFICATION_MIN_SIZE_CONFIG_MAX);
        });
    });
    describe('observeResize() method', () => {
        test('observeResize method add listener to window resize', () => {
            const consoleInspect = jest.spyOn(console, 'log');
            new AlphabetAdapter(JSON.stringify(configJSON));
            global.dispatchEvent(new Event('resize'));
            expect(consoleInspect).toHaveBeenCalledWith('Alphabet reset');
        });
    });
    describe('observeAction() method', () => {
        test('observeAction method will be called when document tree changed', () => {
            const consoleInspect = jest.spyOn(console, 'log');
            const alphabetAdapter = new AlphabetAdapter(JSON.stringify(configJSON));
            alphabetAdapter.observeAction();

            jest.useFakeTimers();
            setTimeout(() => {
                expect(consoleInspect).toHaveBeenCalledWith('observeAction');
            }, 1000);
            jest.runAllTimers();
        });
    });
    describe('likelyAnAlphabet() method', () => {
        test('likelyAnAlphabet method return true if the input string like alphabet', () => {
            const alphabetAdapter = new AlphabetAdapter(JSON.stringify(configJSON));
            expect(alphabetAdapter.likelyAnAlphabet('ABCDEFGHIJKLMNOPQ')).toBe(true);
            expect(alphabetAdapter.likelyAnAlphabet('abcdefghijklmnopq')).toBe(false);
            expect(alphabetAdapter.likelyAnAlphabet('ABCDEFGHIJKLMN')).toBe(false);
            expect(alphabetAdapter.likelyAnAlphabet('123ABCDEFGHIJKLMNOPQ')).toBe(true);
            expect(alphabetAdapter.likelyAnAlphabet('ABCDEFGHIJKLMNO123PQ')).toBe(true);
        });
    });
    describe('reset() method', () => {
        test('reset method will be called when resize and body dom changed', () => {
            const alphabetAdapter = new AlphabetAdapter(JSON.stringify(configJSON));
            alphabetAdapter.scrollAdaptOriginHeight = '400px';
            const alphabetDom = document.createElement('div');
            alphabetAdapter.alphabetDom = alphabetDom;
            alphabetAdapter.reset();
            expect(alphabetAdapter.buttonGroup).toBe(null);
            expect(alphabetAdapter.adaptType).toBe(CONF.BUTTON_GROUP.TYPE);
            expect(alphabetAdapter.scrollAdaptOriginHeight).toBe('');
            expect(alphabetDom.style.height).toBe('400px');

            alphabetAdapter.buttonGroup = new AlphabetButtonGroup();
            alphabetAdapter.reset();
            expect(alphabetAdapter.buttonGroup.alphabet).toBe(null);
        });
    });
    describe('traverseElements() method', () => {
        test('traverseElements', () => {
            const alphabetAdapter = new AlphabetAdapter(JSON.stringify(configJSON));

            Object.defineProperty(window, 'innerWidth', {
                writable: true,
                configurable: true,
                value: 375,
            });

            const traverseResult = alphabetAdapter.traverseElements(document.body);
            expect(traverseResult).toBe(null);

            const str1 = wholeAlphabetStr.substring(0, configJSON.alphabetIdentificationMinSize - 1);
            const alphabetDom1 = document.createElement('ul');
            for(let i = 0; i < str1.length; i++) {
                let liItem = document.createElement('li');
                liItem.innerHTML = str1[i];
                alphabetDom1.appendChild(liItem);
            }
            const traverseResult1 = alphabetAdapter.traverseElements(alphabetDom1);
            expect(traverseResult1).toBe(null);

            const str2 = wholeAlphabetStr.substring(0, configJSON.alphabetIdentificationMinSize);
            const alphabetDom2 = document.createElement('ul');

            alphabetDom2.getBoundingClientRect = undefined;

            for(let j = 0; j < str2.length; j++) {
                let liItem = document.createElement('li');
                liItem.innerHTML = str2[j];
                alphabetDom2.appendChild(liItem);
            }
            const traverseResult2 = alphabetAdapter.traverseElements(alphabetDom2);
            expect(traverseResult2).toBe(null);

            const str3 = wholeAlphabetStr.substring(0, configJSON.alphabetIdentificationMinSize + 1);
            const alphabetDomOuter = document.createElement('div');
            const alphabetDom3 = document.createElement('ul');

            jest.spyOn(alphabetDom3, 'getBoundingClientRect').mockReturnValue(mockRectVal);

            for(let k = 0; k < str3.length; k++) {
                let liItem = document.createElement('li');
                liItem.innerHTML = str3[k];
                alphabetDom3.appendChild(liItem);
            }
            alphabetDomOuter.appendChild(alphabetDom3);
            const traverseResult3 = alphabetAdapter.traverseElements(alphabetDomOuter);
            expect(traverseResult3).toBe(alphabetDom3);

            window.innerWidth = 20;
            const traverseResult4 = alphabetAdapter.traverseElements(alphabetDomOuter);
            expect(traverseResult4).toBe(null);

            window.innerWidth = 375;
            alphabetAdapter.heightWidthMinRatio = 40;
            const traverseResult5 = alphabetAdapter.traverseElements(alphabetDomOuter);
            expect(traverseResult5).toBe(null);
        });
    });
    describe('scrollBoxAdapt() method', () => {
        test('scrollBoxAdapt', () => {
            const alphabetAdapter = new AlphabetAdapter(JSON.stringify(configJSON));
            const alphabetDom = document.createElement('div');
            const childElement = document.createElement('div');
            alphabetDom.appendChild(childElement);
            alphabetAdapter.alphabetDom = alphabetDom;

            jest.spyOn(window, 'getComputedStyle').mockReturnValue({
                overflowY: 'hidden'
            } as CSSStyleDeclaration);

            let scrollAdapt = alphabetAdapter.scrollBoxAdapt();
            expect(scrollAdapt).toBe(false);

            jest.spyOn(window, 'getComputedStyle').mockReturnValue({
                overflowY: 'scroll'
            } as CSSStyleDeclaration);

            jest.spyOn(alphabetDom, 'getBoundingClientRect').mockReturnValue(mockRectVal);

            Object.defineProperty(window, 'innerHeight', {
                writable: true,
                configurable: true,
                value: 450
            });

            scrollAdapt = alphabetAdapter.scrollBoxAdapt();
            expect(scrollAdapt).toBe(false);

            window.innerHeight = 350;
            scrollAdapt = alphabetAdapter.scrollBoxAdapt();
            expect(scrollAdapt).toBe(true);
        });
    });
    describe('updateAlphabetButtonGroupVersion() method', () => {
        test('updateAlphabetButtonGroupVersion', () => {
            const configJSON: AlphabetConfig = {
                alphabetHeightWidthMinRatio: 16,
                alphabetIdentificationMinSize: 15,
            };
            const alphabetAdapter = new AlphabetAdapter(JSON.stringify(configJSON));
            alphabetAdapter.updateAlphabetButtonGroupVersion();
            expect(alphabetAdapter.alphabetDom).toBe(null);

            const alphabetDom = document.createElement('div');
            jest.spyOn(alphabetDom, 'getBoundingClientRect').mockReturnValue(mockRectVal);

            alphabetAdapter.alphabetDom = alphabetDom;
            alphabetAdapter.updateAlphabetButtonGroupVersion();
            expect(alphabetAdapter.alphabetDom).toBe(null);

            configJSON.alphabetHeightWidthMinRatio = 10;
            alphabetAdapter.alphabetDom = alphabetDom;
            alphabetAdapter.setConfig(JSON.stringify(configJSON));

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

            jest.spyOn(window, 'getComputedStyle').mockReturnValue({
                overflowY: 'scroll'
            } as CSSStyleDeclaration);

            Object.defineProperty(window, 'innerHeight', {
                writable: true,
                configurable: true,
                value: 350
            });

            alphabetAdapter.updateAlphabetButtonGroupVersion();
            expect(alphabetAdapter.buttonGroup).toBe(null);

            jest.spyOn(window, 'getComputedStyle').mockReturnValue({
                overflowY: 'hidden'
            } as CSSStyleDeclaration);

            alphabetAdapter.updateAlphabetButtonGroupVersion();
            expect(alphabetAdapter.buttonGroup).not.toBe(null);

            Object.defineProperty(alphabetDom, 'scrollHeight', {
                writable: true,
                configurable: true,
                value: 300
            });
            Object.defineProperty(window, 'innerHeight', {
                writable: true,
                configurable: true,
                value: 450
            });
            alphabetAdapter.updateAlphabetButtonGroupVersion();
            expect(alphabetAdapter.buttonGroup.dom.style.display).toBe(CONF.BUTTON_GROUP.BUTTON_DISPLAY_TYPES.HIDE);
        });
    });
});