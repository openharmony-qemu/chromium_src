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

    private static isAvailable(): boolean {
        if (Framework.stopFlag) {
            Log.d('Framework已停止，不可用', Framework.TAG);
            return false;
        }
        return true;
    }

    static taskinit(): boolean {
        Log.d('========== 任务初始化检查 ==========', Framework.TAG);
        
        if (!Framework.startTime) {
            Framework.startTime = new Date().getTime();
            Log.d(`记录启动时间: ${Framework.startTime}`, Framework.TAG);
        }

        // 窄屏不生效
        const isWideScreen = Utils.isWideScreen();
        const isAvailable = Framework.isAvailable();
        Log.d(`屏幕检查: ${isWideScreen ? '宽屏✅' : '窄屏❌'}, 可用性: ${isAvailable ? '可用✅' : '不可用❌'}`, Framework.TAG);
        
        if (!isWideScreen || !isAvailable) {
            Log.d('任务初始化失败: 屏幕不符合或Framework不可用', Framework.TAG);
            return false;
        }

        if (!Framework.needRunTask()) {
            Log.d('任务条件未满足，重新调度任务', Framework.TAG);
            ObserverHandler.postTask();
            return false;
        }

        Log.d('清理样式缓存', Framework.TAG);
        Cached.clearStyleCache();

        // 骨架屏检测优化：使用 PageContentObserver 统一管控
        if (!PageContentObserver.isContentReady()) {
            Log.info('⚠️ 页面内容未就绪（骨架屏/白屏），启动响应式监听', Framework.TAG);
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

        Log.d('✅ 任务初始化成功', Framework.TAG);
        return true;
    }

    static mainTask(): void {
        Log.info('========== 进入主任务 mainTask ==========', Framework.TAG);
        const startTime = performance.now();
        
        if (!Framework.taskinit()) {
            Log.d('任务初始化失败，退出mainTask', Framework.TAG);
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
        
        Log.info('开始执行智能布局...', Framework.TAG);
        IntelliLayout.intelligentLayout(document.body);

        Log.d('刷新样式并触发重绘', Framework.TAG);
        // flush新计算的样式，触发回流重绘
        StyleSetter.flushAllStyles();
        Cached.clearStyleCache();
        
        const duration = (performance.now() - startTime).toFixed(2);
        Log.info(`✅ 布局执行完成，耗时: ${duration}ms`, Framework.TAG);
        Log.info('========== 离开主任务 mainTask ==========', Framework.TAG);
    }

    static reInit(): void {
        Log.info('========== Framework 重新初始化 ==========', Framework.TAG);
        
        if (!Utils.isWideScreen()) {
            Log.d('非宽屏，跳过初始化', Framework.TAG);
            return;
        }

        Log.d('初始化样式缓存', Framework.TAG);
        SpecificStyleCache.init();
        Cached.clearAllCache();

        Log.info('开始重新初始化各模块...', Framework.TAG);
        // html节点变化需要重新初始化
        // 此处可能会多次调用注意初始化逻辑
        Log.d('步骤1: 重新初始化CSS样式表管理器', Framework.TAG);
        CSSSheetManage.reInit();
        
        Log.d('步骤2: 重新初始化智能布局模块', Framework.TAG);
        IntelligentLayout.reInit();
        
        Log.d('步骤3: 重新初始化观察器处理器', Framework.TAG);
        ObserverHandler.reInit();
        
        Log.d('重置Framework状态', Framework.TAG);
        Framework.stopFlag = false;
        Framework.startTime = 0;
        
        Log.info('✅ Framework重新初始化完成', Framework.TAG);
    }

    static configReady(): void {
        Log.info('配置就绪，等待head准备完成', Framework.TAG);
        WaitSystemReady.headReady(Framework.headReadyTask);
    }

    static headReadyTask(): void {
        Log.info('Head就绪，等待body准备完成', Framework.TAG);
        WaitSystemReady.bodyReady(Framework.reInit);
    }

    private static needRunTask(): boolean {
        Log.d('检查任务运行条件...', Framework.TAG);
        
        if (!Framework.cssIsComplete()) {
            Log.w('⚠️ CSS样式表尚未加载完成，暂不执行任务', Framework.TAG);
            return false;
        }

        Log.d('✅ 任务运行条件满足', Framework.TAG);
        return true;
    }

    /**
     * 判断是否所有css都加载完成
     * @private
     */
    private static cssIsComplete(): boolean {
        Log.d('开始检查CSS加载状态...', Framework.TAG);
        const head = document.head;
        let totalStylesheets = 0;
        let loadedStylesheets = 0;
        let pendingStylesheets: string[] = [];

        for (let i = 0; i < head.children.length; i++) {
            const child = head.children[i] as HTMLElement;
            // 这里应该只判断rel=stylesheet的link
            if (child.localName === Constant.link) {
                const link = child as HTMLLinkElement;
                const rel = link.getAttribute(Constant.rel);
                const href = link.getAttribute(Constant.href);
                
                if (rel !== Constant.stylesheet || href === '' || !href) {
                    continue;
                }

                totalStylesheets++;
                
                if (link.href?.startsWith('http') && !link.sheet) {
                    Log.d(`CSS未加载: ${href}`, Framework.TAG);
                    pendingStylesheets.push(href);
                } else {
                    loadedStylesheets++;
                }
            }
        }

        const isComplete = pendingStylesheets.length === 0;
        Log.d(`CSS加载状态: ${loadedStylesheets}/${totalStylesheets} 已加载 ${isComplete ? '✅完成' : '⏳进行中'}`, Framework.TAG);
        
        if (!isComplete) {
            Log.d(`待加载的样式表: ${pendingStylesheets.join(', ')}`, Framework.TAG);
        }

        return isComplete;
    }


}
