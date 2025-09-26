import { PopupType } from "./PopupType";
import { PopupInfo } from "./PopupInfo";
import Utils from "../Common/Utils";
import LayoutUtils from "../Common/LayoutUtils";
import { CCMConfig } from "../Common/CCMConfig";
import { PopupDecisionTreeType } from "./PopupDecisionTreeType";
import Constant from "../Common/Constant";


interface NearestSibling {
    index: number;
    element: HTMLElement;
}

export class PopupDecisionTree {

    public static judgePopupDecisionTreeType(mComponent: HTMLElement, allNodes: HTMLElement[], popupInfo: PopupInfo): PopupDecisionTreeType {
        const rootNode = popupInfo.root_node;
        
        if (!PopupDecisionTree.isModalWin(allNodes, rootNode, popupInfo)) {  // if (positionType === PopPositionType.center) 
            // @ts-ignore
            window.popWin = "center";
            console.log("----是中心弹窗----"); 
            // 用if判断下面特例
            // 特例1： 先拿到close按钮，判断close是否是absolute，不是的话往上找close按钮是absolute position，判断与close的兄弟节点有重叠
            let closeElements: HTMLElement[] = PopupDecisionTree.getCloseButtons(rootNode, allNodes) as HTMLElement[];
            if (closeElements.length !== 0) {
                let isAbsolute: boolean[] = PopupDecisionTree.isCloseElementAbsolute(closeElements, rootNode, popupInfo);
                let isOverlay: boolean = false;
                for (const closeElement of closeElements) {
                    let index: number = 0;  
                    if (isAbsolute[index] && PopupDecisionTree.hasOverlapWithSiblings(closeElement)) {
                        isOverlay = true;
                        break;
                    }
                    index++;
                }
                if (isOverlay) {
                    // 特例1，特殊处理
                    console.log("-----特例1----:", popupInfo?.root_node?.className);
                    return PopupDecisionTreeType.Center_Button_Overlap;
                } else {
                    // 通用处理
                    console.log("-----无重叠----");
                    return PopupDecisionTreeType.Center;
                }
            } else {
                // 通用处理
                console.log("-----无关闭按钮----");
                return PopupDecisionTreeType.Center;
            }
            
        } else if (PopupDecisionTree.isModalWin(allNodes, rootNode, popupInfo)) {  // else if (positionType === PopPositionType.bottom)
            // @ts-ignore
            window.popWin = "bottom";
            console.log("----是底部弹窗----"); 
            return PopupDecisionTreeType.Bottom;
        } else {
            // 打印弹窗的信息
            console.log("弹窗不属于中心弹窗,也不属于底部弹窗,其rootNode为: " + popupInfo?.root_node?.className);
            return PopupDecisionTreeType.Center;
        }
    }


    /**
     * 获取元素内部的关闭按钮
     * @param {Element | null} el - 要搜索的根元素
     * @returns {Element | null} - 返回关闭按钮
     */
    private static getCloseButtons(rootNode: HTMLElement, allNodes: HTMLElement[]): HTMLElement[] {

        const tmpCloseButtons: HTMLElement[] = allNodes.filter(node => {
            const classList = node.classList ? Array.from(node.classList).join(' ').toLowerCase() : "";
            const style = window.getComputedStyle(node);
            const bgImage = style.backgroundImage.toLowerCase();
            const url = style.backgroundImage;
            return rootNode.contains(node) &&
                    (CCMConfig.getInstance().getcloseButtonPattern()?.some(kw => classList.includes(kw) || bgImage.includes(kw)) ||
                    CCMConfig.getInstance().getSpecialCloseButtonPattern()?.some(kw => url.includes(kw)) || 
                    (node.tagName === "IMG" && CCMConfig.getInstance().getSpecialCloseButtonPattern()?.some(kw => (node as HTMLImageElement).src.includes(kw))));
        })

        // 过滤被其他节点包含的节点
        let closeButtons: HTMLElement[] = this.filterContainedNodes(tmpCloseButtons);
        return closeButtons;
    }

    private static filterContainedNodes(nodes: HTMLElement[]) {
        const elementSet = new Set(nodes);
        const result: HTMLElement[] = [];
        
        for (const element of nodes) {
          let parent = element.parentElement;
          let isContained = false;
          
          // 检查所有祖先是否在原始集合中
          while (parent !== null) {
            if (elementSet.has(parent)) {
              isContained = true;
              break;
            }
            parent = parent.parentElement;
          }
          
          if (!isContained) {
            result.push(element);
          }
        }
        return result;
    }

    /**
     * @description: 判断closeElement（一直向上找到rootNode/maskNode，不包含rootNode/maskNode）的position是否是absolute
     * @param {HTMLElement[]} closeElements - 关闭按钮节点
     * @param {HTMLElement} rootNode - 弹窗根节点
     * @return {boolean[]} - position是absolute返回true，否则false
     */
    private static isCloseElementAbsolute(closeElements: HTMLElement[], rootNode: HTMLElement, popupInfo: PopupInfo): boolean[] {
        let isAbsolute: boolean[] = new Array(closeElements.length).fill(false);
        const popType = popupInfo.popup_type;

        for (let i = 0; i < closeElements.length; i++) {
            let style: CSSStyleDeclaration = window.getComputedStyle(closeElements[i]);
            const topNode = (popType === PopupType.B || popType === PopupType.C) ? rootNode : popupInfo.mask_node;
            if (style.position === "absolute") {
                isAbsolute[i] = true;
            } else {
                while (closeElements[i] && closeElements[i] !== topNode) {
                    closeElements[i] = closeElements[i].parentElement;
                    style = window.getComputedStyle(closeElements[i]);
                    let rect = closeElements[i].getBoundingClientRect();
                    if (style.position === "absolute" && rect.height < 50 && rect.width < 50) {
                        isAbsolute[i] = true;
                        break;
                    }
                }
            }
        }
        
        return isAbsolute;
    }

    /**
     * @description: 判断该节点与自己最近的兄弟节点是否存在重叠
     * @param {HTMLElement} el - 待判断的节点
     * @return {boolean} - 有重叠返回true，反之false
     */
    private static hasOverlapWithSiblings(el: HTMLElement): boolean {
        if (!el || !el.parentElement) {
            return false;
        }
        const index = Array.from(el.parentElement.children).indexOf(el);
        
        // 获取所有兄弟元素（排除自己）
        const siblings: HTMLElement[] = Utils.getVisibleSiblings(el) as HTMLElement[];

        let nearestSiblings: NearestSibling[] = [];
        // 筛选出和el index相邻的节点
        siblings.forEach(sibling => {
            const sIndex = Array.from(sibling.parentElement.children).indexOf(sibling);
            if (sIndex === index - 1 || sIndex === index + 1) {
                nearestSiblings.push({index: sIndex, element: sibling});
            }
        });

        if (nearestSiblings.length === 0) {
            return false;
        }
        
        // 检查是否与相邻兄弟元素重叠
        return nearestSiblings.some(nearestSibling => {
            const siblingStyle = window.getComputedStyle(nearestSibling.element);

            if (siblingStyle.position === "absolute") {  // 相邻节点的position是absolute，直接判断是否重叠
                const ratio = this.calOverlapAreaRatio(nearestSibling.element, el);
                return this.hasOverlap(nearestSibling.element, el) && ratio > 0.2 && ratio < 1;

            } else {  // 否则找离el最近的子节点，并且满足position是absolute，才能判断是否重叠

                if (nearestSibling.index === index - 1) {  // 前一个兄弟节点,找最后面的节点进行重叠判断
                    const lastChild = this.findLastChild(nearestSibling.element);
                    const ratio = this.calOverlapAreaRatio(lastChild, el);
                    return this.hasOverlap(lastChild, el) && ratio > 0.2 && ratio < 1;

                } else {  // 后一个兄弟节点,找最前面的节点进行重叠判断
                    const firstChild = this.findFirstChild(nearestSibling.element);
                    const ratio = this.calOverlapAreaRatio(firstChild, el);
                    return this.hasOverlap(firstChild, el) && ratio > 0.2 && ratio < 1;                  
                }
            }
        });
    }

    /**
     * @description: 循环找到el的第一个元素节点
     * @param {HTMLElement} el
     * @return {HTMLElement}
     */
    private static findFirstChild(el: HTMLElement): HTMLElement {
        if (!el || !el.lastElementChild) {
            return null;
        }
        let firstChild: HTMLElement = el.lastElementChild as HTMLElement;
        while(firstChild.firstElementChild) {
            firstChild = firstChild.firstElementChild as HTMLElement;
        }
        return firstChild;
    }

    /**
     * @description: 循环找到el的最后一个元素节点
     * @param {HTMLElement} el
     * @return {HTMLElement}
     */
    private static findLastChild(el: HTMLElement): HTMLElement | null {
        if (!el || !el.lastElementChild) {
            return null;
        }
        let lastChild: HTMLElement = el.lastElementChild as HTMLElement;
        while(lastChild.lastElementChild) {
            lastChild = lastChild.lastElementChild as HTMLElement;
        }
        return lastChild;
    }

    /**
     * @description: 判断两个元素是否重叠
     * @param {HTMLElement} sibling - 待检测兄弟元素
     * @param {DOMRect} el - 已知元素
     * @return {boolean} - 有重叠返回true，反之返回false
     */    
    private static hasOverlap(sibling: HTMLElement | null, el: HTMLElement): boolean {
        if (sibling === null) {
            return false;
        }
        const siblingRect = sibling.getBoundingClientRect();
        const rect = el.getBoundingClientRect();
        // 括号里的条件都不满足，则表示有重叠
        return !(rect.right < siblingRect.left || rect.left > siblingRect.right || 
                rect.bottom < siblingRect.top || rect.top > siblingRect.bottom);
    }

    /**
     * @description: 计算两个元素的重叠面积占第二个元素的比例
     * @param {HTMLElement} sibling - 待计算兄弟元素
     * @param {HTMLElement} el - 已知元素
     * @return {number}
     */
    private static calOverlapAreaRatio(sibling: HTMLElement | null, el: HTMLElement): number {
        if (sibling === null) {
            return 0;
        }
        const siblingRect = sibling.getBoundingClientRect();
        const rect = el.getBoundingClientRect();

        // 计算重叠区域的边界
        const overlapLeft = Math.max(siblingRect.left, rect.left);
        const overlapRight = Math.min(siblingRect.right, rect.right);
        const overlapTop = Math.max(siblingRect.top, rect.top);
        const overlapBottom = Math.min(siblingRect.bottom, rect.bottom);

        // 计算重叠区域的宽度和高度
        const overlapWidth = Math.max(0, overlapRight - overlapLeft);
        const overlapHeight = Math.max(0, overlapBottom - overlapTop);

        // 计算已知元素面积
        const elArea = rect.width * rect.height;
        
        return elArea > 0 ? (overlapWidth * overlapHeight / elArea) : 0;
    }

    /**
     * 获取给定节点的最上层子节点（或节点集合）
     * @param {HTMLElement} parentNode - 父节点
     * @return {HTMLElement[]} - 最上层的子节点数组（可能多个并列）
     */
    private static getTopmostChildren(parentNode: HTMLElement, popupInfo: PopupInfo): HTMLElement[] {
        const children = Array.from(parentNode.children)
        .filter(child => {
            // 过滤掉不可见或未设置定位的元素
            const style = window.getComputedStyle(child);
            return style.display !== 'none' && 
                style.visibility !== 'hidden' &&
                child instanceof HTMLElement &&
                // parseFloat(style.height) > 0 &&
                // parseFloat(style.width) > 0 &&
                parseFloat(style.opacity) === 1 &&
                !Utils.isBackgroundSemiTransparent(style) &&
                child.id != 'SmartSwitch' &&
                child.id != 'SaveSwitch';
        });
    
        if (children.length === 0) return [];

        // 获取 mask 节点的 z-index
        const maskStyle = window.getComputedStyle(popupInfo.mask_node);
        let maskZIndex: string | number = maskStyle.zIndex;
        if (popupInfo.popup_type == PopupType.C || popupInfo.popup_type == PopupType.A) {
            maskZIndex = -1;
        } else {
            if (maskZIndex === 'auto') {
                maskZIndex = 0; // auto 默认权重为 0
            } else {
                maskZIndex = parseInt(maskZIndex, 10);
            }
        }
    
        // 计算每个子节点的 z-index 权重
        const weightedChildren = children.map(child => {
            const style = window.getComputedStyle(child);
            let zIndex: string|number = style.zIndex;
        
            // 处理 z-index: auto（按 DOM 顺序，后出现的权重更高）
            if (zIndex === 'auto') {
                zIndex = 0; // auto 默认权重为 0，但 DOM 顺序会影响最终比较
            } else {
                zIndex = parseInt(zIndex, 10);
            }
        
            return { element: child as HTMLElement, zIndex: zIndex, domOrder: children.indexOf(child) };
        });
    
        // 筛选出比 mask 节点 z-index 更大的节点（至少等于）
        const filteredChildren = weightedChildren.filter(child => child.zIndex >= maskZIndex);
    
        // 按 z-index 降序 + DOM 顺序升序排序
        filteredChildren.sort((a, b) => {
            if (a.zIndex !== b.zIndex) {
                return b.zIndex - a.zIndex; // z-index 大的在前
            } else {
                return b.domOrder - a.domOrder; // DOM 顺序靠后的在前
            }
        });

        // 提取元素
        const topmostChildren = filteredChildren.map(child => child.element);

        console.log('getTopmostChildren: print topmostChildren size = ' + topmostChildren.length);
        return topmostChildren;
    }

    /**
     * 判断是否紧贴底部的模态窗口，特征：
     * 1、满屏宽
     * 2、bottom=0px
     * 3、boxSizing == 'border-box'
     * 4、top可能有radius，bottom没有radius
     */
    static isModalWin(allNodes: HTMLElement[], rootNode: HTMLElement, popupInfo: PopupInfo): boolean {
        let closeElements: HTMLElement[] = PopupDecisionTree.getCloseButtons(rootNode, allNodes) as HTMLElement[];
        for (const closeElement of closeElements) {
            const rect = closeElement.getBoundingClientRect();
            if (rect.width < 50 && rect.height < 50 && rect.bottom > window.innerHeight * 0.6) {
                return false;
            }
        }
        
        const popType = popupInfo.popup_type;
        if (popType === PopupType.C) {
            if (PopupDecisionTree.hasMoreThanNumChild(rootNode, 1)) {  // content是多节点
                return false;
            } else {  // content是单节点
                const contentNode = rootNode.firstElementChild as HTMLElement | null;
                if (contentNode) {
                    const style = window.getComputedStyle(contentNode);
                    const computedPosition = style.position;
                    const computedBottom = style.bottom;
                    
                    if (parseFloat(computedBottom) !== 0) {
                        if (computedPosition === Constant.absolute || computedPosition == Constant.fixed) {
                            return false;
                        }
                        const rect = contentNode.getBoundingClientRect();
                        return Math.abs(rect.bottom - window.innerHeight) < 0.1 && !(style.flexDirection === "row" && style.alignItems === "center");
                    }  
                    
                    const hasBottomStyle = LayoutUtils.hasBottomStyle(contentNode, computedPosition, computedBottom);

                    if (!hasBottomStyle) {
                        return false;
                    }

                    const rect = contentNode.getBoundingClientRect();
                    return Math.abs(rect.bottom - window.innerHeight) < 0.1 && !(style.flexDirection === "row" && style.alignItems === "center");
                } 
                return false;  // 没有content节点
            }
        } else if (popType === PopupType.B) {
            if (PopupDecisionTree.hasMoreThanNumChild(rootNode, 2)) {  // content是多节点,待讨论
                let tempNode = popupInfo.mask_node;
                while (tempNode.parentElement !== rootNode) {
                    tempNode = tempNode.parentElement;
                }

                const contentNodes = Array.from(rootNode.children).filter(node => {
                    return node !== tempNode;
                }) as HTMLElement[];
                let contentNode = contentNodes[0];
                let maxZIndex = -Infinity;
                let maxNum = 0;
                for (const node of contentNodes) {
                    const style = window.getComputedStyle(node);
                    const zIndex = style.zIndex === "auto" ? 0 : parseFloat(style.zIndex);
                    if (zIndex > maxZIndex) {
                        maxNum = 1;
                        maxZIndex = zIndex;
                        contentNode = node;
                    } else if (zIndex === maxZIndex) {
                        maxNum++;
                    }
                }

                if (maxNum > 1) {
                    return false;
                }
                const isModal = PopupDecisionTree.judgeModalConditions(contentNode);
                const style = window.getComputedStyle(contentNode);
                if (contentNode.children.length === 0) {
                    return isModal && !(style.flexDirection === "row" && style.alignItems === "center");
                }
                if (!isModal) {
                    for (const child of contentNode.children) {
                        if (!PopupDecisionTree.judgeModalConditions(child as HTMLElement)) {
                            return false;
                        }
                    }
                }
                return true;
            } else if (rootNode.children.length === 2) {  // content是单节点
                let tempNode = popupInfo.mask_node;
                while (tempNode.parentElement !== rootNode) {
                    tempNode = tempNode.parentElement;
                }
                const contentNode = (rootNode.children[0] === tempNode ? rootNode.children[1] : rootNode.children[0]) as HTMLElement;
                const isModal = PopupDecisionTree.judgeModalConditions(contentNode);
                const style = window.getComputedStyle(contentNode);
                if (contentNode.children.length === 0) {
                    return isModal && !(style.flexDirection === "row" && style.alignItems === "center");
                }
                if (!isModal) {
                    for (const child of contentNode.children) {
                        if (!PopupDecisionTree.judgeModalConditions(child as HTMLElement)) {
                            return false;
                        }
                    }
                }
                return true;
            } 
            // rootNode的子节点数小于2
            return false;
        }
        // popType = "A"
        return false;
    }

    static judgeModalConditions(node: HTMLElement): boolean {
        const style = window.getComputedStyle(node);
        const computedPosition = style.position;
        const computedBottom = style.bottom;
        const computedWidth = style.width;
        if (parseFloat(computedBottom) !== 0 || (parseFloat(computedWidth) !== window.innerWidth && !PopupDecisionTree.equalToScreenWidth(node, 0.1))) {
            return false;
        }
        const hasBottomStyle = LayoutUtils.hasBottomStyle(node, computedPosition, computedBottom);
        if (!hasBottomStyle) {
            return false;
        }

        const rect = node.getBoundingClientRect();
        return Math.abs(rect.bottom - window.innerHeight) < 0.1 && !(style.flexDirection === "row" && style.alignItems === "center");
    }

    static hasMoreThanNumChild(rootNode: HTMLElement, num: number): boolean {
        const children = rootNode.children;
        return children.length > num;
    }

    // 部分用例的弹窗width为7.5rem，约为326.995px，或者是需要加上padding，所以与屏宽对比时允许存在偏差deviation
    static equalToScreenWidth(node: HTMLElement, deviation: number): boolean {
        const style = window.getComputedStyle(node);
        const paddingLeft = style.paddingLeft;
        const paddingRight = style.paddingRight;
        const computedWidth = style.width;
        if (isNaN(parseFloat(paddingLeft)) || isNaN(parseFloat(paddingRight))) {
            return false;
        }
        return Math.abs(parseFloat(computedWidth) + parseFloat(style.paddingLeft) + parseFloat(style.paddingRight) - window.innerWidth) < deviation;
    }
}