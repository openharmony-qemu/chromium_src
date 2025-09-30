/**
 * 高度重排模块
 * 1. 区分单节点和多节点的
 * 2. 单节点子节点高度excess的时候。完全使用子节点高度的excess。css不刷新
 * 3. 多节点高度变化的时候css刷新
 */

import Tag from '../../../Debug/Tag';
import HeaderAdj from './HeightAdj/HeaderAdj';

export const TAG = Tag.heightReLayout;
/**
 * 高度调整的入口模块
 *
 * 高度调整对外的接口只有 4个
 *
 * 1. postZoomEle 收集高度调整的依赖
 * 2. readElementRealHeight 高度调整使用cloneDOM的方法开始执行
 * 3. startHeightRemediation 开始进行高度调整
 * 4. waterFall 进行瀑布流的高度调整
 *
 * 高度调整依赖 快照树（snapshotTree）
 * 所以这里的所有方法都会对快照树进行处理
 * postZoomEle - 初始化快照树
 * readElementRealHeight - 快照树的height属性读取
 * startHeightRemediation 执行快照树的遍历
 * waterFall 设置高度并且删除快照树
 */
export default class HeightReLayout {
    static initDOMSnapshotTree():void {
    }

    static removeEle(ele: HTMLElement): void {
        HeaderAdj.removeEle(ele);
    }
}