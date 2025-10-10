import { Txt } from '../../../Common/Txt';
import OriginStyleCache from '../../../Common/Style/Getter/OriginStyleGetter/OriginStyleCache';
import ObserverHandler from '../ObserverHandler';
import Utils from '../../../Common/Utils/Utils';
import DiffEleRecord from '../../../Common/Perform/DiffEleRecorder';
import { LayoutKey, LayoutValue } from '../../../Common/Constant';
import Store from '../../../Common/Utils/Store';
import { ObserverRecord, recordType } from '../../../Common/Perform/ChangeRecord';
import Log from '../../../Debug/Log';
 
export default class TransitionEndObserver {
    static init: boolean = false;
    static TAG: string = 'TransitionEndObserver';
 
    static reInit(): void {
        if (TransitionEndObserver.init) {
            return;
        }
        TransitionEndObserver.init = true;
 
        addEventListener('transitionend', TransitionEndObserver.onTransitionEnd);
    }
 
    private static onTransitionEnd(e: TransitionEvent): void {
        Log.i(null,'on transition end',TransitionEndObserver.TAG);
        // todo: 这个方法某些小程序会一致被调用和执行
        const node = e.target as HTMLElement;
 
        if (e.propertyName === Txt.width_) {
            OriginStyleCache.clearToTop(node);
        }
 
        if (Utils.ignoreEle(node)) {
            return;
        }
        // 如果是瀑布流子元素，则不对transition变化进行次数限制
        // 电信营业厅 生活 推荐 下方瀑布流切换tab会进行transition变化，尺寸和位置会进行动画，忽略变动会影响位置计算
        if (
            Store.getValue(node, LayoutKey.LAYOUT_TAG) !== LayoutValue.WATERFALL_ITEM &&
            ObserverRecord.ignoreChange(node, recordType.TRANSITION)
        ) {
            return;
        }
 
        DiffEleRecord.setTag(node);
        ObserverHandler.postTask();
    }
 
    static removeListener(): void {
        TransitionEndObserver.init = false;
        removeEventListener('transitionend', TransitionEndObserver.onTransitionEnd);
    }
}