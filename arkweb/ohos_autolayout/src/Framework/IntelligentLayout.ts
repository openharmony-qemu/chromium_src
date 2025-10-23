import { AComponent } from './Common/base/AComponent';
import Utils from './Utils/Utils';
import { PopupWindowRelayout } from './Popup/PopupWindowRelayout';
import { PopupWindowDetector } from './Popup/PopupWindowDetector';
import { PopupInfo } from './Popup/PopupInfo';
import Log from '../Debug/Log';
import Tag from '../Debug/Tag';

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
                IntelligentLayout.popWindowMap.delete(popupInfo);
                hasValidChange = true;
            }
        }
        return hasValidChange;
    }

    static calculateForPopWin(popupInfo: PopupInfo): void {
        Log.d(`calculate for popWindow ${popupInfo?.root_node?.className}`, IntelligentLayout.TAG);
        const component: AComponent = IntelligentLayout.popWindowMap.has(popupInfo) ?
            IntelligentLayout.popWindowMap.get(popupInfo) : new PopupWindowRelayout(popupInfo);
        if (component && component.isDirty()) {
            component.intelligenceLayout();
            // 清除标记
            component.setDirty(false);
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

    static reInit(): void {
        IntelligentLayout.recoverPopwinStyle();
        IntelligentLayout.popWindowMap.clear();
    }
}
