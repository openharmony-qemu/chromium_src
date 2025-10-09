import Store from '../Utils/Store';
import { HwTag, LayoutKey, LayoutValue } from '../Constant';
import TypeCache from './TypeCache';
import Log from '../../Debug/Log';
import Tag from '../../Debug/Tag';
 
export default class DiffEleRecorder {
    private static allEleDiff: boolean = true;
    private static TAG = Tag.diffRecorder;
 
    // resize/Rem变化时，全部遍历，故清空所有缓存
    static setAllEleDiff(): void {
        DiffEleRecorder.allEleDiff = true;
        TypeCache.clearCache();
        Store.clearTag(HwTag.CROSS_TAG);
    }
 
    static isAllEleDiff(): boolean {
        return DiffEleRecorder.allEleDiff;
    }
 
    static setTag(ele: HTMLElement): void {
        Store.setValue(ele, LayoutKey.DIFF_ELE, LayoutValue.ROOT);
        Store.removeValue(ele, HwTag.CROSS_TAG);
        TypeCache.clearEleCache(ele);
 
        let par = ele.parentElement;
        while (par && par !== document.documentElement) {
            if (!Store.getValue(par, LayoutKey.DIFF_ELE)) {
                Store.setValue(par, LayoutKey.DIFF_ELE, LayoutValue.PARENT);
            }
 
            Store.removeValue(par, HwTag.CROSS_TAG);
            TypeCache.clearEleCache(par);
            par = par.parentElement;
        }
    }
 
    static isDiff(ele: HTMLElement): boolean {
        if (DiffEleRecorder.allEleDiff) {
            return true;
        }
        let eleTag = Store.getValue(ele, LayoutKey.DIFF_ELE);
        if (eleTag === LayoutValue.PARENT) {
            return true;
        }
        // 当前元素是ROOT时，将子元素打标为ROOT（即遍历所有后代元素），自身标记为PARENT（减少遍历时的重复打标）
        if (eleTag === LayoutValue.ROOT) {
            Store.setValue(ele, LayoutKey.DIFF_ELE, LayoutValue.PARENT);
            for (let i = 0; i < ele.children.length; i++) {
                Store.setValue(ele.children[i] as HTMLElement, LayoutKey.DIFF_ELE, LayoutValue.ROOT);
                Store.removeValue(ele.children[i] as HTMLElement, HwTag.CROSS_TAG);
            }
            return true;
        }
        return false;
    }
 
    private static diffEleCache: Set<HTMLElement> = new Set();
    static setTagCache(ele: HTMLElement): void {
        Log.i(ele, '发生变化', this.TAG);
        DiffEleRecorder.diffEleCache.add(ele);
    }
 
    static flushTag(): void {
        Store.clearTag(LayoutKey.DIFF_ELE);
        DiffEleRecorder.allEleDiff = false;
 
        // 刷新需下一轮检测的元素Cache
        for (let ele of this.diffEleCache) {
            Log.i(ele, '标记变化', this.TAG);
            this.setTag(ele);
        }
        DiffEleRecorder.diffEleCache.clear();
    }
}