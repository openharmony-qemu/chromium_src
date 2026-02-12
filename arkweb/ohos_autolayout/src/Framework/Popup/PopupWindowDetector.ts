/*实现一个识别网页上面弹窗组件的功能，弹窗组件有2个特征:
    1)mask特征：背景mask是与视口viewport尺寸一致，屏占比100%，且完全显示在屏幕上，
    2)关闭按钮：第二个特征是它的Content部分，一般都会有一个关闭按钮,它的class或者backgroudimage的url链接中，包含close/guanbi/del/dele等字符串
    弹窗结构一般分为3种：
    A型，content是mask的子节点:组件根节点->mask节点->content
    B型，mask与content同为兄弟节点：组件根节点->mask节点；组件根节点->content
    C型，mask与组件根节点是同节点：组件根节点(自身是mask)->content
    注意：在弹窗判断上，要先判断B型，在判断A型/C型

    所以寻找的核心思路是：
    1、找到所有与viewport尺寸一致，且在视口中正在显示的mask节点
    2、查看mask是否有子节点，子节点的尺寸是否满足屏占比>40%，如果存在屏占比40%的子节点，那么这个弹窗是A型或者C型，如果不存在屏占比40%的子节点，那么这个弹窗可能是B型
    3、针对A型和C型，查看Mask节点的positon是否是fixed/absolute：
    a. 如果不是，那么往上寻找节点，寻找fixed节点作为根节点，此弹窗属于A型，
    b. 如果是，则该节点就是弹窗组件根节点,此弹窗属于C型
    4、对于可能是B型弹窗的情况，那么需要对mask节点的兄弟节点进行搜索，如果兄弟节点的尺寸满足屏占比>40%，那么mask节点的父节点就是弹窗的根节点
*/


import Constant from '../Common/Constant';
import Log from '../../Debug/Log';
import Tag from '../../Debug/Tag';
import LayoutUtils from '../Utils/LayoutUtils';
import { PopupRecog } from './PopupRecog';
import { PopupType } from './PopupType';
import { PopupInfo, PotentialElements } from './PopupInfo';
import Utils from '../Utils/Utils';
import { CCMConfig } from '../Common/CCMConfig';

type StickyElements = {
    potentialStickyTop: Element | null;
    potentialStickyBottom: Element | null;
};

export class PopupWindowDetector {

    /**
     * 从根元素开始，通过广度优先搜索（BFS）查找第一个可见的、全宽的后代元素。
     * 这个函数旨在找到最能代表一个（可能是透明的）容器视觉边界的“实体”元素。
     *
     * @param {HTMLElement} rootEl - 搜索的起始元素（即吸底组件的根元素）。
     * @returns {boolean} - 如果找到符合条件的后代，则返回true。如果没有找到，则返回false
     */
    private static isStickyComponentVisiable(rootEl:Element): boolean {
        // 广度优先搜索（BFS），从直接子元素开始。
        const queue = [rootEl , ...rootEl.children];
        while (queue.length > 0) {
            const currentEl = queue.shift();

            // 跳过非 HTMLElement 节点。
            if (!(currentEl instanceof HTMLElement)) {
                continue;
            }
            
            const style = getComputedStyle(currentEl);
            const elWidth = currentEl.offsetWidth;
            const elHeight = currentEl.offsetHeight;

            // --- 核心筛选条件 ---
            // 1. 检查可见性 (Is Visible?)
            // 元素必须有实际尺寸，且不能被 CSS 隐藏。
            if(style.display === 'none' || style.visibility === 'hidden' || parseFloat(style.opacity) === 0) {
                //具备传染性，不再检查子元素
                continue;
            }
            // 透明节点
            if(elWidth === 0 || elHeight === 0 || (Utils.isColorTransparent(style.backgroundColor) && !style.backgroundImage.includes('url'))) {
                queue.push(...currentEl.children);
                continue;
            }
            
            // 上面过滤完，标志元素可见
            const isFullWidth = Math.abs(elWidth - screen.availWidth) < 2;
            return isFullWidth;
        }
        return false;
    };


/**
     * 从根节点开始，获取所有潜在的Mask、吸顶和吸底元素。
     * @param {HTMLElement} root - 扫描的起始根节点。
     * @returns {PotentialElements} - 包含潜在元素的对​​象。
     */
    private static getPotentialElements(root: HTMLElement): PotentialElements {
        const isNode = (node: Node): node is HTMLElement => node.nodeType === Node.ELEMENT_NODE;
        const allNodes = Array.from(root.getElementsByTagName('*')).filter(isNode);

        let potentialMasks = new Array<Element>();
        let potentialStickyTop: Element | null = null;
        let potentialStickyBottom: Element | null = null;

        for (const el of allNodes) {
            const style = getComputedStyle(el);

            // 1. 使用卫语句进行初步筛选，提前跳出不合格的元素
            if (!this.isElementQualified(el, style)) {
                continue;
            }

            // 2. 检查并添加遮罩元素
            if (this.isPotentialMask(el, style)) {
                potentialMasks.push(el);
                continue; // 如果是遮罩，则不考虑其为吸顶/吸底
            }
            
            // 3. 检查并更新吸顶/吸底元素
            const stickyResult = this.checkStickyElement(el, style, potentialStickyTop, potentialStickyBottom);
            potentialStickyTop = stickyResult.potentialStickyTop;
            potentialStickyBottom = stickyResult.potentialStickyBottom;
        }

        return { potentialMasks, potentialStickyTop, potentialStickyBottom };
    }

    /**
     * 检查元素是否为合格的候选者（可见且宽度足够）。
     * @param el 元素
     * @param style 计算后的样式
     * @returns {boolean}
     */
    private static isElementQualified(el: Element, style: CSSStyleDeclaration): boolean {
        // 排除未渲染的元素
        const isVisible = (el as HTMLElement).offsetWidth > 0 && (el as HTMLElement).offsetHeight > 0 &&
                          style.display !== 'none' && style.visibility !== 'hidden' && parseFloat(style.opacity) > 0;
        
        // 特例：处理opacity为0但背景半透明的特殊弹窗（如蚂蚁宠物小程序）
        const isSpecialPopup = style.visibility !== 'hidden' && parseFloat(style.opacity) === 0 && Utils.isColorSemiTransparent(style.backgroundColor);
        
        if (!isVisible && !isSpecialPopup) {
            return false;
        }

        // 排除宽度与屏幕宽度差异过大的元素
        if (Math.abs((el as HTMLElement).offsetWidth - window.innerWidth) >= 2) {
            return false;
        }

        return true;
    }

    /**
     * 检查元素是否为潜在的遮罩（Mask）。
     * @param el 元素
     * @param style 计算后的样式
     * @returns {boolean}
     */
    private static isPotentialMask(el: Element, style: CSSStyleDeclaration): boolean {
        const screenAreaRatio = Utils.getScreenAreaRatio(el);
        const minMaskAreaRatio = CCMConfig.getInstance().getMinMaskAreaRatioThreshold();

        // 检查长宽比：真正的 mask 不应该是特别细长的
        // 正常的浏览页面可能很长（滚动页面），但真正的弹窗 mask 的长宽比应该接近屏幕比例
        const rect = el.getBoundingClientRect();
        const maskAspectRatio = rect.height / rect.width;
        
        if (maskAspectRatio > Constant.maxMaskAspectRatio) {
            Log.d(`跳过长宽比过大的元素: ${(el as HTMLElement).className}, 长宽比: ${maskAspectRatio.toFixed(2)} (宽=${rect.width.toFixed(0)}, 高=${rect.height.toFixed(0)})`, Tag.popupDetector);
            return false;
        }

        // Case 1: 屏占比足够大且背景半透明
        if (screenAreaRatio > minMaskAreaRatio && Utils.isBackgroundSemiTransparent(style)) {
            Log.d(`找到潜在Mask[Case1-半透明]: ${(el as HTMLElement).className}, 屏占比: ${screenAreaRatio.toFixed(2)}`, Tag.popupDetector);
            return true;
        }

        // Case 2: 特例 - DOM结构弹窗（fixed父+absolute子+关闭按钮）
        if (screenAreaRatio > minMaskAreaRatio && style.position === Constant.fixed && el.children.length === 1) {
            const child = el.children[0];
            if (getComputedStyle(child).position === Constant.absolute &&
                Utils.getScreenAreaRatio(child) > CCMConfig.getInstance().getMinContentAreaRatioThreshold() &&
                Utils.hasCloseButton(child)) {
                Log.d(`找到潜在Mask[Case2-特殊结构]: ${(el as HTMLElement).className}`, Tag.popupDetector);
                return true;
            }
        }
        
        // Case 3: 特例 - 使用box-shadow实现的遮罩
        if (style.position === 'fixed' && LayoutUtils.analyzeComputedBoxShadow(style.boxShadow)) {
            Log.d(`找到潜在Mask[Case3-BoxShadow]: ${(el as HTMLElement).className}`, Tag.popupDetector);
            return true;
        }

        return false;
    }

    /**
     * 检查元素是否为吸顶或吸底元素，并返回更新后的结果。
     * @param el 当前元素
     * @param style 计算后的样式
     * @param potentialStickyTop 已找到的吸顶元素
     * @param potentialStickyBottom 已找到的吸底元素
     * @returns {{potentialStickyTop: Element | null, potentialStickyBottom: Element | null}}
     */
    private static checkStickyElement(el: Element, style: CSSStyleDeclaration,
        potentialStickyTop: Element | null, potentialStickyBottom: Element | null): StickyElements {
        let updatedTop = potentialStickyTop;
        let updatedBottom = potentialStickyBottom;

        const screenAreaRatio = Utils.getScreenAreaRatio(el);
        const isStickyCandidate = style.position === 'fixed' &&
                                  parseInt(style.left) === 0 &&
                                  screenAreaRatio > CCMConfig.getInstance().getMinSARTofStickyComponent() &&
                                  screenAreaRatio < CCMConfig.getInstance().getMaxSARTofStickyComponent() &&
                                  PopupWindowDetector.isStickyComponentVisiable(el);

        if (!isStickyCandidate) {
            return { potentialStickyTop, potentialStickyBottom };
        }

        // 检查是否为吸顶元素
        if (parseInt(style.top) === 0) {
            Log.d(`找到吸顶元素${el.className}`);
            // 如果没有已知的吸顶，或者当前元素在更上层，则更新
            if (updatedTop === null || PopupWindowDetector.isFirstElementOnTop(el, updatedTop)) {
                updatedTop = el;
            }
        }

        // 检查是否为吸底元素
        if (parseInt(style.bottom) === 0) {
            Log.d(`找到吸底元素${el.className}`);
            // 如果没有已知的吸底，或者当前元素在更上层，则更新
            if (updatedBottom === null || PopupWindowDetector.isFirstElementOnTop(el, updatedBottom)) {
                updatedBottom = el;
            }
        }
        
        return { potentialStickyTop: updatedTop, potentialStickyBottom: updatedBottom };
    }

    /**
     * 判断在两个重叠的元素中，第一个元素视觉上更靠上。
     * 该方法通过计算两个元素的相交区域，并检查该区域中心点最顶层的元素来实现。
     *
     * @param {Element} elementA - 第一个元素。
     * @param {Element} elementB - 第二个元素。
     * @returns {boolean} 如果 elementA 在 elementB 之上，则返回 true；如果它们不重叠或 elementB 在上，则返回 false。
     */
    private static isFirstElementOnTop(elementA:Element, elementB:Element): boolean {
        if (!elementA || !elementB) {
            return false;
        }

        // 1. 获取两个元素的几何信息
        const rectA = elementA.getBoundingClientRect();
        const rectB = elementB.getBoundingClientRect();

        // 2. 计算两个矩形的相交区域
        const intersectLeft = Math.max(rectA.left, rectB.left);
        const intersectTop = Math.max(rectA.top, rectB.top);
        const intersectRight = Math.min(rectA.right, rectB.right);
        const intersectBottom = Math.min(rectA.bottom, rectB.bottom);

        // 3. 检查是否存在相交区域
        if (intersectRight <= intersectLeft || intersectBottom <= intersectTop) {
            // 如果没有相交，则它们之间没有层级关系，返回 false
            return false;
        }

        // 4. 计算相交区域的中心点坐标
        const centerX = intersectLeft + (intersectRight - intersectLeft) / 2;
        const centerY = intersectTop + (intersectBottom - intersectTop) / 2;

        // 5. 获取该中心点最顶层的元素
        const topElement = document.elementFromPoint(centerX, centerY);

        if (!topElement) {
            // 如果该点在视口外，则无法判断
            return false;
        }
        
        // 6. 判断最顶层的元素是否是 elementB 或者 elementB 的子孙
        // Node.contains() 方法可以检查一个节点是否是另一个节点的后代
        // 如果 topElement 是 B 或者 B 的子孙，说明 B 压在了相交区域的中心点上
        if (topElement === elementB || elementB.contains(topElement)) {
            return false; // B 在 A 之上
        }
        
        return true; // A 在 B 之上 (或另一个元素在两者之上)
    }


    /**
     * 查找页面上所有符合条件的弹窗组件
     * @param {HTMLElement} root - 根节点
     * @returns { PopupInfo | null }
     */
    static findPopups(root:HTMLElement): PopupInfo | null {
        Log.d('开始弹窗检测流程', Tag.popupDetector);
        // Slow Pass:从所有的节点中查找
        // 获取潜在的Mask节点和吸顶吸底元素
        const potentialElements = PopupWindowDetector.getPotentialElements(root);
        Log.d(`找到 ${potentialElements.potentialMasks.length} 个潜在Mask节点`, Tag.popupDetector);
        if (potentialElements.potentialStickyTop) {
            Log.d(`找到吸顶元素: ${(potentialElements.potentialStickyTop as HTMLElement).className}`, Tag.popupDetector);
        }
        if (potentialElements.potentialStickyBottom) {
            Log.d(`找到吸底元素: ${(potentialElements.potentialStickyBottom as HTMLElement).className}`, Tag.popupDetector);
        }
        const result = PopupWindowDetector.findPopupsInternal(potentialElements);
        Log.d(`弹窗检测完成，结果: ${result ? '找到弹窗' : '未找到弹窗'}`, Tag.popupDetector);
        return result;
    }

/**
     * 从弹窗信息数组中找出最上层的一个。
     * @param popups - 一个包含多个 PopupInfo 对象的数组。
     * @returns 返回 z-index 之和最大的那个 PopupInfo 对象，如果数组为空则返回 null。
     */
    static findTopMostPopup(popups: PopupInfo[]): PopupInfo | null {
        if (!popups || popups.length === 0) {
            return null;
        }
        return popups.reduce((topmost, current) => {
            // zIndex需要根据层叠上下文计算
            return LayoutUtils.compareZIndex(topmost.root_node, current.root_node) >= 0 ? topmost : current;
        });
    }

    /**
     * 在给定的兄弟节点及其所有子节点中，寻找最佳候选弹窗内容。
     * @param sibling - 要搜索的兄弟节点（作为根节点）。
     * @param maskNode - 蒙版节点，用于位置和 z-index 比较。
     * @param maskZIndex - 已经计算好的蒙版 z-index。
     * @returns 返回一个包含最佳候选者及其屏幕面积比的对象。
     */
    private static findBestCandidateInSibling(
        sibling: Element,
        maskNode: Element,
        maskZIndex: number
    ): { candidate: Element | null; ratio: number } {
        Log.d(`========== 在兄弟节点中查找最佳候选 ==========`, Tag.popupDetector);
        Log.d(`兄弟节点: ${(sibling as HTMLElement).className || sibling.tagName}`, Tag.popupDetector);
        Log.d(`Mask节点: ${(maskNode as HTMLElement).className || maskNode.tagName}, maskZIndex=${maskZIndex}`, Tag.popupDetector);
        
        let bestCandidate: Element | null = null;
        let maxRatio = 0;
    
        // 1. 获取所有候选节点（包括兄弟节点自身及其所有后代）
        const candidates = [sibling, ...Array.from(sibling.querySelectorAll('*'))];
        Log.d(`📋 候选节点总数: ${candidates.length} (包括兄弟节点自身)`, Tag.popupDetector);
    
        // 2. 根据兄弟节点是位于蒙版节点之前还是之后，确定z-index的比较偏移量
        //    这解决了长兄/弟弟节点与蒙版z-index相等时的堆叠上下文问题。
        const isPreviousSibling = PopupWindowDetector.isPreviousElementSibling(maskNode, sibling);
        const maskZIndexOffset = isPreviousSibling ? 1 : 0;
        Log.d(`${isPreviousSibling ? '⬆️ 长兄节点' : '⬇️ 弟弟节点'}, z-index偏移量: ${maskZIndexOffset}`, Tag.popupDetector);
        Log.d(`z-index阈值: ${maskZIndex + maskZIndexOffset}`, Tag.popupDetector);
    
        // 3. 遍历所有候选节点，找出最优解
        let qualifiedCount = 0;
        let filteredCount = 0;
        
        for (const node of candidates) {
            // 使用层叠上下文比较，避免子元素z-index为auto(默认0)时被误判。
            // 例：.helpBg 高度为0，但子节点 <img> 具有实际尺寸且默认 z-index 为 auto。
            // 通过 compareZIndex 能正确识别 <img> 相对蒙版处于上方，即使计算出来的数值 z-index 为0。
            // 同时根据兄弟顺序决定是否允许与蒙版处于同一层级。
            const compareResult = LayoutUtils.compareZIndex(node as HTMLElement, maskNode as HTMLElement);
            const isAboveMask = isPreviousSibling ? compareResult > 0 : compareResult >= 0;
            const nodeZIndex = Utils.zIndexToNumber(window.getComputedStyle(node).zIndex);

            // 核心判断：候选节点必须在蒙版之上（长兄需要严格大于，弟弟允许同级）。
            if (isAboveMask) {
                qualifiedCount++;
                const ratio = Utils.getScreenAreaRatio(node);

                if (ratio > maxRatio) {
                    const previousBest = bestCandidate ? `${(bestCandidate as HTMLElement).className || bestCandidate.tagName}(${maxRatio.toFixed(2)})` : '无';
                    maxRatio = ratio;
                    bestCandidate = node;
                    Log.d(`🔄 更新最佳候选: ${(node as HTMLElement).className || node.tagName}, 屏占比=${ratio.toFixed(2)}, z-index=${nodeZIndex}, 原候选=${previousBest}`, Tag.popupDetector);
                }
            } else {
                filteredCount++;
            }
        }
        
        Log.d(`遍历完成: 合格节点=${qualifiedCount}, 过滤节点=${filteredCount}`, Tag.popupDetector);
        
        if (bestCandidate) {
            Log.d(`✅ 找到最佳候选: ${(bestCandidate as HTMLElement).className || bestCandidate.tagName}, 最大屏占比=${maxRatio.toFixed(2)}`, Tag.popupDetector);
        } else {
            Log.d(`❌ 未找到符合条件的候选节点`, Tag.popupDetector);
        }
    
        return { candidate: bestCandidate, ratio: maxRatio };
    }
    
    // 1. 寻找最佳兄弟内容候选者(B型弹窗)
    // 2、如果mask的children节点和兄弟节点的数量都为0，需要继续往上寻找父节点
    // 3、往上找到最多3个节点（规则约束）
    // 4、最先找到一个节点，除mask节点之外还有其他它有子节点，且包含close按钮，则这个节点是根节点
    // 5、如果找到的兄弟节点是mask的长兄节点，则需要判断它的zindex，保证在堆叠关系中，兄弟节点在mask节点的上面
    private static findBestSiblingContent(maskNode: Element): [Element | null, Element | null] {
        Log.d(`开始查找兄弟节点内容(B型弹窗): ${(maskNode as HTMLElement).className}`, Tag.popupDetector);
        let bestCandidate: Element | null = null;
        let maxRatio = 0;
    
        const maskZIndex = Utils.zIndexToNumber(window.getComputedStyle(maskNode).zIndex);
        const [siblings, root] = Utils.findSiblingContent(maskNode);
        Log.d(`找到 ${siblings.length} 个兄弟候选节点`, Tag.popupDetector);
    
        // 遍历所有可能的兄弟容器
        for (const sibling of siblings) {
            // 在每个兄弟容器中寻找最佳候选节点
            const result = this.findBestCandidateInSibling(sibling, maskNode, maskZIndex);
    
            // 如果在当前兄弟容器中找到了更好的候选者，则更新全局最优解
            if (result.ratio > maxRatio) {
                maxRatio = result.ratio;
                bestCandidate = result.candidate;
                Log.d(`更新最佳候选节点，屏占比: ${maxRatio.toFixed(2)}`, Tag.popupDetector);
            }
        }
    
        // 最后，根据配置的阈值决定是否返回找到的最佳候选者
        const minThreshold = CCMConfig.getInstance().getMinContentAreaRatioThreshold();
        const found = maxRatio >= minThreshold;
        Log.d(`兄弟节点查找完成: ${found ? '找到' : '未找到'}内容节点`, Tag.popupDetector);
        return found ? [bestCandidate, root] : [null, null];
    }

    // 2. 寻找最佳后代内容候选者(A型/C型弹窗)
    // 没有mask的特例：C型：fixed mask屏占比100%，absolute子节点Content，包含close按钮
    private static findBestDescendantContent = (maskNode:Element): Element | null => {
        Log.d(`开始查找后代内容节点(A/C型弹窗): ${(maskNode as HTMLElement).className}`, Tag.popupDetector);
        let bestCandidate: Element | null = null;
        let maxRatio = 0;
        const descendants = Array.from(maskNode.querySelectorAll('*'));
        Log.d(`扫描 ${descendants.length} 个后代节点`, Tag.popupDetector);
        
        for (const node of descendants) {
            // 过滤掉不可见节点
            if(Utils.visualFilter(node) === false) {
                continue;
            }
            // 对于后代节点，它们天然在父节点（遮罩）之上，无需比较z-index
            const ratio = Utils.getScreenAreaRatio(node);
            if (ratio > maxRatio) {
                maxRatio = ratio;
                bestCandidate = node;
            }
        }
        const minThreshold = CCMConfig.getInstance().getMinContentAreaRatioThreshold();
        const found = maxRatio >= minThreshold;
        Log.d(`后代节点查找完成: ${found ? '找到' : '未找到'}内容节点，最大屏占比: ${maxRatio.toFixed(2)}`, Tag.popupDetector);
        return found ? bestCandidate : null;
    }
    
    /**
     * 查找页面上所有符合条件的弹窗组件 (主函数)
     * @param {PotentialElements} potentialElements - 包含潜在遮罩和粘性元素的集合
     * @returns {PopupInfo | null} 返回最顶层的有效弹窗信息，否则返回 null
     */
    private static findPopupsInternal(potentialElements: PotentialElements): PopupInfo | null {
        const { potentialMasks, potentialStickyTop, potentialStickyBottom } = potentialElements;
        if (!potentialMasks || potentialMasks.length === 0) {
            Log.d('未找到潜在Mask节点，退出检测', Tag.popupDetector);
            return null;
        }
        potentialMasks.sort((a, b) => Utils.getElementDepth(b) - Utils.getElementDepth(a));
        Log.d(`按DOM深度排序Mask节点，最深: ${Utils.getElementDepth(potentialMasks[0])}`, Tag.popupDetector);

        const allDetectedPopups = potentialMasks
            .map(maskNode => this.identifyPopupFromMask(maskNode))
            .filter((p): p is { info: PopupInfo; node: Element } => p !== null);

        if (allDetectedPopups.length === 0) {
            Log.d('所有Mask节点均未能识别为弹窗', Tag.popupDetector);
            return null;
        }
        Log.d(`识别出 ${allDetectedPopups.length} 个候选弹窗`, Tag.popupDetector);
        
        const nonNestedPopups = this.filterNestedPopups(allDetectedPopups);
        const finalPopups = this.filterByPrediction(nonNestedPopups);
        const topMostPopup = PopupWindowDetector.findTopMostPopup(finalPopups);
        
        if (!topMostPopup) {
            Log.d('经过过滤后未找到有效弹窗', Tag.popupDetector);
            return null;
        }
        Log.d(`最终选定最顶层弹窗: ${topMostPopup.root_node.className}, 类型: ${topMostPopup.popup_type}`, Tag.popupDetector);
        this.updateStickyHeights(topMostPopup, { potentialStickyTop, potentialStickyBottom });
        return topMostPopup;
    }

    /**
     * [辅助函数] 从单个遮罩节点识别并构建弹窗信息对象
     */
    private static identifyPopupFromMask(maskNode: Element): { info: PopupInfo; node: Element } | null {
        Log.d(`开始识别弹窗结构: ${(maskNode as HTMLElement).className}`, Tag.popupDetector);
        const popupDetails = this.determinePopupStructure(maskNode);
        if (!popupDetails) {
            Log.d(`未能确定弹窗结构`, Tag.popupDetector);
            return null;
        }
        const { rootNode, contentNode, popupType } = popupDetails;
        Log.d(`确定弹窗类型: ${popupType}, 根节点: ${(rootNode as HTMLElement).className}`, Tag.popupDetector);
        return { 
            info: this.createPopupInfo(rootNode, maskNode, contentNode, popupType), 
            node: rootNode 
        };
    }
    
    /**
     * 确定弹窗的结构（根、内容、类型）
     */
    private static determinePopupStructure(maskNode: Element): { rootNode: Element; contentNode: Element; popupType: PopupType } | null {
        const [bestSiblingContent, root] = PopupWindowDetector.findBestSiblingContent(maskNode);

        if (bestSiblingContent && root) {
            // 特例处理：如果最终确定的root节点是body，则不是B型弹窗，直接返回null
            if (root === document.body) {
                Log.d(`B型弹窗特例：root节点为body，不是有效的弹窗结构，返回null`, Tag.popupDetector);
                return null;
            }

            // 严格使用 `maskNode.parentNode` 进行可见性检查
            const parentNode = maskNode.parentNode;
            if (!parentNode || !(parentNode instanceof Element)) {
                return null;
            }
            const style = getComputedStyle(parentNode);
            const isVisible = !(style.display === 'none' || style.visibility === 'hidden' || parseFloat(style.opacity) === 0);
            if (isVisible) {
                return { contentNode: bestSiblingContent, rootNode: root, popupType: PopupType.B };
            } else {
                return null;
            }
        } else {
            return this.findDescendantBasedPopup(maskNode);
        }
    }

    /**
     * [辅助函数] 尝试查找 A 或 C 型弹窗
     */
    private static findDescendantBasedPopup(maskNode: Element): { rootNode: Element; contentNode: Element; popupType: PopupType.A | PopupType.C } | null {
        const bestDescendantContent = PopupWindowDetector.findBestDescendantContent(maskNode);
        if (!bestDescendantContent || !Utils.visualFilter(maskNode)) {
            Log.d(`未找到合适的后代内容节点或Mask不可见`, Tag.popupDetector);
            return null;
        }
        const maskPosition = window.getComputedStyle(maskNode).position;
        Log.d(`Mask定位方式: ${maskPosition}`, Tag.popupDetector);

        if (maskPosition === 'fixed' || maskPosition === 'absolute') {
            // 特例处理：如果最终确定的root节点是body，则不是C型弹窗，直接返回null
            // 注意：C型弹窗的mask节点本身就是其根节点
            if (maskNode && maskNode === document.body) {
                Log.d(`C型弹窗特例：root节点为body，不是有效的弹窗结构，返回null`, Tag.popupDetector);
                return null;
            }
            Log.d(`识别为C型弹窗`, Tag.popupDetector);
            return { contentNode: bestDescendantContent, rootNode: maskNode, popupType: PopupType.C };
        } else {
            // 此处逻辑与原始代码的 while + if(!rootNode) fallback 等价
            const rootNode = this.findPositionedAncestor(maskNode) || maskNode.parentElement;
            Log.d(`最终确定的root节点: ${rootNode ? (rootNode as HTMLElement).className || rootNode.tagName : '未找到'}`, Tag.popupDetector);
            
            // 特例处理：如果最终确定的root节点是body，则不是A型弹窗，直接返回null
            if (rootNode && rootNode === document.body) {
                Log.d(`A型弹窗特例：root节点为body，不是有效的弹窗结构，返回null`, Tag.popupDetector);
                return null;
            }

            if (rootNode) {
                Log.d(`识别为A型弹窗，找到定位祖先节点: ${(rootNode as HTMLElement).className}`, Tag.popupDetector);
                return { contentNode: bestDescendantContent, rootNode: rootNode, popupType: PopupType.A };
            }
        }
        return null;
    }

    /**
     * [辅助函数] 向上遍历 DOM 树查找定位的祖先
     */
    private static findPositionedAncestor(element: Element): Element | null {
        let parent = element.parentElement;
        while (parent && parent !== document.body) {
            const position = window.getComputedStyle(parent).position;
            if (position === 'fixed' || position === 'absolute') {
                return parent;
            }
            parent = parent.parentElement;
        }
        return null;
    }

    /**
     * [辅助函数] 过滤掉作为其他弹窗容器的父弹窗
     */
    private static filterNestedPopups(popups: { info: PopupInfo; node: Element }[]): PopupInfo[] {
        Log.d(`开始过滤嵌套弹窗，候选数量: ${popups.length}`, Tag.popupDetector);
        const parentPopups = new Set<PopupInfo>();
        for (const p of popups) {
            for (const q of popups) {
                if (p === q) {
                    continue;
                }
                // **修正 #2**: 严格复现原始的 if/else 互斥逻辑
                let isParent = false;
                if (p.node !== q.node) {
                    isParent = p.node.contains(q.node);
                } else { // 只有在 p.node === q.node 时，才比较 mask
                    const pMask = p.info.mask_node;
                    const qMask = q.info.mask_node;
                    isParent = (pMask !== qMask) && pMask.contains(qMask);
                }

                if (isParent) {
                    parentPopups.add(p.info);
                    Log.d(`发现嵌套关系，标记父弹窗: ${(p.node as HTMLElement).className}`, Tag.popupDetector);
                    break; // p 已确定是父弹窗，跳出内层循环
                }
            }
        }
        const filtered = popups.map(p => p.info).filter(info => !parentPopups.has(info));
        Log.d(`过滤嵌套弹窗完成，剩余: ${filtered.length} 个`, Tag.popupDetector);
        return filtered;
    }

    /**
     * [辅助函数] 通过快速通道规则或模型预测来过滤弹窗
     */
    private static filterByPrediction(popups: PopupInfo[]): PopupInfo[] {
        Log.d(`开始模型预测过滤，候选数量: ${popups.length}`, Tag.popupDetector);
        const filtered = popups.filter(info => {
            const isFastPass = info.has_mask && 
                             info.mask_area_ratio > 95 && 
                             (info.mask_position === 'fixed' || info.mask_position === 'absolute');
            if (isFastPass) {
                Log.d(`快速通道验证通过: ${info.root_node.className}`, Tag.popupDetector);
                return true;
            }
            const prediction = PopupRecog.predictIsPopup(info);
            Log.d(`模型预测结果: ${prediction.prediction}, 弹窗: ${info.root_node.className}`, Tag.popupDetector);
            return prediction.prediction;
        });
        Log.d(`预测过滤完成，剩余: ${filtered.length} 个`, Tag.popupDetector);
        return filtered;
    }
    
    /**
     * [辅助函数] 更新弹窗的 sticky header/footer 高度
     */
    private static updateStickyHeights(popup: PopupInfo,
        stickyElements: { potentialStickyTop?: Element | null, potentialStickyBottom?: Element | null }): void {
        const { potentialStickyTop, potentialStickyBottom } = stickyElements;
        if (potentialStickyTop && LayoutUtils.compareZIndex(popup.mask_node, potentialStickyTop as HTMLElement) <= 0 &&
            !popup.root_node.contains(potentialStickyTop)) {
            popup.stickyTop_height = parseInt(getComputedStyle(potentialStickyTop).height, 10) || 0;
        }
        if (potentialStickyBottom && LayoutUtils.compareZIndex(popup.mask_node, potentialStickyBottom as HTMLElement) <= 0 &&
            !popup.root_node.contains(potentialStickyBottom)) {
            popup.stickyBottom_height = parseInt(getComputedStyle(potentialStickyBottom).height, 10) || 0;
        }
    }

    /**
     * [辅助函数] 创建 PopupInfo 对象
     */
    private static createPopupInfo(rootNode: Element, maskNode: Element, contentNode: Element, popupType: PopupType): PopupInfo {
        const rootStyle = window.getComputedStyle(rootNode);
        const maskStyle = window.getComputedStyle(maskNode);
        return {
            root_node: rootNode as HTMLElement,
            mask_node: maskNode as HTMLElement,
            content_node: contentNode as HTMLElement,
            popup_type: popupType,
            root_position: rootStyle.position,
            root_zindex: Utils.zIndexToNumber(rootStyle.zIndex),
            has_mask: true,
            root_screen_area_ratio: Utils.getScreenAreaRatio(rootNode),
            root_is_visiable: Utils.isElementVisibleInViewPort(rootNode as HTMLElement),
            has_close_button: Utils.hasCloseButton(contentNode || rootNode),
            mask_area_ratio: Utils.getScreenAreaRatio(maskNode),
            mask_position: maskStyle.position,
            mask_zindex: Utils.zIndexToNumber(maskStyle.zIndex),
            stickyTop_height: 0,
            stickyBottom_height: 0
        };
    }

    /**
     * 检查 potentialNodeBelow 是否在 currentNode 的长兄节点
     * @param {Element} currentNode - 当前参考节点
     * @param {Element} potentialNodeBelow - 需要检查的节点
     * @returns {boolean} - 如果 potentialNodeBelow 是长兄节点则返回 true，否则返回 false
     */
    private static isPreviousElementSibling(currentNode:Element, potentialNodeBelow:Element): boolean {
        // 确保两个节点存在且是兄弟关系
        if (!currentNode || !potentialNodeBelow || currentNode.parentNode !== potentialNodeBelow.parentNode) {
            return false;
        }

        let prevSibling = currentNode.previousElementSibling;

        // 循环向前遍历所有的长兄节点
        while (prevSibling) {
            // 如果找到了目标节点，说明它在当前节点的前面，即下方
            if (prevSibling === potentialNodeBelow) {
                return true;
            }
            // 继续向前找
            prevSibling = prevSibling.previousElementSibling;
        }

        // 遍历完所有长兄节点都没找到，说明它不在下方
        return false;
    }

    
    /**
     * 对外暴露的mask节点有效性检测，供生命周期管理使用。
     *
     * @param maskNode 候选的mask节点
     * @returns 当节点仍然满足mask特征并存在于文档中时返回true，否则返回false
     */
    static isMaskNodeActive(maskNode: HTMLElement | null): boolean {
        if (!maskNode) {
            return false;
        }

        // 节点已经从DOM中移除，直接视为失效
        if (!maskNode.isConnected) {
            Log.d('Mask节点已脱离文档结构', Tag.popupDetector);
            return false;
        }

        try {
            const style = window.getComputedStyle(maskNode);

            if (!this.isElementQualified(maskNode, style)) {
                Log.d(`Mask节点不再满足可见或尺寸要求: ${(maskNode as HTMLElement).className}`, Tag.popupDetector);
                return false;
            }

            if (!this.isPotentialMask(maskNode, style)) {
                Log.d(`Mask节点不再满足遮罩特征: ${(maskNode as HTMLElement).className}`, Tag.popupDetector);
                return false;
            }

            return true;
        } catch (error) {
            Log.d(`Mask节点检测异常: ${(maskNode as HTMLElement).className}`, Tag.popupDetector);
            return false;
        }
    }
}