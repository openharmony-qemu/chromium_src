/**
 * dom_tree_extractor_source.js - ArkWeb DOM树状结构提取
 *
 * 主要功能：
 * - 提取可视区域的DOM节点
 * - 提取TextNode节点
 */
(function () {
    const print = (message) => {
        if (typeof window.ArkWebContentChangeDetector !== 'undefined' &&
            typeof window.ArkWebContentChangeDetector.reportContentChange === 'function') {
            window.ArkWebContentChangeDetector.reportContentChange(message);
        }
    };

    if (window.__arkwebDomTreeStatisticLoaded) {
        print({
            type: 'debugLog',
            data: { log: 'dom tree script has loaded' }
        });
        return;
    }

    // ========================
    // == 原始extractor.js核心函数（v4改进版） ==
    // ========================
    // ========================
    // == 工具函数定义 ==
    // ========================

    // v002: one extraction owns one coherent set of read caches. They are
    // reset before every public full extraction, so nothing survives across
    // DOM revisions or reports.
    let extractionXPathCache = new WeakMap();
    let extractionSiblingInfoCache = new WeakMap();
    let extractionStyleCache = new WeakMap();
    let extractionRectCache = new WeakMap();

    function resetExtractionCaches() {
        extractionXPathCache = new WeakMap();
        extractionSiblingInfoCache = new WeakMap();
        extractionStyleCache = new WeakMap();
        extractionRectCache = new WeakMap();
    }

    function getCachedComputedStyle(element) {
        let style = extractionStyleCache.get(element);
        if (!style) {
            style = window.getComputedStyle(element);
            extractionStyleCache.set(element, style);
        }
        return style;
    }

    function getCachedBoundingRect(element) {
        let rect = extractionRectCache.get(element);
        if (!rect) {
            rect = element.getBoundingClientRect();
            extractionRectCache.set(element, rect);
        }
        return rect;
    }

    function getSiblingInfo(element) {
        const parent = element.parentElement;
        if (!parent) {
            return { index: 1, hasSimilarNextSibling: false };
        }

        let siblingInfo = extractionSiblingInfoCache.get(parent);
        if (!siblingInfo) {
            siblingInfo = new WeakMap();
            const tagCounts = new Map();
            for (const child of parent.children) {
                const count = (tagCounts.get(child.nodeName) || 0) + 1;
                tagCounts.set(child.nodeName, count);
                siblingInfo.set(child, {
                    index: count,
                    // Preserve v0's exact rule: only the immediate next
                    // element sibling determines whether the first item gets
                    // an explicit [1].
                    hasSimilarNextSibling: child.nextElementSibling?.nodeName === child.nodeName
                });
            }
            extractionSiblingInfoCache.set(parent, siblingInfo);
        }
        return siblingInfo.get(element) || { index: 1, hasSimilarNextSibling: false };
    }

    /**
     * 获取元素的 xpath 路径
     * @param {Node} element 元素节点
     * @returns {string} 元素的 xpath 路径
     */
    function getXPath(node) {
        if (!(node instanceof Node)) {
            return null;
        }

        // 直接返回ID路径（最短且最高效）
        if (node.nodeType === Node.ELEMENT_NODE && node.id) {
            return `//*[@id="${node.id}"]`;
        }

        if (node.nodeType !== Node.ELEMENT_NODE) {
            return null;
        }

        const unresolved = [];
        let current = node;
        let prefix = null;
        while (current && current.nodeType === Node.ELEMENT_NODE) {
            const cached = extractionXPathCache.get(current);
            if (cached) {
                prefix = cached;
                break;
            }
            unresolved.push(current);
            current = current.parentElement;
        }

        while (unresolved.length) {
            const element = unresolved.pop();
            const { index, hasSimilarNextSibling } = getSiblingInfo(element);
            const tag = element.nodeName.toLowerCase();
            const segment = index > 1 || hasSimilarNextSibling ? `${tag}[${index}]` : tag;
            prefix = prefix ? `${prefix}/${segment}` : `/${segment}`;
            extractionXPathCache.set(element, prefix);
        }

        return prefix;
    }

    function isScrollable(element, direction = 'both') {
        if (!element) {
            return false;
        }

        // 获取计算后的样式
        const computedStyle = getCachedComputedStyle(element);
        const overflowX = computedStyle.overflowX;
        const overflowY = computedStyle.overflowY;

        // 检查 overflow 属性是否允许滚动
        const hasScrollableOverflowX = ['scroll', 'auto'].includes(overflowX);
        const hasScrollableOverflowY = ['scroll', 'auto'].includes(overflowY);

        // 特殊处理 html 和 body 元素
        const isHtmlOrBody = element === document.documentElement;

        let canScrollVertical = false;
        let canScrollHorizontal = false;

        if (isHtmlOrBody) {
            // 对于 html 和 body，检查文档是否可以滚动
            const docElement = document.documentElement;
            const body = document.body;

            // 垂直滚动检查
            if (hasScrollableOverflowY || overflowY === 'visible') {
                const scrollHeight = Math.max(
                    body.scrollHeight,
                    body.offsetHeight,
                    docElement.clientHeight,
                    docElement.scrollHeight,
                    docElement.offsetHeight
                );
                const clientHeight = docElement.clientHeight;
                canScrollVertical = scrollHeight > clientHeight;
            }

            // 水平滚动检查
            if (hasScrollableOverflowX || overflowX === 'visible') {
                const scrollWidth = Math.max(
                    body.scrollWidth,
                    body.offsetWidth,
                    docElement.clientWidth,
                    docElement.scrollWidth,
                    docElement.offsetWidth
                );
                const clientWidth = docElement.clientWidth;
                canScrollHorizontal = scrollWidth > clientWidth;
            }
        } else {
            // 普通元素的滚动检查
            // 垂直滚动：内容高度 > 可见高度
            if (hasScrollableOverflowY) {
                canScrollVertical = element.scrollHeight > element.clientHeight;
            }

            // 水平滚动：内容宽度 > 可见宽度
            if (hasScrollableOverflowX) {
                canScrollHorizontal = element.scrollWidth > element.clientWidth;
            }
        }

        // 根据指定方向返回结果
        if (direction === 'vertical') {
            return canScrollVertical;
        } else if (direction === 'horizontal') {
            return canScrollHorizontal;
        } else {
            return canScrollVertical || canScrollHorizontal;
        }
    }

    let shouldAbortExtraction = false;

    function isSuspiciousShadowDOM(element) {
        // 探测是否存在 Shadow DOM (Open 或 Closed)
        const hasOpenShadow = !!element.shadowRoot;
        const hasClosedShadow = typeof element.hasArkWebShadowRoot === 'function' && element.hasArkWebShadowRoot();
        if (hasOpenShadow || hasClosedShadow) {
            const rect = getCachedBoundingRect(element);
            // 宽高都超过视口的 80%
            const isMassiveByDimension = (rect.width > window.innerWidth * 0.8) && (rect.height > window.innerHeight * 0.8);
            if (isMassiveByDimension) {
                shouldAbortExtraction = true;
                return true;
            } else {
                return false;
            }
        }
        // 安全节点，正常放行
        return false;
    }

    // ========================
    // == 语义角色相关配置 ==
    // ========================

    /**
     * 一组 ARIA 语义区域名称，用于识别页面结构。
     * @type {Set<string>}
     */
    const semanticRoles = new Set([
        'banner', 'main', 'navigation', 'complementary', 'contentinfo', 'search', 'form', 'region'
    ]);

    /**
     * HTML5 标签到 ARIA 角色的映射表。
     * @type {Object}
     */
    const html5ToAriaRole = {
        'MAIN': 'main',
        'NAV': 'navigation',
        'ASIDE': 'complementary',
        'FOOTER': 'contentinfo',
        'HEADER': 'banner',
        'SECTION': 'region',
        'FORM': 'form'
    };


    /**
     * 获取元素的"自身文本内容"（不包括子元素内容）。
     * @param {Node} el 元素或文本节点。
     * @returns {string}
     */
    function getOwnTextContent(el) {
        let text = '';
        if (el.nodeType === Node.TEXT_NODE) {
            return el.textContent.trim();
        }
        for (let node of el.childNodes) {
            if (node.nodeType === Node.TEXT_NODE) {
                text += node.textContent.trim();
            }
        }
        return text;
    }

    /**
     * 判断两个矩形是否相交，且目标矩形至少保留 90% 的面积。
     * @param {DOMRect} rectA 目标矩形
     * @param {DOMRect} rectB 祖先矩形
     * @returns {boolean}
     */
    function hasIntersection(rectA, rectB) {
        const x1 = Math.max(rectA.left, rectB.left);
        const y1 = Math.max(rectA.top, rectB.top);
        const x2 = Math.min(rectA.right, rectB.right);
        const y2 = Math.min(rectA.bottom, rectB.bottom);

        const intersectionWidth = x2 - x1;
        const intersectionHeight = y2 - y1;

        if (intersectionWidth <= 0 || intersectionHeight <= 0) {
            return false;
        }

        const intersectionArea = intersectionWidth * intersectionHeight;
        const targetArea = rectA.width * rectA.height;
        const ratio = intersectionArea / targetArea;

        return ratio >= 0.9;
    }

    /**
     * 判断目标元素是否在祖先元素的水平可见区域内。
     * @param {DOMRect} rectA 目标矩形
     * @param {DOMRect} rectB 祖先矩形
     * @returns {boolean}
     */
    function hasIntersectionXOnly(rectA, rectB) {
        const left = Math.max(rectA.left, rectB.left);
        const right = Math.min(rectA.right, rectB.right);

        if (left >= right) {
            return false;
        }

        const intersectionWidth = right - left;
        const targetWidth = rectA.width;
        const ratio = intersectionWidth / targetWidth;

        return ratio >= 0.9;
    }

    /**
     * 判断节点是否在其祖先节点的可见区域内（未被裁剪）。
     * @param {Node} node 当前节点
     * @param {string|null} semanticRole 当前节点的语义角色
     * @returns {boolean}
     */
    function isVisibleByAncestorIntersections(node, semanticRole) {
        let element;

        if (node.nodeType === Node.TEXT_NODE) {
            element = node.parentElement;
        } else if (node.nodeType === Node.ELEMENT_NODE) {
            element = node;
        } else {
            return false;
        }

        if (element.tagName === 'UL') {
            return true;
        }

        const targetRect = getCachedBoundingRect(element);
        // v004: hoisted once instead of re-evaluating per clipping ancestor.
        const requiresFullIntersection = semanticRole === 'banner';

        let current = element.parentElement;

        while (current && current !== document.body && current !== document.documentElement) {
            const style = getCachedComputedStyle(current);
            const transform = style.transform;

            if (transform && transform !== 'none') {
                current = current.parentElement;
                continue;
            }

            // 判断 overflow 类型
            const hasOverflowClip =
                style.overflow !== 'visible' ||
                style.overflowX !== 'visible' ||
                style.overflowY !== 'visible';

            // 判断 transform（与跳过分支同一份样式值）
            const hasTransform = transform !== 'none';

            // 判断 filter
            const hasFilter = style.filter !== 'none';

            // 判断 clip-path
            const hasClipPath = style.clipPath !== 'none' && style.clipPath !== '';

            // contain: layout 或 strict
            const hasContainLayout = /layout|strict/.test(style.contain);

            // mask
            const hasMask = style.mask !== 'none' || style.webkitMaskImage !== 'none';

            if (hasOverflowClip || hasTransform || hasFilter || hasClipPath || hasContainLayout || hasMask) {
                const ancestorRect = getCachedBoundingRect(current);
                const intersection = requiresFullIntersection
                    ? hasIntersection(targetRect, ancestorRect)
                    : hasIntersectionXOnly(targetRect, ancestorRect);

                if (!intersection) {
                    return false;
                }
            }

            current = current.parentElement;
        }

        return true;
    }

    //获取textNode节点的rect
    function getTextNodeRect(textNode) {
        const range = document.createRange();
        range.selectNodeContents(textNode);
        return range.getBoundingClientRect();
    }

    function isOccluded(element) {
        // 检查元素是否存在且可见
        if (!element || !element.getBoundingClientRect) {
            return false;
        }
        const rect = getCachedBoundingRect(element);
        // 如果元素尺寸为0，认为不可见
        if (rect.width === 0 || rect.height === 0) {
            return true;
        }
        // 检查元素的样式
        const computedStyle = getCachedComputedStyle(element);
        if (computedStyle.display === 'none' ||
            computedStyle.visibility === 'hidden' ||
            computedStyle.opacity === '0') {
            return true;
        }
        // 获取5个检测点坐标：中心 + 九宫格中的上下左右中点
        const points = [
            { x: rect.left + rect.width / 2, y: rect.top + rect.height / 2 },
            { x: rect.left + rect.width * 0.25, y: rect.top + rect.height * 0.25 },
            { x: rect.left + rect.width * 0.25, y: rect.top + rect.height * 0.75 },
            { x: rect.left + rect.width * 0.75, y: rect.top + rect.height * 0.25 },
            { x: rect.left + rect.width * 0.75, y: rect.top + rect.height * 0.75 }
        ];
        let occlusionCount = 0;
        for (const point of points) {
            const topElement = document.elementFromPoint(point.x, point.y);
            // 如果命中的是目标元素或其子元素，说明未被遮挡
            if (topElement === element || element.contains(topElement)) {
                continue;
            }
            // 如果命中的是目标元素的父元素，也不算遮挡
            if (topElement && topElement.contains(element)) {
                continue;
            }
            // 如果没有命中任何元素（如超出视窗），不算遮挡
            if (!topElement) {
                continue;
            }
            // 检查遮挡元素是否透明
            const occludingStyle = getCachedComputedStyle(topElement);
            const opacity = parseFloat(occludingStyle.opacity);
            if (opacity === 0) {
                continue;
            }
            // 检查背景是否透明并无内容
            const backgroundColor = occludingStyle.backgroundColor;
            const isBackgroundTransparent =
                backgroundColor === 'transparent' ||
                backgroundColor === 'rgba(0, 0, 0, 0)';
            const hasVisibleContent =
                topElement.textContent.trim() !== '' ||
                occludingStyle.backgroundImage !== 'none' ||
                occludingStyle.border !== '0px none rgb(0, 0, 0)';
            if (isBackgroundTransparent && !hasVisibleContent) {
                continue;
            }
            // 其他情况视为遮挡
            occlusionCount++;
        }
        // 投票机制：超过半数被遮挡，则返回 true（被遮挡）
        return occlusionCount >= 3;
    }

    /**
     * 检查元素是否可见（v4改进：优化检查逻辑）
     */
    function isVisible(node) {
        let element = node;
        if (node && node.nodeType === Node.TEXT_NODE) {
            element = node.parentElement;
        }

        // 基础检查
        if (element && !(element instanceof Element)) {
            return true;
        }
        if (!element || !document.contains(element)) {
            return false;
        }
        // CSS 检查
        const style = getCachedComputedStyle(element);
        if (style.display === 'none' || style.visibility === 'hidden') {
            return false;
        }
        // 只有完全透明才算不可见
        if (parseFloat(style.opacity) === 0) {
            return false;
        }
        // 尺寸检查
        const rect = getCachedBoundingRect(element);
        if (rect.width < 0.5 && rect.height < 0.5) {
            return false;
        }

        if (rect.width === 0 || rect.height === 0) {
            return false;
        }
        return true;
    }

    function getNodeText(node) {
        if (node.nodeType === Node.TEXT_NODE) {
            return node.textContent.trim();
        }
        if (node.nodeType === Node.ELEMENT_NODE) {
            if (node.tagName === 'META') {
                return node.getAttribute('content') || '';
            }

            const ownText = getOwnTextContent(node);
            if (ownText) {
                return ownText;
            }
        }
        return '';
    }

    function isMeaninglessText(text) {
        const cleanText = text.replace(/[\u200B-\u200D\uFEFF]/g, '').trim();
        return cleanText.length === 0;
    }

    const hideClasses = new Set(['popup-hide', 'popup-anime-hide']);

    function isInputAndNotPassword(element) {
        return ((element.tagName === 'INPUT' && element.type !== 'password') || element.tagName === 'TEXTAREA');
    }

    // ========================
    // == v4改进：基于DOM结构的树状构建 ==
    // ========================
    function getRefinedElementData(element) {
        const rect = getCachedBoundingRect(element);
        let scrollable = isScrollable(element);
        let treeNode = {
            $type: element.tagName ? element.tagName.toLowerCase() : 'text',
            $ID: typeof element.getArkWebDomNodeId === 'function' ? element.getArkWebDomNodeId(element) : 0,
            type: 'web',
            x: rect.left * window.devicePixelRatio || 0,
            y: rect.top * window.devicePixelRatio || 0,
            width: rect.width * window.devicePixelRatio || 0,
            height: rect.height * window.devicePixelRatio || 0,
            $attrs: {
                text: element.text || '',
                xpath: getXPath(element),
                className: element.getAttribute('class') || undefined,
                webid: element.getAttribute('id') || undefined,
                eventListenerTypes: typeof element.getArkWebEventListenerTypes === 'function' ? element.getArkWebEventListenerTypes() : null,
                isScrollable: scrollable,
            },
        }

        if (scrollable) {
            Object.assign(treeNode.$attrs, {
                scrollLeft: element.scrollLeft * window.devicePixelRatio,
                scrollTop: element.scrollTop * window.devicePixelRatio,
                scrollWidth: element.scrollWidth * window.devicePixelRatio,
                scrollHeight: element.scrollHeight * window.devicePixelRatio,
            });
        }
        return treeNode;
    }

    // ========================
    // == v5: Single-pass extraction with viewport culling ==
    // ========================

    const VIEWPORT_MARGIN = 200;

    function isOutsideViewport(rect) {
        return rect.bottom < -VIEWPORT_MARGIN
            || rect.top > window.innerHeight + VIEWPORT_MARGIN
            || rect.right < -VIEWPORT_MARGIN
            || rect.left > window.innerWidth + VIEWPORT_MARGIN;
    }

    const rejectClasses = new Set([
        'hwb-news-nested-list', 'hwb-horizons-more-exploration'
    ]);

    function performDeferredOcclusion(candidates) {
        const interactiveTags = new Set(['A', 'BUTTON', 'INPUT', 'TEXTAREA', 'SELECT', 'LABEL']);
        const toCheck = [];
        for (const c of candidates) {
            if (interactiveTags.has(c.element.tagName)
                || (c.treeNode.$attrs.eventListenerTypes && c.treeNode.$attrs.eventListenerTypes.length > 0)) {
                toCheck.push(c);
            }
        }
        if (toCheck.length > 500) {
            toCheck.sort((a, b) =>
                (b.treeNode.width * b.treeNode.height) - (a.treeNode.width * a.treeNode.height));
            toCheck.length = 500;
        }
        for (const { element, treeNode } of toCheck) {
            treeNode.$attrs.occluded = isOccluded(element);
        }
    }

    function countTreeNodes(children) {
        let count = 0;
        for (const child of children) {
            count++;
            if (child.$children) {
                count += countTreeNodes(child.$children);
            }
        }
        return count;
    }

    function singlePassExtract() {
        const rootChildren = [];
        const xpathSet = new Set();
        const stack = [];
        const occlusionCandidates = [];

        const initialState = {
            semanticRole: null,
            isFixedOrSticky: false,
            isInTable: false,
            hasHideClass: false
        };
        const bodyChildren = document.body.childNodes;
        for (let i = bodyChildren.length - 1; i >= 0; i--) {
            stack.push({
                node: bodyChildren[i],
                parentChildren: rootChildren,
                inherited: initialState
            });
        }

        while (stack.length > 0) {
            const frame = stack.pop();
            const node = frame.node;

            if (node.nodeType === Node.TEXT_NODE) {
                if (frame.inherited.isInTable) continue;
                const text = node.textContent.trim();
                if (!text || text.replace(/[​-‍﻿]/g, '').length === 0) continue;

                frame.parentChildren.push({
                    $type: '$TEXT_NODE$',
                    $ID: typeof node.getArkWebDomNodeId === 'function' ? node.getArkWebDomNodeId(node) : 0,
                    type: 'web',
                    $attrs: { text: text }
                });
                continue;
            }

            if (node.nodeType !== Node.ELEMENT_NODE) continue;

            const element = node;

            if (element.classList) {
                let rejected = false;
                for (const cls of element.classList) {
                    if (rejectClasses.has(cls)) { rejected = true; break; }
                }
                if (rejected) continue;
            }

            const style = getCachedComputedStyle(element);

            if (style.display === 'none'
                || style.visibility === 'hidden'
                || parseFloat(style.opacity) === 0) continue;

            let hasHideClass = frame.inherited.hasHideClass;
            if (!hasHideClass && element.classList) {
                for (const cls of element.classList) {
                    if (hideClasses.has(cls)) { hasHideClass = true; break; }
                }
            }
            if (hasHideClass) continue;

            const selfFixed = (style.position === 'fixed' || style.position === 'sticky')
                || (element.classList && (
                    element.classList.contains('is-fixed')
                    || element.classList.contains('fixed')
                    || element.classList.contains('sticky')
                    || element.classList.contains('Sticky')
                    || element.classList.contains('AppHeader')));
            const isFixedOrSticky = frame.inherited.isFixedOrSticky || selfFixed;

            let rect;
            const isMetaTag = element.tagName === 'TITLE' || element.tagName === 'META';
            if (!isMetaTag) {
                rect = getCachedBoundingRect(element);
            } else {
                rect = { left: 0, top: 0, right: 0, bottom: 0, width: 0, height: 0 };
            }

            if (!isFixedOrSticky && !isMetaTag
                && (!style.transform || style.transform === 'none')
                && isOutsideViewport(rect)) {
                continue;
            }

            if (isSuspiciousShadowDOM(element)) continue;

            if (!isMetaTag) {
                if (rect.width < 0.5 && rect.height < 0.5) continue;
                if (rect.width === 0 || rect.height === 0) continue;
            }

            const role = element.getAttribute('role') || html5ToAriaRole[element.tagName];
            const semanticRole = frame.inherited.semanticRole
                || (role && semanticRoles.has(role) ? role : null);

            if (!isMetaTag && element.tagName !== 'UL') {
                if (!isVisibleByAncestorIntersections(element, semanticRole)) continue;
            }

            const isInTable = frame.inherited.isInTable || element.tagName === 'TABLE';

            const xpath = getXPath(element);
            if (xpath && xpathSet.has(xpath)) continue;
            if (xpath) xpathSet.add(xpath);

            const text = getNodeText(element);
            const scrollable = isScrollable(element);
            const src = element.tagName === 'IMG' ? element.getAttribute('src') : undefined;
            const isInput = isInputAndNotPassword(element);

            const treeNode = {
                $type: element.tagName ? element.tagName.toLowerCase() : 'text',
                $ID: typeof element.getArkWebDomNodeId === 'function' ? element.getArkWebDomNodeId(element) : 0,
                type: 'web',
                x: rect.left * window.devicePixelRatio || 0,
                y: rect.top * window.devicePixelRatio || 0,
                width: rect.width * window.devicePixelRatio || 0,
                height: rect.height * window.devicePixelRatio || 0,
                $attrs: {
                    fixedOrSticky: isFixedOrSticky,
                    fontColor: style.color || '',
                    fontFamily: style.fontFamily || '',
                    fontSize: style.fontSize || '',
                    fontWeight: style.fontWeight || '',
                    occluded: false,
                    text: text || '',
                    xpath: xpath || '',
                    className: element.getAttribute('class') || undefined,
                    eventListenerTypes: typeof element.getArkWebEventListenerTypes === 'function' ? element.getArkWebEventListenerTypes() : null,
                    src,
                    autocomplete: isInput ? element.getAttribute('autocomplete') : undefined,
                    placeholder: isInput ? element.getAttribute('placeholder') : undefined,
                    webid: element.getAttribute('id') || undefined,
                    contenttype: isInput ? element.getAttribute('type') : undefined,
                    value: isInput ? element.value : undefined,
                    optionvalue: (element.tagName === 'SELECT' && element.selectedIndex !== -1) ? element.options[element.selectedIndex].text : undefined,
                    scrollLeft: scrollable ? element.scrollLeft * window.devicePixelRatio : undefined,
                    scrollTop: scrollable ? element.scrollTop * window.devicePixelRatio : undefined,
                    scrollHeight: scrollable ? element.scrollHeight * window.devicePixelRatio : undefined,
                    scrollWidth: scrollable ? element.scrollWidth * window.devicePixelRatio : undefined,
                    isScrollable: scrollable,
                },
                $children: []
            };

            frame.parentChildren.push(treeNode);
            occlusionCandidates.push({ element, treeNode });

            const childState = {
                semanticRole,
                isFixedOrSticky,
                isInTable,
                hasHideClass: false
            };
            const children = element.childNodes;
            for (let i = children.length - 1; i >= 0; i--) {
                stack.push({
                    node: children[i],
                    parentChildren: treeNode.$children,
                    inherited: childState
                });
            }
        }

        performDeferredOcclusion(occlusionCandidates);
        return rootChildren;
    }

    async function singlePassExtractAsync() {
        const rootChildren = [];
        const xpathSet = new Set();
        const stack = [];
        const occlusionCandidates = [];

        const initialState = {
            semanticRole: null,
            isFixedOrSticky: false,
            isInTable: false,
            hasHideClass: false
        };
        const bodyChildren = document.body.childNodes;
        for (let i = bodyChildren.length - 1; i >= 0; i--) {
            stack.push({
                node: bodyChildren[i],
                parentChildren: rootChildren,
                inherited: initialState
            });
        }

        let processedCount = 0;

        while (stack.length > 0) {
            const frame = stack.pop();
            const node = frame.node;

            if (node.nodeType === Node.TEXT_NODE) {
                if (frame.inherited.isInTable) continue;
                const text = node.textContent.trim();
                if (!text || text.replace(/[​-‍﻿]/g, '').length === 0) continue;

                frame.parentChildren.push({
                    $type: '$TEXT_NODE$',
                    $ID: typeof node.getArkWebDomNodeId === 'function' ? node.getArkWebDomNodeId(node) : 0,
                    type: 'web',
                    $attrs: { text: text }
                });
                continue;
            }

            if (node.nodeType !== Node.ELEMENT_NODE) continue;

            processedCount++;
            if (processedCount % 120 === 0) {
                await checkInterrupt();
            }

            const element = node;

            if (element.classList) {
                let rejected = false;
                for (const cls of element.classList) {
                    if (rejectClasses.has(cls)) { rejected = true; break; }
                }
                if (rejected) continue;
            }

            const style = getCachedComputedStyle(element);

            if (style.display === 'none'
                || style.visibility === 'hidden'
                || parseFloat(style.opacity) === 0) continue;

            let hasHideClass = frame.inherited.hasHideClass;
            if (!hasHideClass && element.classList) {
                for (const cls of element.classList) {
                    if (hideClasses.has(cls)) { hasHideClass = true; break; }
                }
            }
            if (hasHideClass) continue;

            const selfFixed = (style.position === 'fixed' || style.position === 'sticky')
                || (element.classList && (
                    element.classList.contains('is-fixed')
                    || element.classList.contains('fixed')
                    || element.classList.contains('sticky')
                    || element.classList.contains('Sticky')
                    || element.classList.contains('AppHeader')));
            const isFixedOrSticky = frame.inherited.isFixedOrSticky || selfFixed;

            let rect;
            const isMetaTag = element.tagName === 'TITLE' || element.tagName === 'META';
            if (!isMetaTag) {
                rect = getCachedBoundingRect(element);
            } else {
                rect = { left: 0, top: 0, right: 0, bottom: 0, width: 0, height: 0 };
            }

            if (!isFixedOrSticky && !isMetaTag
                && (!style.transform || style.transform === 'none')
                && isOutsideViewport(rect)) {
                continue;
            }

            if (isSuspiciousShadowDOM(element)) continue;

            if (!isMetaTag) {
                if (rect.width < 0.5 && rect.height < 0.5) continue;
                if (rect.width === 0 || rect.height === 0) continue;
            }

            const role = element.getAttribute('role') || html5ToAriaRole[element.tagName];
            const semanticRole = frame.inherited.semanticRole
                || (role && semanticRoles.has(role) ? role : null);

            if (!isMetaTag && element.tagName !== 'UL') {
                if (!isVisibleByAncestorIntersections(element, semanticRole)) continue;
            }

            const isInTable = frame.inherited.isInTable || element.tagName === 'TABLE';

            const xpath = getXPath(element);
            if (xpath && xpathSet.has(xpath)) continue;
            if (xpath) xpathSet.add(xpath);

            const text = getNodeText(element);
            const scrollable = isScrollable(element);
            const src = element.tagName === 'IMG' ? element.getAttribute('src') : undefined;
            const isInput = isInputAndNotPassword(element);

            const treeNode = {
                $type: element.tagName ? element.tagName.toLowerCase() : 'text',
                $ID: typeof element.getArkWebDomNodeId === 'function' ? element.getArkWebDomNodeId(element) : 0,
                type: 'web',
                x: rect.left * window.devicePixelRatio || 0,
                y: rect.top * window.devicePixelRatio || 0,
                width: rect.width * window.devicePixelRatio || 0,
                height: rect.height * window.devicePixelRatio || 0,
                $attrs: {
                    fixedOrSticky: isFixedOrSticky,
                    fontColor: style.color || '',
                    fontFamily: style.fontFamily || '',
                    fontSize: style.fontSize || '',
                    fontWeight: style.fontWeight || '',
                    occluded: false,
                    text: text || '',
                    xpath: xpath || '',
                    className: element.getAttribute('class') || undefined,
                    eventListenerTypes: typeof element.getArkWebEventListenerTypes === 'function' ? element.getArkWebEventListenerTypes() : null,
                    src,
                    autocomplete: isInput ? element.getAttribute('autocomplete') : undefined,
                    placeholder: isInput ? element.getAttribute('placeholder') : undefined,
                    webid: element.getAttribute('id') || undefined,
                    contenttype: isInput ? element.getAttribute('type') : undefined,
                    value: isInput ? element.value : undefined,
                    optionvalue: (element.tagName === 'SELECT' && element.selectedIndex !== -1) ? element.options[element.selectedIndex].text : undefined,
                    scrollLeft: scrollable ? element.scrollLeft * window.devicePixelRatio : undefined,
                    scrollTop: scrollable ? element.scrollTop * window.devicePixelRatio : undefined,
                    scrollHeight: scrollable ? element.scrollHeight * window.devicePixelRatio : undefined,
                    scrollWidth: scrollable ? element.scrollWidth * window.devicePixelRatio : undefined,
                    isScrollable: scrollable,
                },
                $children: []
            };

            frame.parentChildren.push(treeNode);
            occlusionCandidates.push({ element, treeNode });

            const childState = {
                semanticRole,
                isFixedOrSticky,
                isInTable,
                hasHideClass: false
            };
            const children = element.childNodes;
            for (let i = children.length - 1; i >= 0; i--) {
                stack.push({
                    node: children[i],
                    parentChildren: treeNode.$children,
                    inherited: childState
                });
            }
        }

        performDeferredOcclusion(occlusionCandidates);
        return rootChildren;
    }

    function wrapDomTree(treeChildren) {
        let htmlElementData = getRefinedElementData(document.documentElement);
        let documentBodyElementData = getRefinedElementData(document.body);
        documentBodyElementData.$children = treeChildren;
        htmlElementData.$children = [documentBodyElementData];
        htmlElementData.$attrs.isUnsafeContent = shouldAbortExtraction;
        htmlElementData.$attrs.url = window.location.href;
        htmlElementData.$attrs.title = document.title;

        const nodeCount = countTreeNodes(treeChildren);
        print({
            type: 'debugLog',
            data: { log: `v5 tree nodes: ${nodeCount}` }
        });

        return {
            $attrs: {
                url: window.location.href,
                title: document.title,
                isUnsafeContent: shouldAbortExtraction
            },
            $child: htmlElementData
        };
    }

    function extractDOMTreeV5() {
        shouldAbortExtraction = false;
        resetExtractionCaches();
        const treeChildren = singlePassExtract();
        return wrapDomTree(treeChildren);
    }

    async function extractDOMTreeV5Async() {
        shouldAbortExtraction = false;
        resetExtractionCaches();
        const treeChildren = await singlePassExtractAsync();
        await checkInterrupt();
        return wrapDomTree(treeChildren);
    }

    // ========================
    // == v4精简输出版本 ==
    // ========================

    /**
     * 全局状态管理
     */
    const ExtractorStateV4 = {
        isFirstRun: true,
        lastDomTree: null,
        changeDetection: {
            observer: null,
            enabled: true
        },
        isAsync: true
    };

    // v003: a single lifecycle/scheduler owns every listener, timer and
    // extraction. Input handlers only update state; expensive extraction is
    // never started from a touch/scroll callback.
    const SchedulerState = {
        active: false,
        generation: 0,
        taskRunning: false,
        pendingInitial: false,
        pendingIncremental: false,
        startTimer: null,
        domReadyHandler: null,
        listeners: []
    };
    let activeTaskGeneration = null;

    function addManagedListener(target, type, handler, options) {
        target.addEventListener(type, handler, options);
        SchedulerState.listeners.push({ target, type, handler, options });
    }

    function removeManagedListeners() {
        for (let index = SchedulerState.listeners.length - 1; index >= 0; index--) {
            const listener = SchedulerState.listeners[index];
            listener.target.removeEventListener(listener.type, listener.handler, listener.options);
        }
        SchedulerState.listeners.length = 0;
    }

    function clearManagedTimer(name) {
        if (SchedulerState[name] !== null) {
            clearTimeout(SchedulerState[name]);
            SchedulerState[name] = null;
        }
    }

    function isCurrentGeneration(generation) {
        return SchedulerState.active && SchedulerState.generation === generation;
    }

    function schedulePendingExtraction() {
        if (!SchedulerState.active || SchedulerState.taskRunning) {
            return;
        }
        if (SchedulerState.pendingInitial) {
            SchedulerState.pendingInitial = false;
            if (ExtractorStateV4.isAsync) {
                void performInitialExtractionV4Async();
            } else {
                performInitialExtractionV4();
            }
            return;
        }
        if (SchedulerState.pendingIncremental) {
            SchedulerState.pendingIncremental = false;
            debounceIncrementalExtractionV4();
        }
    }

    function beginExtractionTask(kind) {
        if (!SchedulerState.active) {
            return null;
        }
        if (SchedulerState.taskRunning) {
            if (kind === 'initial') {
                SchedulerState.pendingInitial = true;
            } else {
                SchedulerState.pendingIncremental = true;
            }
            return null;
        }
        SchedulerState.taskRunning = true;
        activeTaskGeneration = SchedulerState.generation;
        return SchedulerState.generation;
    }

    function finishExtractionTask() {
        SchedulerState.taskRunning = false;
        activeTaskGeneration = null;
        setTimeout(schedulePendingExtraction, 0);
    }

    /**
     * 执行首次完整提取（精简版本v4）
     */
    function performInitialExtractionV4() {
        const generation = beginExtractionTask('initial');
        if (generation === null) {
            return null;
        }
        try {
            const startTime = performance.now();

            // 提取DOM树状结构
            const domTree = extractDOMTreeV5();

            const result = domTree;

            if (!isCurrentGeneration(generation)) {
                return null;
            }

            // 更新状态
            ExtractorStateV4.isFirstRun = false;
            ExtractorStateV4.lastDomTree = domTree;

            const endTime = performance.now();
            const extractionTime = endTime - startTime;

            print({
                type: 'debugLog',
                data: { log: `use time: ${extractionTime.toFixed(3)}ms` }
            });

            // 调用native接口
            if (typeof window._arkWebDomTree !== 'undefined' &&
                typeof window._arkWebDomTree.init === 'function') {
                window._arkWebDomTree.init(JSON.stringify(result));
            } else {
                print({
                    type: 'debugLog',
                    data: { log: `window._arkWebDomTree.init function error` }
                });
            }

            // 启动变化监听
            startChangeObserverV4();

            return result;

        } catch (error) {
            print({
                type: 'debugLog',
                data: { log: `window._arkWebDomTree.init error` }
            });
            return null;
        } finally {
            finishExtractionTask();
        }
    }

    async function performInitialExtractionV4Async() {
        const generation = beginExtractionTask('initial');
        if (generation === null) {
            return;
        }
        try {
            const startTime = performance.now();

            // 提取DOM树状结构
            const domTree = await extractDOMTreeV5Async();

            const result = domTree;

            if (!isCurrentGeneration(generation)) {
                return;
            }

            // 更新状态
            ExtractorStateV4.isFirstRun = false;
            ExtractorStateV4.lastDomTree = domTree;

            const endTime = performance.now();
            const extractionTime = endTime - startTime;

            print({
                type: 'debugLog',
                data: { log: `use time: ${extractionTime.toFixed(3)}ms` }
            });

            // 调用native接口
            if (typeof window._arkWebDomTree !== 'undefined' &&
                typeof window._arkWebDomTree.init === 'function') {
                window._arkWebDomTree.init(JSON.stringify(result));
            } else {
                print({
                    type: 'debugLog',
                    data: { log: `window._arkWebDomTree.init function error` }
                });
            }

            // 启动变化监听
            startChangeObserverV4();

        } catch (error) {
            if (typeof error === 'string' && error === USER_INTERRUPT) {
                InterruptedState.needRequestDomTree = true;
            } else if (typeof error === 'string' && error === TASK_CANCELLED) {
                // remove/restart invalidated this generation; do not report.
            } else {
                print({
                    type: 'debugLog',
                    data: { log: `window._arkWebDomTree.init error` }
                });
            }
        } finally {
            finishExtractionTask();
        }
    }

    /**
     * 执行增量提取（精简版本v4）
     */
    function performIncrementalExtractionV4() {
        const generation = beginExtractionTask('incremental');
        if (generation === null) {
            return null;
        }
        try {
            const startTime = performance.now();

            // 重新提取DOM树状结构
            const currentDomTree = extractDOMTreeV5();
            const result = currentDomTree;

            if (!isCurrentGeneration(generation)) {
                return null;
            }

            // 更新状态
            ExtractorStateV4.lastDomTree = result;

            const endTime = performance.now();
            print({
                type: 'debugLog',
                data: { log: `incremental use time: ${(endTime - startTime).toFixed(3)}ms` }
            });

            // 调用native接口
            if (typeof window._arkWebDomTree !== 'undefined' &&
                typeof window._arkWebDomTree.incremental === 'function') {
                window._arkWebDomTree.incremental(JSON.stringify(result));
            } else {
                print({
                    type: 'debugLog',
                    data: { log: `window._arkWebDomTree.incremental function error` }
                });
            }

            return result;

        } catch (error) {
            print({
                type: 'debugLog',
                data: { log: `window._arkWebDomTree.incremental error` }
            });
            return null;
        } finally {
            finishExtractionTask();
        }
    }

    async function performIncrementalExtractionV4Async() {
        const generation = beginExtractionTask('incremental');
        if (generation === null) {
            return;
        }

        try {
            const startTime = performance.now();

            // 重新提取DOM树状结构
            const currentDomTree = await extractDOMTreeV5Async();
            const result = currentDomTree;

            if (!isCurrentGeneration(generation)) {
                return;
            }

            // 更新状态
            ExtractorStateV4.lastDomTree = currentDomTree;

            const endTime = performance.now();
            print({
                type: 'debugLog',
                data: { log: `incremental use time: ${(endTime - startTime).toFixed(3)}ms` }
            });

            // 调用native接口
            if (typeof window._arkWebDomTree !== 'undefined' &&
                typeof window._arkWebDomTree.incremental === 'function') {
                window._arkWebDomTree.incremental(JSON.stringify(result));
            } else {
                print({
                    type: 'debugLog',
                    data: { log: `window._arkWebDomTree.incremental function error` }
                });
            }

        } catch (error) {
            if (typeof error === 'string' && error === USER_INTERRUPT) {
                InterruptedState.needRequestDomTree = true;
            } else if (typeof error === 'string' && error === TASK_CANCELLED) {
                // remove/restart invalidated this generation; do not report.
            } else {
                print({
                    type: 'debugLog',
                    data: { log: `window._arkWebDomTree.incremental error` }
                });
            }
        } finally {
            finishExtractionTask();
        }
    }

    /**
     * 主动执行提取
     */
    function performGetDomTree() {
        try {
            const startTime = performance.now();

            // 重新提取DOM树状结构
            const currentDomTree = extractDOMTreeV5();

            const result = currentDomTree;

            const endTime = performance.now();
            print({
                type: 'debugLog',
                data: { log: `get dom use time: ${(endTime - startTime).toFixed(3)}ms` }
            });

            return result;

        } catch (error) {
            print({
                type: 'debugLog',
                data: { log: `performGetDomTree error` }
            });
            return null;
        }
    }

    /**
     * 启动DOM变化监听（v4版本）
     */
    function startChangeObserverV4() {
        if (!window.MutationObserver || !ExtractorStateV4.changeDetection.enabled) {
            print({
                type: 'debugLog',
                data: { log: `MutationObserver not available` }
            });
            return;
        }

        // 停止现有监听器
        stopChangeObserverV4();

        const observer = new MutationObserver((mutations) => {
            let hasSignificantChange = false;

            // 预处理变化记录，过滤重要变化
            for (const mutation of mutations) {
                if (mutation.type === 'childList' &&
                    (mutation.addedNodes.length > 0 || mutation.removedNodes.length > 0)) {
                    hasSignificantChange = true;
                    break;
                }
                if (mutation.type === 'characterData' &&
                    mutation.target.textContent.trim().length > 0) {
                    hasSignificantChange = true;
                    break;
                }
            }

            if (hasSignificantChange) {
                debounceIncrementalExtractionV4();
            }
        });

        observer.observe(document.body, {
            childList: true,
            subtree: true,
            attributes: true,
            characterData: true
        });
        ExtractorStateV4.changeDetection.observer = observer;
    }

    /**
     * 停止DOM变化监听（v4版本）
     */
    function stopChangeObserverV4() {
        if (ExtractorStateV4.changeDetection.observer) {
            ExtractorStateV4.changeDetection.observer.disconnect();
            ExtractorStateV4.changeDetection.observer = null;
        }
    }

    /**
     * 手动触发提取（v4版本）
     * @param {boolean} forceFull 是否强制完整提取
     */
    function triggerExtractionV4(forceFull = false) {
        if (ExtractorStateV4.isFirstRun || forceFull) {
            return performInitialExtractionV4();
        } else {
            performIncrementalExtractionV4();
        }
    }

    /**
     * 手动触发提取（v4版本）
     * @param {boolean} forceFull 是否强制完整提取
     */
    function triggerExtractionV4Async(forceFull = false) {
        if (ExtractorStateV4.isFirstRun || forceFull) {
            performInitialExtractionV4Async();
        } else {
            performIncrementalExtractionV4Async();
        }
    }

    /**
     * 重置提取器状态（v4版本）
     */
    function resetExtractorV4() {
        SchedulerState.active = false;
        SchedulerState.generation++;
        SchedulerState.pendingInitial = false;
        SchedulerState.pendingIncremental = false;
        clearManagedTimer('startTimer');
        if (debouncedTimer !== null) {
            clearTimeout(debouncedTimer);
            debouncedTimer = null;
        }
        if (interruptScrollEndTimer !== null) {
            clearTimeout(interruptScrollEndTimer);
            interruptScrollEndTimer = null;
        }
        removeManagedListeners();

        ExtractorStateV4.isFirstRun = true;
        ExtractorStateV4.lastDomTree = null;
        ExtractorStateV4.isAsync = true;
        InterruptedState.isScrolling = false;
        InterruptedState.isScrollingInterrupted = false;
        InterruptedState.shouldInterrupted = false;
        InterruptedState.needRequestDomTree = false;
        stopChangeObserverV4();
    }

    function scrollEndReport(event) {
        if (ExtractorStateV4.isAsync && InterruptedState.isScrollingInterrupted) {
            return;
        }
        let target = null;
        const targetElement = event.target;
        let top;
        let left;
        if (targetElement === document) {
            // document.scrollingElement 是现代浏览器标准
            top = targetElement.scrollingElement.scrollTop * window.devicePixelRatio;
            left = targetElement.scrollingElement.scrollLeft * window.devicePixelRatio;
            target = targetElement.scrollingElement;
        }

        // 情况 2: target 是普通 Element (如 div, ul 等局部滚动容器)
        // 只有 Element 节点才有 scrollTop 属性
        if (targetElement instanceof Element) {
            top = targetElement.scrollTop * window.devicePixelRatio;
            left = targetElement.scrollLeft * window.devicePixelRatio;
            target = targetElement;
        }

        if (target == null) {
            return;
        }

        let id = typeof target.getArkWebDomNodeId === 'function' ? target.getArkWebDomNodeId(target) : -1;
        let result = {
            $ID: id,
            scrollLeft: left,
            scrollTop: top,
        };
        if (typeof window._arkWebDomTree !== 'undefined' &&
            typeof window._arkWebDomTree.scrollInfo === 'function') {
            window._arkWebDomTree.scrollInfo(JSON.stringify(result));
        } else {
            print({
                type: 'debugLog',
                data: { log: `window._arkWebDomTree.scrollInfo function error` }
            });

        }
    }

    // 滚动过程中异步打断报错
    const USER_INTERRUPT = 'USER_INTERRUPT';
    const TASK_CANCELLED = 'TASK_CANCELLED';

    // 滚动过程中中断相关状态
    const InterruptedState = {
        isScrolling: false,
        isScrollingInterrupted: false,
        shouldInterrupted: false,
        needRequestDomTree: false
    }

    let lastInterruptedCheckTime = 0;
    async function checkInterrupt(threshold = 16) {
        if (!SchedulerState.active || activeTaskGeneration !== SchedulerState.generation) {
            throw TASK_CANCELLED;
        }
        const now = performance.now();

        // 只有距离上次检查超过阈值时，才真正释放主线程
        if (now - lastInterruptedCheckTime > threshold) {
            // 核心：释放主线程，让浏览器执行 touchstart 回调
            await new Promise(resolve => setTimeout(resolve, 0));
            lastInterruptedCheckTime = performance.now();

            // 感知并抛出异常
            if (InterruptedState.shouldInterrupted) {
                throw USER_INTERRUPT;
            }
        }
    }

    const interruptTouchStart = () => {
        InterruptedState.shouldInterrupted = true;
        if (InterruptedState.isScrolling || SchedulerState.taskRunning) {
            InterruptedState.isScrollingInterrupted = true;
        }
    }

    const interruptTouchEnd = () => {
        // 手指抬起时，开启一个短延时判断，如果没触发scroll，就还原状态
        clearTimeout(interruptScrollEndTimer);
        interruptScrollEndTimer = setTimeout(interruptScrollEnd, 150);
    }

    const interruptScrolling = () => {
        InterruptedState.shouldInterrupted = true;
        InterruptedState.isScrolling = true;

        // 150ms 没动静就强制还原
        clearTimeout(interruptScrollEndTimer);
        interruptScrollEndTimer = setTimeout(interruptScrollEnd, 150);
    }

    let interruptScrollEndTimer = null;
    const interruptScrollEnd = () => {
        InterruptedState.shouldInterrupted = false;

        clearTimeout(interruptScrollEndTimer);
        interruptScrollEndTimer = null;
        if (InterruptedState.needRequestDomTree) {
            InterruptedState.needRequestDomTree = false;
            if (ExtractorStateV4.isFirstRun) {
                SchedulerState.pendingInitial = true;
                schedulePendingExtraction();
            } else {
                debounceIncrementalExtractionV4();
            }
        }

        InterruptedState.isScrolling = false;
        InterruptedState.isScrollingInterrupted = false;
    };

    let debouncedTimer = null;
    function debounceIncrementalExtractionV4() {
        // True trailing-edge debounce. Every meaningful mutation/force call
        // moves the deadline, and scrolling never starts extraction inline.
        if (!SchedulerState.active) {
            return;
        }
        if (InterruptedState.shouldInterrupted || InterruptedState.isScrolling) {
            InterruptedState.needRequestDomTree = true;
            return;
        }
        if (debouncedTimer !== null) {
            clearTimeout(debouncedTimer);
        }
        const generation = SchedulerState.generation;
        debouncedTimer = setTimeout(() => {
            debouncedTimer = null;
            if (!isCurrentGeneration(generation)) {
                return;
            }
            if (SchedulerState.taskRunning) {
                SchedulerState.pendingIncremental = true;
                return;
            }
            if (ExtractorStateV4.isAsync) {
                void performIncrementalExtractionV4Async();
            } else {
                performIncrementalExtractionV4();
            }
        }, 500);
    }

    // ========================
    // == 自动执行v4 ==
    // ========================
    function activateExtractor(isAsync) {
        if (SchedulerState.active) {
            resetExtractorV4();
        }

        SchedulerState.active = true;
        SchedulerState.generation++;
        ExtractorStateV4.isAsync = isAsync;
        const generation = SchedulerState.generation;
        const start = () => {
            if (!isCurrentGeneration(generation)) {
                return;
            }
            SchedulerState.startTimer = null;
            if (isAsync) {
                void triggerExtractionV4Async();
            } else {
                triggerExtractionV4();
            }
        };

        if (document.readyState === 'loading') {
            addManagedListener(document, 'DOMContentLoaded', start, { once: true });
        } else {
            SchedulerState.startTimer = setTimeout(start, 100);
        }

        addManagedListener(document, 'scrollend', scrollEndReport, true);
        if (isAsync) {
            addManagedListener(window, 'touchstart', interruptTouchStart, { passive: true });
            addManagedListener(window, 'touchend', interruptTouchEnd, { passive: true });
            addManagedListener(window, 'scroll', interruptScrolling, { passive: true });
            addManagedListener(window, 'scrollend', interruptScrollEnd, false);
        }
        addManagedListener(window, 'beforeunload', resetExtractorV4, { once: true });
    }

    window.__arkWebDomTree = {
        addDomTreeReported: function () {
            activateExtractor(false);
        },
        addDomTreeReportedAsync: function () {
            activateExtractor(true);
        },
        removeDomTreeReported: function () {
            resetExtractorV4();
        },
        getWebDom: performGetDomTree,
        forceReport: debounceIncrementalExtractionV4,
    }

    window.__arkwebDomTreeStatisticLoaded = true;

})();
