/**
 * Module Main.ts
 * 主入口模块
 */

import Utils from "./Common/Utils/Utils";
import Framework from "./Framework/Framework";
import { HwRelayout } from "./Framework/HwRelayout";
import WaitSystemReady from "./Framework/Utils/WaitSystemReady";

export class Main {
    // C.SPC(后两位).B(后两位).参数
    static readonly version_name = "2.0.10.1";
    static start(): void {
        HwRelayout.start_();
    }

    static stop(): void {
        Framework.stopFlag = true;
    }

    static restart(): void {
        Framework.stopFlag = false;
    }

    static onNewPage(url: string): void {
        Utils.hadCalledRelayoutHappen = false;
    }

    static forceAllOpen(): void {
        Framework.forceAllOpenFlag = true;
    }
}


WaitSystemReady.waitForSystemReady();