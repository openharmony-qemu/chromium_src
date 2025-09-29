import { LayoutKey, LayoutValue } from '../../Common/Constant';
import Store from '../../Common/Utils/Store';
import Utils from '../../Common/Utils/Utils';
import Framework from '../Framework';
import Throttle from '../Utils/Throttle';
import ModifyObserver from './Observers/ModifyObserver';
import ResizeObserver from './Observers/ResizeObserver';
import TransitionEndObserver from './Observers/TransitionEndObserver';

export default class ObserverHandler {
    static relayoutHandler: Throttle;
    private static TIMEOUT = 200;

    static postTask(): void {
        if (!ObserverHandler.relayoutHandler) {
            ObserverHandler.relayoutHandler = new Throttle(ObserverHandler.TIMEOUT, Framework.mainTask);
        }
        ObserverHandler.relayoutHandler.postTask();
    }

    static reInit():void {
        console.log('ObserverHandler reInit');
        ModifyObserver.reInit();
        TransitionEndObserver.reInit();
        ResizeObserver.init_();
        ObserverHandler.postTask();
    }

    static updateObserver(): void {
        if (Utils.isWideScreen()) {
            return;
        }
        ObserverHandler.removeObserver();
    }

    static removeObserver(): void {
        ModifyObserver.disconnect();
        TransitionEndObserver.removeListener();
        ResizeObserver.removeListener();
    }

    static nextRoundAndPost(ele: HTMLElement): void {
        ObserverHandler.nextRound(ele);
        ObserverHandler.postTask();
    }

    static nextRound(ele: HTMLElement): void {
        Store.setValue(ele, LayoutKey.LAYOUT_TAG, LayoutValue.NEXT_ROUND);
        Store.nextRoundCache.push(ele);
    }
}
