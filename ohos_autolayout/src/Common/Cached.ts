import CacheStyleGetter from "./Style/Common/CacheStyleGetter";
import { Txt } from "./Txt";
import Utils from "./Utils/Utils";
import Store from "./Utils/Store";

export default class Cached {
    private static bodyCache: HTMLElement = undefined;
    private static documentCache: HTMLElement = undefined;
    private static innerHCache: number = undefined;
    private static innerWCache: number = undefined;
    private static rootFontSize: number = undefined;

    static clearAllCache() {
        Store.removeNextRoundTags();

        this.bodyCache = undefined;
        this.innerHCache = undefined;
        this.innerWCache = undefined;
        this.documentCache = undefined;
        this.rootFontSize = undefined;
    }

    static clearStyleCache() {
        CacheStyleGetter.clearSpecCache();
    }

    static body_() {
        if (!this.bodyCache) {
            this.bodyCache = document.body;
        }

        return this.bodyCache;
    }

    static documentElement_() {
        if (!this.documentCache) {
            this.documentCache = document.documentElement;
        }

        return this.documentCache;
    }

    static innerH() {
        if (!this.innerHCache) {
            this.innerHCache = innerHeight;
        }

        return this.innerHCache;
    }

    static innerW() {
        if (!this.innerWCache) {
            // 在放大模式下，因为InnerWidth被修改了，所以需要通过这个方式来读取页面的宽度
            this.innerWCache = document.documentElement.offsetWidth;
        }

        return this.innerWCache;
    }

    static rootFont() {
        if (!this.rootFontSize) {
            this.rootFontSize = parseFloat(CacheStyleGetter.computedStyle(Cached.documentElement_(), Txt.fontSize_));
        }

        return this.rootFontSize;
    }

    static availW(): number {
        return Utils.getAvailWidth();
    }
}
