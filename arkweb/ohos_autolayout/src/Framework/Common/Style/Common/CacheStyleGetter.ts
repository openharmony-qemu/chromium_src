import Log from '../../../../Debug/Log';

/**
 * 缓存具体样式
 */
export class SpecificStyleCache {
    static clearMap: Map<Map<HTMLElement, number | string>, () => void> = new Map();
    static offsetHeightCache: Function;
    static scrollHeightCache: Function;
    static offsetWidthCache: Function;
    static offsetTopCache: Function;
    static offsetLeftCache: Function;
    static scrollWidthCache: Function;
    static childCountCache: Function;
    static getRectCache: Function;
    static getComputedStyleCache: Function;
    static localNameCache: Function;
    static getRectBeforeZoom: Function;

    static specStyleCache: Map<HTMLElement, Map<string, string>> = new Map();

    static init(): void {
        if (this.offsetHeightCache) {
            return;
        }

        this.offsetHeightCache = this.createSpecStyleCache((dom: HTMLElement) => dom.offsetHeight);
        this.offsetWidthCache = this.createSpecStyleCache((dom: HTMLElement) => dom.offsetWidth);
        this.offsetTopCache = this.createSpecStyleCache((dom: HTMLElement) => dom.offsetTop);
        this.offsetLeftCache = this.createSpecStyleCache((dom: HTMLElement) => dom.offsetLeft);
        this.scrollHeightCache = this.createSpecStyleCache((dom: HTMLElement) => dom.scrollHeight);
        this.scrollWidthCache = this.createSpecStyleCache((dom: HTMLElement) => dom.scrollWidth);
        this.childCountCache = this.createSpecStyleCache((dom: HTMLElement) => dom.childElementCount);
        this.getRectCache = this.createSpecStyleCache((dom: HTMLElement) => dom.getBoundingClientRect());
        this.getComputedStyleCache = this.createSpecStyleCache(getComputedStyle);
        this.localNameCache = this.createSpecStyleCache((dom: HTMLElement) => dom.localName);
    }

    /**
     * 创建一个带缓存的方法
     * 注意函数的第一个参数会用作缓存的key
     * @param fn 函数名称
     */
    static createSpecStyleCache(fn: Function): Function {
        const cache = new Map();
        this.clearMap.set(cache, () => cache.clear());

        return function (dom: HTMLElement) {
            if (!dom) {
                Log.e('input null to CacheStyleGetter');
                return undefined;
            }

            const cacheRes = cache.get(dom);

            if (cacheRes) {
                return cacheRes;
            }

            const result = fn.apply(null, arguments);

            cache.set(dom, result);
            return result;
        };
    }
}

export default class CacheStyleGetter {
    static visibleOffsetH(ele: HTMLElement): number {
        return 0;
    }

    static offsetW(ele: HTMLElement): number {
        return SpecificStyleCache.offsetWidthCache(ele) || 0;
    }

    static offsetH(ele: HTMLElement): number {
        return SpecificStyleCache.offsetHeightCache(ele) || 0;
    }

    static computedStyleDec(ele: HTMLElement): CSSStyleDeclaration {
        return SpecificStyleCache.getComputedStyleCache(ele);
    }

    static computedStyle(ele: HTMLElement, attr: string): string {
        let eleStyle = SpecificStyleCache.specStyleCache.get(ele);
        let value: string;

        if (eleStyle) {
            value = eleStyle.get(attr);
            if (value) {
                return value;
            }
        } else {
            eleStyle = new Map<string, string>();
        }

        if (!CacheStyleGetter.computedStyleDec || !SpecificStyleCache.getComputedStyleCache) {
            return value;
        }
        value = CacheStyleGetter.computedStyleDec(ele).getPropertyValue(attr);
        eleStyle.set(attr, value);
        SpecificStyleCache.specStyleCache.set(ele, eleStyle);

        return value;
    }

    static localName(ele: HTMLElement): string {
        if (!SpecificStyleCache.localNameCache) {
            return '';
        }
        return SpecificStyleCache.localNameCache(ele);
    }

    // 清除具体样式缓存表
    static clearSpecCache():void {
        for (const fn of SpecificStyleCache.clearMap.values()) {
            fn.call(null);
        }

        SpecificStyleCache.specStyleCache.clear();
    }
}
