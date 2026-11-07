(function(s) {
    'use strict';

    const n = (d) => {
        if (s.ArkWebContentChangeDetector && s.ArkWebContentChangeDetector.reportContentChange) {
            s.ArkWebContentChangeDetector.reportContentChange(d);
        } else {
            console.warn('ArkWebContentChangeDetector not found');
        }
    };

    class ImageChangeDetector {
        constructor() {
            this.observedNodes = new WeakSet();
            this.pendingQueue = [];
            this.reportTimer = null;
            this.mutationObserver = null;
            this.intersectionObserver = null;
            this.config = {
                MAX_BATCH_SIZE: 10,
                BATCH_DELAY: 100
            };
        }

        handleMutation(m) {
            if (m.addedNodes && m.addedNodes.forEach) {
                m.addedNodes.forEach(node => {
                    if (node.nodeType === 1) {
                        this.scanAndTrack(node);
                    }
                });
            }

            const isImgSrcChange = m.type === 'attributes' && 
                                m.attributeName === 'src' && 
                                m.target.tagName === 'IMG';
                                
            if (isImgSrcChange) {
                this.intersectionObserver.observe(m.target);
            }
        }

        start() {
            console.log('NWebImageChangeDetection core starting...');
            this.initIntersectionObserver();
            this.scanAndTrack(s.document.body);

            this.mutationObserver = new MutationObserver((mutations) => {
                mutations.forEach(m => this.handleMutation(m));
            });

            this.mutationObserver.observe(s.document.body, { 
                childList: true, 
                subtree: true, 
                attributes: true,
                attributeFilter: ['src']
            });
        }

        initIntersectionObserver() {
            this.intersectionObserver = new IntersectionObserver((entries) => {
                entries.forEach(entry => {
                    if (entry.isIntersecting) {
                        this.setupImgListener(entry.target);
                    }
                });
            }, {
                root: null,
                threshold: 0.01
            });
        }

        stop() {
            if (this.mutationObserver) {
                this.mutationObserver.disconnect();
            }
            if (this.intersectionObserver) {
                this.intersectionObserver.disconnect();
            }
            clearTimeout(this.reportTimer);
            this.pendingQueue = [];
            console.log('NWebImageChangeDetection core stopped.');
        }

        scanAndTrack(root) {
            const imgs = root.tagName === 'IMG' ? [root] : root.querySelectorAll('img');
            imgs.forEach(img => {
                this.intersectionObserver.observe(img);
            });
        }

        setupImgListener(img) {
            const onComplete = () => {
                requestAnimationFrame(() => {
                  requestAnimationFrame(() => {
                    this.handleImageEvent(img);
                    });
                });
            };
            const isRealGif = (url) => /\.(gif)(\?.*)?$/i.test(url) || url.startsWith('data:image/gif');
            if (img.complete) {
                if (!isRealGif(img.currentSrc || img.src)) {
                    onComplete();
                }
            } else {
                img.addEventListener('load', () => {
                    if (!isRealGif(img.currentSrc || img.src)) {
                        onComplete();
                    }
                }, { once: true });
            }
        }
        
        handleImageEvent(img) {
            const rect = img.getBoundingClientRect();
            const vh = s.innerHeight || s.document.documentElement.clientHeight;
            const vw = s.innerWidth || s.document.documentElement.clientWidth;
            const isInViewport = (rect.bottom > 0 && rect.right > 0 && rect.top < vh && rect.left < vw);
        
            if (isInViewport) {
                const currentSrc = img.currentSrc || img.src;
                const lastSrc = img.getAttribute('data-last-reported-src');
                if (lastSrc === currentSrc) {
                    return;
                }
                img.setAttribute('data-last-reported-src', currentSrc);
                this.addToQueue(img, rect);
            }
        }

        addToQueue(img, rect) {
            const dpr = s.devicePixelRatio || 1;
            const renderedWidth = Math.round(rect.width * dpr);
            const renderedHeight = Math.round(rect.height * dpr);
            const absoluteX = Math.round(rect.left * dpr);
            const absoluteY = Math.round((rect.top + window.screenY)* dpr);

            this.pendingQueue.push({
                nodeId: img.id || '',
                src: img.currentSrc || img.src,
                width: renderedWidth,
                height: renderedHeight,
                x: absoluteX,
                y: absoluteY,
                area: renderedWidth * renderedHeight
            });

            if (!this.reportTimer) {
                this.reportTimer = setTimeout(() => this.flushQueue(), this.config.BATCH_DELAY);
            }
        }

        flushQueue() {
            if (this.pendingQueue.length === 0) {
                return;
            }
            let finalBatch = this.pendingQueue;
            if (finalBatch.length > this.config.MAX_BATCH_SIZE) {
                finalBatch.sort((a, b) => b.area - a.area);
                finalBatch = finalBatch.slice(0, this.config.MAX_BATCH_SIZE);
            }

            const reportData = {
                count: finalBatch.length,
                images: finalBatch.map(({area, ...rest}) => rest)
            };

            console.log(JSON.stringify(reportData, null, 2));

            n(JSON.stringify({
                type: 'imageChange',
                data: reportData
            }));
        
            this.pendingQueue = [];
            this.reportTimer = null;
        }
    }

    if (!s.ImageDetectorInstance) {
        s.ImageDetectorInstance = new ImageChangeDetector();
        console.log('NWebImageChangeDetection instance initialized.');
    }

    s.arkImageDetector = {
        start: function() {
            s.ImageDetectorInstance.start();
        },
        stop: function() {
            s.ImageDetectorInstance.stop();
        }
    };

})(window);