import CacheStyleGetter from '../../../../Common/Style/Common/CacheStyleGetter';
import Utils from '../../../../Common/Utils/Utils';
import Tag from '../../../../Debug/Tag';
import PageObserver from '../../../../Framework/Observer/Observers/PageObserver';

export const TAG = Tag.heightReLayout;
export default class HeaderAdj {
    static fixedList: Set<HTMLElement> = new Set();

    static collectHeaderEle(ele: HTMLElement): void {
        if (!PageObserver.hadScroll) {
            return;
        }

        if (ele.offsetParent !== null) {
            return;
        }

        // 排除display none的影响
        if (CacheStyleGetter.offsetH(ele) === 0 && CacheStyleGetter.offsetW(ele) === 0) {
            return;
        }

        HeaderAdj.postFixedDOM(ele);
    }

    static postFixedDOM(ele: HTMLElement): void {
        if (Utils.ignoreLocalName(ele)) {
            return;
        }

        this.fixedList.add(ele);
    }

    static removeEle(ele: HTMLElement): void {
        this.fixedList.delete(ele);
    }
}