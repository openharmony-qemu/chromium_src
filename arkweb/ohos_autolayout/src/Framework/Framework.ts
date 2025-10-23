/**
 * Module Framework.ts
 * 框架模块
 */
import Cached from './Common/Cached';
import CSSSheetManage from './Common/Style/Setter/CSSSheetManage';
import StyleSetter from './Common/Style/Setter/StyleSetter';
import Utils from './Common/Utils/Utils';
import Log from '../Debug/Log';
import Tag from '../Debug/Tag';
import ObserverHandler from './Observer/ObserverHandler';
import PageContentObserver from './Observer/Observers/PageContentObserver';
import WaitSystemReady from './Utils/WaitSystemReady';
import IntelliLayout from './IntelligentLayout';
import IntelligentLayout from './IntelligentLayout';
import { LayoutConstraintMetrics } from '../Framework/Common/LayoutConstraintDetector';
import { SpecificStyleCache } from './Common/Style/Common/CacheStyleGetter';
import { CCMConfig, CheckRuleStateResult } from './Common/CCMConfig';
import { Main } from '../Main';
import Constant from './Common/Constant';

export default class Framework {
    static TAG = Tag.framework;
    static stopFlag: boolean = false;

    static startTime: number;
    private static layoutLockCount: number = 0;

    static lockLayout(): void {
        Framework.layoutLockCount++;
        Log.info(`Layout上锁:${Framework.layoutLockCount}`, Framework.TAG);
    }

    static unLockLayout(): void {
        Framework.layoutLockCount--;
        if (Framework.layoutLockCount < 0) {
            Log.e(`Layout解锁数量错误: ${Framework.layoutLockCount}`, Framework.TAG);
        }
        Log.info(`Layout解锁: ${Framework.layoutLockCount}`, Framework.TAG);
    }

    private static isLayoutLocked(): boolean {
        Log.info('检查Layout是否锁定', Framework.TAG);
        return Framework.layoutLockCount > 0;
    }

    private static isAvailable(): boolean {
        if (Framework.stopFlag) {
            return false;
        }
        return true;
    }

    static taskinit(): boolean {
        if (!Framework.startTime) {
            Framework.startTime = new Date().getTime();
        }

        // 窄屏不生效
        if (!Utils.isWideScreen() || !Framework.isAvailable()) {
            return false;
        }

        if (!Framework.needRunTask()) {
            ObserverHandler.postTask();
            return false;
        }

        Cached.clearStyleCache();

        // 骨架屏检测优化：使用 PageContentObserver 统一管控
        if (!PageContentObserver.isContentReady()) {
            Log.info('[Framework] 页面内容未就绪（骨架屏/白屏），启动响应式监听', Framework.TAG);
            PageContentObserver.startObserving();

            const metrics: LayoutConstraintMetrics = {
                resultCode: -1,
                errorMsg: '页面内容未就绪，等待内容加载',
                duration: 0,
                report: '启动内容观察器，响应式等待',
            };
            // @ts-ignore
            window.layoutConstraintResult = metrics;

            return false;
        }

        return true;
    }

    static mainTask(): void {
        Log.info('进入 mainTask', Framework.TAG);
        if (!Framework.taskinit()) {
            return;
        }

        if (CCMConfig.getInstance().checkRule() === CheckRuleStateResult.outOfWhiteList) {
            Log.w('检查不通过:Appid:' + CCMConfig.getInstance().getAppID() +
                ', Page:' + CCMConfig.getInstance().getPage(), Framework.TAG);
            Main.stop();
            return;
        } else if (CCMConfig.getInstance().checkRule() === CheckRuleStateResult.initial) {
            Log.info('初始状态, 不检查', Framework.TAG);
            return;
        }
        if (!Framework.isLayoutLocked()) {
            Log.info('MainTask 执行重布局', Framework.TAG);
            IntelliLayout.intelligentLayout(document.body);

            // flush新计算的样式，触发回流重绘
            StyleSetter.flushAllStyles();
            Cached.clearStyleCache();
        }
        Log.info('离开 mainTask', Framework.TAG);
    }

    static recoverStyle(): void {
        IntelliLayout.recoverPopwinStyle();
    }

    static reInit(): void {
        if (!Utils.isWideScreen()) {
            return;
        }

        SpecificStyleCache.init();
        Cached.clearAllCache();

        Log.info('Framework reInit', Framework.TAG);
        // html节点变化需要重新初始化
        // 此处可能会多次调用注意初始化逻辑
        CSSSheetManage.reInit();
        IntelligentLayout.reInit();
        ObserverHandler.reInit();
        Framework.layoutLockCount = 0;
        Framework.stopFlag = false;
        Framework.startTime = 0;
    }

    static configReady(): void {
        WaitSystemReady.headReady(Framework.headReadyTask);
    }

    static headReadyTask(): void {
        WaitSystemReady.bodyReady(Framework.reInit);
    }

    private static needRunTask(): boolean {
        if (!Framework.cssIsComplete()) {
            Log.d('css 尚未加载完成，暂不执行', Framework.TAG);
            return false;
        }

        return true;
    }

    /**
     * 判断是否所有css都加载完成
     * @private
     */
    private static cssIsComplete(): boolean {
        const head = document.head;

        for (let i = 0; i < head.children.length; i++) {
            const child = head.children[i] as HTMLElement;
            // 这里应该只判断rel=stylesheet的link
            if (child.localName === Constant.link) {
                const link = child as HTMLLinkElement;
                const rel = link.getAttribute(Constant.rel);
                const href = link.getAttribute(Constant.href);
                Log.i(link, `rel: ${rel}, href: ${href}`, this.TAG);
                if (rel !== Constant.stylesheet || href === '' || !href) {
                    continue;
                }

                Log.i(link, `sheet: ${link.sheet}`, this.TAG);
                if (link.href?.startsWith('http') && !link.sheet) {
                    return false;
                }
            }
        }

        return true;
    }


}
