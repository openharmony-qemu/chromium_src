import Tag from './Tag';

/**
 * Module Log.ts
 * 统一日志模块
 * 
 * 日志级别：
 * - DEBUG: 详细的调试信息（仅开发环境）
 * - INFO: 一般信息（仅开发环境）
 * - WARN: 警告信息（所有环境）
 * - ERROR: 错误信息（所有环境）
 * 
 * 使用说明：
 * 1. 开发模式：所有日志都会打印
 * 2. 生产模式：只打印 WARN 和 ERROR，DEBUG 和 INFO 代码会被 Terser 删除
 * 
 * Terser 配置要求：
 * - 通过全局变量 __DEV__ 控制
 * - 生产环境 __DEV__ = false，Terser 会删除 if (__DEV__) 包裹的代码
 */

// 声明全局变量（由 Webpack DefinePlugin 注入）
declare const __DEV__: boolean;

/**
 * 日志级别枚举
 */
export enum LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARN = 2,
    ERROR = 3,
}

export default class Log {
    // 当前日志级别（生产环境自动设置为 WARN）
    private static currentLevel: LogLevel = typeof __DEV__ !== 'undefined' && !__DEV__ 
        ? LogLevel.WARN 
        : LogLevel.DEBUG;

    /**
     * 设置日志级别
     * @param level 日志级别
     */
    static setLevel(level: LogLevel): void {
        Log.currentLevel = level;
    }

    /**
     * 获取当前日志级别
     */
    static getLevel(): LogLevel {
        return Log.currentLevel;
    }

    /**
     * DEBUG 级别日志 - 仅开发环境
     * 在生产环境编译时会被 Terser 删除
     */
    static d(msg: string, tag?: string): void {
        if (__DEV__) {
            if (Log.currentLevel <= LogLevel.DEBUG) {
                const prefix = tag ? `[${tag}]` : '[DEBUG]';
                console.log(`${prefix} ${msg}`);
            }
        }
    }

    /**
     * DEBUG 级别日志（带元素信息）- 仅开发环境
     */
    static debug(ele: HTMLElement | null | undefined, msg: string, tag?: string): void {
        if (__DEV__) {
            if (Log.currentLevel <= LogLevel.DEBUG) {
                const prefix = tag ? `[${tag}]` : '[DEBUG]';
                const eleInfo = ele ? `${ele.tagName}.${ele.className || 'no-class'}` : 'null';
                console.log(`${prefix} [${eleInfo}] ${msg}`);
            }
        }
    }

    /**
     * INFO 级别日志 - 仅开发环境
     * 在生产环境编译时会被 Terser 删除
     */
    static i(ele: HTMLElement | null | undefined, log: string, tag?: string): void {
        if (__DEV__) {
            if (Log.currentLevel <= LogLevel.INFO) {
                const prefix = tag ? `[${tag}]` : '[INFO]';
                const eleInfo = ele ? `${ele.tagName}.${ele.className || 'no-class'}` : 'null';
                console.info(`${prefix} [${eleInfo}] ${log}`);
            }
        }
    }

    /**
     * INFO 级别日志（简化版）- 仅开发环境
     */
    static info(msg: string, tag?: string): void {
        if (__DEV__) {
            if (Log.currentLevel <= LogLevel.INFO) {
                const prefix = tag ? `[${tag}]` : '[INFO]';
                console.info(`${prefix} ${msg}`);
            }
        }
    }

    /**
     * WARN 级别日志 - 所有环境
     */
    static w(msg: string, tag?: string): void {
        if (Log.currentLevel <= LogLevel.WARN) {
            const prefix = tag ? `[${tag}]` : '[WARN]';
            console.warn(`${prefix} ${msg}`);
        }
    }

    /**
     * WARN 级别日志（带元素信息）- 所有环境
     */
    static warn(ele: HTMLElement | null | undefined, msg: string, tag?: string): void {
        if (Log.currentLevel <= LogLevel.WARN) {
            const prefix = tag ? `[${tag}]` : '[WARN]';
            const eleInfo = ele ? `${ele.tagName}.${ele.className || 'no-class'}` : 'null';
            console.warn(`${prefix} [${eleInfo}] ${msg}`);
        }
    }

    /**
     * ERROR 级别日志 - 所有环境
     */
    static e(msg: string, tag?: string, error?: Error): void {
        if (Log.currentLevel <= LogLevel.ERROR) {
            const prefix = tag ? `[${tag}]` : '[ERROR]';
            console.error(`${prefix} ${msg}`, error || '');
        }
    }

    /**
     * ERROR 级别日志（带元素信息）- 所有环境
     */
    static error(ele: HTMLElement | null | undefined, msg: string, tag?: string, error?: Error): void {
        if (Log.currentLevel <= LogLevel.ERROR) {
            const prefix = tag ? `[${tag}]` : '[ERROR]';
            const eleInfo = ele ? `${ele.tagName}.${ele.className || 'no-class'}` : 'null';
            console.error(`${prefix} [${eleInfo}] ${msg}`, error || '');
        }
    }

    static taskTimes(): void {
        if (__DEV__) {
            Log.d('taskTimes called', Tag.performance);
        }
    }

    static showValue(ele: HTMLElement, key: string, value: string): void {
        if (__DEV__) {
            Log.debug(ele, `showValue - key: ${key}, value: ${value}`, Tag.store);
        }
    }

    static deleteValue(ele: HTMLElement, key: string): void {
        if (__DEV__) {
            Log.debug(ele, `deleteValue - key: ${key}`, Tag.store);
        }
    }

    static clearTag(map: Map<string, WeakMap<HTMLElement, string>>, key: string): void {
        if (__DEV__) {
            Log.d(`clearTag - key: ${key}`, Tag.store);
        }
    }

    static logCaller(ele: HTMLElement): void {
        if (__DEV__) {
            Log.debug(ele, 'logCaller', Tag.debug);
        }
    }

    static consoleLog(tag: string, ...args: any[]): void {
        if (__DEV__) {
            console.info(`[${tag}]`, ...args);
        }
    }

    static markWithColor(tag: string, color: string, ...elements: HTMLElement[]): void {
        if (__DEV__) {
            Log.d(`markWithColor - tag: ${tag}, color: ${color}, elements: ${elements.length}`, Tag.visual);
        }
    }

    static checkState(position?: string): void {
        if (__DEV__) {
            Log.d(`checkState - position: ${position}`, Tag.state);
        }
    }

    static checkFeatureCache(ele: HTMLElement, key: string, cache: string): void {
        if (__DEV__) {
            Log.debug(ele, `checkFeatureCache - key: ${key}, cache: ${cache}`, Tag.cache);
        }
    }

    static printStyle(ele: HTMLElement, key: string, value: string, tag: string): void {
        if (__DEV__) {
            Log.debug(ele, `style - ${key}: ${value}`, tag);
        }
    }

    /**
     * @deprecated 使用 Log.info(msg, Tag.component) 代替
     */
    static printComInfo(msg: string): void {
        if (__DEV__) {
            Log.info(msg, Tag.component);
        }
    }

    /**
     * @deprecated 使用 Log.info(msg, Tag.node) 代替
     */
    static printNodeInfo(msg: string): void {
        if (__DEV__) {
            Log.info(msg, Tag.node);
        }
    }

    /**
     * @deprecated 使用 Log.d(msg, Tag.debug) 代替
     */
    static printDebugMsg(msg: string): void {
        if (__DEV__) {
            Log.d(msg, Tag.debug);
        }
    }
}
