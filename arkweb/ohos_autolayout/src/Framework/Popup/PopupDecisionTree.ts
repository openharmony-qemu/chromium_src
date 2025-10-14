import { PopupType } from './PopupType';
import { PopupInfo } from './PopupInfo';
import Utils from '../Utils/Utils';
import LayoutUtils from '../Utils/LayoutUtils';
import { CCMConfig } from '../Common/CCMConfig';
import { PopupDecisionTreeType } from './PopupDecisionTreeType';
import Constant from '../Common/Constant';
import Log from '../../Debug/Log';
import Tag from '../../Debug/Tag';


interface NearestSibling {
    index: number;
    element: HTMLElement;
}

export class PopupDecisionTree {
    private static discrepancy: number = 1.0;

    /**
     * 检查中心弹窗是否含有与兄弟节点重叠的“绝对定位”关闭按钮
     * @returns {boolean} true 如果存在重叠的按钮, 否则 false
     */
    private static hasOverlappingCloseButton(rootNode: HTMLElement, allNodes: HTMLElement[], popupInfo: PopupInfo): boolean {
        const closeElements: HTMLElement[] = PopupDecisionTree.getCloseButtons(rootNode, allNodes) as HTMLElement[];

        // 如果没有关闭按钮，直接返回 false
        if (closeElements.length === 0) {
            Log.d('无关闭按钮', Tag.popupDecisionTree);
            return false;
        }

        const isAbsoluteFlags: boolean[] = PopupDecisionTree.isCloseElementAbsolute(closeElements, rootNode, popupInfo);

        const isOverlay = closeElements.some((element, index) => {
            // 修正了原始代码中可能存在的bug（index不会在循环中重置）
            const isAbsolute = isAbsoluteFlags[index];
            return isAbsolute && PopupDecisionTree.hasOverlapWithSiblings(element);
        });

        if (isOverlay) {
            Log.d(`特例1: 关闭按钮存在重叠，弹窗: ${popupInfo?.root_node?.className}`, Tag.popupDecisionTree);
            return true;
        } else {
            Log.d('无重叠', Tag.popupDecisionTree);
            return false;
        }
    }

    /**
     * 根据弹窗的视觉和结构属性（如位置、关闭按钮状态等），判断并归类弹窗的具体类型。
     *
     * @param mComponent - 当前正在分析的组件根元素。
     * @param allNodes - 页面上所有相关节点的集合数组，用于上下文分析。
     * @param popupInfo - 包含弹窗核心信息的对象，其中最重要的属性是 `root_node` (弹窗的根节点)。
     * @returns {PopupDecisionTreeType} - 返回一个 PopupDecisionTreeType 枚举值，表示该弹窗的最终分类（例如 Center, Bottom, Center_Button_Overlap 等）。
     */
    public static judgePopupDecisionTreeType(mComponent: HTMLElement, allNodes: HTMLElement[], popupInfo: PopupInfo): PopupDecisionTreeType {
        const rootNode = popupInfo.root_node;
        
        const isBottomPopup = PopupDecisionTree.isModalWin(allNodes, rootNode, popupInfo);

        if (isBottomPopup) {
            // @ts-ignore
            window.popWin = 'bottom';
            Log.d('判定为底部弹窗', Tag.popupDecisionTree);
            return PopupDecisionTreeType.Bottom;
        }

        // @ts-ignore
        window.popWin = 'center';
        Log.d('判定为中心弹窗', Tag.popupDecisionTree);

        if (this.hasOverlappingCloseButton(rootNode, allNodes, popupInfo)) {
            return PopupDecisionTreeType.Center_Button_Overlap;
        } else {
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
            const classList = node.classList ? Array.from(node.classList).join(' ').toLowerCase() : '';
            const style = window.getComputedStyle(node);
            const bgImage = style.backgroundImage.toLowerCase();
            const url = style.backgroundImage;
            return rootNode.contains(node) &&
                    (CCMConfig.getInstance().getcloseButtonPattern()?.some(kw => classList.includes(kw) || bgImage.includes(kw)) ||
                    CCMConfig.getInstance().getcloseButtonPattern()?.some(kw => url.includes(kw)) || 
                    (node.tagName === 'IMG' && CCMConfig.getInstance().getcloseButtonPattern()?.some(kw => (node as HTMLImageElement).src.includes(kw))));
        })

        // 过滤被其他节点包含的节点
        let closeButtons: HTMLElement[] = this.filterContainedNodes(tmpCloseButtons);
        return closeButtons;
    }

    private static filterContainedNodes(nodes: HTMLElement[]): HTMLElement[] {
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
     * 检查单个元素或其祖先节点（直到 stopNode）是否为 'absolute' 定位。
     * @param element - 要检查的起始 HTML 元素。
     * @param stopNode - 向上遍历时停止的祖先节点。
     * @returns {boolean} - 如果找到 absolute 定位的元素则返回 true，否则返回 false。
     */
    private static isElementOrAncestorAbsolute(element: HTMLElement, stopNode: HTMLElement | null): boolean {
        // 检查元素自身
        let style = window.getComputedStyle(element);
        if (style.position === 'absolute') {
            return true;
        }

        // 使用一个新变量来遍历祖先节点，避免修改原始数据
        let currentNode = element.parentElement;

        // 向上遍历 DOM 树
        while (currentNode && currentNode !== stopNode) {
            style = window.getComputedStyle(currentNode);
            const rect = currentNode.getBoundingClientRect();
            
            // 检查祖先节点是否是 absolute 并且尺寸较小
            if (style.position === 'absolute' && rect.height < 50 && rect.width < 50) {
                return true;
            }
            currentNode = currentNode.parentElement;
        }

        return false;
    }


    /**
     * @description: 判断 closeElement 数组中每个元素或其祖先的 position 是否是 absolute。
     * @param {HTMLElement[]} closeElements - 需要检查的关闭按钮节点数组。
     * @param {HTMLElement} rootNode - 弹窗根节点。
     * @param {PopupInfo} popupInfo - 弹窗的附加信息。
     * @return {boolean[]} - 返回一个布尔值数组，对应每个 closeElement 的检查结果。
     */
    private static isCloseElementAbsolute(closeElements: HTMLElement[], rootNode: HTMLElement, popupInfo: PopupInfo): boolean[] {
        const { popup_type, mask_node } = popupInfo;

        // 1. 首先确定向上遍历的终点节点
        const stopNode = (popup_type === PopupType.B || popup_type === PopupType.C) 
            ? rootNode 
            : mask_node;

        // 2. 使用 .map() 方法，对每个元素应用判断逻辑，返回新数组
        return closeElements.map(element => 
            this.isElementOrAncestorAbsolute(element, stopNode)
        );
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

            if (siblingStyle.position === 'absolute') {  // 相邻节点的position是absolute，直接判断是否重叠
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
                !Utils.isBackgroundSemiTransparent(style)
        });
    
        if (children.length === 0) {
            return [];
        }
        // 获取 mask 节点的 z-index
        const maskStyle = window.getComputedStyle(popupInfo.mask_node);
        let maskZIndex: string | number = maskStyle.zIndex;
        if (popupInfo.popup_type === PopupType.C || popupInfo.popup_type === PopupType.A) {
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

        Log.d(`topmostChildren 数量: ${topmostChildren.length}`, Tag.popupDecisionTree);
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
        // 1. 初步筛选：检查关闭按钮的位置，这是一个通用的前置条件。
        if (!this.passesCloseButtonCheck(rootNode, allNodes)) {
            return false;
        }

        // 2. 根据不同的 popup 类型，分派给专门的函数处理。
        switch (popupInfo.popup_type) {
            case PopupType.C:
                return this.isModalForTypeC(rootNode);
            case PopupType.B:
                return this.isModalForTypeB(rootNode, popupInfo);
            default:
                // 对于类型 'A' 或其他未知的类型，直接返回 false。
                return false;
        }
    }


    /**
     * 检查关闭按钮是否满足特定位置要求。
     * @returns {boolean} 如果所有按钮都通过检查，则返回 true。
     */
    private static passesCloseButtonCheck(rootNode: HTMLElement, allNodes: HTMLElement[]): boolean {
        const closeElements = PopupDecisionTree.getCloseButtons(rootNode, allNodes) as HTMLElement[];
        // 使用 .some() 可以让代码更简洁：如果“存在”一个不满足条件的按钮，则检查失败。
        const hasInvalidCloseButton = closeElements.some(element => {
            const rect = element.getBoundingClientRect();
            return rect.width < 50 && rect.height < 50 && rect.bottom > window.innerHeight * 0.6;
        });
        return !hasInvalidCloseButton; // 如果没有无效按钮，则检查通过。
    }

    /**
     *  PopupType.C 判断逻辑。
     */
    private static isModalForTypeC(rootNode: HTMLElement): boolean {
        // 卫语句：如果子节点多于一个，则不满足条件。
        if (PopupDecisionTree.hasMoreThanNumChild(rootNode, 1)) {
            return false;
        }

        const contentNode = rootNode.firstElementChild as HTMLElement | null;
        // 卫语句：如果不存在内容节点，则不满足条件。
        if (!contentNode) {
            return false;
        }

        const style = window.getComputedStyle(contentNode);
        const { position, bottom, flexDirection, alignItems } = style;
        const rect = contentNode.getBoundingClientRect();

        // 核心判断逻辑
        const isFlushWithBottom = Math.abs(rect.bottom - window.innerHeight) < this.discrepancy;
        const isNotFlexRowCenter = !(flexDirection === 'row' && alignItems === 'center');

        // 注意：原始代码这里有逻辑问题（存在不可达代码），这里进行了修正和简化。
        // 检查是否明确设置了 bottom: 0px 或其位置紧贴底部。
        if (parseFloat(bottom) === 0) {
            const hasBottomStyle = LayoutUtils.hasBottomStyle(contentNode, position, bottom);
            if (!hasBottomStyle) {
                return false;
            }
        } else {
            if (position === Constant.absolute || position === Constant.fixed) {
                return false;
            }
        }

        return isFlushWithBottom && isNotFlexRowCenter;
    }

    /**
     * PopupType.B 判断逻辑。
     */
    private static isModalForTypeB(rootNode: HTMLElement, popupInfo: PopupInfo): boolean {
        // 卫语句：B 类型的弹窗至少需要 mask 和 content 两个子节点。
        if (rootNode.children.length < 2) {
            return false;
        }
        
        // 查找作为主要内容的节点（z-index 最高）。
        const contentNode = this.findMainContentNode(rootNode, popupInfo.mask_node);
        if (!contentNode) {
            return false;
        }
        
        // 复用通用的模态条件检查逻辑。
        return this.checkNodeAndChildrenAreModal(contentNode);
    }

    /**
     * 在 B 类型弹窗的子节点中，根据 z-index 找到作为“前景内容”的节点。
     * @returns {HTMLElement | null} 返回找到的内容节点，如果找不到或存在多个 z-index 最高的节点，则返回 null。
     */
    private static findMainContentNode(rootNode: HTMLElement, maskNode: HTMLElement): HTMLElement | null {
        // 找到作为直接子节点的 mask 元素
        let directMaskChild = maskNode;
        while (directMaskChild.parentElement !== rootNode) {
            directMaskChild = directMaskChild.parentElement!;
            if (!directMaskChild) {
                return null; // 如果找不到，则结构异常
            }
        }

        // 过滤掉 mask，剩下的就是内容节点
        const contentNodes = Array.from(rootNode.children).filter(node => node !== directMaskChild) as HTMLElement[];

        if (contentNodes.length === 0) {
            return null;
        }
        if (contentNodes.length === 1) {
            return contentNodes[0];
        }
        
        // 如果有多个内容节点，通过 z-index 判断哪一个在最上层
        let topNode: HTMLElement | null = null;
        let maxZIndex = -Infinity;
        let zIndexCount = 0;

        for (const node of contentNodes) {
            const style = window.getComputedStyle(node);
            const zIndex = style.zIndex === 'auto' ? 0 : parseFloat(style.zIndex);
            if (zIndex > maxZIndex) {
                maxZIndex = zIndex;
                topNode = node;
                zIndexCount = 1;
            } else if (zIndex === maxZIndex) {
                zIndexCount++;
            }
        }
        
        // 如果存在多个 z-index 最高的节点，则无法判断，视为不满足条件。
        return zIndexCount === 1 ? topNode : null;
    }


    /**
     *  检查一个节点本身或其所有子节点是否满足模态条件。
     */
    private static checkNodeAndChildrenAreModal(node: HTMLElement): boolean {
        const isNodeModal = PopupDecisionTree.judgeModalConditions(node);
        const style = window.getComputedStyle(node);
        
        if (node.children.length === 0) {
            const isNotFlexRowCenter = !(style.flexDirection === 'row' && style.alignItems === 'center');
            return isNodeModal && isNotFlexRowCenter;
        }

        // 如果节点本身不满足，则检查其所有子节点是否都满足。
        if (!isNodeModal) {
            // 使用 .every() 检查是否“所有”子节点都满足条件。
            return Array.from(node.children).every(child => 
                PopupDecisionTree.judgeModalConditions(child as HTMLElement)
            );
        }
        
        return true;
    }

    static judgeModalConditions(node: HTMLElement): boolean {
        const style = window.getComputedStyle(node);
        const computedPosition = style.position;
        const computedBottom = style.bottom;
        const computedWidth = style.width;
        if (parseFloat(computedBottom) !== 0 ||
            (parseFloat(computedWidth) !== window.innerWidth && !PopupDecisionTree.equalToScreenWidth(node, this.discrepancy))) {
            return false;
        }
        const hasBottomStyle = LayoutUtils.hasBottomStyle(node, computedPosition, computedBottom);
        if (!hasBottomStyle) {
            return false;
        }

        const rect = node.getBoundingClientRect();
        return Math.abs(rect.bottom - window.innerHeight) < this.discrepancy && !(style.flexDirection === 'row' && style.alignItems === 'center');
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