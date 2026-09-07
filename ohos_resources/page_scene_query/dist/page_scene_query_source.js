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

    function onScrollStart() {}

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
                return JSON.stringify({ errorCode: 2, controls: [], error: 'nodeTypesStr parse: ' + e.message });
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
