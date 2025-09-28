import StyleSetter from "../../Common/Style/Setter/StyleSetter";
import Constant from "../Common/Constant";
import Utils from "../Common/Utils";
import LayoutUtils from "../Common/LayoutUtils";
import { AComponent } from "../Common/base/AComponent";
import { DetectorInst } from "../Common/DetectorInst";
import { LayoutConstraintMetricsDetector, LayoutConstraintMetrics } from "../Common/LayoutConstraintDetector";
import { PopupInfo } from "../Popup/PopupInfo";
import { PopupType } from "../Popup/PopupType";
import { PopupDecisionTreeType } from "../Popup/PopupDecisionTreeType";
import { PopupDecisionTree } from "../Popup/PopupDecisionTree";

/**
 * 弹窗
 * 输入为一个popupInfo
 */
export class PopWindow extends AComponent {
    private scale: number = 1.0;
    private minTop: number = Infinity;
    private maxBottom: number = -Infinity;
    private truncateNodes: HTMLElement[] = [];  // 被截断的节点
    private truncateBkgImgNodes: HTMLElement[] = [];    // 背景图被截断的节点（但是rect没有在视口内被截断）
    private popupInfo: PopupInfo;
    private scaleMinLimit: number = 0.5;
    private bottomNode: HTMLElement = null;
    private originalStyles = new Map<HTMLElement, any>();
    private relayoutTimes: number = 0;
    private needLayoutConstraintNodes = new Set<HTMLElement>();
    private layoutConstraintResult: LayoutConstraintMetrics = null;
    private visualHeight: number = window.innerHeight;
    private isCloseButtonTruncatedByScroll = false;
    private popupDecisionTreeType = PopupDecisionTreeType.Center;
    private equivalentMask: HTMLElement;

    resetStyle(): void {
        throw new Error("Method not implemented.");
    }

    constructor(popupInfo: PopupInfo) {
        super(popupInfo.root_node);
        this.popupInfo = popupInfo;
    }

    printTree(color: boolean = false): void { }

    static recognize(root: HTMLElement): boolean {
        return true;
    }

    public intelligenceLayout() {
        if (this.mComponent == null) {
            return;
        }

        this.visualHeight = window.innerHeight - this.popupInfo.stickyBottom_height - this.popupInfo.stickyTop_height;

        console.log(`intelligenceLayout for popWin: ${this.mComponent.className}}, window.innerWidth = ${window.innerWidth}, time = ${new Date()}`);
        this.relayout();
    }

    async relayout(): Promise<void> {
        const allNodes = this.traverseTree(this.mComponent, []);

        // 先将所有flex布局的子节点都设置flex-shrink:0，避免控件缩放导致的子节点高度压缩问题，也为了更好的计算高度。
        if (this.relayoutTimes == 0) {
            // 首次触发重布局，先解除所有flex压缩子节点的设定。其他逻辑下一次重布局执行
            this.fixFlexShrink(this.mComponent);
            StyleSetter.flushAllStyles();
            await this.forceLayoutUpdate(1);
        }
        this.relayoutTimes++;

        // step1: 计算被截断的节点
        this.findTruncateNodes(allNodes);

        if (this.truncateNodes.length == 0) {
            console.log(`no truncateNodes found.`);
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

        // step2: 判断弹窗决策树类型
        this.popupDecisionTreeType = PopupDecisionTree.judgePopupDecisionTreeType(this.mComponent, allNodes, this.popupInfo);

        // step3: 恢复背景图片被截断的节点
        if (this.truncateBkgImgNodes.length !== 0) {
            this.resetTruncateBkgImgNodes();
        }

        // step4: 计算缩放系数
        this.calScale();
        DetectorInst.getInstance().recordOriginalPosition(this.popupInfo.content_node);

        if (this.scale > 1) {
            console.log(`scale > 1, skip.`);
            return;
        }

        // step5: 应用缩放系数
        this.resetByScale();

        // step6: 修复按钮重合
        if (this.popupDecisionTreeType === PopupDecisionTreeType.Center_Button_Overlap) {
            this.fixButtonOverlap();
        }

        StyleSetter.flushAllStyles();
        // step7: 自验证算法
        this.getLayoutConstraintReport();
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
                    !Utils.isBackgroundSemiTransparent(style) &&
                    child.id != 'SmartSwitch' &&
                    child.id != 'SaveSwitch';
            });

        if (children.length === 0) return [];

        // 获取 mask 节点的 z-index
        const maskStyle = window.getComputedStyle(this.popupInfo.mask_node);
        let maskZIndex: string | number = maskStyle.zIndex;
        if (popup_type == PopupType.C || popup_type == PopupType.A) {
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

        console.log('getTopmostChildren: print topmostChildren size = ' + topmostChildren.length);
        return topmostChildren;
    }

    /**
    * 过滤 grandChildren，移除 translateY 超过屏幕高度的元素
    * @param {HTMLElement[]} grandChildren - 需要过滤的 grandChildren 数组
    * @returns {HTMLElement[]} 过滤后的 grandChildren 数组
    */
    private getValidGrandChildren(grandChildren: HTMLElement[]) {
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
     * step3：scale = (screenHeight * 0.7) / (maxBottom - minTop)
     * step4: 记录maxBottom对应的bottomNode
     */
    private calScale() {
        this.truncateNodes.forEach(truncateNode => {
            console.log('tangcd print truncateNode classname = ' + truncateNode ?.className);
            let rect = truncateNode.getBoundingClientRect();
            this.minTop = Math.min(this.minTop, rect.top);
            if (rect.bottom > this.maxBottom) {
                this.bottomNode = truncateNode;
                this.maxBottom = rect.bottom;
            }
            let tmpNode = truncateNode;
            if (truncateNode == this.mComponent) {
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
        this.scale = Math.max(this.scale, this.scaleMinLimit);
        console.log(`PopWindow智能布局: calcScale = ${this.scale}, bottomNode: ${this.bottomNode ?.className}`);
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
        // 如果mask和rootNode是同一个节点，则直接缩放rootNode的所有子节点。
        if (this.popupInfo.popup_type == PopupType.C) {
            let topNodes = this.getTopmostChildren(this.mComponent, this.popupInfo.popup_type);
            for (let child of topNodes) {
                const childStyle = child.children.length > 0 ? getComputedStyle(child.children[0]) : null;
                const childRect = child.getBoundingClientRect();
                const isFixedOrAbsolute = childStyle ? childStyle.position === 'fixed' || childStyle.position === 'absolute' : false;
                const isZeroSize = childRect.width === 0 || childRect.height === 0;
                if (childStyle && isFixedOrAbsolute && isZeroSize) {
                    const grandChildren = this.getValidGrandChildren(Array.from(child.children) as HTMLElement[]);
                    for (let i = 0; i < grandChildren.length; i++) {
                        const grandchild = grandChildren[i];
                        this.scaleByTransform(grandchild, this.scale, topNodes.length > 1, topNodes, grandChildren.length > 1, grandChildren);
                        console.log(`resetByScale for type C: ${grandchild.className}`);
                    }
                }
                else {
                    this.scaleByTransform(child, this.scale, topNodes.length > 1, topNodes, false, []);
                    console.log(`resetByScale for type C: ${child.className}`);
                }
            }
        } else if (this.popupInfo.popup_type == PopupType.B) {
            this.equivalentMask = this.getEquivalentMask();
            // 如果mask和content是兄弟节点，则其他兄弟节点做缩放
            let topNodes = this.getTopmostChildren(this.equivalentMask.parentElement, this.popupInfo.popup_type);
            topNodes.filter(node => node !== this.equivalentMask);
            for (let child of topNodes) {
                if (this.equivalentMask.contains(child)) {
                    continue;
                }
                const childStyle = child.children.length > 0 ? getComputedStyle(child.children[0]) : null;
                const childRect = child.getBoundingClientRect();
                const isFixedOrAbsolute = childStyle ? childStyle.position === 'fixed' || childStyle.position === 'absolute' : false;
                const isZeroSize = childRect.width === 0 || childRect.height === 0;
                if (childStyle && isFixedOrAbsolute && isZeroSize) {
                    const grandChildren = this.getValidGrandChildren(Array.from(child.children) as HTMLElement[]);
                    for (let i = 0; i < grandChildren.length; i++) {
                        const grandchild = grandChildren[i];
                        if (this.popupDecisionTreeType === PopupDecisionTreeType.Bottom) {
                            this.scaleByTransform(grandchild, this.scale, false, topNodes, grandChildren.length > 1, grandChildren);
                        }
                        else {
                            this.scaleByTransform(grandchild, this.scale, topNodes.length > 1, topNodes, grandChildren.length > 1, grandChildren);
                        }
                        console.log(`resetByScale for type B: ${grandchild.className}`);
                    }
                }
                else {
                    if (this.popupDecisionTreeType === PopupDecisionTreeType.Bottom) {
                        this.scaleByTransform(child, this.scale, false, topNodes, false, []);
                    }
                    else {
                        this.scaleByTransform(child, this.scale, topNodes.length > 1, topNodes, false, []);
                    }
                    console.log(`resetByScale for type B: ${child.className}`);
                }
            }
        } else if (this.popupInfo.popup_type == PopupType.A) {
            // 如果mask是rootNode的子节点，content是mask的子节点，则对mask的所有子节点以及它的兄弟节点做缩放
            let topNodes = this.getTopmostChildren(this.popupInfo.mask_node, this.popupInfo.popup_type);
            for (let child of topNodes) {
                this.scaleByTransform(child as HTMLElement, this.scale, false, topNodes, false, []);
                console.log(`resetByScale for type A: ${child.className}`);
            }
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
        if (this.popupInfo.popup_type != PopupType.B) {
            return oriMaskNode;
        }
        let equivalentMask = oriMaskNode;
        if (oriMaskNode != this.mComponent && oriMaskNode.parentElement != this.mComponent && !LayoutUtils.hasElementSiblings(oriMaskNode as HTMLElement)) {
            let maskParentNode = oriMaskNode.parentElement;
            while (maskParentNode != this.mComponent && !LayoutUtils.hasElementSiblings(maskParentNode as HTMLElement)) {
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
            if (node != this.popupInfo.mask_node) {
                this.saveOriginalStyles(node);
                StyleSetter.setStyle(node, Constant.background_size, `contain`);
            }
        });
    }


    /**
     * 递归地将节点及其所有父节点的 overflow 属性设置为 visible。
     * @param {HTMLElement} node - 需要处理的节点。
     */
    private makeAllOverflowVisible(node: HTMLElement) {
        while (node && node !== document.documentElement) {
            const style = window.getComputedStyle(node);
            if (style.overflow !== 'visible') {
                this.saveOriginalStyles(node);
                StyleSetter.setStyle(node, Constant.overflow, 'visible');
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
                this.saveOriginalStyles(truncateNode.parentElement);
                StyleSetter.setStyle(truncateNode.parentElement, 'overflow', 'visible');
                this.isCloseButtonTruncatedByScroll = true;
                this.makeAllOverflowVisible(truncateNode.parentElement);
            }
        })
    }

    /**
     * 查找弹窗根节点下被截断的节点
     * 对于被截断节点，如果兄弟节点也是被截断则保留，子节点不予保留
     * @param {HTMLElement[]} allNodes - 根节点下的所有节点
     * @returns {HTMLElement[]} - 符合条件的节点数组
     */
    private findTruncateNodes(allNodes: HTMLElement[]) {
        // 1. 查找所有符合条件的节点
        const tmpTruncateNodes = allNodes.filter(node => {
            return LayoutUtils.isNodeTruncated(node, this.popupInfo);
        });
        this.truncateBkgImgNodes = allNodes.filter(node => {
            return LayoutUtils.checkIfBackgroundImgTruncated(node);
        });

        this.handleScrollbar(tmpTruncateNodes);

        // 2. 过滤被其他节点包含的节点
        this.truncateNodes = this.filterContainedNodes(tmpTruncateNodes);
    }

    /**
     * 遍历节点树，收集所有节点
     * @param {Node} node - 当前节点
     * @param {Array} [result=[]] - 结果数组
     * @returns {Array} 所有节点的数组
     */
    private traverseTree(node: HTMLElement, result: HTMLElement[]) {
        result.push(node);
        for (let i = 0; i < node.childNodes.length; i++) {
            if (node.childNodes[i] instanceof HTMLElement) {
                this.traverseTree(node.childNodes[i] as HTMLElement, result);
            }
        }
        return result;
    }

    private async getLayoutConstraintReport(): Promise<void> {
        if (this.needLayoutConstraintNodes.size == 0) {
            console.log('no report needed, because there is no relayout nodes');
            return;
        }
        await this.forceLayoutUpdate(16);
        this.layoutConstraintResult = LayoutConstraintMetricsDetector.detectLayoutConstraintMetrics(this.popupInfo, this.needLayoutConstraintNodes);
        if (this.layoutConstraintResult.resultCode === Constant.ERR_CODE_GAPS || this.layoutConstraintResult.resultCode === Constant.ERR_CODE_OVERFLOW) {
            this.restoreStyles();
            console.log(`resultCode: ${this.layoutConstraintResult.resultCode}, need to restore`);
        }
        // @ts-ignore
        window.layoutConstraintResult = this.layoutConstraintResult;
    }

    /**
     * 强制布局更新
     */
    private async forceLayoutUpdate(rafCount = 1): Promise<void> {
        return new Promise<void>((resolve) => {
            const raf = () => {
                // 强制触发布局重排
                void document.body.offsetHeight;

                if (rafCount > 1) {
                    requestAnimationFrame(() => {
                        this.forceLayoutUpdate(rafCount - 1).then(resolve);
                    });
                } else {
                    resolve();
                }
            };

            requestAnimationFrame(raf);
        });
    }

    private filterContainedNodes(nodes: HTMLElement[]) {
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
    private scaleByTransform(element: HTMLElement, newScale: number, hasBrother: boolean, brotherNodes: HTMLElement[], hasGrandChild: boolean, grandChildNodes: HTMLElement[]): void {
        console.log(`scaleByTransform: ${element ?.className}, hasBrother = ${hasBrother}`);
        // 1. 获取当前视觉位置和变换状态
        const rect = LayoutUtils.getVisualBoundingRect(element, this.isCloseButtonTruncatedByScroll, this.popupDecisionTreeType);
        const style = window.getComputedStyle(element);
        const parentStyle = window.getComputedStyle(element.parentElement);
        const currentTransform = style.transform === 'none' ? '' : style.transform;
        let offsetX = 0;
        let offsetY = 0;

        // 解析 matrix 变换，提取 translateY 值
        const matrixMatch = style.transform.match(/matrix\((.*?),(.*?),(.*?),(.*?),(.*?),(.*?)\)/);
        const translateY = matrixMatch ? parseFloat(matrixMatch[6]) : 0;

        // 检查 translateY 是否超过屏幕高度 && 且元素不在视口区域内
        if (Math.abs(translateY) > window.innerHeight && (rect.top >= window.innerHeight || rect.left >= window.innerWidth || rect.bottom <= 0 || rect.right <= 0)) {
            console.log('scaleByTransform: translateY 超过屏幕高度，不进行缩放');
            return;
        }

        if (this.popupDecisionTreeType === PopupDecisionTreeType.Bottom && rect.scrollElement) {
            const scrollElementRect = rect.scrollElement.getBoundingClientRect();
            // 检查元素是否在视口内
            if (scrollElementRect.top >= window.innerHeight ||
                scrollElementRect.left >= window.innerWidth ||
                scrollElementRect.bottom <= 0 ||
                scrollElementRect.right <= 0) {
                console.log('scaleByTransform: 滚动元素在屏幕外，不进行缩放');
                return;
            }
            else {
                // 设置 max-height，解决一些滚动弹窗不可用滚动的问题
                const maxHeightVh = ((window.innerHeight - scrollElementRect.top) / window.innerHeight) * 100;
                this.saveOriginalStyles(rect.scrollElement);
                StyleSetter.setStyle(rect.scrollElement, Constant.max_height, `${maxHeightVh}vh`);
            }
        }

        this.saveOriginalStyles(element);
        this.needLayoutConstraintNodes.add(element);

        // 2. 计算元素中心点在视口中的绝对位置
        let visualCenterY = rect.top + rect.height / 2;
        if (this.popupDecisionTreeType == PopupDecisionTreeType.Bottom) {
            offsetY = window.innerHeight - (visualCenterY + rect.height / 2 * newScale);
        } else if (this.popupDecisionTreeType == PopupDecisionTreeType.Center ||
            this.popupDecisionTreeType == PopupDecisionTreeType.Center_Button_Overlap) {
            if (hasBrother) {
                // A. 计算“组”需要移动的距离：将“组中心”移动到“视口中心”
                // to do: 后续优化下相关逻辑，提升计算效果，看看是否和calScale进行合并计算
                // const groupCenterY = this.minTop + (this.maxBottom - this.minTop) / 2;
                const groupCenterY = this.calculateGroupCenter(element, brotherNodes).centerY;
                const groupTranslationY = window.innerHeight / 2 - groupCenterY;
                // B. 计算“当前元素”因缩放而相对于“组中心”产生的位移
                //    (visualCenterY - groupCenterY) 是当前元素中心相对于组中心的偏移量
                //    (newScale - 1) 是这个偏移量因缩放而发生改变的比例
                const elementRelativeShiftY = (visualCenterY - groupCenterY) * (newScale - 1);
                // C. 总的 offsetY 是这两部分的和
                offsetY = groupTranslationY + elementRelativeShiftY;
            } else {
                offsetY = (window.innerHeight / 2 - visualCenterY);
            }

            if (hasGrandChild) {
                const groupCenterY = this.calculateGroupCenter(element, grandChildNodes).centerY;
                const elementRelativeShiftY = (groupCenterY - visualCenterY) * (newScale - 1);
                offsetY += elementRelativeShiftY;
            }
            console.log(`scaleByTransform: print offsetY: ${offsetY}`);
        }

        // 3. 更新元素中心的偏移量（子元素高度超过父元素的情况）
        offsetY += newScale * rect.offsetY;

        // 4. 如果父布局设置了display: flex，则子节点需要设置flex-shrink: 0; 处理子节点高度被挤压的场景，仅靠scale无法恢复
        if (style.display == 'flex') {
            for (let child of element.children) {
                this.saveOriginalStyles(child as HTMLElement);
                StyleSetter.setStyle(child as HTMLElement, Constant.flex_shrink, '0');
            }
        }
        if (parentStyle.display == 'flex') {
            StyleSetter.setStyle(element, Constant.flex_shrink, '0');
        }

        // 5. 对于有吸顶吸底组件的弹窗，重新调整offsetY（只针对居中弹窗需要调整）
        if (this.popupDecisionTreeType == PopupDecisionTreeType.Center ||
            this.popupDecisionTreeType == PopupDecisionTreeType.Center_Button_Overlap) {
            offsetY -= this.popupInfo.stickyBottom_height / 2;
            offsetY += this.popupInfo.stickyTop_height / 2;
        }

        // 6. 应用新缩放
        StyleSetter.setStyle(element, Constant.transform, `${currentTransform}
            translate(${offsetX}px, ${offsetY}px)
            scale(${newScale})`);
        StyleSetter.setStyle(element, Constant.transition, 'all 0.1s ease-in');

        // 7. transform 属性对 display: inline的元素不起作用
        if (style.display === 'inline') {
            StyleSetter.setStyle(element, Constant.display, 'block');
        }

        // 8. 设置子节点的margin
        for (let i = 0; i < element.children.length; i++) {
            let child = element.children[i] as HTMLElement;
            if (child.style.bottom !== '' && child.style.bottom !== 'auto') {
                this.saveOriginalStyles(child);
                StyleSetter.setStyle(element, Constant.bottom, 'unset');
            }
        }

        // 9. 确保给定元素的所有子元素宽度不为0，如果子元素宽度为0，则将其设置为100%
        this.adjustChildWidths(element);
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
                this.saveOriginalStyles(child);
                StyleSetter.setStyle(child, Constant.width, '100%');
            }
        }
    }

    // 保存样式的方法
    private saveOriginalStyles(node: HTMLElement): void {
        // 只保存一次，避免覆盖
        if (!this.originalStyles.has(node)) {
            const computedStyle = window.getComputedStyle(node);
            this.originalStyles.set(node, {
                transform: computedStyle.transform,
                transition: computedStyle.transition,
                width: computedStyle.width,
                max_width: computedStyle.maxWidth,
                max_height: computedStyle.maxHeight,
                bottom: computedStyle.bottom,
                background_size: computedStyle.backgroundSize,
                overflow: computedStyle.overflow,
                flex_shrink: computedStyle.flexShrink,
                display: computedStyle.display,
            });
        }
    }

    // 恢复样式
    private restoreStyles(): void {
        console.log("恢复原始样式");

        this.originalStyles.forEach((style, node) => {
            StyleSetter.setStyle(node, Constant.transform, style.transform);
            StyleSetter.setStyle(node, Constant.transition, style.transition);
            StyleSetter.setStyle(node, Constant.width, style.width);
            StyleSetter.setStyle(node, Constant.max_width, style.max_width);
            StyleSetter.setStyle(node, Constant.max_height, style.max_height);
            StyleSetter.setStyle(node, Constant.bottom, style.bottom);
            StyleSetter.setStyle(node, Constant.background_size, style.backgroundSize);
            StyleSetter.setStyle(node, Constant.overflow, style.overflow);
            StyleSetter.setStyle(node, Constant.flex_shrink, style.flex_shrink);
            StyleSetter.setStyle(node, Constant.display, style.display);
        });
        StyleSetter.flushAllStyles();
        this.originalStyles.clear();
    }

    private fixFlexShrink(node: HTMLElement): void {
        // 查找所有flex容器
        const flexContainers = [];

        // 检查根节点
        if (window.getComputedStyle(this.mComponent).display.includes('flex')) {
            flexContainers.push(this.mComponent);
        }

        // 检查所有子节点
        this.mComponent.querySelectorAll('*').forEach(element => {
            if (window.getComputedStyle(element).display.includes('flex')) {
                flexContainers.push(element);
            }
        });

        // 设置所有子节点的flex-shrink: 0
        let totalProcessed = 0;

        flexContainers.forEach(container => {
            Array.from(container.children).forEach(child => {
                this.saveOriginalStyles(child as HTMLElement);
                StyleSetter.setStyle(child as HTMLElement, Constant.flex_shrink, '0');
                // 检查父容器的尺寸
                if (container.clientWidth > 0 && container.clientHeight > 0) {
                    StyleSetter.setStyle(child as HTMLElement, Constant.max_width, '100%');
                }
                if (Utils.hasButton(child)) {
                    // 写进默认生效样式
                    const selfStyle = window.getComputedStyle(child);
                    const width = selfStyle.width;
                    StyleSetter.setStyle(child as HTMLElement, 'width', width);
                }
                totalProcessed++;
            });
        });

        console.log(`fix ${flexContainers.length} flex contaner, ${totalProcessed} child elements`);
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
        // 查找所有可能的关闭按钮
        const closeElements = this.mComponent.querySelectorAll('[class*="close"]');
        if (closeElements.length != 1) {
            return;
        }
        console.log('ther is a close button to be fixed.');
        const closeButton = closeElements[0] as HTMLElement;
        const buttonStyle = getComputedStyle(closeButton);
        // 保存节点的原始样式并刷新needLayoutConstraintNodes
        this.saveOriginalStyles(closeButton);
        this.needLayoutConstraintNodes.add(closeButton);

        const currentTransform = buttonStyle.transform === 'none' ? '' : buttonStyle.transform;

        let translateY: number = 0;
        const buttonTop = closeButton.getBoundingClientRect().top;
        const bottomNodeStyle = getComputedStyle(this.bottomNode);
        const bottomNodeBottom = this.bottomNode.getBoundingClientRect().bottom;
        if (closeButton == this.bottomNode) {
            translateY = parseFloat(bottomNodeStyle.height);
        } else {
            translateY = bottomNodeBottom - buttonTop;
        }
        translateY /= this.scale;

        let newStyle = `${currentTransform} translate(0px, ${translateY}px)`;
        StyleSetter.setStyle(closeButton, Constant.transform, newStyle);
        StyleSetter.setStyle(closeButton, Constant.transition, 'all 0.1s ease-in');
    }
}
