import Cached from '../../Common/Cached';
import StyleSetter from '../../Common/Style/Setter/StyleSetter';
import Log from '../../Debug/Log';
 
/**
 * 所有的动态高度处理都通过这个方法去执行
 * 当我们通过MutationObserver去观察的时候，假如观察subtree的时候，
 * 可能会有多个回调产生
 * 通过这个方法的task管理，减少处理次数，提高性能
 *
 */
 
export default class MiniTask {
    static tasks: Map<HTMLElement, (dom: HTMLElement) => void> = new Map();
    static endTasks: Map<HTMLElement, (dom: HTMLElement) => void> = new Map();
 
    static timer: number;
 
    static post(dom: HTMLElement, handle: (dom: HTMLElement) => void): void {
        // 去重， 进来的dom元素中有将要进行处理的task 将task移除
        this.tasks.set(dom, handle);
 
        this.call();
    }
 
    static postEndTask(dom: HTMLElement, handle: (dom: HTMLElement) => void): void {
        // 去重， 进来的dom元素中有将要进行处理的task 将task移除
        this.endTasks.set(dom, handle);
    }
 
    static call(): void {
        if (this.timer) {
            clearTimeout(this.timer);
        }
 
        // 延迟处理，收集全部依赖，减少重复操作
        this.timer = window.setTimeout(() => {
            this.startTask();
            this.timer = null;
        }, 0);
    }
 
    static execute(dom: HTMLElement, handle: (dom: HTMLElement) => void): void {
        handle.call(null, dom);
    }
 
    static startTask(): void {
        console.time('mini task');
        Log.taskTimes();
 
        StyleSetter.flushAllStyles();
        Cached.clearStyleCache();
 
        this.tasks.forEach((handle, dom) => {
            this.execute(dom, handle);
        });
 
        StyleSetter.flushAllStyles();
        Cached.clearStyleCache();
  
        StyleSetter.flushAllStyles();
        Cached.clearStyleCache();
 
        MiniTask.endTasks.forEach((handle, dom) => {
            this.execute(dom, handle);
        });
 
        StyleSetter.flushAllStyles();
        Cached.clearStyleCache();
 
        this.tasks = new Map();
        console.timeEnd('mini task');
    }
}