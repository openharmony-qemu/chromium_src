import OriginStyle from '../../../../Common/Style/Getter/OriginStyleGetter/OriginStyle';
import OriginStyleGetter from '../../../../Common/Style/Getter/OriginStyleGetter/OriginStyleGetter';
import Log from '../../../../Debug/Log';
import CacheStyleGetter from '../../../../Common/Style/Common/CacheStyleGetter';
import { Txt } from '../../../../Common/Txt';
 
export default class FeatureCache {
    static styleCacheMap: Map<string, Map<string, string>> = new Map();
    static typeCacheMap: Map<string, Map<string, string>> = new Map();
 
    static getOriginStyleCached(ele: HTMLElement, key: string): string {
        const feature = this.getFeature(ele);
        let cacheMap = this.styleCacheMap.get(feature) || new Map();
        if (!cacheMap.size) {
            OriginStyle.cacheArr.forEach((cacheKey) => {
                cacheMap.set(cacheKey, OriginStyleGetter.getStyleInCss(ele, cacheKey));
            });
        }
 
        this.styleCacheMap.set(feature, cacheMap);
 
        Log.checkFeatureCache(ele, key, cacheMap.get(key));
 
        return cacheMap.get(key);
    }
 
    // 类型缓存
    static setTypeCached(ele: HTMLElement, type: string, result: string): void {
        const feature = this.getFeature(ele);
        let cacheMap = this.typeCacheMap.get(feature) || new Map();
 
        cacheMap.set(type, result);
 
        this.typeCacheMap.set(feature, cacheMap);
    }
 
    // 类型缓存
    static getTypeCached(ele: HTMLElement, type: string): string {
        const feature = this.getFeature(ele);
 
        let cacheMap = this.typeCacheMap.get(feature);
 
        if (cacheMap) {
            return cacheMap.get(type);
        }
        return null;
    }
 
    //  class attr:data-v
    private static getFeature(
        ele: HTMLElement,
        childLocalName: string = this.getClosedCSSText(ele) + this.getDataAttr(ele),
        level: number = 5
    ): string {
        if (!ele || level === 0) {
            return childLocalName;
        }
 
        const { localName, className, id } = ele;
        let currentFeatureStr = localName;
 
        if (className) {
            currentFeatureStr += className;
        }
 
        if (id) {
            currentFeatureStr += id.replace(/[0-9]/g, '');
        }
 
        if (level === 4) {
            currentFeatureStr += CacheStyleGetter.computedStyle(ele, Txt.lineHeight_);
        }
 
        return this.getFeature(ele.parentElement, currentFeatureStr + childLocalName, level - 1);
    }
 
    static getClosedCSSText(ele: HTMLElement): string {
        let result = '';
 
        OriginStyle.cacheArr.forEach((key: string) => {
            result += key;
            result += ele.style.getPropertyValue(key);
        });
 
        return result;
    }
 
    static getDataAttr(ele: HTMLElement): string {
        if (!ele.hasAttributes()) {
            return '';
        }
        const { attributes } = ele;
        for (let i = 0; i < attributes.length; i++) {
            const attrName = attributes[i].name;
            if (attrName.includes('data')) {
                return attrName;
            }
        }
        return '';
    }
}