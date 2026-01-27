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

const CONF = require('./Conf');
const Utils = require('./Utils');

class AlphabetButtonBox {
    dom = null;

    alphabet = null;

    stepPassed = 0;
    stepCount = 0;
    firstPageTop = 0;
    stepNumber = 0;

    styleSheet = {
        buttonBox: {
            zIndex: 99999,
            display: 'none',
            position: 'fixed',
            right: `${CONF.BUTTON_BOX.SIDE_DISTANCE.RIGHT_DEFAULT}px`,
            bottom: `${CONF.BUTTON_BOX.SIDE_DISTANCE.BOTTOM_DEFAULT}px`,
            flexDirection: 'column',
            borderWidth: '1px',
            borderStyle: 'solid',
            borderColor: '#999999',
            borderRadius: '10px',
            backgroundColor: '#ffffff',
            transition: 'all 0.5s ease-in-out',
        },
        buttonBoxInnerButton: {
            backgroundColor: 'transparent',
            padding: '2px 5px',
            lineHeight: '1.5',
            fontSize: '12px',
            color: '#666666',
            border: 'none',
        },
    };

    alphabetOriginalStyle = {
        transform: '',
        transition: '',
        computedTransform: '',
    };

    constructor() {
        const box = document.createElement('div');
        const upButton = document.createElement('button');
        const downButton = document.createElement('button');

        for (let i in this.styleSheet.buttonBox) {
            if (Object.prototype.hasOwnProperty.call(this.styleSheet.buttonBox, i)) {
                box.style[i] = this.styleSheet.buttonBox[i];
            }
        }

        for (let j in this.styleSheet.buttonBoxInnerButton) {
            if (Object.prototype.hasOwnProperty.call(this.styleSheet.buttonBoxInnerButton, j)) {
                upButton.style[j] = this.styleSheet.buttonBoxInnerButton[j];
                downButton.style[j] = this.styleSheet.buttonBoxInnerButton[j];
            }
        }

        upButton.style.borderBottomWidth = this.styleSheet.buttonBox.borderWidth;
        upButton.style.borderBottomStyle = this.styleSheet.buttonBox.borderStyle;
        upButton.style.borderBottomColor = this.styleSheet.buttonBox.borderColor;

        upButton.innerText = CONF.BUTTON_BOX.BUTTON_TEXT.UP;
        downButton.innerText = CONF.BUTTON_BOX.BUTTON_TEXT.DOWN;

        upButton.addEventListener('click', () => {
            this.handleButtonClick(CONF.BUTTON_BOX.BUTTON_SWITCH_TYPES.UP);
        });

        downButton.addEventListener('click', () => {
            this.handleButtonClick(CONF.BUTTON_BOX.BUTTON_SWITCH_TYPES.DOWN);
        });

        box.appendChild(upButton);
        box.appendChild(downButton);

        document.body.appendChild(box);

        this.dom = box;
    }

    static checkIfShowNavButtonBox(alphabet) {
        if (!alphabet) {
            return false;
        }
        const visibleHeight = Math.min(window.innerHeight - alphabet.getBoundingClientRect().y, alphabet.clientHeight);
        return visibleHeight < alphabet.scrollHeight - 1;
    }

    handleButtonClick(type) {
        if (!this.alphabet) {
            return;
        }
        if (this.stepNumber === 0 && type === CONF.BUTTON_BOX.BUTTON_SWITCH_TYPES.UP) {
            return;
        }
        if (this.stepNumber === (this.stepCount - 1) && type === CONF.BUTTON_BOX.BUTTON_SWITCH_TYPES.DOWN) {
            return;
        }

        const stepLength = this.getStepLength();
        this.stepNumber += type === CONF.BUTTON_BOX.BUTTON_SWITCH_TYPES.DOWN ? 1 : -1;

        let translateY = this.getAlphabetOriginalTranslateY();
        if (this.stepNumber === 0) {
        } else if (this.stepNumber === this.stepCount - 1) {
            const visibleHeight =
                (this.alphabet.clientHeight === this.alphabet.scrollHeight) ?
                (window.innerHeight - CONF.BUTTON_BOX.LAST_PAGE_BOTTOM_DISTANCE) :
                Math.min(window.innerHeight, this.alphabet.clientHeight);
            translateY -= this.alphabet.scrollHeight + this.firstPageTop - visibleHeight;
        } else {
            translateY -= stepLength * this.stepNumber;
        }
        this.alphabet.style.transform = this.generateTransformByTranslateY(translateY);
    }

    transformOriginalTransformToArray() {
        const transformStyle = this.alphabetOriginalStyle.computedTransform;
        if (transformStyle.startsWith('matrix')) {
            try {
                return transformStyle.match(/matrix(?<is3d>3d)?\((?<params>[^)]+)\)/)[2].split(', ');
            } catch (error) {
                Utils.log('parse transform style failed');
            }
        }
        return [];
    }

    getAlphabetOriginalTranslateY() {
        const matrixValues = this.transformOriginalTransformToArray();
        if (matrixValues.length === 6) {
            return parseFloat(matrixValues[5]);
        } else if (matrixValues.length === 16) {
            return parseFloat(matrixValues[13]);
        } else {
            return 0;
        }
    }

    generateTransformByTranslateY(translateY) {
        const matrixValues = this.transformOriginalTransformToArray();
        if (matrixValues.length === 6) {
            matrixValues[5] = translateY;
            return `matrix(${matrixValues.join(', ')})`;
        } else if (matrixValues.length === 16) {
            matrixValues[13] = translateY;
            return `matrix3d(${matrixValues.join(', ')})`;
        } else {
            return `translateY(${translateY}px)`;
        }
    }

    getStepLength() {
        return Math.min(window.innerHeight, this.alphabet.clientHeight);
    }

    show(alphabet) {
        if (!alphabet) {
            return;
        }

        this.alphabet = alphabet;

        const computedStyle = getComputedStyle(alphabet);

        this.alphabetOriginalStyle.transition = alphabet.style.transition;
        this.alphabetOriginalStyle.transform = alphabet.style.transform;

        this.calculateSteps();

        this.alphabet.style.transition = CONF.BUTTON_BOX.ALPHABET_TRANSITION_STYLE;

        const alphabetClientRect = alphabet.getBoundingClientRect();
        const bottom = window.innerHeight - (alphabetClientRect.y + alphabetClientRect.height);
        const right = window.innerWidth - alphabetClientRect.x;
        const buttonBoxRight =
          right > 0
            ? right + CONF.BUTTON_BOX.SIDE_DISTANCE.RIGHT_PLUS
            : CONF.BUTTON_BOX.SIDE_DISTANCE.RIGHT_DEFAULT;
        const buttonBoxBottom =
          bottom > 0
            ? bottom + CONF.BUTTON_BOX.SIDE_DISTANCE.BOTTOM_PLUS
            : CONF.BUTTON_BOX.SIDE_DISTANCE.BOTTOM_DEFAULT;

        this.dom.style.display = 'flex';
        this.dom.style.right = `${buttonBoxRight}px`;
        this.dom.style.bottom = `${buttonBoxBottom}px`;
    }

    hide() {
        this.dom.style.display = 'none';
    }

    reset() {
        this.hide();
        this.stepNumber = 0;
        this.dom.style.right = `${CONF.BUTTON_BOX.SIDE_DISTANCE.RIGHT_DEFAULT}px`;
        this.dom.style.bottom = `${CONF.BUTTON_BOX.SIDE_DISTANCE.BOTTOM_DEFAULT}px`;
        if (this.alphabet) {
            this.alphabet.style.transform = this.alphabetOriginalStyle.transform;
            this.alphabet.style.transition = this.alphabetOriginalStyle.transition;
        }
        this.alphabet = null;
        this.alphabetOriginalStyle.transform = '';
        this.alphabetOriginalStyle.transition = '';
        this.alphabetOriginalStyle.computedTransform = '';
        this.firstPageTop = 0;
        this.stepCount = 0;
    }

    calculateSteps() {
        let totalHeight = this.alphabet.scrollHeight;
        if (window.innerHeight - this.alphabet.getBoundingClientRect().y < this.alphabet.clientHeight) {
            this.firstPageTop = this.alphabet.getBoundingClientRect().y;
            totalHeight += this.firstPageTop;
        }
        this.stepCount = Math.ceil(totalHeight / this.getStepLength());
    }
}

module.exports = AlphabetButtonBox;