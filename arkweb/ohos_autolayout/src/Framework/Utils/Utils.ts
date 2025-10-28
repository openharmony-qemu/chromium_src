import extUtils from '../Common/Utils/Utils' 
import Log from '../../Debug/Log';
import Tag from '../../Debug/Tag';
import LayoutUtils from './LayoutUtils';
import { CCMConfig } from '../Common/CCMConfig';
import ModifyObserver from '../Observer/Observers/ModifyObserver';
import ObserverHandler from '../Observer/ObserverHandler';
export default class Utils {
    private static TAG = Tag.util;

    static isImageCanSafetyStrech(element:HTMLElement): boolean {
        // 类型1：CSS背景图容器
        const bgImage = getComputedStyle(element).backgroundImage;
        if (bgImage && bgImage.includes('linear-gradient')) {
            return true 
        };
        return false;
    }

    static isPureText(node:HTMLElement): boolean {
        if (node.nodeType === Node.TEXT_NODE) {
            return true; // 是文本节点
        }

        // 遍历单子节点,如果子节点只有1个，且是纯文本节点，放回true
        return (node.childNodes.length === 1 && node.firstChild.nodeType === Node.TEXT_NODE);
    }
    
    /**
     * 判断是否为图片容器（支持所有已知类型）
     * @param {Element} element 
     * @returns {boolean}
     */
    static isImageContainer(element:HTMLElement): boolean {
        // 类型1：原生IMG标签
        if (element.tagName === 'IMG') {
            return true
        };
    
        // 类型2：ARIA语义容器
        if (element.getAttribute('role') === 'img') {
            return true 
        };
    
        // 类型3：CSS背景图容器
        const bgImage = getComputedStyle(element).backgroundImage;
        if (bgImage && bgImage !== 'none' && bgImage !== 'initial') {
            return true;
        }

        // 类型4：CSS Mask图容器
        const maskImage = getComputedStyle(element).webkitMaskImage;
        if (maskImage && maskImage !== 'none' && maskImage.includes('url')) {
            return true;
        }

        // 类型5：响应式图片容器中的SOURCE标签
        if (element.tagName === 'SOURCE' && 
            element.parentElement?.tagName === 'PICTURE') {
            return true;
        }
    
        // 类型6：SVG图片元素
        if (element.tagName === 'image' && 
            element.namespaceURI === 'http://www.w3.org/2000/svg') {
            return true;
        }
    
        // 类型7：Canvas画布（需自定义属性标记）
        if (element.tagName === 'CANVAS' && 
            element.dataset.imageRole === 'container') {
            return true;
        }
    
        // 类型9：伪元素图片容器（通过类名标记）
        if (element.classList.contains('pseudo-image-container')) {
            return true;
        }
    
        // 类型10：框架容器
        if (element.tagName === 'IFRAME' && 
            element.dataset.role === 'image-viewer') {
            return true;
        }

        // 类型11：lottie-img
        if (element.classList.contains('a-lottie')) {
            return true;
        }
    
        return false;
    }

    /**
     * 判断是否是一个object
     * @param source 原始节点
     * @returns 
     */
    static isObjectNode(source:HTMLElement) : boolean {
        return source.tagName === 'object';
    }
    
    static copyEventListeners(source:HTMLElement, target:HTMLElement) : HTMLElement {
        // @ts-ignore
        const events = getEventListeners(source);
        for (const eventType in events) {
            // @ts-ignore
            events[eventType].forEach((listener) => {
                target.addEventListener(eventType, listener.listener);
            });
        }
        // 遍历子节点
        const sourceChildren = source.children;
        const targetChildren = target.children;
        for (let i = 0; i < sourceChildren.length; i++) {
            Utils.copyEventListeners(sourceChildren[i] as HTMLElement, targetChildren[i] as HTMLElement);
        }
        return target as HTMLElement;
    }


    /**
     * 判断子节点是否溢出父容器，且造成截断，显示不全
     * @param {HTMLElement} node, {HTMLElement} root - 根节点
     * @returns {boolean}
     */
    static isOverFlowHidden(node:HTMLElement, root:HTMLElement): boolean{
        while (node && node !== root) {
            const style = getComputedStyle(node);
            if(style.overflow === 'hidden') {
                Log.d(`${node?.className} overflow为hidden`, Utils.TAG);
                return true;
            }
            node = node.parentElement;
        }
        return false;
    }

    /**
     * 在树形结构中修剪共同祖先节点。
     * 
     * 该函数接收一个包含 HTML 元素的集合 `candidates` 和一个根 HTML 元素 `root`。
     * 它会遍历树形结构，查找 `candidates` 集合中元素的共同祖先节点，并将这些共同祖先节点从结果中移除。
     * 最后返回一个新的 HTML 元素集合，该集合不包含被移除的共同祖先节点。
     * 需要注意的是：
     * 1、脱离文档流的节点不参与其中
     * 2、如果节点是包含关系，则保留父节点
     * 
     * @param {Map<HTMLElement,boolean>} candidates - 候选节点元素的集合，包含共同祖先节点，需要从寻找出来删除。
     * @param {HTMLElement} root - 树形结构的根 HTML 元素，作为查找共同祖先节点的结束点。
     * @returns {Map<HTMLElement,boolean>} - 一个新的 HTML 元素集合，该集合不包含 `candidates` 中元素的共同祖先节点。
     */
    static pruneCommonAncestorsInTree(candidates:Map<HTMLElement,boolean>,root:HTMLElement):Map<HTMLElement,boolean> {
        const removeNodeSet = new Set();
        removeNodeSet.add(root);
        for(const [node,outOfFlow] of candidates) {
            // 过滤脱离文档流节点
            if(outOfFlow) {
                continue;
            }
            let parent = node.parentElement;
            while (parent && parent !== root) {
                if(candidates.has(parent)) {
                    removeNodeSet.add(parent);
                }
                parent = parent.parentElement;
            }
        };
        // 删除父节点，保留子节点
        removeNodeSet.forEach((item:HTMLElement) => candidates.delete(item));
        return candidates;
    }

    static getValidChildNodeLength(root:HTMLElement):number {
        let count = 0;
        if(root && root.childNodes) {
            root.childNodes.forEach( (node:HTMLElement) => {
                if(!Utils.shouldSkip(node)){
                    count += 1;
                }
            });
        }
        return count;
    }
    /**
     * 在轮播图中查找indicator节点
     * 
     * @param {HTMLElement} root - Swiper Container的父HTML 元素，indicator的父节点。
     * @returns {HTMLElement} - 找到的indicator。
     */
    static getSwiperIndicator(root:HTMLElement):HTMLElement {
        if(root && root.childNodes) {
            const indicators = root.querySelectorAll('[class*="indicator"]');
            if(indicators && (indicators.length === 1)) {
                return indicators[0] as HTMLElement;
            }
        }
        return null;
    }


    static findVisibleImageNode(item:HTMLElement): HTMLElement {
        while(Utils.getValidChildNodeLength(item) !== 0) {
            for (const it of item.childNodes) {
                const imgNode = it as HTMLElement;
                if(Utils.shouldSkip(imgNode)) {
                    continue;
                }
                if(Utils.isImageContainer(imgNode)){
                    return imgNode;
                }
                return Utils.findVisibleImageNode(imgNode);
            }
        }
        return null;
    }

    // 跳过不需要处理的元素
    static shouldSkip(ele: HTMLElement) :boolean {
        if (!ele || ele.nodeType !== Node.ELEMENT_NODE) {
            return true;
        }
        if (extUtils.ignoreLocalName(ele)) {
            Log.d(`${ele?.className} 非法标签名`, Utils.TAG);
            return true;
        }

        if (!LayoutUtils.canBeRelayout(ele)) {
            Log.d(`${ele?.className} 元素已被调整过`, Utils.TAG);
            return true;
        }

        if (extUtils.isInvisibleElement(ele)) {
            Log.d(`${ele?.className} 跳过不可见节点`, Utils.TAG);
            return true;
        }

        return false;
    }

    /**
     * 检查指定的 HTML 元素在给定根元素的树形结构中是否可见，综合考虑元素自身的样式属性（如 display、visibility 等）以及它在根元素树形结构中的位置等因素来判断元素的可见性。。这个函数中最复杂的部分在于，它要往根节点进行回溯，可能会碰到以下情况
     * Case 1: root->a(blcok)->b(blcok)->c(blcok)->node,最普遍的情况
     * Case 2: root->a(blcok)->b(absolute/Fixed/sticky)->c(relative)->node，它要脱离文档流
     * Case 3: root->a(block)->b(hidden)->c(block)->node ，祖先节点不可见，它可以传递
     * Case 4: root->a(block)->b(block小)->c(relative)->node(被放大) ，最复杂情况，元素溢出了父容器，造成截断
     * 
     * @static
     * @param {HTMLElement} node - 要检查可见性的目标 HTML 元素。该元素应该是根元素树形结构中的一个节点。
     * @param {HTMLElement} root - 作为检查范围的根 HTML 元素。函数会在以这个根元素为起点的树形结构内进行可见性检查。
     * @returns {boolean} - 如果目标元素在根元素的树形结构中是可见的，则返回 true；否则返回 false。
     */
    static isElementVisible(node:HTMLElement, root:HTMLElement): boolean {
        // 步骤1: 快速排除
        if (!node || node.nodeType !== Node.ELEMENT_NODE) {
            return false;
        }
        // 最后合并后续节点时，会将合并body节点
    
        // 步骤2: 当前节点基础隐藏检测
        const style = getComputedStyle(node);
        if (style.display === 'none' || 
            style.visibility === 'hidden' || 
            style.opacity === '0') {
                Log.d(`${node?.className} 样式为不可见`, Utils.TAG);
                return false;
            } 
        
        // 步骤3: 自身物理尺寸检测
        const rect = node.getBoundingClientRect();
        if (rect.width === 0 || rect.height === 0) {
            Log.d(`${node?.className} 宽/高为0`, Utils.TAG);
            return false;
        }
    
        // 步骤4: 处理position，它与整个祖先节点的样式都有关系
        const parent = node.parentElement;
        switch(style.position) {
            // 步骤4.1: 脱离文档流，固定定位元素,祖先节点的overflow会影响子节点
            //需检查祖先的 overflow 和尺寸，即使元素未溢出父容器，若祖先链中某层设置 overflow: hidden，仍会被裁剪。
            case 'absolute':
                // 需要关注父节点在不可见的情况下，是否存在overflow为hidden的情况
                // 这里可以使用Cache优化性能，避免重复遍历
                if(Utils.isOverFlowHidden(parent, root)) {
                    return false;
                }
                break;
            case 'fixed': 
                //相对于视口定位，不受父容器 overflow 影响,但是受祖先容器的visibility影响，由于之前已经算过本节点的可见性（继承而来）了，所以此处无需再次计算
            case 'sticky':
                //在特定滚动阈值内表现为相对定位，超出后表现为固定定位，需结合滚动位置判断可见性,todo FIXME
                break;
            // 步骤4.2: 未脱离文档流，祖先节点的
            default:
                break;
        }
        // 步骤5: 定位上下文检测

        return true;
    }

    static isElementInViewport(node:HTMLElement):boolean {
        if(!node) {
            return false;
        }
        const rect = node.getBoundingClientRect();
        return (
          rect.width > 0 && rect.height > 0 &&
          rect.top < window.innerHeight &&
          rect.bottom > 0 &&
          rect.left < window.innerWidth &&
          rect.right > 0
        );
    }

     /**
     * 计算元素的DOM深度
     * @param {Element} el - 目标元素
     * @returns {number}
     */
    static getElementDepth(el: Element): number {
        if(!el) {
            return 0;
        }
        let depth = 0;
        let current: Element | null = el;
        while (current.parentElement) {
            depth++;
            current = current.parentElement;
        }
        return depth;
    }

    static zIndexToNumber(value: string): number {
        if (!value) {
            return 0;
        }
        // 首先检查字符串是否为 'auto'（忽略大小写）
        if (value.toLowerCase() === 'auto') {
            return 0;
        }

        // 如果不是 'auto'，则尝试将其转换为整数
        // 第二个参数 10 表示使用十进制进行解析
        const numberValue = parseInt(value, 10);

        // 如果解析失败（例如，输入是 "hello"），parseInt会返回 NaN (Not a Number)
        // 你可以根据需要处理这种情况，比如返回 0 或抛出错误
        return isNaN(numberValue) ? 0 : numberValue;
    }

    /**
     * 从CSS颜色字符串中提取alpha通道值。
     * @param {string} colorValue - CSS颜色字符串 (e.g., "rgba(0, 0, 0, 0.5)", "#ff0000", "transparent")
     * @returns {boolean} - 是否是透明。
     */
    static isColorTransparent(colorValue: string): boolean {
        if (!colorValue) {
            return false;
        }
        colorValue = colorValue.toLowerCase().trim();
        
        // 注意：纯透明不算
        if (colorValue === 'transparent') {
            return true;
        }
        
        // RGBA 透明度检测,(e.g., "rgba(0, 0, 0, 0.5)")
        // HSLA 透明度检测,(e.g.,  hsla(240, 100%, 50%, 0.3); /* 蓝色30%透明度 */)
        if (colorValue.startsWith('rgba') || colorValue.startsWith('hsla')) {
            const alpha = parseFloat(colorValue.split(',')[3]);
            // 有alpha通道且值小于1,大于0
            return !isNaN(alpha) && alpha === 0;
        }
        
        // HEX 带透明度 (#RRGGBBAA/#RGBA)
        if (colorValue.startsWith('#') && (colorValue.length === 5 || colorValue.length === 9)) {
            const hex = colorValue.slice(1);
            // #RGBA 格式
            if (hex.length === 4) {
                const alpha = parseInt(hex[3] + hex[3], 16) / 255;
                return alpha === 0;
            }
            // #RRGGBBAA 格式
            if (hex.length === 8) {
                const alpha = parseInt(hex.slice(6, 8), 16) / 255;
                return alpha === 0;
            }
        }
        
        return false;
    }

    /**
     * 从CSS颜色字符串中提取alpha通道值。
     * @param {string} colorValue - CSS颜色字符串 (e.g., "rgba(0, 0, 0, 0.5)", "#ff0000", "transparent")
     * @returns {boolean} - 是否是半透明。
     */
    static isColorSemiTransparent(colorValue: string): boolean {
        if (!colorValue) {
            return false;
        }
        colorValue = colorValue.toLowerCase().trim();
        const opacityRange = CCMConfig.getInstance().getOpacityFilter();
        
        // 注意：纯透明不算
        if (colorValue === 'transparent') {
            return false;
        }
        // RGBA 透明度检测,(e.g., "rgba(0, 0, 0, 0.5)")
        // HSLA 透明度检测,(e.g.,  hsla(240, 100%, 50%, 0.3); /* 蓝色30%透明度 */)
        if (colorValue.startsWith('rgba') || colorValue.startsWith('hsla')) {
            const alpha = parseFloat(colorValue.split(',')[3]);
            // 有alpha通道且值小于1,大于0
            return !isNaN(alpha) && alpha > opacityRange[0] / 100 && alpha < opacityRange[1] / 100;
        }
        
        // HEX 带透明度 (#RRGGBBAA/#RGBA)
        if (colorValue.startsWith('#') && (colorValue.length === 5 || colorValue.length === 9)) {
            const hex = colorValue.slice(1);
            // #RGBA 格式
            if (hex.length === 4) {
                const alpha = parseInt(hex[3] + hex[3], 16) / 255;
                return alpha > opacityRange[0] / 100 && alpha < opacityRange[1] / 100;
            }
            // #RRGGBBAA 格式
            if (hex.length === 8) {
                const alpha = parseInt(hex.slice(6, 8), 16) / 255;
                return alpha > opacityRange[0] / 100 && alpha < opacityRange[1] / 100;
            }
        }
        
        return false;
    }

    /**
     * 检测渐变背景是否包含半透明
     */
    static hasSemiTransparentGradient(gradientValue: string): boolean {
        if (!gradientValue || gradientValue === 'none') {
            return false;
        }
        // 注意：纯透明不算
        if (gradientValue.includes('transparent')) {
            return false;
        }
        // 检查RGBA/HSLA透明
        if (/rgba$$.+?$$|hsla$$.+?$$/i.test(gradientValue)) {
            return true;
        }
        return false;
    }

    /**
     * 检测滤镜是否包含半透明度效果
     */
    static hasSemiTransparentFilter(filterValue: string): boolean {
        if (!filterValue) {
            return false;
        }
        // 检测透明度滤镜
        if (/opacity$$[^)]*$$/i.test(filterValue)) {
            const match = filterValue.match(/opacity$$([^)]+)$$/);
            const opacityRange = CCMConfig.getInstance().getOpacityFilter();
            if (match) {
                const alpha = parseFloat(match[1].trim());
                // 值小于100%表示透明度效果
                return alpha > opacityRange[0] / 100 && alpha < opacityRange[1] / 100;
            }
        }
        return false;
    }

    /**
     * 判断元素背景是否存在半透明效果
     * 检测范围包括:
     * - 透明背景色 (rgba/hsla/transparent) CSS颜色字符串 (e.g., "rgba(0, 0, 0, 0.5)", "#ff0000", "transparent")
     * - 透明渐变 (gradients with transparency)
     * - 混合模式 (mix-blend-mode)
     * - 滤镜透明度 (filter: opacity())
     * - 背景滤镜 (backdrop-filter)
     * - 遮罩 (mask)
     * - 元素整体透明度 (opacity < 1)
     */
    static isBackgroundSemiTransparent(style:  CSSStyleDeclaration): boolean {
        if(!style) {
            return false;
        }
        // 2. 检查背景颜色透明度,98%是这种情况,即使opacity是1，也不影响背景透明
        if (Utils.isColorSemiTransparent(style.backgroundColor)) {
            return true;
        }

        // 2. 检查元素整体透明度
        const op = parseFloat(style.opacity);
        const opacityRange = CCMConfig.getInstance().getOpacityFilter();
        if ( op > opacityRange[0] / 100 && op < opacityRange[1] / 100) {
            return true;
        }
        // 3. 检查背景图片中的渐变透明
        if (Utils.hasSemiTransparentGradient(style.backgroundImage)) {
            return true;
        }
        // 4. 检查滤镜透明度
        if (Utils.hasSemiTransparentFilter(style.filter)) {
            return true;
        }
        // 5. 检查背景滤镜（毛玻璃效果）
        if (style.backdropFilter !== 'none') {
            return true;
        }
        // 6. 检查混合模式（可能产生透明叠加效果）
        if (/overlay|multiply|screen|soft-light/.test(style.mixBlendMode)) {
            return true;
        }
        // 7. 检查CSS遮罩
        if (style.mask !== 'none') {
            return true;
        }
        return false;
    }

    /**
     * 检查元素是否在视口中基本可见
     * @param {Element | null} el - 要检查的元素
     * @returns {boolean}
     */
    static isElementVisibleInViewPort(el: Element | null): el is Element {
        if (!el) {
            return false;
        }
        const style = getComputedStyle(el);
        if (style.display === 'none' || style.visibility === 'hidden' || parseFloat(style.opacity) === 0) {
            Log.d(`isElementVisibleInViewPort ${(el as HTMLElement).className} display:${style.display}, style: ${style.display} ,opacity:${style.opacity}`);
            return false;
        }
        const rect = el.getBoundingClientRect();
        return rect.width > 0 && rect.height > 0;
    }

    /**
     * 计算元素占视口的面积比例
     * @param {Element} el - 要计算的元素
     * @returns {number} - 比例值 (0 to 100)
     */
    static getScreenAreaRatio(el: Element): number {
        if (!el) {
            return 0;
        }
        const rect = el.getBoundingClientRect();
        const viewportWidth = window.innerWidth;
        const viewportHeight = window.innerHeight;
        const intersectionArea = Math.max(0, Math.min(rect.right, viewportWidth) - Math.max(rect.left, 0)) *
                            Math.max(0, Math.min(rect.bottom, viewportHeight) - Math.max(rect.top, 0));
        const result = intersectionArea / (viewportWidth * viewportHeight)*100;
        return result;
    }

    /**
     * 递归检查元素内部是否包含关闭按钮
     * @param {Element | null} el - 要搜索的根元素
     * @returns {boolean}
     */
    static hasCloseButton(el: Element | null): boolean {
        if (!el) {
            return false;
        }
        try {
            const classList = el.classList ? Array.from(el.classList).join(' ').toLowerCase() : '';
            const style = window.getComputedStyle(el);
            const bgImage = style.backgroundImage.toLowerCase();
            if (CCMConfig.getInstance().getcloseButtonPattern()?.some(kw => classList.includes(kw) || bgImage.includes(kw))) {
                return true;
            }
            for (const child of Array.from(el.children)) {
                if (Utils.hasCloseButton(child)) {
                    return true;
                }
            }
        } catch (e) {
            // 忽略无法处理的元素
        }
        return false;
    }

    /**
     * 检查元素是否是按钮
     * @param {Element | null} el - 要搜索的元素
     * @returns {boolean}
     */
    public static hasButton(el: Element | null): boolean {
        if (!el) {
            return false;
        }
        
        try {
            const classList = el.classList ? Array.from(el.classList).join(' ').toLowerCase() : '';
            if (CCMConfig.getInstance().getButtonPattern()?.some(kw => classList.includes(kw))) {
                return true;
            }
        } catch (e) {
            // 忽略无法处理的元素
        }
        return false;
    }

    /**
     * 查找指定节点的可见兄弟节点。
     * @param {Element} node - 要查找其兄弟节点的DOM元素。
     * @returns {Element[]} - 可见的兄弟节点数组。
     */
    static getVisibleSiblings(node:Element): Element[] {
        Log.d(`========== 查找可见兄弟节点 ==========`, Utils.TAG);
        Log.d(`目标节点: ${(node as HTMLElement).className || node.tagName}`, Utils.TAG);
        
        // 确保节点及其父节点存在
        if (!node || !node.parentNode) {
            Log.d(`❌ 节点或父节点不存在`, Utils.TAG);
            return [];
        }

        const allChildren = Array.from(node.parentNode.children);
        Log.d(`父节点总子元素数: ${allChildren.length}`, Utils.TAG);

        let selfCount = 0;
        let invisibleCount = 0;
        let visibleCount = 0;

        const visibleSiblings = allChildren.filter(el => {
            // 排除节点自身
            if (el === node) {
                selfCount++;
                return false;
            }
            
            const style = getComputedStyle(el);
            // 注意：如果找到的兄弟节点的opacity为0，可能是动画开始前的状态，所以需要检查它的AnimationDuration
            if (style.display !== 'none' && style.visibility !== 'hidden' && parseFloat(style.opacity) === 0) {
                // 检查动画持续时间
               const animationDuration = ModifyObserver.getDurationFromElement(el as HTMLElement);
               if (animationDuration > 0) {
                   Log.d(`存在动画节点：: ${(el as HTMLElement).className || el.tagName} (display=${style.display}, visibility=${style.visibility}, opacity=${style.opacity})，动画时长: ${animationDuration}ms`, Utils.TAG);
                   ObserverHandler.postTask();
               }
            }
            const isVisible = style.display !== 'none' && 
                style.visibility !== 'hidden' && 
                parseFloat(style.opacity) > 0;
            if (!isVisible) {
                invisibleCount++;
                Log.d(`  ⚫ 不可见兄弟: ${(el as HTMLElement).className || el.tagName} (display=${style.display}, visibility=${style.visibility}, opacity=${style.opacity})`, Utils.TAG);
                return false;
            }
            
            visibleCount++;
            Log.d(`  ✅ 可见兄弟: ${(el as HTMLElement).className || el.tagName}`, Utils.TAG);
            return true;
        });

        Log.d(`统计结果: 自身=${selfCount}, 不可见=${invisibleCount}, 可见=${visibleCount}`, Utils.TAG);
        Log.d(`${visibleSiblings.length > 0 ? '✅' : '❌'} 找到 ${visibleSiblings.length} 个可见兄弟节点`, Utils.TAG);
        
        return visibleSiblings;
    };

    static visualFilter(el :Element) : boolean {
        if(!el) {
            return false;
        }
        const style = getComputedStyle(el);
        // 兄弟节点看不见
        if((el as HTMLElement).offsetWidth === 0 || (el as HTMLElement).offsetHeight === 0 ||
                style.display === 'none' || style.visibility === 'hidden' || parseFloat(style.opacity) === 0) {
            return false;
        }
        return true;
    }

    /**
     * 从蒙版节点开始，向上查找弹窗的主体内容节点。
     * @param {Element} maskNode - 弹窗的蒙版DOM节点。
     * @param {number} maxDepth - 向上查找的最大层数。
     * @returns {[Element[], Element]} - 找到的可见主体内容节点数组与根节点，未找到则返回空数组。
     */
    static findSiblingContent(maskNode:Element, maxDepth = 3):[Element[], Element] {
        // 步骤 1: 检查 maskNode 当前层级的可见兄弟节点
        const initialSiblings = Utils.getVisibleSiblings(maskNode);
        if (initialSiblings.length > 0) {
            Log.d(`在初始层级找到了 ${initialSiblings.length} 个主体节点。`);
            return [initialSiblings, maskNode.parentNode as Element];
        }

        // 步骤 2: 检查是否满足向上搜索的条件
        // 条件: 没有可见的兄弟节点，并且自身没有子节点
        if (maskNode.children.length > 0) {
            Log.d('蒙版节点没有可见兄弟节点，但存在子节点，停止搜索');
            return [[], null]; // 不满足向上搜索的条件，直接返回空数组
        }

        // 步骤 3: 如果满足条件，则开始向上循环查找
        Log.d('蒙版节点没有可见兄弟节点和子节点，开始向上搜索...');
        let currentNode = maskNode.parentNode as HTMLElement;
        let currentDepth = 1;

        while (currentNode && currentNode !== document.body && currentDepth <= maxDepth) {
            const siblings = Utils.getVisibleSiblings(currentNode);

            // 如果在当前层级找到了可见的兄弟节点，则立即返回它们
            if (siblings.length > 0) {
                Log.d(`在向上第 ${currentDepth} 层找到了 ${siblings.length} 个主体节点。`);
                return [siblings, currentNode.parentNode as Element];
            }

            // 继续向上
            currentNode = currentNode.parentNode as HTMLElement;
            currentDepth++;
        }

        // 如果循环结束仍未找到，返回空数组
        Log.d(`向上查找了 ${maxDepth} 层，未找到任何可见的主体节点。`);
        return [[], null];
    };

}