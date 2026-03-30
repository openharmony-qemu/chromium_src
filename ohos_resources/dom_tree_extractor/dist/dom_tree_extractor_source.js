/**
 * dom_tree_extractor_source.js - ArkWeb DOM树状结构提取
 *
 * 主要功能：
 * - 保持完整树状结构提取
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
            data: {log: 'dom tree script has loaded'}
        });
        return;
    }


// ========================
// == 原始extractor.js核心函数（v4改进版） ==
// ========================
    /**
     * 从原始extractor.js改进 - extractAllUniqueNodes函数
     * 提取所有可见的DOM节点，确保无重复
     */

// ========================
// == 工具函数定义 ==
// ========================

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

        const segments = [];
        let current = node;

        while (current && current.nodeType === Node.ELEMENT_NODE) {
            // 计算节点在兄弟节点中的索引
            let index = 1;
            let sibling = current.previousElementSibling;
            while (sibling) {
                if (sibling.nodeName === current.nodeName) {
                    index++;
                }
                sibling = sibling.previousElementSibling;
            }

            // 生成路径段
            const tag = current.nodeName.toLowerCase();
            const hasSimilarSiblings = current.nextElementSibling?.nodeName === current.nodeName;
            const segment = index > 1 || hasSimilarSiblings ? `${tag}[${index}]` : tag;

            segments.unshift(segment);
            current = current.parentNode;
        }

        return segments.length ? `/${segments.join('/')}` : null;
    }

    function isScrollable(element, direction = 'both') {
        if (!element) {
            return false;
        }

        // 获取计算后的样式
        const computedStyle = window.getComputedStyle(element);
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
            const rect = element.getBoundingClientRect();
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

    function extractAllUniqueNodes() {
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
         * 剔除节点数组被包含的节点通过判断位置和宽高
         * @param {nodes} rectA 目标矩形
         * @returns {nodes}
         */
        function filterCovered(nodes) {
            if (nodes.length <= 1) {
                return nodes;
            }

            function getRect(node) {
                const r = node.getBoundingClientRect();
                return {left: r.left, top: r.top, right: r.right, bottom: r.bottom};
            }

            function contains(a, b) {
                return (
                    a.left <= b.left &&
                    a.top <= b.top &&
                    a.right >= b.right &&
                    a.bottom >= b.bottom
                );
            }

            function recurse(list, i = 0) {
                if (i >= list.length) {
                    return list;
                }
                const rectA = getRect(list[i]);
                for (let j = 0; j < list.length; j++) {
                    if (i === j) {
                        continue;
                    }
                    const rectB = getRect(list[j]);
                    // 如果 A 被 B 包含，去掉 A
                    if (contains(rectB, rectA)) {
                        list.splice(i, 1);
                        return recurse(list, i);
                    }
                }
                return recurse(list, i + 1);
            }

            return recurse([...nodes]);
        }

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

        // 判断是否有 transform
        function isTransformed(node) {
            const transform = window.getComputedStyle(node).transform;
            return transform && transform !== 'none';
        }

        function isClippingNode(node) {
            if (node === document.body || node === document.documentElement) {
                return false;
            }

            const style = window.getComputedStyle(node);

            // 判断 overflow 类型
            const hasOverflowClip =
                style.overflow !== 'visible' ||
                style.overflowX !== 'visible' ||
                style.overflowY !== 'visible';

            // 判断 transform
            const hasTransform = style.transform !== 'none';

            // 判断 filter
            const hasFilter = style.filter !== 'none';

            // 判断 clip-path
            const hasClipPath = style.clipPath !== 'none' && style.clipPath !== '';

            // contain: layout 或 strict
            const hasContainLayout = /layout|strict/.test(style.contain);

            // mask
            const hasMask = style.mask !== 'none' || style.webkitMaskImage !== 'none';

            return (
                hasOverflowClip ||
                hasTransform ||
                hasFilter ||
                hasClipPath ||
                hasContainLayout ||
                hasMask
            );
        }

        function hasTableAncestorRecursive(node, filter) {
            // 如果当前节点没有父节点，说明已经到达根节点，没有找到
            if (!node.parentNode) {
                return false;
            }
            // 检查当前父节点是否是 <table>
            const parent = node.parentNode;
            if (parent.tagName && parent.tagName.toLowerCase() === filter) {
                return true;
            }

            // 递归检查父节点的父节点
            return hasTableAncestorRecursive(parent, filter);
        }

        function hasClassInAncestors(node, className = 'harmony-author-intro') {
            let current = node;
            while (current) {
                if (current.classList && current.classList.contains(className)) {
                    return true;
                }
                current = current.parentElement;
            }
            return false;
        }

        /**
         * 判断节点是否在其祖先节点的可见区域内（未被裁剪）。
         * @param {Node} node 当前节点
         * @param {WeakMap} semanticMap 语义区域映射表
         * @returns {boolean}
         */
        function isVisibleByAncestorIntersections(node, semanticMap) {
            let element;

            if (node.nodeType === Node.TEXT_NODE) {
                element = node.parentElement;
            } else if (node.nodeType === Node.ELEMENT_NODE) {
                element = node;
            } else {
                return false;
            }

            const targetRect = element.getBoundingClientRect();
            const semantic = semanticMap.get(element);

            let current = element.parentElement;

            while (current && current !== document.body && current !== document.documentElement) {
                if (isTransformed(current)) {
                    current = current.parentElement;
                    continue;
                }

                if (isClippingNode(current)) {
                    const ancestorRect = current.getBoundingClientRect();
                    const intersection = 'banner' === semantic ? hasIntersection(targetRect, ancestorRect) : hasIntersectionXOnly(targetRect, ancestorRect);

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

        // ========================
        // == v4改进：唯一节点提取逻辑 ==
        // ========================

        const nodes = [];
        const processedNodes = new Set(); // v4新增：记录已处理的节点，避免重复

        // 第一步：提取所有文本节点
        function getTextNodes(node) {
            let tmpTextNodes = [];
            const textWalker = document.createTreeWalker(
                node,
                NodeFilter.SHOW_TEXT,
                {
                    acceptNode: (node) => {
                        if (hasTableAncestorRecursive(node, 'table')) {
                            return NodeFilter.FILTER_SKIP;
                        }
                        const cleanText = getOwnTextContent(node).trim().replace(/[\u200B-\u200D\uFEFF]/g, '');
                        const isMeaningTextValue = !isMeaninglessText(cleanText);
                        return node.nodeType === Node.TEXT_NODE && isMeaningTextValue ? NodeFilter.FILTER_ACCEPT : NodeFilter.FILTER_SKIP;
                    }
                }
            );
            while (textWalker.nextNode()) {
                const textNode = textWalker.currentNode;
                // v4改进：检查是否已经处理过这个文本节点
                if (!processedNodes.has(textNode)) {
                    tmpTextNodes.push(textNode);
                    processedNodes.add(textNode);
                }
            }
            return tmpTextNodes;
        }

        const textNodes = getTextNodes(document.body);

        // 第二步：提取所有可见的元素节点
        function getAllVisibleElements(node) {
            let tmpElements = [];
            const elementWalker = document.createTreeWalker(
                node,
                NodeFilter.SHOW_ELEMENT,
                {
                    acceptNode: (node) => {
                        return node.nodeType === Node.ELEMENT_NODE ? NodeFilter.FILTER_ACCEPT : NodeFilter.FILTER_SKIP;
                    }
                }
            );
            while (elementWalker.nextNode()) {
                const element = elementWalker.currentNode;
                // v4改进：检查是否已经处理过这个元素节点
                if (!processedNodes.has(element)) {
                    tmpElements.push(element);
                    processedNodes.add(element);
                }
            }
            return tmpElements;
        }

        const allElements = getAllVisibleElements(document.body);

        // 第三步：构建语义区域映射表
        function buildSemanticMap() {
            const semanticMap = new WeakMap();

            const walker = document.createTreeWalker(
                document.body,
                NodeFilter.SHOW_ELEMENT,
                {
                    acceptNode: (node) => {
                        const role = node.getAttribute('role') || html5ToAriaRole[node.tagName];
                        if (role && semanticRoles.has(role)) {
                            return NodeFilter.FILTER_ACCEPT;
                        }
                        return NodeFilter.FILTER_SKIP;
                    }
                }
            );

            while (walker.nextNode()) {
                const element = walker.currentNode;
                const role = element.getAttribute('role') || html5ToAriaRole[element.tagName];

                const subWalker = document.createTreeWalker(
                    element,
                    NodeFilter.SHOW_ELEMENT | NodeFilter.SHOW_TEXT,
                    null
                );

                while (subWalker.nextNode()) {
                    const child = subWalker.currentNode;
                    if (!semanticMap.has(child)) {
                        semanticMap.set(child, role);
                    }
                }
            }

            return semanticMap;
        }

        function isOccluded(element) {
            // 检查元素是否存在且可见
            if (!element || !element.getBoundingClientRect) {
                return false;
            }
            const rect = element.getBoundingClientRect();
            // 如果元素尺寸为0，认为不可见
            if (rect.width === 0 || rect.height === 0) {
                return true;
            }
            // 检查元素的样式
            const computedStyle = window.getComputedStyle(element);
            if (computedStyle.display === 'none' ||
                computedStyle.visibility === 'hidden' ||
                computedStyle.opacity === '0') {
                return true;
            }
            // 获取5个检测点坐标：中心 + 九宫格中的上下左右中点
            const points = [
                {x: rect.left + rect.width / 2, y: rect.top + rect.height / 2},
                {x: rect.left + rect.width * 0.25, y: rect.top + rect.height * 0.25},
                {x: rect.left + rect.width * 0.25, y: rect.top + rect.height * 0.75},
                {x: rect.left + rect.width * 0.75, y: rect.top + rect.height * 0.25},
                {x: rect.left + rect.width * 0.75, y: rect.top + rect.height * 0.75}
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
                const occludingStyle = window.getComputedStyle(topElement);
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
            const style = window.getComputedStyle(element);
            if (style.display === 'none' || style.visibility === 'hidden') {
                return false;
            }
            // 只有完全透明才算不可见
            if (parseFloat(style.opacity) === 0) {
                return false;
            }
            // 尺寸检查
            const rect = element.getBoundingClientRect();
            if (rect.width < 0.5 && rect.height < 0.5) {
                return false;
            }

            if (rect.width === 0 || rect.height === 0) {
                return false;
            }
            return true;
        }

        function buildFixedOrStickyMap() {
            const map = new WeakMap();

            const walker = document.createTreeWalker(
                document.body,
                NodeFilter.SHOW_ELEMENT,
                {
                    acceptNode: (node) => {
                        const style = window.getComputedStyle(node);
                        if (['fixed', 'sticky'].includes(style.position)) {
                            return NodeFilter.FILTER_ACCEPT;
                        }
                        if (['is-fixed', 'fixed', 'sticky', 'Sticky', 'AppHeader'].some(cls => node.classList.contains(cls))) {
                            return NodeFilter.FILTER_ACCEPT;
                        }
                        return NodeFilter.FILTER_SKIP;
                    }
                }
            );

            while (walker.nextNode()) {
                const fixedOrStickyNode = walker.currentNode;

                const subWalker = document.createTreeWalker(
                    fixedOrStickyNode,
                    NodeFilter.SHOW_ELEMENT | NodeFilter.SHOW_TEXT,
                    null
                );

                while (subWalker.nextNode()) {
                    const child = subWalker.currentNode;
                    if (!map.has(child)) {
                        map.set(child, fixedOrStickyNode);
                    }
                }
            }

            return map;
        }

        /**
         * 构建 hiddenMap
         */
        function buildHiddenMap() {
            const hiddenMap = new WeakMap();

            const walker = document.createTreeWalker(
                document.body,
                NodeFilter.SHOW_ELEMENT,
                {
                    acceptNode: (node) => {
                        const style = window.getComputedStyle(node);
                        const rect = node.getBoundingClientRect();

                        const isHiddenByDisplay = style.display === 'none';
                        const isHiddenByVisibility = style.visibility === 'hidden';
                        const isHiddenByOpacity = parseFloat(style.opacity) === 0;

                        if (isHiddenByDisplay || isHiddenByVisibility || isHiddenByOpacity) {
                            return NodeFilter.FILTER_ACCEPT;
                        }

                        return NodeFilter.FILTER_SKIP;
                    }
                }
            );

            while (walker.nextNode()) {
                const hiddenNode = walker.currentNode;

                const subWalker = document.createTreeWalker(
                    hiddenNode,
                    NodeFilter.SHOW_ELEMENT | NodeFilter.SHOW_TEXT,
                    null
                );

                while (subWalker.nextNode()) {
                    const child = subWalker.currentNode;
                    if (!hiddenMap.has(child)) {
                        hiddenMap.set(child, hiddenNode);
                    }
                }
            }

            return hiddenMap;
        }

        const semanticMap = buildSemanticMap();
        const fixedOrStickyMap = buildFixedOrStickyMap();
        const hiddenMap = buildHiddenMap();

        // 第四步：v4改进 - 唯一节点过滤和转换
        const acceptedNodes = new Set([...textNodes, ...allElements]);
        const nodeXPathMap = new Map(); // v4新增：用于检测重复xpath
        const nodeIdNodeMap = new Map();
        const filteredGlobalWalker = document.createTreeWalker(
            document.body,
            NodeFilter.SHOW_ALL,
            {
                acceptNode: (node) => {
                    // 减少过滤条件
                    if (node.classList && (node.classList.contains('hwb-news-nested-list') ||
                        node.classList.contains('hwb-horizons-more-exploration'))) {
                        return NodeFilter.FILTER_REJECT;
                    }
                    return acceptedNodes.has(node) && isVisibleByAncestorIntersections(node, semanticMap)
                        ? NodeFilter.FILTER_ACCEPT
                        : NodeFilter.FILTER_SKIP;
                },
            }
        );

        // 第五步：转换节点为标准对象
        function isAncestorFixedOrSticky(node, map) {
            return map.has(node);
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

        function isHiddenByClass(node) {
            const hideClasses = ['popup-hide', 'popup-anime-hide'];
            let current = node;
            while (current && current !== document.body) {
                if (current.classList) {
                    if ([...current.classList].some(cls => hideClasses.includes(cls))) {
                        return true;
                    }
                }
                current = current.parentElement;
            }
            return false;
        }

        function isInputAndNotPassword(element) {
            return ((element.tagName === 'INPUT' && element.type !== 'password') || element.tagName === 'TEXTAREA');
        }

        function convertNode(node, element, cache) {
            if (node.nodeType === Node.TEXT_NODE) {
                const nodeData = {
                    text: node.textContent.trim(),
                    tagName: '$TEXT_NODE$',
                    element: element,
                    node: node,
                    domNodeId: typeof node.getArkWebDomNodeId === 'function' ? node.getArkWebDomNodeId(node) : null,
                };
                // TOOD: No xpath, should add node id map.
                nodes.push(nodeData);
            } else {
                let text = getNodeText(node);
                const computedStyle = window.getComputedStyle(element);

                let rect;
                if (node.nodeType === Node.TEXT_NODE) {
                    rect = getTextNodeRect(node);
                } else if (!['TITLE', 'META'].includes(element.tagName)) {
                    rect = element.getBoundingClientRect();
                } else {
                    rect = {left: 0, top: 0, width: 0, height: 0};
                }

                const isVisible =
                    // rect.width > 0 &&
                    // rect.height > 0 &&
                    (node.nodeType === Node.TEXT_NODE || element.offsetParent !== null || computedStyle.position === 'fixed') &&
                    computedStyle.visibility === 'visible' &&
                    parseFloat(computedStyle.opacity) > 0;

                // 判断是否被父节点隐藏
                const isHiddenByParent = hiddenMap.has(element) || hiddenMap.has(node);

                if (isHiddenByParent || isHiddenByClass(element) || !isVisible) {
                    return;
                }

                if (isSuspiciousShadowDOM(element)) {
                    return;
                }

                // v4改进：生成唯一标识符
                const xpath = getXPath(element);
                const uniqueId = `${element.tagName}_${xpath}_${element.textContent.substring(0, 20)}`;

                // Export img src.
                const src = (element.tagName === 'IMG') ? element.getAttribute('src') : undefined;

                const autocomplete = isInputAndNotPassword(element) ? element.getAttribute('autocomplete') : undefined;
                const placeholder = isInputAndNotPassword(element) ? element.getAttribute('placeholder') : undefined;
                const contenttype = isInputAndNotPassword(element) ? element.getAttribute('type') : undefined;
                const inputvalue = isInputAndNotPassword(element) ? element.value : undefined;
                const optionvalue = (element.tagName === 'SELECT') ? element.options[element.selectedIndex].text : undefined;

                const scrollable = isScrollable(element);

                // v4改进：检查是否已经处理过相同xpath的元素
                if (nodeXPathMap.has(xpath)) {
                    return;
                }

                nodeXPathMap.set(xpath, true);

                const nodeData = {
                    text: text,
                    x: rect.left * window.devicePixelRatio,
                    y: rect.top * window.devicePixelRatio,
                    width: rect.width * window.devicePixelRatio,
                    height: rect.height * window.devicePixelRatio,
                    tagName: element.tagName,
                    fontSize: computedStyle.fontSize,
                    fontColor: computedStyle.color,
                    fontWeight: computedStyle.fontWeight,
                    fontFamily: computedStyle.fontFamily,
                    fixedOrSticky: isAncestorFixedOrSticky(element, cache.fixedOrStickyMap),
                    xpath: xpath,
                    occluded: isOccluded(element),
                    element: element,
                    node: node,
                    uniqueId: uniqueId,  // v4新增：唯一标识符
                    className: element.getAttribute('class') || undefined,
                    eventListenerTypes: typeof element.getArkWebEventListenerTypes === 'function' ? element.getArkWebEventListenerTypes() : null,
                    domNodeId: typeof element.getArkWebDomNodeId === 'function' ? element.getArkWebDomNodeId(element) : null,
                    src,
                    autocomplete,
                    placeholder,
                    webid: element.getAttribute('id') || undefined,
                    contenttype,
                    inputvalue,
                    optionvalue,
                    scrollTop: scrollable ? element.scrollTop * window.devicePixelRatio : undefined,
                    scrollLeft: scrollable ? element.scrollLeft * window.devicePixelRatio : undefined,
                    scrollHeight: scrollable ? element.scrollHeight * window.devicePixelRatio : undefined,
                    scrollWidth: scrollable ? element.scrollWidth * window.devicePixelRatio : undefined,
                    isScrollable: scrollable,
                };

                nodes.push(nodeData);
            }
        }

        // 第六步：遍历并转换节点
        while (filteredGlobalWalker.nextNode()) {
            const node = filteredGlobalWalker.currentNode;
            let element = node.nodeType === Node.TEXT_NODE ? node.parentElement : node;

            convertNode(node, element, {
                fixedOrStickyMap,
                semanticMap
            });
        }

        // Step 7: Filter those invisible subtree.

        // 步骤1: 建立父子关系映射 O(n)
        const childrenMap = new Map(); // parentNode -> Set of children
        const parentMap = new Map();   // childNode -> parentNode
        const nodeDataMap = new Map(); // node -> nodeData

        // 初始化映射
        nodes.forEach(nodeData => {
            const node = nodeData.node;
            nodeDataMap.set(node, nodeData);

            // 获取父节点
            const parentNode = node.parentNode;
            if (parentNode) {
                parentMap.set(node, parentNode);

                if (!childrenMap.has(parentNode)) {
                    childrenMap.set(parentNode, new Set());
                }
                childrenMap.get(parentNode).add(node);
            }
        });

        // 步骤2: 找出所有叶子节点 O(n)
        const toRemove = new Set();
        const leafNodes = [];

        nodes.forEach(nodeData => {
            const node = nodeData.node;
            const children = childrenMap.get(node);

            // 如果没有子节点或子节点集合为空，则为叶子节点
            if (!children || children.size === 0) {
                leafNodes.push(node);
            }
        });

        // 步骤3: 使用队列处理叶子节点 O(n)
        const queue = [...leafNodes];

        while (queue.length > 0) {
            const node = queue.shift();

            // 检查是否可见
            if (!isVisible(node)) {
                toRemove.add(node);

                // 从父节点的子节点集合中移除
                const parentNode = parentMap.get(node);
                if (parentNode) {
                    const siblings = childrenMap.get(parentNode);
                    if (siblings) {
                        siblings.delete(node);

                        // 如果父节点现在变成了叶子节点，加入队列
                        if (siblings.size === 0 && !isVisible(parentNode)) {
                            queue.push(parentNode);
                        }
                    }
                }
            }
        }

        // 步骤4: 过滤数组 O(n)
        return nodes.filter(nodeData => !toRemove.has(nodeData.node));
    }

    function getRefinedElementData(element) {
        const rect = element.getBoundingClientRect();
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
// == v4改进：基于DOM结构的树状构建 ==
// ========================

    /**
     * 基于实际DOM结构构建树状结构（v4改进版）
     * @param {Array} flatNodes 扁平化的节点数组
     * @returns {Object} DOM树状结构
     */
    function buildDOMTreeFromDOMV4(flatNodes) {

        // v4改进：使用Map来确保唯一性，避免重复
        const nodeElementMap = new Map();
        const nodeIdMap = new Map();
        const processedNodes = new Set(); // v4新增：记录已处理的元素
        flatNodes.forEach(nodeData => {
            // v4改进：检查元素是否已经被处理
            if (!processedNodes.has(nodeData.node)) {
                nodeElementMap.set(nodeData.xpath, nodeData);
                nodeIdMap.set(nodeData.domNodeId, nodeData);
                processedNodes.add(nodeData.node);
            }
        });

        // 收集所有有效的DOM元素
        const allElements = Array.from(processedNodes);

        // v4改进：更智能的根节点识别
        let rootNodes = [];
        document.body.childNodes.forEach((node) => {
            if (typeof node.getArkWebDomNodeId === 'function') {
                let nodeId = typeof node.getArkWebDomNodeId === 'function' ? node.getArkWebDomNodeId() : -1;
                if (nodeIdMap.has(nodeId)) {
                    rootNodes.push(nodeIdMap.get(nodeId));
                }
            } else {
                let xpath = getXPath(node);
                if (xpath && nodeElementMap.get(xpath)) {
                    rootNodes.push(nodeElementMap.get(xpath));
                }
            }
        })

        // v4改进：递归构建树状结构，确保唯一性和完整性
        function buildTreeNodeV4(nodeData) {
            if (!nodeData.element || !processedNodes.has(nodeData.element)) {
                return null;
            }

            const element = nodeData.element;
            const node = nodeData.node;
            const children = [];

            // v4改进：查找所有直接子元素，确保唯一性
            const childNodes = Array.from(node.childNodes).filter(childNode => {
                return processedNodes.has(childNode);
            });

            childNodes.forEach(childNode => {
                // 查找对应的节点数据
                let childNodeData = null;

                // First using domNodeId.
                let childNodeId = typeof childNode.getArkWebDomNodeId === 'function' ? childNode.getArkWebDomNodeId() : -1;
                childNodeData = nodeIdMap.get(childNodeId);

                if (childNodeData) {
                    const childTreeNode = buildTreeNodeV4(childNodeData);
                    if (childTreeNode) {
                        children.push(childTreeNode);
                    }
                } else {
                    // 如果没有找到对应的节点数据，记录警告但不创建回退节点
                    print({
                        type: 'debugLog',
                        data: {log: `child not find data, tag : ${childNode.tagName}`}
                    });
                }
            });

            // v4改进：构建树节点
            let treeNode = null;
            if (node.nodeType === Node.TEXT_NODE) {
                treeNode = {
                    $type: nodeData.tagName,
                    $ID: nodeData.domNodeId || 0,
                    type: 'web',
                    $attrs: {
                        text: nodeData.text || '',
                    }
                    // No children as expecting
                }
            } else {
                treeNode = {
                    $type: element.tagName ? element.tagName.toLowerCase() : 'text',
                    $ID: nodeData.domNodeId || 0,
                    type: 'web',
                    x: nodeData.x || 0,
                    y: nodeData.y || 0,
                    width: nodeData.width || 0,
                    height: nodeData.height || 0,
                    $attrs: {
                        fixedOrSticky: nodeData.fixedOrSticky || false,
                        fontColor: nodeData.fontColor || '',
                        fontFamily: nodeData.fontFamily || '',
                        fontSize: nodeData.fontSize || '',
                        fontWeight: nodeData.fontWeight || '',
                        occluded: nodeData.occluded || false,
                        text: nodeData.text || '',
                        xpath: nodeData.xpath || '',
                        className: nodeData.className,
                        eventListenerTypes: nodeData.eventListenerTypes || null,
                        src: nodeData.src,
                        autocomplete: nodeData.autocomplete,
                        placeholder: nodeData.placeholder,
                        webid: nodeData.webid,
                        contenttype: nodeData.contenttype,
                        value: nodeData.inputvalue,
                        optionvalue: nodeData.optionvalue,
                        scrollLeft: nodeData.scrollLeft,
                        scrollTop: nodeData.scrollTop,
                        scrollHeight: nodeData.scrollHeight,
                        scrollWidth: nodeData.scrollWidth,
                        isScrollable: nodeData.isScrollable,
                    },
                    $children: children
                };
            }

            return treeNode;
        }

        // 构建最终的树状结构
        const treeChildren = [];
        const builtNodes = new Set(); // v4新增：记录已构建的节点
        rootNodes.forEach(rootNode => {
            // v4改进：确保根节点也是唯一的
            if (!builtNodes.has(rootNode.uniqueId)) {
                const treeNode = buildTreeNodeV4(rootNode);
                if (treeNode) {
                    treeChildren.push(treeNode);
                    builtNodes.add(rootNode.uniqueId);
                }
            }
        });

        // Append htmlElement and bodyElement.

        let htmlElementData = getRefinedElementData(document.documentElement);
        let documentBodyElementData = getRefinedElementData(document.body);
        documentBodyElementData.$children = treeChildren;
        htmlElementData.$children = [documentBodyElementData];

        htmlElementData.$attrs.isUnsafeContent = shouldAbortExtraction;

        const domTree = {
            $attrs: {
                url: window.location.href,
                title: document.title,
                // 【新增】将安全标志位暴露给 MSDP
                isUnsafeContent: shouldAbortExtraction
            },
            $child: htmlElementData
        };

        // v4改进：验证树状结构的唯一性和完整性
        function validateTreeStructureV4(node, depth = 0) {
            let nodeCount = 1;
            if (depth > 10) {
                print({
                    type: 'debugLog',
                    data: {log: `tree is too deep may contain circular references`}
                });
                return nodeCount;
            }

            if (node.$children) {
                node.$children.forEach(child => {
                    nodeCount += validateTreeStructureV4(child, depth + 1);
                });
            }

            return nodeCount;
        }

        let totalTreeNodes = 0;
        treeChildren.forEach(rootNode => {
            totalTreeNodes += validateTreeStructureV4(rootNode);
        });
        print({
            type: 'debugLog',
            data: {log: `tree node : ${totalTreeNodes}，origin node: ${flatNodes.length}`}
        });

        const completeness = (totalTreeNodes / flatNodes.length * 100).toFixed(1);

        return domTree;
    }

    /**
     * 提取DOM树状结构的主函数（v4版本）
     * @returns {Object} DOM树状结构
     */
    function extractDOMTreeV4() {
        // 【新增】每次提取前重置报警状态
        shouldAbortExtraction = false;

        // 首先获取扁平化的节点信息
        const flatNodes = extractAllUniqueNodes();

        // 构建树状结构
        const domTree = buildDOMTreeFromDOMV4(flatNodes);

        return domTree;
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
        }
    };

    /**
     * 执行首次完整提取（精简版本v4）
     */
    function performInitialExtractionV4() {
        try {
            const startTime = performance.now();

            // 提取DOM树状结构
            const domTree = extractDOMTreeV4();

            const result = domTree;

            // 更新状态
            ExtractorStateV4.isFirstRun = false;
            ExtractorStateV4.lastDomTree = domTree;

            const endTime = performance.now();
            const extractionTime = endTime - startTime;

            print({
                type: 'debugLog',
                data: {log: `use time: ${extractionTime.toFixed(3)}ms`}
            });

            // 调用native接口
            if (typeof window._arkWebDomTree !== 'undefined' &&
                typeof window._arkWebDomTree.init === 'function') {
                window._arkWebDomTree.init(JSON.stringify(result));
            } else {
                print({
                    type: 'debugLog',
                    data: {log: `window._arkWebDomTree.init function error`}
                });
            }

            // 启动变化监听
            startChangeObserverV4();

            return result;

        } catch (error) {
            print({
                type: 'debugLog',
                data: {log: `window._arkWebDomTree.init error`}
            });
            return null;
        }
    }

    /**
     * 执行增量提取（精简版本v4）
     */
    function performIncrementalExtractionV4() {
        try {
            const startTime = performance.now();

            // 重新提取DOM树状结构
            const currentDomTree = extractDOMTreeV4();

            const result = currentDomTree;

            // 更新状态
            ExtractorStateV4.lastDomTree = currentDomTree;

            const endTime = performance.now();
            print({
                type: 'debugLog',
                data: {log: `incremental use time: ${(endTime - startTime).toFixed(3)}ms`}
            });

            // 调用native接口
            if (typeof window._arkWebDomTree !== 'undefined' &&
                typeof window._arkWebDomTree.incremental === 'function') {
                window._arkWebDomTree.incremental(JSON.stringify(result));
            } else {
                print({
                    type: 'debugLog',
                    data: {log: `window._arkWebDomTree.incremental function error`}
                });
            }

            return result;

        } catch (error) {
            print({
                type: 'debugLog',
                data: {log: `window._arkWebDomTree.incremental error`}
            });
            return null;
        }
    }

    /**
     * 主动执行提取
     */
    function performGetDomTree() {
        try {
            const startTime = performance.now();

            // 重新提取DOM树状结构
            const currentDomTree = extractDOMTreeV4();

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
                data: {log: `MutationObserver not available`}
            });
            return;
        }

        // 停止现有监听器
        stopChangeObserverV4();

        let timeoutId = null;

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
                // 防抖处理，避免频繁触发
                if (!timeoutId) {
                    timeoutId = setTimeout(() => {
                        performIncrementalExtractionV4();
                        timeoutId = null;
                    }, 500);
                }
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
            return performIncrementalExtractionV4();
        }
    }

    /**
     * 重置提取器状态（v4版本）
     */
    function resetExtractorV4() {
        ExtractorStateV4.isFirstRun = true;
        ExtractorStateV4.lastDomTree = null;
        stopChangeObserverV4();
    }

    function scrollEndReport(event) {
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
                data: {log: `window._arkWebDomTree.scrollInfo function error`}
            });

        }
    }


// ========================
// == 自动执行v4 ==
// ========================

    window.__arkWebDomTree = {
        addDomTreeReported: function () {
            // 确保DOM加载完成
            if (document.readyState === 'loading') {
                document.addEventListener('DOMContentLoaded', function () {
                    triggerExtractionV4();
                });
            } else {
                // 页面已加载，延迟执行
                setTimeout(() => {
                    triggerExtractionV4();
                }, 100);
            }

            document.addEventListener('scrollend', scrollEndReport, true);

            window.addEventListener('beforeunload', function () {
                stopChangeObserverV4();
                document.removeEventListener('scrollend', scrollEndReport, true);
            });
        },
        getWebDom: performGetDomTree,
        forceReport: performIncrementalExtractionV4
    }

    window.__arkwebDomTreeStatisticLoaded = true;

})();