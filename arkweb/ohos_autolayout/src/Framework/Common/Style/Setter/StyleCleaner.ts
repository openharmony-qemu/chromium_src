import Log from '../../../../Debug/Log';
import Tag from '../../../../Debug/Tag';
import StyleCommon from '../Common/StyleCommon';
import Store from '../../Utils/Store';
import { LayoutKey, LayoutValue } from '../../Constant';
import Cached from '../../Cached';
import StyleSetter from './StyleSetter';
import Utils from '../../Utils/Utils';
import ObserverHandler from '../../../Observer/ObserverHandler';
import Constant from '../../Constant';
 
export default class StyleCleaner {
    static removeAllStyle(ele: HTMLElement): void {
        Log.i(ele, 'removeAllStyle', Tag.styleCleaner);
 
        const styleMap = new Map().set(Constant.delete, Constant.true);
        StyleCommon.styleCache.set(ele, styleMap);
    }
 
    static removeSingleStyle(ele: HTMLElement, attr: string): void {
        Log.i(ele, 'removeStyle: ' + attr, Tag.styleCleaner);
        StyleSetter.setStyle(ele, attr, Constant.delete);
    }
 
    // 重置Tag及子元素中的大图处理
    static resetEle(ele: HTMLElement, isDelete: boolean = false): void {
        Log.i(ele, 'resetEle', Tag.styleCleaner);
 
        Log.i(ele, Store.getValue(ele, LayoutKey.LAYOUT_TAG), Tag.styleCleaner);
        Log.i(ele, Store.getValue(ele, LayoutKey.CONFIG_LAYOUT_TAG), Tag.styleCleaner);
 
        StyleCleaner.resetChild(ele, isDelete);
        StyleCleaner.resetParent(ele.parentElement);
    }
 
    private static resetChild(ele: HTMLElement, isDelete: boolean): void {
        if (!ele) {
            return;
        }
        if (Store.getValue(ele, LayoutKey.INSERT_SWIPER)) {
            Log.i(ele.parentElement, '遇到swiper-box', Tag.styleCleaner);
            ele.remove();
            return;
        }
 
        StyleCleaner.removeAllStyle(ele);
        Store.clearAllTag(ele, isDelete);
 
        // 遍历时会做删除操作，用for i循环index会出错，用for of也会，用forEach不会
        [...ele.children].forEach((child) => {
            StyleCleaner.resetChild(child as HTMLElement, isDelete);
        });
    }
 
    static resetParent(ele: HTMLElement): void {
        if (!ele || ele === Cached.documentElement_()) {
            return;
        }
 
        if (
            Store.getValue(ele, LayoutKey.LAYOUT_TAG) !== LayoutValue.ZOOM_PARENT &&
            Store.getValue(ele, LayoutKey.CONFIG_LAYOUT_TAG) !== LayoutValue.ZOOM_PARENT
        ) {
            return;
        }
 
        for (let i = 0; i < ele.children.length; i++) {
            const child = ele.children[i] as HTMLElement;
            if (!child) {
                continue;
            }
            if (Store.getValue(child, LayoutKey.LAYOUT_TAG) || Store.getValue(child, LayoutKey.CONFIG_LAYOUT_TAG)) {
                return;
            }
        }
        this.removeAllStyle(ele);
        Store.clearAllTag(ele, false);
        this.resetParent(ele.parentElement);
    }
 
    /**
     * 向上查找元素是否已被其他布局缩小过
     * 当元素的祖先元素被缩小过，需要撤销祖先元素的处理
     * 如果祖先元素被调整过宽度，这个方法会返回true
     * 当调用这个方法的函数得到true的返回值，会将元素暂时打标记并立刻return，防止其他布局处理
     * 等待下一轮循环样式刷新后再进行处理
     * @param ele
     */
    static resetParentRelayout(ele: HTMLElement): boolean {
        if (!ele || Utils.isZoomParent(ele)) {
            return false;
        }
 
        Log.i(ele, 'tag: ' + Store.getValue(ele, LayoutKey.LAYOUT_TAG), Tag.styleCleaner);
        if (Store.getValue(ele, LayoutKey.LAYOUT_TAG) === LayoutValue.NEXT_ROUND) {
            return true;
        }
        if (Utils.hadZoomLayout(ele)) {
            Log.i(ele, '该元素的后代被处理，需要撤销这个元素的处理', Tag.styleCleaner);
 
            if (Utils.hadStretchedWidth(ele)) {
                Log.i(ele, '该元素被拉伸过宽度', Tag.styleCleaner);
                // 判断拉伸宽度依赖于元素上的store，因此应该在判断后reset
                StyleCleaner.resetEle(ele);
                // 在被拉伸过宽度的元素上打next_round标记，等待下一轮刷新样式后删除
                // 在调用这个函数的元素上打标记无法防止其他布局处理这个被拉伸宽度的元素
                ObserverHandler.nextRoundAndPost(ele);
                return true;
            }
 
            StyleCleaner.resetEle(ele);
            return false;
        }
 
        return StyleCleaner.resetParentRelayout(ele.parentElement);
    }
 
    static removeWidth(ele: HTMLElement): void {
        StyleCleaner.removeSingleStyle(ele, Constant.width);
        StyleCleaner.removeSingleStyle(ele, Constant.max_width);
        StyleCleaner.removeSingleStyle(ele, Constant.flex_basis);
    }
}