import StyleCleaner from '../../Common/Style/Setter/StyleCleaner';
import Log from '../../../Debug/Log';
import Tag from '../../../Debug/Tag';
import IntelligentLayout from '../../../Framework/IntelligentLayout';
import ObserverHandler from '../ObserverHandler';
import Constant from '../../Common/Constant';
import Framework from '../../Framework';

interface AnimationDurations {
    animationDur: number,
    transitionDur: number,
    total: number
}

export default class ModifyObserver {
    static modifyObserver: MutationObserver;
    private static TAG = Tag.modifyObserver;
    
    // 用于批处理的变量
    private static pendingRecords: MutationRecord[] = [];
    private static scheduledWork = false;

    static reInit(): void {
        Log.info('run reInit modifyObserver', ModifyObserver.TAG);
        
        if (ModifyObserver.modifyObserver) {
            Log.info('disconnect modifyObserver', ModifyObserver.TAG);
            ModifyObserver.modifyObserver.disconnect();
        }

        Log.d(`document.body.id: ${document.body.id}`, ModifyObserver.TAG);
        ModifyObserver.modifyObserver = new MutationObserver(ModifyObserver.onElementModify);
        ModifyObserver.modifyObserver.observe(document.body, {
            childList: true,
            subtree: true,
            attributes: true,
            attributeOldValue: true,
        });
    }

    static disconnect(): void {
        Log.info('disconnect modifyObserver', ModifyObserver.TAG);
        ModifyObserver.modifyObserver?.disconnect();
        ModifyObserver.modifyObserver = null;
        
        // 清理待处理的记录
        ModifyObserver.pendingRecords = [];
        ModifyObserver.scheduledWork = false;
    }

    private static handleRemove(records: MutationRecord[]): boolean {
        let hasValidChange:boolean = false;
        for (const item of records) {
            for (let i = 0; i < item.removedNodes.length; i++) {
                let child = item.removedNodes[i] as HTMLElement;

                if (child.nodeType !== Node.ELEMENT_NODE) {
                    continue;
                }
                hasValidChange = hasValidChange || IntelligentLayout.removePopwinCache(child);
                StyleCleaner.resetEle(child, true);
            }

            StyleCleaner.resetParent(item.target as HTMLElement);
        }
        return hasValidChange;
    }

    private static checkElementAddNeedPostTask(item: MutationRecord): boolean {
        let needPostTask = false;

        for (let i = 0; i < item.addedNodes.length; i++) {
            const node = item.addedNodes[i] as HTMLElement;
            if (node.nodeType !== Node.ELEMENT_NODE) {
                continue;
            }

            Log.i(node, '新增元素', ModifyObserver.TAG);

            if (node.style.display === Constant.none) {
                Log.i(node, '忽略隐藏节点', ModifyObserver.TAG);
                continue;
            }

            Log.i(node, '变动有效', ModifyObserver.TAG);
            needPostTask = true;
        }
        return needPostTask;
    }

    /**
     * 1、监听节点属性变化、节点增加减少
     * 2、通过弹窗的root节点是否包含这些节点变化，判断弹窗是否需要重新修复。
     * 3、如果没有弹窗根节点，则通过300ms的定时任务，检测变化的节点的宽度是否与屏幕宽度是否一致。如果一致，则启动findPopups遍历节点查找弹窗。
     * @param records
     */
    private static onElementModify(records: MutationRecord[]): void {
        // 将records加入待处理队列，使用微任务批处理，避免同步DOM操作
        ModifyObserver.pendingRecords.push(...records);
        
        if (!ModifyObserver.scheduledWork) {
            ModifyObserver.scheduledWork = true;
            // 使用微任务批处理，在当前事件循环的微任务阶段执行
            queueMicrotask(() => {
                // 先保存当前待处理的记录，再清空队列，这样可以避免在处理过程中新加入的记录被丢失
                const recordsToProcess = ModifyObserver.pendingRecords;
                ModifyObserver.pendingRecords = [];
                ModifyObserver.scheduledWork = false;
                
                // 处理保存的记录
                ModifyObserver.processBatch(recordsToProcess);
            });
        }
    }
    
    /**
     * 批量处理累积的MutationRecord，减少DOM查询、合并操作、提前退出
     * @param records 待处理的记录数组
     */
    private static processBatch(records: MutationRecord[]): void {
        if (records.length === 0) {
            return;
        }
        
        Log.d(`批处理 ${records.length} 个变更记录`, ModifyObserver.TAG);
        
        // 分类记录，减少重复遍历
        const removeRecords: MutationRecord[] = [];
        const addRecords: MutationRecord[] = [];
        const attrRecords: MutationRecord[] = [];
        
        for (let i = 0; i < records.length; i++) {
            const record = records[i];
            if (record.removedNodes.length > 0) {
                removeRecords.push(record);
            }
            if (record.addedNodes.length > 0) {
                addRecords.push(record);
            }
            if (record.type === 'attributes') {
                attrRecords.push(record);
            }
        }
        
        // STEP 1: 处理节点移除
        const hasValidRemove = ModifyObserver.handleRemove(removeRecords);

        // STEP 2: 计算动画延迟
        let animationDuration = ModifyObserver.calculateAnimationDuration(addRecords, attrRecords);
        // 如果存在动画时延，给layout上锁
        if(animationDuration > 0 ) {
            Framework.lockLayout();
            setTimeout(() => {
                Framework.unLockLayout();
                ObserverHandler.postTask();
            }, animationDuration);
        }

        // STEP 3: 延迟处理节点添加，只在有添加记录时才设置定时器
        const hasValidAdd = ModifyObserver.handleAddedNodes(addRecords);
        // 如果animationDuration不为0，前面就已经posttask，所以此处只需要处理为0，且存在validchange的情况
        if(animationDuration === 0 && (hasValidRemove || hasValidAdd)) {
            ObserverHandler.postTask();
        }
    }

    private static handleAddedNodes(addRecords: MutationRecord[]): boolean {
        let hasValidChange = false;
        if (addRecords.length > 0) {
            // 批量处理，减少函数调用
            for (let i = 0; i < addRecords.length; i++) {
                let hasChange = ModifyObserver.checkElementAddNeedPostTask(addRecords[i]);
                // 只在有有效变更时才触发重布局
                if (hasChange) {
                    hasValidChange = true;
                    // 只标记一次dirty，而不是每个record都标记
                    IntelligentLayout.markDirty(addRecords[i].target as HTMLElement);
                }
            }
        }
        return hasValidChange;
    }

    private static calculateAnimationDuration(addRecords: MutationRecord[], attrRecords: MutationRecord[]): number {
        let animationDuration = 0;
        // 只在有添加或属性变更时才计算动画
        if (addRecords.length === 0 || attrRecords.length === 0) {
            return animationDuration;
        }
        // 使用Set去重，避免重复计算同一元素
        const processedElements = new Set<HTMLElement>();

        // 处理属性变更的动画
        animationDuration = ModifyObserver.getAnimationDurationInAttriChange(attrRecords, processedElements, animationDuration);

        // 处理新增节点的动画
        animationDuration = ModifyObserver.getAnimationDurationInAddNodes(addRecords, processedElements, animationDuration);

        if (animationDuration > 0) {
            Log.d(`检测到动画，延迟 ${animationDuration}ms 处理`, ModifyObserver.TAG);
        }
        return animationDuration;
    }

    private static getAnimationDurationInAddNodes(addRecords: MutationRecord[], processedElements: Set<HTMLElement>, animationDuration: number): number {
        for (let i = 0; i < addRecords.length; i++) {
            const addedNodes = addRecords[i].addedNodes;
            for (let j = 0; j < addedNodes.length; j++) {
                const node = addedNodes[j];
                if (node instanceof HTMLElement && !processedElements.has(node)) {
                    animationDuration = Math.max(ModifyObserver.getDurationFromElement(node),animationDuration);
                    processedElements.add(node);
                }
            }
        }
        return animationDuration;
    }

    private static getAnimationDurationInAttriChange(attrRecords: MutationRecord[], processedElements: Set<HTMLElement>, animationDuration: number): number {
        for (let i = 0; i < attrRecords.length; i++) {
            const target = attrRecords[i].target;
            if (target instanceof HTMLElement && !processedElements.has(target)) {
                processedElements.add(target);
                const duration = ModifyObserver.getDurationFromElement(target);
                if (duration > animationDuration) {
                    animationDuration = duration;
                    IntelligentLayout.markDirty(target);
                }
            }
        }
        return animationDuration;
    }

    /**
     * 从单个元素中获取动画时长，直接返回最大值
     */
    private static getDurationFromElement(element: HTMLElement): number {
        const animationInfo = ModifyObserver.getAnimDurations(element);
        return Math.max(animationInfo.animationDur, animationInfo.transitionDur);
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
