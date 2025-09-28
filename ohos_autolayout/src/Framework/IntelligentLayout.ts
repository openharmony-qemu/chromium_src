import { AComponent } from "./Common/base/AComponent";
import Logger from "./Common/Logger";
import Utils from "./Common/Utils";
import { PerfExecution, Level } from "./Common/Perf";
import { PopWindow } from "./PopWindow/PopWindow";
import { PopupWindow } from "./Popup/PopupWindow";
import { PopupInfo } from "./Popup/PopupInfo";

export default class IntelligentLayout {
    static TAG = "IntelligentLayout";
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

    static discardElements(comp:HTMLElement) {
        if(!comp || Utils.shouldSkip(comp)) return;
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
        Logger.printComInfo(`relayoutForPopWin run`);
        let popupInfo: PopupInfo = null;
        if (this.popWindowMap.size > 0) {
            popupInfo = this.popWindowMap.keys().next().value;
        } else {
            popupInfo = PopupWindow.findPopups(document.body);
        }

        if (popupInfo != null) {
            this.calculateForPopWin(popupInfo);
        }
    }
    
    @PerfExecution({ level: Level.INFO })
    public static intelligentLayout(root: HTMLElement): void {
        Logger.printComInfo(`intelligentLayout run`);

        this.rootNode = root;
        let popupInfo: PopupInfo = null;
        if (this.popWindowMap.size > 0) {
            popupInfo = this.popWindowMap.keys().next().value;
            // @ts-ignore
            window.popupInfo = popupInfo;
        } else {
            popupInfo = PopupWindow.findPopups(root);
            // @ts-ignore
            window.popupInfo = popupInfo;
        }
        // @ts-ignore
        console.log('intelligentLayout: print popupInfo root_node className = ' + window.popupInfo?.root_node?.className);

        if (popupInfo != null) {
            this.calculateForPopWin(popupInfo);
        }
    }


    public static removePopwinCache(node: HTMLElement) {
        this.popWindowMap.forEach((popupWindow, popupInfo)=>{
            if (node.contains(popupInfo.root_node)) {
                this.popWindowMap.delete(popupInfo);
            }
        });
    }

    private static calculateForPopWin(popupInfo: PopupInfo) {
        Logger.printDebugMsg(`calculate for popWindow ${popupInfo?.root_node?.className} `);
        const component:AComponent = this.popWindowMap.has(popupInfo) ? this.popWindowMap.get(popupInfo) : new PopWindow(popupInfo);
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
    static markDirty(item: MutationRecord) {
        if(!item) return;
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

    @PerfExecution({ level: Level.INFO })
    static reInit() {
        IntelligentLayout.HardCodeElementsCache.clear();
        for (const [ele, comp] of IntelligentLayout.ComponentMap) {
            if (!comp) continue;
            comp.setDirty(true);
        }
    }
}
