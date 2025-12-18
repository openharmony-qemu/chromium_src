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

class AlphabetButtonGroup {
    dom = null;
    upButton = null;
    downButton = null;

    alphabet = null;

    stepPassed = 0;
    stepCount = 0;
    firstPageTop = 0;
    stepNumber = 0;

    styleSheet = {
        buttonBox: {
            display: CONF.BUTTON_GROUP.BOX_DISPLAY_TYPES.HIDE,
            width: 0,
            height: 0,
        },
        buttonBoxInnerButton: {
            zIndex: 99999,
            display: 'block',
            textAlign: 'center',
            verticalAlign: 'middle',
            position: 'fixed',
            right: `${CONF.BUTTON_GROUP.SIDE_DISTANCE.RIGHT_DEFAULT}px`,
            backgroundColor: 'transparent',
            width: `${CONF.BUTTON_GROUP.BUTTON_SIZE.WIDTH}px`,
            height: `${CONF.BUTTON_GROUP.BUTTON_SIZE.HEIGHT}px`,
            border: 'none',
            transition: 'opacity 0.5s ease-in-out',
            boxSizing: 'border-box',
            padding: `${CONF.BUTTON_GROUP.BUTTON_PADDING}px`,
            margin: 0,
            opacity: CONF.BUTTON_GROUP.OPACITY_VALUES.HIDE,
        },
        buttonBoxInnerContent: {
            display: 'block',
            width: '100%',
            height: '100%',
            color: '#666666',
            backgroundColor: '#ffffff',
            borderRadius: '100%',
            fontSize: `${CONF.BUTTON_GROUP.BUTTON_CONTENT_SIZE}px`,
            lineHeight: `${CONF.BUTTON_GROUP.BUTTON_CONTENT_SIZE}px`,
            fontFamily: CONF.BUTTON_GROUP.HM_SYMBOL_FONT_FAMILY_NAME,
        },
    };

    alphabetOriginalStyle = {
        transform: '',
        transition: '',
        computedTransform: '',
    };

    constructor() {
        const box = document.createElement('div');
        const upButton = document.createElement('div');
        const downButton = document.createElement('div');

        const upButtonContent = document.createElement('span');
        const downButtonContent = document.createElement('span');

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

        for (let k in this.styleSheet.buttonBoxInnerContent) {
            if (Object.prototype.hasOwnProperty.call(this.styleSheet.buttonBoxInnerContent, k)) {
                upButtonContent.style[k] = this.styleSheet.buttonBoxInnerContent[k];
                downButtonContent.style[k] = this.styleSheet.buttonBoxInnerContent[k];
            }
        }

        upButton.style.top = `${CONF.BUTTON_GROUP.SIDE_DISTANCE.TOP_DEFAULT}px`;
        downButton.style.bottom = `${CONF.BUTTON_GROUP.SIDE_DISTANCE.BOTTOM_DEFAULT}px`;

        upButtonContent.innerText = CONF.BUTTON_GROUP.BUTTON_TEXT.UP;
        downButtonContent.innerText = CONF.BUTTON_GROUP.BUTTON_TEXT.DOWN;

        upButton.addEventListener('click', () => {
            this.handleButtonClick(CONF.BUTTON_GROUP.BUTTON_SWITCH_TYPES.UP);
        });

        downButton.addEventListener('click', () => {
            this.handleButtonClick(CONF.BUTTON_GROUP.BUTTON_SWITCH_TYPES.DOWN);
        });

        upButton.appendChild(upButtonContent);
        downButton.appendChild(downButtonContent);

        box.appendChild(upButton);
        box.appendChild(downButton);

        document.body.appendChild(box);

        this.dom = box;
        this.upButton = upButton;
        this.downButton = downButton;
    }

    static checkIfShowNavButtonGroup(alphabet) {
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
        if (this.stepNumber === 0 && type === CONF.BUTTON_GROUP.BUTTON_SWITCH_TYPES.UP) {
            return;
        }
        if (this.stepNumber === (this.stepCount - 1) && type === CONF.BUTTON_GROUP.BUTTON_SWITCH_TYPES.DOWN) {
            return;
        }

        const stepLength = this.getStepLength();
        this.stepNumber += type === CONF.BUTTON_GROUP.BUTTON_SWITCH_TYPES.DOWN ? 1 : -1;

        let translateY = this.getAlphabetOriginalTranslateY();
        let showUpButton = true;
        let showDownButton = true;
        if (this.stepNumber === 0) {
            showUpButton = false;
        } else if (this.stepNumber === this.stepCount - 1) {
            const visibleHeight =
                (this.alphabet.clientHeight === this.alphabet.scrollHeight) ?
                (window.innerHeight - CONF.BUTTON_GROUP.LAST_PAGE_BOTTOM_DISTANCE) :
                Math.min(window.innerHeight, this.alphabet.clientHeight);
            translateY -= this.alphabet.scrollHeight + this.firstPageTop - visibleHeight;
            showDownButton = false;
        } else {
            translateY -= stepLength * this.stepNumber;
        }
        this.alphabet.style.transform = this.generateTransformByTranslateY(translateY);
        this.showButtons(showUpButton, showDownButton);
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

    showButtons(ifShowUpButton, ifShowDownButton) {
        if (!ifShowUpButton) {
            this.upButton.style.opacity = CONF.BUTTON_GROUP.OPACITY_VALUES.HIDE;
        } else {
            this.upButton.style.display = CONF.BUTTON_GROUP.BUTTON_DISPLAY_TYPES.SHOW;
        }
        if (!ifShowDownButton) {
            this.downButton.style.opacity = CONF.BUTTON_GROUP.OPACITY_VALUES.HIDE;
        } else {
            this.downButton.style.display = CONF.BUTTON_GROUP.BUTTON_DISPLAY_TYPES.SHOW;
        }

        setTimeout(() => {
            if (this.alphabet) {
                const alphabetClientRect = this.alphabet.getBoundingClientRect();

                const rightPlus = (alphabetClientRect.width - CONF.BUTTON_GROUP.BUTTON_SIZE.WIDTH) * 0.5;
                const right = window.innerWidth - alphabetClientRect.right + rightPlus;
                const buttonBoxRight = right > 0 ? right : CONF.BUTTON_GROUP.SIDE_DISTANCE.RIGHT_DEFAULT;

                const top = alphabetClientRect.top - CONF.BUTTON_GROUP.BUTTON_SIZE.HEIGHT - CONF.BUTTON_GROUP.SIDE_DISTANCE.TOP_PLUS;
                const buttonBoxTop = top > 0 ? top : CONF.BUTTON_GROUP.SIDE_DISTANCE.TOP_DEFAULT;

                this.upButton.style.top = `${buttonBoxTop}px`;
                this.upButton.style.right = `${buttonBoxRight}px`;
                this.downButton.style.right = `${buttonBoxRight}px`;
            }

            if (!ifShowUpButton) {
                this.upButton.style.display = CONF.BUTTON_GROUP.BUTTON_DISPLAY_TYPES.HIDE;
            } else {
                this.upButton.style.opacity = CONF.BUTTON_GROUP.OPACITY_VALUES.SHOW;
            }
            if (!ifShowDownButton) {
                this.downButton.style.display = CONF.BUTTON_GROUP.BUTTON_DISPLAY_TYPES.HIDE;
            } else {
                this.downButton.style.opacity = CONF.BUTTON_GROUP.OPACITY_VALUES.SHOW;
            }
        }, CONF.BUTTON_GROUP.SLIDE_TRANSITION_MICRO_SECONDS);
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

        this.alphabetOriginalStyle.computedTransform = computedStyle.transform;
        this.calculateSteps();

        this.alphabet.style.transition = CONF.BUTTON_GROUP.ALPHABET_TRANSITION_STYLE;
        this.dom.style.display = CONF.BUTTON_GROUP.BOX_DISPLAY_TYPES.SHOW;

        this.showButtons(false, true);
    }

    hide() {
        this.dom.style.display = CONF.BUTTON_GROUP.BOX_DISPLAY_TYPES.HIDE;
    }

    reset() {
        this.hide();
        this.stepNumber = 0;
        this.dom.style.right = `${CONF.BUTTON_GROUP.SIDE_DISTANCE.RIGHT_DEFAULT}px`;
        this.dom.style.bottom = `${CONF.BUTTON_GROUP.SIDE_DISTANCE.BOTTOM_DEFAULT}px`;
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
        this.upButton.style.top = `${CONF.BUTTON_GROUP.SIDE_DISTANCE.TOP_DEFAULT}px`;
        this.downButton.style.bottom = `${CONF.BUTTON_GROUP.SIDE_DISTANCE.BOTTOM_DEFAULT}px`;
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

module.exports = AlphabetButtonGroup;
