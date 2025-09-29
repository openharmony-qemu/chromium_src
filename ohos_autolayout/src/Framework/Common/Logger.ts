/**
 * Module DebugLog.ts
 * 调试日志模块
 */

export default class Logger {
    private static readonly EnanlePrintStyle = false;
    private static readonly EnanlePrintRecoComp = true;
    private static readonly EnanlePrintSysInfo = true;
    private static readonly EnanlePrintNodeInfo = false;
    private static readonly MiscInfo = true;

    static printStyle(ele: HTMLElement, key: string, value: string, tag: string): void {
        if (Logger.EnanlePrintStyle) {
            console.log(`[${tag}] HTMLElement: ${ele ?.className}, key: ${key}, value: ${value}`);
        }
    }

    static printComInfo(msg: string): void {
        if (Logger.EnanlePrintRecoComp) {
            console.log(msg);
        }
    }

    static d(msg: string): void {
        if (Logger.EnanlePrintSysInfo) {
            console.log(msg);
        }
    }

    static printNodeInfo(msg: string): void {
        if (Logger.EnanlePrintNodeInfo) {
            console.log(msg);
        }
    }
    static printDebugMsg(msg: string): void {
        if (Logger.MiscInfo) {
            console.log(msg);
        }
    }
}
