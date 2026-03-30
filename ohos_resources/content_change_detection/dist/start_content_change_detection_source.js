(function(window) {
    'use strict';

    const print = (message) => {
        window.ArkWebContentChangeDetector.reportContentChange(message);
    };

    if (window.__viewportFontStatisticLoaded) {
        print({
            type: 'debugLog',
            data: { log: 'Viewport font statistic script already loaded, skipping duplicate execution' }
        });
        return;
    }
    window.__viewportFontStatisticLoaded = true;

    const _document = window.document;
    const _MutationObserver = window.MutationObserver;
    const _getComputedStyle = window.getComputedStyle;
    const _setTimeout = window.setTimeout;
    const _clearTimeout = window.clearTimeout;
    const _setInterval = window.setInterval;
    const _clearInterval = window.clearInterval;

    const DEFAULT_CONFIG = {
        minTextLength: 2,
        changeThreshold: 0.15,
        reportInterval: 100,
        debounceDelay: 50,
        scrollEndDelay: 200,
        orientationEndDelay: 300,
        clickEndDelay: 200,
        maxBufferSize: 20,
        maxTextCacheSize: 1000,
        maxSnapshotCount: 5
    };

    class ViewportSnapshot {
        constructor(monitor) {
            this.monitor = monitor;
            this.snapshots = new Map();
            this.snapshotOrder = [];
        }

        createSnapshot(snapshotType) {
            const elements = this.monitor.getViewportElements();
            const viewportArea = this.monitor.viewportArea;

            const snapshot = {
                timestamp: Date.now(),
                type: snapshotType,
                viewportArea: viewportArea,
                elements: [],
                totalTextArea: 0,
                elementMap: new Map()
            };

            elements.forEach(element => {
                const key = this.monitor.getElementKey(element);
                if (key && key !== 'error') {
                    const area = this.monitor.calculateElementArea(element);
                    if (area > 0) {
                        const elementData = {
                            key: key,
                            area: area,
                            textContent: this.monitor.getCleanText(element)
                        };

                        snapshot.elements.push(elementData);
                        snapshot.elementMap.set(key, elementData);
                        snapshot.totalTextArea += area;
                    }
                }
            });

            print({
                type: 'debugLog',
                data: {
                    log: `Create ${snapshotType} snapshot: ${snapshot.elements.length} elements, total area ${snapshot.totalTextArea.toFixed(0)}px²`
                }
            });

            this.manageSnapshots(snapshotType, snapshot);
            return snapshot;
        }

        manageSnapshots(snapshotType, snapshot) {
            if (this.snapshots.has(snapshotType)) {
                this.snapshots.delete(snapshotType);
                const index = this.snapshotOrder.indexOf(snapshotType);
                if (index > -1) {
                    this.snapshotOrder.splice(index, 1);
                }
            }

            this.snapshots.set(snapshotType, snapshot);
            this.snapshotOrder.push(snapshotType);

            while (this.snapshotOrder.length > this.monitor.config.maxSnapshotCount) {
                const oldestType = this.snapshotOrder.shift();
                this.snapshots.delete(oldestType);
            }
        }

        getSnapshot(snapshotType) {
            return this.snapshots.get(snapshotType);
        }

        clearSnapshot(snapshotType) {
            if (snapshotType) {
                this.snapshots.delete(snapshotType);
                const index = this.snapshotOrder.indexOf(snapshotType);
                if (index > -1) {
                    this.snapshotOrder.splice(index, 1);
                }
            } else {
                this.snapshots.clear();
                this.snapshotOrder.length = 0;
            }
        }

        compareSnapshots(startSnapshot, endSnapshot, source) {
            if (!startSnapshot || !endSnapshot) {
                return 0;
            }
            const currentViewportArea = endSnapshot.viewportArea || this.monitor.viewportArea;
            if (currentViewportArea <= 0) {
                return 0;
            }

            const startMap = startSnapshot.elementMap;
            const endMap = endSnapshot.elementMap;
            let disappearedArea = 0;
            let newAppearedArea = 0;
            let matchedElements = new Set();

            for (const [startKey, startData] of startMap) {
                if (endMap.has(startKey)) {
                    matchedElements.add(startKey);
                } else {
                    disappearedArea += startData.area;
                }
            }

            for (const [endKey, endData] of endMap) {
                if (!matchedElements.has(endKey)) {
                    newAppearedArea += endData.area;
                }
            }

            disappearedArea = Math.max(0, disappearedArea);
            newAppearedArea = Math.max(0, newAppearedArea);

            const maxDisappeared = Math.min(disappearedArea, currentViewportArea);
            const maxNewAppeared = Math.min(newAppearedArea, currentViewportArea);

            let visualChangeArea;
            if (maxDisappeared === 0 && maxNewAppeared === 0) {
                visualChangeArea = 0;
            } else if (maxDisappeared > 0 && maxNewAppeared > 0) {
                visualChangeArea = (maxDisappeared + maxNewAppeared) / 2;
            } else {
                visualChangeArea = Math.max(maxDisappeared, maxNewAppeared);
            }
            visualChangeArea = Math.min(visualChangeArea, currentViewportArea);
            let changeRate = currentViewportArea > 0 ? visualChangeArea / currentViewportArea : 0;
            changeRate = Math.max(0, Math.min(1, changeRate));
            const sourceText = source === 'scroll' ? 'scroll' : source === 'click' ? 'click' : 'mutationObserver';
            print({
                type: 'debugLog',
                data: {
                    log: `Comparison result(${sourceText}): viewport ${currentViewportArea.toFixed(0)}px² | ` +
                        `disappeared ${disappearedArea.toFixed(0)}px² | ` +
                        `new appeared ${newAppearedArea.toFixed(0)}px² | ` +
                        `matched ${matchedElements.size} elements | ` +
                        `ratio ${(changeRate * 100).toFixed(2)}%`
                }
            });

            return changeRate;
        }
    }

    class FontChangeMonitor {
        constructor(changeThreshold, reportInterval) {
            this.config = {
                ...DEFAULT_CONFIG,
                changeThreshold: (() => {
                    let value = changeThreshold !== undefined ? changeThreshold : DEFAULT_CONFIG.changeThreshold;
                    if (value < 0) {
                        return 0;
                    } else if (value > 100) {
                        return 100;
                    }
                    return value;
                })(),
                reportInterval: reportInterval !== undefined ? reportInterval : DEFAULT_CONFIG.reportInterval
            };

            this.registry = new Map();
            this.formValues = new WeakMap();
            this.formDebounceMap = new WeakMap();
            this.viewportArea = this.calculateViewportArea();
            this.snapshotManager = new ViewportSnapshot(this);

            this.isProcessing = false;
            this.isFirstLoad = true;
            this.isScrolling = false;
            this.scrollTimer = null;
            this.scrollStartPosition = { x: 0, y: 0 };
            this.isClickHandling = false;
            this.clickTimer = null;

            this.eventHandlers = {
                handleScrollStart: null,
                handleScroll: null,
                handleTouchend: null,
                handleMouseup: null,
                handleClickStart: null,
                handleClickEnd: null,
                handleDragStart: null,
                handleFormChange: null,
                handleViewportChange: null,
                handleOrientationChange: null
            };

            this.originalMethods = {
                scrollIntoView: null,
                scrollTo: null,
                scrollBy: null,
                addEventListener: null
            };

            this.allTimers = new Set();
            this.resizeTimer = null;
            this.orientationTimer = null;
            this.changeRateBuffer = [];
            this.isReporting = false;
            this.reportIntervalId = null;

            this.elementTextCache = null;
            this.cachedViewportArea = null;
            this.windowScale = window.visualViewport.scale;
            this.lastReport = {
                rate: null,
                timestamp: 0
            };
            this.reportCooldown = 1000;
            this.reportHistory = [];
            this.init();
        }

        init() {
            print({
                type: 'debugLog',
                data: {
                    log: `Initialize viewport area: ${this.viewportArea} px² (${window.innerWidth} × ${window.innerHeight})`
                }
            });

            print({
                type: 'debugLog',
                data: {
                    log: `Using configuration: changeThreshold=${this.config.changeThreshold}, reportInterval=${this.config.reportInterval}ms`
                }
            });

            this.bindEventHandlers();
            this.setupMutationObserver();
            this.setupScrollListener();
            this.setupClickListener();
            this.setupFormListeners();
            this.setupViewportListeners();
            this.setupJsScrollDetection();

            this.isFirstLoad = false;

            this.scheduleTimer(() => {
                this.startReportPolling();
                this.scheduleTimer(() => {
                    this.reportInitialLoad();
                }, 50);
            }, 50);

            print({
                type: 'debugLog',
                data: { log: 'Font change monitor started (memory optimized version)' }
            });
        }

        checkScrollEnd() {
            if (this.isScrolling) {
                this.isScrolling = false;

                const endSnapshot = this.snapshotManager.createSnapshot('scroll-end');
                const startSnapshot = this.snapshotManager.getSnapshot('scroll-start');

                if (startSnapshot) {
                    const changeRate = this.snapshotManager.compareSnapshots(startSnapshot, endSnapshot, 'scroll');
                    this.addToBuffer(changeRate, 'scroll');
                    this.snapshotManager.clearSnapshot('scroll-start');
                }

                print({
                    type: 'debugLog',
                    data: { log: 'Scroll end' }
                });
            }
        }

        setupJsScrollDetection() {
            const that = this;
            this.originalMethods.scrollIntoView = Element.prototype.scrollIntoView;
            this.originalMethods.scrollTo = window.scrollTo;
            this.originalMethods.scrollBy = window.scrollBy;

            Element.prototype.scrollIntoView = function(options) {
                const result = that.originalMethods.scrollIntoView.call(this, options);

                if (!that.isScrolling) {
                    that.isScrolling = true;
                    that.snapshotManager.createSnapshot('scroll-start');
                    print({
                        type: 'debugLog',
                        data: { log: `JS scroll start (scrollIntoView)` }
                    });
                }

                if (that.scrollTimer) {
                    that.clearTimer(that.scrollTimer);
                }

                that.scrollTimer = that.scheduleTimer(() => {
                    that.checkScrollEnd();
                }, that.config.scrollEndDelay);

                return result;
            };

            window.scrollTo = function(...args) {
                const result = that.originalMethods.scrollTo.apply(this, args);

                if (!that.isScrolling) {
                    that.isScrolling = true;
                    that.snapshotManager.createSnapshot('scroll-start');
                    print({
                        type: 'debugLog',
                        data: { log: `JS scroll start (scrollTo)` }
                    });
                }

                if (that.scrollTimer) {
                    that.clearTimer(that.scrollTimer);
                }

                that.scrollTimer = that.scheduleTimer(() => {
                    that.checkScrollEnd();
                }, that.config.scrollEndDelay);

                return result;
            };

            window.scrollBy = function(...args) {
                const result = that.originalMethods.scrollBy.apply(this, args);
                if (!that.isScrolling) {
                    that.isScrolling = true;
                    that.snapshotManager.createSnapshot('scroll-start');
                    print({
                        type: 'debugLog',
                        data: { log: `JS scroll start (scrollBy)` }
                    });
                }

                if (that.scrollTimer) {
                    that.clearTimer(that.scrollTimer);
                }

                that.scrollTimer = that.scheduleTimer(() => {
                    that.checkScrollEnd();
                }, that.config.scrollEndDelay);

                return result;
            };

            print({
                type: 'debugLog',
                data: { log: 'JavaScript scroll detection set up' }
            });
        }

        initTextCacheIfNeeded() {
            if (!this.elementTextCache) {
                this.elementTextCache = new WeakMap();
                print({
                    type: 'debugLog',
                    data: { log: 'Lazy initialize text cache' }
                });
            }
        }

        checkTextChange(element) {
            this.initTextCacheIfNeeded();

            const currentText = this.getCleanText(element);
            if (!currentText || currentText.length < this.config.minTextLength) {
                return false;
            }

            const cachedText = this.elementTextCache.get(element);
            if (!cachedText) {
                this.elementTextCache.set(element, currentText);
                return true;
            }

            if (currentText !== cachedText) {
                this.elementTextCache.set(element, currentText);
                return true;
            }

            return false;
        }

        bindEventHandlers() {
            const that = this;
            this.eventHandlers = {
                handleScrollStart: (event) => {
                    if (event.type === 'touchstart' && event.touches.length !== 1){
                        return;
                    }
                    if (!that.isScrolling) {
                        that.isScrolling = true;
                        if (event.type === 'touchstart') {
                            that.scrollStartPosition.x = event.touches[0].clientX;
                            that.scrollStartPosition.y = event.touches[0].clientY;
                        } else if (event.type === 'mousedown') {
                            that.scrollStartPosition.x = event.clientX;
                            that.scrollStartPosition.y = event.clientY;
                        }
                        that.snapshotManager.createSnapshot('scroll-start');
                        print({
                            type: 'debugLog',
                            data: { log: `Scroll start` }
                        });
                    }
                    if (that.scrollTimer) {
                        that.clearTimer(that.scrollTimer);
                        that.scrollTimer = null;
                    }
                },
                handleScroll: () => {
                    if (!that.isScrolling) {
                        that.isScrolling = true;
                    }
                    if (that.scrollTimer) {
                        that.clearTimer(that.scrollTimer);
                    }
                    that.scrollTimer = that.scheduleTimer(() => {
                        that.checkScrollEnd();
                    }, that.config.scrollEndDelay);
                },
                handleTouchend: () => {
                    that.scheduleTimer(() => {
                        that.checkScrollEnd();
                    }, that.config.scrollEndDelay);
                },
                handleMouseup: () => {
                    that.scheduleTimer(() => {
                        that.checkScrollEnd();
                    }, that.config.scrollEndDelay);
                },
                handleClickStart: (event) => {
                    const isLeftClick = event.button === 0;
                    const isSingleTouch = event.type === 'touchstart' && event.touches.length === 1;
                    if (!isLeftClick && !isSingleTouch) {
                        return;
                    }
                    if (that.isClickHandling) {
                        that.clearTimer(that.clickTimer);
                        that.clickTimer = null;
                    }
                    that.isClickHandling = true;
                    that.snapshotManager.createSnapshot('click-start');
                    print({ type: 'debugLog', data: { log: `Click start` } });
                },
                handleClickEnd: (event) => {
                    if (!that.isClickHandling) {
                        return;
                    }
                    if (that.clickTimer) {
                        that.clearTimer(that.clickTimer);
                        that.clickTimer = null;
                    }
                    that.clickTimer = that.scheduleTimer(() => {
                        if (that.isClickHandling) {
                            that.viewportArea = that.calculateViewportArea();
                            const endSnapshot = that.snapshotManager.createSnapshot('click-end');
                            const startSnapshot = that.snapshotManager.getSnapshot('click-start');
                            if (startSnapshot) {
                                const changeRate = that.snapshotManager.compareSnapshots(startSnapshot, endSnapshot, 'click');
                                that.addToBuffer(changeRate, 'click');
                                that.snapshotManager.clearSnapshot('click-start');
                            }
                            that.isClickHandling = false;
                            print({ type: 'debugLog', data: { log: `Click end` } });
                        }
                    }, that.config.clickEndDelay);
                },
                handleDragStart: () => {
                    if (that.isClickHandling) {
                        that.isClickHandling = false;
                        that.snapshotManager.clearSnapshot('click-start');
                    }
                },
                handleFormChange: (event) => {
                    const element = event.target;
                    const tagName = element.tagName.toLowerCase();
                    const isFormElement = /*tagName === 'input' ||*/ tagName === 'textarea' || tagName === 'select';
                    if (!isFormElement) {
                        return;
                    }
                    let timer = that.formDebounceMap.get(element);
                    if (timer) {
                        that.clearTimer(timer);
                    }
                    timer = that.scheduleTimer(() => {
                        that.handleFormElementChange(element);
                        that.formDebounceMap.delete(element);
                    }, that.config.debounceDelay);
                    that.formDebounceMap.set(element, timer);
                },
                handleViewportChange: () => {
                    if (that.resizeTimer) {
                        that.clearTimer(that.resizeTimer);
                        that.resizeTimer = null;
                    }
                    that.resizeTimer = that.scheduleTimer(() => {
                        that.viewportArea = that.calculateViewportArea();
                        print({
                            type: 'debugLog',
                            data: { log: `Viewport area updated(resize): ${that.viewportArea} px²` }
                        });
                    }, that.config.debounceDelay);
                },
                handleOrientationChange: () => {
                    print({
                        type: 'debugLog',
                        data: {
                            log: `Orientation changed (${window.orientation}°)`
                        }
                    });
                    that.orientationTimer = that.scheduleTimer(() => {
                        that.viewportArea = that.calculateViewportArea();
                        print({
                            type: 'debugLog',
                            data: { log: `Viewport area updated(orientation): ${that.viewportArea} px²` }
                        });
                    }, that.config.orientationEndDelay);
                }
            };
        }

        setupMutationObserver() {
            if (!_MutationObserver) {
                return;
            }

            this.mutationObserver = new _MutationObserver((mutations) => {
                this.handleMutations(mutations);
            });

            this.mutationObserver.observe(_document.body, {
                childList: true,
                subtree: true,
                characterData: true,
                attributes: false,
                characterDataOldValue: false,
                childListOldValue: false
            });

            print({
                type: 'debugLog',
                data: { log: 'MutationObserver set up (lightweight configuration)' }
            });
        }

        handleMutations(mutations) {
            if (this.isProcessing) {
                return;
            }

            this.isProcessing = true;
            let hasChanges = false;

            try {
                for (let i = 0; i < mutations.length; i++) {
                    const mutation = mutations[i];

                    if (mutation.type === 'characterData') {
                        const element = mutation.target.parentElement;
                        if (element && this.isElementVisible(element)) {
                            if (this.checkTextChange(element)) {
                                this.handleTextChange(element);
                                hasChanges = true;
                            }
                        }
                    } else if (mutation.type === 'childList') {
                        this.handleChildListMutation(mutation);
                        hasChanges = true;
                    }
                }
            } finally {
                this.isProcessing = false;
            }
        }

        handleChildListMutation(mutation) {
            const target = mutation.target;

            const processNode = (node) => {
                if (node.nodeType === 1) {
                    if (this.isElementVisible(node) && this.isElementInViewport(node)) {
                        if (this.checkTextChange(node)) {
                            this.handleTextChange(node);
                        }
                    }

                    if (node.children && node.children.length > 0) {
                        this.checkElementChildren(node);
                    }
                } else if (node.nodeType === 3) {
                    const parent = node.parentElement;
                    if (parent && this.isElementVisible(parent) &&
                        this.isElementInViewport(parent)) {
                        if (this.checkTextChange(parent)) {
                            this.handleTextChange(parent);
                        }
                    }
                }
            };

            mutation.addedNodes.forEach(processNode);

            mutation.removedNodes.forEach(node => {
                if (node.nodeType === 1) {
                    this.handleRemovedElement(node);
                }
            });
        }

        checkElementChildren(element) {
            const children = element.children;
            for (let i = 0; i < children.length; i++) {
                const child = children[i];
                if (this.isElementVisible(child) && this.isElementInViewport(child)) {
                    if (this.checkTextChange(child)) {
                        this.handleTextChange(child);
                    }
                }
            }
        }

        setupScrollListener() {
            window.addEventListener('touchstart', this.eventHandlers.handleScrollStart, { passive: true });
            window.addEventListener('mousedown', this.eventHandlers.handleScrollStart);
            window.addEventListener('scroll', this.eventHandlers.handleScroll, { passive: true });
            window.addEventListener('touchmove', this.eventHandlers.handleScroll, { passive: true });
            window.addEventListener('touchend', this.eventHandlers.handleTouchend, { passive: true });
            window.addEventListener('mouseup', this.eventHandlers.handleMouseup);
        }

        setupClickListener() {
            _document.addEventListener('mousedown', this.eventHandlers.handleClickStart);
            _document.addEventListener('touchstart', this.eventHandlers.handleClickStart, { passive: true });
            _document.addEventListener('mouseup', this.eventHandlers.handleClickEnd);
            _document.addEventListener('touchend', this.eventHandlers.handleClickEnd, { passive: true });
            _document.addEventListener('dragstart', this.eventHandlers.handleDragStart);
        }

        setupFormListeners() {
            _document.addEventListener('input', this.eventHandlers.handleFormChange, true);
            _document.addEventListener('change', this.eventHandlers.handleFormChange, true);
        }

        setupViewportListeners() {
            window.addEventListener('resize', this.eventHandlers.handleViewportChange);
            window.addEventListener('orientationchange', this.eventHandlers.handleOrientationChange);
        }

        startReportPolling() {
            if (this.reportIntervalId) {
                this.clearTimer(this.reportIntervalId);
            }

            this.reportIntervalId = _setInterval(() => {
                this.pollBuffer();
            }, this.config.reportInterval);
            this.allTimers.add(this.reportIntervalId);

            print({
                type: 'debugLog',
                data: { log: `Polling report: check every ${this.config.reportInterval}ms` }
            });
        }

        pollBuffer() {
            if (this.changeRateBuffer.length === 0) {
                return;
            }

            if (this.changeRateBuffer.length >= 5) {
                print({
                    type: 'debugLog',
                    data: { log: `Polling: ${this.changeRateBuffer.length} pending change rates` }
                });
            }

            this.processBuffer();
        }

        addToBuffer(changeRate, source) {
            changeRate = Math.max(0, Math.min(1, changeRate));
            const sourceText = this.getSourceText(source);

            if (changeRate >= this.config.changeThreshold) {
                if (this.changeRateBuffer.length >= this.config.maxBufferSize) {
                    this.changeRateBuffer.shift();
                }

                this.changeRateBuffer.push({
                    rate: changeRate,
                    source: source,
                    timestamp: Date.now()
                });

                print({
                    type: 'debugLog',
                    data: {
                        log: `Added to buffer(${sourceText}): ${(changeRate * 100).toFixed(2)}%` +
                            ` | Threshold: ${(this.config.changeThreshold * 100).toFixed(2)}%` +
                            ` | Buffer size: ${this.changeRateBuffer.length}/${this.config.maxBufferSize}`
                    }
                });

                if (this.changeRateBuffer.length >= 8) {
                    this.scheduleTimer(() => {
                        this.pollBuffer();
                    }, 50);
                }
            }
        }

        getSourceText(source) {
            switch (source) {
                case 'scroll':
                    return 'scroll';
                case 'click':
                    return 'click';
                case 'mutation':
                    return 'DOMChange';
                case 'form':
                    return 'formChange';
                case 'removal':
                    return 'removeElement';
                default:
                    return source || 'unknown';
            }
        }

        processBuffer() {
            if (this.changeRateBuffer.length === 0 || this.isReporting) {
                return;
            }

            this.isReporting = true;

            try {
                const recentReports = this.getRecentReportHistory();
                const filteredResults = [];
                
                for (let i = 0; i < this.changeRateBuffer.length; i++) {
                    const item = this.changeRateBuffer[i];
                    const shouldReport = this.shouldReportItem(item, recentReports);       
                    if (shouldReport) {
                        filteredResults.push(item);
                        this.updateReportHistory(item);
                    }
                }

                filteredResults.forEach(item => {
                    this.reportSingleChange(item.rate, item.source, item.timestamp);
                });

                const originalSize = this.changeRateBuffer.length;
                this.changeRateBuffer.length = 0;
                
                print({
                    type: 'debugLog',
                    data: {
                        log: `Process buffer: ${filteredResults.length} reported from ${originalSize}`
                    }
                });
            } finally {
                this.isReporting = false;
            }
        }

        shouldReportItem(item, recentReports) {
            const { rate, source, timestamp } = item;

            if (this.shouldSkipReport(rate, timestamp)) {
                return false;
            }

            for (const report of recentReports) {
                const timeDiff = Math.abs(timestamp - report.timestamp);
                const rateDiff = Math.abs(rate - report.rate);
                if (rateDiff < 0.001 && timeDiff < 1000) {
                    return false;
                }
            }
            
            return true;
        }

        getRecentReportHistory() {
            if (!this.reportHistory) {
                this.reportHistory = [];
            }
            return this.reportHistory;
        }

        updateReportHistory(item) {
            if (!this.reportHistory) {
                this.reportHistory = [];
            }
            
            this.reportHistory.push({
                rate: item.rate,
                timestamp: item.timestamp,
                source: item.source
            });
            const maxHistorySize = 10;
            if (this.reportHistory.length > maxHistorySize) {
                this.reportHistory = this.reportHistory.slice(-maxHistorySize);
            }
        }

        reportSingleChange(changeRate, source, timestamp) {
            changeRate = Math.max(0, Math.min(1, changeRate));
            const now = timestamp && Date.now();
            if (this.shouldSkipReport(changeRate, now)) {
                print({
                    type: 'debugLog',
                    data: {
                        log: `Skip duplicate report: rate=${(changeRate*100).toFixed(2)}%, ` +
                             `time since last=${now - this.lastReport.timestamp}ms`
                    }
                });
                return;
            }
            this.lastReport = {
                rate: changeRate,
                timestamp: now
            };

            if (typeof window.__arkWebDomTree !== 'undefined' &&
                typeof window.__arkWebDomTree.forceReport === 'function') {
                window.__arkWebDomTree.forceReport();
            } else {
                print({
                    type: 'debugLog',
                    data: {log: `window.__arkWebDomTree.forceReport function error`}
                });
            }

            print({
                type: 'textChange',
                data: { textChangeRatio: changeRate }
            });
        }

        shouldSkipReport(changeRate, timestamp) {
            if (!this.lastReport.rate) {
                return false;
            }
            const timeDiff = timestamp - this.lastReport.timestamp;
            const isTimeClose = timeDiff < this.reportCooldown;
            const isRateEqual = Math.abs(changeRate - this.lastReport.rate) < 0.0001;
            return isTimeClose && isRateEqual;
        }

        reportInitialLoad() {
            if (this.viewportArea <= 0) {
                this.viewportArea = this.calculateViewportArea();
            }

            if (typeof window.__arkWebDomTree !== 'undefined' &&
                typeof window.__arkWebDomTree.forceReport === 'function') {
                window.__arkWebDomTree.forceReport();
            } else {
                print({
                    type: 'debugLog',
                    data: {log: `window.__arkWebDomTree.forceReport function error`}
                });
            }

            print({
                type: 'textChange',
                data: { textChangeRatio: 1.0 }
            });
        }

        calculateViewportArea() {
            let area = 0;
            if (window.visualViewport) {
                const vp = window.visualViewport;
                area = Math.max(1, vp.width * vp.height);
            } else {
                const width = Math.min(
                    window.innerWidth || 0,
                    document.documentElement.clientWidth || 0,
                    document.body.clientWidth || 0
                );
                const height = Math.min(
                    window.innerHeight || 0,
                    document.documentElement.clientHeight || 0,
                    document.body.clientHeight || 0
                );
                area = Math.max(1, width * height);
            }
            this.cachedViewportArea = {
                value: area,
                timestamp: Date.now()
            };

            return area;
        }

        calculateDesktopViewportArea() {
            return window.innerWidth * window.innerHeight;
        }

        getMobileViewportHeight(isIOS, isLandscape) {
            const docClientHeight = document.documentElement.clientHeight;
            const windowInnerHeight = window.innerHeight;

            if (isIOS) {
                return Math.max(docClientHeight, windowInnerHeight);
            }

            return Math.max(docClientHeight, windowInnerHeight);
        }

        getMobileViewportWidth(isIOS, isLandscape) {
            return Math.min(
                window.innerWidth || 375,
                document.documentElement.clientWidth || 375,
                window.screen.width || 375
            );
        }

        getViewportElements() {
            const elements = [];

            const walker = _document.createTreeWalker(
                _document.body,
                NodeFilter.SHOW_ELEMENT,
                null,
                false
            );

            let node;
            while (node = walker.nextNode()) {
                if (!this.shouldSkipElement(node) &&
                    this.isElementInViewport(node) &&
                    this.isElementVisible(node)) {
                    elements.push(node);
                }
            }

            return elements;
        }

        isElementInViewport(element) {
            if (!element || element.nodeType !== 1) {
                return false;
            }
            const rect = element.getBoundingClientRect();

            const isInViewport = (
                rect.top < window.innerHeight &&
                rect.bottom > 0 &&
                rect.left < window.innerWidth &&
                rect.right > 0
            );
            if (window.visualViewport.scale !== this.windowScale && isInViewport) {
                const vp = window.visualViewport;
                const hasIntersection = !(
                    rect.right < vp.offsetLeft ||
                    rect.left > (vp.offsetLeft + vp.width) ||
                    rect.bottom < vp.offsetTop ||
                    rect.top > (vp.offsetTop + vp.height)
                );
                
                return hasIntersection;
            }

            return isInViewport;
        }

        isElementVisible(element) {
            if (!element || element.nodeType !== 1) {
                return false;
            }
            const style = _getComputedStyle(element);
            if (style.display === 'none' ||
                style.visibility === 'hidden' ||
                parseFloat(style.opacity) <= 0.1) {
                return false;
            }
            return this.hasVisibleText(element);
        }

        hasVisibleText(element) {
            const tagName = element.tagName.toLowerCase();

            if (tagName === 'select') {
                const selected = element.options[element.selectedIndex];
                const value = selected ? selected.textContent || selected.value || '' : '';
                return value.length >= this.config.minTextLength;
            }

            if (/*tagName === 'input' ||*/ tagName === 'textarea') {
                const value = element.value || '';
                return value.length >= this.config.minTextLength;
            }

            const textContent = element.textContent || '';
            const trimmedText = textContent.replace(/\s+/g, ' ').trim();
            return trimmedText.length >= this.config.minTextLength;
        }

        calculateElementArea(element) {
            if (!element || element.nodeType !== 1) {
                return 0;
            }

            try {
                const rect = element.getBoundingClientRect();
                const width = Math.max(0, rect.width || 0);
                const height = Math.max(0, rect.height || 0);

                if (width <= 0 || height <= 0) {
                    return 0;
                }

                const area = width * height;
                const maxReasonableArea = this.viewportArea > 0 ?
                    this.viewportArea * 50 :
                    (window.innerWidth * window.innerHeight) * 50;

                if (area > maxReasonableArea) {
                    return maxReasonableArea;
                }

                const style = _getComputedStyle(element);
                const opacity = parseFloat(style.opacity) || 1;
                const display = style.display;

                if (display === 'none' || opacity <= 0.1) {
                    return 0;
                }

                return area * opacity;

            } catch (error) {
                return 0;
            }
        }

        getCleanText(element) {
            const tagName = element.tagName.toLowerCase();

            if (tagName === 'select') {
                const selected = element.options[element.selectedIndex];
                return (selected ? selected.textContent || selected.value || '' : '').trim();
            }

            if (/*tagName === 'input' ||*/ tagName === 'textarea') {
                return (element.value || '').trim();
            }

            const textContent = element.textContent || '';
            return textContent.replace(/\s+/g, ' ').trim();
        }

        getElementKey(element) {
            if (!element || element.nodeType !== 1) {
                return 'error';
            }

            if (element.id && typeof element.id === 'string' && element.id.length > 0) {
                return `id_${element.id}`;
            }

            const path = [];
            let current = element;
            let depth = 0;
            const maxDepth = 6;

            while (current && current.nodeType === 1 && depth < maxDepth) {
                let selector = current.tagName.toLowerCase();

                if (current.className && typeof current.className === 'string') {
                    const firstClass = current.className.split(/\s+/)[0];
                    if (firstClass && firstClass.length > 0) {
                        selector += `.${firstClass}`;
                    }
                }

                if (current.parentNode) {
                    const siblings = Array.from(current.parentNode.children);
                    const index = siblings.indexOf(current);
                    if (index >= 0) {
                        selector += `:nth-child(${index + 1})`;
                    }
                }

                path.unshift(selector);
                depth++;

                if (current === _document.body || current === _document.documentElement) {
                    break;
                }
                current = current.parentNode;
            }

            return `path_${path.join('>')}`;
        }

        shouldSkipElement(element) {
            const tagName = element.tagName.toLowerCase();
            const excludedTags = [
                'script', 'style', 'noscript', 'link', 'meta',
                'br', 'hr', 'img', 'svg', 'path', 'circle', 'rect',
                'ellipse', 'polygon', 'line',
                'iframe', 'embed', 'object', 'canvas',
                'audio', 'video', 'source', 'track',
                'picture', 'source'
            ];

            return excludedTags.includes(tagName);
        }

        handleTextChange(element) {
            const area = this.calculateElementArea(element);
            if (area <= 0 || this.viewportArea <= 0) {
                return;
            }

            const changeRate = area / this.viewportArea;
            this.addToBuffer(changeRate, 'mutation');
        }

        handleFormElementChange(element) {
            if (!this.isElementVisible(element) || !this.isElementInViewport(element)) {
                return;
            }

            const area = this.calculateElementArea(element);
            if (area <= 0) {
                return;
            }

            const changeRate = this.viewportArea > 0 ? (area / this.viewportArea) : 0;
            this.addToBuffer(changeRate, 'form');
        }

        handleRemovedElement(element) {
            const area = this.calculateElementArea(element);
            if (area <= 0 || this.viewportArea <= 0) {
                return;
            }

            const changeRate = area / this.viewportArea;
            this.addToBuffer(changeRate, 'removal');

            if (this.elementTextCache) {
                this.elementTextCache.delete(element);
            }
        }

        scheduleTimer(fn, delay) {
            const timerId = _setTimeout(() => {
                fn();
                this.allTimers.delete(timerId);
            }, delay);

            this.allTimers.add(timerId);
            return timerId;
        }

        clearTimer(timerId) {
            if (timerId) {
                if (this.reportIntervalId === timerId) {
                    _clearInterval(timerId);
                } else {
                    _clearTimeout(timerId);
                }
                this.allTimers.delete(timerId);
            }
        }

        stop() {
            print({
                type: 'debugLog',
                data: { log: 'Stopping monitor...' }
            });
            this.restoreOriginalMethods();

            this.clearAllTimers();
            this.stopMutationObserver();
            this.removeAllEventListeners();
            this.clearAllData();
            this.cleanupGlobalState();

            print({
                type: 'debugLog',
                data: { log: 'Monitor completely stopped' }
            });
        }

        restoreOriginalMethods() {
            if (this.originalMethods.scrollIntoView) {
                Element.prototype.scrollIntoView = this.originalMethods.scrollIntoView;
            }

            if (this.originalMethods.scrollTo) {
                window.scrollTo = this.originalMethods.scrollTo;
            }

            if (this.originalMethods.scrollBy) {
                window.scrollBy = this.originalMethods.scrollBy;
            }

            window.addEventListener = function(type, listener, options) {
                return EventTarget.prototype.addEventListener.call(this, type, listener, options);
            };

            print({
                type: 'debugLog',
                data: { log: 'Original methods restored' }
            });
        }

        clearAllTimers() {
            this.allTimers.forEach(timerId => {
                this.clearTimer(timerId);
            });
            this.allTimers.clear();

            this.scrollTimer = null;
            this.clickTimer = null;
            this.resizeTimer = null;
            this.orientationTimer = null;
            this.reportIntervalId = null;
            print({
                type: 'debugLog',
                data: {
                    log: 'clear all timers end'
                }
            })
        }

        stopMutationObserver() {
            if (this.mutationObserver) {
                this.mutationObserver.disconnect();
                this.mutationObserver = null;
            }
            print({
                type: 'debugLog',
                data: {
                    log: 'end mutation observer'
                }
            })
        }

        removeAllEventListeners() {
            const handlers = this.eventHandlers;

            if (handlers.handleScrollStart) {
                window.removeEventListener('touchstart', handlers.handleScrollStart);
                window.removeEventListener('mousedown', handlers.handleScrollStart);
            }

            if (handlers.handleScroll) {
                window.removeEventListener('scroll', handlers.handleScroll);
                window.removeEventListener('touchmove', handlers.handleScroll);
            }

            if (handlers.handleTouchend) {
                window.removeEventListener('touchend', handlers.handleTouchend);
            }

            if (handlers.handleMouseup) {
                window.removeEventListener('mouseup', handlers.handleMouseup);
            }

            if (handlers.handleViewportChange) {
                window.removeEventListener('resize', handlers.handleViewportChange);
                window.removeEventListener('orientationchange', handlers.handleOrientationChange);
            }

            if (handlers.handleClickStart) {
                _document.removeEventListener('mousedown', handlers.handleClickStart);
                _document.removeEventListener('touchstart', handlers.handleClickStart);
            }

            if (handlers.handleClickEnd) {
                _document.removeEventListener('mouseup', handlers.handleClickEnd);
                _document.removeEventListener('touchend', handlers.handleClickEnd);
            }

            if (handlers.handleDragStart) {
                _document.removeEventListener('dragstart', handlers.handleDragStart);
            }

            if (handlers.handleFormChange) {
                _document.removeEventListener('input', handlers.handleFormChange);
                _document.removeEventListener('change', handlers.handleFormChange);
            }

            print({
                type: 'debugLog',
                data: {
                    log: 'end removeAllEventListeners'
                }
            })
        }

        clearAllData() {


            this.registry.clear();
            this.formValues = new WeakMap();
            this.formDebounceMap = new WeakMap();
            this.snapshotManager.clearSnapshot();

            this.changeRateBuffer.length = 0;
            this.isReporting = false;

            this.elementTextCache = null;
            this.cachedViewportArea = null;
            print({
                type: 'debugLog',
                data: {
                    log: 'end clear all data'
                }
            })
        }

        cleanupGlobalState() {
            this.isProcessing = false;
            this.isScrolling = false;
            this.isClickHandling = false;
            this.scrollStartPosition = { x: 0, y: 0 };
            this.snapshotManager.clearSnapshot();
            this.changeRateBuffer.length = 0;
            this.isReporting = false;
            print({
                type: 'debugLog',
                data: {
                    log: 'end cleanupGlobalState'
                }
            })
        }
    }

    window.viewportFontMonitor = {
        _monitor: null,

        start: function(changeThreshold, reportInterval) {
            if (this._monitor) {
                print({
                    type: 'debugLog',
                    data: { log: 'Monitor is already running, please stop first and then restart' }
                });
                return;
            }
            const checkBodyReady = () => {
                return new Promise((resolve) => {
                    if (_document && _document.body && _document.body.nodeType === 1) {
                        print({
                            type: 'debugLog',
                            data: { log: 'Document body is ready' }
                        });
                        resolve();
                    } else if (_document.readyState === 'loading') {
                        print({
                            type: 'debugLog',
                            data: { log: 'Waiting for document body...' }
                        });
                        _document.addEventListener('DOMContentLoaded', resolve);
                    } else {
                        const interval = _setInterval(() => {
                            if (document.body && document.body.nodeType === 1) {
                                _clearInterval(interval);
                                print({
                                    type: 'debugLog',
                                    data: { log: 'Document body is ready (after polling)' }
                                });
                                resolve();
                            }
                        }, 100);

                        _setTimeout(() => {
                            _clearInterval(interval);
                            print({
                                type: 'debugLog',
                                data: { log: 'Body not ready after timeout, proceeding anyway' }
                            });
                            resolve();
                        }, 5000);
                    }
                });
            };
            const startMonitor = async () => {
                try {
                    await checkBodyReady();
                    this._monitor = new FontChangeMonitor(changeThreshold, reportInterval);

                    print({
                        type: 'debugLog',
                        data: {
                            log: `Monitor start successfully, parameters: changeThreshold=${changeThreshold !== undefined ? changeThreshold : DEFAULT_CONFIG.changeThreshold}, reportInterval=${reportInterval !== undefined ? reportInterval : DEFAULT_CONFIG.reportInterval}ms`
                        }
                    });
                } catch (error) {
                    print({
                        type: 'debugLog',
                        data: { log: `Monitor start failed: ${error.message}` }
                    });
                    print({
                        type: 'reportHiSysEvent',
                        data: {
                            log: `Monitor start faild: ${error.message}`
                        }
                    });
                }
            };
            startMonitor();
        },

        stop: function() {
            if (this._monitor) {
                this._monitor.stop();
                this._monitor = null;
                print({
                    type: 'debugLog',
                    data: { log: 'Monitor stopped' }
                });
            } else {
                print({
                    type: 'debugLog',
                    data: { log: 'Monitor is not running, no need to stop' }
                });
            }
        },

        getStats: function() {
            if (!this._monitor) {
                return null;
            }
            return {
                viewportArea: this._monitor.viewportArea,
                bufferSize: this._monitor.changeRateBuffer.length,
                config: {
                    changeThreshold: this._monitor.config.changeThreshold,
                    reportInterval: this._monitor.config.reportInterval
                }
            };
        },

        triggerPoll: function() {
            if (this._monitor) {
                this._monitor.pollBuffer();
            }
        }
    };
})(window);