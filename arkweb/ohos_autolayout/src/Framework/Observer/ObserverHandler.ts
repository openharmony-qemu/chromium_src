import { LayoutKey, LayoutValue } from '../Common/Constant';
import Store from '../Common/Utils/Store';
import Utils from '../Common/Utils/Utils';
import Log from '../../Debug/Log';
import Tag from '../../Debug/Tag';
import Framework from '../Framework';
import Throttle from '../Utils/Throttle';
import ModifyObserver from './Observers/ModifyObserver';
import PageContentObserver from './Observers/PageContentObserver';
import ResizeObserver from './Observers/ResizeObserver';

export default class ObserverHandler {
    static relayoutHandler: Throttle;
    private static readonly TIMEOUT = 200;

    static postTask(): void {
        ObserverHandler.relayoutHandler.postTask();
        Log.d('任务已加入节流队列', Tag.observerHandler);
    }

    static reInit():void {
        Log.info('========== ObserverHandler 重新初始化 ==========', Tag.observerHandler);
        
        Log.d('步骤1: 初始化DOM变更观察器', Tag.observerHandler);
        ModifyObserver.reInit();
        
        Log.d('步骤2: 初始化页面内容观察器', Tag.observerHandler);
        PageContentObserver.reInit();
        
        Log.d('步骤3: 初始化窗口尺寸观察器', Tag.observerHandler);
        ResizeObserver.init_();
        
        if (!ObserverHandler.relayoutHandler) {
            Log.d(`创建节流处理器 (延迟: ${ObserverHandler.TIMEOUT}ms)`, Tag.observerHandler);
            ObserverHandler.relayoutHandler = new Throttle(ObserverHandler.TIMEOUT, Framework.mainTask);
        }
        Log.d('步骤4: 触发任务调度', Tag.observerHandler);
        setTimeout(() => { ObserverHandler.postTask(); }, 400);
        
        Log.info('✅ ObserverHandler初始化完成', Tag.observerHandler);
    }

    static updateObserver(): void {
        Log.d('更新观察器状态', Tag.observerHandler);
        
        if(Framework.stopFlag) {
            Log.w('Framework已停止，移除尺寸监听器', Tag.observerHandler);
            ResizeObserver.removeListener();
        }
        
        if (Utils.isWideScreen()) {
            Log.d('当前为宽屏，保持观察器运行', Tag.observerHandler);
            return;
        }
        
        Log.w(`窄屏检测: ${screen.availWidth}×${screen.availHeight}，移除观察器`, Tag.observerHandler);
        ObserverHandler.removeObserver();
    }

    private static removeObserver(): void {
        Log.info('移除所有观察器', Tag.observerHandler);
        
        Log.d('断开DOM变更观察器', Tag.observerHandler);
        ModifyObserver.disconnect();
        
        Log.d('断开页面内容观察器', Tag.observerHandler);
        PageContentObserver.disconnect();
        
        Log.info('所有观察器已移除', Tag.observerHandler);
    }

    static nextRoundAndPost(ele: HTMLElement): void {
        Log.d(`标记元素进入下一轮并触发任务: ${ele.tagName}.${ele.className}`, Tag.observerHandler);
        ObserverHandler.nextRound(ele);
        ObserverHandler.postTask();
    }

    static nextRound(ele: HTMLElement): void {
        Log.d(`标记元素进入下一轮: ${ele.tagName}.${ele.className}`, Tag.observerHandler);
        Store.setValue(ele, LayoutKey.LAYOUT_TAG, LayoutValue.NEXT_ROUND);
        Store.nextRoundCache.push(ele);
        Log.d(`下一轮缓存大小: ${Store.nextRoundCache.length}`, Tag.observerHandler);
    }
}
