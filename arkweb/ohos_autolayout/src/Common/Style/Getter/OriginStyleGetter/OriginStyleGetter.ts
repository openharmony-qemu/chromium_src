import OriginStyle from './OriginStyle';
import OriginStyleCache from './OriginStyleCache';

// 样式对外接口
export default class OriginStyleGetter {
    static getStyleInCss(ele: HTMLElement, styleKey: string): string {
        if (!ele) {
            return '';
        }

        const cacheStyle = OriginStyleCache.get(ele, styleKey);
        if (cacheStyle) {
            return cacheStyle;
        }

        return OriginStyle.getAndCacheStyle(ele, styleKey);
    }
}