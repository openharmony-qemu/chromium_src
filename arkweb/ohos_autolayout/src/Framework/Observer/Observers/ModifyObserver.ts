import StyleCleaner from '../../Common/Style/Setter/StyleCleaner';
import Utils from '../../Common/Utils/Utils';
import Log from '../../../Debug/Log';
import Tag from '../../../Debug/Tag';
import IntelligentLayout from '../../../Framework/IntelligentLayout';
import Constant from '../../Common/Constant';
import ObserverHandler from '../ObserverHandler';

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

        Log.d(`document.body: ${document.body}`, ModifyObserver.TAG);
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

    private static onElementModify(records: MutationRecord[]): void {
        // 将 records 加入待处理队列
        ModifyObserver.pendingRecords.push(...records);

        if (!ModifyObserver.scheduledWork) {
            ModifyObserver.scheduledWork = true;
            // 使用微任务批处理，在当前事件循环的微任务阶段执行
            queueMicrotask(() => {
                // 1. 快照当前待处理的记录，并清空队列，避免在处理过程中新加入的记录被处理
                const recordsToProcess = ModifyObserver.pendingRecords;
                ModifyObserver.pendingRecords = [];
                ModifyObserver.scheduledWork = false;

                // 2. 批量处理快照的记录
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

        // 使用 Set 收集所有需要处理的元素，去重
        const addedNodes = new Set<HTMLElement>();
        const removedNodesInfo = new Map<HTMLElement, HTMLElement>(); // <removedNode, parent>
        const attributeChangedNodes = new Set<HTMLElement>();

        for (let i = 0; i < records.length; i++) {
            const record = records[i];

            // 收集新增节点
            for (let j = 0; j < record.addedNodes.length; j++) {
                const node = record.addedNodes[j] as HTMLElement;
                if (node.nodeType === Node.ELEMENT_NODE) {
                    addedNodes.add(node);
                }
            }

            // 收集移除节点
            for (let j = 0; j < record.removedNodes.length; j++) {
                const node = record.removedNodes[j] as HTMLElement;
                if (node.nodeType === Node.ELEMENT_NODE) {
                    removedNodesInfo.set(node, record.target as HTMLElement);
                }
            }

            // 收集属性变更节点
            if (record.type === 'attributes' && record.target.nodeType === Node.ELEMENT_NODE) {
                attributeChangedNodes.add(record.target as HTMLElement);
            }
        }

        // STEP 1: 处理节点移除
        if (removedNodesInfo.size > 0) {
            removedNodesInfo.forEach((parent, node) => {
                ModifyObserver.handleRemove(node, parent);
            });
        }

        // STEP 2: 处理新增节点和属性变更
        let hasValidChange = false;
        const allChangedNodes = new Set([...addedNodes, ...attributeChangedNodes]);

        if (allChangedNodes.size > 0) {
            // 统一处理新增和属性变更
            allChangedNodes.forEach(node => {
                if (ModifyObserver.handleElementChange(node)) {
                    hasValidChange = true;
                }
            });
        }

        // STEP 3: 计算动画延迟
        const animationDuration = ModifyObserver.calculateAnimationDuration(allChangedNodes);

        // STEP 4: 根据是否有有效变更来决定是否触发重排布
        if (hasValidChange) {
            setTimeout(() => {
                ObserverHandler.postTask();
                // 标记 dirty
                if (allChangedNodes.size > 0) {
                    // 从 Set 中获取第一个元素
                    const firstNode = allChangedNodes.values().next().value;
                    if (firstNode) {
                        IntelligentLayout.markDirty(firstNode);
                    }
                }
            }, animationDuration);
        }
    }

    private static handleRemove(node: HTMLElement, parent: HTMLElement): void {
        IntelligentLayout.removePopwinCache(node);
        StyleCleaner.resetEle(node, true);
        StyleCleaner.resetParent(parent);
    }

    private static handleElementChange(node: HTMLElement): boolean {
        if (node.style.display === Constant.none || Utils.ignoreEle(node)) {
            Log.i(node, '忽略隐藏或无意义节点', ModifyObserver.TAG);
            return false;
        }
        return true;
    }

    private static calculateAnimationDuration(changedNodes: Set<HTMLElement>): number {
        let maxDuration = 0;
        if (changedNodes.size > 0) {
            changedNodes.forEach(node => {
                const duration = ModifyObserver.getDurationFromElement(node);
                if (duration > maxDuration) {
                    maxDuration = duration;
                }
            });

            if (maxDuration > 0) {
                Log.d(`检测到动画，延迟 ${maxDuration}ms 处理`, ModifyObserver.TAG);
            }
        }
        return maxDuration;
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
