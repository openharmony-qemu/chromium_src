import { LayoutKey, LayoutValue } from "../../Common/Constant";
import Store from "../../Common/Utils/Store";
import Utils from "./Utils";
import Constant from "./Constant";
import { BoxShadow } from "./BoxShaodw";
import { PopupInfo } from "../Popup/PopupInfo";
import { PopupDecisionTreeType } from "../Popup/PopupDecisionTreeType";

interface VisualBoundingRect {
    left: number;
    top: number;
    right: number;
    bottom: number;
    width: number;
    height: number;
    offsetY: number;
    scrollElement: HTMLElement | null; // 明确声明 scrollElement 的类型
}

export default class LayoutUtils{
    private static hasInitFontSizeMediQuery = false;

    // 硬编码宽高的节点缓存
    private static hardSizeNodeCache = new Map<HTMLElement, Map<string,string>>();
    
    /**
     * 检查从指定元素到结束元素的路径上是否满足跨行条件。
     * 
     * 该方法会遍历从 `element` 到 `endElement` 的元素路径，检查每个元素的 `display` 样式属性。
     * 如果在遍历过程中遇到 `display` 为 `flex` 且子节点数量大于 2 的元素，或者遇到 `display` 既不是 `block` 也不是 `flex` 的元素，则认为不满足跨行条件，返回 `false`。
     * 如果成功遍历到 `endElement` 且未遇到上述不符合条件的元素，则认为满足跨行条件，返回 `true`。
     * 
     * @static
     * @param {HTMLElement} element - 起始 HTML 元素，作为遍历的起点。
     * @param {HTMLElement} endElement - 结束 HTML 元素，作为遍历的终点。
     * @returns {boolean} - 如果从 `element` 到 `endElement` 的路径满足跨行条件，返回 `true`；否则返回 `false`。
     */
    static isCrossRow(element: HTMLElement,endElement: HTMLElement): boolean {
        let ele = element;
        while(ele != endElement) {
            const display = window.getComputedStyle(ele).display;
            if(display ==  'flex'&& ele.childNodes.length > 2) {
                return false;
            }
            if(! ['block', 'flex'].includes(display) ) {
                return false; 
            }
            ele = ele.parentElement;
        }
        return true;
    }

    // 检查是否存在被clip的节点，即子节点尺寸>容器尺寸
    static isExsitClipNode(root: HTMLElement,isRefTree:boolean) {
        function traverse(node: HTMLElement) {
            if (node.nodeType === Node.ELEMENT_NODE) {
                const nodeWidth = isRefTree?node.style.width: window.getComputedStyle(node).width;
                const rootWidth = isRefTree?root.style.width: window.getComputedStyle(root).width;
                const wdiff = parseFloat(nodeWidth) -parseFloat(rootWidth);
                if (wdiff > 10 ) {
                    return true;
                }
                for (let i = 0; i < node.children.length; i++) {
                    const ele = node.children[i];
                    if (traverse(ele as HTMLElement)) {
                        return true;
                    }
                }
            }
            return false;
        }
        return traverse(root);
    }
 
    static hasBottomStyle(element: HTMLElement, computedPosition: string, computedBottom: string): boolean {

        // 先检查内联样式(style属性)
        const bottomInlineValue = element.style.bottom;
        if (bottomInlineValue !== "" && this.convertToPxUnits(bottomInlineValue) === computedBottom) {
            return true;
        }

        // 获取元素的所有 CSS 规则（包含内部样式（style标签），外部样式（同源、跨域））
        const stylesheets = document.styleSheets;
        let hasCrossDomainStyle = false;
        for (const sheet of stylesheets) {
          try {
            for (const rule of sheet.cssRules) {
              if (rule instanceof CSSStyleRule && rule.selectorText && element.matches(rule.selectorText)) {
                const bottomValue = rule.style.getPropertyValue(Constant.bottom);
                if (bottomValue.length > 0) {  

                    const convertValue = LayoutUtils.convertToPxUnits(bottomValue);
                    if (convertValue === computedBottom) {
                        return true;
                    }  

                }
              }
            }
          } catch (e) {
            hasCrossDomainStyle = true;
            console.log('无法访问跨域样式表:', sheet.href);
          }
        }

        if (computedPosition === Constant.absolute || computedPosition === Constant.fixed) {  // 绝对定位元素的bottom必须有值
            return hasCrossDomainStyle;     
        } 

        // 对应position为relative|static，并且bottom计算样式为0px
        return true;  
    }

    static hasTopStyle(element: HTMLElement, computedPosition: string, computedTop: string): boolean {

        // 先检查内联样式(style属性)
        const topInlineValue = element.style.top;
        if (topInlineValue !== "" && this.convertToPxUnits(topInlineValue) === computedTop) {
            return true;
        }

        // 获取元素的所有 CSS 规则（包含内部样式（style标签），外部样式（同源、跨域））
        const stylesheets = document.styleSheets;
        let hasCrossDomainStyle = false;
        for (const sheet of stylesheets) {
          try {
            for (const rule of sheet.cssRules) {
              if (rule instanceof CSSStyleRule && rule.selectorText && element.matches(rule.selectorText)) {
                const topValue = rule.style.getPropertyValue(Constant.top);
                if (topValue.length > 0) {  

                    const convertValue = LayoutUtils.convertToPxUnits(topValue);
                    if (convertValue === computedTop) {
                        return true;
                    }  

                }
              }
            }
          } catch (e) {
            hasCrossDomainStyle = true;
            console.log('无法访问跨域样式表:', sheet.href);
          }
        }

        if (computedPosition === Constant.absolute || computedPosition === Constant.fixed) {  // 绝对定位元素的top必须有值
            return hasCrossDomainStyle;     
        } 

        // 对应position为relative|static，并且top计算样式为0px
        return true;  
    }

    // 将获取到的宽度属性值转换为px单位
    static convertToPxUnits(value: string): string {
        const px = "px";
        if (value.endsWith("rem")) {
            return parseFloat(value) * parseFloat(getComputedStyle(document.documentElement).fontSize) + px;
        } else if (value.endsWith("vw") || value.endsWith("%")) {
            return window.innerWidth * parseFloat(value) / 100 + px;
        } else {  // value本身就是px单位， 或是其他特殊情况
            return value;
        }
    }

    /**
     * 是否在视口内有截断
     * @param node 
     * @param root 
     * @returns 
     */
    static isNodeTruncated(node: Element, popupInfo: PopupInfo): boolean {
        if (!(node instanceof HTMLElement)) {
            return false;
        }
        const root = popupInfo.root_node;
        const { top, bottom, left, right } = node.getBoundingClientRect();
        const topInt = Math.floor(top);
        const bottomInt = Math.floor(bottom);
        const leftInt = Math.floor(left);
        const rightInt = Math.floor(right);

        const visualHeight = window.innerHeight - popupInfo.stickyTop_height - popupInfo.stickyBottom_height;

        if ((topInt < 0 || bottomInt > visualHeight || leftInt < 0 || rightInt > window.innerWidth) && Utils.isElementVisible(node, root)) {
            console.log(`find truncate node: ${node.className}`);
            return true;
        }
        if (node.tagName === 'IMG') {
            const isHidden = getComputedStyle(node).visibility === 'hidden';
            const parentNode = node.parentElement;
            const height = parseInt(getComputedStyle(parentNode).height);
            if (isHidden && parentNode && height < parentNode.scrollHeight) {
                parentNode.style.minHeight = parentNode.scrollHeight.toString();
            }
        }
        return false;
    }

    /**
     * 对于一个包含backgroundSize为cover的背景图的节点，一致做处理
     * @param node 
     */
    static checkIfBackgroundImgTruncated(node: Element): boolean {
        if (!(node instanceof HTMLElement)) {
            return false;
        }
        let computedStyle = getComputedStyle(node);
        if (computedStyle.backgroundImage != 'none' && computedStyle.backgroundSize == 'cover') {
            return true;
        }
        return false;
    }

    static getPropFromStyle(node:HTMLElement, property: string): string {
        let nodeClassList = node.classList;
        let propValue = '';
        const regex = new RegExp(`(^|;)\\s*bottom\\s*:\\s*([^;]+)`, 'i');
        // 检查动态添加的<style>标签
        document.querySelectorAll('style').forEach(styleTag => {
            const cssText = styleTag.textContent;
    
            // 按规则拆分（简单正则匹配）
            const rules = cssText.split('}');
            rules.forEach(rule => {
                for (let className of nodeClassList) {
                    if (rule.includes(className + ":")) {
                        // 提取完整的规则文本
                        const fullRule = rule + '}';
                        console.log('getPropFromStyle: find target rule ', fullRule);
                        
                        // 提取top值（更精确的正则匹配）
                        const topMatch = fullRule.match(regex);
                        if (topMatch && parseFloat(topMatch[2].trim()) != 0) {
                            console.log('getPropFromStyle: find top ' + topMatch + ' with className ' + className);
                            propValue = topMatch[2].trim();
                        }
                    }
                }
            });
        });
        return propValue;
    }

    static getWidthAsPx(element:HTMLElement, prop:string): string {
        const computedStyle = window.getComputedStyle(element);
        const value = computedStyle.getPropertyValue(prop);
        return value;
    }

    static canBeRelayout(ele: HTMLElement): boolean {
        const layoutOfHw = Store.getValue(ele, LayoutKey.LAYOUT_TAG);
        const needRelayoutTags = [
            null,
            LayoutValue.ZOOM_PARENT,
            LayoutValue.ONE_LINE,
            LayoutValue.WATERFALL,
            LayoutValue.ASIDE_BAR,
            LayoutValue.HEADER,
            LayoutValue.BOTTOM,
            LayoutValue.SCROLL_LIST,
            LayoutValue.VERTICAL_GRID,
            LayoutValue.SWIPER,
        ];
        return needRelayoutTags.includes(layoutOfHw);
    }

    static getVisibleChildren(root:HTMLElement):Array<Element> {
        return Array.from(root.children).filter((child:HTMLElement) => {
            if (!child || child.nodeType !== Node.ELEMENT_NODE) return false;
            const childStyle = window.getComputedStyle(child);
            return childStyle.display !== 'none' &&
                    childStyle.visibility !== 'hidden' &&
                    childStyle.opacity !== '0'
        });
    }

    /**
     * 递归查找包含至少两个相同尺寸子元素的父节点
     * @param {HTMLElement} root - 遍历的起始节点
     * @param {number} [tolerance=5] - 尺寸容差（像素）
     * @returns {HTMLElement|null} 首个符合条件的父节点
     */
    static findParentWithEqualChildren(root:HTMLElement, tolerance = 5):HTMLElement {
        // 过滤不可见节点
        if (!root || root.nodeType !== Node.ELEMENT_NODE) return null;
        const style = window.getComputedStyle(root);
        if (style.display === 'none') return null;
        
        // 获取可见子元素
        const visibleChildren = Array.from(root.children).filter((child:HTMLElement) => {
            const childStyle = window.getComputedStyle(child);
            return childStyle.display !== 'none' &&
                childStyle.visibility !== 'hidden' &&
                childStyle.opacity !== '0'
        });

        // 尺寸检测逻辑
        if (visibleChildren.length >= 2) {
            const sizeMap = new Map();
            
            // 带容差的尺寸标准化
            const normalize = (val: number) => 
                Math.round(val / tolerance) * tolerance;
        
            // 统计尺寸出现次数
            for (const child of visibleChildren) {
                const rect = child.getBoundingClientRect();
                const key = `${normalize(rect.width)}, ${normalize(rect.height)}`;
                sizeMap.set(key, (sizeMap.get(key) || 0) + 1);
            }
        
            // 检查是否存在重复尺寸
            if ([...sizeMap.values()].some(count => count >= 2)) {
                return root;
            }
        }
    
        // 深度优先递归搜索
        for (const child of visibleChildren) {
            const foundEle = this.findParentWithEqualChildren(child as HTMLElement, tolerance);
            if (root != foundEle && root.contains(foundEle)) {
                continue;
            }
            if (foundEle) return foundEle;
        }
    
        return null;
    }

    /**
     * 解析一个通过 getComputedStyle 获取的 box-shadow 字符串，并判断它是否为全屏遮罩。
     * @param computedBoxShadow - 从 getComputedStyle(...).boxShadow 获取的字符串。
     * @param viewportWidth - 当前视口的宽度 (单位: px)。
     * @param viewportHeight - 当前视口的高度 (单位: px)。
     * @returns {false | true} 如果遮罩是非全屏的，返回false，是全屏的返回true。
     */
    static analyzeComputedBoxShadow( computedBoxShadow: string): boolean {
        // 过滤无效输入
        if (!computedBoxShadow || typeof computedBoxShadow !== 'string' || computedBoxShadow === 'none' || computedBoxShadow === '') {
            return null;
        }

        // 正则表达式，用于匹配颜色部分（无论在开头还是结尾）
        const colorRegex = /(rgba?\(.*?\)|hsla?\(.*?\)|#[\da-f]{3,8}|[a-z]+)/i;
        const colorMatch = computedBoxShadow.match(colorRegex);

        if (!colorMatch) {
            return null; // 未找到颜色值，无法解析
        }

        const color = colorMatch[0];

        // 从字符串中移除颜色部分，剩下的就是数值
        const numericsString = computedBoxShadow.replace(color, '').trim();
        const numericParts = numericsString.split(/\s+/).filter(Boolean);
        
        // 正常的阴影至少应包含2个数值（X和Y偏移）
        if (numericParts.length < 2) {
            return null;
        }

        // 构建基础分析对象
        const analysis: Omit<BoxShadow, 'isFullScreenMask'> = {
            offsetX: numericParts[0] ?? '0px',
            offsetY: numericParts[1] ?? '0px',
            blurRadius: numericParts[2] ?? '0px',
            spreadRadius: numericParts[3] ?? '0px', // 扩展半径是第四个值
            color: color,
        };

        // --- 核心判断逻辑 ---
        const spreadPx = parseFloat(analysis.spreadRadius);
        
        // 判断条件：扩展半径是否大于等于视口宽高中的最大值
        const isSpreadLargeEnough = spreadPx >= Math.max(window.innerWidth, window.innerHeight);

        // 综合判断
        const isMask =
            (analysis.offsetX).trim().startsWith('0') &&
            (analysis.offsetY).trim().startsWith('0') &&
            (analysis.blurRadius).trim().startsWith('0') &&
            isSpreadLargeEnough;

        return isMask;
    }


    /**
     * step1: 判断是否close按钮
     * step2：判断close按钮是否遮挡了弹窗的其他内容，遮挡判定有两种情况：
     * 1、position: absolute，bottom < 0, 和父布局重叠
     * 2、和其他兄弟节点有重叠
     * @param popup 
     * @returns 
     */
    static isBottomCloseButtonOverlap(popup: PopupInfo): boolean {
        // 查找所有可能的关闭按钮
        const closeElements = popup.root_node.querySelectorAll('[class*="close"]');
        if (closeElements.length != 1) {
            return false;
        }
        const closeButton = closeElements[0] as HTMLElement;
        const closeBtnStyle = getComputedStyle(closeButton);
        const closeBtnRect = closeButton.getBoundingClientRect();
        if (this.isCloseButton(closeButton)) {
            // 如果是绝对布局，且bottom为负，则说明这个组件会溢出父布局，判断它和父布局是否完全脱离（是否重叠）
            if (closeBtnStyle.position === 'absolute' && parseFloat(closeBtnStyle.bottom) < 0) {
                const buttonTop = closeBtnRect.top;
                const parentTop = closeButton.parentElement.getBoundingClientRect().top;
                const parentBottom = closeButton.parentElement.getBoundingClientRect().bottom;
                if (buttonTop < parentBottom && buttonTop > (parentTop + parentBottom) / 2) {
                    return true;
                }
            }
        }
        
        return false;
    }

    static isCloseButton(element: HTMLElement): boolean {
        const closeBtnStyle = getComputedStyle(element);

        if (closeBtnStyle.background.includes('url') || element.getAttribute('role') == 'button') {
            console.log('this is a close button');
            return true;
        }
        return false;
    }

    /**
     * 判断是否存在底部按钮，并且它或者它的父布局与其他兄弟节点有重叠
     * @param popup 
     * @returns 
     */
    static isAbsoluteButtonInBottom(popup: PopupInfo): boolean {
        const closeElements = popup.root_node.querySelectorAll('[class*="button"]');

        return false;
    }

    /**
     * 检测当前节点是否有元素兄弟节点
     * @param node 
     * @returns 
     */
    static hasElementSiblings(node: HTMLElement): boolean {
        return node.previousElementSibling !== null || node.nextElementSibling !== null;
    }

    /**
     * 获取节点及其所有子孙节点可视区域
     * @param element 
     * @returns 
     */
    static getVisualBoundingRectForAll(element: HTMLElement) {
        if (!element || typeof element.getBoundingClientRect !== 'function') {
            return null;
        }
        const parentRect = element.getBoundingClientRect();
    
        // 1. 初始化边界：以父元素自身的边界为起点
        let minTop = parentRect.top;
        let maxBottom = parentRect.bottom;
    
        const treeWalker = document.createTreeWalker(
            element,
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
            minTop = Math.min(minTop, childTop);
            maxBottom = Math.max(maxBottom, childBottom);

            currentNode = treeWalker.nextNode() as HTMLElement;
          }

        return {
            top: minTop,
            bottom: maxBottom
        }
    }

    /**
     * 计算一个元素及其所有后代元素所形成的整体视觉边界框。
     * 返回一个类似 DOMRect 的对象，包含 {x, y, top, left, bottom, right, width, height}。
     * @param {HTMLElement} element - 要计算的目标父元素。
     * @param {isCloseButtonTruncatedByScroll} boolean 关闭按钮是否被scroll截断
     * @param {originY} string 缩放中心
     * @returns {object|null} 一个类似 DOMRect 的对象，如果元素无效则返回 null。
     */
    static getVisualBoundingRect(element: HTMLElement, isCloseButtonTruncatedByScroll: boolean, popupDecisionTreeType: PopupDecisionTreeType): VisualBoundingRect {
        if (!element || typeof element.getBoundingClientRect !== 'function') {
            return null;
        }
        const parentRect = element.getBoundingClientRect();
        let scrollElement = null;
        let offsetY = 0;

        // 递归函数，用于更新边界
        const updateBounds = (el: HTMLElement, rootNode: HTMLElement) => {
            // 跳过具有滚动条的元素
            if (this.hasScrollbar(el)) {
                if (isCloseButtonTruncatedByScroll) {
                    if (popupDecisionTreeType === PopupDecisionTreeType.Bottom) {
                        offsetY -= (el.scrollHeight - el.getBoundingClientRect().height);
                    } else if (popupDecisionTreeType === PopupDecisionTreeType.Center || popupDecisionTreeType === PopupDecisionTreeType.Center_Button_Overlap) {
                        offsetY -= (el.scrollHeight - el.getBoundingClientRect().height) / 2;
                    }
                }
                scrollElement = el;
                return;
            }
            const rect = el.getBoundingClientRect();
            const style = window.getComputedStyle(el);
            // 节点被隐藏，不继续参与遍历计算
            if (el !== rootNode && style.overflowY === 'hidden' && el.scrollHeight > el.clientHeight) {
                return;
            }
            // 检查元素是否可见
            const isElementVisible = rect.width > 0 && rect.height > 0 &&
                style.display !== 'none' && style.visibility !== 'hidden' &&
                parseFloat(style.opacity) > 0;
            if (isElementVisible) {
                // 计算宽高比并检查是否在合理范围内
                const aspectRatio = rect.height / rect.width;
                if (aspectRatio > 0.1 && aspectRatio < 10) {
                    // 更新边界坐标
                    minX = Math.min(minX, rect.left);
                    minY = Math.min(minY, rect.top);
                    maxX = Math.max(maxX, rect.right);
                    maxY = Math.max(maxY, rect.bottom);
                }
            }
            // 递归遍历子节点
            for (const child of el.children) {
                if (child instanceof HTMLElement) {
                    updateBounds(child, rootNode);
                }
            }
        };
        // 1. 初始化边界
        let minX = Number.MAX_SAFE_INTEGER;
        let minY = Number.MAX_SAFE_INTEGER;
        let maxX = Number.MIN_SAFE_INTEGER;
        let maxY = Number.MIN_SAFE_INTEGER;
        if (parentRect.height !== 0 && parentRect.width !== 0) {
            minX = Math.min(minX, parentRect.left);
            minY = Math.min(minY, parentRect.top);
            maxX = Math.max(maxX, parentRect.right);
            maxY = Math.max(maxY, parentRect.bottom);
        }
        // 遍历所有节点来更新边界
        updateBounds(element, element);
        // 计算最终的边界尺寸
        const width = maxX - minX;
        const height = maxY - minY;
        // 计算子元素超出父元素区域的偏差高度
        if (parentRect.height !== 0 && parentRect.width !== 0) {
            offsetY += (minY - parentRect.top + (height - parentRect.height) / 2);
        }
        return {
            left: minX,
            top: minY,
            right: maxX,
            bottom: maxY,
            width: width,
            height: height,
            offsetY: offsetY,
            scrollElement: scrollElement
        };
    }

    static hasScrollbar(element: HTMLElement): boolean {
        // 检查CSS属性
        const style = window.getComputedStyle(element);
        const overflowY = style.overflowY;
        
        const isScrollableY = (overflowY === 'scroll' || overflowY === 'auto' );
        
        // const hasVerticalScroll = element.scrollHeight > element.clientHeight;
        
        // 综合考虑
        return isScrollableY;
    }

    /**
     * 获取节点的层叠上下文链
     */
    static getStackingContextChain(node: HTMLElement) {
        const chain = [];
        let current = node;
        
        while (current && current !== document.documentElement) {
            if (LayoutUtils.isStackingContext(current)) {
                chain.push({
                    element: current,
                    zIndex: LayoutUtils.getEffectiveZIndex(current)
                });
            }
            current = current.parentElement;
        }
        
        // 添加根层叠上下文
        chain.push({
            element: document.documentElement,
            zIndex: 0
        });
        
        return chain.reverse(); // 从根到叶子
    }

    /**
     * 判断元素是否创建层叠上下文
     */
    static isStackingContext(element: HTMLElement): boolean {
        const style = window.getComputedStyle(element);
        
        // 特殊情况：根元素总是层叠上下文
        if (element === document.documentElement) return true;
        
        // position + z-index ≠ auto
        if (style.position !== 'static') {
            if (style.zIndex !== 'auto') return true;
        }
        
        // 其他创建层叠上下文的情况
        if (parseFloat(style.opacity) < 1 
            || style.transform !== 'none'
            || style.filter !== 'none'
            || style.isolation === 'isolate'
            || style.mixBlendMode !== 'normal') {
                return true;
            }

        if (style.willChange === 'z-index' || style.willChange.includes('transform') || 
            style.willChange.includes('opacity')) {
                return true;
            }
        
        return false;
    }

    /**
     * 获取有效的 z-index 值
     */
    static getEffectiveZIndex(element: HTMLElement): number {
        const style = window.getComputedStyle(element);
        if (style.zIndex === 'auto') return 0;
        return parseInt(style.zIndex, 10) || 0;
    }

    /**
     * 比较两个可见节点的层叠顺序
     * @returns 1: nodeA在上, -1: nodeB在上, 0: 同一层级或无法比较
     */
    static compareZIndex(nodeA: HTMLElement, nodeB: HTMLElement) {
        if (nodeA === nodeB) return 0;
        
        const chainA = LayoutUtils.getStackingContextChain(nodeA);
        const chainB = LayoutUtils.getStackingContextChain(nodeB);
        
        // 找到第一个不同的层叠上下文祖先
        let i = 0;
        while (i < chainA.length && i < chainB.length) {
            const ctxA = chainA[i];
            const ctxB = chainB[i];
            
            if (ctxA.element !== ctxB.element) {
                // 比较这两个兄弟层叠上下文的z-index
                return ctxA.zIndex > ctxB.zIndex ? 1 : -1;
            }
            
            // 如果是同一个层叠上下文，继续向叶子节点比较
            i++;
        }
        
        // 如果到达这里，说明在同一个层叠上下文中
        // 比较它们在DOM中的顺序（后来者居上）
        const position = nodeA.compareDocumentPosition(nodeB);
        if (position & Node.DOCUMENT_POSITION_FOLLOWING) {
            return -1; // B在A后面，B在上
        } else if (position & Node.DOCUMENT_POSITION_PRECEDING) {
            return 1; // A在B后面，A在上
        }
        
        return 0;
    }
}