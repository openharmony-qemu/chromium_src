/**
 * Module Main.ts
 * 主入口模块
 */
import { CCMConfig } from './Framework/Common/CCMConfig';
import Framework from './Framework/Framework';
import { HwRelayout } from './Framework/HwRelayout';


export class Main {
    static start(config:string): void {
        if(CCMConfig.getInstance().fromJson(config)) {
            Main.start_();
        }
    }
    static start_(): void {
        HwRelayout.start_();
    }
    static stop(): void {
        Framework.stopFlag = true;
    }

    static restart(): void {
        Framework.stopFlag = false;
    }

    static forceAllOpen(): void {
        Framework.forceAllOpenFlag = true;
    }
}