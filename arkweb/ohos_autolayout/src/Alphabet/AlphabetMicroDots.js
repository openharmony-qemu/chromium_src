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

class AlphabetMicroDots {
    alphabetItemDefaultDisplayStyle = '';

    distributeWeightType = CONF.MICRO_DOTS.DISTRIBUTE_WEIGHT_TYPE_DEFAULT;

    hiddenElements = [];

    dottedElements = [];

    constructor(distributeWeightType) {
        if (distributeWeightType &&
            CONF.MICRO_DOTS.DISTRIBUTE_WEIGHT_TYPES.indexOf(distributeWeightType) !== -1) {
            this.distributeWeightType = distributeWeightType;
        }
    }

    adjustAlphabet(toKeep, element) {
        let preMoved = false;
        for (let i = 0; i < element.childNodes.length; i++) {
            let child = element.childNodes[i];
            let c = Utils.cleanText(child.textContent);
            if (!Utils.isAlphabetLetter(c[0])) {
                continue;
            }
            if (!this.alphabetItemDefaultDisplayStyle) {
                this.alphabetItemDefaultDisplayStyle = getComputedStyle(child).display;
            }
            if (!toKeep.includes(c)) {
                if (!preMoved) {
                    this.showItem(child, true);
                } else {
                    this.hideItem(child);
                }
                preMoved = true;
            } else {
                this.showItem(child);
                preMoved = false;
            }
        }
    }

    hideItem(itemElement) {
        itemElement.style.display = 'none';
        this.hiddenElements.push(itemElement);
    }

    showItem(itemElement, isMicro = false) {
        if (isMicro) {
            const textNode = this.getTextNode(itemElement);
            const originalText = textNode.nodeName === CONF.MICRO_DOTS.TEXT_NODE_NAME ? textNode.nodeValue : textNode.innerText;
            itemElement.dataset.originTag = originalText;
            if (textNode.nodeName === CONF.MICRO_DOTS.TEXT_NODE_NAME) {
                textNode.nodeValue = CONF.MICRO_DOTS.MICRO_DISPLAY_TEXT;
            } else {
                textNode.innerText = CONF.MICRO_DOTS.MICRO_DISPLAY_TEXT;
            }
            this.dottedElements.push(itemElement);
        }
        itemElement.style.display = this.alphabetItemDefaultDisplayStyle;
    }

    reset() {
        this.resetHiddenElements();
        this.resetDottedElements();
    }

    resetHiddenElements() {
        this.hiddenElements.forEach(itemElement => {
            itemElement.style.display = this.alphabetItemDefaultDisplayStyle;
        });
        this.hiddenElements = [];
    }

    resetDottedElements() {
        this.dottedElements.forEach(itemElement => {
            if (itemElement.dataset.originTag) {
                const textNode = this.getTextNode(itemElement);
                if (textNode.nodeName === CONF.MICRO_DOTS.TEXT_NODE_NAME) {
                    textNode.nodeValue = itemElement.dataset.originTag;
                } else {
                    textNode.innerText = itemElement.dataset.originTag;
                }
            }
        });
        this.dottedElements = [];
    }

    getTextNode(itemElement) {
        if ((itemElement.nodeName === CONF.MICRO_DOTS.TEXT_NODE_NAME) || (itemElement.childNodes.length < 1)) {
            return itemElement;
        }
        let result = null;
        for (let i = 0; i < itemElement.childNodes.length; i++) {
            if (itemElement.childNodes[i].nodeName === CONF.MICRO_DOTS.TEXT_NODE_NAME) {
                const text = itemElement.childNodes[i].nodeValue.trim();
                if (text) {
                    result = itemElement.childNodes[i];
                }
            } else if (itemElement.childNodes[i].nodeName !== CONF.MICRO_DOTS.COMMENT_NODE_NAME) {
                result = this.getTextNode(itemElement.childNodes[i]);
            }
            if (result) {
                break;
            }
        }
        return result ? result : itemElement;
    }

    calculateNumOfLettersToRemove(element) {
        let charHeight = 0;
        let innerText = '';
        let visibleHeight = Math.min(window.innerHeight - element.getBoundingClientRect().y, element.clientHeight);
        let total = 0;

        if (visibleHeight >= element.scrollHeight) {
            return {
                removeCount: 0,
                total,
                innerText,
            };
        }

        let deltaLength = element.scrollHeight - visibleHeight;
        let totalContentHeight = 0;
        let removeCount = 0;
        let extra = 0;

        for (let child of element.childNodes) {
            if (child.nodeName === CONF.MICRO_DOTS.TEXT_NODE_NAME) {
                continue;
            }
            let c = child.textContent;
            c = Utils.cleanText(c);
            if (Utils.isAlphabetLetter(c[0])) {
                if (total === 0) {
                    charHeight = child.clientHeight;
                    removeCount = Math.ceil(deltaLength / charHeight);
                }
                total++;
                innerText += c;
            } else {
                extra++;
            }
            totalContentHeight += child.clientHeight;
        }

        if (totalContentHeight > element.scrollHeight) {
            deltaLength = totalContentHeight - visibleHeight;
            removeCount = Math.ceil(deltaLength / charHeight);
        }

        removeCount += extra;

        if (total < 5 + removeCount) {
            removeCount = total - 5;
        }

        return {
            removeCount,
            total,
            innerText,
        };
    }

    distributeInteger(total, buckets) {
        if (buckets === 0) {
            return [];
        }

        if (total < buckets) {
            // should not happen given constraints, but safeguard
            const res = new Array(buckets).fill(0);
            for (let i = 0; i < total; i++) {
                res[i] = 1;
            }
            return res;
        }

        const base = Math.floor(total / buckets);
        const remainder = total % buckets;
        const res = new Array(buckets).fill(base);

        // Distribute remainder to which side
        // Default is center
        // If need config, should make it be exposed.
        if (remainder > 0) {
            let startIndex = Math.floor((buckets - remainder) / 2);
            for (let i = 0; i < remainder; i++) {
                if (this.distributeWeightType === 'start') {
                    res[i] += 1;
                } else if (this.distributeWeightType === 'end') {
                    res[buckets - 1 - i] += 1;
                } else {
                    res[startIndex] += 1;
                }
            }
        }
        return res;
    }

    solveStringCut(originalStr, removeCount) {
        const total = originalStr.length;
        const targetLen = total - removeCount;

        // --- Cases ---

        // Must keep at least 2 chars
        if (total < 2) {
            return {
                result:  originalStr,
                error: 'String too short',
            };
        }

        // return original if removeCount = 0
        if (removeCount === 0) {
            return {
                result: originalStr,
                indices: Array.from({length: total}, (_, i) => i),
                stats: 'No cuts',
                pipes: 0,
                kept: total,
                error: null,
            };
        }

        /**
         * description
         * L = total - removeCount (Target Length)
         * L = k + p (Kept chars + Pipes)
         * Need to find valid k and p
         * p = L - k
         * k >= 2 (keep start and end)
         * p >= 1 (since removeCount > 0, must have cuts)
         * R >= 2 * p (consumed >= 2 each pipe)
         * R = total - k
         */
        let bestConfig = null;

        for (let p = removeCount; p >= 1; p--) {
            const k = targetLen - p;

            if (k < 2) {
                continue;
            }

            const removed = total - k;
            if (removed < 2 * p) {
                continue;
            }

            if (p > k - 1) {
                continue;
            }

            bestConfig = {
                k,
                p,
            };
            break;
        }

        const {
            k,
            p,
        } = bestConfig;
        const removedCount = total - k;
        const numGroups = p + 1;

        // --- Distribution ---

        // Distribute k characters into groups
        const groupSizes = this.distributeInteger(k, numGroups);

        // Distribute removed characters info p pipes.
        const baseGapRemove = p * 2;
        const slackRemove = removedCount - baseGapRemove;

        // Distribute the slack evenly among the p gaps
        const slackDist = this.distributeInteger(slackRemove, p);

        const gapSizes = slackDist.map(s => 2 + s);

        // --- Reconstruct Indices ---
        const indicesToKeep = [];
        let currentOriginalIndex = 0;

        for (let i = 0; i < numGroups; i++) {
            const gSize = groupSizes[i];
            for (let j = 0; j < gSize; j++) {
                indicesToKeep.push(currentOriginalIndex);
                currentOriginalIndex++;
            }

            if (i < numGroups - 1) {
                const gapSize = gapSizes[i];
                currentOriginalIndex += gapSize;
            }
        }

        // --- Build String ---
        let resStr = '';
        let lastIdx = -1;

        for (let idx of indicesToKeep) {
            if (lastIdx !== -1 && idx > lastIdx + 1) {
                resStr += '|';
            }
            resStr += originalStr[idx];
            lastIdx = idx;
        }

        return {
            result: resStr,
            indices: indicesToKeep,
            stats: `Kept: ${k}, Pipes: ${p}`,
            error: null,
        };
    }
}

module.exports = AlphabetMicroDots;