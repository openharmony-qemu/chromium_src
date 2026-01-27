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
const AlphabetButtonGroup = require('./AlphabetButtonGroup');

class AlphabetAdapter {
    alphabetDom = null;
    adaptType = CONF.BUTTON_GROUP.TYPE;
    heightWidthMinRatio = CONF.BASE.HEIGHT_WIDTH_MIN_RATIO_DEFAULT;
    identificationMinSize = CONF.BASE.IDENTIFICATION_MIN_SIZE;
    buttonGroup = null;
    scrollAdaptOriginHeight = '';

    likelyAnAlphabet(str) {
        const minEd = 26;
        for (let i = 0; i <= str.length - this.identificationMinSize; i++) {
            if (Utils.isIncreasingAlphabet(str.substr(i, minEd), this.identificationMinSize)) {
                return true;
            }
        }
        return false;
    }

    traverseElements(element) {
        if (element.checkVisibility === undefined || !element.checkVisibility()) {
            return null;
        }

        let hasAlphabet = false;
        let textContent = Utils.cleanText(element.textContent);

        // not an alphabet node or alphabet root node
        if (textContent.length < this.identificationMinSize) {
            return null;
        }

        // check when textContent length match and child nodes number enough
        if ((textContent.length < CONF.BASE.MAX_STRING_LENGTH_ON_CHECK_ALPHABET) &&
            element.childNodes &&
            (element.childNodes.length >= this.identificationMinSize)) {
            hasAlphabet = this.likelyAnAlphabet(textContent);
        }

        Utils.log('hasAlphabet', hasAlphabet);

        if (element.childNodes && (element.childNodes.length > 0)) {
            for (let i of element.childNodes) {
                if (i.textContent.length < this.identificationMinSize) {
                    continue;
                }
                let ret = this.traverseElements(i);
                if (ret !== null) {
                    return ret;
                }
            }
        }

        if (element.getBoundingClientRect === null || element.getBoundingClientRect === undefined) {
            return null;
        }
        let boundingBox = element.getBoundingClientRect();
        Utils.log(
            'heightWidthMinRatio-width-height-hasAlphabet',
            this.heightWidthMinRatio,
            boundingBox.width,
            boundingBox.height,
            hasAlphabet,
            element
        );

        // 正文可能被识别到
        if (boundingBox.width >= window.innerWidth * 0.5) {
            return null;
        }

        if (boundingBox.width * this.heightWidthMinRatio > boundingBox.height) {
            return null;
        }

        if (hasAlphabet) {
            return element;
        }

        return null;
    }

    scrollBoxAdapt() {
        const computedStyle = getComputedStyle(this.alphabetDom);
        const boxCanScroll = (computedStyle.overflowY === 'scroll') || (computedStyle.overflowY === 'auto');
        if (!boxCanScroll) {
            return false;
        }
        const clientRect = this.alphabetDom.getBoundingClientRect();
        if (clientRect.bottom <= window.innerHeight) {
            return false;
        }
        this.scrollAdaptOriginHeight = this.alphabetDom.style.height;
        const newHeight = window.innerHeight - clientRect.top;
        this.alphabetDom.style.height = `${newHeight}px`;
        return true;
    }

    updateAlphabetButtonGroupVersion() {
        if (this.alphabetDom !== null && this.alphabetDom.getBoundingClientRect !== undefined) {
            let boundingBox = this.alphabetDom.getBoundingClientRect();
            if (boundingBox.width * this.heightWidthMinRatio > boundingBox.height) {
                this.alphabetDom = null;
            }
        }

        if (this.alphabetDom === null || !this.alphabetDom.checkVisibility()) {
            this.alphabetDom = this.traverseElements(document.body);
        }
        Utils.log('Alphabet', this.alphabetDom);

        if (this.alphabetDom === null) {
            if (this.buttonGroup) {
                this.buttonGroup.hide();
            }
            return;
        }

        const ifShowNavButtonGroup = AlphabetButtonGroup.checkIfShowNavButtonGroup(this.alphabetDom);

        if (ifShowNavButtonGroup) {
            if (this.scrollBoxAdapt()) {
                return;
            }
            if (!this.buttonGroup) {
                this.buttonGroup = new AlphabetButtonGroup();
            }
            this.buttonGroup.show(this.alphabetDom);
        } else {
            if (this.buttonGroup) {
                this.buttonGroup.hide();
            }
        }
    }

    update() {
        this.updateAlphabetButtonGroupVersion();
    }

    reset() {
        Utils.log('Alphabet reset');
        if (this.buttonGroup) {
            this.buttonGroup.reset();
        }
        if (this.scrollAdaptOriginHeight) {
            this.alphabetDom.style.height = this.scrollAdaptOriginHeight;
            this.scrollAdaptOriginHeight = '';
        }
        this.alphabetDom = null;
        this.update();
    }

    observeAction(mutationList, observer) {
        Utils.log('observeAction');
        this.reset();
    }

    observeDocumentChange() {
        const targetNode = document.body;
        const config = {
            // attributes: true,
            childList: true,
            subtree: true,
        };
        const observeCallback = Utils.debounce(this.observeAction.bind(this), 200);
        const observer = new MutationObserver(observeCallback);
        observer.observe(targetNode, config);
    }

    observeResize() {
        window.addEventListener('resize', () => {
            this.reset();
        });
    }

    run() {
        Utils.log('Alphabet run');
        this.update();
        this.observeResize();
        this.observeDocumentChange();
    }

    setConfig(config) {
        try {
            let {
                alphabetHeightWidthMinRatio,
                alphabetIdentificationMinSize,
            } = JSON.parse(config);

            Utils.log('Config got:', config);

            if (!isNaN(alphabetHeightWidthMinRatio) && (alphabetHeightWidthMinRatio > 0)) {
                this.heightWidthMinRatio =
                    alphabetHeightWidthMinRatio <=
                    CONF.BASE.HEIGHT_WIDTH_MIN_RATIO_CONFIG_MAX
                        ? alphabetHeightWidthMinRatio
                        : CONF.BASE.HEIGHT_WIDTH_MIN_RATIO_CONFIG_MAX;
            }

            if (!isNaN(alphabetIdentificationMinSize) && (alphabetIdentificationMinSize > 0)) {
                this.identificationMinSize =
                    alphabetIdentificationMinSize <=
                    CONF.BASE.IDENTIFICATION_MIN_SIZE_CONFIG_MAX
                        ? alphabetIdentificationMinSize
                        : CONF.BASE.IDENTIFICATION_MIN_SIZE_CONFIG_MAX;
            }

            Utils.log(
                `Config applied: heightWidthMinRatio=${this.heightWidthMinRatio}`,
                `, identificationMinSize=${this.identificationMinSize}`
            );
        } catch (error) {
            Utils.log(
                'Not a valid json config, use default instead.',
                `Values: heightWidthMinRatio=${this.heightWidthMinRatio}`,
                `, identificationMinSize=${this.identificationMinSize}`
            );
        }
    }

    constructor(config) {
        Utils.log('Alphabet start');

        this.setConfig(config);

        if (document.readyState === 'loading') {
            Utils.log('document on loading');
            document.addEventListener('DOMContentLoaded', () => {
                this.run();
            });
        } else {
            Utils.log('document loaded');
            this.run();
        }
    }
}

module.exports = AlphabetAdapter;
