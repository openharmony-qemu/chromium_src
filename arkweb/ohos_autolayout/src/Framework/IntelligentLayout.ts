import { AComponent } from './Common/base/AComponent';
import Utils from './Utils/Utils';
import { PopupWindowRelayout } from './Popup/PopupWindowRelayout';
import { PopupWindowDetector } from './Popup/PopupWindowDetector';
import { PopupInfo } from './Popup/PopupInfo';
import Log from '../Debug/Log';
import Tag from '../Debug/Tag';

export default class IntelligentLayout {
    static TAG = Tag.intelligentLayout;
    static ComponentMap = new Map<HTMLElement, AComponent>();

    // 布局参数硬编码的节点缓存
    static HardCodeElementsCache = new Map<HTMLElement,Boolean>();
    // 不需要检查布局参数硬编码的节点缓存
    static DiscardElementsCache = new Set<HTMLElement>();
    static imageCache = new Map<HTMLElement,Boolean>();
    static mEnabled = false; // 默认不生效
    static isAIEnable = false;   // 是否走AI识别的开关
    static screenWidth = window.innerWidth;
    static screenHeight = window.innerHeight;
    static rootNode: HTMLElement = null;

    static popWindowMap = new Map<PopupInfo, AComponent>();
    // 0: init  1:relayout  2:restorelayout
    static operation_state:number = 0;

    private static hasProcessed(element: HTMLElement, candidates: Map<HTMLElement, boolean>): boolean {
        if(!element || !candidates) {
            return false;
        }
        for (const [container,outOfFlow] of candidates) {
            if(container.contains(element)) {
                return true;
            }
        }
        return false;
    }

    static discardElements(comp:HTMLElement): void {
        if(!comp || Utils.shouldSkip(comp)) {
            return;
        }
        if( IntelligentLayout.HardCodeElementsCache.has(comp)) {
            IntelligentLayout.HardCodeElementsCache.delete(comp);
        }
        if( IntelligentLayout.DiscardElementsCache.has(comp)) {
            IntelligentLayout.DiscardElementsCache.delete(comp);
        }
    }

    /**
     * 响应开启重布局的按钮事件，局部刷新节点
     */
    public static relayoutForPopWin(): void {
        Log.info('relayoutForPopWin run', IntelligentLayout.TAG);
        let popupInfo: PopupInfo = null;
        if (this.popWindowMap.size > 0) {
            popupInfo = this.popWindowMap.keys().next().value;
        } else {
            popupInfo = PopupWindowDetector.findPopups(document.body);
        }

        if (popupInfo != null) {
            this.calculateForPopWin(popupInfo);
        }
    }
    
    public static intelligentLayout(root: HTMLElement): void {
        Log.info('intelligentLayout run', IntelligentLayout.TAG);

        this.rootNode = root;
        let popupInfo: PopupInfo = null;
        if (this.popWindowMap.size > 0) {
            popupInfo = this.popWindowMap.keys().next().value;
            // @ts-ignore
            window.popupInfo = popupInfo;
        } else {
            popupInfo = PopupWindowDetector.findPopups(root);
            // @ts-ignore
            window.popupInfo = popupInfo;
        }
        Log.d(`popupInfo root_node: ${popupInfo?.root_node?.className}`, IntelligentLayout.TAG);

        if (popupInfo != null) {
            this.calculateForPopWin(popupInfo);
        }
    }

    public static recoverPopwinStyle(): void {
        if (this.popWindowMap.size > 0) {
            const component:PopupWindowRelayout = this.popWindowMap.values().next().value;
            component.restoreStyles();
        }
        this.popWindowMap.clear();
    }


    public static removePopwinCache(node: HTMLElement): void {
        this.popWindowMap.forEach((popupWindow, popupInfo)=>{
            if (node.contains(popupInfo.root_node)) {
                this.popWindowMap.delete(popupInfo);
            }
        });
    }

    static calculateForPopWin(popupInfo: PopupInfo): void {
        Log.d(`calculate for popWindow ${popupInfo?.root_node?.className}`, IntelligentLayout.TAG);
        const component:AComponent = this.popWindowMap.has(popupInfo) ? this.popWindowMap.get(popupInfo) : new PopupWindowRelayout(popupInfo);
        if (component && component.isDirty()) {
            component.intelligenceLayout();
            // 清除标记
            component.setDirty(false);
            // this.operation_state = 1; // 已经重布局过，避免多次
        } else {
            // fallback,兜底方案
        }

        if(component && !IntelligentLayout.popWindowMap.has(popupInfo)) {
            IntelligentLayout.popWindowMap.set(popupInfo, component);
        }
    }

    // 新增节点是某个组件下的节点，标记这个组件为脏，下一次布局的时候，只需要布局这个组件就可以了
    // todo FIXME: 节点需要精细化处理
    static markDirty(item: MutationRecord): void {
        if(!item) {
            return;
        }
        for (let i = 0; i < item.addedNodes.length; i++) {
            if(Utils.shouldSkip(item.addedNodes[i] as HTMLElement)) {
                continue;
            }
            // workaround,只要发生节点变化，就重新刷新界面
            IntelligentLayout.ComponentMap.forEach(comp => comp.setDirty(true));
            break;
        }
        for (let i = 0; i < item.removedNodes.length; i++) {
            if(Utils.shouldSkip(item.addedNodes[i] as HTMLElement)) {
                continue;
            }
            IntelligentLayout.discardElements(item.addedNodes[i]  as HTMLElement);
        }
    }

    static reInit(): void {
        IntelligentLayout.HardCodeElementsCache.clear();
        IntelligentLayout.recoverPopwinStyle();
        for (const [ele, comp] of IntelligentLayout.ComponentMap) {
            if (!comp) {
                continue;
            }
            comp.setDirty(true);
        }
    }
}
