import { LayoutKey, LayoutValue } from '../Common/Constant';
import Log from '../../Debug/Log';
import Tag from '../../Debug/Tag';
import Store from '../Common/Utils/Store';
import Constant from '../Common/Constant';
import { BoxShadow } from '../Common/BoxShadow';
import { PopupInfo } from '../Popup/PopupInfo';
import { PopupDecisionTreeType } from '../Popup/PopupDecisionTreeType';

export interface VisualBoundingRect {
    left: number;
    top: number;
    right: number;
    bottom: number;
    width: number;
    height: number;
    offsetY: number;
    scrollElement: HTMLElement | null; // 明确声明 scrollElement 的类型
}

type StackingContextInfo = {
    element: HTMLElement,
    zIndex: number
}

export default class LayoutUtils{
    /**
     * 验证一个元素的某个样式属性是否被显式定义过。
     * 警告：此函数会遍历页面所有样式规则，可能导致性能问题，请谨慎使用。
     *
     * @param element - 要检查的 HTML 元素。
     * @param cssProperty - 要检查的 CSS 属性名 (例如 'top', 'bottom')。
     * @param computedValue - `getComputedStyle` 预先计算出的属性值。
     * @param computedPosition - `getComputedStyle` 预先计算出的 position 值。
     * @returns {boolean} 如果样式被显式定义或符合特定布局规则，则返回 true。
     */
    private static hasExplicitlyDefinedStyle(element: HTMLElement, cssProperty: 'top' | 'bottom', computedValue: string, computedPosition: string): boolean {
        Log.d(`========== 检测显式样式定义 ==========`, Tag.layoutUtils);
        Log.d(`检查元素: ${element.className || element.tagName}, 属性: ${cssProperty}, 计算值: ${computedValue}`, Tag.layoutUtils);
        
        // 1. 检查内联样式 (最快)
        // 使用 bracket notation (方括号) 来动态访问属性
        const inlineValue = element.style[cssProperty];
        if (inlineValue && this.convertToPxUnits(inlineValue) === computedValue) {
            Log.d(`✅ 在内联样式中找到匹配: ${inlineValue}`, Tag.layoutUtils);
            return true;
        }
        
        // 2. 遍历所有样式表 (性能开销巨大)
        Log.d(`🔍 开始遍历样式表...`, Tag.layoutUtils);
        const sheetCheckResult = this.findStyleInSheets(element, cssProperty, computedValue);
        if (sheetCheckResult.isFound) {
            Log.d(`✅ 在样式表中找到匹配`, Tag.layoutUtils);
            return true;
        }
       
        if (!sheetCheckResult.crossDomainSheetEncountered) {
            Log.d(`❌ 未找到显式定义`, Tag.layoutUtils);
            return false;
        }
        
        Log.d(`⚠️ 遇到跨域样式表，尝试位置计算`, Tag.layoutUtils);
        const rect = element.getBoundingClientRect();
        if (cssProperty === 'top') {
            const result = Math.abs(rect[cssProperty] - parseFloat(computedValue)) < Constant.discrepancy;
            Log.d(`${result ? '✅' : '❌'} top位置匹配结果: rect.top=${rect.top}, 差异=${Math.abs(rect[cssProperty] - parseFloat(computedValue))}`, Tag.layoutUtils);
            return result;
        }
        const result = Math.abs(rect[cssProperty] - window.innerHeight) < Constant.discrepancy;
        Log.d(`${result ? '✅' : '❌'} bottom位置匹配结果: rect.bottom=${rect.bottom}, 差异=${Math.abs(rect[cssProperty] - window.innerHeight)}`, Tag.layoutUtils);
        return result;
    }

    /**
     * 遍历所有样式表，查找匹配的样式规则。
     * @returns 一个包含查找结果和是否遇到跨域样式表标志的对象。
     */
    private static findStyleInSheets(element: HTMLElement, cssProperty: string, computedValue: string):
        { isFound: boolean; crossDomainSheetEncountered: boolean } {
        let crossDomainSheetEncountered = false;
        Log.d(`📄 遍历样式表数量: ${document.styleSheets.length}`, Tag.layoutUtils);

        // 主函数职责：遍历样式表，调用处理器，并聚合结果。嵌套深度只有 1。
        for (const sheet of document.styleSheets) {
            const result = this._processSingleSheet(sheet, element, cssProperty, computedValue);

            if (result.error) {
                crossDomainSheetEncountered = true;
            }

            if (result.isFound) {
                // 一旦找到，就可以提前返回最终结果
                Log.d(`✅ 在样式表中找到匹配规则`, Tag.layoutUtils);
                return { isFound: true, crossDomainSheetEncountered };
            }
        }

        // 遍历完所有样式表都未找到
        Log.d(`❌ 遍历完所有样式表未找到匹配, 跨域遇到: ${crossDomainSheetEncountered}`, Tag.layoutUtils);
        return { isFound: false, crossDomainSheetEncountered };
    }

    /**
     * 处理单个样式表
     * @returns 返回一个对象，表明是否找到匹配项以及是否发生错误。
     */
    private static _processSingleSheet(
        sheet: CSSStyleSheet,
        element: HTMLElement,
        cssProperty: string,
        computedValue: string
    ): { isFound: boolean; error: boolean } {
        try {
            const isFound = Array.from(sheet.cssRules).some(rule =>
                this._isRuleMatch(rule, element, cssProperty, computedValue)
            );
            return { isFound, error: false };
        } catch (e) {
            Log.w(`无法访问跨域样式表，某些样式检查可能不准确: ${sheet.href}`, Tag.layoutUtils);
            return { isFound: false, error: true };
        }
    }

    /**
     * 检查单个 CSS 规则是否匹配。
     * @returns 如果规则匹配，则返回 true。
     */
    private static _isRuleMatch(rule: CSSRule, element: HTMLElement, cssProperty: string, computedValue: string): boolean {
        // 使用卫语句（Guard Clause）提前退出，避免嵌套
        if (!(rule instanceof CSSStyleRule && rule.selectorText && element.matches(rule.selectorText))) {
            return false;
        }

        const propertyValue = rule.style.getPropertyValue(cssProperty);
        
        // 返回最终的比较结果
        return propertyValue && LayoutUtils.convertToPxUnits(propertyValue) === computedValue;
    }

    /**
     * 检查元素的 top 样式是否被显式定义。
     */
    static hasTopStyle(element: HTMLElement, computedPosition: string, computedTop: string): boolean {
        // 直接调用通用函数，传入 'top'
        return this.hasExplicitlyDefinedStyle(element, 'top', computedTop, computedPosition);
    }

    /**
     * 检查元素的 bottom 样式是否被显式定义。
     */
    static hasBottomStyle(element: HTMLElement, computedPosition: string, computedBottom: string): boolean {
        // 直接调用通用函数，传入 'bottom'
        return this.hasExplicitlyDefinedStyle(element, 'bottom', computedBottom, computedPosition);
    }

    // 将获取到的宽度属性值转换为px单位
    static convertToPxUnits(value: string): string {
        const px = 'px';
        if (value.endsWith('rem')) {
            return parseFloat(value) * parseFloat(getComputedStyle(document.documentElement).fontSize) + px;
        } else if (value.endsWith('vw') || value.endsWith('%')) {
            return window.innerHeight * parseFloat(value) / 100 + px;
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
        
        Log.d(`========== 检测节点截断 ==========`, Tag.layoutUtils);
        Log.d(`节点: ${node.className || node.tagName}`, Tag.layoutUtils);

        // 1. 获取视口尺寸
        const viewportHeight = window.innerHeight;

        // 2. 获取吸顶吸底高度信息
        const stickyTopHeight = popupInfo?.stickyTop_height || 0;
        const stickyBottomHeight = popupInfo?.stickyBottom_height || 0;

        // 3. 计算有效可视区域高度
        const effectiveViewportHeight = viewportHeight - stickyTopHeight - stickyBottomHeight;

        Log.d(`视口信息: 总高度=${viewportHeight}, 吸顶高度=${stickyTopHeight}, 吸底高度=${stickyBottomHeight}`, Tag.layoutUtils);
        Log.d(`有效可视区域高度: ${effectiveViewportHeight}`, Tag.layoutUtils);

        // 4. 获取元素的边界框（相对于视口）
       const rect = node.getBoundingClientRect();

        // 5. 获取计算后的样式
        const style = window.getComputedStyle(node);
        const borderTop = parseFloat(style.borderTopWidth);
        const borderBottom = parseFloat(style.borderBottomWidth);
        const paddingTop = parseFloat(style.paddingTop);
        const paddingBottom = parseFloat(style.paddingBottom);

        // 6. 计算内容框（Content Box）在视口中的坐标
        const contentTop = rect.top + borderTop + paddingTop;
        const contentBottom = rect.bottom - borderBottom - paddingBottom;

        Log.d(`内容区坐标: top=${contentTop}, bottom=${contentBottom}`, Tag.layoutUtils);

        // 边界情况处理：如果元素的padding和border过大，可能导致内容区尺寸为0或负数
        // 在这种情况下，我们认为其内容没有"空间"被截断
        if (contentTop >= contentBottom) {
            Log.d(`⚠️ 内容区尺寸无效 (contentTop >= contentBottom)`, Tag.layoutUtils);
            return false;
        }

        // 7. 比较内容框与有效可视区域的边界
        // 顶部截断：内容在吸顶区域上方
        const isTopTruncated = Math.round(contentTop) < stickyTopHeight;
        // 底部截断：内容在吸底区域下方（即 contentBottom > viewportHeight - stickyBottomHeight），允许偏差 2px
        const isBottomTruncated = Math.round(contentBottom) > (viewportHeight - stickyBottomHeight) + Constant.truncateThreshold;

        if(isTopTruncated) {
            Log.d(`✂️ 节点 ${node.className} 顶部截断: contentTop(${contentTop}) < stickyTopHeight(${stickyTopHeight})`, Tag.layoutUtils);
        }
        if(isBottomTruncated) {
            Log.d(`✂️ 节点 ${node.className} 底部截断: contentBottom(${contentBottom}) > viewportHeight-bottomHeight(${viewportHeight - stickyBottomHeight})`, Tag.layoutUtils);
        }

        const result = isTopTruncated || isBottomTruncated;
        Log.d(`${result ? '✂️ 存在截断' : '✅ 无截断'} (考虑吸顶吸底组件)`, Tag.layoutUtils);
        return result;
    }

    /**
     * 检测是否是带滚动条的节点
     * @param node 
     * @returns 
     */
    static isOverflowScroll(node: HTMLElement): boolean {
        const isScrollableY = this.hasScrollbar(node);
        
        // 检查内容是否溢出
        const isOverFlow = node.scrollHeight > node.clientHeight;
        
        return isScrollableY && isOverFlow;
    }

    static isOverflowScrollChild(node: HTMLElement, scrollNodes: HTMLElement[]): boolean {
        return scrollNodes.some(scrollNode => {
            return scrollNode.contains(node);
        });
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
        const hasCoverBg = computedStyle.backgroundImage !== 'none' && computedStyle.backgroundSize === 'cover';
        if (hasCoverBg) {
            Log.d(`🖼️ 节点 ${node.className || node.tagName} 包含cover背景图`, Tag.layoutUtils);
        }
        return hasCoverBg;
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
            LayoutValue.HEADER,
            LayoutValue.BOTTOM,
            LayoutValue.VERTICAL_GRID,
        ];
        return needRelayoutTags.includes(layoutOfHw);
    }

    static getVisibleChildren(root:HTMLElement):Array<Element> {
        return Array.from(root.children).filter((child:HTMLElement) => {
            if (!child || child.nodeType !== Node.ELEMENT_NODE) {
                return false;
            }
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
        if (!root || root.nodeType !== Node.ELEMENT_NODE) {
            return null;
        }
        const style = window.getComputedStyle(root);
        if (style.display === 'none') {
            return null;
        }
        
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
            const normalize = (val: number): number => 
                Math.round(val / tolerance) * tolerance;
        
            // 统计尺寸出现次数
            for (const child of visibleChildren) {
                const rect = child.getBoundingClientRect();
                const key = `${normalize(rect.width)}, ${normalize(rect.height)}`;
                sizeMap.set(key, (sizeMap.get(key) || 0) + 1);
            }
        
            // 检查是否存在重复尺寸
            if ([...sizeMap.values()].some(count => count >= 2)) {
                Log.d(`✅ 找到包含相同尺寸子元素的父节点: ${root.className || root.tagName}, 可见子元素数: ${visibleChildren.length}`, Tag.layoutUtils);
                return root;
            }
        }
    
        // 深度优先递归搜索
        for (const child of visibleChildren) {
            const foundEle = this.findParentWithEqualChildren(child as HTMLElement, tolerance);
            if (root !== foundEle && root.contains(foundEle)) {
                continue;
            }
            if (foundEle) {
                return foundEle;
            }
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
        Log.d(`========== 分析box-shadow ==========`, Tag.layoutUtils);
        
        // 过滤无效输入
        if (!computedBoxShadow || typeof computedBoxShadow !== 'string' || computedBoxShadow === 'none' || computedBoxShadow === '') {
            Log.d(`❌ 无效的box-shadow值: ${computedBoxShadow}`, Tag.layoutUtils);
            return null;
        }

        // 正则表达式，用于匹配颜色部分（无论在开头还是结尾）
        const colorRegex = /(rgba?\(.*?\)|hsla?\(.*?\)|#[\da-f]{3,8}|[a-z]+)/i;
        const colorMatch = computedBoxShadow.match(colorRegex);

        if (!colorMatch) {
            Log.d(`❌ 未找到颜色值`, Tag.layoutUtils);
            return null; // 未找到颜色值，无法解析
        }

        const color = colorMatch[0];

        // 从字符串中移除颜色部分，剩下的就是数值
        const numericsString = computedBoxShadow.replace(color, '').trim();
        const numericParts = numericsString.split(/\s+/).filter(Boolean);
        
        // 正常的阴影至少应包含2个数值（X和Y偏移）
        if (numericParts.length < 2) {
            Log.d(`❌ 数值部分不足: ${numericParts.length}`, Tag.layoutUtils);
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

        Log.d(`解析结果: offsetX=${analysis.offsetX}, offsetY=${analysis.offsetY}, blur=${analysis.blurRadius}, spread=${analysis.spreadRadius}, color=${color}`, Tag.layoutUtils);

        // --- 核心判断逻辑 ---
        const spreadPx = parseFloat(analysis.spreadRadius);
        const maxViewport = Math.max(window.innerWidth, window.innerHeight);
        
        // 判断条件：扩展半径是否大于等于视口宽高中的最大值
        const isSpreadLargeEnough = spreadPx >= maxViewport;

        // 综合判断
        const isMask =
            (analysis.offsetX).trim().startsWith('0') &&
            (analysis.offsetY).trim().startsWith('0') &&
            (analysis.blurRadius).trim().startsWith('0') &&
            isSpreadLargeEnough;

        Log.d(`${isMask ? '🎭 是全屏遮罩' : '❌ 非全屏遮罩'} (spread=${spreadPx}px, 视口max=${maxViewport}px)`, Tag.layoutUtils);
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
        Log.d(`========== 检测底部关闭按钮重叠 ==========`, Tag.layoutUtils);
        
        if (!popup || !popup.root_node) {
            Log.d(`❌ 无效的popup信息`, Tag.layoutUtils);
            return false;
        }
        // 查找所有可能的关闭按钮
        const closeElements = popup.root_node.querySelectorAll('[class*="close"]');
        Log.d(`找到候选关闭按钮数量: ${closeElements.length}`, Tag.layoutUtils);
        
        if (closeElements.length !== 1) {
            Log.d(`❌ 关闭按钮数量不为1`, Tag.layoutUtils);
            return false;
        }
        const closeButton = closeElements[0] as HTMLElement;
        const closeBtnStyle = getComputedStyle(closeButton);
        const closeBtnRect = closeButton.getBoundingClientRect();
        
        Log.d(`关闭按钮样式: position=${closeBtnStyle.position}, bottom=${closeBtnStyle.bottom}`, Tag.layoutUtils);
        
        if (this.isCloseButton(closeButton)) {
            // 如果是绝对布局，且bottom为负，则说明这个组件会溢出父布局，判断它和父布局是否完全脱离（是否重叠）
            if (closeBtnStyle.position === 'absolute' && parseFloat(closeBtnStyle.bottom) < 0) {
                const buttonTop = closeBtnRect.top;
                const parentTop = closeButton.parentElement.getBoundingClientRect().top;
                const parentBottom = closeButton.parentElement.getBoundingClientRect().bottom;
                
                Log.d(`位置信息: buttonTop=${buttonTop}, parentTop=${parentTop}, parentBottom=${parentBottom}`, Tag.layoutUtils);
                
                if (buttonTop < parentBottom && buttonTop > (parentTop + parentBottom) / 2) {
                    Log.d(`⚠️ 检测到底部关闭按钮重叠`, Tag.layoutUtils);
                    return true;
                }
            }
        }
        
        Log.d(`✅ 无底部关闭按钮重叠`, Tag.layoutUtils);
        return false;
    }

    static isCloseButton(element: HTMLElement): boolean {
        const closeBtnStyle = getComputedStyle(element);
        const hasBackgroundUrl = closeBtnStyle.background.includes('url');
        const isButtonRole = element.getAttribute('role') === 'button';

        if (hasBackgroundUrl || isButtonRole) {
            Log.d(`✅ 确认为关闭按钮: 背景图=${hasBackgroundUrl}, role=button ${isButtonRole}`, Tag.layoutUtils);
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
    static getVisualBoundingRectForAll(element: HTMLElement): {top: number ; bottom: number} | null {
        Log.d(`========== 获取全部子元素可视边界 ==========`, Tag.layoutUtils);
        
        if (!element || typeof element.getBoundingClientRect !== 'function') {
            Log.d(`❌ 无效的元素`, Tag.layoutUtils);
            return null;
        }
        const parentRect = element.getBoundingClientRect();
    
        // 1. 初始化边界：以父元素自身的边界为起点
        let minTop = parentRect.top;
        let maxBottom = parentRect.bottom;
        let nodeCount = 0;
    
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
            nodeCount++;

            currentNode = treeWalker.nextNode() as HTMLElement;
          }

        Log.d(`遍历节点数: ${nodeCount}, 最终边界: top=${minTop}, bottom=${maxBottom}`, Tag.layoutUtils);
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
         * @param {popupDecisionTreeType} PopupDecisionTreeType 弹窗决策树类型
         * @returns {object|null} 一个类似 DOMRect 的对象，如果元素无效则返回 null。
         */
    static getVisualBoundingRect(element: HTMLElement, isCloseButtonTruncatedByScroll: boolean,
        popupDecisionTreeType: PopupDecisionTreeType): VisualBoundingRect {
        Log.d(`========== 获取可视边界矩形 ==========`, Tag.layoutUtils);
        Log.d(`元素: ${element.className || element.tagName}, 关闭按钮被滚动截断: ${isCloseButtonTruncatedByScroll}, 决策树类型: ${popupDecisionTreeType}`, Tag.layoutUtils);
        
        if (!element || typeof element.getBoundingClientRect !== 'function') {
            Log.d(`❌ 无效的元素`, Tag.layoutUtils);
            return null;
        }

        const parentRect = element.getBoundingClientRect();
        let scrollElement: HTMLElement | null = null;
        let offsetY = 0;

        // 1. 初始化边界
        let minX = Number.MAX_SAFE_INTEGER;
        let minY = Number.MAX_SAFE_INTEGER;
        let maxX = Number.MIN_SAFE_INTEGER;
        let maxY = Number.MIN_SAFE_INTEGER;

        if (parentRect.height !== 0 && parentRect.width !== 0) {
            minX = parentRect.left;
            minY = parentRect.top;
            maxX = parentRect.right;
            maxY = parentRect.bottom;
            Log.d(`父元素边界: left=${minX}, top=${minY}, right=${maxX}, bottom=${maxY}`, Tag.layoutUtils);
        }

        /**
         * 处理带有滚动条的元素
         * @param el 当前元素
         */
        const handleScrollableElement = (el: HTMLElement): void => {
            scrollElement = el;
            Log.d(`📜 发现滚动元素: ${el.className || el.tagName}`, Tag.layoutUtils);
            
            if (!isCloseButtonTruncatedByScroll) {
                return;
            }
            
            const scrollDiff = el.scrollHeight - el.getBoundingClientRect().height;
            Log.d(`滚动差值: ${scrollDiff}`, Tag.layoutUtils);
            
            if (popupDecisionTreeType === PopupDecisionTreeType.Bottom) {
                offsetY -= scrollDiff;
                Log.d(`底部弹窗偏移: offsetY=${offsetY}`, Tag.layoutUtils);
            } else if (popupDecisionTreeType === PopupDecisionTreeType.Center || popupDecisionTreeType === PopupDecisionTreeType.Center_Button_Overlap) {
                offsetY -= scrollDiff / 2;
                Log.d(`居中弹窗偏移: offsetY=${offsetY}`, Tag.layoutUtils);
            }
        };

        /**
         * 为可见元素更新边界
         * @param el 当前元素
         */
        const updateBoundsForVisibleElement = (el: HTMLElement): void => {
            const rect = el.getBoundingClientRect();
            const style = window.getComputedStyle(el);

            const isElementVisible = rect.width > 0 && rect.height > 0 &&
                style.display !== 'none' && style.visibility !== 'hidden' &&
                parseFloat(style.opacity) > 0;

            if (!isElementVisible) {
                return;
            }

            const aspectRatio = rect.height / rect.width;
            if (aspectRatio > 0.1 && aspectRatio < 10) {
                minX = Math.min(minX, rect.left);
                minY = Math.min(minY, rect.top);
                maxX = Math.max(maxX, rect.right);
                maxY = Math.max(maxY, rect.bottom);
            }
        };

        /**
         * 递归函数，用于遍历并更新边界
         */
        const updateBounds = (el: HTMLElement, rootNode: HTMLElement): void => {
            // 卫语句：如果是滚动元素，特殊处理后直接返回
            if (this.hasScrollbar(el)) {
                handleScrollableElement(el);
                return;
            }

            // 卫语句：如果节点内容被隐藏，则跳过
            const style = window.getComputedStyle(el);
            if (el !== rootNode && style.overflowY === 'hidden' && el.scrollHeight > el.clientHeight) {
                return;
            }
            
            // 更新当前元素的边界
            updateBoundsForVisibleElement(el);

            // 递归遍历子节点
            for (const child of el.children) {
                if (child instanceof HTMLElement) {
                    updateBounds(child, rootNode);
                }
            }
        };

        // 遍历所有节点来更新边界
        updateBounds(element, element);

        // 计算最终的边界尺寸
        const width = maxX - minX;
        const height = maxY - minY;

        // 计算子元素超出父元素区域的偏差高度
        if (parentRect.height !== 0 && parentRect.width !== 0) {
            offsetY += (minY - parentRect.top + (height - parentRect.height) / 2);
        }

        Log.d(`最终边界: left=${minX}, top=${minY}, right=${maxX}, bottom=${maxY}`, Tag.layoutUtils);
        Log.d(`尺寸: width=${width}, height=${height}, offsetY=${offsetY}`, Tag.layoutUtils);
        Log.d(`滚动元素: ${scrollElement ? (scrollElement.className || scrollElement.tagName) : '无'}`, Tag.layoutUtils);

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
        
        // to do: 将 scrollHeight 综合考虑进去
        const isScrollableY = (overflowY === 'scroll' || overflowY === 'auto' );               
        return isScrollableY;
    }

    /**
     * 获取节点的层叠上下文链
     */
    static getStackingContextChain(node: HTMLElement): StackingContextInfo[] {
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
        if (element === document.documentElement) {
            return true;
        }
        // position + z-index ≠ auto
        if (style.position !== 'static') {
            if (style.zIndex !== 'auto') {
                return true;
            }
        }
        
        // 其他创建层叠上下文的情况
        if (parseFloat(style.opacity) < 1 ||
            style.transform !== 'none' ||
            style.filter !== 'none' ||
            style.isolation === 'isolate' ||
            style.mixBlendMode !== 'normal') {
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
        if (style.zIndex === 'auto') {
            return 0;
        }
        return parseInt(style.zIndex, 10) || 0;
    }

    /**
     * 比较两个可见节点的层叠顺序
     * @returns 1: nodeA在上, -1: nodeB在上, 0: 同一层级或无法比较
     */
    static compareZIndex(nodeA: HTMLElement, nodeB: HTMLElement): number{
        Log.d(`========== 比较z-index层级 ==========`, Tag.layoutUtils);
        
        if (nodeA === nodeB) {
            Log.d(`✅ 相同节点`, Tag.layoutUtils);
            return 0;
        }

        const chainA = LayoutUtils.getStackingContextChain(nodeA);
        const chainB = LayoutUtils.getStackingContextChain(nodeB);
        
        Log.d(`节点A层叠链长度: ${chainA.length}, 节点B层叠链长度: ${chainB.length}`, Tag.layoutUtils);
        
        // 找到第一个不同的层叠上下文祖先
        let i = 0;
        while (i < chainA.length && i < chainB.length) {
            const ctxA = chainA[i];
            const ctxB = chainB[i];
            
            if (ctxA.element !== ctxB.element) {
                // 比较这两个兄弟层叠上下文的z-index
                const result = ctxA.zIndex > ctxB.zIndex ? 1 : -1;
                Log.d(`${result === 1 ? '🔼 节点A在上' : '🔽 节点B在上'} (zA=${ctxA.zIndex}, zB=${ctxB.zIndex})`, Tag.layoutUtils);
                return result;
            }
            
            // 如果是同一个层叠上下文，继续向叶子节点比较
            i++;
        }
        
        // 如果到达这里，说明在同一个层叠上下文中
        // 比较它们在DOM中的顺序（后来者居上）
        Log.d(`同一层叠上下文，比较DOM顺序`, Tag.layoutUtils);
        const position = nodeA.compareDocumentPosition(nodeB);
        if (position & Node.DOCUMENT_POSITION_FOLLOWING) {
            Log.d(`🔽 节点B在上 (B在A后面)`, Tag.layoutUtils);
            return -1; // B在A后面，B在上
        } else if (position & Node.DOCUMENT_POSITION_PRECEDING) {
            Log.d(`🔼 节点A在上 (A在B后面)`, Tag.layoutUtils);
            return 1; // A在B后面，A在上
        }
        
        Log.d(`⚠️ 无法比较`, Tag.layoutUtils);
        return 0;
    }
}