/**
 * Module ModeUtils.ts
 * 工具模块
 */
import Tag from '../../../Debug/Tag';
import Log from '../../../Debug/Log';
import { CCMConfig } from '../CCMConfig';
import CacheStyleGetter from '../Style/Common/CacheStyleGetter';
import Store from './Store';
import Constant, { LayoutKey, LayoutValue } from '../Constant';

export default class Utils {
    static TAG = Tag.util;

    static hadCalledRelayoutHappen = false;

    static getHTMLElement(node: Node | HTMLElement): HTMLElement {
        return node instanceof HTMLElement ? node : node.parentElement;
    }

    /**
     * 根据华为开发者文档定义，判断给定的屏幕尺寸是否符合特定折叠屏设备的横向断点状态。
     * @returns {boolean} 如果尺寸匹配阔折叠(内外屏)、双折叠(内屏)、三折叠(M/G态)的横向断点状态，则返回 true；否则返回 false。
     * @description
     * 此函数的数据源于 HarmonyOS 设计规范，用于识别特定的设备形态，以便应用进行针对性布局。
     * 数据参考自：https://developer.huawei.com/consumer/cn/doc/best-practices/bpta-multi-device-responsive-layout#section186821126131515
     * * 横向断点状态定义 (以屏幕方向正向为例，宽度 < 高度):
     * - 阔折叠:内屏：707(高) * 440(宽),宽高比=0.6223, 外屏：326(高) * 326(宽),宽高比=1, 外屏最小：200(高) * 326(宽),宽高比=1.63
     * - 双折叠 内屏:782(高)*717(宽),宽高比=0.917 , 内屏：798 * 711(宽)，,宽高比=0.891
     * - 三折叠 M态: 776 * 712(宽),宽高比=0.918
     * - 三折叠 G态: 776 * 1107(宽),宽高比=1.423
     */
    static isWideScreen(): boolean {
        const screenHeight = screen.availHeight;
        const screenWidth = screen.availWidth;
        // 防止除以0的错误
        if (screenHeight === 0) {
            return false;
        }
        const config = CCMConfig.getInstance();
        const r = config.findAspectRatioRange(screenWidth);
        if(r === undefined) {
            return false;
        }

        // Todo: 增加屏幕size变化感知，减少重复计算
        const aspectRatio = screenWidth / screenHeight ;
        if(aspectRatio > r.min && aspectRatio < r.max) {
            return true;
        }
        return false;
    }

    static setParentTag(
        parent: HTMLElement | null,
        key: string = LayoutKey.LAYOUT_TAG,
        from: string = LayoutValue.ZOOM_PARENT
    ): void {
        if (!parent) {
            return;
        }
        if (Store.getValue(parent, key)) {
            return;
        }
        Store.setValue(parent, key, from);
        this.setParentTag(parent.parentElement, key, from);
    }

    static hadRelayOut(ele: HTMLElement): boolean {
        return Store.getValue(ele, LayoutKey.LAYOUT_TAG) != null;
    }

    static hadZoomRelayout(ele: HTMLElement): boolean {
        return ![null, LayoutValue.ZOOM_PARENT].includes(Store.getValue(ele, LayoutKey.LAYOUT_TAG));
    }

    static hadConfig(ele: HTMLElement): boolean {
        return Store.getValue(ele, LayoutKey.CONFIG_LAYOUT_TAG) != null;
    }

    static timesBetween(a: number, b: number, times: number): boolean {
        return a / b < times && b / a < times;
    }

    static getInterval(A: [number, number], B: [number, number]): number {
        if (A[0] > A[1] || B[0] > B[1]) {
            return null;
        }
        // A,B相离且A[aIndex]的最大值小于B[bIndex]最小值
        if (A[1] < B[0] || A[0] > B[1]) {
            return 0;
        }
        return Math.min(A[1], B[1]) - Math.max(A[0], B[0]);
    }

    // 判断MapA中是否包含MapB中的全部内容
    static MapAContainsMapB(mapA: Map<string, string>, mapB: Map<string, string>): boolean {
        for (const [attr, valueB] of mapB.entries()) {
            let valueA = mapA.get(attr);
            if (valueA !== valueB) {
                // 如果MapB中key的值为delete且MapA中对应为空时，认为符合contains条件
                if (valueB !== Constant.delete || valueA) {
                    return false;
                }
            }
        }
        return true;
    }

    static isZoomParent(ele: HTMLElement): boolean {
        return Store.getValue(ele, LayoutKey.LAYOUT_TAG) === LayoutValue.ZOOM_PARENT;
    }

    static hadZoomLayout(ele: HTMLElement): boolean {
        const layoutTag = Store.getValue(ele, LayoutKey.LAYOUT_TAG);
        return layoutTag != null && layoutTag !== LayoutValue.ZOOM_PARENT;
    }

    static hadStretchedWidth(ele: HTMLElement): boolean {
        const layoutTag = Store.getValue(ele, LayoutKey.LAYOUT_TAG);
        return [
            LayoutValue.ACROSS_COLUMN,
            LayoutValue.ONE_LINE,
            LayoutValue.EMPTY,
            LayoutValue.COMMENT,
            LayoutValue.SCROLL_LIST,
        ].includes(layoutTag);
    }

    static hadZoomConfig(ele: HTMLElement): boolean {
        const layoutTag = Store.getValue(ele, LayoutKey.CONFIG_LAYOUT_TAG);
        return layoutTag != null && layoutTag !== LayoutValue.ZOOM_PARENT;
    }

    static isInvisibleElement(ele: HTMLElement): boolean {
        const display = CacheStyleGetter.computedStyle(ele, Constant.display);
        if (display === Constant.none) {
            return true;
        }

        const visibility = CacheStyleGetter.computedStyle(ele, Constant.visibility);
        if (visibility === Constant.hidden) {
            return true;
        }
        // 宽度为0和高度为0的元素需要跳过吗？如果它的子节点存在absolute布局的情况如何处理？

        const opacity = CacheStyleGetter.computedStyle(ele, Constant.opacity);
        if (opacity === '0') {
            return true;
        }

        return false;
    }

    static getAvailWidth(): number {
        Log.d('使用screen.availWidth');
        return screen.availWidth;
    }

    static ignoreEle(ele: HTMLElement): boolean {
        const ignoreList: string[] = [];//HWCfgReader.cfg.ignoreChange;
        if (!ignoreList.length) {
            return false;
        }
        if (!ele.className) {
            return false;
        }

        for (let i = 0; i < ele.classList.length; i++) {
            if (ignoreList.includes(ele.classList[i])) {
                return true;
            }
        }
        return false;
    }

    static ignoreLocalName(ele: HTMLElement, extraTag: string[] = []): boolean {
        const ignoreList = [Constant.script, Constant.style, Constant.swiperBox, Constant.noscript].concat(extraTag);
        const localName = CacheStyleGetter.localName(ele);
        return ignoreList.includes(localName) || localName.startsWith(Constant.back);
    }

    static callRelayoutHappen(): void {
        if (Utils.hadCalledRelayoutHappen) {
            return;
        }

        Utils.hadCalledRelayoutHappen = true;
    }

    // 判断是否有文本（文本不为null 且 不为空白文本）
    static isEmptyText(text: string): boolean {
        if (!text) {
            return true;
        }
        return !/[^\t\n\r ]/.test(text);
    }

    static paddingFillHeight(ele: HTMLElement): boolean {
        const paddingTop = parseFloat(CacheStyleGetter.computedStyle(ele, Constant.paddingTop));
        const paddingBottom = parseFloat(CacheStyleGetter.computedStyle(ele, Constant.paddingBottom));

        if (paddingTop === 0 && paddingBottom === 0) {
            return false;
        }

        if (paddingTop && paddingBottom) {
            return false;
        }

        // offsetHeight的处理逻辑是四舍五入的
        // 对paddingTop和paddingBottom也进行四舍五入
        const paddingHeight = Math.round(paddingTop || paddingBottom);

        // 理论上当一个元素是通过百分比padding撑开
        // 那么高度的值一定要paddingHeight相等
        if (CacheStyleGetter.offsetH(ele) !== paddingHeight) {
            return false;
        }

        return true;
    }

    /**
     * 恢复左侧边距，使其相对位置不变
     * @param ele
     * @param zoomScale
     */
    static restoreLeftMarginSpace(ele: HTMLElement, zoomScale: number): void {
    }

    /**
     * 取两个边界之间的值，超出边界取边界
     * 第2、3个参数没有大小顺序要求
     * @param num
     * @param boundary1
     * @param boundary2
     */
    static getNumBetween(num: number, boundary1: number, boundary2: number): number {
        const max = Math.max(boundary1, boundary2);
        const min = Math.min(boundary1, boundary2);
        return Math.min(Math.max(num, min), max);
    }

    // 检查内部图片是否都加载完毕
    static isImgCompleted(element: HTMLElement): boolean {
        return true;
    }

    static hasSameSize(size1: Size, size2: Size): boolean {
        return false;
    }
}

export class Size {
    width: number;
    height: number;

    static createByEle(ele: HTMLElement): Size {
        const width = CacheStyleGetter.offsetW(ele);
        const height = CacheStyleGetter.offsetH(ele);
        return Size.createBySize(width, height);
    }

    static createBySize(width: number, height: number): Size {
        const size = new Size();
        size.width = width;
        size.height = height;
        return size;
    }
}
