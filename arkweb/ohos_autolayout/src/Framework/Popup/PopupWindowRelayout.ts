import StyleSetter from '../Common/Style/Setter/StyleSetter';
import Log from '../../Debug/Log';
import Tag from '../../Debug/Tag';
import Constant from '../Common/Constant';
import Utils from '../Utils/Utils';
import LayoutUtils from '../Utils/LayoutUtils';
import { VisualBoundingRect } from '../Utils/LayoutUtils';
import { AComponent } from '../Common/base/AComponent';
import { DetectorInst } from '../Common/DetectorInst';
import { LayoutConstraintMetricsDetector, LayoutConstraintMetrics } from '../Common/LayoutConstraintDetector';
import { PopupInfo } from './PopupInfo';
import { PopupType } from './PopupType';
import { PopupDecisionTreeType } from './PopupDecisionTreeType';
import { PopupDecisionTree } from './PopupDecisionTree';
import { CCMConfig } from '../Common/CCMConfig';
import StyleCleaner from '../Common/Style/Setter/StyleCleaner';
import { PopupStateManager } from './PopupStateManager';
import { PopupLayoutState } from './PopupLayoutState';

interface BoundingRect {
    top: number,
    left: number,
    bottom: number,
    right: number
}

/**
 * 弹窗
 * 输入为一个popupInfo
 */
export class PopupWindowRelayout extends AComponent {
    resetStyle(): void {
        throw new Error('Method not implemented.');
    }
    
    //  添加取消令牌字段
    private cancellationToken: { cancelled: boolean; generation: number } = {
        cancelled: false,
        generation: 0
    };
    
    private scale: number = 1.0;
    private minTop: number = Infinity;
    private maxBottom: number = -Infinity;
    private truncateNodes: HTMLElement[] = [];  // 被截断的节点
    private truncateBkgImgNodes: HTMLElement[] = [];    // 背景图被截断的节点（但是rect没有在视口内被截断）
    private popupInfo: PopupInfo;
    private bottomNode: HTMLElement = null;
    private needRestoreStyleNodes: Set<HTMLElement> = new Set<HTMLElement>;
    private needLayoutConstraintNodes: Set<HTMLElement> = new Set<HTMLElement>();
    private layoutConstraintResult: LayoutConstraintMetrics = null;
    private visualHeight: number = window.innerHeight;
    private isCloseButtonTruncatedByScroll = false;
    private popupDecisionTreeType = PopupDecisionTreeType.Center;
    private equivalentMask: HTMLElement;
    private minScaleFactor = CCMConfig.getInstance().getMinScaleFactor() / 100;
    private scaleAnimationDuration = CCMConfig.getInstance().getScaleAnimationDuration();

    constructor(popupInfo: PopupInfo) {
        super(popupInfo.root_node);
        this.popupInfo = popupInfo;
    }

    printTree(color: boolean = false): void { }

    setDirty(dirty: boolean): void {
        this.mDirty = dirty;
    }

    public intelligenceLayout(): void {
        if (this.mComponent === null) {
            Log.d('组件为空，跳过智能布局', Tag.popupRelayout);
            return;
        }
        this.visualHeight = window.innerHeight - this.popupInfo.stickyBottom_height - this.popupInfo.stickyTop_height;

        Log.d(`开始弹窗智能布局: ${this.mComponent.className}, 窗口尺寸: ${window.innerWidth}x${window.innerHeight}, 可视高度: ${this.visualHeight}`, Tag.popupRelayout);
        Log.d(`弹窗类型: ${this.popupInfo.popup_type}, 吸顶高度: ${this.popupInfo.stickyTop_height}, 吸底高度: ${this.popupInfo.stickyBottom_height}`, Tag.popupRelayout);
        this.relayout();
    }
    
    //  公开取消方法
    public cancelPendingValidation(): void {
        this.cancellationToken.cancelled = true;
        Log.d(`取消待处理的验证任务 (generation: ${this.cancellationToken.generation})`, Tag.popupRelayout);
    }

    private relayout():void {
        Log.d('========== 开始弹窗重新布局流程 ==========', Tag.popupRelayout);
        
        //  生成新的取消令牌
        this.cancellationToken = { cancelled: false, generation: Date.now() };
        const token = this.cancellationToken;
        Log.d(`生成新的取消令牌 (generation: ${token.generation})`, Tag.popupRelayout);
        
        const allNodes = this.traverseTree(this.mComponent, []);
        Log.d(`遍历DOM树，共 ${allNodes.length} 个节点`, Tag.popupRelayout);

        // step1: 计算被截断的节点
        this.findTruncateNodes(allNodes);

        if (this.truncateNodes.length === 0) {
            Log.d(`未找到被截断的节点，无需布局调整`, Tag.popupRelayout);
            let metrics: LayoutConstraintMetrics = {
                resultCode: -1,
                errorMsg: 'no truncateNodes found',
                duration: 0,
                report: 'no truncateNodes found',
            };
            // @ts-ignore
            window.layoutConstraintResult = metrics;
            return;
        }
        Log.d(`找到 ${this.truncateNodes.length} 个被截断的节点`, Tag.popupRelayout);

        // step2: 判断弹窗决策树类型
        this.popupDecisionTreeType = PopupDecisionTree.judgePopupDecisionTreeType(allNodes, this.popupInfo);
        Log.d(`弹窗决策树类型: ${this.popupDecisionTreeType}`, Tag.popupRelayout);

        // step3: 恢复背景图片被截断的节点
        if (this.truncateBkgImgNodes.length !== 0) {
            Log.d(`修复 ${this.truncateBkgImgNodes.length} 个背景图被截断的节点`, Tag.popupRelayout);
            this.resetTruncateBkgImgNodes();
        }

        // step4: 计算缩放系数
        this.calScale();
        DetectorInst.getInstance().recordOriginalPosition(this.popupInfo.content_node);

        if (this.scale > 1) {
            Log.d(`缩放系数 ${this.scale.toFixed(3)} > 1，无需缩放`, Tag.popupRelayout);
            return;
        }
        Log.d(`计算缩放系数: ${this.scale.toFixed(3)}`, Tag.popupRelayout);

        // step5: 应用缩放系数
        Log.d('开始应用缩放变换', Tag.popupRelayout);
        this.resetByScale();

        // step6: 修复按钮重合
        if (this.popupDecisionTreeType === PopupDecisionTreeType.Center_Button_Overlap) {
            Log.d('检测到按钮重叠，开始修复', Tag.popupRelayout);
            this.fixButtonOverlap();
        }

        StyleSetter.flushAllStyles();
        Log.d('样式刷新完成', Tag.popupRelayout);
        
        // ⚠️ 注意：不要在这里设置 WAITING_VALIDATION 状态
        // 状态设置移到 getLayoutConstraintReport() 的异步函数开始处
        
        // step7: 自验证算法
        Log.d('开始布局约束验证', Tag.popupRelayout);
        //  传递令牌给异步验证
        this.getLayoutConstraintReport(token);
    }

    /**
     * 获取给定节点的最上层子节点（或节点集合）
     * @param {HTMLElement} parentNode - 父节点
     * @return {HTMLElement[]} - 最上层的子节点数组（可能多个并列）
     */
    private getTopmostChildren(parentNode: HTMLElement, popup_type: number): HTMLElement[] {
        const children = Array.from(parentNode.children)
            .filter(child => {
                // 过滤掉不可见或未设置定位的元素
                const style = window.getComputedStyle(child);
                return style.display !== 'none' &&
                    style.visibility !== 'hidden' &&
                    child instanceof HTMLElement &&
                    parseFloat(style.opacity) === 1 &&
                    !Utils.isBackgroundSemiTransparent(style)
            });

        if (children.length === 0) {
            return [];
        }
        // 获取 mask 节点的 z-index
        const maskStyle = window.getComputedStyle(this.popupInfo.mask_node);
        let maskZIndex: string | number = maskStyle.zIndex;
        if (popup_type === PopupType.C || popup_type === PopupType.A) {
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
            let zIndex: string | number = style.zIndex;

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

        Log.d('getTopmostChildren: print topmostChildren size = ' + topmostChildren.length);
        return topmostChildren;
    }

    /**
    * 过滤 grandChildren，移除 translateY 超过屏幕高度的元素
    * @param {HTMLElement[]} grandChildren - 需要过滤的 grandChildren 数组
    * @returns {HTMLElement[]} 过滤后的 grandChildren 数组
    */
    private getValidGrandChildren(grandChildren: HTMLElement[]): HTMLElement[] {
        return grandChildren.filter(grandchild => {
            const style = getComputedStyle(grandchild);
            const matrixMatch = style.transform.match(/matrix\((.*?),(.*?),(.*?),(.*?),(.*?),(.*?)\)/);
            const translateY = matrixMatch ? parseFloat(matrixMatch[6]) : 0;
            return Math.abs(translateY) <= window.innerHeight;
        });
    }

    /**
     * 计算缩放系数
     * step1：获取所有被截断节点同层节点（兄弟节点），统计他们所有的子孙节点rect区域
     * step2：找出step1中所有rect的minTop和maxBottom
     * step3：scale = (screenHeight * 0.7) / (maxBottom - minTop)???
     * step4: 记录maxBottom对应的bottomNode
     */
    private calScale(): void {
        Log.d('开始计算缩放系数', Tag.popupRelayout);
        this.truncateNodes.forEach(truncateNode => {
            Log.d(`处理被截断节点: ${truncateNode.className}`, Tag.popupRelayout);
            let rect = truncateNode.getBoundingClientRect();
            this.minTop = Math.min(this.minTop, rect.top);
            if (rect.bottom > this.maxBottom) {
                this.bottomNode = truncateNode;
                this.maxBottom = rect.bottom;
            }
            let tmpNode = truncateNode;
            if (truncateNode === this.mComponent) {
                tmpNode = truncateNode;
            } else {
                tmpNode = truncateNode.parentElement;
            }

            const treeWalker = document.createTreeWalker(
                tmpNode,
                NodeFilter.SHOW_ELEMENT,  // 或 NodeFilter.SHOW_ELEMENT 只获取元素节点
                {
                    acceptNode(node) {
                        if (!(node instanceof HTMLElement)) {
                            return NodeFilter.FILTER_REJECT;
                        }
                        // 如果是一个可以滚动的列表，则子元素不纳入缩放系数的计算
                        const style = window.getComputedStyle(node.parentElement);

                        const overflowY = style.overflowY;
                        const isScrollableY = (overflowY === 'scroll' || overflowY === 'auto');

                        // 检查内容是否溢出
                        const hasVerticalScroll = node.parentElement.scrollHeight > node.parentElement.clientHeight;

                        // 综合考虑
                        if (isScrollableY && hasVerticalScroll) {
                            return NodeFilter.FILTER_REJECT;
                        }

                        return NodeFilter.FILTER_ACCEPT;
                    }
                }
            );

            let currentNode = treeWalker.nextNode() as HTMLElement;
            while (currentNode) {
                let childTop = currentNode.getBoundingClientRect().top;
                let childBottom = currentNode.getBoundingClientRect().bottom;
                this.minTop = Math.min(this.minTop, childTop);
                if (childBottom > this.maxBottom) {
                    this.bottomNode = currentNode;
                    this.maxBottom = childBottom;
                }
                currentNode = treeWalker.nextNode() as HTMLElement;
            }
        })

        let oriHeight = this.maxBottom - this.minTop;
        let screenHeight = this.visualHeight;
        this.scale = (screenHeight * 0.7) / oriHeight;
        this.scale = Math.max(this.scale, this.minScaleFactor);
        Log.d(`PopWindow智能布局: calcScale = ${this.scale}, bottomNode: ${this.bottomNode.className}`);
    }

    /**
     * 缩放处理，具有以下规则约束：
     * 1、对弹窗根节点下，除mask节点之外的所有节点进行缩放（父节点缩放，子节点会继承），具体实施如下：
     *  1）如果根节点和mask是同一个节点，则缩放根节点的所有子节点；
     *  2）如果mask是根节点的子节点，且content是mask的兄弟节点，则缩放mask的所有兄弟节点
     *  3）如果content是mask的子节点，则缩放mask的所有子节点。
     * 2、如果是一个底部模态窗口，则用zoom缩放，否则用transfrom scale
     * 3、如果原窗口紧贴底部，则缩放中心为bottom，否则为center，这样呈现效果最佳。
     * 4、需要找出顶层节点作为缩放目标，因为content节点可能是个节点集，会出现兄弟节点分布在多层的情况，实际用户能操作的弹窗页面只有最顶层节点。
     */
    private resetByScale(): void {
        Log.d(`开始应用缩放，弹窗类型: ${PopupType[this.popupInfo.popup_type]}`, Tag.popupRelayout);
        // 如果mask和rootNode是同一个节点，则直接缩放rootNode的所有子节点。
        if (this.popupInfo.popup_type === PopupType.C) {
            Log.d('C型弹窗: 缩放根节点的顶层子节点', Tag.popupRelayout);
            let topNodes = this.getTopmostChildren(this.mComponent, this.popupInfo.popup_type);
            Log.d(`找到 ${topNodes.length} 个顶层节点`, Tag.popupRelayout);
            for (let child of topNodes) {
                const childStyle = child.children.length > 0 ? getComputedStyle(child.children[0]) : null;
                const childRect = child.getBoundingClientRect();
                const isFixedOrAbsolute = childStyle ? childStyle.position === 'fixed' || childStyle.position === 'absolute' : false;
                const isZeroSize = childRect.width === 0 || childRect.height === 0;
                this.scaleChildForTypeC(childStyle, isFixedOrAbsolute, isZeroSize, child, topNodes);
            }
        } else if (this.popupInfo.popup_type === PopupType.B) {
            Log.d('B型弹窗: 缩放Mask的兄弟节点', Tag.popupRelayout);
            this.equivalentMask = this.getEquivalentMask();
            Log.d(`等效Mask节点: ${this.equivalentMask.className}`, Tag.popupRelayout);
            // 如果mask和content是兄弟节点，则其他兄弟节点做缩放
            let topNodes = this.getTopmostChildren(this.equivalentMask.parentElement, this.popupInfo.popup_type);
            topNodes.filter(node => node !== this.equivalentMask);
            Log.d(`找到 ${topNodes.length} 个顶层节点`, Tag.popupRelayout);
            for (let child of topNodes) {
                if (this.equivalentMask.contains(child)) {
                    continue;
                }
                const childStyle = child.children.length > 0 ? getComputedStyle(child.children[0]) : null;
                const childRect = child.getBoundingClientRect();
                const isFixedOrAbsolute = childStyle ? childStyle.position === 'fixed' || childStyle.position === 'absolute' : false;
                const isZeroSize = childRect.width === 0 || childRect.height === 0;
                if (childStyle && isFixedOrAbsolute && isZeroSize) {
                    this.scaleGrandChildrenForTypeB(child, topNodes);
                }
                else {
                    this.scaleChildForTypeB(child, topNodes);
                }
            }
        } else if (this.popupInfo.popup_type === PopupType.A) {
            Log.d('A型弹窗: 缩放Mask的子节点', Tag.popupRelayout);
            // 如果mask是rootNode的子节点，content是mask的子节点，则对mask的所有子节点以及它的兄弟节点做缩放
            let topNodes = this.getTopmostChildren(this.popupInfo.mask_node, this.popupInfo.popup_type);
            Log.d(`找到 ${topNodes.length} 个顶层节点`, Tag.popupRelayout);
            for (let child of topNodes) {
                this.scaleByTransform(child as HTMLElement, this.scale, false, topNodes, false, []);
                Log.d(`A型弹窗缩放完成: ${child.className}`, Tag.popupRelayout);
            }
        }
        Log.d('缩放应用完成', Tag.popupRelayout);
    }

    private scaleChildForTypeB(child: HTMLElement, topNodes: HTMLElement[]): void {
        if (this.popupDecisionTreeType === PopupDecisionTreeType.Bottom) {
            Log.d(`B型底部弹窗缩放: ${child.className}`, Tag.popupRelayout);
            this.scaleByTransform(child, this.scale, false, topNodes, false, []);
        }
        else {
            Log.d(`B型居中弹窗缩放: ${child.className}, 有兄弟节点: ${topNodes.length > 1}`, Tag.popupRelayout);
            this.scaleByTransform(child, this.scale, topNodes.length > 1, topNodes, false, []);
        }
    }

    private scaleGrandChildrenForTypeB(child: HTMLElement, topNodes: HTMLElement[]): void {
        const grandChildren = this.getValidGrandChildren(Array.from(child.children) as HTMLElement[]);
        Log.d(`B型弹窗孙节点缩放: 父节点 ${child.className}, 有效孙节点数 ${grandChildren.length}`, Tag.popupRelayout);
        for (let i = 0; i < grandChildren.length; i++) {
            const grandchild = grandChildren[i];
            if (this.popupDecisionTreeType === PopupDecisionTreeType.Bottom) {
                this.scaleByTransform(grandchild, this.scale, false, topNodes, grandChildren.length > 1, grandChildren);
            }
            else {
                this.scaleByTransform(grandchild, this.scale, topNodes.length > 1, topNodes, grandChildren.length > 1, grandChildren);
            }
            Log.d(`孙节点缩放完成: ${grandchild.className}`, Tag.popupRelayout);
        }
    }

    private scaleChildForTypeC(childStyle: CSSStyleDeclaration, isFixedOrAbsolute: boolean,
        isZeroSize: boolean, child: HTMLElement, topNodes: HTMLElement[]): void {
        if (childStyle && isFixedOrAbsolute && isZeroSize) {
            Log.d(`C型弹窗孙节点缩放: ${child.className} (fixed/absolute且尺寸为0)`, Tag.popupRelayout);
            const grandChildren = this.getValidGrandChildren(Array.from(child.children) as HTMLElement[]);
            Log.d(`有效孙节点数: ${grandChildren.length}`, Tag.popupRelayout);
            for (let i = 0; i < grandChildren.length; i++) {
                const grandchild = grandChildren[i];
                this.scaleByTransform(grandchild, this.scale, topNodes.length > 1, topNodes, grandChildren.length > 1, grandChildren);
                Log.d(`C型孙节点缩放完成: ${grandchild.className}`, Tag.popupRelayout);
            }
        }
        else {
            Log.d(`C型弹窗直接缩放: ${child.className}`, Tag.popupRelayout);
            this.scaleByTransform(child, this.scale, topNodes.length > 1, topNodes, false, []);
        }
    }

    /**
    * 满足以下条件，mask向上追溯。
    * 1、mask和root不是一个节点
    * 2、mask的父节点也不是root
    * 3、mask没有兄弟节点
    * 4、type为B
    */
    getEquivalentMask(): HTMLElement {
        let oriMaskNode = this.popupInfo.mask_node;
        if (this.popupInfo.popup_type !== PopupType.B) {
            return oriMaskNode;
        }
        let equivalentMask = oriMaskNode;
        if (oriMaskNode !== this.mComponent && oriMaskNode.parentElement !== this.mComponent && !LayoutUtils.hasElementSiblings(oriMaskNode as HTMLElement)) {
            let maskParentNode = oriMaskNode.parentElement;
            while (maskParentNode !== this.mComponent && !LayoutUtils.hasElementSiblings(maskParentNode as HTMLElement)) {
                maskParentNode = maskParentNode.parentElement;
            }
            equivalentMask = maskParentNode;
        }
        return equivalentMask;
    }

    /**
     * 修复图片内容被截断的场景
     */
    private resetTruncateBkgImgNodes(): void {
        this.truncateBkgImgNodes.forEach((node: HTMLElement) => {
            if (node !== this.popupInfo.mask_node) {
                StyleSetter.setStyle(node, Constant.background_size, `contain`);
                this.needRestoreStyleNodes.add(node);
            }
        });
    }

    /**
     * 递归地将节点及其所有父节点的 overflow 属性设置为 visible。
     * @param {HTMLElement} node - 需要处理的节点。
     */
    private makeAllOverflowVisible(node: HTMLElement): void {
        while (node && node !== document.documentElement) {
            const style = window.getComputedStyle(node);
            if (style.overflow !== 'visible') {
                StyleSetter.setStyle(node, Constant.overflow, 'visible');
                this.needRestoreStyleNodes.add(node);
            }
            node = node.parentElement;
        }
    }

    /**
    * 如果截断节点包含关闭按钮，并且是滚动条，
    * 按照规则，应该让内容完全显示，方便点击关闭按钮
    * @param {HTMLElement[]} truncateNodes - 根节点下所有截断节点
    * @returns {void}
    */
    private handleScrollbar(truncateNodes: HTMLElement[]): void {
        truncateNodes.forEach(truncateNode => {
            const parentStyle = window.getComputedStyle(truncateNode.parentElement);
            const overflowY = parentStyle.overflowY;
            if (Utils.hasCloseButton(truncateNode) && (overflowY === 'auto' || overflowY === 'scroll')) {
                StyleSetter.setStyle(truncateNode.parentElement, Constant.overflow, 'visible');
                this.isCloseButtonTruncatedByScroll = true;
                this.makeAllOverflowVisible(truncateNode.parentElement);
                this.needRestoreStyleNodes.add(truncateNode.parentElement);
            }
        })
    }

    /**
     * 查找弹窗根节点下被截断的节点
     * 对于被截断节点，如果兄弟节点也是被截断则保留，子节点不予保留
     * @param {HTMLElement[]} allNodes - 根节点下的所有节点
     * @returns {HTMLElement[]} - 符合条件的节点数组
     */
    private findTruncateNodes(allNodes: HTMLElement[]): void {
        Log.d('开始查找被截断的节点', Tag.popupRelayout);
        // 1. 查找所有符合条件的节点
        const tmpTruncateNodes = allNodes.filter(node => {
            return LayoutUtils.isNodeTruncated(node, this.popupInfo);
        });
        Log.d(`初步筛选出 ${tmpTruncateNodes.length} 个被截断的节点`, Tag.popupRelayout);
        
        this.truncateBkgImgNodes = allNodes.filter(node => {
            return LayoutUtils.checkIfBackgroundImgTruncated(node);
        });
        Log.d(`找到 ${this.truncateBkgImgNodes.length} 个背景图被截断的节点`, Tag.popupRelayout);

        this.handleScrollbar(tmpTruncateNodes);

        // 2. 过滤被其他节点包含的节点
        this.truncateNodes = this.filterContainedNodes(tmpTruncateNodes);
        Log.d(`过滤后最终 ${this.truncateNodes.length} 个截断节点`, Tag.popupRelayout);
        this.truncateNodes.forEach(node => {
            Log.d(`  - ${node.className}`, Tag.popupRelayout);
        });
    }

    /**
     * 遍历节点树，收集所有节点
     * @param {Node} node - 当前节点
     * @param {Array} [result=[]] - 结果数组
     * @returns {Array} 所有节点的数组
     */
    private traverseTree(node: HTMLElement, result: HTMLElement[]): HTMLElement[] {
        result.push(node);
        for (let i = 0; i < node.childNodes.length; i++) {
            if (node.childNodes[i] instanceof HTMLElement) {
                this.traverseTree(node.childNodes[i] as HTMLElement, result);
            }
        }
        return result;
    }

    private async getLayoutConstraintReport(token: { cancelled: boolean; generation: number }): Promise<void> {
        if (this.needLayoutConstraintNodes.size === 0) {
            Log.d('无需布局约束验证，因为没有重新布局的节点', Tag.popupRelayout);
            return;
        }

        try {
            //  步骤1：在异步等待开始时才设置 WAITING_VALIDATION 状态
            if (!PopupStateManager.setState(this.popupInfo.root_node, PopupLayoutState.WAITING_VALIDATION, '开始等待验证')) {
                Log.d('无法设置等待验证状态，跳过验证', Tag.popupRelayout);
                return;
            }
            
            Log.d(`开始布局约束验证，共 ${this.needLayoutConstraintNodes.size} 个节点`, Tag.popupRelayout);
            
            //  步骤2：等待 26 帧（约 450ms）
            const frameCount = 26;
            await this.forceLayoutUpdate(frameCount);
            
            //  步骤3：检查取消令牌
            if (token.cancelled) {
                Log.d(`验证已取消 (令牌已失效, generation: ${token.generation})`, Tag.popupRelayout);
                return;
            }
            
            //  步骤4：检查状态是否仍然有效
            const currentState = PopupStateManager.getState(this.popupInfo.root_node);
            if (currentState !== PopupLayoutState.WAITING_VALIDATION) {
                Log.d(`验证已取消 - 状态已改变为 ${currentState}`, Tag.popupRelayout);
                return;
            }
            
            //  步骤5：设置为验证中状态
            if (!PopupStateManager.setState(this.popupInfo.root_node, PopupLayoutState.VALIDATING, '开始验证')) {
                Log.d('验证已取消 - 无法切换到验证状态', Tag.popupRelayout);
                return;
            }

            //  步骤6：执行验证
            this.layoutConstraintResult = LayoutConstraintMetricsDetector.detectLayoutConstraintMetrics(this.popupInfo, this.needLayoutConstraintNodes);
            Log.d(`布局约束验证完成，结果代码: ${this.layoutConstraintResult.resultCode}`, Tag.popupRelayout);
            
            //  步骤7：再次检查取消令牌
            if (token.cancelled) {
                Log.d('验证完成但令牌已失效，不更新状态', Tag.popupRelayout);
                return;
            }
            
            //  步骤8：更新最终状态
            if (this.layoutConstraintResult.resultCode === Constant.ERR_CODE_GAPS || this.layoutConstraintResult.resultCode === Constant.ERR_CODE_OVERFLOW) {
                Log.d(`检测到布局问题 (代码: ${this.layoutConstraintResult.resultCode})，恢复原始样式`, Tag.popupRelayout);
                this.restoreStyles();
                PopupStateManager.setState(this.popupInfo.root_node, PopupLayoutState.RESTORED, '验证失败，已恢复');
            } else {
                Log.d('布局约束验证通过', Tag.popupRelayout);
                PopupStateManager.setState(this.popupInfo.root_node, PopupLayoutState.COMPLETED, '验证通过');
            }
            
            this.needLayoutConstraintNodes.clear();
            // @ts-ignore
            window.layoutConstraintResult = this.layoutConstraintResult;
            Log.d('========== 弹窗重新布局流程完成 ==========', Tag.popupRelayout);
        } catch (error) {
            //  只有令牌未取消时才处理错误
            if (!token.cancelled) {
                Log.e('布局约束验证失败', Tag.popupRelayout, error as Error);
                PopupStateManager.setState(this.popupInfo.root_node, PopupLayoutState.FAILED, `验证异常: ${error}`);
                this.restoreStyles();
            }
        }
    }

    /**
     * 强制布局更新
     */
    private async forceLayoutUpdate(rafCount = 1): Promise<void> {
        // 返回一个在下一动画帧开始时解析的 Promise
        const nextFrame = (): Promise<number> => new Promise(resolve => requestAnimationFrame(resolve));

        if (rafCount <= 0) {
            return;
        }

        // 第一次更新比较特殊，只等待一帧
        await nextFrame();
        void document.body.offsetHeight;

        // 从第二次更新开始，每次都需要等待两帧
        for (let i = 1; i < rafCount; i++) {
            await nextFrame();
            await nextFrame();
            void document.body.offsetHeight;
        }
    }

    private filterContainedNodes(nodes: HTMLElement[]): HTMLElement[] {
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

    private calculateGroupCenter(elements: HTMLElement, brotherNodes: HTMLElement[]): { centerX: number, centerY: number } {
        if (!elements) {
            return { centerX: 0, centerY: 0 };
        }

        const parent = elements.parentNode;
        if (!parent) {
            return { centerX: 0, centerY: 0 };
        }

        // 过滤掉宽度和高度为0的子节点
        const validElements = brotherNodes.filter(el => {
            const rect = el.getBoundingClientRect();
            return rect.width > 0 && rect.height > 0;
        });

        // 获取所有元素的getBoundingClientRect
        const rects = validElements.map(el => el.getBoundingClientRect());

        // 计算包围所有元素的总矩形
        const minTop = Math.min(...rects.map(r => r.top));
        const maxBottom = Math.max(...rects.map(r => r.bottom));
        const minLeft = Math.min(...rects.map(r => r.left));
        const maxRight = Math.max(...rects.map(r => r.right));

        // 计算中心点
        const centerX = minLeft + (maxRight - minLeft) / 2;
        const centerY = minTop + (maxBottom - minTop) / 2;

        return { centerX, centerY };
    }

    /**
     * 缩放有两种场景：
     * 1、通过transform scale底部缩放
     * 2、通过transform scale中心缩放，其中中心缩放时，有兄弟节点和没有兄弟节点，偏移量计算方式不同。有兄弟节点时，兄弟节点之间的距离也要予以缩放。
     * @param element 
     * @param newScale 
     * @param hasBrother
     */
    private scaleByTransform(element: HTMLElement, newScale: number, hasBrother: boolean, brotherNodes: HTMLElement[],
        hasGrandChild: boolean, grandChildNodes: HTMLElement[]): void {
        Log.d(`应用Transform缩放: ${element?.className}, 缩放比例: ${newScale.toFixed(3)}, 有兄弟: ${hasBrother}, 有孙子: ${hasGrandChild}`, Tag.popupRelayout);
    
        // 1. 提炼前置检查逻辑，使用卫语句提前退出
        if (this._shouldSkipScaling(element)) {
            Log.d(`跳过缩放: ${element?.className}`, Tag.popupRelayout);
            return;
        }
    
        const style = window.getComputedStyle(element);
        const rect = LayoutUtils.getVisualBoundingRect(element, this.isCloseButtonTruncatedByScroll, this.popupDecisionTreeType);
    
        // 2. 记录进行了布局约束的节点，用于后续布局约束计算
        this.needLayoutConstraintNodes.add(element);
    
        // 3. 提炼核心计算逻辑
        let offsetY = this._calculateOffsetY(element, rect, newScale, { hasBrother, brotherNodes, hasGrandChild, grandChildNodes });
        Log.d(`计算偏移量: offsetY = ${offsetY.toFixed(2)}px, 元素: ${element.className}`, Tag.popupRelayout);
    
        // 4. 提炼所有样式修复和调整的逻辑
        this._applyStyleFixes(element, style);
        
        // 5. 提炼最终应用变换的逻辑
        this._applyTransform(element, style, offsetY, newScale);
        Log.d(`缩放应用完成: ${element.className}`, Tag.popupRelayout);
    }
        
    /**
     * 检查是否应跳过缩放操作。封装了所有前置条件判断。
     * @returns {boolean} 如果应该跳过，则返回 true。
     */
    private _shouldSkipScaling(element: HTMLElement): boolean {
        const style = window.getComputedStyle(element);
        const rect = LayoutUtils.getVisualBoundingRect(element, this.isCloseButtonTruncatedByScroll, this.popupDecisionTreeType);
    
        // 检查1: 根据 transform 的 translateY 判断元素是否远在屏幕外
        const matrixMatch = style.transform.match(/matrix\((.*?),(.*?),(.*?),(.*?),(.*?),(.*?)\)/);
        const translateY = matrixMatch ? parseFloat(matrixMatch[6]) : 0;
        if (Math.abs(translateY) > window.innerHeight && !this._isElementInViewport(rect)) {
            Log.d('scaleByTransform: elements that exceed the screen height and are not within the viewport, skip scaling');
            return true;
        }
    
        // 检查2: 针对底部弹窗，检查其滚动容器是否在屏幕外
        if (this.popupDecisionTreeType === PopupDecisionTreeType.Bottom && rect.scrollElement) {
            const scrollElementRect = rect.scrollElement.getBoundingClientRect();
            if (!this._isElementInViewport(scrollElementRect)) {
                Log.d('scaleByTransform: Scrolling elements are off-screen, skip scaling');
                return true;
            } else {
                // 此处保留了对滚动容器的样式设置，因为它属于前置处理的一部分
                const maxHeightVh = ((window.innerHeight - scrollElementRect.top) / window.innerHeight) * 100;
                Log.d(`设置滚动容器最大高度: ${maxHeightVh.toFixed(2)}vh`, Tag.popupRelayout);
                StyleSetter.setStyle(rect.scrollElement, Constant.max_height, `${maxHeightVh}vh`);
                this.needRestoreStyleNodes.add(rect.scrollElement);
            }
        }
    
        // 检测3：检查弹窗是否是选择器弹窗
        if (this.popupDecisionTreeType === PopupDecisionTreeType.Picker) {
            Log.d('scaleByTransform: the Popup is TimePicker, skip scaling');
            return true;
        }
        
        return false;
    }
    
    /**
     * 封装判断元素是否在视口内的逻辑
     */
    private _isElementInViewport(rect: BoundingRect): boolean {
        return rect.top < window.innerHeight && rect.left < window.innerWidth && rect.bottom > 0 && rect.right > 0;
    }
    
    
    /**
     * 核心计算函数：根据弹窗类型计算垂直方向的偏移量 (offsetY)
     */
    private _calculateOffsetY(element: HTMLElement, rect: VisualBoundingRect, newScale: number,
        groupInfo: { hasBrother: boolean, brotherNodes: HTMLElement[], hasGrandChild: boolean, grandChildNodes: HTMLElement[] }): number {
        let offsetY = 0;
        const visualCenterY = rect.top + rect.height / 2;
    
        switch (this.popupDecisionTreeType) {
            case PopupDecisionTreeType.Bottom:
                offsetY = window.innerHeight - (visualCenterY + rect.height / 2 * newScale);
                break;
            
            case PopupDecisionTreeType.Center:
            case PopupDecisionTreeType.Center_Button_Overlap:
                offsetY = this._calculateCenterOffsetY(element, visualCenterY, newScale, groupInfo);
                // 针对居中弹窗，调整吸顶吸底组件带来的偏移
                offsetY -= this.popupInfo.stickyBottom_height / 2;
                offsetY += this.popupInfo.stickyTop_height / 2;
                break;
        }
        
        // 应用子元素高度超过父元素的偏移量
        offsetY += newScale * rect.offsetY;
    
        return offsetY;
    }
    
    /**
     * 专门为居中弹窗计算 offsetY，处理有兄弟节点或孙子节点的复杂情况
     */
    private _calculateCenterOffsetY(element: HTMLElement, visualCenterY: number, newScale: number,
        groupInfo: { hasBrother: boolean, brotherNodes: HTMLElement[], hasGrandChild: boolean, grandChildNodes: HTMLElement[] }): number {
        let offsetY = 0;
    
        if (groupInfo.hasBrother) {
            const groupCenterY = this.calculateGroupCenter(element, groupInfo.brotherNodes).centerY;
            const groupTranslationY = window.innerHeight / 2 - groupCenterY;
            const elementRelativeShiftY = (visualCenterY - groupCenterY) * (newScale - 1);
            offsetY = groupTranslationY + elementRelativeShiftY;
            Log.d(`居中弹窗(有兄弟): 组中心Y=${groupCenterY.toFixed(0)}, 偏移=${offsetY.toFixed(2)}`, Tag.popupRelayout);
        } else {
            offsetY = (window.innerHeight / 2 - visualCenterY);
            Log.d(`居中弹窗(无兄弟): 偏移=${offsetY.toFixed(2)}`, Tag.popupRelayout);
        }
    
        if (groupInfo.hasGrandChild) {
            const groupCenterY = this.calculateGroupCenter(element, groupInfo.grandChildNodes).centerY;
            const elementRelativeShiftY = (groupCenterY - visualCenterY) * (newScale - 1);
            offsetY += elementRelativeShiftY;
            Log.d(`孙节点调整: 额外偏移=${elementRelativeShiftY.toFixed(2)}`, Tag.popupRelayout);
        }
        
        return offsetY;
    }
    
    
    /**
     * 应用所有非 transform 的样式修复和调整
     */
    private _applyStyleFixes(element: HTMLElement, style: CSSStyleDeclaration): void {
        // 确保 transform 生效
        if (style.display === 'inline') {
            StyleSetter.setStyle(element, Constant.display, 'block');
            this.needRestoreStyleNodes.add(element);
        }
    
        // 重置子节点的 bottom 样式
        for (let child of element.children) {
            if ((child as HTMLElement).style.bottom !== '' && (child as HTMLElement).style.bottom !== 'auto') {
                StyleSetter.setStyle(child as HTMLElement, Constant.bottom, 'unset');
                this.needRestoreStyleNodes.add(child as HTMLElement);
            }
        }
    
        // 确保子元素宽度不为0
        this.adjustChildWidths(element);
    }
    
    /**
     * 应用最终的 transform 和 transition 样式
     */
    private _applyTransform(element: HTMLElement, style: CSSStyleDeclaration, offsetY: number, newScale: number): void {
        const currentTransform = style.transform === Constant.none ? '' : style.transform;
        const offsetX = 0; // offsetX 在原逻辑中始终为0
    
        StyleSetter.setStyle(element, Constant.transform, `${currentTransform} translate(${offsetX}px, ${offsetY}px) scale(${newScale})`);
        StyleSetter.setStyle(element, Constant.transition, `all ${this.scaleAnimationDuration}ms ease-in`);
        this.needRestoreStyleNodes.add(element);
    }

    /**
     * 确保给定元素的所有子元素宽度不为0，如果子元素宽度为0，则将其设置为100%。
     * 
     * @param element
     * 
     * 该函数遍历指定元素的所有子元素，检查每个子元素的宽度。
     * 如果发现某个子元素的宽度为0，则将其宽度设置为100%，
     * 以确保子元素在父元素中可见。
     */
    private adjustChildWidths(element: HTMLElement): void {
        const elementStyle = window.getComputedStyle(element);

        // 检查父元素的宽度
        if (parseFloat(elementStyle.width) === 0) {
            // 如果父元素宽度为0，则直接返回
            return;
        }
        // 获取元素的所有子节点
        const children = element.children;
        // 遍历每一个子节点
        for (let i = 0; i < children.length; i++) {
            const child = children[i] as HTMLElement;;
            const childStyle = window.getComputedStyle(child);

            // 检查子节点的宽度
            if (parseFloat(childStyle.width) === 0) {
                // 如果宽度为0，则设置为100%
                StyleSetter.setStyle(child, Constant.width, Constant.num100percent);
                this.needRestoreStyleNodes.add(child);
            }
        }
    }

    // 恢复样式
    public restoreStyles(): void {
        Log.d(`开始恢复原始样式，共 ${this.needRestoreStyleNodes.size} 个节点`, Tag.popupRelayout);

        this.needRestoreStyleNodes.forEach((style, node) => {
            StyleCleaner.removeAllStyle(node);
        });
        StyleSetter.flushAllStyles();
        this.needRestoreStyleNodes.clear();
        Log.d('样式恢复完成', Tag.popupRelayout);
    }

    public fixFlexShrink(node: HTMLElement): void {
        // 查找所有flex容器
        const flexContainers = [];

        // 检查根节点
        if (window.getComputedStyle(this.mComponent).display.includes(Constant.flex)) {
            flexContainers.push(this.mComponent);
        }

        // 检查所有子节点
        this.mComponent.querySelectorAll('*').forEach(element => {
            if (window.getComputedStyle(element).display.includes(Constant.flex)) {
                flexContainers.push(element);
            }
        });

        // 设置所有子节点的flex-shrink: 0
        let totalProcessed = 0;

        flexContainers.forEach(container => {
            Array.from(container.children).forEach(child => {
                if (getComputedStyle(container).flexDirection === Constant.column) {
                    StyleSetter.setStyle(child as HTMLElement, Constant.flex_shrink, '0');
                    this.needRestoreStyleNodes.add(child as HTMLElement);
                }
                // 检查父容器的尺寸
                if (container.clientWidth > 0 && container.clientHeight > 0) {
                    StyleSetter.setStyle(child as HTMLElement, Constant.max_width, '100%');
                    this.needRestoreStyleNodes.add(child as HTMLElement);
                }
                if (Utils.hasButton(child as HTMLElement)) {
                    // 写进默认生效样式
                    const selfStyle = window.getComputedStyle(child as HTMLElement);
                    const width = selfStyle.width;
                    StyleSetter.setStyle(child as HTMLElement, Constant.width, width);
                    this.needRestoreStyleNodes.add(child as HTMLElement);
                }
                totalProcessed++;
            });
        });

        Log.d(`fix ${flexContainers.length} flex contaner, ${totalProcessed} child elements`);
    }

    /**
     * 修复closebutton和弹窗主体重叠的场景。
     * 判定规则：
     * 1、找到calss包含close，role为button的按钮
     * 2、判断是不是一个底部close按钮。
     * 2、判断它和其他弹窗主体的关系是否为兄弟节点，且只有同层兄弟节点
     * 3、判断它和其他兄弟节点是否有重叠
     * 
     * 修复规则：
     * 1、y轴进行平移，平移量需要超出父布局。即：
     */
    private fixButtonOverlap(): void {
        Log.d('开始修复关闭按钮重叠问题', Tag.popupRelayout);
        // 查找所有可能的关闭按钮
        const closeElements = this.mComponent.querySelectorAll('[class*="close"]');
        if (closeElements.length !== 1) {
            Log.d(`关闭按钮数量不为1 (${closeElements.length})，跳过修复`, Tag.popupRelayout);
            return;
        }
        const closeButton = closeElements[0] as HTMLElement;
        Log.d(`找到关闭按钮: ${closeButton.className}`, Tag.popupRelayout);
        
        const buttonStyle = getComputedStyle(closeButton);
        // 刷新needLayoutConstraintNodes
        this.needLayoutConstraintNodes.add(closeButton);
        const currentTransform = buttonStyle.transform === Constant.none ? '' : buttonStyle.transform;
        let translateY: number = 0;
        const buttonTop = closeButton.getBoundingClientRect().top;
        const bottomNodeStyle = getComputedStyle(this.bottomNode);
        const bottomNodeBottom = this.bottomNode.getBoundingClientRect().bottom;
        if (closeButton === this.bottomNode) {
            translateY = parseFloat(bottomNodeStyle.height);
            Log.d(`关闭按钮是底部节点，平移高度: ${translateY.toFixed(2)}px`, Tag.popupRelayout);
        } else {
            translateY = bottomNodeBottom - buttonTop;
            Log.d(`计算平移距离: ${translateY.toFixed(2)}px (bottom: ${bottomNodeBottom.toFixed(0)}, top: ${buttonTop.toFixed(0)})`, Tag.popupRelayout);
        }
        translateY /= this.scale;

        let newStyle = `${currentTransform} translate(0px, ${translateY}px)`;
        StyleSetter.setStyle(closeButton, Constant.transform, newStyle);
        StyleSetter.setStyle(closeButton, Constant.transition, `all ${this.scaleAnimationDuration}ms ease-in`);
        this.needRestoreStyleNodes.add(closeButton);
        Log.d(`关闭按钮重叠修复完成，最终平移: ${translateY.toFixed(2)}px`, Tag.popupRelayout);
    }
}