import StyleCleaner from '../../Common/Style/Setter/StyleCleaner';
import Log from '../../../Debug/Log';
import Tag from '../../../Debug/Tag';
import IntelligentLayout from '../../../Framework/IntelligentLayout';
import ObserverHandler from '../ObserverHandler';
import Constant from '../../Common/Constant';
import Framework from '../../Framework';
import { PopupStateManager } from '../../Popup/PopupStateManager';
import { PopupLayoutState } from '../../Popup/PopupLayoutState';

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
    
    // 动画监听相关
    private static animationListenerAdded = false;
    private static animationStartHandler: ((event: AnimationEvent) => void) | null = null;
    private static transitionStartHandler: ((event: TransitionEvent) => void) | null = null;
    
    // 动画超时管理：存储所有活跃的动画延迟任务
    private static activeAnimationTimeouts: Map<number, HTMLElement> = new Map();

    static reInit(): void {
        Log.info('========== 初始化DOM监听器 ==========', ModifyObserver.TAG);
        
        if (ModifyObserver.modifyObserver) {
            Log.info('断开现有监听器', ModifyObserver.TAG);
            ModifyObserver.modifyObserver.disconnect();
        }

        Log.d(`监听目标: document.body (id: ${document.body.id})`, ModifyObserver.TAG);
        Log.d('监听配置: childList=true, subtree=true, attributes=true, attributeOldValue=true', ModifyObserver.TAG);
        ModifyObserver.modifyObserver = new MutationObserver(ModifyObserver.onElementModify);
        ModifyObserver.modifyObserver.observe(document.body, {
            childList: true,
            subtree: true,
            attributes: true,
            attributeOldValue: true,
        });
        // 添加动画事件监听
        ModifyObserver.addAnimationListeners();
        Log.info('DOM监听器启动成功', ModifyObserver.TAG);
    }
    
    /**
     * 取消所有活跃的动画延迟任务
     * 用于在resize或其他中断场景下清理未完成的动画等待
     */
    static cancelAllAnimationTimeouts(): void {
        const count = ModifyObserver.activeAnimationTimeouts.size;
        if (count > 0) {
            Log.info(`取消所有动画超时任务: ${count}个`, ModifyObserver.TAG);
            for (const [timeoutId, element] of ModifyObserver.activeAnimationTimeouts.entries()) {
                clearTimeout(timeoutId);
                Log.d(`取消超时任务: ${timeoutId} (${element.tagName}.${element.className})`, ModifyObserver.TAG);
            }
            ModifyObserver.activeAnimationTimeouts.clear();
            Log.info('所有动画超时任务已取消', ModifyObserver.TAG);
        }
    }
    
    /**
     * 添加全局动画事件监听，补充 MutationObserver 无法捕获的 CSS 动画
     */
    private static addAnimationListeners(): void {
        if (ModifyObserver.animationListenerAdded) {
            Log.d('动画监听器已存在，跳过添加', ModifyObserver.TAG);
            return;
        }
        
        Log.info('========== 添加CSS动画事件监听 ==========', ModifyObserver.TAG);
        
        // 创建并保存 animation 监听器
        ModifyObserver.animationStartHandler = (event: AnimationEvent) => {
            const target = event.target as HTMLElement;
            Log.d(`🎬 CSS动画开始: ${target.tagName}.${target.className}, 动画名: ${event.animationName}`, ModifyObserver.TAG);
            
            //  查找弹窗根节点并设置状态
            const popupRoot = ModifyObserver.findPopupRoot(target);
            if (popupRoot) {
                const currentState = PopupStateManager.getState(popupRoot);
                // 只有在IDLE状态才设置为WAITING_ANIMATION，避免覆盖正在进行的布局流程
                if (currentState === PopupLayoutState.IDLE) {
                    PopupStateManager.setState(popupRoot, PopupLayoutState.WAITING_ANIMATION, '检测到动画开始');
                } else {
                    Log.d(`弹窗当前状态为 ${PopupLayoutState[currentState]}，跳过设置WAITING_ANIMATION`, ModifyObserver.TAG);
                }
            }
            
            const duration = ModifyObserver.getDurationFromElement(target);
            if (duration > 0) {
                Log.d(`锁定布局，延迟 ${duration}ms`, ModifyObserver.TAG);
                const timeoutId = setTimeout(() => {
                    // 从Map中移除已完成的超时任务
                    ModifyObserver.activeAnimationTimeouts.delete(timeoutId as any);
                    Log.d(`CSS动画延迟结束，解锁布局`, ModifyObserver.TAG);
                    
                    //  动画完成后，重置为 IDLE
                    if (popupRoot && PopupStateManager.getState(popupRoot) === PopupLayoutState.WAITING_ANIMATION) {
                        PopupStateManager.setState(popupRoot, PopupLayoutState.IDLE, '动画完成');
                    }
                    
                    ObserverHandler.postTask();
                }, duration);
                // 注册超时任务到Map中
                ModifyObserver.activeAnimationTimeouts.set(timeoutId as any, target);
            }
        };
        
        // 创建并保存 transition 监听器
        ModifyObserver.transitionStartHandler = (event: TransitionEvent) => {
            const target = event.target as HTMLElement;
            Log.d(`🎬 CSS过渡开始: ${target.tagName}.${target.className}, 属性: ${event.propertyName}`, ModifyObserver.TAG);
            
            //  查找弹窗根节点并设置状态
            const popupRoot = ModifyObserver.findPopupRoot(target);
            if (popupRoot) {
                const currentState = PopupStateManager.getState(popupRoot);
                // 只有在IDLE状态才设置为WAITING_ANIMATION，避免覆盖正在进行的布局流程
                if (currentState === PopupLayoutState.IDLE) {
                    PopupStateManager.setState(popupRoot, PopupLayoutState.WAITING_ANIMATION, '检测到过渡开始');
                } else {
                    Log.d(`弹窗当前状态为 ${PopupLayoutState[currentState]}，跳过设置WAITING_ANIMATION`, ModifyObserver.TAG);
                }
            }
            
            const duration = ModifyObserver.getDurationFromElement(target);
            if (duration > 0) {
                Log.d(`锁定布局，延迟 ${duration}ms`, ModifyObserver.TAG);
                const timeoutId = setTimeout(() => {
                    // 从Map中移除已完成的超时任务
                    ModifyObserver.activeAnimationTimeouts.delete(timeoutId as any);
                    Log.d(`CSS过渡延迟结束，解锁布局`, ModifyObserver.TAG);
                    //  过渡完成后，重置为 IDLE
                    if (popupRoot && PopupStateManager.getState(popupRoot) === PopupLayoutState.WAITING_ANIMATION) {
                        PopupStateManager.setState(popupRoot, PopupLayoutState.IDLE, '过渡完成');
                    }
                    
                    ObserverHandler.postTask();
                }, duration);
                // 注册超时任务到Map中
                ModifyObserver.activeAnimationTimeouts.set(timeoutId as any, target);
            }
        };
        
        // 添加监听器
        document.body.addEventListener('animationstart', ModifyObserver.animationStartHandler, true);
        document.body.addEventListener('transitionstart', ModifyObserver.transitionStartHandler, true);
        
        ModifyObserver.animationListenerAdded = true;
        Log.info('CSS动画事件监听器添加成功', ModifyObserver.TAG);
    }
    
    /**
     * 查找元素所属的弹窗根节点
     * @private
     */
    private static findPopupRoot(element: HTMLElement): HTMLElement | null {
        for (const [popupInfo] of IntelligentLayout.popWindowMap.entries()) {
            if (popupInfo.root_node && popupInfo.root_node.contains(element)) {
                return popupInfo.root_node;
            }
        }
        return null;
    }

    static disconnect(): void {
        Log.info('========== 断开DOM监听器 ==========', ModifyObserver.TAG);
        const pendingCount = ModifyObserver.pendingRecords.length;
        if (pendingCount > 0) {
            Log.w(`清理 ${pendingCount} 个待处理的变更记录`, ModifyObserver.TAG);
        }
        ModifyObserver.modifyObserver?.disconnect();
        ModifyObserver.modifyObserver = null;
        
        // 清理待处理的记录
        ModifyObserver.pendingRecords = [];
        ModifyObserver.scheduledWork = false;
        
        // 取消所有动画超时任务
        ModifyObserver.cancelAllAnimationTimeouts();
        
        // 移除动画监听器
        if (ModifyObserver.animationListenerAdded) {
            Log.d('移除CSS动画事件监听器', ModifyObserver.TAG);
            if (ModifyObserver.animationStartHandler) {
                document.body.removeEventListener('animationstart', ModifyObserver.animationStartHandler, true);
                ModifyObserver.animationStartHandler = null;
            }
            if (ModifyObserver.transitionStartHandler) {
                document.body.removeEventListener('transitionstart', ModifyObserver.transitionStartHandler, true);
                ModifyObserver.transitionStartHandler = null;
            }
            ModifyObserver.animationListenerAdded = false;
            Log.d('CSS动画事件监听器已移除', ModifyObserver.TAG);
        }
        
        Log.info('DOM监听器已断开', ModifyObserver.TAG);
    }

    private static handleRemove(records: MutationRecord[]): boolean {
        if (records.length === 0) {
            return false;
        }
        
        Log.d(`处理节点移除: ${records.length} 条记录`, ModifyObserver.TAG);
        let hasValidChange:boolean = false;
        let totalRemovedElements = 0;
        
        for (const item of records) {
            for (let i = 0; i < item.removedNodes.length; i++) {
                let child = item.removedNodes[i] as HTMLElement;

                if (child.nodeType !== Node.ELEMENT_NODE) {
                    continue;
                }
                totalRemovedElements++;
                const removedFromCache = IntelligentLayout.removePopwinCache(child);
                if (removedFromCache) {
                    Log.d(`从缓存中移除节点: ${child.tagName}.${child.className}`, ModifyObserver.TAG);
                }
                hasValidChange = hasValidChange || removedFromCache;
                StyleCleaner.resetEle(child, true);
            }

            StyleCleaner.resetParent(item.target as HTMLElement);
        }
        
        Log.d(`节点移除处理完成: 移除${totalRemovedElements}个元素节点, 有效变更: ${hasValidChange}`, ModifyObserver.TAG);
        return hasValidChange;
    }

    private static checkElementAddNeedPostTask(item: MutationRecord): boolean {
        let needPostTask = false;
        let validCount = 0;
        let hiddenCount = 0;

        for (let i = 0; i < item.addedNodes.length; i++) {
            const node = item.addedNodes[i] as HTMLElement;
            if (node.nodeType !== Node.ELEMENT_NODE) {
                continue;
            }

            Log.d(`新增元素: ${node.tagName}.${node.className}`, ModifyObserver.TAG);

            if (node.style.display === Constant.none) {
                Log.d(`跳过隐藏节点: ${node.tagName}.${node.className}`, ModifyObserver.TAG);
                hiddenCount++;
                continue;
            }

            Log.d(`有效变动: ${node.tagName}.${node.className}`, ModifyObserver.TAG);
            validCount++;
            needPostTask = true;
        }
        
        if (validCount > 0 || hiddenCount > 0) {
            Log.d(`节点添加统计: 有效${validCount}个, 隐藏${hiddenCount}个`, ModifyObserver.TAG);
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
        Log.d(`收到DOM变更通知: ${records.length} 条记录`, ModifyObserver.TAG);
        
        // 将records加入待处理队列，使用微任务批处理，避免同步DOM操作
        ModifyObserver.pendingRecords.push(...records);
        
        if (!ModifyObserver.scheduledWork) {
            Log.d('调度微任务批处理', ModifyObserver.TAG);
            ModifyObserver.scheduledWork = true;
            // 使用微任务批处理，在当前事件循环的微任务阶段执行
            queueMicrotask(() => {
                // 先保存当前待处理的记录，再清空队列，这样可以避免在处理过程中新加入的记录被丢失
                const recordsToProcess = ModifyObserver.pendingRecords;
                ModifyObserver.pendingRecords = [];
                ModifyObserver.scheduledWork = false;
                
                Log.d(`开始批处理: 累计${recordsToProcess.length}条记录`, ModifyObserver.TAG);
                // 处理保存的记录
                ModifyObserver.processBatch(recordsToProcess);
            });
        } else {
            Log.d(`追加到待处理队列: 当前队列${ModifyObserver.pendingRecords.length}条`, ModifyObserver.TAG);
        }
    }
    
    /**
     * 批量处理累积的MutationRecord，减少DOM查询、合并操作、提前退出
     * @param records 待处理的记录数组
     */
    private static processBatch(records: MutationRecord[]): void {
        if (records.length === 0) {
            Log.d('批处理队列为空，跳过处理', ModifyObserver.TAG);
            return;
        }
        
        Log.d(`========== 开始批处理 ${records.length} 个变更记录 ==========`, ModifyObserver.TAG);
        
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
        
        Log.d(`记录分类: 移除${removeRecords.length}条, 添加${addRecords.length}条, 属性${attrRecords.length}条`, ModifyObserver.TAG);
        
        // STEP 1: 处理节点移除
        const hasValidRemove = ModifyObserver.handleRemove(removeRecords);

        // STEP 2: 计算动画延迟
        Log.d('开始计算动画延迟', ModifyObserver.TAG);
        let animationDuration = ModifyObserver.calculateAnimationDuration(addRecords, attrRecords);
        if(animationDuration > 0 ) {
            Log.d(`检测到动画，锁定布局并延迟 ${animationDuration}ms`, ModifyObserver.TAG);
            const timeoutId = setTimeout(() => {
                // 从Map中移除已完成的超时任务
                ModifyObserver.activeAnimationTimeouts.delete(timeoutId as any);
                Log.d(`动画延迟结束，解锁布局并触发任务`, ModifyObserver.TAG);
                ObserverHandler.postTask();
            }, animationDuration);
            // 注册超时任务到Map中
            ModifyObserver.activeAnimationTimeouts.set(timeoutId as any, document.body);
        }

        // STEP 3: 延迟处理节点添加，只在有添加记录时才设置定时器
        Log.d('开始处理节点添加', ModifyObserver.TAG);
        const hasValidAdd = ModifyObserver.handleAddedNodes(addRecords);
        // 如果animationDuration不为0，前面就已经posttask，所以此处只需要处理为0，且存在validchange的情况
        if(animationDuration === 0 && (hasValidRemove || hasValidAdd)) {
            Log.d(`立即触发任务 (无动画): 移除变更=${hasValidRemove}, 添加变更=${hasValidAdd}`, ModifyObserver.TAG);
            ObserverHandler.postTask();
        } else {
            Log.d(`跳过任务触发: 动画延迟=${animationDuration}ms, 移除变更=${hasValidRemove}, 添加变更=${hasValidAdd}`, ModifyObserver.TAG);
        }
        
        Log.d('========== 批处理完成 ==========', ModifyObserver.TAG);
    }

    private static handleAddedNodes(addRecords: MutationRecord[]): boolean {
        let hasValidChange = false;
        if (addRecords.length === 0) {
            Log.d('无节点添加记录', ModifyObserver.TAG);
            return false;
        }
        
        Log.d(`处理节点添加: ${addRecords.length} 条记录`, ModifyObserver.TAG);
        let dirtyCount = 0;
        
        // 批量处理，减少函数调用
        for (let i = 0; i < addRecords.length; i++) {
            let hasChange = ModifyObserver.checkElementAddNeedPostTask(addRecords[i]);
            // 只在有有效变更时才触发重布局
            if (hasChange) {
                hasValidChange = true;
                // 只标记一次dirty，而不是每个record都标记
                IntelligentLayout.markDirty(addRecords[i].target as HTMLElement);
                dirtyCount++;
            }
        }
        
        Log.d(`节点添加处理完成: 标记${dirtyCount}个dirty节点, 有效变更: ${hasValidChange}`, ModifyObserver.TAG);
        return hasValidChange;
    }

    private static calculateAnimationDuration(addRecords: MutationRecord[], attrRecords: MutationRecord[]): number {
        let animationDuration = 0;
        // 只在有添加或属性变更时才计算动画
        if (addRecords.length === 0 && attrRecords.length === 0) {
            Log.d('无添加或属性变更记录，跳过动画检测', ModifyObserver.TAG);
            return animationDuration;
        }
        
        Log.d(`开始动画检测: 添加记录${addRecords.length}条, 属性变更${attrRecords.length}条`, ModifyObserver.TAG);
        // 使用Set去重，避免重复计算同一元素
        const processedElements = new Set<HTMLElement>();

        // 处理属性变更的动画
        animationDuration = ModifyObserver.getAnimationDurationInAttriChange(attrRecords, processedElements, animationDuration);

        // 处理新增节点的动画
        animationDuration = ModifyObserver.getAnimationDurationInAddNodes(addRecords, processedElements, animationDuration);

        if (animationDuration > 0) {
            Log.d(`动画检测完成: 最大延迟 ${animationDuration}ms, 检测元素${processedElements.size}个`, ModifyObserver.TAG);
        } else {
            Log.d('未检测到动画效果', ModifyObserver.TAG);
        }
        return animationDuration;
    }

    private static getAnimationDurationInAddNodes(addRecords: MutationRecord[], processedElements: Set<HTMLElement>, animationDuration: number): number {
        let detectedCount = 0;
        for (let i = 0; i < addRecords.length; i++) {
            const addedNodes = addRecords[i].addedNodes;
            for (let j = 0; j < addedNodes.length; j++) {
                const node = addedNodes[j];
                if (node instanceof HTMLElement && !processedElements.has(node)) {
                    const duration = ModifyObserver.getDurationFromElement(node);
                    if (duration > 0) {
                        Log.d(`新增节点动画: ${node.tagName}.${node.className} = ${duration}ms`, ModifyObserver.TAG);
                        detectedCount++;
                    }
                    animationDuration = Math.max(duration, animationDuration);
                    processedElements.add(node);
                }
            }
        }
        if (detectedCount > 0) {
            Log.d(`新增节点动画检测: ${detectedCount}个节点有动画`, ModifyObserver.TAG);
        }
        return animationDuration;
    }

    private static getAnimationDurationInAttriChange(attrRecords: MutationRecord[], processedElements: Set<HTMLElement>, animationDuration: number): number {
        let detectedCount = 0;
        for (let i = 0; i < attrRecords.length; i++) {
            const target = attrRecords[i].target;
            const attributeName = attrRecords[i].attributeName;
            if (target instanceof HTMLElement && !processedElements.has(target)) {
                processedElements.add(target);
                const duration = ModifyObserver.getDurationFromElement(target);
                if (duration > 0) {
                    Log.d(`属性变更动画: ${target.tagName}.${target.className} (${attributeName}) = ${duration}ms`, ModifyObserver.TAG);
                    detectedCount++;
                }
                if (duration > animationDuration) {
                    animationDuration = duration;
                    IntelligentLayout.markDirty(target);
                }
            }
        }
        if (detectedCount > 0) {
            Log.d(`属性变更动画检测: ${detectedCount}个节点有动画`, ModifyObserver.TAG);
        }
        return animationDuration;
    }

    /**
     * 从单个元素中获取动画时长，直接返回最大值
     */
    static getDurationFromElement(element: HTMLElement): number {
        const animationInfo = ModifyObserver.getAnimDurations(element);
        const maxDuration = Math.max(animationInfo.animationDur, animationInfo.transitionDur);
        if (maxDuration > 0) {
            Log.d(`元素动画时长: animation=${animationInfo.animationDur}ms, transition=${animationInfo.transitionDur}ms, max=${maxDuration}ms`, ModifyObserver.TAG);
        }
        return maxDuration;
    }

    static cssTimeToMs(str: string): number {
        if (!str || str === '0s' || str === '0ms') {
            return 0;
        }
        const match = str.trim().match(/^([\d.]+)(s|ms)$/);
        if (!match) {
            Log.d(`无效的CSS时间格式: ${str}`, ModifyObserver.TAG);
            return 0;
        }
        const [, num, unit] = match;
        const ms = unit === 's' ? parseFloat(num) * 1000 : parseFloat(num);
        return ms;
    }
  
    // 获取元素当前正在运行的动画总时长（animation + transition）
    static getAnimDurations(el: HTMLElement): AnimationDurations {
        const cs = getComputedStyle(el);
        const animDur = ModifyObserver.cssTimeToMs(cs.animationDuration);
        const transDur = ModifyObserver.cssTimeToMs(cs.transitionDuration);
        const total = animDur + transDur;
        
        if (total > 0) {
            Log.d(`获取动画时长: ${el.tagName}.${el.className} - animation: ${cs.animationDuration}(${animDur}ms), transition: ${cs.transitionDuration}(${transDur}ms)`, ModifyObserver.TAG);
        }
        
        return { animationDur: animDur, transitionDur: transDur, total: total };
    }
}
