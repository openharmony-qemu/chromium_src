import Tag from '../../../../Debug/Tag';
import Log from '../../../../Debug/Log';
 
export default class OriginStyleCache {
    private static TAG = Tag.originStyleCache;
    static CACHE: Map<HTMLElement, Map<string, string>> = new Map();
    static REM_CACHE: number = -1;
    static WAIT_CACHE_QUEUE: Set<HTMLElement> = new Set();
 
    /**
     * 存入缓存
     * @param ele
     * @param key
     * @param val
     */
    static set(ele: HTMLElement, key: string, val: string): string {
        const cache: Map<string, string> = this.CACHE.get(ele);
 
        if (cache) {
            cache.set(key, val);
            return val;
        }
        this.CACHE.set(ele, new Map().set(key, val));
        Log.i(ele, '存入缓存' + val, this.TAG);
        return val;
    }
 
    /**
     * 读取缓存
     * @param ele
     * @param key
     */
    static get(ele: HTMLElement, key: string): string {
        const cache: Map<string, string> = this.CACHE.get(ele);
        if (!cache) {
            return null;
        }
        Log.i(ele, '读取缓存' + cache.get(key), this.TAG);
        return cache.get(key);
    }
 
    /**
     * 判断缓存中是否有这个记录
     * @param ele
     */
    static has(ele: HTMLElement): boolean {
        return OriginStyleCache.CACHE.has(ele);
    }
 
    /**
     * 删除记录
     * @param ele
     */
    static delete(ele: HTMLElement): void {
        OriginStyleCache.CACHE.delete(ele);
    }
 
    /**
     * 清除所有缓存
     */
    static clearAll(): void {
        OriginStyleCache.CACHE.clear();
    }
 
    /**
     * 将失效依赖删除
     */
    static clearUnusedCache(): void {
        const newCache = new Map();
        for (const dom of this.WAIT_CACHE_QUEUE) {
            const cache = this.CACHE.get(dom);
            if (cache) {
                newCache.set(dom, cache);
            }
        }
        this.CACHE = newCache;
    }
 
    /**
     * 从下向上清除缓存
     * @param ele
     */
    static clearToTop(ele: HTMLElement): void {
        if (!ele) {
            return;
        }
 
        Log.i(ele, '清除缓存', this.TAG);
        this.CACHE.delete(ele);
        this.clearToTop(ele.parentElement);
    }
}