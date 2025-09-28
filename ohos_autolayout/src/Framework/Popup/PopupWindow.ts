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


import Constant from "../Common/Constant";
import LayoutUtils from "../Common/LayoutUtils";
import { Level, PerfExecution } from "../Common/Perf";
import { PopupRecog } from "./PopupRecog";
import { PopupType } from "./PopupType";
import { PopupInfo, PotentialElements } from "./PopupInfo";
import Utils from "../Common/Utils";

export class PopupWindow {

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
            if(style.display === 'none' || style.visibility === 'hidden' || parseFloat(style.opacity) == 0) {
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
     * 从根节点开始，获取所有潜在的Mask元素。
     * 该函数合并了节点遍历和Mask节点筛选的逻辑。
     * @param {HTMLElement} root - 扫描的起始根节点。
     * @returns {Element[]} - 符合Mask节点条件的元素数组。
     */
    @PerfExecution(({ level: Level.INFO }))
    private static getPotentialElements(root: HTMLElement): PotentialElements {
        const isNode = (node: Node): node is HTMLElement => node.nodeType === Node.ELEMENT_NODE;
        const allNodes = Array.from(root.getElementsByTagName("*")).filter(isNode);
        const potentialMasks = new Array<Element>();
        let potentialStickyTop:Element = null;
        let potentialStickyBottom:Element = null;
        // 对所有节点进行筛选，找到潜在的Mask和吸顶吸底元素
        for(const el of allNodes) {
            const style = getComputedStyle(el);
            // 排除未渲染的元素，
            if ((el as HTMLElement).offsetWidth === 0 || (el as HTMLElement).offsetHeight === 0 ||
                style.display === 'none' || style.visibility === 'hidden' || parseFloat(style.opacity) === 0) {
                // 添加特例规则：蚂蚁宠物小程序，它既设置了opacity为0，设置了background透明度
                // 1、如果尺寸为0，则会被下面的屏占比条件过滤掉 2、hidden的visibility仍然会占据布局尺寸，所以opacity和visibility为hidden需要同时判断
                if( style.visibility !== 'hidden' && parseFloat(style.opacity) === 0 && Utils.isColorSemiTransparent(style.backgroundColor) ) {
                    console.log("special popup,just skip");
                }else{
                    continue;
                }
            }
            // 弹窗/吸顶/吸底元素都具备一个特征:宽度与屏幕的宽度保持一致,这样可以过滤绝大多数的节点
            if (Math.abs(el.offsetWidth - window.innerWidth) >= 2 ) {
                continue;
            }

            // 判断元素是否覆盖整个视口, 屏占比大于{screen_ratio_threshold}的元素被认为是潜在的Mask节点。
            const screenAreaRatio = Utils.getScreenAreaRatio(el);
            if(screenAreaRatio > Constant.maxScreenAreaRatioThreshold) {
                // 硬性条件，mask是半透明的
                if(Utils.isBackgroundSemiTransparent(style)) {
                    potentialMasks.push(el);
                    continue;
                }
                // 特例1：发现有非半透明情况，重新考虑
                // DOM结构弹窗：遮罩是fixed，100%屏占比，子节点是absolute,并且带有close按钮
                if(style.position === Constant.fixed && el.children.length == 1) {
                    const child = el.children[0];
                    if (getComputedStyle(child).position === Constant.absolute && Utils.getScreenAreaRatio(child) > Constant.minScreenAreaRatioThreshold) {
                        if (Utils.hasCloseButton(el.children[0])) {
                            potentialMasks.push(el);
                            continue;
                        }
                    }
                }

            }
            // 识别特例蒙版
            if (style.position === 'fixed' && LayoutUtils.analyzeComputedBoxShadow(style.boxShadow)) {
                // 特例，有一些弹窗的遮罩是使用BoxShadow来实现的
                potentialMasks.push(el);
                continue;
            }

            // 识别吸顶吸底元素
            if (style.position === 'fixed' && screenAreaRatio > Constant.minSARTofStickyComponent && 
                screenAreaRatio < Constant.maxSARTofStickyComponent && parseInt(style.left) == 0) {
                // 有的容器是透明的，但是子元素是不透明的
                // 有的容器是透明的，没有子元素
                // 因此，要分别区别处理，全透明的子元素不影响弹窗缩放
                // 如果容器透明，需要往下寻找不透明的全宽子元素
                if(PopupWindow.isStickyComponentVisiable(el)) {
                    // 只保留一个层级最高的吸顶吸底元素
                    if(parseInt(style.top) == 0) {
                        console.log(`找到吸顶元素${el.className}`);
                        if(potentialStickyTop != null && !PopupWindow.isFirstElementOnTop(el, potentialStickyTop)) {
                            continue;
                        }
                        potentialStickyTop = el;
                    }
                    if(parseInt(style.bottom) == 0) {
                        console.log(`找到吸底元素${el.className}`);
                        if(potentialStickyBottom != null && !PopupWindow.isFirstElementOnTop(el, potentialStickyBottom)) {
                            continue;
                        }
                        potentialStickyBottom = el;
                    }
                }
            }
        }
        return {potentialMasks, potentialStickyTop, potentialStickyBottom};
    }

    /**
     * 判断在两个重叠的元素中，第一个元素视觉上更靠上。
     * 该方法通过计算两个元素的相交区域，并检查该区域中心点最顶层的元素来实现。
     *
     * @param {Element} elementA - 第一个元素。
     * @param {Element} elementB - 第二个元素。
     * @returns {boolean} 如果 elementA 在 elementB 之上，则返回 true；如果它们不重叠或 elementB 在上，则返回 false。
     */
    private static isFirstElementOnTop(elementA:Element, elementB:Element) {
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
    @PerfExecution(({ level: Level.INFO }))
    static findPopups(root:HTMLElement): PopupInfo | null {
        // Slow Pass:从所有的节点中查找
        // 获取潜在的Mask节点和吸顶吸底元素
        const potentialElements = PopupWindow.getPotentialElements(root);
        return PopupWindow.findPopupsInternal(potentialElements);
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

    // 1. 寻找最佳兄弟内容候选者(B型弹窗)
    // 2、如果mask的children节点和兄弟节点的数量都为0，需要继续往上寻找父节点
    // 3、往上找到最多3个节点（规则约束）
    // 4、最先找到一个节点，除mask节点之外还有其他它有子节点，且包含close按钮，则这个节点是根节点
    // 5、如果找到的兄弟节点是mask的长兄节点，则需要判断它的zindex，保证在堆叠关系中，兄弟节点在mask节点的上面
    private static findBestSiblingContent(maskNode:Element) : [Element,Element] {
        let bestCandidate: Element | null = null;
        let maxRatio = 0;
        const maskZIndex = Utils.zIndexToNumber(window.getComputedStyle(maskNode).zIndex);
        
        const [siblings, root] = Utils.findSiblingContent(maskNode);

        // CSS堆叠上下文的判断逻辑
        for (const sibling of siblings) {
            // 只要有一个兄弟容器的层级高于或等于遮罩，就认为其内部所有内容都可见
            const candidatesInSibling: Element[] = [sibling];
            // 兄弟节点自身也必须算在内
            candidatesInSibling.push(sibling);
            sibling.querySelectorAll('*').forEach(node_ => candidatesInSibling.push(node_));
            // 对于长兄节点，它的zindex与蒙版节点zindex相等，会导致长兄节点显示在蒙版的底层，不满足弹窗内容节点的筛选要求
            // 通过设置zindex的偏移解决这个比对问题,长兄节点与蒙版zindex+1比较，弟弟节点与蒙版zindex比较。
            const maskZIndexOffset = PopupWindow.isPreviousElementSibling(maskNode,sibling)?1:0;
            for (const node of candidatesInSibling) {
                const siblingZIndex = Utils.zIndexToNumber(window.getComputedStyle(node).zIndex);
                if (siblingZIndex >= (maskZIndex + maskZIndexOffset)) {
                    const ratio = Utils.getScreenAreaRatio(node);
                    if (ratio > maxRatio) {
                        maxRatio = ratio;
                        bestCandidate = node;
                    }
                }
            }
        }
        return maxRatio >= Constant.minScreenAreaRatioThreshold ? [bestCandidate, root] : [null, null];
    }

    // 2. 寻找最佳后代内容候选者(A型/C型弹窗)
    // 没有mask的特例：C型：fixed mask屏占比100%，absolute子节点Content，包含close按钮
    private static findBestDescendantContent = (maskNode:Element): Element | null => {
        let bestCandidate: Element | null = null;
        let maxRatio = 0;
        const descendants = Array.from(maskNode.querySelectorAll('*'));
        
        for (const node of descendants) {
            // 过滤掉不可见节点
            if(Utils.visualFilter(node) == false) {
                continue;
            }
            // 对于后代节点，它们天然在父节点（遮罩）之上，无需比较z-index
            const ratio = Utils.getScreenAreaRatio(node);
            if (ratio > maxRatio) {
                maxRatio = ratio;
                bestCandidate = node;
            }
        }
        return maxRatio >= Constant.minScreenAreaRatioThreshold ? bestCandidate : null;
    }
    
    /**
     * 查找页面上所有符合条件的弹窗组件
     * @param {HTMLElement} root - 根节点
     * @returns { PopupInfo | null }
     */
    private static findPopupsInternal(potentialElements:PotentialElements): PopupInfo | null {
        const potentialMasks: Element[] = potentialElements.potentialMasks;
        if(!potentialMasks || potentialMasks.length <1) {
            return null;
        }
 
        // 临时存储所有找到的弹窗及其节点
        const allDetectedPopups: { info: PopupInfo; node: Element }[] = [];

        potentialMasks.sort((a, b) => Utils.getElementDepth(b) - Utils.getElementDepth(a));

        potentialMasks.forEach(maskNode => {
            let rootNode: Element | null = null;
            let contentNode: Element | null = null;
            let popupType = PopupType.Unknown;

            const [bestSiblingContent, root] = PopupWindow.findBestSiblingContent(maskNode);
            
            // 3. 决策：优先选择兄弟节点作为内容（B型），因为这是更明确的结构
            if (bestSiblingContent) {
                const style = getComputedStyle(maskNode.parentNode as Element);
                // 即使子节点的 position 是 absolute 或 fixed，它仍然会受到父节点 opacity: 0 的影响，从而变得不可见
                // 因此对于B型弹窗，需要过滤根节点节透明的情况
                if(!(style.display === 'none' || style.visibility === 'hidden' || parseFloat(style.opacity) === 0)) {
                    console.log(`Process potentialMasks ${maskNode}, has Large Child: ${bestSiblingContent}`);
                    popupType = PopupType.B;
                    contentNode = bestSiblingContent;
                    rootNode = root;
                }
            } else {
                // 继承2.2 mask的后代节点
                const bestDescendantContent = PopupWindow.findBestDescendantContent(maskNode);
                // 如果没有合适的兄弟节点，再采纳后代节点作为内容（A/C型）
                // 对于A/c型弹窗，需要过滤mask节点为透明的情况)
                if(bestDescendantContent && Utils.visualFilter(maskNode)) {
                    contentNode = bestDescendantContent;
                    const maskPosition = window.getComputedStyle(maskNode).position;
                    console.log(`Process potentialMasks ${maskNode}, maskPosition: ${maskPosition}`);
                    if (maskPosition === 'fixed' || maskPosition === 'absolute') {
                        popupType = PopupType.C;
                        rootNode = maskNode;
                    } else {
                        popupType = PopupType.A;
                        let parent: Element | null = maskNode.parentElement;
                        while (parent && parent !== document.body) {
                            const parentPosition = window.getComputedStyle(parent).position;
                            if (parentPosition === 'fixed' || parentPosition === 'absolute') {
                                rootNode = parent;
                                break;
                            }
                            parent = parent.parentElement;
                        }
                        if (!rootNode) rootNode = maskNode.parentElement;
                    }
                }else{
                    console.log(`Unkown PopUp type, mask node is ${maskNode}`);
                }
            }
            if (rootNode && maskNode) {
                console.log(`Process potentialMasks ${maskNode.className}, rootNode: ${rootNode.className}`);
                const rootStyle = window.getComputedStyle(rootNode);
                const maskStyle = window.getComputedStyle(maskNode);
                const popupInfo: PopupInfo = {
                    root_node: rootNode as HTMLElement,
                    mask_node: maskNode as HTMLElement,
                    content_node: contentNode as HTMLElement,
                    popup_type: popupType,
                    root_position: rootStyle.position,
                    root_zindex: Utils.zIndexToNumber(rootStyle.zIndex),
                    has_mask:true,
                    root_screen_area_ratio: Utils.getScreenAreaRatio(rootNode),
                    root_is_visiable: Utils.isElementVisibleInViewPort(rootNode),
                    has_close_button: Utils.hasCloseButton(contentNode || rootNode),
                    mask_area_ratio: Utils.getScreenAreaRatio(maskNode),
                    mask_position: maskStyle.position,
                    mask_zindex: Utils.zIndexToNumber(maskStyle.zIndex),
                    stickyTop_height:0,
                    stickyBottom_height:0
                };
                
                // 将弹窗信息和节点存入临时列表
                allDetectedPopups.push({ info: popupInfo, node: rootNode });
            }
        });

        // 1. 筛选出所有作为“父弹窗”的弹窗对象
        const parentPopups = allDetectedPopups.filter(p => {
                // 判断是否存在任何一个 q 被 p 包含。
                return allDetectedPopups.some(q => {
                    if(p.node !== q.node) {
                        return p.node.contains(q.node);
                    } else {
                        // 如果mask存在“父子关系，取子元素”
                        const pm = p.info.mask_node;
                        const qm = q.info.mask_node;
                        return (qm !== pm) && pm.contains(qm);
                    }
                }
            );
        });

        // 2. 将找到的父弹窗的 info 属性提取出来，放入 Set 中
        const popupsToRemove = new Set<PopupInfo>(parentPopups.map(p => p.info));

        // 3. 返回过滤后的结果，使用模型预测
        const finalPopups = allDetectedPopups
            .map(p => p.info)
            .filter(info => {
                if(!popupsToRemove.has(info)) {
                    // fastPass快速判断
                    if(info.has_mask && info.mask_area_ratio > 95 && (info.mask_position == 'fixed' || info.mask_position == 'absolute')) {
                        return  true;
                    }
                    // 通过模型判断
                    const result = PopupRecog.predictIsPopup(info);
                    return result.prediction;
                }
                return false;
            });
        
        // 多个弹窗，选择最上层的一个返回
        const finalPop = PopupWindow.findTopMostPopup(finalPopups);
        // 如果存在吸顶和吸底元素，需要更新finalPop的吸顶吸底元素的高度值
        if(finalPop!= null && potentialElements.potentialStickyBottom != null) {
            const stickyBottomStyle = getComputedStyle(potentialElements.potentialStickyBottom);
            if (LayoutUtils.compareZIndex(finalPop.mask_node, potentialElements.potentialStickyBottom as HTMLElement) <= 0) {
                finalPop.stickyBottom_height = parseInt(stickyBottomStyle.height);
            }
        }
        if(finalPop != null && potentialElements.potentialStickyTop != null) {
            const stickyTopStyle = getComputedStyle(potentialElements.potentialStickyTop);
            if (LayoutUtils.compareZIndex(finalPop.mask_node, potentialElements.potentialStickyTop as HTMLElement) <= 0) {
                finalPop.stickyTop_height = parseInt(stickyTopStyle.height);
            }
        }

        return finalPop;
    }

    /**
     * 检查 potentialNodeBelow 是否在 currentNode 的长兄节点
     * @param {Element} currentNode - 当前参考节点
     * @param {Element} potentialNodeBelow - 需要检查的节点
     * @returns {boolean} - 如果 potentialNodeBelow 是长兄节点则返回 true，否则返回 false
     */
    private static isPreviousElementSibling(currentNode:Element, potentialNodeBelow:Element) {
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
}