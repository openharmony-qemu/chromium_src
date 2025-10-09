/**
 * Module Framework.ts
 * 框架模块
 */
import Cached from '../Common/Cached';
import CSSSheetManage from '../Common/Style/Setter/CSSSheetManage';
import StyleSetter from '../Common/Style/Setter/StyleSetter';
import { Txt } from '../Common/Txt';
import Utils from '../Common/Utils/Utils';
import Log from '../Debug/Log';
import Tag from '../Debug/Tag';
import ObserverHandler from './Observer/ObserverHandler';
import HtmlChangedChecker from './Utils/HtmlChangedChecker';
import WaitSystemReady from './Utils/WaitSystemReady';
import IntelliLayout from './IntelligentLayout';
import IntelligentLayout from './IntelligentLayout';
import { LayoutConstraintMetrics } from '../Framework/Common/LayoutConstraintDetector';
import { SpecificStyleCache } from '../Common/Style/Common/CacheStyleGetter';
import { CCMConfig } from './Common/CCMConfig';
import { Main } from '../Main';

export default class Framework {
    static TAG = Tag.framework;
    static init: boolean = false;
    static stopFlag: boolean = false;
    static forceAllOpenFlag: boolean = false;

    static startTime: number;

    private static isAvailable(): boolean {
        if (Framework.stopFlag) {
            return false;
        }

        if (Framework.forceAllOpenFlag) {
            return true;
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

        if (IntelligentLayout.operation_state === 1) {
            return false;
        } else if (IntelligentLayout.operation_state === 2) {
            return false;
        }

        Cached.clearStyleCache();

        // skip for debug
        if (Framework.isSkeletonScreen()) {
            console.log('骨架屏，暂不处理');
            let metrics: LayoutConstraintMetrics = {
                resultCode: -1,
                errorMsg: '骨架屏，暂不处理',
                duration: 0,
                report: '骨架屏，暂不处理',
            };
            // @ts-ignore
            window.layoutConstraintResult = metrics;
            ObserverHandler.postTask();
            return false;
        }

        return true;
    }

    static mainTask(): void {
        console.log('执行mainTask');
        if (!Framework.taskinit()) {
            return;
        }
        if (!CCMConfig.getInstance().checkRule()) {
            console.log('检查不通过:Appid:'+ CCMConfig.getInstance().getAppID() + 
                                ', Page:' + CCMConfig.getInstance().getPage());
            Main.stop();
            return;
        }
        IntelliLayout.intelligentLayout(document.body);

        // flush新计算的样式，触发回流重绘
        StyleSetter.flushAllStyles();
        Cached.clearStyleCache();
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

        console.log('Framework reInit');
        if (!Framework.init) {
            console.log('change Framework.init to true');
            HtmlChangedChecker.startCheckHtml();
            Framework.init = true;
        }
        // 获取基准FontSize
        console.log('reInitAll ,font-size: ' + document.documentElement.style.fontSize);
        // html节点变化需要重新初始化
        // 此处可能会多次调用注意初始化逻辑
        CSSSheetManage.reInit();
        ObserverHandler.reInit();
        IntelligentLayout.reInit();
    }

    static configReady(): void {
        WaitSystemReady.headReady(Framework.headReadyTask);
    }

    static headReadyTask(): void {
        WaitSystemReady.bodyReady(Framework.reInit);
    }

    private static needRunTask(): boolean {
        if (!Framework.cssIsComplete()) {
            console.log('css 尚未加载完成，暂不执行');
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
            if (child.localName === Txt.link_) {
                const link = child as HTMLLinkElement;
                const rel = link.getAttribute(Txt.rel_);
                const href = link.getAttribute(Txt.href_);
                Log.i(link, 'rel: ' + rel + ' href: ' + href, this.TAG);
                if (rel !== Txt.stylesheet_ || href === '' || !href) {
                    continue;
                }

                Log.i(link, 'sheet: ' + link.sheet, this.TAG);
                if (link.href ?.startsWith('http') && !link.sheet) {
                    return false;
                }
            }
        }

        return true;
    }

    private static isSkeletonScreen(): boolean {
        if (new Date().getTime() - Framework.startTime > 600) {
            return false;
        }

        return true;
    }
}
