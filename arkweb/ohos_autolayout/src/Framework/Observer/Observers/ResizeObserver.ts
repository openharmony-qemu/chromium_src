import ObserverHandler from '../ObserverHandler';
import WaitSystemReady from '../../Utils/WaitSystemReady';
import CSSSheetManage from '../../Common/Style/Setter/CSSSheetManage';
import Framework from '../../Framework';
import Log from '../../../Debug/Log';
import Tag from '../../../Debug/Tag';
import { Main } from '../../../Main';
import Constant from '../../Common/Constant';
 
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
        window.addEventListener(Constant.resize, ResizeObserver.resizeCallback);
    }

    private static resizeCallback(): void {
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
        setTimeout(()=>{
            Framework.headReadyTask();
            ObserverHandler.updateObserver();
            Log.checkState('after updateObserver');
            CSSSheetManage.updateState();
            
            Log.d('try to postTask', ResizeObserver.TAG);
            ObserverHandler.postTask();
        }, 100);
    }

    static removeListener(): void {
        Log.info('remove', ResizeObserver.TAG);
        ResizeObserver.init = false;
        removeEventListener(Constant.resize, ResizeObserver.resizeCallback);
    }
}