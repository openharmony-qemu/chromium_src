/**
 * Module Main.ts
 * 主入口模块
 */
import Utils from './Framework/Common/Utils/Utils';
import Log from './Debug/Log';
import { CCMConfig } from './Framework/Common/CCMConfig';
import Framework from './Framework/Framework';
import WaitSystemReady from './Framework/Utils/WaitSystemReady';
import Alphabet from './Alphabet/Main';

export class Main {
    static initFlag = false;
    static start(config:string): void {
        if(CCMConfig.getInstance().fromJson(config)) {
            Main.start_();
        }
    }
    static start_(): void {
        if (Utils.isWideScreen()) {
            if (!Main.initFlag) {
                WaitSystemReady.configReady(Framework.configReady);
                Main.initFlag = true;
            }
        }
    }
    // 彻底关闭
    static stop(): void {
        Framework.stopFlag = true;
    }

    static restart(): void {
        Framework.stopFlag = false;
        Log.d('Autolayout reInit');
        Main.start_()
    }

    static alphabetStart(config: string): void {
        Alphabet.start(config);
    }
}