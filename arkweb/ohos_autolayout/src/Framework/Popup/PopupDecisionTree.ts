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
     * @param allNodes - 页面上所有相关节点的集合数组，用于上下文分析。
     * @param popupInfo - 包含弹窗核心信息的对象，其中最重要的属性是 `root_node` (弹窗的根节点)。
     * @returns {PopupDecisionTreeType} - 返回一个 PopupDecisionTreeType 枚举值，表示该弹窗的最终分类（例如 Center, Bottom, Center_Button_Overlap 等）。
     */
    public static judgePopupDecisionTreeType(allNodes: HTMLElement[], popupInfo: PopupInfo): PopupDecisionTreeType {
        const rootNode = popupInfo.root_node;
        
        const isPickerPopup = PopupDecisionTree.isTimePickerPopup(rootNode);
        if (isPickerPopup) {
            return PopupDecisionTreeType.Picker;
        }
        
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
            if (!rootNode.contains(node)) {
                return false;
            }

            const classList = node.classList ? Array.from(node.classList).join(' ').toLowerCase() : '';
            const style = window.getComputedStyle(node);
            const bgImage = style.backgroundImage?.toLowerCase?.() ?? '';
            const url = style.backgroundImage ?? '';
            const patterns = CCMConfig.getInstance().getcloseButtonPattern() ?? [];

            const matchByClassOrBg = patterns.some(kw => classList.includes(kw) || bgImage.includes(kw));
            const matchByUrl = !matchByClassOrBg && patterns.some(kw => url.includes(kw));
            const matchByImgSrc = node.tagName === 'IMG' && patterns.some(kw => (node as HTMLImageElement).src.includes(kw));

            return matchByClassOrBg || matchByUrl || matchByImgSrc;
        });

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
     * 检查一个根节点 (rootNode) 是否包含一个“滚轮选择器”(Picker)。
     *
     * 检查三个核心特征：
     * 1. 存在 'picker' 类名。
     * 2. 存在一个具有统一内联高度子元素的列表。
     * 3. 存在一个带有 'linear-gradient' 背景的元素。
     *
     *
     * @param rootNode - 要检查的弹窗根元素 (如 .a-view.ant-popup)。
     * @returns {boolean} - 如果是，则返回 true。
     */
    private static isTimePickerPopup(rootNode: HTMLElement): boolean {
        // 1. 初始化三个特征的“信号旗”
        let hasPickerClass = false;
        let hasUniformInlineHeight = false;
        let hasLinearGradient = false;

        // 2. 只遍历一次：获取所有子孙节点
        const allDescendants = rootNode.querySelectorAll('*');

        // 3. 开始单次遍历
        for (const element of Array.from(allDescendants)) {
            if (!(element instanceof HTMLElement)) {
                continue;
            }
            
            // 特征 1: 检查 'picker' 类名
            if (!hasPickerClass) {
                hasPickerClass = this.checkPickerClass(element);
            }

            // 特征 2: 检查统一内联高度的子元素
            if (!hasUniformInlineHeight) {
                // 我们检查 *当前元素* 的 *子元素* 是否满足条件
                hasUniformInlineHeight = this.checkUniformHeight(element);
            }

            // 特征 3: 检查 'linear-gradient'
            if (!hasLinearGradient) {
                hasLinearGradient = this.checkLinearGradient(element);
            }
            
            // 如果三个特征都已找到，立即停止遍历
            if (hasPickerClass && hasUniformInlineHeight && hasLinearGradient) {
                break; 
            }
        }

        Log.d(
            `检测结果: 'picker' 类名=${hasPickerClass}, 统一内联高度=${hasUniformInlineHeight}, linear-gradient=${hasLinearGradient}`,
            Tag.popupDecisionTree
        );

        // 4. 最终裁决
        return hasPickerClass && hasUniformInlineHeight && hasLinearGradient;
    }

    /**
     * 【辅助函数】检查单个元素是否在其类名中包含 'picker'。
     */
    private static checkPickerClass(element: HTMLElement): boolean {
        return typeof element.className === 'string' && element.className.includes('picker');
    }

    /**
     * 【辅助函数】检查单个元素的 *直接子元素* 是否具有统一的实际高度（Computed Height）。
     * * @param element 要检查的 HTML 元素。
     * @returns 如果所有子元素的计算高度相同，则返回 true。
     */
    private static checkUniformHeight(element: HTMLElement): boolean {
        const children = element.children;

        // 1. 必须有至少2个子元素才能判断“统一性”
        if (children.length < 2) {
            return false;
        }

        // --- 获取第一个子元素的实际计算高度 ---
        const firstChild = children[0] as HTMLElement;
        
        // 使用 clientHeight 获取元素的可见高度（包含 padding，不包含 border, margin, 滚动条）
        // 或者使用 getBoundingClientRect().height / offsetHeight，但 clientHeight 适用于获取元素内容高度的常见场景。
        const firstChildHeight = firstChild.clientHeight;

        // 如果第一个子元素的高度为 0，通常说明元素未渲染或被隐藏，不认为是有效的统一高度。
        if (firstChildHeight === 0) {
            return false;
        }

        // 2. 遍历剩余子元素，并比较它们的实际高度
        for (let i = 1; i < children.length; i++) {
            const child = children[i] as HTMLElement;
            const childHeight = child.clientHeight;

            // 只要有一个子元素的实际高度与第一个子元素的高度不相等，就返回 false
            if (childHeight !== firstChildHeight) {
                return false;
            }
        }

        // 3. 如果循环完成，说明所有子元素都具有统一的实际高度
        return true;
    }

    /**
     * 【辅助函数】检查单个元素的计算样式是否包含 'linear-gradient' 
     * 并且该渐变中包含半透明颜色（用于判断是否实现了虚化/渐隐效果）。
     * @param element 要检查的 HTML 元素。
     * @returns 如果找到包含半透明颜色的 linear-gradient，则返回 true。
     */
    private static checkLinearGradient(element: HTMLElement): boolean {
        const computedStyle = window.getComputedStyle(element);

        // --- 增加对 mask-image 的检查 ---
        const bgImage = computedStyle.backgroundImage;
        const maskImage = computedStyle.maskImage;

        // 将所有需要检查的属性值放入一个数组
        const checkValues = [bgImage, maskImage].filter(Boolean) as string[];

        for (const cssValue of checkValues) {
            // 1. 快速失败：如果连 "linear-gradient" 字符串都没有，跳过
            if (!cssValue.includes('linear-gradient')) {
                continue;
            }

            // 2. 使用正则表达式提取所有可能的颜色值
            const colorRegex = /(rgba?\(.*?\)|hsla?\(.*?\)|#\w{3,8}|transparent)/gi;
            const foundColors = cssValue.match(colorRegex);

            if (!foundColors) {
                continue;
            }

            // 3. 检查是否有半透明颜色或 'transparent' 关键字
            const isFaded = foundColors.some(color => Utils.isColorSemiTransparent(color));

            if (isFaded) {
                return true; // 只要在一个属性中找到，就返回 true
            }
        }

        return false;
    }

    /**
     * 判断是否紧贴底部的模态窗口，特征：
     * 1、满屏宽
     * 2、bottom=0px
     * 3、boxSizing == 'border-box'
     * 4、top可能有radius，bottom没有radius
     */
    static isModalWin(allNodes: HTMLElement[], rootNode: HTMLElement, popupInfo: PopupInfo): boolean {
        // 查找作为主要内容的节点（z-index 最高）。
        const contentNode = this.findMainContentNode(rootNode, popupInfo);
        if (!contentNode) {
            return false;
        }

        if (this.CheckCenterCloseButton(contentNode, allNodes)) {
            return false;
        }
 
        popupInfo.content_node = contentNode;
 
        // 根据不同的 popup 类型，分派给专门的函数处理。
        switch (popupInfo.popup_type) {
            case PopupType.C:
                return PopupDecisionTree.isModalForTypeC(popupInfo);
            case PopupType.B:
                return PopupDecisionTree.isModalForTypeB(popupInfo);
            default:
                // 对于类型 'A' 或其他未知的类型，直接返回 false。
                return false;
        }
    }


    /**
     * 检查一种典型中心弹窗关闭按钮的特征
     * 特征1：弹窗只有一个关闭按钮；
     * 特征2：该关闭按钮在屏幕下方；
     * 特征3：该关闭按钮大小有限制。
     * @returns {boolean} 
     */
    private static CheckCenterCloseButton(contentNode: HTMLElement, allNodes: HTMLElement[]): boolean {
        const closeElements = PopupDecisionTree.getCloseButtons(contentNode, allNodes) as HTMLElement[];
        if (closeElements.length < 1) {
            return false;
        }
        
        const rect = closeElements[0].getBoundingClientRect();
        const isTypicalCenterCloseButton = closeElements.length === 1 && 
            rect.height < window.innerHeight * Constant.maxCloseButtonSizeRatio &&
            rect.bottom > window.innerHeight * Constant.bottomCloseButtonRatio;                               
        return isTypicalCenterCloseButton; 
    }

    /**
     *  PopupType.C 判断逻辑。
     */
    private static isModalForTypeC(popupInfo: PopupInfo): boolean {
        const rootNode = popupInfo.root_node;
        const contentNode = popupInfo.content_node;
        return this.judgeModalConditions(rootNode, contentNode);
    }

    /**
     * PopupType.B 判断逻辑。
     */
    private static isModalForTypeB(popupInfo: PopupInfo): boolean {
        const rootNode = popupInfo.root_node;
        const contentNode = popupInfo.content_node;
        // 复用通用的模态条件检查逻辑。
        const isNodeModal = PopupDecisionTree.judgeModalConditions(rootNode, contentNode);
        const hasChildren = contentNode.children.length > 0;

        if (!hasChildren) {
            return isNodeModal;
        }

        if (isNodeModal) {
            return true;
        }

        // 如果节点本身不满足，则检查其所有子节点是否存在满足条件的。
        return Array.from(contentNode.children).some(child =>
            PopupDecisionTree.judgeModalConditions(rootNode, child as HTMLElement)
        );
    }

    /**
     * 在 B 类型弹窗的子节点中，根据 z-index 找到作为“前景内容”的节点。
     * @returns {HTMLElement | null} 返回找到的内容节点，如果找不到或存在多个 z-index 最高的节点，则返回 null。
     */
    private static findMainContentNode(rootNode: HTMLElement, popupInfo: PopupInfo): HTMLElement | null {
        // 找到作为直接子节点的 mask 元素
        let directMaskChild = popupInfo.mask_node;
        if (popupInfo.popup_type === PopupType.B) {
            while (directMaskChild.parentElement !== rootNode) {
                directMaskChild = directMaskChild.parentElement!;
                if (!directMaskChild) {
                    return null; // 如果找不到，则结构异常
                }
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
     * 要求1：满宽
     * 要求2：贴底，即bottom为0
     * 要求3：不贴顶，即top不为0
     */
    static judgeModalConditions(rootNode: HTMLElement, contentNode: HTMLElement): boolean {
        // 特殊中心弹窗条件
        if (PopupDecisionTree.specialCenterCondition(contentNode)) {
            return false;
        }
        // 特殊底部弹窗条件
        if (PopupDecisionTree.specialBottomCondition(rootNode, contentNode)) {
            return true;
        }

        const style = window.getComputedStyle(contentNode);
        const { position, bottom, top } = style;
        const rect = contentNode.getBoundingClientRect();

        // 核心判断逻辑
        const isFullWidth = PopupDecisionTree.equalToScreenWidth(contentNode, Constant.discrepancy);
        const isStickToBottom = Math.abs(rect.bottom - window.innerHeight) < Constant.discrepancy;
        const isStickToTop = Math.abs(rect.top) < Constant.discrepancy;
        
        if (!isFullWidth) {
            return false;
        }

        // 判断position为static并且bottom为auto的特殊情况
        // 判断它的bottom的rect值是否贴底、top的rect值是否贴顶
        if (position === Constant.static) {
            if (bottom === 'auto') {
                return isStickToBottom && !isStickToTop;
            }
        
        }
        if (parseFloat(bottom) !== 0) { 
            return false;
        }   
        
        const hasBottomStyle = LayoutUtils.hasBottomStyle(contentNode, position, bottom);
        if (!hasBottomStyle) {
            return false;
        }

        if (top.endsWith('px')) {
            if (parseFloat(top) === 0) { 
                if (LayoutUtils.hasTopStyle(contentNode, position, top)) {
                    return false;
                }
            } else {
                return true;
            }
            
        }

        return isStickToBottom;
    }

    /**
     * @description: 特殊的底部弹窗判断条件
     * @param {HTMLElement} rootNode - 弹窗根节点
     * @param {HTMLElement} contentNode - 弹窗内容节点
     * @return {boolean} 
     */
    static specialBottomCondition(rootNode: HTMLElement, contentNode: HTMLElement): boolean {
        const style = window.getComputedStyle(contentNode);
        const rootStyle = window.getComputedStyle(rootNode);
        return (style.flexDirection === 'column' && (style.justifyContent === 'flex-end' || style.justifyContent === 'end')) ||
            (style.flexDirection === 'row' && (style.alignItems === 'flex-end' || style.alignItems === 'end')) ||
            (rootStyle.flexDirection === 'column' && (rootStyle.justifyContent === 'flex-end' || rootStyle.justifyContent === 'end')) ||
            (rootStyle.flexDirection === 'row' && (rootStyle.alignItems === 'flex-end' || rootStyle.alignItems === 'end'))
    }

    /**
     * @description: 特殊的中心弹窗判断条件
     * @param {HTMLElement} contentNode - 弹窗内容节点
     * @return {boolean} 
     */
    static specialCenterCondition(contentNode: HTMLElement): boolean {
        const style = window.getComputedStyle(contentNode);
        return (style.flexDirection === 'row' && style.alignItems === 'center') ||
            (style.flexDirection === 'column' && style.justifyContent === 'center');
    }

    // 校验满宽条件，允许存在偏差discrepancy
    static equalToScreenWidth(node: HTMLElement, discrepancy: number): boolean {
        const style = window.getComputedStyle(node);
        const {paddingLeft, paddingRight, width, boxSizing, borderLeft, borderRight} = style;

        // boxSizing = border-box
        if (boxSizing === 'border-box') {
            return Math.abs(parseFloat(width) - window.innerWidth) < discrepancy;
        }
        
        // boxSizing = content-box
        if (isNaN(parseFloat(paddingLeft)) || isNaN(parseFloat(paddingRight)) ||
            isNaN(parseFloat(borderLeft)) || isNaN(parseFloat(borderRight)) ||
            boxSizing !== 'content-box') {
            return false;
        }
        return Math.abs(parseFloat(width) + parseFloat(paddingLeft) + parseFloat(paddingRight) - window.innerWidth) < discrepancy;
    }
}