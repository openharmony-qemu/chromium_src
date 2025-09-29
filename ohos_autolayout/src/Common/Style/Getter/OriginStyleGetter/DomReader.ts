// 创建隐藏dom，读取样式
import { Txt } from '../../../Txt';
import Store from '../../../Utils/Store';
import { HwTag } from '../../../Constant';
import StyleSetter from '../../Setter/StyleSetter';
import Tag from '../../../../Debug/Tag';
import Log from '../../../../Debug/Log';
 
export default class DomReader {
    private static TAG = Tag.domReader;
 
    /**
     * 清空待处理队列
     * @param waitReadQueue
     */
    static clearWaitReadQueue(waitReadQueue: HTMLElement[]): void {
        // 这里需要直接删除克隆节点，如果异步删除，会导致测试节点被识别到
        waitReadQueue.forEach((dom) => {
            dom.parentElement.removeChild(dom);
        });
    }
 
    /**
     * 创建一个克隆节点
     * @param ele
     */
    static createCloneDom(ele: HTMLElement): HTMLElement {
        const cloneNode = ele.cloneNode(false) as HTMLElement;
        // 这里设置！important防止被网页自身的display属性覆盖（下厨房 课堂）
        cloneNode.style.setProperty(Txt.display_, Txt.none_, Txt.importantText_);
        StyleSetter.delAttr(cloneNode);
        Store.setValue(cloneNode, HwTag.CLONE_DOM, Txt.true_);
        // 记录由谁创建的克隆节点，防止没删除时不知道是哪里创建的
        Log.logCaller(cloneNode);
        return ele.parentElement.appendChild(<Node>cloneNode) as HTMLElement;
    }
 
    /**
     * 读取样式表中宽度
     * @param cloneDom
     */
    static getComputedWidth(cloneDom: HTMLElement): string {
        return getComputedStyle(cloneDom, null).getPropertyValue(Txt.width_);
    }
 
    static getComputedStyle(cloneDom: HTMLElement, propertyName: string): string {
        return getComputedStyle(cloneDom, null).getPropertyValue(propertyName);
    }
}