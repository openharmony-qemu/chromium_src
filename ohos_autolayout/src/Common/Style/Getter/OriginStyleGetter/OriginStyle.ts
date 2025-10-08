// 没有缓存进行计算并存入缓存
import { Txt } from '../../../Txt';
import DomReader from './DomReader';
import OriginStyleCache from './OriginStyleCache';

export default class OriginStyle {
    static cacheArr = [
        Txt.width_,
        Txt.height_,
        Txt.minHeight_,
        Txt.paddingTop_,
        Txt.paddingBottom_,
        Txt.marginTop_,
        Txt.marginBottom_,
        Txt.fontSize_,
        Txt.lineHeight_,
    ];

    /**
      * 获取并缓存用户设置的指定样式
      * @param ele
      * @param styleKey
      */
    static getAndCacheStyle(ele: HTMLElement, styleKey: string): string {
        const cloneDom = DomReader.createCloneDom(ele);
        const computedStyle = getComputedStyle(cloneDom);
 
        const styleValue = computedStyle.getPropertyValue(styleKey);
 
        this.cacheArr.forEach((key) => {
            OriginStyleCache.set(ele, key, computedStyle.getPropertyValue(key));
        });
 
        cloneDom.parentElement.removeChild(<Node>cloneDom);
        return OriginStyleCache.set(ele, styleKey, styleValue);
    }
}