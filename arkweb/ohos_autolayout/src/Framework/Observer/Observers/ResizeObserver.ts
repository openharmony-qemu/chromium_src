import { Txt } from '../../../Common/Txt';
import ObserverHandler from '../ObserverHandler';
import { HwRelayout } from '../../HwRelayout';
import WaitSystemReady from '../../Utils/WaitSystemReady';
import CSSSheetManage from '../../../Common/Style/Setter/CSSSheetManage';
import DiffEleRecord from '../../../Common/Perform/DiffEleRecorder';
import Framework from '../../Framework';
import BoundingRectFix from '../../SystemFix/BoundingRectFix';
import Log from '../../../Debug/Log';
 
export default class ResizeObserver {
    static init: boolean = false;

    static init_(): void {
        console.log('ResizeObserver init');
        if (ResizeObserver.init) {
            return;
        }
        ResizeObserver.init = true;

        console.log('ResizeObserver addEventListener');
        window.addEventListener(Txt.resize_, ResizeObserver.resizeCallback);
    }
    static resizeCallback(): void {
        console.log('ResizeObserver resizeCallback');
        if (HwRelayout.initFlag) {
            ResizeObserver.onResize();
            return;
        }
        HwRelayout.reInit();
    }
 
    static onResize(): void {
        console.log('ResizeObserver onResize');
        if (!WaitSystemReady.hasBodyReady || !Framework.init) {
            return;
        }
        Framework.headReadyTask();
        ObserverHandler.updateObserver();
        Log.checkState('after updateObserver');
        BoundingRectFix.updateState();
        CSSSheetManage.updateState();
        // 当窗口大小或内容发生变化时，判断是否需要调整布局
        DiffEleRecord.setAllEleDiff();
        
        console.log('ResizeObserver try to postTask');
        setTimeout(()=>{
            ObserverHandler.postTask();
        }, 100);
    }

    static removeListener(): void {
        console.log('ResizeObserver remove');
        ResizeObserver.init = false;
        removeEventListener(Txt.resize_, ResizeObserver.resizeCallback);
    }
}