/**
 * page_scene_query_source.js - PageScene input control query script for ArkWeb
 *
 * Exposes window.__pageSceneQuery with one method:
 *   request(ruleJsonStr, ruleId, nodeTypesStr) - Query controls, optionally observe if nodeTypes non-empty
 *
 * nodeTypes empty []  -> query only, return result via IIFE
 * nodeTypes non-empty -> query; if no results, set MutationObserver; observer results via onDomReady
 *
 * Query result format: { errorCode: number, controls: [...] }
 * Error codes: 0 = success, 2 = exception, 3 = DOM pending
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
        const parts = [];
        while (el && el.nodeType === 1) {
            let idx = 0;
            let sib = el.previousSibling;
            while (sib) {
                if (sib.nodeType === 1 && sib.tagName === el.tagName) {
                    idx++;
                }
                sib = sib.previousSibling;
            }
            const tag = el.tagName.toLowerCase();
            parts.unshift(tag + (idx > 0 ? '[' + (idx + 1) + ']' : ''));
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

    function isVisible(el) {
        const rect = el.getBoundingClientRect();
        if (rect.width === 0 && rect.height === 0) {
            return false;
        }
        const style = window.getComputedStyle(el);
        if (style.display === 'none' || style.visibility === 'hidden' || style.opacity === '0') {
            return false;
        }
        return true;
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

    function buildControl(el, nodeType, config) {
        const control = {};
        control.nodeId = typeof el.getArkWebDomNodeId === 'function'
            ? el.getArkWebDomNodeId(el) : 0;
        control.nodeType = nodeType;
        control.focusable = !el.disabled && el.tabIndex >= 0;
        control.editable = (nodeType === 'input' || nodeType === 'textarea' || el.contentEditable === 'true');

        const rect = el.getBoundingClientRect();
        const dpr = window.devicePixelRatio || 0;
        control.rect = {
            x: rect.left * dpr,
            y: rect.top * dpr,
            width: rect.width * dpr,
            height: rect.height * dpr
        };
        control.text = el.value || el.textContent || '';

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
            return JSON.stringify({ errorCode: 2, controls: [], error: e.message });
        }
        const results = [];

        config.nodeTypes.forEach(function (nodeType) {
            const elements = document.querySelectorAll(nodeType);
            elements.forEach(function (el) {
                if (!matchAttrRules(el, nodeType, getAttrRules(config, nodeType))) {
                    return;
                }
                if (config.onlyVisible && !isVisible(el)) {
                    return;
                }
                if (!isFocusable(el, nodeType, config.includeUnfocusableTextInput)) {
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

    // Scroll detection state
    let isScrolling = false;
    let scrollEndTimer = null;
    const scrollEndDelay = 200;
    const mutationDebounceMs = 200;

    function stopObserver(ruleId) {
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

    function nodeMatchesNodeTypes(node, nodeTypes) {
        for (let k = 0; k < nodeTypes.length; k++) {
            if (node.tagName && node.tagName.toLowerCase() === nodeTypes[k]) {
                return true;
            }
            const descendants = node.getElementsByTagName ? node.getElementsByTagName(nodeTypes[k]) : [];
            if (descendants.length > 0) {
                return true;
            }
        }
        return false;
    }

    function mutationMatchesNodeTypes(mutations, nodeTypes) {
        for (let i = 0; i < mutations.length; i++) {
            const mutation = mutations[i];
            if (mutation.type !== 'childList') {
                continue;
            }
            const addedNodes = mutation.addedNodes;
            for (let j = 0; j < addedNodes.length; j++) {
                if (nodeMatchesNodeTypes(addedNodes[j], nodeTypes)) {
                    return true;
                }
            }
        }
        return false;
    }

    // Scroll event handlers — only manage isScrolling state
    // touchstart/mousedown do NOT set isScrolling: a tap without scroll is not scrolling.
    // Only actual scroll/touchmove sets isScrolling = true.
    function onScrollStart() {
        // no-op: tap is not scroll
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
        window.addEventListener('touchstart', onScrollStart, { passive: true });
        window.addEventListener('mousedown', onScrollStart);
        window.addEventListener('scroll', onScroll, { passive: true });
        window.addEventListener('touchmove', onScroll, { passive: true });
        window.addEventListener('touchend', onScrollEnd, { passive: true });
        window.addEventListener('touchcancel', onScrollEnd, { passive: true });
        window.addEventListener('mouseup', onScrollEnd);
    }

    const scrollCheckMaxRetries = 15;
    const observerTimeoutMs = 2000;

    function setupObserver(selectorConfigStr, selectorJsonStr, ruleId, nodeTypes) {
        if (!nodeTypes || nodeTypes.length === 0) {
            return;
        }

        const observer = new MutationObserver(function (mutations) {
            if (!mutationMatchesNodeTypes(mutations, nodeTypes)) {
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
            function check() {
                if (isScrolling && scrollRetries < scrollCheckMaxRetries) {
                    scrollRetries++;
                    entry.debounceTimer = setTimeout(check, scrollEndDelay);
                    return;
                }
                entry.debounceTimer = null;
                stopObserver(ruleId);
                const result = queryControls(selectorConfigStr);
                const controlCount = JSON.parse(result).controls.length;
                console.log('[PageScene] MutationObserver: ruleId=' + ruleId +
                    ' controls=' + controlCount +
                    (isScrolling ? ' (force after max retries)' : ''));
                reportResult(result, selectorJsonStr);
            }
            entry.debounceTimer = setTimeout(check, mutationDebounceMs);
        });

        observer.observe(document.documentElement, { childList: true, subtree: true });

        const timeoutId = setTimeout(function () {
            console.log('[PageScene] Observer timeout: ruleId=' + ruleId);
            stopObserver(ruleId);
            const result = queryControls(selectorConfigStr);
            const controlCount = JSON.parse(result).controls.length;
            if (controlCount > 0) {
                reportResult(result, selectorJsonStr);
            }
        }, observerTimeoutMs);

        const entry = {
            observer: observer,
            debounceTimer: null,
            timeoutId: timeoutId
        };

        window.__pageSceneObservers[ruleId] = entry;
    }

    function onDomReadyForObserver(selectorConfigStr, selectorJsonStr, ruleId, nodeTypes) {
        const result = queryControls(selectorConfigStr);
        const parsed = JSON.parse(result);
        if (parsed.controls.length > 0) {
            console.log('[PageScene] DOMContentLoaded: ruleId=' + ruleId +
                ' controls=' + parsed.controls.length);
            reportResult(result, selectorJsonStr);
        } else {
            setupObserver(selectorConfigStr, selectorJsonStr, ruleId, nodeTypes);
        }
    }

    function startObserver(selectorConfigStr, selectorJsonStr, ruleId, nodeTypes) {
        stopObserver(ruleId);
        if (document.readyState === 'loading') {
            document.addEventListener('DOMContentLoaded', function () {
                setTimeout(function () {
                    onDomReadyForObserver(selectorConfigStr, selectorJsonStr, ruleId, nodeTypes);
                }, 100);
            });
            return;
        }
        setupObserver(selectorConfigStr, selectorJsonStr, ruleId, nodeTypes);
    }

    // ========== Public API ==========
    window.__pageSceneQuery = {
        request: function (ruleJsonStr, ruleId, nodeTypesStr) {
            const nodeTypes = JSON.parse(nodeTypesStr);
            const result = queryControls(ruleJsonStr);
            const parsed = JSON.parse(result);
            console.log('[PageScene] request: ruleId=' + ruleId +
                ' controls=' + parsed.controls.length);
            if (parsed.controls.length === 0 && nodeTypes && nodeTypes.length > 0) {
                startObserver(ruleJsonStr, ruleJsonStr, ruleId, nodeTypes);
            }
            return result;
        }
    };

    // Initialize scroll listener once
    setupScrollListener();
})();
