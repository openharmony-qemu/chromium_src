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

import AlphabetButtonGroup from '../src/Alphabet/AlphabetButtonGroup';
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

describe('AlphabetButtonGroup', () => {
    describe('Basic Structure', () => {
        test('AlphabetButtonGroup instance', () => {
            const alphabetButtonGroup = new AlphabetButtonGroup();
            expect(typeof AlphabetButtonGroup.checkIfShowNavButtonGroup).toBe('function');
            expect(typeof alphabetButtonGroup.handleButtonClick).toBe('function');
            expect(typeof alphabetButtonGroup.showButtons).toBe('function');
            expect(typeof alphabetButtonGroup.getStepLength).toBe('function');
            expect(typeof alphabetButtonGroup.show).toBe('function');
            expect(typeof alphabetButtonGroup.hide).toBe('function');
            expect(typeof alphabetButtonGroup.reset).toBe('function');
            expect(typeof alphabetButtonGroup.calculateSteps).toBe('function');
            expect(typeof alphabetButtonGroup.getAlphabetOriginalTranslateY).toBe('function');
            expect(typeof alphabetButtonGroup.generateTransformByTranslateY).toBe('function');
            expect(typeof alphabetButtonGroup.transformOriginalTransformToArray).toBe('function');
        });
    });
    describe('constructor() function', () => {
        test('AlphabetButtonGroup instance', () => {
            const alphabetButtonGroup = new AlphabetButtonGroup();
            const buttonClickInspect = jest.spyOn(alphabetButtonGroup, 'handleButtonClick');

            alphabetButtonGroup.downButton.click();
            expect(buttonClickInspect).toHaveBeenCalledWith(CONF.BUTTON_GROUP.BUTTON_SWITCH_TYPES.DOWN);

            alphabetButtonGroup.upButton.click();
            expect(buttonClickInspect).toHaveBeenCalledWith(CONF.BUTTON_GROUP.BUTTON_SWITCH_TYPES.UP);
        });
    });
    describe('checkIfShowNavButtonGroup() function', () => {
        test('checkIfShowNavButtonGroup method will return false if input is null', () => {
            const checkResult: boolean = AlphabetButtonGroup.checkIfShowNavButtonGroup(null);
            expect(checkResult).toBe(false);
        });
    });
    describe('show() function', () => {
        test('show method will return void if input is empty', () => {
            const alphabetButtonGroup = new AlphabetButtonGroup();
            const showButtonsInspect = jest.spyOn(alphabetButtonGroup, 'showButtons');
            alphabetButtonGroup.show(null);
            expect(showButtonsInspect).toHaveBeenCalledTimes(0);
        });
    });
    describe('showButtons() function', () => {
        test('showButtons display details ', () => {
            jest.useFakeTimers();
            const alphabetButtonGroup = new AlphabetButtonGroup();
            const alphabetDom = document.createElement('div');
            jest.spyOn(alphabetDom, 'getBoundingClientRect').mockReturnValue(mockRectVal);
            Object.defineProperty(window, 'innerWidth', {
                writable: true,
                configurable: true,
                value: 375,
            });
            alphabetButtonGroup.alphabet = alphabetDom;
            alphabetButtonGroup.showButtons(true, true);

            setTimeout(() => {
                expect(alphabetButtonGroup.upButton.style.right).toBe('53px');
                expect(alphabetButtonGroup.upButton.style.top).toBe('71px');
                expect(alphabetButtonGroup.upButton.style.display).toBe(CONF.BUTTON_GROUP.BUTTON_DISPLAY_TYPES.SHOW);
                expect(alphabetButtonGroup.downButton.style.display).toBe(CONF.BUTTON_GROUP.BUTTON_DISPLAY_TYPES.SHOW);
            }, 1000);
            jest.runAllTimers();
        });
        test('showButtons display details ', () => {
            jest.useFakeTimers();
            const alphabetButtonGroup = new AlphabetButtonGroup();
            const alphabetDom = document.createElement('div');
            jest.spyOn(alphabetDom, 'getBoundingClientRect').mockReturnValue(mockRectVal);
            Object.defineProperty(window, 'innerWidth', {
                writable: true,
                configurable: true,
                value: 375,
            });
            alphabetButtonGroup.alphabet = alphabetDom;
            alphabetButtonGroup.showButtons(false, true);
            setTimeout(() => {
                expect(alphabetButtonGroup.upButton.style.display).toBe(CONF.BUTTON_GROUP.BUTTON_DISPLAY_TYPES.HIDE);
                expect(alphabetButtonGroup.downButton.style.display).toBe(CONF.BUTTON_GROUP.BUTTON_DISPLAY_TYPES.SHOW);
            }, 1000);
            jest.runAllTimers();
        });
        test('showButtons display details ', () => {
            jest.useFakeTimers();
            const alphabetButtonGroup = new AlphabetButtonGroup();
            const alphabetDom = document.createElement('div');
            jest.spyOn(alphabetDom, 'getBoundingClientRect').mockReturnValue(mockRectVal);
            Object.defineProperty(window, 'innerWidth', {
                writable: true,
                configurable: true,
                value: 375,
            });
            alphabetButtonGroup.alphabet = alphabetDom;
            alphabetButtonGroup.showButtons(true, false);
            setTimeout(() => {
                expect(alphabetButtonGroup.upButton.style.display).toBe(CONF.BUTTON_GROUP.BUTTON_DISPLAY_TYPES.SHOW);
                expect(alphabetButtonGroup.downButton.style.display).toBe(CONF.BUTTON_GROUP.BUTTON_DISPLAY_TYPES.HIDE);
            }, 1000);
            jest.runAllTimers();
        });
    });
    describe('transformOriginalTransformToArray() function', () => {
        test('transformOriginalTransformToArray method', () => {
            const consoleInspect = jest.spyOn(console, 'log');
            const alphabetButtonGroup = new AlphabetButtonGroup();

            alphabetButtonGroup.alphabetOriginalStyle.computedTransform = 'none';
            let transformResult = alphabetButtonGroup.transformOriginalTransformToArray();
            expect(transformResult.length).toBe(0);

            alphabetButtonGroup.alphabetOriginalStyle.computedTransform = 'matrix()';
            transformResult = alphabetButtonGroup.transformOriginalTransformToArray();
            expect(transformResult.length).toBe(0);
            expect(consoleInspect).toHaveBeenCalledWith('parse transform style failed');

            alphabetButtonGroup.alphabetOriginalStyle.computedTransform = 'matrix(1, 0, 0, 1, -100, -100)';
            transformResult = alphabetButtonGroup.transformOriginalTransformToArray();
            expect(transformResult.length).toBe(6);

            alphabetButtonGroup.alphabetOriginalStyle.computedTransform = 'matrix3d(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, -100, -100, -100, 1)';
            transformResult = alphabetButtonGroup.transformOriginalTransformToArray();
            expect(transformResult.length).toBe(16);
        });
    });
    describe('getAlphabetOriginalTranslateY() function', () => {
        test('getAlphabetOriginalTranslateY method', () => {
            const alphabetButtonGroup = new AlphabetButtonGroup();

            alphabetButtonGroup.alphabetOriginalStyle.computedTransform = 'none';
            let originalTranslateY = alphabetButtonGroup.getAlphabetOriginalTranslateY();
            expect(originalTranslateY).toBe(0);

            alphabetButtonGroup.alphabetOriginalStyle.computedTransform = 'matrix(1, 0, 0, 1, -100, -100)';
            originalTranslateY = alphabetButtonGroup.getAlphabetOriginalTranslateY();
            expect(originalTranslateY).toBe(-100);

            alphabetButtonGroup.alphabetOriginalStyle.computedTransform = 'matrix3d(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, -100, -100, -100, 1)';
            originalTranslateY = alphabetButtonGroup.getAlphabetOriginalTranslateY();
            expect(originalTranslateY).toBe(-100);
        });
    });
    describe('generateTransformByTranslateY() function', () => {
        test('generateTransformByTranslateY method', () => {
            const alphabetButtonGroup = new AlphabetButtonGroup();

            alphabetButtonGroup.alphabetOriginalStyle.computedTransform = 'none';
            let transformStyle = alphabetButtonGroup.generateTransformByTranslateY(100);
            expect(transformStyle).toBe('translateY(100px)');

            alphabetButtonGroup.alphabetOriginalStyle.computedTransform = 'matrix(1, 0, 0, 1, -100, -100)';
            transformStyle = alphabetButtonGroup.generateTransformByTranslateY(100);
            expect(transformStyle).toBe('matrix(1, 0, 0, 1, -100, 100)');

            alphabetButtonGroup.alphabetOriginalStyle.computedTransform = 'matrix3d(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, -100, -100, -100, 1)';
            transformStyle = alphabetButtonGroup.generateTransformByTranslateY(100);
            expect(transformStyle).toBe('matrix3d(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, -100, 100, -100, 1)');
        });
    });
    describe('reset() function', () => {
        test('reset method', () => {
            const alphabetButtonGroup = new AlphabetButtonGroup();
            const alphabet = document.createElement('div');
            const originTransform = 'translate(-100px, -100px)';
            const originTransition = 'all 1s ease-in';

            alphabet.style.transform = originTransform;
            alphabet.style.transition = originTransition;

            alphabetButtonGroup.show(alphabet);

            alphabetButtonGroup.reset();

            expect(alphabet.style.transform).toBe(originTransform);
            expect(alphabet.style.transition).toBe(originTransition);
        });
    });
    describe('handleButtonClick() function', () => {
        test('upwards will not trigger if current step is the first', () => {
            const alphabetButtonGroup = new AlphabetButtonGroup();
            const alphabet = document.createElement('div');
            alphabetButtonGroup.show(alphabet);
            alphabetButtonGroup.stepCount = 2;
            alphabetButtonGroup.stepNumber = 0;
            alphabetButtonGroup.handleButtonClick(CONF.BUTTON_GROUP.BUTTON_SWITCH_TYPES.UP);
            expect(alphabetButtonGroup.stepNumber).toBe(0);
            alphabetButtonGroup.handleButtonClick(CONF.BUTTON_GROUP.BUTTON_SWITCH_TYPES.DOWN);
            expect(alphabetButtonGroup.stepNumber).toBe(1);
        });
        test('downwards will not trigger if current step is the first', () => {
            const alphabetButtonGroup = new AlphabetButtonGroup();
            const alphabet = document.createElement('div');
            alphabetButtonGroup.show(alphabet);
            alphabetButtonGroup.stepCount = 2;
            alphabetButtonGroup.stepNumber = 1;
            alphabetButtonGroup.handleButtonClick(CONF.BUTTON_GROUP.BUTTON_SWITCH_TYPES.DOWN);
            expect(alphabetButtonGroup.stepNumber).toBe(1);
            alphabetButtonGroup.handleButtonClick(CONF.BUTTON_GROUP.BUTTON_SWITCH_TYPES.UP);
            expect(alphabetButtonGroup.stepNumber).toBe(0);
        });
        test('calculate translateY', () => {
            const alphabetButtonGroup = new AlphabetButtonGroup();
            const alphabet = document.createElement('div');

            Object.defineProperty(window, 'innerHeight', {
                writable: true,
                configurable: true,
                value: 450
            });

            Object.defineProperty(alphabet, 'clientHeight', {
                writable: true,
                configurable: true,
                value: 1200
            });

            Object.defineProperty(alphabet, 'scrollHeight', {
                writable: true,
                configurable: true,
                value: 1210
            });

            alphabetButtonGroup.show(alphabet);
            alphabetButtonGroup.stepCount = 3;
            alphabetButtonGroup.stepNumber = 0;

            alphabetButtonGroup.handleButtonClick(CONF.BUTTON_GROUP.BUTTON_SWITCH_TYPES.DOWN);
            expect(alphabet.style.transform).toBe('translateY(-450px)');

            alphabetButtonGroup.handleButtonClick(CONF.BUTTON_GROUP.BUTTON_SWITCH_TYPES.DOWN);
            expect(alphabet.style.transform).toBe('translateY(-760px)');

            alphabetButtonGroup.handleButtonClick(CONF.BUTTON_GROUP.BUTTON_SWITCH_TYPES.DOWN);
            expect(alphabet.style.transform).toBe('translateY(-760px)');
        });
    });
});