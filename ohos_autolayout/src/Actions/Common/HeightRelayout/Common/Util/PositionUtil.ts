import Cached from '../../../../../Common/Cached';
import CacheStyleGetter from '../../../../../Common/Style/Common/CacheStyleGetter';
import { Txt } from '../../../../../Common/Txt';
 
export class PositionUtils {
    static isFixedOrAbsoluteEle(ele: HTMLElement): boolean {
        return this.positionTypeCheck(ele, [Txt.fixed_, Txt.absolute_]);
    }
 
    static isPositionedEle(ele: HTMLElement): boolean {
        return this.positionTypeCheck(ele, [Txt.relative_, Txt.fixed_, Txt.absolute_, Txt.sticky_]);
    }
 
    static isFixedEle(ele: HTMLElement): boolean {
        return this.positionTypeCheck(ele, Txt.fixed_);
    }
 
    static isAbsoluteEle(ele: HTMLElement): boolean {
        return this.positionTypeCheck(ele, Txt.absolute_);
    }
 
    static positionTypeCheck(ele: HTMLElement, str: string | string[]): boolean {
        if (!ele) {
            return false;
        }
 
        const positionType = CacheStyleGetter.computedStyle(ele, Txt.position_);
 
        if (Array.isArray(str)) {
            return str.includes(positionType);
        }
 
        return str === positionType;
    }
 
    static getAbsoluteElePaintEle(ele: HTMLElement): HTMLElement {
        let current = ele.parentElement;
 
        while (current && current !== Cached.body_()) {
            if (this.isPositionedEle(current)) {
                return current;
            }
 
            current = current.parentElement;
        }
 
        return Cached.body_();
    }
 
    /**
     * 寻找fixed元素的容器元素
     * 当元素祖先的 transform, perspective 或 filter 属性非 none 时，容器由视口改为该祖先
     */
    static getFixedElePaintEle(ele: HTMLElement): HTMLElement {
        let current = ele.parentElement;
 
        while (current && current !== Cached.body_()) {
            const isTransform = CacheStyleGetter.computedStyle(current, Txt.transform_) !== Txt.none_;
            const isPerspective = CacheStyleGetter.computedStyle(current, Txt.perspective_) !== Txt.none_;
            const isFilter = CacheStyleGetter.computedStyle(current, Txt.filter_) !== Txt.none_;
 
            if (isTransform || isPerspective || isFilter) {
                return current;
            }
 
            current = current.parentElement;
        }
 
        return Cached.body_();
    }
 
    static getPaintEle(ele: HTMLElement): HTMLElement {
        if (this.isFixedEle(ele)) {
            return this.getFixedElePaintEle(ele);
        }
 
        if (this.isAbsoluteEle(ele)) {
            return this.getAbsoluteElePaintEle(ele);
        }
 
        return ele.parentElement;
    }
}