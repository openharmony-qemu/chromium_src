import { Txt } from '../../../Common/Txt';
import ObserverHandler from '../ObserverHandler';
import WaitSystemReady from '../../Utils/WaitSystemReady';
import CSSSheetManage from '../../../Common/Style/Setter/CSSSheetManage';
import DiffEleRecord from '../../../Common/Perform/DiffEleRecorder';
import Framework from '../../Framework';
import BoundingRectFix from '../../SystemFix/BoundingRectFix';
import Log from '../../../Debug/Log';
import Tag from '../../../Debug/Tag';
import { Main } from '../../../Main';
 
export default class ResizeObserver {
    private static readonly TAG = Tag.resizeObserver;
    static init: boolean = false;

    static init_(): void {
        Log.info('init', ResizeObserver.TAG);
        if (ResizeObserver.init) {
            return;
        }
        ResizeObserver.init = true;

        Log.d('addEventListener', ResizeObserver.TAG);
        window.addEventListener(Txt.resize_, ResizeObserver.resizeCallback);
    }
    static resizeCallback(): void {
        Log.d('resizeCallback', ResizeObserver.TAG);
        if (Main.initFlag) {
            ResizeObserver.onResize();
            return;
        }
        Main.restart();
    }
 
    static onResize(): void {
        Log.d('onResize', ResizeObserver.TAG);
        if (!WaitSystemReady.hasBodyReady) {
            return;
        }
        Framework.headReadyTask();
        ObserverHandler.updateObserver();
        Log.checkState('after updateObserver');
        BoundingRectFix.updateState();
        CSSSheetManage.updateState();
        // 当窗口大小或内容发生变化时，判断是否需要调整布局
        DiffEleRecord.setAllEleDiff();
        
        Log.d('try to postTask', ResizeObserver.TAG);
        setTimeout(()=>{
            ObserverHandler.postTask();
        }, 100);
    }

    static removeListener(): void {
        Log.info('remove', ResizeObserver.TAG);
        ResizeObserver.init = false;
        removeEventListener(Txt.resize_, ResizeObserver.resizeCallback);
    }
}