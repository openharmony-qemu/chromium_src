import Cached from '../Cached';
import { LayoutKey, LayoutValue } from '../Constant';
import CacheStyleGetter from '../Style/Common/CacheStyleGetter';
import { Txt } from '../Txt';
import Store from '../Utils/Store';
import Utils from '../Utils/Utils';
 
export enum EleType {
    IMG,
    TEXT,
}
 
export default class TypeCache {
    static init(): void {
        CacheMethod.getCache(Cached.body_());
    }
 
    static getImgAndText(ele: HTMLElement): { node: Node; type: EleType }[] {
        const list: { node: Node; type: EleType }[] = CacheMethod.imgAndText.get(ele);
        return list ? list : [];
    }
 
    static clearCache(): void {
        CacheMethod.childImg.clear();
        CacheMethod.childText.clear();
        CacheMethod.imgAndText.clear();
 
        Store.clearTag(LayoutValue.IS_IMG);
        Store.clearTag(LayoutValue.HAS_TEXT_CHILD);
    }
 
    static clearEleCache(ele: HTMLElement): void {
        CacheMethod.childImg.delete(ele);
        CacheMethod.childText.delete(ele);
        CacheMethod.imgAndText.delete(ele);
        Store.removeValue(ele, LayoutValue.IS_IMG);
        Store.removeValue(ele, LayoutValue.HAS_TEXT_CHILD);
    }
}
 
export class ImgCache {
    static getList(ele: HTMLElement): HTMLElement[] {
        const imgList = CacheMethod.childImg.get(ele);
        return imgList ? imgList : [];
    }
 
    static getImgType(ele: HTMLElement): string {
        return Store.getValue(ele, LayoutValue.IS_IMG);
    }
 
    static getIndexOfBodyImgsList(targetImg: HTMLElement): number {
        // 返回-2，防止遍历时index + 1后变成0，需要额外的判断
        if (!targetImg) {
            return -2;
        }
 
        return ImgCache.getList(Cached.body_()).findIndex((img) => img === targetImg);
    }
 
    static getImgOfBodyImgsList(imgIndex: number): HTMLElement {
        const imgsList = ImgCache.getList(Cached.body_());
        if (imgsList.length === 0) {
            return null;
        }
 
        if (imgIndex < 0 || imgIndex >= imgsList.length) {
            return null;
        }
 
        return imgsList[imgIndex];
    }
}
 
export class TextCache {
    static getList(ele: HTMLElement): Node[] {
        const textList = CacheMethod.childText.get(ele);
        return textList ? textList : [];
    }
 
    static hasTextChild(ele: HTMLElement): boolean {
        return Store.getValue(ele, LayoutValue.HAS_TEXT_CHILD) === Txt.true_;
    }
}
 
class CacheMethod {
    static childImg: Map<HTMLElement, HTMLElement[]> = new Map(); // 存放某节点的后代图片元素列表
    static childText: Map<HTMLElement, Node[]> = new Map(); // 存放某节点的后代文本元素列表
    static imgAndText: Map<HTMLElement, { node: Node; type: EleType }[]> = new Map(); // 存放某节点的后代文本元素列表
 
    static getCache(ele: HTMLElement): [HTMLElement[], Node[], { node: Node; type: EleType }[]] {
        if (CacheMethod.noNeedCheck(ele)) {
            return [[], [], []];
        }
        // 若元素在cache中已有缓存，直接返回
        const textCache = CacheMethod.childText.get(ele);
        if (textCache) {
            return [CacheMethod.childImg.get(ele), textCache, CacheMethod.imgAndText.get(ele)];
        }
 
        let childImgList: HTMLElement[] = [];
        let childTextList: Node[] = [];
        let imgAndTextList: { node: Node; type: EleType }[] = [];
        let childHasTextFlag: boolean = false;
        for (let i = 0; i < ele.childNodes.length; i++) {
            let childNode = ele.childNodes[i];
 
            // 忽略隐藏图片
            if (CacheStyleGetter.computedStyle(ele, Txt.display_) === Txt.none_) {
                continue;
            }
 
            const nodeType = childNode.nodeType;
            if (CacheMethod.isText(childNode, nodeType)) {
                childTextList.push(childNode as Node);
                imgAndTextList.push({ node: childNode as Node, type: EleType.TEXT });
                childHasTextFlag = true;
            }
 
            // 不关注非文本、非元素类型的子节点
            if (nodeType !== Node.ELEMENT_NODE) {
                continue;
            }
 
            let childEle = childNode as HTMLElement;
            // 若当前子元素符合图片要求，加入缓存列表
            if (CacheMethod.isImg(childEle)) {
                childImgList.push(childEle);
                imgAndTextList.push({ node: childEle as Node, type: EleType.IMG });
            }
 
            // 拼接后代Text/Img元素列表
            let [descendantImgList, descendantTextList, descendantImgAndTextList] = CacheMethod.getCache(childEle);
            childImgList = childImgList.concat(descendantImgList);
            childTextList = childTextList.concat(descendantTextList);
            imgAndTextList = imgAndTextList.concat(descendantImgAndTextList);
        }
        // 标记当前元素的子元素（不包括后代元素）中有文本元素
        if (childHasTextFlag) {
            Store.setValue(ele, LayoutValue.HAS_TEXT_CHILD, Txt.true_);
        }
 
        CacheMethod.childImg.set(ele, childImgList);
        CacheMethod.childText.set(ele, childTextList);
        CacheMethod.imgAndText.set(ele, imgAndTextList);
 
        return [childImgList, childTextList, imgAndTextList];
    }
 
    private static noNeedCheck(ele: HTMLElement): boolean {
        if (ele.getAttribute(LayoutKey.HW_BUTTON)) {
            return true;
        }
 
        if (Utils.ignoreLocalName(ele)) {
            return true;
        }
        if (CacheStyleGetter.offsetW(ele) === 0 && CacheStyleGetter.computedStyle(ele, Txt.display_) === Txt.none_) {
            return true;
        }
        if (ele.childNodes.length === 0) {
            return true;
        }
        return false;
    }
 
    private static isText(childNode: Node, nodeType: number): boolean {
        // 缓存文本子节点 (过滤空格、回车等空文本节点)
        if (nodeType === Node.TEXT_NODE && !Utils.isEmptyText(childNode.nodeValue)) {
            return true;
        }
        return false;
    }
 
    private static isImg(ele: HTMLElement): boolean {
        const localName = CacheStyleGetter.localName(ele);
        const hasBackTags = [Txt.div_, Txt.i_, Txt.span_, Txt.aTag_, Txt.button_, Txt.li_];
        
        // 大图处理添加的背景不加入列表
        if (Store.getValue(ele, LayoutKey.LAYOUT_TAG) === LayoutValue.BACK_IMG) {
            return false;
        }
 
        if (localName === Txt.img_) {
            Store.setValue(ele, LayoutValue.IS_IMG, Txt.img_);
            return true;
        }
 
        if (hasBackTags.includes(localName)) {
            if (CacheMethod.isBackground(ele)) {
                Store.setValue(ele, LayoutValue.IS_IMG, Txt.backImg_);
                return true;
            }
 
            return false;
        }
 
        if (localName === Txt.canvas_) {
            Store.setValue(ele, LayoutValue.IS_IMG, Txt.canvas_);
            return true;
        }
 
        if (localName === Txt.svg_) {
            Store.setValue(ele, LayoutValue.IS_IMG, Txt.svg_);
            return true;
        }
 
        return false;
    }
 
    private static isBackground(ele: HTMLElement): boolean {
        const backImg = CacheStyleGetter.computedStyle(ele, Txt.backgroundImage_);
        const dataBack = CacheStyleGetter.computedStyle(ele, Txt.dataBackground_);
 
        // 背景图的尺寸或不以px结尾（auto、cover、contain...），或以px结尾但超过3px（有背景图，但是背景图尺寸小，不应该算图片）
        const backgroundSize = CacheStyleGetter.computedStyle(ele, Txt.backgroundSize_).split(' ');
        const isSmallBack = backgroundSize.some((size) => {
            return size.includes(Txt.px_) && parseFloat(size) < 3;
        });
 
        return !isSmallBack;
    }
 
    private static hasBackUrl(backgroundString: string): boolean {
        return backgroundString && backgroundString.startsWith(Txt.url_);
    }
}