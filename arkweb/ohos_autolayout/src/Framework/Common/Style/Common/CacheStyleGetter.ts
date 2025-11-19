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
     * 泛型说明:
     * @template Args - 剩余参数的类型数组 (例如 getComputedStyle 可能有第二个参数)
     * @template R - 原函数的返回值类型 (例如 number, DOMRect, CSSStyleDeclaration)
     * @param fn 具体要执行的函数
     */
    static createSpecStyleCache<Args extends unknown[], R>(
        fn: (dom: HTMLElement, ...args: Args) => R
    ): (dom: HTMLElement | null | undefined, ...args: Args) => R | undefined {
        
        // 1. 明确 Map 存储的是 DOM 节点到返回值 R 的映射
        const cache = new Map();
        this.clearMap.set(cache, () => cache.clear());

        // 2. 返回函数的签名与原函数保持逻辑一致 (除了 dom 可能为空的处理)
        return function (dom: HTMLElement | null | undefined, ...args: Args): R | undefined {
            // 3. 这里处理了 dom 为空的情况，所以返回值类型必须包含 undefined
            if (!dom) {
                Log.e('input null to CacheStyleGetter');
                return undefined;
            }

            // 4. 读取缓存
            const cacheRes = cache.get(dom);
            if (cacheRes !== undefined) {
                return cacheRes;
            }

            // 5. 调用原函数：使用 call，显式传入 dom 和 展开的 args
            // 这里的类型是安全的：dom 确定是 HTMLElement (经过上面判空)，args 类型由泛型保障
            const result = fn.call(null, dom, ...args);

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
