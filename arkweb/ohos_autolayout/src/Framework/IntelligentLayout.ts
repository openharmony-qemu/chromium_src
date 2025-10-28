import { AComponent } from './Common/base/AComponent';
import { LayoutConstraintMetrics } from './Common/LayoutConstraintDetector';
import Utils from './Utils/Utils';
import { PopupWindowRelayout } from './Popup/PopupWindowRelayout';
import { PopupWindowDetector } from './Popup/PopupWindowDetector';
import { PopupInfo } from './Popup/PopupInfo';
import Log from '../Debug/Log';
import Tag from '../Debug/Tag';
import { PopupStateManager } from './Popup/PopupStateManager';
import { PopupLayoutState } from './Popup/PopupLayoutState';

export default class IntelligentLayout {
    static TAG = Tag.intelligentLayout;

    static popWindowMap = new Map<PopupInfo, AComponent>();

    /**
     * 响应开启重布局的按钮事件，局部刷新节点
     */
    public static relayoutForPopWin(): void {
        Log.info('relayoutForPopWin run', IntelligentLayout.TAG);
        let popupInfo: PopupInfo = null;
        if (IntelligentLayout.popWindowMap.size > 0) {
            popupInfo = IntelligentLayout.popWindowMap.keys().next().value;
        } else {
            popupInfo = PopupWindowDetector.findPopups(document.body);
        }

        if (popupInfo != null) {
            IntelligentLayout.calculateForPopWin(popupInfo);
        }
    }
    
    public static intelligentLayout(root: HTMLElement): void {
        Log.info('进入 intelligentLayout', IntelligentLayout.TAG);

        let popupInfo = IntelligentLayout.popWindowMap.size > 0?
            IntelligentLayout.popWindowMap.keys().next().value : 
            PopupWindowDetector.findPopups(root);
        Log.d(`popupInfo root_node: ${popupInfo?.root_node?.className}`, IntelligentLayout.TAG);

        if (popupInfo != null) {
            IntelligentLayout.calculateForPopWin(popupInfo);
        } else {
            let metrics: LayoutConstraintMetrics = {
                resultCode: -2,
                errorMsg: 'no popup found',
                duration: 0,
                report: 'no popup found',
            };
            // @ts-ignore
            window.layoutConstraintResult = metrics;
        }
        Log.info('离开 intelligentLayout', IntelligentLayout.TAG);
    }

    public static recoverPopwinStyle(): void {
        for (const component of IntelligentLayout.popWindowMap.values()) {
            if (component instanceof PopupWindowRelayout) {
                component.restoreStyles(); 
            }
        }
        IntelligentLayout.popWindowMap.clear();
    }

    public static removePopwinCache(node: HTMLElement): boolean {
        let hasValidChange:boolean = false;
        for (const [popupInfo, comp] of IntelligentLayout.popWindowMap.entries()) {
            if (node.contains(popupInfo.root_node)) {
                Log.info(`弹窗消失: ${popupInfo.root_node}`, IntelligentLayout.TAG);
                
                //  清理弹窗状态
                PopupStateManager.clearState(popupInfo.root_node);
                
                IntelligentLayout.popWindowMap.delete(popupInfo);
                hasValidChange = true;
            }
        }
        return hasValidChange;
    }

    static calculateForPopWin(popupInfo: PopupInfo): void {
        Log.d(`calculate for popWindow ${popupInfo?.root_node?.className}`, IntelligentLayout.TAG);
        
        //  步骤1：检查弹窗状态机 - 是否可以开始布局
        if (!PopupStateManager.canStartLayout(popupInfo.root_node)) {
            PopupStateManager.printState(popupInfo.root_node, '跳过布局 - 弹窗正在处理中');
            return;
        }
        
        //  步骤2：设置为布局中状态
        PopupStateManager.setState(popupInfo.root_node, PopupLayoutState.LAYOUTING, '开始布局');
        
        const component: AComponent = IntelligentLayout.popWindowMap.has(popupInfo) ?
            IntelligentLayout.popWindowMap.get(popupInfo) : new PopupWindowRelayout(popupInfo);
        if (component && component.isDirty()) {
            try {
                component.intelligenceLayout();
                // 清除标记
                component.setDirty(false);
            } catch (error) {
                //  步骤4：布局失败
                PopupStateManager.setState(popupInfo.root_node, PopupLayoutState.FAILED, `布局失败: ${error}`);
                Log.e(`布局失败: ${error}`, IntelligentLayout.TAG);
            }
        }

        if(component && !IntelligentLayout.popWindowMap.has(popupInfo)) {
            IntelligentLayout.popWindowMap.set(popupInfo, component);
        }
        // @ts-ignore
        window.popupInfo = popupInfo;
    }

    // 新增节点是某个组件下的节点，标记这个组件为脏，下一次布局的时候，只需要布局这个组件就可以了
    static markDirty(item: HTMLElement): void {
        if(!item || Utils.shouldSkip(item)) {
            return;
        }

        // 节点变化，就重新刷新界面
        for (const [info, comp] of IntelligentLayout.popWindowMap.entries()) {
            if(comp.isDirty()) {
	    	continue;
	    }
            if(info && info.root_node && comp && info.root_node.contains(item)) {
                comp.setDirty(true);
            }
        }                
    }

    static resetAllPopWindows(): void {
        Log.d('========== 重置所有弹窗状态并取消异步任务 ==========', IntelligentLayout.TAG);
        
        for (const [popupInfo, component] of IntelligentLayout.popWindowMap.entries()) {
            Log.d(`处理弹窗: ${popupInfo.root_node?.className}`, IntelligentLayout.TAG);
            
            //  步骤1：取消组件的异步验证任务
            if (component instanceof PopupWindowRelayout) {
                component.cancelPendingValidation();
            }
            
            //  步骤2：重置弹窗状态为 IDLE
            PopupStateManager.resetState(popupInfo.root_node, '窗口尺寸变化');
            
            //  步骤3：恢复弹窗样式
            if (component instanceof PopupWindowRelayout) {
                component.restoreStyles();
            }
            
            //  步骤4：标记为脏，需要重新布局
            component.setDirty(true);
        }
        
        //  步骤5：清空popWindowMap，避免reInit()重复恢复样式
        IntelligentLayout.popWindowMap.clear();
        
        Log.d('所有弹窗状态重置完成', IntelligentLayout.TAG);
    }

    static reInit(): void {
        IntelligentLayout.recoverPopwinStyle();
        IntelligentLayout.popWindowMap.clear();
    }
}
