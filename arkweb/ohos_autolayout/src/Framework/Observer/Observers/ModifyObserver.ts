import HeaderAdj from '../../../Actions/Common/HeightRelayout/HeightAdj/HeaderAdj';
import { ObserverRecord, recordType } from '../../../Common/Perform/ChangeRecord';
import DiffEleRecord from '../../../Common/Perform/DiffEleRecorder';
import OriginStyleCache from '../../../Common/Style/Getter/OriginStyleGetter/OriginStyleCache';
import StyleCleaner from '../../../Common/Style/Setter/StyleCleaner';
import { Txt } from '../../../Common/Txt';
import Utils from '../../../Common/Utils/Utils';
import Log from '../../../Debug/Log';
import Tag from '../../../Debug/Tag';
import IntelligentLayout from '../../../Framework/IntelligentLayout';
import ObserverHandler from '../ObserverHandler';

interface AnimationDurations {
    animationDur: number,
    transitionDur: number,
    total: number
}

export default class ModifyObserver {
    static modifyObserver: MutationObserver;
    private static TAG = Tag.modifyObserver;
    private static records:MutationRecord[] = [];

    static reInit(): void {
        console.log(' run reInit modifyObserver');
        
        if (ModifyObserver.modifyObserver) {
            console.log(' disconnect modifyObserver');
            ModifyObserver.modifyObserver.disconnect();
        }

        console.log(' print document.body = ' + document.body);
        ModifyObserver.modifyObserver = new MutationObserver(ModifyObserver.onElementModify);
        ModifyObserver.modifyObserver.observe(document.body, {
            childList: true,
            subtree: true,
            attributes: true,
            attributeOldValue: true,
        });
    }

    static disconnect(): void {
        console.log(' disconnect modifyObserver');
        ModifyObserver.modifyObserver?.disconnect();
        ModifyObserver.modifyObserver = null;
    }

    private static handleRemove(item: MutationRecord): void {
        let needSetTag = false;

        for (let i = 0; i < item.removedNodes.length; i++) {
            let child = item.removedNodes[i] as HTMLElement;
            IntelligentLayout.removePopwinCache(child);

            if (child.nodeType !== Node.ELEMENT_NODE) {
                continue;
            }

            StyleCleaner.resetEle(child, true);

            if (child.style.display !== Txt.none_) {
                needSetTag = true;
            }
        }

        if (needSetTag) {
            // 图片如果删除，找不到其父元素，在上边的循环中向上清除缓存时找不到parentElement，在这里进行清理
            DiffEleRecord.setTag(item.target as HTMLElement);
        }

        StyleCleaner.resetParent(item.target as HTMLElement);
    }

    private static handleElementAdd(item: MutationRecord): boolean {
        let needPostTask = false;

        for (let i = 0; i < item.addedNodes.length; i++) {
            const node = item.addedNodes[i] as HTMLElement;
            if (node.nodeType !== Node.ELEMENT_NODE) {
                continue;
            }

            Log.i(node, '新增元素', this.TAG);

            if (node.style.display === Txt.none_) {
                Log.i(node, '忽略隐藏节点', this.TAG);
                continue;
            }

            if (Utils.ignoreEle(node)) {
                Log.i(node, '忽略无意义节点', this.TAG);
                continue;
            }

            OriginStyleCache.clearToTop(node);
            HeaderAdj.collectHeaderEle(node);

            if (ObserverRecord.ignoreChange(node, recordType.ADD)) {
                Log.i(node, '忽略频繁变动', this.TAG);
                continue;
            }

            Log.i(node, '变动有效', this.TAG);
            needPostTask = true;
            DiffEleRecord.setTag(node);
        }
        return needPostTask;
    }

    /**
     * todo：
     * 1、监听节点属性变化、节点增加减少
     * 2、通过弹窗的root节点是否包含这些节点变化，判断弹窗是否需要重新修复。
     * 3、如果没有弹窗根节点，则通过300ms的定时任务，检测变化的节点的宽度是否与屏幕宽度是否一致。如果一致，则启动findPopups遍历节点查找弹窗。
     * @param records
     */
    private static onElementModify(records: MutationRecord[]): void {
        // 当窗口大小或内容发生变化时，判断是否需要调整布局

        for (let item of records) {
            ModifyObserver.handleRemove(item);
        }

        let animationDuration: number = 0;
        let tmpAddedNodes: HTMLElement[] = [];
        for (let item of records) {
            const recordDuration = ModifyObserver.calDuration(item, tmpAddedNodes);
            animationDuration = Math.max(animationDuration, recordDuration);
        }
        if (animationDuration > 0) {
            console.log(` post task with delay: ${animationDuration}`);
        }

        setTimeout(() => {
            for (let item of records) {
                const needPostTask = ModifyObserver.handleElementAdd(item);

                if (needPostTask || animationDuration > 0) {
                    ObserverHandler.postTask();
                    IntelligentLayout.markDirty(item);
                }
            }
        }, animationDuration);
    }

    /**
     * 从单个元素中获取动画时长
     */
    private static getDurationFromElement(element: HTMLElement): number {
        const animationInfo = ModifyObserver.getAnimDurations(element);
        return Math.max(animationInfo.animationDur, animationInfo.transitionDur);
    }

    /**
     * 处理 'attributes' 类型的变更
     */
    private static handleAttributeMutation(record: MutationRecord): number {
        if (record.target instanceof HTMLElement) {
            return this.getDurationFromElement(record.target);
        }
        return 0;
    }

    /**
     * 处理 'childList' 类型的变更
     */
    private static handleChildListMutation(record: MutationRecord, tmpAddedNodes: HTMLElement[]): number {
        let maxDuration = 0;
        for (const node of record.addedNodes) {
            // 使用卫语句提前跳过不符合条件的节点
            if (!(node instanceof HTMLElement) || tmpAddedNodes.includes(node)) {
                continue;
            }

            tmpAddedNodes.push(node);
            const currentDuration = this.getDurationFromElement(node);
            maxDuration = Math.max(maxDuration, currentDuration);
        }
        return maxDuration;
    }

    /**
     * 动画时长通过两种方式获取
     * 1、animation
     * 2、transition
     * 逻辑如下：
     * 1、对于attributes和变化引发的回调，筛选record.attributeName为style的变化，遍历节点，获取transition和animation
     * 2、对于childList变化引发的回调，遍历节点，获取transition和animation
     * @param record 
     * @param tmpAddedNodes 
     * @returns 
     */
    private static calDuration(record: MutationRecord, tmpAddedNodes: HTMLElement[]): number {
        let duration = 0;

        if (record.type === 'attributes') {
            duration = this.handleAttributeMutation(record);
        } else if (record.type === 'childList') {
            duration = this.handleChildListMutation(record, tmpAddedNodes);
        }

        if (duration > 0) {
            console.log('pause');
        }

        return duration;
    }

    static cssTimeToMs(str: string): number {
        if (!str || str === '0s' || str === '0ms') {
            return 0;
        }
        const match = str.trim().match(/^([\d.]+)(s|ms)$/);
        if (!match) {
            return 0;
        }
        const [, num, unit] = match;
        return unit === 's' ? parseFloat(num) * 1000 : parseFloat(num);
    }
  
    // 获取元素当前正在运行的动画总时长（animation + transition）
    static getAnimDurations(el: HTMLElement): AnimationDurations {
        const cs = getComputedStyle(el);
        const animDur = ModifyObserver.cssTimeToMs(cs.animationDuration);
        const transDur = ModifyObserver.cssTimeToMs(cs.transitionDuration);
        return { animationDur: animDur, transitionDur: transDur, total: animDur + transDur };
    }
}
