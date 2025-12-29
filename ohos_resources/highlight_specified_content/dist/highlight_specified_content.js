/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

(function (window) {
    'use strict';

    if (window.__highlightSpecifiedContentLoaded) {
        return;
    }
    window.__highlightSpecifiedContentLoaded = true;

    const _document = window.document;

    const HIGHLIGHT_CONTENT_CONFIG = {
        maxFixLength: 20,
        fixBufferLength: 50,
        maxContentLength: 300,
    };

    window.AgentHighlightUtils = {
        ParseHashFromText(preText, nodeText, nextText, targetText) {
            let prefix = '';
            let suffix = '';
            let start = targetText;
            let end = '';

            let targetPos = nodeText.indexOf(targetText);
            if (targetPos !== -1) {
                let searchPrefix = '';
                if (targetPos > 0) {
                    searchPrefix = nodeText.substring(Math.max(0, targetPos - HIGHLIGHT_CONTENT_CONFIG.fixBufferLength), targetPos).trim();
                } else {
                    searchPrefix = preText?.substring(Math.max(0, preText.length - HIGHLIGHT_CONTENT_CONFIG.fixBufferLength)).trim();
                }

                if (searchPrefix) {
                    const segmenter = new Intl.Segmenter('en', { granularity: 'word' });
                    const iteratorPrefix = segmenter.segment(searchPrefix)[Symbol.iterator]();
                    let prefixSegments = [];
                    let value = iteratorPrefix.next().value;
                    while (value) {
                        prefixSegments.push(value);
                        value = iteratorPrefix.next().value;
                    }

                    let firstSegment = prefixSegments.pop().segment;
                    while (prefix.length + firstSegment.length <= HIGHLIGHT_CONTENT_CONFIG.maxFixLength && prefixSegments.length) {
                        prefix = firstSegment + prefix;
                        firstSegment = prefixSegments.pop().segment;
                    }
                    prefix += prefix.length ? '-,' : '';
                }

                let searchSuffix = '';
                if (targetPos + targetText.length < nodeText.length) {
                    searchSuffix = nodeText.substring(targetPos + targetText.length,
                        Math.min(nodeText.length, targetPos + targetText.length + HIGHLIGHT_CONTENT_CONFIG.fixBufferLength)).trim();
                } else {
                    searchSuffix = nextText?.substring(0, Math.min(nextText.length, HIGHLIGHT_CONTENT_CONFIG.fixBufferLength)).trim();
                }

                if (searchSuffix) {
                    const segmenter = new Intl.Segmenter('en', { granularity: 'word' });
                    const iteratorSuffix = segmenter.segment(searchSuffix)[Symbol.iterator]();
                    let suffixSegments = [];
                    let value = iteratorSuffix.next().value;
                    while (value) {
                        suffixSegments.push(value);
                        value = iteratorSuffix.next().value;
                    }

                    let i = 1;
                    let lastSegment = suffixSegments[0].segment;
                    while (suffix.length + lastSegment.length <= HIGHLIGHT_CONTENT_CONFIG.maxFixLength && i < suffixSegments.length) {
                        suffix += lastSegment;
                        lastSegment = suffixSegments[i].segment;
                        i++;
                    }
                    suffix = (suffix.length ? ',-' : '') + suffix;
                }
            }
            if (targetText.length > HIGHLIGHT_CONTENT_CONFIG.maxContentLength) {
                start = targetText.substring(0, HIGHLIGHT_CONTENT_CONFIG.maxFixLength);
                end = `,${targetText.substring(targetText.length - HIGHLIGHT_CONTENT_CONFIG.maxFixLength, targetText.length)}`;
            }

            const result = prefix + start + end + suffix;
            return encodeURI(result);
        },

        HighlightTargetContent(XpathJson, contentJson) {
            const Xpaths = decodeURIComponent(XpathJson);
            const content = decodeURIComponent(contentJson);
            const XpathNodes = JSON.parse(Xpaths);
            let urlString = '';
            let errorMessage = [];
            for (const XpathNode of XpathNodes) {
                let iterator = null;
                let nodeText = '';
                let preText = '';
                let nextText = '';
                try {
                    iterator = _document.evaluate(XpathNode.value, _document, null,
                        XPathResult.FIRST_ORDERED_NODE_TYPE, null);
                    nodeText = iterator.singleNodeValue.innerText;
                    preText = iterator.singleNodeValue.previousElementSibling?.innerText;
                    nextText = iterator.singleNodeValue.nextElementSibling?.innerText;
                } catch (error) {
                    errorMessage.push(error.message);
                    continue;
                }
                let hashText = `text=${this.ParseHashFromText(preText, nodeText, nextText, content)}`;
                urlString += (urlString.length ? '&' : ':~:') + hashText;
                errorMessage.push('noErr');
            }
            location.hash = urlString;
            return JSON.stringify(errorMessage);
        },
    };
})(window);