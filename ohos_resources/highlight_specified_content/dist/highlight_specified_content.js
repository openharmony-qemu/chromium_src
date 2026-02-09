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

    const ERROR_TYPE = {
        SUCCESS: 0,
        XPATH_ERROR: 1,
        NODE_NOT_FOUND: 2,
        TEXT_NOT_MATCH: 3,
    };

    window.AgentHighlightUtils = {
        // Text containing control characters (e.g., \n) can cause errors.
        __regex: /\p{C}/ug,

        EscapeText(text) {
            return encodeURIComponent(text).replace(/-/g, '%2D');
        },

        CollectSegments(text) {
            const segments = [];
            const segmenter = new Intl.Segmenter('en', { granularity: 'word' });
            const iterator = segmenter.segment(text)[Symbol.iterator]();
            let { value } = iterator.next();
            while (value) {
                segments.push(value.segment);
                ({ value } = iterator.next());
            }
            return segments;
        },

        BuildFromStart(segments, maxLength) {
            let result = '';
            let i = 0;
            while (i < segments.length && result.length + segments[i].length <= maxLength) {
                if (segments[i].match(this.__regex)) {
                    break;
                }
                result += segments[i];
                i++;
            }
            return result;
        },

        BuildFromEnd(segments, maxLength) {
            let result = '';
            while (segments.length && result.length + segments[segments.length - 1].length <= maxLength) {
                if (segments[segments.length - 1].match(this.__regex)) {
                    break;
                }
                result = segments.pop() + result;
            }
            return result;
        },

        GetTargetText(targetText) {
            let start;
            let end;

            if (targetText.match(this.__regex)) {
                const segmenter = new Intl.Segmenter('en', { granularity: 'word' });
                const iterator = segmenter.segment(targetText)[Symbol.iterator]();
                let { value } = iterator.next();

                start = '';
                while (value && !value.segment.match(this.__regex) &&
                    start.length <= HIGHLIGHT_CONTENT_CONFIG.maxFixLength) {
                    start += value.segment;
                    ({ value } = iterator.next());
                }

                const segments = [];
                while (value) {
                    if (!value.segment.match(this.__regex)) {
                        segments.push(value.segment);
                    }
                    ({ value } = iterator.next());
                }
                end = this.BuildFromEnd(segments, HIGHLIGHT_CONTENT_CONFIG.maxFixLength);
            } else if (targetText.length > HIGHLIGHT_CONTENT_CONFIG.maxContentLength) {
                start = targetText.substring(0, HIGHLIGHT_CONTENT_CONFIG.maxFixLength);
                end = targetText.substring(targetText.length - HIGHLIGHT_CONTENT_CONFIG.maxFixLength);
            } else {
                start = targetText;
                end = '';
            }

            return [this.EscapeText(start), (end.length ? ',' : '') + this.EscapeText(end)];
        },

        GetPrefix(targetPos, nodeText, preText) {
            let searchPrefix;
            if (targetPos > 0) {
                searchPrefix = nodeText.substring(
                    Math.max(0, targetPos - HIGHLIGHT_CONTENT_CONFIG.fixBufferLength),
                    targetPos
                ).trim();
            } else {
                searchPrefix = preText?.substring(
                    Math.max(0, preText.length - HIGHLIGHT_CONTENT_CONFIG.fixBufferLength)
                ).trim() || '';
            }

            if (!searchPrefix) {
                return '';
            }
            const segments = this.CollectSegments(searchPrefix);
            const prefix = this.BuildFromEnd(segments, HIGHLIGHT_CONTENT_CONFIG.maxFixLength);
            return this.EscapeText(prefix) + (prefix.length ? '-,' : '');
        },

        GetSuffix(targetPos, targetLength, nodeText, nextText) {
            let searchSuffix;
            if (targetPos + targetLength < nodeText.length) {
                searchSuffix = nodeText.substring(
                    targetPos + targetLength,
                    Math.min(nodeText.length, targetPos + targetLength + HIGHLIGHT_CONTENT_CONFIG.fixBufferLength)
                ).trim();
            } else {
                searchSuffix = nextText?.substring(
                    0, Math.min(nextText.length, HIGHLIGHT_CONTENT_CONFIG.fixBufferLength)
                ).trim() || '';
            }

            if (!searchSuffix) {
                return '';
            }
            const segments = this.CollectSegments(searchSuffix);
            const suffix = this.BuildFromStart(segments, HIGHLIGHT_CONTENT_CONFIG.maxFixLength);
            return (suffix.length ? ',-' : '') + this.EscapeText(suffix);
        },

        ParseHashFromText(preText, nodeText, nextText, targetText) {
            if (!targetText) {
                return '';
            }

            const targetPos = nodeText.indexOf(targetText);
            if (targetPos === -1) {
                const [start, end] = this.GetTargetText(targetText);
                return start + end;
            }

            const prefix = this.GetPrefix(targetPos, nodeText, preText);
            const suffix = this.GetSuffix(targetPos, targetText.length, nodeText, nextText);
            const [start, end] = this.GetTargetText(targetText);

            return prefix + start + end + suffix;
        },

        ProcessXPathNode(XpathNode, index, content) {
            try {
                const iterator = _document.evaluate(XpathNode.value, _document, null,
                    XPathResult.FIRST_ORDERED_NODE_TYPE, null);
                if (!iterator.singleNodeValue) {
                    return { type: ERROR_TYPE.NODE_NOT_FOUND, index, content, message: 'Node not found' };
                }

                const nodeText = iterator.singleNodeValue.innerText;
                const preText = iterator.singleNodeValue.previousElementSibling?.innerText || '';
                const nextText = iterator.singleNodeValue.nextElementSibling?.innerText || '';

                // Check if text matches, but continue execution regardless of match result,
                // shadow-root where text cannot be read.
                const isTextMatch = nodeText.indexOf(content) !== -1;
                const hashText = this.ParseHashFromText(preText, nodeText, nextText, content);

                if (!isTextMatch) {
                    return {
                        success: true,
                        hashText: (hashText ? 'text=' : '') + hashText,
                        warning: { type: ERROR_TYPE.TEXT_NOT_MATCH, index, content, message: 'Target content not found in node.' },
                    };
                }

                return { success: true, hashText: (hashText ? 'text=' : '') + hashText };
            } catch (error) {
                return { type: ERROR_TYPE.XPATH_ERROR, index, content, message: error.message };
            }
        },

        HighlightTargetContent(XpathJson, contentJson) {
            const content = decodeURIComponent(contentJson);
            const XpathNodes = JSON.parse(decodeURIComponent(XpathJson));
            let urlString = '';
            const errorMessage = [];

            XpathNodes.forEach((XpathNode, index) => {
                const result = this.ProcessXPathNode(XpathNode, index, content);

                if (result.success) {
                    urlString += result.hashText.length ?
                        (urlString.length ? '&' : '#:~:') + result.hashText : '';
                    if (result.warning) {
                        errorMessage.push(result.warning);
                    }
                } else {
                    errorMessage.push(result);
                }
            });

            if (urlString.length) {
                location.replace(urlString);
            }
            return JSON.stringify(errorMessage);
        },
    };
})(window);