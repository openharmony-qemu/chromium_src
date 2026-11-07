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
    const NodeFilter = window.NodeFilter;

    const DEFAULT_CONFIG = {
        minTextLength: 2,
        changeThreshold: 0.15,
        reportInterval: 100,
        debounceDelay: 50,
        scrollEndDelay: 200,
        orientationEndDelay: 300,
        clickEndDelay: 200,
        maxBufferSize: 20,
        maxSnapshotCount: 5
    };

    // Stable per-element identity. The previous implementation derived a key
    // from tag + first className + nth-child, which (a) cost O(siblings) per
    // ancestor level with an array allocation each, and (b) changed whenever a
    // class changed — so a skeleton resolving into text looked like one
    // element disappearing and a different one appearing, double-counting the
    // area. A WeakMap identity id is stable across class mutations and is O(1)
    // after the first lookup.
    //
    // The id counter is module-scoped, not per-instance, on purpose: keys are
    // used as Map keys in snapshot.elementMap and compared across snapshots. If
    // two caches each counted from 1, 'e1' in one snapshot would silently alias
    // a different element in another — a wrong disappeared/appeared diff with
    // no error, just a bad area number. One global counter makes every id
    // unique across every instance and every start/stop cycle.
    let nextElementId = 1;

    class ElementKeyCache {
        constructor() {
            this.keys = new WeakMap();
            this.pathMisses = 0;
        }

        keyOf(element) {
            if (!element || element.nodeType !== 1) {
                return 'error';
            }
            const cached = this.keys.get(element);
            if (cached !== undefined) {
                return cached;
            }
            this.pathMisses++;
            const key = 'e' + (nextElementId++);
            this.keys.set(element, key);
            return key;
        }

        // Only for elements that are genuinely detached and will never be
        // observed again: ids are not recycled, so a forgotten element that
        // comes back gets a new key and reads as one element disappearing and
        // another appearing — the exact double-counting this class prevents.
        // Not a general cache-trimming lever; the WeakMap frees entries itself.
        forget(element) {
            if (element && element.nodeType === 1) {
                this.keys.delete(element);
            }
        }
    }

    // Every code point JS \s matches, which is what the textContent path this
    // class replaces trims away. Narrower lists let CJK indentation (U+3000),
    // line/paragraph separators and the BOM count as visible characters, which
    // would start elements being tracked that currently read as empty and add
    // spurious change-rate contributions. Module-scoped so it is built once,
    // not per instance and not per call; one Set lookup per character keeps the
    // early-exit win. U+200B is deliberately absent: it is not \s, so the old
    // path does not trim it and both paths must keep counting it.
    const WHITESPACE_CODES = new Set([
        9, 10, 11, 12, 13, 32, 160, 5760,
        8192, 8193, 8194, 8195, 8196, 8197, 8198, 8199, 8200, 8201, 8202,
        8232, 8233, 8239, 8287, 12288, 65279
    ]);

    // Answers "does this element hold at least minTextLength visible
    // characters" without building a string. element.textContent walks the
    // whole descendant subtree and allocates (Blink: node.cc, StringBuilder
    // over NodeTraversal::InclusiveDescendantsOf), and the old code called it
    // once in hasVisibleText and again in getCleanText. This walks manually
    // and stops the moment the threshold is met.
    class TextProbe {
        constructor(minTextLength) {
            this.minTextLength = minTextLength;
        }

        hasText(element) {
            if (!element || element.nodeType !== 1) {
                return false;
            }
            const tagName = element.tagName.toLowerCase();

            if (tagName === 'select') {
                const selected = element.options && element.options[element.selectedIndex];
                const value = selected
                    ? (selected.textContent || selected.value || '')
                    : '';
                // Raw, untrimmed length on purpose: the hasVisibleText code this
                // replaces compares value.length, and report-emission equivalence
                // is the contract. Trimming would flip true->false for a
                // source-formatted option label like '\n    A\n  '.
                return value.length >= this.minTextLength;
            }

            if (tagName === 'textarea') {
                // Raw, untrimmed length on purpose, same reason as 'select'
                // above: a textarea holding only spaces must keep reading as
                // having text, exactly as it does today.
                return (element.value || '').length >= this.minTextLength;
            }

            return this.countTextAtLeast(element, this.minTextLength);
        }

        // Depth-first over child nodes, counting non-whitespace characters,
        // bailing out at `needed`. Explicit stack: recursion would be deeper
        // than necessary on wide trees.
        countTextAtLeast(root, needed) {
            if (needed <= 0) {
                return true;
            }
            let found = 0;
            const stack = [root];
            while (stack.length > 0) {
                const node = stack.pop();
                const kids = node.childNodes;
                if (!kids) {
                    continue;
                }
                for (let i = kids.length - 1; i >= 0; i--) {
                    const child = kids[i];
                    // 3 is Text, 4 is CDATASection — CDATASection extends Text
                    // and textContent includes it, so it must count here too.
                    // Only reachable through inline SVG/MathML foreign content.
                    if (child.nodeType === 3 || child.nodeType === 4) {
                        const data = child.data || '';
                        for (let c = 0; c < data.length; c++) {
                            if (!WHITESPACE_CODES.has(data.charCodeAt(c))) {
                                found++;
                                if (found >= needed) {
                                    return true;
                                }
                            }
                        }
                    } else if (child.nodeType === 1) {
                        stack.push(child);
                    }
                }
            }
            return false;
        }
    }

    // Hoisted out of the old shouldSkipElement(), which rebuilt this array on
    // every element visit. A Set also turns the membership test from a linear
    // scan into a hash lookup.
    // Excluded from collection themselves AND pruned with their whole subtree.
    // The subtree pruning is an accepted divergence: the old shouldSkipElement()
    // was a self-only skip, so the old walk descended into these and excluded
    // the descendants on rect/text grounds instead. None of these renders
    // text-bearing descendants that the old walk actually collected - the two
    // tags that do are in SKIP_SELF_TAGS below.
    const SKIP_TAGS = new Set([
        'script', 'style', 'noscript', 'link', 'meta',
        'br', 'hr', 'img', 'path', 'circle', 'rect',
        'ellipse', 'polygon', 'line',
        'iframe', 'embed', 'canvas',
        'audio', 'video', 'source', 'track',
        'picture'
    ]);

    // Excluded themselves but NOT pruned, because their descendants really do
    // get laid out and the old per-element walk collected them:
    //   svg     - an SVG <text> has a non-zero rect and textContent. Pruning it
    //             would systematically under-count chart pages.
    //   object  - an <object> whose resource fails to load (blocked plugin,
    //             404 PDF) represents its child content, which is then laid out
    //             with a real rect and real text.
    // Nothing else qualifies: video/audio/canvas/iframe fallback is not
    // rendered when the element type is supported, which it is in
    // ArkWeb/Chromium; picture's only rendered child is an <img>, itself in
    // SKIP_TAGS and text-free; embed is void.
    // Disjoint from SKIP_TAGS by construction.
    const SKIP_SELF_TAGS = new Set(['svg', 'object']);

    // Single-pass viewport collection. Three changes from the original:
    //   1. The TreeWalker filter returns FILTER_REJECT for skipTags members,
    //      display:none and opacity === 0, pruning entire subtrees. The old code
    //      passed a null filter and tested inside the loop, so a hidden
    //      container with 5000 descendants still measured all 5000. Everything
    //      the old walk could still see below the failing element -
    //      visibility:hidden, near-transparent-but-not-zero, the skipSelfTags
    //      members (<svg>, <object>), and the pinch-zoom narrowing - returns
    //      FILTER_SKIP instead, so the subtree is still walked and the element
    //      set is unchanged.
    //   2. rect and style are measured once and carried forward. The old code
    //      measured in isElementInViewport/isElementVisible and then measured
    //      again in calculateElementArea.
    //   3. No text extraction. The old snapshot stored getCleanText() output
    //      that compareSnapshots never read.
    function collectViewportElements(ctx) {
        const out = [];
        const win = ctx.window;
        const viewportW = win.innerWidth;
        const viewportH = win.innerHeight;
        // The SAME expression VisibilityTracker.maxReasonableArea() evaluates,
        // with ctx.viewportArea standing in for its getViewportArea(): the
        // caller's measure first, then visualViewport, then the layout box, all
        // floored by calculateViewportArea()'s own Math.max(1, ...) before the
        // 50x multiplier.
        //
        // The two used to disagree by orders of magnitude in the degenerate
        // case - the tracker floored at 1 (clamp 50) while this multiplied a
        // zeroed layout box straight through (clamp 0). A clamp of 0 makes
        // `area > max` true for EVERY element, so every collected element is
        // worth 0, every rate is 0, and the traversal goes silent on a page the
        // tracker would still measure. Since the ladder is supposed to be a
        // slower detector rather than a different one, both paths now compute
        // the number the same way.
        let clampBase = ctx.viewportArea > 0 ? ctx.viewportArea : 0;
        if (!(clampBase > 0)) {
            // Guarded dereference: engines without visualViewport must fall
            // through to the window box, not throw on the path that feeds
            // every changeRate.
            const clampVp = win.visualViewport;
            if (clampVp && clampVp.width > 0 && clampVp.height > 0) {
                clampBase = clampVp.width * clampVp.height;
            } else {
                clampBase = (viewportW || 0) * (viewportH || 0);
            }
        }
        const maxReasonableArea = Math.max(1, clampBase) * 50;

        // Read into a local and call it bare. ctx.getComputedStyle is the
        // window-detached window.getComputedStyle; calling it as a method of
        // ctx would pass ctx as the WebIDL thisValue and throw
        // "Illegal invocation". A bare call leaves thisValue undefined, which
        // WebIDL replaces with the global - the same shape isElementVisible()
        // and calculateElementArea() already use for _getComputedStyle.
        const readStyle = ctx.getComputedStyle;

        // Carries the measured rect/style from acceptNode to the loop body so
        // neither is computed twice.
        let pendingRect = null;
        let pendingOpacity = 1;

        // Both sets come from ctx, symmetrically. Reading the closure constants
        // instead would silently ignore a caller-supplied set, changing the
        // element set and therefore the changeRate.
        const skipTags = ctx.skipTags;
        const skipSelfTags = ctx.skipSelfTags;

        const filter = {
            acceptNode: function (node) {
                const tagName = node.tagName.toLowerCase();
                if (skipTags.has(tagName)) {
                    return NodeFilter.FILTER_REJECT;
                }
                if (skipSelfTags.has(tagName)) {
                    // Self-only: <svg> and <object> are not collected, but
                    // their laid-out descendants (an SVG <text>, an <object>'s
                    // fallback content) were collected by the old walk and
                    // still are.
                    return NodeFilter.FILTER_SKIP;
                }
                const style = readStyle(node);
                if (style.display === 'none') {
                    return NodeFilter.FILTER_REJECT;
                }
                if (style.visibility === 'hidden') {
                    // visibility is inherited but a descendant can override it
                    // back to visible, so skip this node without pruning.
                    return NodeFilter.FILTER_SKIP;
                }
                const opacity = parseFloat(style.opacity);
                const resolvedOpacity = isNaN(opacity) ? 1 : opacity;
                if (resolvedOpacity <= 0.1) {
                    // SKIP, not REJECT, and that now includes opacity exactly 0.
                    //
                    // opacity is NOT inherited, so a descendant of a 0.05 - or
                    // of a 0 - container computes to "1", and the shipped code
                    // collected it at full area: isElementVisible() tested only
                    // the element itself, and getViewportElements() walked with
                    // a null NodeFilter that pruned nothing. VisibilityTracker
                    // does the same, because nothing on its observe path prunes
                    // an opacity:0 subtree (see opacityOf()'s note).
                    //
                    // Rejecting the opacity:0 subtree here made this traversal a
                    // different detector rather than a slower one: on an engine
                    // without IntersectionObserver, or once the tracker
                    // degrades, every page with an opacity:0 pre-animation
                    // wrapper was measured with its text children missing -
                    // silently, permanently, and in the under-report direction.
                    // Skip this node only, matching the old isElementVisible
                    // rule, and keep walking the subtree.
                    return NodeFilter.FILTER_SKIP;
                }

                let rect;
                try {
                    rect = node.getBoundingClientRect();
                } catch (error) {
                    // The old calculateElementArea() wrapped this read in a
                    // try/catch returning 0. Without per-element containment a
                    // single bad element throws out of the walker, kills the
                    // whole snapshot, and with it the reporting path.
                    return NodeFilter.FILTER_SKIP;
                }
                let inViewport = rect.top < viewportH && rect.bottom > 0 &&
                    rect.left < viewportW && rect.right > 0;
                // Old isElementInViewport() narrowed to the visual viewport once
                // the user pinch-zoomed away from the scale captured at
                // construction. The `vp &&` guard matches the shape the plan's
                // later tasks adopt for the constructor and for the surviving
                // isElementInViewport: once the constructor's dereference is
                // guarded, nothing else touches visualViewport before a
                // snapshot, and an unguarded read here would throw on the hot
                // path on any engine without it - killing all reporting.
                const vp = win.visualViewport;
                if (inViewport && vp && vp.scale !== ctx.windowScale) {
                    inViewport = !(
                        rect.right < vp.offsetLeft ||
                        rect.left > (vp.offsetLeft + vp.width) ||
                        rect.bottom < vp.offsetTop ||
                        rect.top > (vp.offsetTop + vp.height)
                    );
                }
                if (!inViewport) {
                    // SKIP, not REJECT: the old code narrowed per element and a
                    // descendant can sit inside the visual viewport when its
                    // parent does not.
                    return NodeFilter.FILTER_SKIP;
                }
                if (!ctx.textProbe.hasText(node)) {
                    return NodeFilter.FILTER_SKIP;
                }

                pendingRect = rect;
                pendingOpacity = resolvedOpacity;
                return NodeFilter.FILTER_ACCEPT;
            }
        };

        const walker = ctx.document.createTreeWalker(
            ctx.document.body,
            NodeFilter.SHOW_ELEMENT,
            filter,
            false
        );

        let node;
        while ((node = walker.nextNode())) {
            const rect = pendingRect || node.getBoundingClientRect();
            const opacity = pendingOpacity;
            pendingRect = null;
            pendingOpacity = 1;

            const width = Math.max(0, rect.width || 0);
            const height = Math.max(0, rect.height || 0);
            if (width <= 0 || height <= 0) {
                continue;
            }
            let area = width * height;
            if (area > maxReasonableArea) {
                // Old calculateElementArea() returned the clamp from an early
                // return, before reading opacity. Keep the clamp unscaled so an
                // oversized element reports the same area it always has.
                area = maxReasonableArea;
            } else {
                area = area * opacity;
            }
            if (area <= 0) {
                continue;
            }

            out.push({
                key: ctx.keyCache.keyOf(node),
                element: node,
                area: area,
                rect: rect
            });
        }

        return out;
    }

    // Distance beyond the viewport, in viewport heights, at or past which a
    // target is unobserved. Infinite-scroll pages only ever add DOM, so without
    // eviction the observer's target list grows without bound.
    //
    // Eviction is one-way: nothing re-observes an element that scrolls back
    // into view, because no entry is delivered for a target the observer no
    // longer holds, so the live set decays on a long static page and
    // `generation` stops advancing - which reads as "no content changed" and
    // fails toward silence. Mutation-driven observeSubtree covers only nodes
    // that are newly inserted, not ones that were merely scrolled past.
    // reseed() is the repair, and Task 7's Rule 1 is its one caller: see that
    // method for the contract, including why it is called only on the quiet
    // path and not on every long scroll.
    const EVICT_VIEWPORT_MULTIPLE = 2;

    // Maintains the set of currently-visible text elements using
    // IntersectionObserver, so that a "snapshot" is a key-set copy and a
    // "compare" is a set difference. Entry rects come from the browser's own
    // rendering lifecycle, so the hot path never calls getBoundingClientRect.
    class VisibilityTracker {
        constructor(opts) {
            this.window = opts.window;
            this.keyCache = opts.keyCache;
            this.textProbe = opts.textProbe;
            this.rootMargin = opts.rootMargin || '0px';
            this.onDegrade = opts.onDegrade || null;
            // The monitor's cached viewportArea, which is measured from
            // visualViewport and so is NOT innerWidth * innerHeight under
            // pinch-zoom. areaOf() needs it to clamp at the same threshold the
            // traversal clamps at.
            this.getViewportArea = opts.getViewportArea || null;

            // key -> { element, rect, area, opacity }, all of them as of the
            // last entry delivered for that element. `area` is the raw w*h,
            // before the clamp and before opacity; areaOf() is the value
            // consumers must use, and it needs no style read of its own.
            this.live = new Map();
            this.observed = new Set();   // elements currently observed
            this.pendingObserve = [];
            this.pendingUnobserve = [];
            this.flushScheduled = false;

            this.generation = 0;
            // Monotonic counter, bumped by observe(), by unobserve(), and by
            // stop() when it empties a non-empty `observed`. `generation`
            // tracks `live` and targetCount() is a level; neither can witness
            // an observe followed by an eviction, because the two cancel and
            // leave both readings exactly where they started. That pairing is
            // the normal infinite-scroll case - content is appended below the
            // fold, observed, and evicted on its first non-intersecting entry -
            // and it is precisely what must invalidate a cached "the region is
            // blank" answer. A counter that only ever climbs witnesses it.
            //
            // NOT a complete count of every mutation of `observed`. flush()
            // deletes from `observed` on two error paths (the no-observer
            // early return and the catch's un-marking of entries the observer
            // never received) without bumping this. Both are reachable only
            // when there is no observer or the tracker has just degraded, and
            // in both states reseed() answers -1, so the one consumer of this
            // counter never trusts a cached answer through them. Do not build
            // on the stronger reading: what is guaranteed is that a transition
            // this counter misses implies a tracker that cannot answer.
            this.observeChurn = 0;
            // Monotonic count of MutationObserver records DELIVERED to the
            // monitor, fed in by noteMutationRecords(). It lives here, next to
            // the other two counters, because Rule 1's reseed cache reads all
            // its keys off the tracker; the tracker itself never looks at it.
            //
            // It is a WITNESS, not a repair. A DOM insertion that never reaches
            // observeSubtree/observeTextChain is invisible to `generation`, to
            // `observeChurn` and to targetCount() alike - the information is
            // destroyed inside the tracker and no cache key can recover it. What
            // this does catch is the far likelier case: a record that WAS
            // delivered while the observe path silently failed to act on it. A
            // cached authoritative `gate-zero` must not survive across that.
            this.mutationRecords = 0;
            this.degraded = false;
            this.observer = null;
        }

        start() {
            const IO = this.window.IntersectionObserver;
            if (typeof IO !== 'function') {
                // Route through markDegraded rather than setting the flag: a
                // missing IntersectionObserver is the most likely degradation
                // in the field, and a consumer that wires its fallback ladder
                // through onDegrade would otherwise never hear about it.
                this.markDegraded(new Error('IntersectionObserver is unavailable'));
                return false;
            }
            const that = this;
            this.observer = new IO(function (entries) {
                that.handleEntries(entries);
            }, { root: null, rootMargin: this.rootMargin, threshold: 0 });
            return true;
        }

        handleEntries(entries) {
            // A batch queued before stop() can still be delivered after it;
            // without the observer check it would repopulate a live set that
            // stop() just cleared.
            if (this.degraded || !this.observer) {
                return;
            }
            try {
                const viewportH = this.window.innerHeight || 1;
                const evictBand = viewportH * EVICT_VIEWPORT_MULTIPLE;

                for (let i = 0; i < entries.length; i++) {
                    const entry = entries[i];
                    const element = entry.target;
                    if (!this.observed.has(element)) {
                        // Queued for unobserve but not flushed yet, so it is
                        // still a real IO target and still delivers entries.
                        // Without this the element re-enters `live` after
                        // unobserve() removed it, and nothing ever reports on
                        // it again: a permanently stale member holding a
                        // non-zero area.
                        continue;
                    }
                    const key = this.keyCache.keyOf(element);
                    if (key === 'error') {
                        continue;
                    }

                    if (entry.isIntersecting) {
                        const r = entry.boundingClientRect;
                        const area = Math.max(0, r.width) * Math.max(0, r.height);
                        if (area <= 0) {
                            // collectViewportElements drops width/height <= 0
                            // outright, but Blink reports a zero-area target
                            // inside the root as intersecting with ratio 0.
                            // Letting it in would make a region read as
                            // non-blank and suppress the blank-region report -
                            // the quiet direction.
                            if (this.live.delete(key)) {
                                this.generation++;
                            }
                            continue;
                        }
                        const existing = this.live.get(key);
                        // Opacity is captured HERE, on the entry path, where the
                        // cost is O(entries in this callback). areaOf() runs
                        // over the whole live set at every scroll start
                        // (Task 7's captureAreas), so a style read there would
                        // be a viewport-sized style storm on the scroll path -
                        // this plan's own defect, moved from the document to the
                        // viewport. Invariant: every key in `live` carries an
                        // opacity captured no earlier than the entry that put it
                        // there, refreshed on every subsequent entry, so any
                        // threshold crossing re-reads it.
                        const opacity = this.opacityOf(element);
                        if (!existing) {
                            this.live.set(key, {
                                element: element, area: area, rect: r, opacity: opacity
                            });
                            this.generation++;
                        } else {
                            // Geometry is a refresh; the opacity SENTINEL is
                            // not. A 0 means "the traversal would not have
                            // collected this at all", which snapshot() reads as
                            // absence, so crossing that boundary moves
                            // membership exactly as an add or a delete does and
                            // must move `generation` with it. Rule 1's gate-zero
                            // trusts `generation` as the membership counter.
                            existing.area = area;
                            existing.rect = r;
                            if ((existing.opacity === 0) !== (opacity === 0)) {
                                this.generation++;
                            }
                            existing.opacity = opacity;
                        }
                    } else {
                        if (this.live.delete(key)) {
                            this.generation++;
                        }
                        const r = entry.boundingClientRect;
                        const distance = r.top > 0 ? r.top - viewportH : -r.bottom;
                        // A detached element has no layout box, so its entry
                        // carries an all-zero rect and `distance` is 0: the
                        // geometry test alone can never evict it, and it stays
                        // pinned in `observed` and in the real target list for
                        // the page's lifetime. Key the extra test on
                        // disconnection, not on the rect - an all-zero rect is
                        // ambiguous (a collapsed but attached element reports
                        // the same thing) and a display:none element
                        // legitimately comes back. isConnected is a DOM
                        // property, not ES syntax, so the ES2017 constraint
                        // does not bite; engines predating it report undefined,
                        // where the distance test still governs.
                        if (element.isConnected === false || distance >= evictBand) {
                            this.unobserve(element);
                        }
                    }
                }
            } catch (error) {
                this.markDegraded(error);
            }
        }

        markDegraded(error) {
            if (this.degraded) {
                // Degradation is one-way and notified once, not once per
                // failing entry batch.
                return;
            }
            // Set first: everything below is best-effort, and this method runs
            // from inside the IntersectionObserver callback, where a throw
            // would escape onto the hot path.
            this.degraded = true;
            try {
                print({
                    type: 'debugLog',
                    data: {
                        log: 'VisibilityTracker degraded, falling back to traversal: ' +
                            (error && error.message ? error.message : String(error))
                    }
                });
                if (this.onDegrade) {
                    this.onDegrade(error);
                }
            } catch (ignored) {
                // Already degraded; the caller falls back to traversal either
                // way, and there is nothing left to report the failure to.
            }
        }

        observe(element) {
            // Once degraded there is no observer to hand these to, and
            // `observed` is a strong-reference Set: continuing to fill it on an
            // engine without IntersectionObserver would pin every text element
            // the page has ever rendered.
            if (this.degraded) {
                return;
            }
            if (!element || element.nodeType !== 1 || this.observed.has(element)) {
                return;
            }
            this.observed.add(element);
            this.observeChurn++;
            this.pendingObserve.push(element);
            this.scheduleFlush();
        }

        unobserve(element) {
            if (!element || !this.observed.has(element)) {
                return;
            }
            this.observed.delete(element);
            this.observeChurn++;
            this.pendingUnobserve.push(element);
            const key = this.keyCache.keyOf(element);
            if (this.live.delete(key)) {
                this.generation++;
            }
            this.scheduleFlush();
        }

        // A lazy batch can insert hundreds of text elements in one mutation
        // record. Coalescing into a microtask keeps that to one flush.
        scheduleFlush() {
            if (this.flushScheduled) {
                return;
            }
            this.flushScheduled = true;
            const that = this;
            // Read into a local and call it bare, as with getComputedStyle:
            // window.queueMicrotask invoked as a method of anything other than
            // the window throws "Illegal invocation".
            const schedule = this.window.queueMicrotask ||
                function (fn) { Promise.resolve().then(fn); };
            schedule(function () { that.flush(); });
        }

        flush() {
            this.flushScheduled = false;
            if (!this.observer) {
                // Nothing was actually observed, so `observed` must not keep
                // claiming these; otherwise targetCount() lies and the elements
                // are never collectable.
                for (let k = 0; k < this.pendingObserve.length; k++) {
                    this.observed.delete(this.pendingObserve[k]);
                }
                this.pendingObserve.length = 0;
                this.pendingUnobserve.length = 0;
                return;
            }
            // Declared outside the try: a `let` inside the block is not in
            // scope in the catch, and the catch needs both the failure point
            // and which loop actually threw.
            let i = 0;
            let observeLoopReached = false;
            try {
                // `observed` is the authority, not the arrival order. One batch
                // can hold both an evict and a re-seed of the same element -
                // Task 7's Rule 1 re-seeds straight after an eviction entry -
                // and replaying the queues in order would then observe it and
                // immediately unobserve it again, leaving the observer without
                // a target the tracker believes it holds.
                for (let j = 0; j < this.pendingUnobserve.length; j++) {
                    const dropped = this.pendingUnobserve[j];
                    if (!this.observed.has(dropped)) {
                        this.observer.unobserve(dropped);
                    }
                }
                observeLoopReached = true;
                for (; i < this.pendingObserve.length; i++) {
                    const added = this.pendingObserve[i];
                    if (this.observed.has(added)) {
                        this.observer.observe(added);
                    }
                }
            } catch (error) {
                // Un-mark only what the observer never received. If the throw
                // came from the observe loop, entries before `i` were handed
                // over successfully and are real IO targets: deleting them
                // from `observed` would make unobserve() - gated on
                // observed.has - unable to ever evict them (the target-list
                // leak T2 closed on the ordinary path, reopened on degraded).
                // From `i` onward nothing reached the observer, so `observed`
                // must not keep claiming them: targetCount() would over-report
                // a degraded tracker's reach.
                //
                // If the throw came from the UNOBSERVE loop instead,
                // `observeLoopReached` is false and the observe queue is left
                // unmarked. That over-claims temporarily - targetCount() may
                // count entries the observer does not yet hold - but those
                // entries may include re-seeds of elements already held from
                // an earlier flush (Task 7's Rule 1 shape). Un-marking them
                // would strand them permanently. Over-claiming is recoverable
                // (stop() clears both); stranding is not.
                if (observeLoopReached) {
                    for (let p = i; p < this.pendingObserve.length; p++) {
                        this.observed.delete(this.pendingObserve[p]);
                    }
                }
                this.markDegraded(error);
            }
            this.pendingObserve.length = 0;
            this.pendingUnobserve.length = 0;
        }

        observeSubtree(root) {
            if (!root || root.nodeType !== 1) {
                return;
            }
            try {
                const stack = [root];
                while (stack.length > 0) {
                    const node = stack.pop();
                    const tagName = node.tagName.toLowerCase();
                    // Same two-set split as collectViewportElements, and it must
                    // stay in step with it: the IO live set is what gates the
                    // blank-region check, so a node the traversal collects but
                    // the tracker never observes would read as blank. That
                    // agreement holds at the instant this runs and no longer: an
                    // element that gains text later is collected but was
                    // rejected here, which is what observeTextChain() below
                    // exists to repair.
                    // SKIP_TAGS: skip the node and its whole subtree.
                    if (SKIP_TAGS.has(tagName)) {
                        continue;
                    }
                    // SKIP_SELF_TAGS (<svg>, <object>): do not observe the
                    // element itself, but keep descending - their descendants
                    // are laid out and are collected.
                    if (!SKIP_SELF_TAGS.has(tagName) && !this.isWalkRoot(node) &&
                        this.textProbe.hasText(node)) {
                        this.observe(node);
                    }
                    const kids = node.children;
                    if (kids) {
                        for (let i = 0; i < kids.length; i++) {
                            stack.push(kids[i]);
                        }
                    }
                }
            } catch (error) {
                // Runs from a MutationObserver callback and from reseed() on
                // the scroll path; a throw either way kills all reporting.
                this.markDegraded(error);
            }
        }

        // observeSubtree's hasText gate is evaluated once, at the instant it
        // runs, so an element that gains text LATER is collected by
        // collectViewportElements but was never observed - the read-as-blank
        // direction, and the lazy-load case the plan's second global constraint
        // is about. The whole wrapper chain has the same problem, since hasText
        // gates each ancestor too and wrappers carry the largest areas.
        //
        // Climb from the changed element, observing each ancestor that has just
        // become text-bearing. The stop conditions and the reason the climb is
        // unconditional are on observeTextChain() below.
        //
        // The traversal walks from document.body, and a TreeWalker never
        // returns its own root, so neither the body nor the documentElement is
        // part of the element set. Observing one would put a page-sized,
        // permanently-intersecting member in the live set: every region would
        // read as non-blank forever and the blank-region gate would never fire.
        isWalkRoot(node) {
            const doc = this.window.document;
            if (!doc) {
                return false;
            }
            return node === doc.body || node === doc.documentElement;
        }

        // Climb from the changed element to the walk root, so the cost is the
        // depth of the change, not the size of the DOM. Task 8 must call this
        // from the text-change path; see the report's Produces block for which
        // call site and why handleTextChange alone is not sufficient.
        //
        // The climb deliberately does NOT stop at the first already-observed
        // ancestor, and must not be "optimized" to. That stop would only be
        // sound if everything in `observed` had arrived via a whole-body walk,
        // so that an observed node implied observed ancestors. Task 8 Step 9
        // calls observeSubtree(addedNode) on individual mutation-added nodes,
        // which observes a node WITHOUT its ancestors; stopping there would
        // leave a just-became-text-bearing wrapper chain permanently out of the
        // live set, under-counting every later changeRate - the quiet
        // direction, on the lazy-load path specifically. observe() is already
        // idempotent against `observed`, so climbing past costs one hasText per
        // ancestor and nothing else.
        observeTextChain(element) {
            if (!element || element.nodeType !== 1) {
                return;
            }
            try {
                const chain = [];
                let node = element;
                while (node && node.nodeType === 1 && !this.isWalkRoot(node)) {
                    const tagName = node.tagName.toLowerCase();
                    if (SKIP_TAGS.has(tagName)) {
                        // The traversal prunes this subtree whole, so nothing
                        // below it may be observed either: drop the chain, do
                        // not observe what was already gathered.
                        return;
                    }
                    if (!SKIP_SELF_TAGS.has(tagName)) {
                        chain.push(node);
                    }
                    node = node.parentElement;
                }
                for (let i = 0; i < chain.length; i++) {
                    if (this.textProbe.hasText(chain[i])) {
                        this.observe(chain[i]);
                    }
                }
            } catch (error) {
                // Destined for a MutationObserver callback: a throw here would
                // escape onto the hot path and kill all reporting.
                this.markDegraded(error);
            }
        }

        // Re-walk `root` (the document body by default) and observe everything
        // text-bearing that is not already observed, applying it immediately.
        // Idempotent: observe() is a no-op for anything already in `observed`,
        // so a second call in a row returns 0 and costs one walk.
        //
        // Returns:
        //    > 0  that many targets were brought back - the gate was lying
        //     0   nothing to bring back - the region really was blank
        //    -1   CANNOT bring anything back (degraded, stopped, not started,
        //         or the walk threw). Distinct from 0 on purpose: Rule 1 reads
        //         0 as authoritative silence, and answering 0 for "I cannot
        //         answer" would put that case on the silent side.
        //
        // Why this exists: distance eviction is one-way. Nothing re-observes a
        // target that scrolls back into view, because the observer delivers no
        // entries for a target it no longer holds, so on a long static page the
        // live set decays and `generation` stops advancing. Task 7's Rule 1 -
        // the one place in the design that treats "nothing changed" as
        // authoritative - must call this before it returns gate-zero, and must
        // treat any non-zero return as "do not trust the gate". The full walk
        // is paid only on that quiet path.
        reseed(root) {
            if (this.degraded || !this.observer) {
                return -1;
            }
            const doc = this.window.document;
            const target = root || (doc ? doc.body : null);
            if (!target) {
                return -1;
            }
            const before = this.observed.size;
            try {
                this.observeSubtree(target);
                this.flush();
            } catch (error) {
                // R11 put this on the scroll path, where a throw propagates out
                // of Task 7's endScroll and through the scroll handler.
                this.markDegraded(error);
                return -1;
            }
            // observeSubtree contains its own throws by degrading, so re-check:
            // a degraded tracker cannot vouch for the count.
            if (this.degraded) {
                return -1;
            }
            return this.observed.size - before;
        }

        unobserveSubtree(root) {
            if (!root || root.nodeType !== 1) {
                return;
            }
            try {
                const stack = [root];
                while (stack.length > 0) {
                    const node = stack.pop();
                    this.unobserve(node);
                    // Deliberately no keyCache.forget(): this runs from
                    // mutation.removedNodes, and a MOVE is a removal plus an
                    // insertion. Forgetting a moved node re-keys it on the way
                    // back in, so its area counts once as disappeared and once
                    // as appeared - a manufactured report. keyCache is a
                    // WeakMap, so a genuinely detached element is collected
                    // without help, which is why Task 2 chose one.
                    const kids = node.children;
                    if (kids) {
                        for (let i = 0; i < kids.length; i++) {
                            stack.push(kids[i]);
                        }
                    }
                }
            } catch (error) {
                // Runs from a MutationObserver callback; a throw kills all
                // reporting.
                this.markDegraded(error);
            }
        }

        // Re-reads the opacity gate for every live entry and writes it back, so
        // that the key-set snapshot() returns is a membership FACT at the
        // moment the set is read rather than a cache left over from the last
        // IntersectionObserver entry.
        //
        // Why it has to exist: IntersectionObserver v1 delivers nothing for a
        // `visibility` or `opacity` change that moves no geometry, and this
        // branch does not use v2's trackVisibility. Without this sweep a
        // visibility:hidden panel revealed by a click keeps its stale 0
        // sentinel, sits in BOTH the start and the end key-set, and contributes
        // to neither the disappeared nor the appeared term - because
        // diffRateWith and residualRate are purely membership-driven. The
        // shipped original re-walks the DOM at every snapshot and so books the
        // reveal as an appearance. Both directions failed quiet: a reveal lost
        // an `appeared` term and a hide lost a `disappeared` one. The
        // MutationObserver runs with attributes:false, so there is no mitigating
        // path - neither a style write nor a class swap produces a record.
        //
        // Cost is O(live set) - viewport-sized - at exactly the two moments
        // (interaction start and interaction end) at which the shipped original
        // ran an O(document) TreeWalker with a getComputedStyle per candidate.
        // It is strictly cheaper than the code it replaces. Do NOT move it onto
        // the entry path or the observe path: a per-node style read there is the
        // document-wide style storm this design exists to avoid, and do NOT put
        // it in areaOf(), which callers run in a loop.
        //
        // A crossing of the sentinel bumps `generation` for the same reason
        // handleEntries does: `generation` is the membership counter Rule 1's
        // gate-zero trusts, and a reveal that moves no geometry would otherwise
        // leave it unmoved, let reseed() recover nothing, and earn the one
        // verdict the design calls authoritative for rate 0.
        refreshOpacity() {
            if (this.degraded) {
                return;
            }
            const that = this;
            this.live.forEach(function (entry) {
                const opacity = that.opacityOf(entry.element);
                if ((entry.opacity === 0) !== (opacity === 0)) {
                    that.generation++;
                }
                entry.opacity = opacity;
            });
        }

        // null, not a Set, once degraded: `live` freezes non-empty and
        // `generation` stops advancing, so a consumer that reads the gate
        // without checking `degraded` would silently trust a stale set and go
        // permanently quiet - fail-toward-silence, which the plan forbids. A
        // null forces the caller onto the fallback ladder instead.
        //
        // Keys carrying the opacityOf() 0 sentinel are EXCLUDED. The sentinel
        // means "collectViewportElements would not have collected this", which
        // is absence, not area 0, and the traversal encodes it as absence.
        // Leaving such a key in the set put it in both snapshots, where every
        // rate path scores it zero whatever its area - so a page-sized hidden
        // wrapper being revealed was worth nothing at all. Excluding it makes
        // start-side exclusion and end-side inclusion move membership exactly
        // as the traversal's gates do, in both directions.
        snapshot() {
            if (this.degraded) {
                return null;
            }
            this.refreshOpacity();
            const keys = new Set();
            this.live.forEach(function (entry, key) {
                if (entry.opacity !== 0) {
                    keys.add(key);
                }
            });
            return keys;
        }

        // The same arithmetic collectViewportElements applies, in the same
        // order. areaOf feeds disappeared/appeared, which become changeRate,
        // which is compared against changeThreshold: an element worth 2000 here
        // and 1000 there decides reports differently through the two paths.
        //
        // Pure arithmetic over the cached rect and the cached opacity: no style
        // read, no layout read. Task 7 calls this over the entire live set at
        // every scroll and click start.
        areaOf(key) {
            const entry = this.live.get(key);
            if (!entry) {
                return 0;
            }
            const r = entry.rect;
            const width = Math.max(0, (r && r.width) || 0);
            const height = Math.max(0, (r && r.height) || 0);
            if (width <= 0 || height <= 0) {
                return 0;
            }
            const area = width * height;

            // The opacity gate runs BEFORE the clamp, because that is the order
            // the traversal uses: collectViewportElements rejects display:none
            // and skips visibility:hidden / opacity <= 0.1 in acceptNode,
            // before it measures anything, so such an element is absent from
            // the element set and never reaches the clamp at all.
            //
            // entry.opacity === 0 is a SENTINEL meaning "the traversal would
            // not have collected this", not "present but invisible" - see
            // opacityOf(). Testing the clamp first returned max from inside its
            // branch and threw the sentinel away with it, so a page-sized
            // visibility:hidden wrapper was worth the full clamp where the
            // traversal is worth 0. IntersectionObserver ignores visibility
            // entirely, so such a wrapper keeps delivering intersecting entries
            // and stays in `live` indefinitely: fed to a changeRate that is 1.0
            // against a 0.15 threshold, a guaranteed report at a moment the old
            // code was silent.
            if (entry.opacity === 0) {
                return 0;
            }

            const max = this.maxReasonableArea();
            if (area > max) {
                // Past the gate, the clamp is returned UNSCALED: a real opacity
                // scale (0.5, say) is deliberately not applied here, matching
                // the traversal's early return from calculateElementArea.
                return max;
            }
            return area * entry.opacity;
        }

        // The threshold collectViewportElements clamps against. Task 8 Step 4
        // must pass getViewportArea so both paths clamp at the same number; the
        // fallback below exists because a missing one must not throw or zero
        // the clamp, and it mirrors calculateViewportArea()'s own measure
        // rather than the window box - visualViewport first, because at pinch
        // scale 2 the two differ by 4x on the value that decides whether a
        // comparison crosses changeThreshold.
        maxReasonableArea() {
            let base = 0;
            if (this.getViewportArea) {
                try {
                    base = this.getViewportArea();
                } catch (error) {
                    base = 0;
                }
            }
            if (!(base > 0)) {
                // Guarded dereference: engines without visualViewport must fall
                // through to the window box, not throw on a path that feeds
                // every changeRate.
                const vp = this.window.visualViewport;
                if (vp && vp.width > 0 && vp.height > 0) {
                    base = vp.width * vp.height;
                } else {
                    base = (this.window.innerWidth || 0) * (this.window.innerHeight || 0);
                }
            }
            // Math.max(1, base) is calculateViewportArea()'s own floor, in both
            // of its branches. Without it a zeroed viewport - an iframe before
            // layout, a hidden document, no getViewportArea and no
            // visualViewport - returns 0, and then `area > max` is true for
            // EVERY element in areaOf and every one of them returns max === 0.
            // Membership is untouched, so the blank-region gate does not fire;
            // instead every diffRate term is 0 and no report is emitted where
            // the original code would emit one. Fail-toward-silence through
            // magnitude instead of membership.
            return Math.max(1, base) * 50;
        }

        // Returns one of two kinds of number, and the distinction matters at
        // every call site:
        //   > 0  an actual opacity scale, to multiply an area by;
        //   0    a SENTINEL meaning "collectViewportElements would not have
        //        collected THIS ELEMENT" - display:none, visibility:hidden, or
        //        opacity <= 0.1. NOT "present but invisible". Anything that
        //        consumes this value must test the sentinel before any other
        //        area arithmetic, exactly as the traversal runs its gates in
        //        acceptNode before it measures. See areaOf(), where testing the
        //        clamp first once bypassed it.
        //
        // The sentinel is SELF-ONLY, and there is deliberately no subtree
        // equivalent. acceptNode prunes a whole opacity === 0 subtree
        // (FILTER_REJECT, see the comment there); nothing on the observe path
        // does, so a text child of an opacity:0 wrapper stays in `live` at full
        // area while the traversal collects neither. That is not a divergence
        // from the SHIPPED baseline, which is what equivalence is measured
        // against: opacity is not inherited, so the original isElementVisible()
        // computed opacity 1 for that child and collected it at full area, and
        // the original getViewportElements() walked with a null NodeFilter and
        // pruned nothing. The subtree prune is Task 4's own accepted divergence
        // (the accepted "prune the subtree only when opacity === 0" ledger
        // entry), and its direction is UNDER-report; declining to adopt it
        // here is the safe side. Do not
        // "fix" this by making observeSubtree/observeTextChain refuse to descend
        // past opacity:0 - that is a style read per node on the observe path,
        // which is the cost R10 exists to avoid, and it would move the tracker
        // away from the shipped baseline to chase a Task 4 divergence.
        //
        // Called from the entry path, once per entry, and from refreshOpacity()
        // over the whole live set at every snapshot(). The accepted cost of the
        // entry-path placement is staleness between snapshots: an opacity
        // animated by a CSS transition that crosses no IntersectionObserver
        // threshold keeps the value from its last entry until the next
        // snapshot() refreshes it, where the old code re-read it on every
        // snapshot. For a real opacity SCALE that error is bounded and
        // bidirectional - a fade-in under-counts by as much as a fade-out
        // over-counts - and it is a magnitude error only.
        //
        // The SENTINEL half was neither bounded nor bidirectional, and it was a
        // MEMBERSHIP error rather than a magnitude one: a visibility:hidden or
        // display:none element revealed with unchanged geometry delivers no new
        // IO entry, so the cached 0 persisted, the key stayed in both snapshots,
        // and every rate path scored it zero whatever its area - systematically
        // quiet in both directions. That is why snapshot() now calls
        // refreshOpacity() before it reads the set, and why it excludes the
        // sentinel from the key-set. See refreshOpacity().
        opacityOf(element) {
            // _getComputedStyle is the window-detached window.getComputedStyle.
            // Call it BARE: the method form passes the receiver as the WebIDL
            // thisValue, fails the Window brand check and throws
            // "Illegal invocation" in a real browser, while passing every unit
            // test against a plain harness function.
            if (!_getComputedStyle) {
                return 1;
            }
            try {
                const style = _getComputedStyle(element);
                if (!style) {
                    return 1;
                }
                // Both gates the traversal applies before it collects, and the
                // same two the shipped isElementVisible() applies before it
                // measures. An element hidden after its last IO entry is still
                // in `live` - IO delivery is asynchronous relative to the style
                // change - and must not contribute a stale area.
                if (style.display === 'none' || style.visibility === 'hidden') {
                    return 0;
                }
                const opacity = parseFloat(style.opacity);
                // A disconnected element has an empty computed style, so
                // parseFloat('') is NaN. Resolve to 1, matching the traversal's
                // own isNaN handling.
                const resolved = isNaN(opacity) ? 1 : opacity;
                // The traversal excludes these from the element set entirely,
                // so their area must not reach a changeRate through this path.
                return resolved <= 0.1 ? 0 : resolved;
            } catch (error) {
                // A throw here would kill all reporting. An unknowable opacity
                // is 1, same as the NaN case above.
                return 1;
            }
        }

        rectOf(key) {
            const entry = this.live.get(key);
            return entry ? entry.rect : null;
        }

        targetCount() {
            return this.observed.size;
        }

        // Called by the monitor's MutationObserver callback with the length of
        // the batch it was handed, BEFORE any re-entrancy guard drops it: a
        // record the monitor declined to process is still a record the page
        // produced, and the point of the counter is to stop a cached
        // authoritative zero from outliving evidence that the DOM moved.
        // Guarded against a non-number so a bad caller cannot poison the
        // counter into NaN, where every later comparison would be false and the
        // witness would silently stop witnessing.
        noteMutationRecords(count) {
            if (typeof count === 'number' && count > 0) {
                this.mutationRecords += count;
            }
        }

        stop() {
            if (this.observer) {
                this.observer.disconnect();
                this.observer = null;
            }
            this.live.clear();
            // Emptying `observed` is a membership transition like any other,
            // and the one consumer of observeChurn - Task 7's Rule 1 cache -
            // is keyed on it precisely so that a cached "the region is blank"
            // answer cannot outlive the evidence for it. Clearing the set
            // silently would leave that cache stamped at a churn value that no
            // longer describes anything: generation is unmoved, snapshot()
            // still returns a (now empty, still truthy) Set because `degraded`
            // is false, and the stale 0 replays as gate-zero - a positive
            // claim of blankness about a tracker whose own reseed() would now
            // answer -1, "I cannot answer". Bumping here makes the counter
            // self-consistent and closes that door inside the tracker, rather
            // than relying on every consumer to remember to reset() on a
            // stop/restart boundary.
            //
            // This closes the cached-ZERO door only. live.clear() above is the
            // one place `live` empties without moving `generation`, so a
            // consumer holding a cached NON-zero still skips the refresh (its
            // key is generation-only) and answers from a walk that described a
            // tracker which no longer exists: a stranded node makes it report
            // a band where a fresh walk gives a smaller one, and a sub-scroller
            // makes it return 'diff' rate 0 where a fresh walk gives
            // 'unanswerable' - losing the escalation signal, which is the
            // silent direction. A consumer that stops and restarts a tracker
            // must still call its own reset() on that boundary.
            if (this.observed.size > 0) {
                this.observeChurn++;
            }
            this.observed.clear();
            this.pendingObserve.length = 0;
            this.pendingUnobserve.length = 0;
            this.flushScheduled = false;
        }
    }

    // Tracks document scroll offset between mark() and delta(), and notices
    // when an inner overflow:auto element scrolled instead of the document.
    // Inner-scroller events do not bubble, but they are observable during the
    // capture phase at window — without this, an inner scroll would show
    // dx/dy of 0 and be silently under-reported.
    class ScrollMetrics {
        constructor(win) {
            this.window = win;
            this.originX = 0;
            this.originY = 0;
            this.subScroller = false;
        }

        currentX() {
            if (typeof this.window.scrollX === 'number') {
                return this.window.scrollX;
            }
            const docEl = this.window.document && this.window.document.documentElement;
            return docEl && typeof docEl.scrollLeft === 'number' ? docEl.scrollLeft : 0;
        }

        currentY() {
            if (typeof this.window.scrollY === 'number') {
                return this.window.scrollY;
            }
            const docEl = this.window.document && this.window.document.documentElement;
            return docEl && typeof docEl.scrollTop === 'number' ? docEl.scrollTop : 0;
        }

        mark() {
            this.originX = this.currentX();
            this.originY = this.currentY();
            this.subScroller = false;
        }

        noteScrollEvent(event) {
            if (!event) {
                return;
            }
            const target = event.target;
            // A document/window-level scroll has target === document.
            if (target && target.nodeType === 1) {
                this.subScroller = true;
            }
        }

        delta() {
            return {
                dx: this.currentX() - this.originX,
                dy: this.currentY() - this.originY,
                subScroller: this.subScroller
            };
        }

        reset() {
            this.originX = this.currentX();
            this.originY = this.currentY();
            this.subScroller = false;
        }
    }

    // Chooses how a change rate is derived and computes it.
    //
    // Path 1 (band): for a plain document scroll the newly-exposed region is
    //   exactly min(|dy|, H) tall, so the rate has a closed form and costs
    //   O(1). Lazy content landing inside that band is already accounted for;
    //   lazy content in the retained region is added as a residual.
    // Path 2 (diff): a set difference over the live set, O(changes), using
    //   IO-cached rects.
    // Path 3 (single element): unchanged from the original, in the monitor.
    //
    // The `path` returned alongside every rate names how it was derived:
    //
    //   'gate-zero'    membership did not change and a body walk confirmed the
    //                  live set is not lying. rate 0.
    //   'diff'         set difference over the live set. rate measured.
    //   'band'         closed-form scroll band plus retained-region residual.
    //   'band-only'    the live set could not answer, but the page moved, so
    //                  the band alone is a real lower bound. rate measured.
    //   'no-motion'    nothing moved: no page delta and no sub-scroller. rate 0.
    //   'unanswerable' something moved that this layer cannot measure - an
    //                  overflow:auto region scrolled, or the tracker is blind.
    //                  rate 0 because nothing is measurable, NOT because
    //                  nothing changed.
    //
    // CONTRACT: `rate === 0` is an authoritative claim about the page ONLY for
    // 'gate-zero'. 'no-motion' and 'unanswerable' both carry rate 0 for
    // entirely different reasons - "there was nothing to measure" and "there
    // was something to measure and I could not". A consumer that treats every
    // zero as "nothing changed" will go silent exactly when it should escalate
    // to the traversal fallback.
    class ChangeRateCalculator {
        constructor(opts) {
            this.window = opts.window;
            this.tracker = opts.tracker;
            this.metrics = opts.metrics;
            this.getViewportArea = opts.getViewportArea;

            this.scrollStartSet = null;
            this.scrollStartGeneration = 0;
            this.scrollStartAreas = null;
            this.reseedGeneration = -1;
            this.reseedChurn = -1;
            this.reseedMutations = 0;
            this.reseedResult = 0;
            this.clickStartSet = null;
            this.clickStartAreas = null;
        }

        // The tracker's delivered-mutation counter, or 0 when the tracker does
        // not carry one. Defaulting to 0 rather than to something that always
        // differs matters: a tracker with no counter would otherwise invalidate
        // the re-seed cache on every quiet scroll and reinstate the O(document)
        // walk-per-scroll cost the cache exists to avoid. 0 reproduces exactly
        // the pre-witness behaviour for such a tracker, and nothing more.
        mutationWitness() {
            const count = this.tracker.mutationRecords;
            return typeof count === 'number' ? count : 0;
        }

        // Areas must be captured when the set is captured: an element that
        // later leaves the live set no longer has an entry to read.
        //
        // Every caller takes its keySet from snapshot(), which has just run
        // refreshOpacity() over the whole live set, so the opacity areaOf()
        // reads here is as fresh as the membership the key-set encodes. Do not
        // call this with a key-set from anywhere else without refreshing first,
        // or the areas and the membership will describe two different instants.
        captureAreas(keySet) {
            const areas = new Map();
            const that = this;
            keySet.forEach(function (key) {
                areas.set(key, {
                    area: that.tracker.areaOf(key),
                    rect: that.tracker.rectOf(key)
                });
            });
            return areas;
        }

        beginScroll() {
            this.metrics.mark();
            // snapshot() is null once the tracker is degraded. Null must not
            // reach captureAreas(), whose forEach would throw inside the
            // touchstart handler and leave scrollStartSet null forever — every
            // later scroll would then read as gate-zero, which is the silence
            // the null return exists to prevent.
            const startSet = this.tracker.snapshot();
            this.scrollStartSet = startSet;
            this.scrollStartAreas = startSet ? this.captureAreas(startSet) : null;
            this.scrollStartGeneration = this.tracker.generation;
        }

        endScroll() {
            const delta = this.metrics.delta();
            const endSet = this.scrollStartSet ? this.tracker.snapshot() : null;

            // Degraded, or no matching beginScroll. Either way the live set
            // cannot answer, so fall through to the geometric path rather than
            // reporting zero: a tracker that cannot see must not be able to
            // silence the scroll.
            if (!this.scrollStartSet || !endSet) {
                this.scrollStartSet = null;
                return this.geometricFallback(delta);
            }

            // Rule 1 — nothing entered or left: blank or image-only region.
            // Checked before everything else, including inner scrollers.
            //
            // An unchanged generation is not evidence on its own. Distance
            // eviction is one-way, so on a long static page the live set decays
            // and generation stops advancing for a reason that has nothing to
            // do with the page being blank. Re-seed and re-read before
            // believing it: a non-zero return means the gate was lying and the
            // scroll falls through to the geometric path below.
            //
            // `reseed()` is three-valued and the three cases are NOT ordered, so
            // test for 0 exactly rather than `<= 0`. Only 0 means "walked the
            // body, found nothing to bring back" — authoritative blankness, the
            // one answer that may silence the scroll. -1 means the tracker
            // cannot answer (no body yet, or it degraded during the walk, which
            // is reachable here: `snapshot()` was still non-null when
            // beginScroll ran). Reading -1 as blankness would let a tracker that
            // has just gone blind suppress a report the old code would emit.
            //
            // The full walk is paid only here, on the quiet path, and at most
            // once per membership-change epoch: reseedGeneration is stamped
            // after the walk, so a second quiet scroll with nothing changed in
            // between skips it. Without that guard the tick after each re-seed
            // re-evicts what the walk brought back and the next quiet scroll
            // walks again — turning "paid only on the quiet path" into "paid on
            // every quiet path".
            //
            // The skip must replay the epoch's ACTUAL answer, not assume 0.
            // Defaulting the skipped case to 0 would mean: the first quiet
            // scroll of an epoch gets -1 and correctly falls to the band, and
            // every later quiet scroll in that same epoch reads a manufactured
            // 0 and returns gate-zero — the exact silence the -1 branch exists
            // to prevent, re-entered through the cache. reseedGeneration starts
            // at -1 so the first walk is never skipped (generation starts at 0).
            //
            // The cache is keyed on observeChurn as well as generation, because
            // generation alone lets a stale 0 replay for the rest of an epoch.
            // `observed` can lose a target without generation moving: a node
            // whose first IO entry is non-intersecting and far off-screen is
            // evicted by handleEntries, and since it was never in `live` the
            // delete returns false and nothing bumps generation. That is the
            // ordinary infinite-scroll case, and replaying the cached 0 through
            // it returns gate-zero - a positive claim of blankness - about a
            // page that is actively appending content.
            //
            // observeChurn rather than targetCount(): the append and the
            // eviction cancel, so the count is a level that reads identically
            // before and after. Only a monotonic transition count sees it.
            // Every targetCount() change is also a churn change, so this
            // strictly dominates.
            //
            // Churn invalidates a cached ZERO only. That restriction is what
            // keeps the walk O(1)-amortised, and it is not a weakening:
            //
            //   - A cached 0 is the only answer that can silence a scroll, so
            //     it is the only one whose staleness can suppress a report.
            //     Churn is exactly the evidence that it may be stale.
            //   - A cached > 0 or < 0 already routes to a measurement (Rule
            //     2/3/4 for > 0, the geometric fallback for < 0), so
            //     re-walking to refresh it cannot turn silence into a report -
            //     there was no silence.
            //
            // Skipping that refresh is NOT free, and an earlier version of this
            // comment claimed it was. Re-walking a stale > 0 can answer a fresh
            // 0, so declining to re-walk keeps a scroll reporting where a fresh
            // walk would have said gate-zero. That is over-reporting, which the
            // equivalence contract permits and the shipped original does too
            // (it has no such cache and re-measures every scroll), whereas the
            // suppression a stale 0 causes is the direction the contract
            // forbids. The restriction is a deliberate trade of a noisier
            // answer for a bounded cost, not a free lunch.
            //
            // The premise above - that an unmoved generation means the start
            // and end sets are equal - holds everywhere except stop(), whose
            // live.clear() is the one place `live` empties without moving
            // generation. See the note on observeChurn in stop().
            //
            // Without the restriction the key re-arms itself and the walk is
            // paid on EVERY quiet scroll of any page taller than ~2 viewports.
            // reseed() re-observes the distance-evicted nodes (churn += N),
            // the observer delivers their initial non-intersecting entries,
            // handleEntries evicts them again (churn += N), and generation
            // never moves throughout because those nodes were never in `live`.
            // Measured on a 200-paragraph article: 6 walks and 1170
            // re-observes across 6 quiet scrolls, against 1 and 195 here.
            // The fourth key is a DELIVERED-MUTATION witness, and it exists for
            // the one hole the other three cannot see. A DOM insertion that
            // never reaches observeSubtree/observeTextChain moves `generation`,
            // `observeChurn` and targetCount() not at all, so within one epoch a
            // second quiet scroll replays the cached authoritative 0 about a
            // page that has actually gained content. No cache key can witness a
            // record that was never delivered - that information is destroyed
            // inside the tracker - but a record that WAS delivered and whose
            // observe path then failed is both likelier and visible, and this
            // counter sees it. Like observeChurn it invalidates a cached ZERO
            // only, for the same reason: a cached 0 is the only answer that can
            // silence a scroll, and every other answer already routes to a
            // measurement.
            //
            // It does NOT change what gate-zero means. reseed() still decides;
            // this only stops a stale gate-zero from outliving the evidence
            // that the DOM moved underneath it.
            if (this.tracker.generation === this.scrollStartGeneration) {
                const mutations = this.mutationWitness();
                if (this.reseedGeneration !== this.tracker.generation ||
                        (this.reseedResult === 0 &&
                         (this.reseedChurn !== this.tracker.observeChurn ||
                          this.reseedMutations !== mutations))) {
                    this.reseedResult = this.tracker.reseed();
                    this.reseedGeneration = this.tracker.generation;
                    this.reseedChurn = this.tracker.observeChurn;
                    // Stamp the value that was READ before the walk, never a
                    // fresher one: a record delivered while the walk ran must
                    // still invalidate the answer the walk produced.
                    this.reseedMutations = mutations;
                }
                const recovered = this.reseedResult;
                if (recovered === 0) {
                    this.scrollStartSet = null;
                    return { rate: 0, path: 'gate-zero' };
                }
                if (recovered < 0) {
                    // Cannot answer: fall to the geometric path, the same
                    // fallback the null-snapshot branch above takes.
                    this.scrollStartSet = null;
                    return this.geometricFallback(delta);
                }
            }

            // Rule 2 — an inner overflow:auto element scrolled; dx/dy does
            // not describe that motion.
            // Rule 3 — no page motion at all, so the band is empty.
            if (delta.subScroller || (delta.dx === 0 && delta.dy === 0)) {
                const rate = this.diffRateWith(
                    this.scrollStartSet, endSet, this.scrollStartAreas);
                this.scrollStartSet = null;
                return { rate: rate, path: 'diff' };
            }

            // Rule 4 — band plus retained-region residual.
            const band = this.bandRate(delta.dy, delta.dx);
            const residual = this.residualRate(
                this.scrollStartSet, endSet, delta.dy, this.scrollStartAreas);
            this.scrollStartSet = null;
            return { rate: Math.min(band + residual, 1), path: 'band' };
        }

        // The one place the unanswerable cases are spelled out, shared by the
        // null-snapshot branch and the reseed()-of--1 branch so the two cannot
        // drift apart.
        //
        // Order matters. Real page motion is checked FIRST, even when a
        // sub-scroller also moved: bandRate is a genuine lower bound on how
        // much of the viewport turned over, and discarding it in favour of
        // rate 0 would be exactly the bias toward silence this class exists to
        // avoid. Only once the page itself is known not to have moved does
        // subScroller decide between "an overflow:auto region scrolled and I
        // cannot describe it" and "nothing moved at all".
        geometricFallback(delta) {
            if (delta.dx !== 0 || delta.dy !== 0) {
                return { rate: this.bandRate(delta.dy, delta.dx), path: 'band-only' };
            }
            if (delta.subScroller) {
                return { rate: 0, path: 'unanswerable' };
            }
            return { rate: 0, path: 'no-motion' };
        }

        beginClick() {
            // Null when degraded — see beginScroll.
            const startSet = this.tracker.snapshot();
            this.clickStartSet = startSet;
            this.clickStartAreas = startSet ? this.captureAreas(startSet) : null;
        }

        endClick() {
            const endSet = this.clickStartSet ? this.tracker.snapshot() : null;
            if (!this.clickStartSet || !endSet) {
                this.clickStartSet = null;
                // Not 'diff'. A click has no motion to measure, so rate stays
                // 0, but returning 'diff' would make this byte-identical to a
                // healthy click that genuinely changed nothing - leaving the
                // caller no way to tell "I measured, nothing moved" from "I
                // never managed to measure" and no reason to escalate.
                return { rate: 0, path: 'unanswerable' };
            }
            const rate = this.diffRateWith(
                this.clickStartSet, endSet, this.clickStartAreas);
            this.clickStartSet = null;
            return { rate: rate, path: 'diff' };
        }

        // The viewport dimensions the band is measured against, on the same
        // basis as the area the result is divided by.
        //
        // Follows calculateViewportArea()'s branch order - visualViewport
        // first, else the layout box - because that is what getViewportArea()
        // resolves to. Measuring the band in layout-viewport units and then
        // dividing by a visual-viewport area mixes two quantities that differ
        // by the square of the pinch scale (4x at scale 2), and the band half
        // of that mismatch under-reports, which is the silent direction.
        //
        // It follows the ORDER, not the branch conditions, and the two differ
        // in exactly two places. Both differences are deliberate, and both
        // bias toward a smaller rate ceiling, i.e. toward still-reporting
        // rather than toward a manufactured saturated one:
        //
        //   1. calculateViewportArea() takes `if (window.visualViewport)`;
        //      this takes `vp && vp.width > 0 && vp.height > 0`. On a
        //      zero-sized visualViewport the original's area collapses (and is
        //      then floored at 1), while this falls through to the layout box.
        //   2. calculateViewportArea() folds the client dimensions in with a
        //      literal Math.min(innerWidth, docEl.clientWidth || 0,
        //      body.clientWidth || 0), which collapses to 0 whenever EITHER
        //      client value is missing or zero. Here zero and missing client
        //      dimensions are treated as "no constraint" instead. As an
        //      *area* the original's 0 is merely clamped up to 1 and the
        //      damage is invisible; as a *band denominator* it would make
        //      |dy| / 1 saturate, so every 40px scroll on a page with a
        //      collapsed body would report rate 1 - a manufactured report,
        //      which the equivalence contract forbids just as firmly as a
        //      suppressed one. Pinned by the zero-dimension test.
        //
        // Every dereference is guarded: visualViewport is absent on older
        // engines, document.body is null before parsing finishes, and this runs
        // on the path feeding every changeRate, so a throw here stops all
        // reporting. The final floor at 1 exists because this value is only
        // ever used as a denominator.
        viewportBox() {
            const win = this.window;
            const vp = win.visualViewport;
            if (vp && vp.width > 0 && vp.height > 0) {
                return { width: vp.width, height: vp.height };
            }
            const doc = win.document;
            const docEl = doc ? doc.documentElement : null;
            const body = doc ? doc.body : null;
            let width = win.innerWidth > 0 ? win.innerWidth : 0;
            let height = win.innerHeight > 0 ? win.innerHeight : 0;
            if (docEl && docEl.clientWidth > 0) {
                width = width > 0 ? Math.min(width, docEl.clientWidth) : docEl.clientWidth;
            }
            if (docEl && docEl.clientHeight > 0) {
                height = height > 0 ? Math.min(height, docEl.clientHeight) : docEl.clientHeight;
            }
            if (body && body.clientWidth > 0) {
                width = width > 0 ? Math.min(width, body.clientWidth) : body.clientWidth;
            }
            if (body && body.clientHeight > 0) {
                height = height > 0 ? Math.min(height, body.clientHeight) : body.clientHeight;
            }
            return { width: width > 0 ? width : 1, height: height > 0 ? height : 1 };
        }

        bandRate(dy, dx) {
            const box = this.viewportBox();
            const vertical = Math.min(Math.abs(dy) / box.height, 1);
            const horizontal = Math.min(Math.abs(dx) / box.width, 1);
            return Math.max(vertical, horizontal);
        }

        // NOTE: there is deliberately no public diffRate(startSet, endSet).
        // It was removed: capturing areas at call time meant every start-side
        // key that had already left the live set read as area 0, so its
        // `disappeared` term was structurally always 0 and it could never
        // produce the (d + n) / 2 answer it advertised. diffRateWith below is
        // the real Path 2 core - pass it the areas captured at beginScroll or
        // beginClick time, which is the whole point.
        //
        // Mirrors the original compareSnapshots arithmetic exactly:
        //   both sides non-zero -> (d + n) / 2
        //   one side zero       -> max(d, n)
        //   each side clamped to the viewport area, result clamped to [0, 1]
        //
        // `startAreas` is REQUIRED despite being last, and omitting it now
        // throws. It used to degrade silently: `startAreas && startAreas.get()`
        // is falsy for every key, so the whole disappeared loop collapsed to 0
        // and the caller got a wrong answer that looked like a valid one -
        // silence dressed as a measurement.
        diffRateWith(startSet, endSet, startAreas) {
            if (!startAreas) {
                throw new TypeError('diffRateWith requires startAreas, the map ' +
                    'captured alongside startSet');
            }
            const viewportArea = this.getViewportArea();
            if (viewportArea <= 0) {
                return 0;
            }

            let disappeared = 0;
            let appeared = 0;
            const that = this;

            startSet.forEach(function (key) {
                if (!endSet.has(key)) {
                    const captured = startAreas.get(key);
                    disappeared += captured ? captured.area : 0;
                }
            });
            endSet.forEach(function (key) {
                if (!startSet.has(key)) {
                    appeared += that.tracker.areaOf(key);
                }
            });

            return this.combine(disappeared, appeared, viewportArea);
        }

        combine(disappeared, appeared, viewportArea) {
            const d = Math.min(Math.max(0, disappeared), viewportArea);
            const n = Math.min(Math.max(0, appeared), viewportArea);
            let visualChangeArea;
            if (d === 0 && n === 0) {
                visualChangeArea = 0;
            } else if (d > 0 && n > 0) {
                visualChangeArea = (d + n) / 2;
            } else {
                visualChangeArea = Math.max(d, n);
            }
            visualChangeArea = Math.min(visualChangeArea, viewportArea);
            return Math.max(0, Math.min(1, visualChangeArea / viewportArea));
        }

        // Change confined to the region that stayed on screen. Scrolling down
        // by dy leaves [0, H-dy] retained and exposes [H-dy, H]; scrolling up
        // mirrors it. Elements that left are located by translating their
        // pre-scroll rect into post-scroll coordinates.
        //
        // `startAreas` is REQUIRED despite being last, and omitting it now
        // throws. It used to degrade silently: `startAreas && startAreas.get()`
        // is falsy for every key, so the entire disappeared loop was skipped and
        // the term became 0 — a wrong answer that looks like a valid one. Always
        // pass the map captured alongside `startSet`.
        residualRate(startSet, endSet, dy, startAreas) {
            if (!startAreas) {
                throw new TypeError('residualRate requires startAreas, the map ' +
                    'captured alongside startSet');
            }
            const viewportH = this.viewportBox().height;
            const viewportArea = this.getViewportArea();
            if (viewportArea <= 0 || Math.abs(dy) >= viewportH) {
                return 0;   // the band is the whole viewport; nothing retained
            }

            let retainedTop, retainedBottom;
            if (dy > 0) {
                retainedTop = 0;
                retainedBottom = viewportH - dy;
            } else {
                retainedTop = -dy;
                retainedBottom = viewportH;
            }

            const that = this;
            let disappeared = 0;
            let appeared = 0;

            startSet.forEach(function (key) {
                if (endSet.has(key)) {
                    return;
                }
                const captured = startAreas.get(key);
                if (!captured || !captured.rect) {
                    return;
                }
                // Translate the pre-scroll rect into post-scroll coordinates.
                const top = captured.rect.top - dy;
                const bottom = captured.rect.bottom - dy;
                disappeared += that.overlapFraction(
                    top, bottom, retainedTop, retainedBottom) * captured.area;
            });

            endSet.forEach(function (key) {
                if (startSet.has(key)) {
                    return;
                }
                const r = that.tracker.rectOf(key);
                if (!r) {
                    return;
                }
                appeared += that.overlapFraction(
                    r.top, r.bottom, retainedTop, retainedBottom) *
                    that.tracker.areaOf(key);
            });

            return this.combine(disappeared, appeared, viewportArea);
        }

        overlapFraction(top, bottom, regionTop, regionBottom) {
            const height = bottom - top;
            if (height <= 0) {
                return 0;
            }
            const overlap = Math.min(bottom, regionBottom) - Math.max(top, regionTop);
            if (overlap <= 0) {
                return 0;
            }
            return Math.min(1, overlap / height);
        }

        reset() {
            this.scrollStartSet = null;
            this.scrollStartAreas = null;
            this.clickStartSet = null;
            this.clickStartAreas = null;
            // The re-seed cache is keyed on tracker state, so a reset that left
            // it stamped could replay an answer from before whatever the reset
            // was marking.
            this.reseedGeneration = -1;
            this.reseedChurn = -1;
            this.reseedMutations = 0;
            this.reseedResult = 0;
        }
    }

    // The mutation path must compare actual text to detect a change, so this
    // one caller still materialises a string. Everything on the scroll/click
    // hot path uses TextProbe instead.
    function readElementText(element) {
        const tagName = element.tagName.toLowerCase();
        if (tagName === 'select') {
            const selected = element.options && element.options[element.selectedIndex];
            return (selected ? (selected.textContent || selected.value || '') : '').trim();
        }
        if (tagName === 'textarea') {
            return (element.value || '').trim();
        }
        return (element.textContent || '').replace(/\s+/g, ' ').trim();
    }

    class ViewportSnapshot {
        constructor(monitor) {
            this.monitor = monitor;
            this.snapshots = new Map();
            this.snapshotOrder = [];
        }

        createSnapshot(snapshotType) {
            const monitor = this.monitor;
            const collected = collectViewportElements({
                document: _document,
                window: window,
                keyCache: monitor.keyCache,
                textProbe: monitor.textProbe,
                viewportArea: monitor.viewportArea,
                getComputedStyle: _getComputedStyle,
                skipTags: SKIP_TAGS,
                skipSelfTags: SKIP_SELF_TAGS,
                windowScale: monitor.windowScale
            });

            const snapshot = {
                timestamp: Date.now(),
                type: snapshotType,
                viewportArea: monitor.viewportArea,
                elements: collected,
                totalTextArea: 0,
                elementMap: new Map()
            };

            for (let i = 0; i < collected.length; i++) {
                const item = collected[i];
                snapshot.elementMap.set(item.key, item);
                snapshot.totalTextArea += item.area;
            }

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

            this.keyCache = new ElementKeyCache();
            this.textProbe = new TextProbe(this.config.minTextLength);
            this.formDebounceMap = new WeakMap();
            this.viewportArea = this.calculateViewportArea();
            this.snapshotManager = new ViewportSnapshot(this);

            const that = this;
            this.tracker = new VisibilityTracker({
                window: window,
                keyCache: this.keyCache,
                textProbe: this.textProbe,
                rootMargin: '0px',
                // The tracker clamps areaOf() against the same maxReasonableArea
                // collectViewportElements uses. Pass the monitor's measured
                // viewportArea so both clamp against one number: the tracker's
                // own fallback measures the layout viewport, while
                // calculateViewportArea() measures the visual one, and under
                // pinch-zoom those differ by the square of the scale.
                getViewportArea: function () { return that.viewportArea; },
                onDegrade: function () { that.fallbackMode = true; }
            });
            this.fallbackMode = !this.tracker.start();
            // Per-interaction latches. Read the note in beginScrollInteraction:
            // fallbackMode can flip mid-interaction, and start/end must agree
            // on which measurement path they are on or the report is dropped.
            this.scrollUsedFallback = false;
            this.clickUsedFallback = false;
            // Whether a scroll interaction was actually begun. The original
            // reported from checkScrollEnd only when a 'scroll-start' snapshot
            // existed; this is that same guard for the calculator path, which
            // has no snapshot to test. See checkScrollEnd.
            this.scrollStarted = false;
            this.metrics = new ScrollMetrics(window);
            this.calculator = new ChangeRateCalculator({
                window: window,
                tracker: this.tracker,
                metrics: this.metrics,
                getViewportArea: function () { return that.viewportArea; }
            });

            this.isProcessing = false;
            this.isScrolling = false;
            this.scrollTimer = null;
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
                handleOrientationChange: null,
                handleCaptureScroll: null
            };

            this.originalMethods = {
                scrollIntoView: null,
                scrollTo: null,
                scrollBy: null
            };

            this.allTimers = new Set();
            this.resizeTimer = null;
            this.orientationTimer = null;
            this.changeRateBuffer = [];
            this.isReporting = false;
            this.reportIntervalId = null;

            this.elementTextCache = null;
            // Guarded: visualViewport is absent on older engines, and an
            // unguarded read here threw before the monitor had constructed
            // anything at all, taking every report with it.
            this.windowScale = window.visualViewport
                ? window.visualViewport.scale : 1;
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

            this.scheduleTimer(() => {
                this.startReportPolling();
                this.scheduleTimer(() => {
                    this.reportInitialLoad();
                }, 50);
            }, 50);

            // Seed the live set from the document as it stands. Everything
            // after this arrives through the MutationObserver path.
            if (!this.fallbackMode && _document.body) {
                this.tracker.observeSubtree(_document.body);
                this.tracker.flush();
            }

            print({
                type: 'debugLog',
                data: { log: 'Font change monitor started (memory optimized version)' }
            });
        }

        getStats() {
            return {
                viewportArea: this.viewportArea,
                bufferSize: this.changeRateBuffer.length,
                ioTargetCount: this.tracker ? this.tracker.targetCount() : 0,
                fallbackMode: this.fallbackMode,
                config: {
                    changeThreshold: this.config.changeThreshold,
                    reportInterval: this.config.reportInterval
                }
            };
        }

        // IntersectionObserver callbacks are delivered before the next
        // animation frame, so a rAF hop guarantees the live set is current.
        // Falls back to a timer where rAF is unavailable.
        afterNextFrame(fn) {
            if (typeof window.requestAnimationFrame === 'function') {
                window.requestAnimationFrame(function () { fn(); });
            } else {
                this.scheduleTimer(fn, 16);
            }
        }

        // The one place a scroll interaction begins, shared by the touchstart/
        // mousedown handler and by all three patched JS scroll entry points.
        //
        // Latch the mode HERE, and have checkScrollEnd branch on the latch
        // rather than on the live fallbackMode. The tracker can degrade
        // MID-SCROLL: start would then run the calculator path (no
        // 'scroll-start' snapshot is taken) while end would see
        // fallbackMode === true, look for that snapshot, find none, and emit
        // nothing at all. That is a report the original always made, silently
        // dropped - and it would also bypass the calculator's band-only
        // fallback, which exists for precisely this case. Latching keeps start
        // and end on the same path; a tracker that dies mid-scroll is handled
        // by endScroll()'s null-snapshot branch.
        beginScrollInteraction() {
            this.scrollUsedFallback = this.fallbackMode;
            this.scrollStarted = true;
            if (this.fallbackMode) {
                this.snapshotManager.createSnapshot('scroll-start');
            } else {
                // O(1). The old code ran a full-document traversal here,
                // synchronously inside touchstart, which cost the first frame
                // of every scroll.
                this.calculator.beginScroll();
            }
        }

        checkScrollEnd() {
            if (!this.isScrolling) {
                return;
            }
            this.isScrolling = false;

            // Mirrors the original's `if (startSnapshot)` guard, on BOTH paths.
            // handleScroll flips isScrolling without beginning an interaction -
            // a wheel or keyboard scroll reaches here with no start at all -
            // and the original emitted nothing there because no 'scroll-start'
            // snapshot existed. Running endScroll() unguarded would instead
            // answer from a ScrollMetrics origin left over from some earlier
            // interaction: a manufactured report when that stale delta is
            // large, and - when the page happens to be back at the marked
            // offset - a 'no-motion' zero, which is the silent direction.
            const started = this.scrollStarted;
            this.scrollStarted = false;

            if (started) {
                if (this.scrollUsedFallback) {
                    const endSnapshot = this.snapshotManager.createSnapshot('scroll-end');
                    const startSnapshot = this.snapshotManager.getSnapshot('scroll-start');
                    if (startSnapshot) {
                        const changeRate = this.snapshotManager.compareSnapshots(
                            startSnapshot, endSnapshot, 'scroll');
                        this.addToBuffer(changeRate, 'scroll');
                        this.snapshotManager.clearSnapshot('scroll-start');
                    }
                } else {
                    const result = this.calculator.endScroll();
                    print({
                        type: 'debugLog',
                        data: {
                            log: `Scroll end via ${result.path}: ${(result.rate * 100).toFixed(2)}%`
                        }
                    });
                    this.addToBuffer(result.rate, 'scroll');
                }
            }

            print({
                type: 'debugLog',
                data: { log: 'Scroll end' }
            });
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
                    // Same latch as the touchstart path. Leaving the raw
                    // createSnapshot here would keep one of the synchronous
                    // traversals this task removes AND orphan the snapshot:
                    // checkScrollEnd now branches on scrollUsedFallback, so a
                    // 'scroll-start' nobody latched for is never read.
                    that.beginScrollInteraction();
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
                    that.beginScrollInteraction();
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
                    that.beginScrollInteraction();
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

            const currentText = readElementText(element);
            if (!currentText || currentText.length < this.config.minTextLength) {
                return false;
            }

            const cachedText = this.elementTextCache.get(element);
            if (cachedText === undefined) {
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
                        // The touchstart/mousedown coordinates used to be
                        // recorded here into a field nothing ever read. The
                        // touchstart guard above already proves touches[0]
                        // exists, so dropping the reads changes no behaviour.
                        that.beginScrollInteraction();
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
                    // Latch, for the same reason beginScrollInteraction does:
                    // a tracker that degrades between click start and click end
                    // must not leave end looking for a snapshot start never
                    // took.
                    that.clickUsedFallback = that.fallbackMode;
                    if (that.fallbackMode) {
                        that.snapshotManager.createSnapshot('click-start');
                    } else {
                        that.calculator.beginClick();
                    }
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
                        if (!that.isClickHandling) {
                            return;
                        }
                        that.viewportArea = that.calculateViewportArea();

                        if (that.clickUsedFallback) {
                            const endSnapshot = that.snapshotManager.createSnapshot('click-end');
                            const startSnapshot = that.snapshotManager.getSnapshot('click-start');
                            if (startSnapshot) {
                                const changeRate = that.snapshotManager.compareSnapshots(
                                    startSnapshot, endSnapshot, 'click');
                                that.addToBuffer(changeRate, 'click');
                                that.snapshotManager.clearSnapshot('click-start');
                            }
                            that.isClickHandling = false;
                            print({ type: 'debugLog', data: { log: `Click end` } });
                        } else {
                            // Align to the IO callback so the live set reflects
                            // this frame. rAF fires after IO delivery.
                            that.afterNextFrame(() => {
                                const result = that.calculator.endClick();
                                that.addToBuffer(result.rate, 'click');
                                that.isClickHandling = false;
                                print({
                                    type: 'debugLog',
                                    data: {
                                        log: `Click end via ${result.path}: ${(result.rate * 100).toFixed(2)}%`
                                    }
                                });
                            });
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
            // Delivered-mutation witness for Rule 1's re-seed cache, recorded
            // BEFORE the re-entrancy guard below: a batch this callback declines
            // to process is still a batch the page produced, and the counter's
            // whole job is to stop a cached authoritative `gate-zero` from
            // outliving evidence that the DOM moved. Counting the dropped batch
            // costs one extra re-seed and moves toward reporting; not counting
            // it would leave the silence in place.
            if (this.tracker && typeof this.tracker.noteMutationRecords === 'function') {
                this.tracker.noteMutationRecords(mutations ? mutations.length : 0);
            }

            if (this.isProcessing) {
                return;
            }

            this.isProcessing = true;

            try {
                for (let i = 0; i < mutations.length; i++) {
                    const mutation = mutations[i];

                    if (mutation.type === 'characterData') {
                        const element = mutation.target.parentElement;
                        // BEFORE the isElementVisible gate, deliberately. A
                        // characterData mutation makes an existing chain
                        // text-bearing without adding any node, and that gate is
                        // exactly the off-screen filter the chain has to
                        // survive: a lazily-filled wrapper below the fold would
                        // otherwise never enter the live set.
                        if (!this.fallbackMode && element) {
                            this.tracker.observeTextChain(element);
                        }
                        if (element && this.isElementVisible(element)) {
                            if (this.checkTextChange(element)) {
                                this.handleTextChange(element);
                            }
                        }
                    } else if (mutation.type === 'childList') {
                        this.handleChildListMutation(mutation);
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

            if (!this.fallbackMode) {
                mutation.addedNodes.forEach((node) => {
                    if (node.nodeType === 1) {
                        this.tracker.observeSubtree(node);
                        // observeSubtree observes the node and what is under it.
                        // observeTextChain walks the other way — up to the walk
                        // root — for the ancestors that became text-bearing only
                        // because this node arrived. A lazily-filled wrapper chain
                        // is exactly that case, and without the climb the chain
                        // stays outside the live set permanently. Cost is DOM
                        // depth, not DOM size; observe() is idempotent, so the
                        // overlap with observeSubtree costs nothing.
                        //
                        // It must be called HERE and not hung off
                        // handleTextChange: that is reached only through
                        // isElementVisible(node) && isElementInViewport(node), so
                        // an off-screen insertion — the infinite-scroll case this
                        // whole gate exists for — never reaches it.
                        this.tracker.observeTextChain(node);
                    }
                });
            }

            mutation.removedNodes.forEach(node => {
                if (node.nodeType === 1) {
                    this.handleRemovedElement(node);
                }
            });

            // AFTER handleRemovedElement, never before. calculateElementArea
            // now prefers the tracker's IO-cached rect, and for a node that has
            // already been detached that cache is the ONLY source of a non-zero
            // area — getBoundingClientRect on a detached node returns all
            // zeros. Unobserving first drops the cache entry, handleRemovedElement
            // measures 0, and the removal report is silently dropped.
            if (!this.fallbackMode) {
                mutation.removedNodes.forEach((node) => {
                    if (node.nodeType === 1) {
                        this.tracker.unobserveSubtree(node);
                    }
                });
            }
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

            // Inner overflow:auto scrolls do not bubble, but are observable
            // during capture. Without this an inner scroll looks like dy == 0.
            const that = this;
            this.eventHandlers.handleCaptureScroll = function (event) {
                that.metrics.noteScrollEvent(event);
            };
            window.addEventListener('scroll', this.eventHandlers.handleCaptureScroll,
                { passive: true, capture: true });
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
                    _document.documentElement.clientWidth || 0,
                    _document.body.clientWidth || 0
                );
                const height = Math.min(
                    window.innerHeight || 0,
                    _document.documentElement.clientHeight || 0,
                    _document.body.clientHeight || 0
                );
                area = Math.max(1, width * height);
            }

            return area;
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
            // Guarded dereference. This was the last unguarded read left after
            // the constructor's was fixed, and it was the worse of the two: an
            // engine without a visual viewport constructed cleanly and then
            // threw on the first in-viewport childList mutation, killing every
            // report for the page silently. With no visual viewport there is
            // nothing to narrow against, so answer from the layout viewport -
            // the still-reporting direction.
            const vp = window.visualViewport;
            if (vp && vp.scale !== this.windowScale && isInViewport) {
                return !(
                    rect.right < vp.offsetLeft ||
                    rect.left > (vp.offsetLeft + vp.width) ||
                    rect.bottom < vp.offsetTop ||
                    rect.top > (vp.offsetTop + vp.height)
                );
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
            // hasVisibleText() is gone; TextProbe.hasText() is its verified
            // equivalent and answers the same question without building the
            // string. Step 9 of the brief deleted hasVisibleText but left this
            // call site, which would have thrown on every mutation.
            return this.textProbe.hasText(element);
        }

        calculateElementArea(element) {
            if (!element || element.nodeType !== 1) {
                return 0;
            }

            // Prefer the IO-cached rect for a DETACHED element, and only for
            // one. For removed elements this is the only correct source:
            // getBoundingClientRect on a detached node returns all zeros. That
            // reason covers detached nodes and nothing else.
            //
            // For an ATTACHED element the shortcut is actively wrong. IO rects
            // are delivered on the rendering lifecycle, while a
            // MutationObserver callback runs BEFORE the next paint, so `live`
            // still holds the pre-change rect. A lazily-filled div that IO
            // recorded at 400x100 and that fills in to 400x600 would answer
            // 40000 instead of 240000 — 0.125 against a 0.15 threshold, i.e.
            // no report at all, where the original measured 0.75 and reported.
            // Suppressing a report the original emits is the one direction
            // this plan forbids. The measured path is both available and
            // fresher here, so the shortcut buys nothing and costs a threshold
            // crossing.
            //
            // isConnected is a DOM property, not ES syntax, so the ES2017
            // constraint does not bite — same idiom, and same reasoning, as
            // VisibilityTracker.handleEntries. An engine predating it yields
            // undefined, `=== false` is false, and we fall through to
            // measurement: the safe direction, and for a detached node exactly
            // the pre-task behaviour, never worse than shipped.
            //
            // `cached > 0`, not `cached >= 0`: a 0 from areaOf is ambiguous —
            // it means "not in the live set" OR "in the set but gated out"
            // (Task 5's sentinel). Neither is an answer, so both fall through
            // to the measured path. Do not shortcut this to `return cached`.
            //
            // The measured path below deliberately keeps the ORIGINAL ordering
            // — clamp before the style read — even though Task 5's areaOf runs
            // the gate first (R12). The two are not in conflict: areaOf mirrors
            // the traversal, which gates in acceptNode before any arithmetic,
            // while this function mirrors the old single-element path, which
            // returned the clamp from an early return. Reordering it here would
            // turn an oversized display:none element from "reports 50× the
            // viewport" into "reports nothing" — suppressing a report the old
            // code emits, which is the forbidden direction.
            if (!this.fallbackMode && this.tracker && element.isConnected === false) {
                const key = this.keyCache.keyOf(element);
                const cached = this.tracker.areaOf(key);
                if (cached > 0) {
                    return cached;
                }
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
                    // Clamp returns unscaled, before the style read - this is
                    // the ORIGINAL ordering of this function, kept on purpose.
                    // See the note above the tracker shortcut.
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

            // The old code assigned a *new* function to window.addEventListener
            // here. The matching originalMethods slot was never populated, so
            // this was an overwrite, not a restore: stop() left the window with
            // a synthetic delegate in place of whatever the engine shipped.
            // Nothing in this script patches addEventListener, so there is
            // nothing to restore.

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

            // Must repeat { capture: true }: a capture listener is a distinct
            // registration and a bubble-phase removal does not match it.
            if (handlers.handleCaptureScroll) {
                window.removeEventListener('scroll', handlers.handleCaptureScroll,
                    { capture: true });
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


            this.formDebounceMap = new WeakMap();
            this.snapshotManager.clearSnapshot();

            this.changeRateBuffer.length = 0;
            this.isReporting = false;

            this.elementTextCache = null;

            // Order matters, and so does having both. stop() bumps
            // observeChurn, which closes the stale-cache hole only for a cached
            // ZERO; a calculator holding a cached NON-zero would skip its
            // refresh and answer from a walk describing a tracker that no
            // longer exists. reset() is the consumer-side half the tracker's
            // own comment asks every consumer to supply on a stop/restart
            // boundary. Both run with every listener already removed and every
            // timer already cleared (see stop()), so no interaction can observe
            // the window between them.
            if (this.tracker) {
                this.tracker.stop();
            }
            if (this.calculator) {
                this.calculator.reset();
            }
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
            this.scrollStarted = false;
            this.scrollUsedFallback = false;
            this.clickUsedFallback = false;
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
                            if (_document.body && _document.body.nodeType === 1) {
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
            return this._monitor.getStats();
        },

        triggerPoll: function() {
            if (this._monitor) {
                this._monitor.pollBuffer();
            }
        }
    };

    // Test-only surface. Published solely when a test harness sets
    // window.__CCD_TEST_EXPORTS__ = true before this script runs, so
    // production never sees these internals on window.
    if (window.__CCD_TEST_EXPORTS__ === true) {
        window.__CCD_TEST_EXPORTS__ = {
            DEFAULT_CONFIG: DEFAULT_CONFIG,
            ElementKeyCache: ElementKeyCache,
            TextProbe: TextProbe,
            SKIP_TAGS: SKIP_TAGS,
            SKIP_SELF_TAGS: SKIP_SELF_TAGS,
            collectViewportElements: collectViewportElements,
            VisibilityTracker: VisibilityTracker,
            ScrollMetrics: ScrollMetrics,
            ChangeRateCalculator: ChangeRateCalculator,
            ViewportSnapshot: ViewportSnapshot,
            FontChangeMonitor: FontChangeMonitor
        };
    }
})(window);