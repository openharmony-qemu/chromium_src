/**
 * page_scene_query_source.js - PageScene input control query script for ArkWeb
 *
 * Exposes window.__pageSceneQuery with one method:
 *   request(ruleJsonStr, ruleId, nodeTypesStr) - Query controls, start observer if not running
 *
 * Observer watches for ANY DOM change (childList + attributes + characterData).
 * When change detected, reports errorCode=3 to C++, which re-triggers request() after delay.
 *
 * Query result format: { errorCode: number, controls: [...] }
 * Error codes: 0 = success, 2 = exception, 3 = page changed (re-query needed)
 *
 * Observer callback: window.ArkWebPageSceneReady.onDomReady(result, ruleJson)
 */
(function () {
    if (window.__pageSceneQueryLoaded) {
        return;
    }
    window.__pageSceneQueryLoaded = true;

    // ========== XPath helper ==========
    function getXPath(el) {
        if (!el || el.nodeType !== 1) {
            return '';
        }
        if (el.id) {
            const id = el.id;
            const hasSingle = id.indexOf("'") !== -1;
            const hasDouble = id.indexOf('"') !== -1;
            if (!hasSingle) {
                return "//*[@id='" + id + "']";
            }
            if (!hasDouble) {
                return '//*[@id="' + id + '"]';
            }
            const parts = id.split("'");
            const concatArgs = parts.map(function (p) {
                return '"' + p + '"';
            }).join(",'\"',");
            return '//*[@id=concat(' + concatArgs + ')]';
        }
        const parts = [];
        while (el && el.nodeType === 1) {
            let index = 1;
            let sib = el.previousElementSibling;
            while (sib) {
                if (sib.tagName === el.tagName) {
                    index++;
                }
                sib = sib.previousElementSibling;
            }
            let hasSimilarSiblings = false;
            let nextSib = el.nextElementSibling;
            while (nextSib) {
                if (nextSib.tagName === el.tagName) {
                    hasSimilarSiblings = true;
                    break;
                }
                nextSib = nextSib.nextElementSibling;
            }
            const tag = el.tagName.toLowerCase();
            const segment = (index > 1 || hasSimilarSiblings) ? (tag + '[' + index + ']') : tag;
            parts.unshift(segment);
            el = el.parentNode;
        }
        return '/' + parts.join('/');
    }

    // ========== Query sub-functions ==========
    function matchAttrRules(el, nodeType, attrRules) {
        if (!attrRules || attrRules.length === 0) {
            return true;
        }
        for (let r = 0; r < attrRules.length; r++) {
            const rule = attrRules[r];
            if (!rule || !rule.value) {
                continue;
            }
            let attrVal = el.getAttribute(rule.attr);
            if (rule.attr === 'type' && nodeType === 'input' && !attrVal) {
                attrVal = 'text';
            }
            if (attrVal && rule.value.indexOf(attrVal) !== -1) {
                return true;
            }
        }
        return false;
    }

    const hideClassesSet = { 'popup-hide': true, 'popup-anime-hide': true };

    function hasClassInSet(el, set) {
        if (!el.classList) {
            return false;
        }
        for (let i = 0; i < el.classList.length; i++) {
            if (set[el.classList[i]]) {
                return true;
            }
        }
        return false;
    }

    // rectTooSmall: width/height <= 0 covers zero and negative; both < 0.5 covers tiny-but-nonzero rects
    function rectTooSmall(rect) {
        return rect.width <= 0 || rect.height <= 0 || (rect.width < 0.5 && rect.height < 0.5);
    }

    function hasIntersectionRect(rectA, rectB) {
        if (rectA.width <= 0 || rectA.height <= 0) {
            return false;
        }
        const x1 = Math.max(rectA.left, rectB.left);
        const y1 = Math.max(rectA.top, rectB.top);
        const x2 = Math.min(rectA.right, rectB.right);
        const y2 = Math.min(rectA.bottom, rectB.bottom);
        const iw = x2 - x1;
        const ih = y2 - y1;
        if (iw <= 0 || ih <= 0) {
            return false;
        }
        return (iw * ih) / (rectA.width * rectA.height) >= 0.9;
    }

    function hasIntersectionXOnlyRect(rectA, rectB) {
        if (rectA.width <= 0) {
            return false;
        }
        const left = Math.max(rectA.left, rectB.left);
        const right = Math.min(rectA.right, rectB.right);
        if (left >= right) {
            return false;
        }
        return (right - left) / rectA.width >= 0.9;
    }

    // isClippingAncestor: true if the ancestor's style could clip el's visible rect.
    // Transformed ancestors are skipped (transform establishes a new containing block, not a clip).
    function isClippingAncestor(style) {
        const transform = style.transform;
        if (transform && transform !== 'none') {
            return false;
        }
        const hasOverflowClip = style.overflow !== 'visible' || style.overflowX !== 'visible' || style.overflowY !== 'visible';
        const hasFilter = style.filter !== 'none';
        const hasClipPath = style.clipPath !== 'none' && style.clipPath !== '';
        const hasContainLayout = /layout|strict/.test(style.contain);
        const hasMask = style.mask !== 'none' || style.webkitMaskImage !== 'none';
        return hasOverflowClip || hasFilter || hasClipPath || hasContainLayout || hasMask;
    }

    // walkAncestorsForVisibility: single ancestor walk — one getComputedStyle + one getBoundingClientRect per node.
    // Returns null if any ancestor makes el invisible; otherwise returns { hasHideClass, hasBannerRole, clippingAncestors }.
    function walkAncestorsForVisibility(el) {
        let node = el;
        let hasHideClass = false;
        let hasBannerRole = false;
        const clippingAncestors = [];
        while (node && node.nodeType === 1) {
            if (!hasHideClass && hasClassInSet(node, hideClassesSet)) {
                hasHideClass = true;
            }
            if (!hasBannerRole) {
                const role = node.getAttribute && node.getAttribute('role');
                if (role === 'banner' || node.tagName === 'HEADER') {
                    hasBannerRole = true;
                }
            }
            const style = window.getComputedStyle(node);
            if (style.display === 'none' || style.visibility === 'hidden' || parseFloat(style.opacity) === 0) {
                return null;
            }
            const rect = node.getBoundingClientRect();
            if (rectTooSmall(rect)) {
                // Align with dom_tree_extractor: if the node has children, its rect may be
                // zero in one dimension (e.g. flex/transition container with height=0) while
                // children are still visible. Only skip if it has no children.
                if (!node.children.length) {
                    return null;
                }
            }
            // Skip el itself for clipping check; only ancestors clip el's visible area.
            if (node !== el && isClippingAncestor(style)) {
                clippingAncestors.push(rect);
            }
            node = node.parentElement;
        }
        return { hasHideClass: hasHideClass, hasBannerRole: hasBannerRole, clippingAncestors: clippingAncestors };
    }

    // isOversizedShadowRoot: true when el has a shadow root covering most of the viewport.
    function isOversizedShadowRoot(el, elRect) {
        const hasShadow = !!el.shadowRoot || (typeof el.hasArkWebShadowRoot === 'function' && el.hasArkWebShadowRoot());
        return hasShadow && elRect.width > window.innerWidth * 0.8 && elRect.height > window.innerHeight * 0.8;
    }

    // passesClippingCheck: verifies el's rect remains visible through every clipping ancestor.
    // Uses area-based intersection when hasBannerRole is true (banner may legitimately span full width),
    // x-axis-only intersection otherwise.
    function passesClippingCheck(elRect, clippingAncestors, hasBannerRole) {
        for (let i = 0; i < clippingAncestors.length; i++) {
            const ancestorRect = clippingAncestors[i];
            const intersects = hasBannerRole
                ? hasIntersectionRect(elRect, ancestorRect)
                : hasIntersectionXOnlyRect(elRect, ancestorRect);
            if (!intersects) {
                return false;
            }
        }
        return true;
    }

    // isVisible: returns true if el is visible to the user. Composes ancestor walk, shadow check,
    // and clipping intersection check. Split into helpers to keep cyclomatic complexity under 20.
    function isVisible(el) {
        if (!el) {
            return false;
        }
        const ancestorInfo = walkAncestorsForVisibility(el);
        if (!ancestorInfo) {
            return false;
        }
        if (ancestorInfo.hasHideClass) {
            return false;
        }
        const elRect = el.getBoundingClientRect();
        if (isOversizedShadowRoot(el, elRect)) {
            return false;
        }
        if (el.tagName === 'UL') {
            return true;
        }
        return passesClippingCheck(elRect, ancestorInfo.clippingAncestors, ancestorInfo.hasBannerRole);
    }

    function isFocusable(el, nodeType, includeUnfocusable) {
        if (includeUnfocusable) {
            return true;
        }
        if (nodeType === 'input' || nodeType === 'textarea') {
            if (el.disabled || el.tabIndex < 0) {
                return false;
            }
        }
        return true;
    }

    function isEditable(el, nodeType) {
        if (el.readOnly || el.disabled) {
            return false;
        }
        if (nodeType === 'input' || nodeType === 'textarea') {
            return true;
        }
        if (el.contentEditable === 'true') {
            return true;
        }
        return false;
    }

    function buildControl(el, nodeType, config) {
        const control = {};
        control.nodeId = typeof el.getArkWebDomNodeId === 'function'
            ? el.getArkWebDomNodeId(el) : 0;
        control.nodeType = nodeType;
        control.focusable = !el.disabled && el.tabIndex >= 0;
        control.editable = isEditable(el, nodeType);

        const rect = el.getBoundingClientRect();
        const dpr = window.devicePixelRatio || 0;
        control.rect = {
            x: rect.left * dpr,
            y: rect.top * dpr,
            width: rect.width * dpr,
            height: rect.height * dpr
        };
        if (nodeType === 'input' && el.type === 'password') {
            control.text = '';
        } else {
            control.text = el.value || el.textContent || '';
        }

        if (config.includeAutocomplete) {
            const isInputNoPassword = (nodeType === 'input' && el.type !== 'password') || nodeType === 'textarea';
            control.autocomplete = isInputNoPassword ? (el.getAttribute('autocomplete') || '') : '';
        }

        if (config.includeXpath) {
            control.xpath = getXPath(el);
        }

        return control;
    }

    // Safe getter: returns [] if config.typeAttrRules or a nodeType key is missing
    function getAttrRules(config, nodeType) {
        return (config && config.typeAttrRules && config.typeAttrRules[nodeType]) || [];
    }

    // ========== Core query function ==========

    function queryControls(selectorConfigStr) {
        let config;
        try {
            config = JSON.parse(selectorConfigStr);
        } catch (e) {
            return JSON.stringify({ errorCode: 2, controls: [], error: e.message || String(e) });
        }
        const results = [];

        config.nodeTypes.forEach(function (nodeType) {
            const elements = document.querySelectorAll(nodeType);
            elements.forEach(function (el) {
                if (!matchAttrRules(el, nodeType, getAttrRules(config, nodeType))) {
                    return;
                }
                if (!isFocusable(el, nodeType, config.includeUnfocusableTextInput)) {
                    return;
                }
                if (config.onlyVisible && !isVisible(el)) {
                    return;
                }
                results.push(buildControl(el, nodeType, config));
            });
        });

        return JSON.stringify({ errorCode: 0, controls: results });
    }

    // ========== Observer management ==========

    if (!window.__pageSceneObservers) {
        window.__pageSceneObservers = {};
    }

    let isScrolling = false;
    let scrollEndTimer = null;
    const scrollEndDelay = 200;
    const mutationDebounceMs = 200;
    const observerTimeoutMs = 3500;
    const scrollCheckMaxRetries = 15;

    function stopObserver(ruleId) {
        if (!window.__pageSceneObservers) {
            return;
        }
        const entry = window.__pageSceneObservers[ruleId];
        if (!entry) {
            return;
        }
        if (entry.observer) {
            entry.observer.disconnect();
        }
        if (entry.debounceTimer) {
            clearTimeout(entry.debounceTimer);
        }
        if (entry.timeoutId) {
            clearTimeout(entry.timeoutId);
        }
        delete window.__pageSceneObservers[ruleId];
    }

    function reportResult(resultStr, selectorJsonStr) {
        if (typeof window.ArkWebPageSceneReady !== 'undefined') {
            window.ArkWebPageSceneReady.onDomReady(resultStr, selectorJsonStr);
        }
    }

    let lastScrollReset = 0;

    function onScroll() {
        if (!isScrolling) {
            isScrolling = true;
        }
        const now = Date.now();
        if (lastScrollReset && now - lastScrollReset < 50) {
            return;
        }
        lastScrollReset = now;
        if (scrollEndTimer) {
            clearTimeout(scrollEndTimer);
            scrollEndTimer = null;
        }
        scrollEndTimer = setTimeout(function () {
            isScrolling = false;
        }, scrollEndDelay);
    }

    function onScrollEnd() {
        if (scrollEndTimer) {
            clearTimeout(scrollEndTimer);
            scrollEndTimer = null;
        }
        scrollEndTimer = setTimeout(function () {
            isScrolling = false;
        }, scrollEndDelay);
    }

    function setupScrollListener() {
        window.addEventListener('scroll', onScroll, { passive: true });
        window.addEventListener('touchmove', onScroll, { passive: true });
        window.addEventListener('touchend', onScrollEnd, { passive: true });
        window.addEventListener('touchcancel', onScrollEnd, { passive: true });
        window.addEventListener('mouseup', onScrollEnd);
    }

    function setupObserver(selectorConfigStr, selectorJsonStr, ruleId) {
        if (!window.__pageSceneObservers) {
            window.__pageSceneObservers = {};
        }
        if (window.__pageSceneObservers[ruleId]) {
            return;
        }

        const observer = new MutationObserver(function (mutations) {
            if (!window.__pageSceneObservers) {
                return;
            }
            const entry = window.__pageSceneObservers[ruleId];
            if (!entry) {
                return;
            }
            if (entry.debounceTimer) {
                clearTimeout(entry.debounceTimer);
            }
            let scrollRetries = 0;
            function notifyChanged() {
                if (isScrolling && scrollRetries < scrollCheckMaxRetries) {
                    scrollRetries++;
                    entry.debounceTimer = setTimeout(notifyChanged, scrollEndDelay);
                    return;
                }
                reportResult(JSON.stringify({ errorCode: 3, controls: [] }), selectorJsonStr);
                entry.debounceTimer = null;
                stopObserver(ruleId);
            }
            entry.debounceTimer = setTimeout(notifyChanged, mutationDebounceMs);
        });

        observer.observe(document.documentElement, {
            childList: true, subtree: true,
            attributes: true, characterData: true
        });

        const timeoutId = setTimeout(function () {
            stopObserver(ruleId);
        }, observerTimeoutMs);

        const entry = {
            observer: observer,
            debounceTimer: null,
            timeoutId: timeoutId
        };

        window.__pageSceneObservers[ruleId] = entry;
    }

    // ========== Public API ==========
    window.__pageSceneQuery = {
        request: function (ruleJsonStr, ruleId, nodeTypesStr) {
            let nodeTypes;
            try {
                nodeTypes = JSON.parse(nodeTypesStr);
            } catch (e) {
                return JSON.stringify({ errorCode: 2, controls: [], error: 'nodeTypesStr parse: ' + (e.message || String(e)) });
            }
            const resultStr = queryControls(ruleJsonStr);
            let parsed;
            try {
                parsed = JSON.parse(resultStr);
            } catch (e) {
                return resultStr;
            }
            if (nodeTypes && nodeTypes.length > 0) {
                if (document.readyState === 'loading') {
                    document.addEventListener('DOMContentLoaded', function () {
                        setupObserver(ruleJsonStr, ruleJsonStr, ruleId);
                    });
                } else {
                    setupObserver(ruleJsonStr, ruleJsonStr, ruleId);
                }
            }
            return resultStr;
        }
    };

    setupScrollListener();
})();
