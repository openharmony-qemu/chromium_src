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

    private static activePopupWindow: { popupInfo: PopupInfo | null; popupComponent: AComponent | null } = {
        popupInfo: null,
        popupComponent: null,
    };

    public static getActivePopupWindowInfo(): PopupInfo | null {
        return IntelligentLayout.activePopupWindow.popupInfo;
    }

    public static getActivePopupWindowComponent(): AComponent | null {
        return IntelligentLayout.activePopupWindow.popupComponent;
    }

    private static setActivePopupWindow(popupInfo: PopupInfo, component: AComponent): void {
        IntelligentLayout.activePopupWindow = { popupInfo: popupInfo, popupComponent: component };
    }

    public static clearActivePopupWindow(): void {
        IntelligentLayout.activePopupWindow = { popupInfo: null, popupComponent: null };
    }
    
    public static intelligentLayout(root: HTMLElement): void {
        Log.info('进入 intelligentLayout', IntelligentLayout.TAG);

        const activePopup = IntelligentLayout.getActivePopupWindowInfo();
        const popupInfo = activePopup ?? PopupWindowDetector.findPopups(root);
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

    public static removePopwinCache(node: HTMLElement): boolean {
        const popupInfo = IntelligentLayout.getActivePopupWindowInfo();
        if (!popupInfo || !popupInfo.root_node) {
            return false;
        }

        if (node.contains(popupInfo.root_node)) {
            Log.info(`弹窗消失: ${popupInfo.root_node}`, IntelligentLayout.TAG);

            PopupStateManager.clearState(popupInfo.root_node);
            IntelligentLayout.clearActivePopupWindow();
            return true;
        }

        return false;
    }

    static calculateForPopWin(popupInfo: PopupInfo): void {
        Log.d(`calculate for popWindow ${popupInfo?.root_node?.className}`, IntelligentLayout.TAG);
        
        //  步骤1：检查弹窗状态机 - 是否可以开始布局
        if (!PopupStateManager.canStartLayout(popupInfo.root_node)) {
            PopupStateManager.printState(popupInfo.root_node, '跳过布局 - 弹窗正在处理中');
            return;
        }
        
        const activePopupInfo = IntelligentLayout.getActivePopupWindowInfo();
        let activePopupComponent = IntelligentLayout.getActivePopupWindowComponent();

        if (!activePopupInfo || activePopupInfo != popupInfo) {
            activePopupComponent = new PopupWindowRelayout(popupInfo);
            IntelligentLayout.setActivePopupWindow(popupInfo, activePopupComponent);
        }

        if (activePopupComponent && activePopupComponent.isDirty()) {
            try {
                activePopupComponent.intelligenceLayout();
            } catch (error) {
                //  步骤4：布局失败
                PopupStateManager.setState(popupInfo.root_node, PopupLayoutState.FAILED, `布局失败: ${error}`);
                Log.e(`布局失败: ${error}`, IntelligentLayout.TAG);
            }
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
        const popupInfo = IntelligentLayout.getActivePopupWindowInfo();
        const component = IntelligentLayout.getActivePopupWindowComponent();

        if (!popupInfo || !component || component.isDirty()) {
            return;
        }

        if (popupInfo.root_node && popupInfo.root_node.contains(item)) {
            component.setDirty(true);
        }            
    }

    static resetPopWindows(reason: string): void {
        Log.d('========== 重置所有弹窗状态并取消异步任务 ==========', IntelligentLayout.TAG);
        const popupInfo = IntelligentLayout.getActivePopupWindowInfo();
        const component = IntelligentLayout.getActivePopupWindowComponent();
        
        if (!popupInfo || !component) {
            IntelligentLayout.clearActivePopupWindow();
            return;
        }

        Log.d(`处理弹窗: ${popupInfo.root_node?.className}`, IntelligentLayout.TAG);
        
        //  步骤1：取消组件的异步验证任务 及 恢复弹窗样式
        if (component instanceof PopupWindowRelayout) {
            component.cancelPendingValidation();
            component.restoreStyles();
        }
        
        //  步骤2：重置弹窗状态为 IDLE
        PopupStateManager.resetState(popupInfo.root_node, reason);
        
        //  步骤3：标记为脏，需要重新布局
        component.setDirty(true);
        
        //  步骤4：清空activePopup，避免reInit()重复恢复样式
        IntelligentLayout.clearActivePopupWindow();
        
        Log.d('所有弹窗状态重置完成', IntelligentLayout.TAG);
    }

    static reInit(reason: string): void {
        IntelligentLayout.resetPopWindows(reason);
    }
}
