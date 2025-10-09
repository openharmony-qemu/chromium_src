import WaitSystemReady from './Utils/WaitSystemReady';
import Framework from './Framework';
import Utils from '../Common/Utils/Utils';

export class HwRelayout {
    static initFlag = false;
    static start_(): void {
        if (Utils.isWideScreen()) {
            HwRelayout.reInit();
        }
    }

    static reInit(): void {
        console.log('HwRelayout reInit');
        if (!HwRelayout.initFlag) {
            WaitSystemReady.configReady(Framework.configReady);
            HwRelayout.initFlag = true;
        }
    }
}
