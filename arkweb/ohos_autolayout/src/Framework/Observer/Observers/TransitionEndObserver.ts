import ObserverHandler from '../ObserverHandler';
import Utils from '../../Common/Utils/Utils';
import Log from '../../../Debug/Log';
import Constant from '../../Common/Constant';
 
export default class TransitionEndObserver {
    static init: boolean = false;
    static TAG: string = 'TransitionEndObserver';
 
    static reInit(): void {
        if (TransitionEndObserver.init) {
            return;
        }
        TransitionEndObserver.init = true;
 
        addEventListener(Constant.transitionend, TransitionEndObserver.onTransitionEnd);
    }
 
    private static onTransitionEnd(e: TransitionEvent): void {
        Log.i(null,'on transition end',TransitionEndObserver.TAG);
        // todo: 这个方法某些小程序会一致被调用和执行
        const node = e.target as HTMLElement;
        if (Utils.ignoreEle(node)) {
            return;
        }
        ObserverHandler.postTask();
    }
 
    static removeListener(): void {
        TransitionEndObserver.init = false;
        removeEventListener(Constant.transitionend, TransitionEndObserver.onTransitionEnd);
    }
}