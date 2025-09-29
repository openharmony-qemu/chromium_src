/**
 * Module Log.ts
 * 调试日志模块
 */

export default class Log {
    static taskTimes() { }

    static showValue(ele: HTMLElement, key: string, value: string) {
        console.log(`showValue HTMLElement: ${ele?.className}, key: ${key}, value: ${value}`);
    }

    static deleteValue(ele: HTMLElement, key: string) {
        console.log(`deleteValue HTMLElement: ${ele?.className}, key: ${key}`);
    }

    static clearTag(map: Map<string, WeakMap<HTMLElement, string>>, key: string) {
        console.log(`clearTag key: ${key}`);
    }

    static e(log: string) {
        console.error(log);
    }

    static i(ele: HTMLElement, log: string, tag: string) {
        // console.info(`HTMLElement: ${ele?.className}, log: ${log}, tag: ${tag}`);
    }
    static info(ele: HTMLElement, log: string, tag: string) {
        console.info(`HTMLElement: ${ele ?.className}, log: ${log}, tag: ${tag}`);
    }

    static logCaller(ele: HTMLElement): void { }

    static consoleLog(tag: string, ...args: any): void {
        console.info(tag, args);
    }

    static markWithColor(tag: string, color: string, ...elements: HTMLElement[]): void {
        console.log(`markWithColor tag: ${tag}, color: ${color}`);
    }

    static checkState(position?: string) {
        console.log(`checkState position: ${position}`);
    }

    static checkFeatureCache(ele: HTMLElement, key: string, cache: string) {
        console.log(`checkFeatureCache HTMLElement: ${ele ?.className}, key: ${key}, cache: ${cache}`);
    }
}
