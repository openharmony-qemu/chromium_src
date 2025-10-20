import { LayoutKey, LayoutValue } from '../Common/Constant';
import Store from '../Common/Utils/Store';
import Utils from '../Common/Utils/Utils';
import Log from '../../Debug/Log';
import Tag from '../../Debug/Tag';
import Framework from '../Framework';
import Throttle from '../Utils/Throttle';
import ModifyObserver from './Observers/ModifyObserver';
import PageContentObserver from './Observers/PageContentObserver';
import ResizeObserver from './Observers/ResizeObserver';

export default class ObserverHandler {
    static relayoutHandler: Throttle;
    private static readonly TIMEOUT = 200;

    static postTask(): void {
        if (!ObserverHandler.relayoutHandler) {
            ObserverHandler.relayoutHandler = new Throttle(ObserverHandler.TIMEOUT, Framework.mainTask);
        }
        ObserverHandler.relayoutHandler.postTask();
    }

    static reInit():void {
        Log.info('ObserverHandler reInit', Tag.observerHandler);
        ModifyObserver.reInit();
        PageContentObserver.reInit();
        ResizeObserver.init_();
        ObserverHandler.postTask();
    }

    static updateObserver(): void {
        if(Framework.stopFlag) {
            ResizeObserver.removeListener();
        }
        if (Utils.isWideScreen()) {
            return;
        }
        Log.d(`screenHeight: ${screen.availHeight}, screenWidth: ${screen.availWidth}`, Tag.resizeObserver);
        ObserverHandler.removeObserver();
    }

    private static removeObserver(): void {
        ModifyObserver.disconnect();
        PageContentObserver.disconnect();
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
