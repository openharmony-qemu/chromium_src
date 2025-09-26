import { Txt } from "../../../Common/Txt";
import ObserverHandler from "../ObserverHandler";
import { HwRelayout } from "../../HwRelayout";
import WaitSystemReady from "../../Utils/WaitSystemReady";
import CSSSheetManage from "../../../Common/Style/Setter/CSSSheetManage";
import DiffEleRecord from "../../../Common/Perform/DiffEleRecorder";
import Framework from "../../Framework";
import BoundingRectFix from "../../SystemFix/BoundingRectFix";
import Log from "../../../Debug/Log";
 
export default class ResizeObserver {
    static init_() {
        window.addEventListener(Txt.resize_, () => {
            Log.checkState("resize");
            if (HwRelayout.initFlag) {
                ResizeObserver.onResize();
                return;
            }
            HwRelayout.reInit();
        });
    }
 
    static onResize() {
        console.log("onResize");
        if (!WaitSystemReady.hasBodyReady || !Framework.init) {
            return;
        }
        Framework.headReadyTask();
        ObserverHandler.updateObserver();
        Log.checkState("after updateObserver");
        BoundingRectFix.updateState();
        CSSSheetManage.updateState();
        // 当窗口大小或内容发生变化时，判断是否需要调整布局
        DiffEleRecord.setAllEleDiff();
        ObserverHandler.postTask();
    }
}