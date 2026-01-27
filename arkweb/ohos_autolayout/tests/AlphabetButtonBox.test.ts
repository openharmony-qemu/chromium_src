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

import AlphabetButtonBox from '../src/Alphabet/AlphabetButtonBox';
import CONF from '../src/Alphabet/Conf';

describe('AlphabetButtonBox', () => {
    describe('Basic Structure', () => {
        test('AlphabetButtonBox instance', () => {
            const alphabetButtonBox = new AlphabetButtonBox();
            expect(typeof AlphabetButtonBox.checkIfShowNavButtonBox).toBe('function');
            expect(typeof alphabetButtonBox.handleButtonClick).toBe('function');
            expect(typeof alphabetButtonBox.getStepLength).toBe('function');
            expect(typeof alphabetButtonBox.show).toBe('function');
            expect(typeof alphabetButtonBox.hide).toBe('function');
            expect(typeof alphabetButtonBox.reset).toBe('function');
            expect(typeof alphabetButtonBox.calculateSteps).toBe('function');
            expect(typeof alphabetButtonBox.getAlphabetOriginalTranslateY).toBe('function');
            expect(typeof alphabetButtonBox.generateTransformByTranslateY).toBe('function');
            expect(typeof alphabetButtonBox.transformOriginalTransformToArray).toBe('function');
        });
    });
    describe('checkIfShowNavButtonBox() function', () => {
        test('checkIfShowNavButtonBox method will return false if input is null', () => {
            const checkResult: boolean = AlphabetButtonBox.checkIfShowNavButtonBox(null);
            expect(checkResult).toBe(false);
        });
    });
    describe('show() function', () => {
        test('show method will return void if input is empty', () => {
            const alphabetButtonBox = new AlphabetButtonBox();
            alphabetButtonBox.show(null);
            expect(alphabetButtonBox.dom.style.display).toBe('none');
            const alphabet = document.createElement('div');
            alphabetButtonBox.show(alphabet);
            expect(alphabetButtonBox.dom.style.display).toBe('flex');
            alphabetButtonBox.hide();
            expect(alphabetButtonBox.dom.style.display).toBe('none');
        });
    });
    describe('reset() function', () => {
        test('reset method', () => {
            const alphabetButtonBox = new AlphabetButtonBox();
            const alphabet = document.createElement('div');
            const originTransform = 'translate(-100px, -100px)';
            const originTransition = 'all 1s ease-in';

            alphabet.style.transform = originTransform;
            alphabet.style.transition = originTransition;

            alphabetButtonBox.show(alphabet);

            alphabetButtonBox.reset();

            expect(alphabet.style.transform).toBe(originTransform);
            expect(alphabet.style.transition).toBe(originTransition);
        });
    });
    describe('transformOriginalTransformToArray() function', () => {
        test('transformOriginalTransformToArray method', () => {
            const consoleInspect = jest.spyOn(console, 'log');
            const alphabetButtonBox = new AlphabetButtonBox();

            alphabetButtonBox.alphabetOriginalStyle.computedTransform = 'none';
            let transformResult = alphabetButtonBox.transformOriginalTransformToArray();
            expect(transformResult.length).toBe(0);

            alphabetButtonBox.alphabetOriginalStyle.computedTransform = 'matrix()';
            transformResult = alphabetButtonBox.transformOriginalTransformToArray();
            expect(transformResult.length).toBe(0);
            expect(consoleInspect).toHaveBeenCalledWith('parse transform style failed');

            alphabetButtonBox.alphabetOriginalStyle.computedTransform = 'matrix(1, 0, 0, 1, -100, -100)';
            transformResult = alphabetButtonBox.transformOriginalTransformToArray();
            expect(transformResult.length).toBe(6);

            alphabetButtonBox.alphabetOriginalStyle.computedTransform = 'matrix3d(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, -100, -100, -100, 1)';
            transformResult = alphabetButtonBox.transformOriginalTransformToArray();
            expect(transformResult.length).toBe(16);
        });
    });
    describe('getAlphabetOriginalTranslateY() function', () => {
        test('getAlphabetOriginalTranslateY method', () => {
            const alphabetButtonBox = new AlphabetButtonBox();

            alphabetButtonBox.alphabetOriginalStyle.computedTransform = 'none';
            let originalTranslateY = alphabetButtonBox.getAlphabetOriginalTranslateY();
            expect(originalTranslateY).toBe(0);

            alphabetButtonBox.alphabetOriginalStyle.computedTransform = 'matrix(1, 0, 0, 1, -100, -100)';
            originalTranslateY = alphabetButtonBox.getAlphabetOriginalTranslateY();
            expect(originalTranslateY).toBe(-100);

            alphabetButtonBox.alphabetOriginalStyle.computedTransform = 'matrix3d(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, -100, -100, -100, 1)';
            originalTranslateY = alphabetButtonBox.getAlphabetOriginalTranslateY();
            expect(originalTranslateY).toBe(-100);
        });
    });
    describe('generateTransformByTranslateY() function', () => {
        test('generateTransformByTranslateY method', () => {
            const alphabetButtonBox = new AlphabetButtonBox();

            alphabetButtonBox.alphabetOriginalStyle.computedTransform = 'none';
            let transformStyle = alphabetButtonBox.generateTransformByTranslateY(100);
            expect(transformStyle).toBe('translateY(100px)');

            alphabetButtonBox.alphabetOriginalStyle.computedTransform = 'matrix(1, 0, 0, 1, -100, -100)';
            transformStyle = alphabetButtonBox.generateTransformByTranslateY(100);
            expect(transformStyle).toBe('matrix(1, 0, 0, 1, -100, 100)');

            alphabetButtonBox.alphabetOriginalStyle.computedTransform = 'matrix3d(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, -100, -100, -100, 1)';
            transformStyle = alphabetButtonBox.generateTransformByTranslateY(100);
            expect(transformStyle).toBe('matrix3d(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, -100, 100, -100, 1)');
        });
    });
    describe('handleButtonClick() function', () => {
        test('upwards will not trigger if current step is the first', () => {
            const alphabetButtonBox = new AlphabetButtonBox();
            const alphabet = document.createElement('div');
            alphabetButtonBox.show(alphabet);
            alphabetButtonBox.stepCount = 2;
            alphabetButtonBox.stepNumber = 0;
            alphabetButtonBox.handleButtonClick(CONF.BUTTON_BOX.BUTTON_SWITCH_TYPES.UP);
            expect(alphabetButtonBox.stepNumber).toBe(0);
            alphabetButtonBox.handleButtonClick(CONF.BUTTON_BOX.BUTTON_SWITCH_TYPES.DOWN);
            expect(alphabetButtonBox.stepNumber).toBe(1);
        });
        test('downwards will not trigger if current step is the first', () => {
            const alphabetButtonBox = new AlphabetButtonBox();
            const alphabet = document.createElement('div');
            alphabetButtonBox.show(alphabet);
            alphabetButtonBox.stepCount = 2;
            alphabetButtonBox.stepNumber = 1;
            alphabetButtonBox.handleButtonClick(CONF.BUTTON_BOX.BUTTON_SWITCH_TYPES.DOWN);
            expect(alphabetButtonBox.stepNumber).toBe(1);
            alphabetButtonBox.handleButtonClick(CONF.BUTTON_BOX.BUTTON_SWITCH_TYPES.UP);
            expect(alphabetButtonBox.stepNumber).toBe(0);
        });
        test('calculate translateY', () => {
            const alphabetButtonBox = new AlphabetButtonBox();
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

            alphabetButtonBox.show(alphabet);
            alphabetButtonBox.stepCount = 3;
            alphabetButtonBox.stepNumber = 0;

            alphabetButtonBox.handleButtonClick(CONF.BUTTON_BOX.BUTTON_SWITCH_TYPES.DOWN);
            expect(alphabet.style.transform).toBe('translateY(-450px)');

            alphabetButtonBox.handleButtonClick(CONF.BUTTON_BOX.BUTTON_SWITCH_TYPES.DOWN);
            expect(alphabet.style.transform).toBe('translateY(-760px)');

            alphabetButtonBox.handleButtonClick(CONF.BUTTON_BOX.BUTTON_SWITCH_TYPES.DOWN);
            expect(alphabet.style.transform).toBe('translateY(-760px)');
        });
    });
});