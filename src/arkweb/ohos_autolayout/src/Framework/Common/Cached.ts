import CacheStyleGetter from './Style/Common/CacheStyleGetter';
import Store from './Utils/Store';

export default class Cached {
    private static bodyCache: HTMLElement = undefined;
    private static documentCache: HTMLElement = undefined;

    static clearAllCache(): void {
        Store.removeNextRoundTags();

        this.bodyCache = undefined;
        this.documentCache = undefined;
    }

    static clearStyleCache(): void {
        CacheStyleGetter.clearSpecCache();
    }

    static body_(): HTMLElement {
        if (!this.bodyCache) {
            this.bodyCache = document.body;
        }

        return this.bodyCache;
    }

    static documentElement_(): HTMLElement {
        if (!this.documentCache) {
            this.documentCache = document.documentElement;
        }

        return this.documentCache;
    }
}
