import StyleCleaner from '../../Common/Style/Setter/StyleCleaner';
import Log from '../../../Debug/Log';
import Tag from '../../../Debug/Tag';
import IntelligentLayout from '../../../Framework/IntelligentLayout';
import ObserverHandler from '../ObserverHandler';
import Constant from '../../Common/Constant';
import { PopupStateManager } from '../../Popup/PopupStateManager';
import { PopupLayoutState } from '../../Popup/PopupLayoutState';
import { PopupInfo } from '../../Popup/PopupInfo';
import { PopupWindowDetector } from '../../Popup/PopupWindowDetector';
import { PopupWindowRelayout } from '../../Popup/PopupWindowRelayout';

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
    private static activeAnimationTimeouts: Map<NodeJS.Timeout, HTMLElement> = new Map();

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
        ModifyObserver.animationStartHandler = (event: AnimationEvent): void => {
            // 将 'animation' 作为事件类型传递
            ModifyObserver.handleAnimationEvent(event, 'animation');
        };
        
        // 创建并保存 transition 监听器
        ModifyObserver.transitionStartHandler = (event: TransitionEvent): void => {
            // 将 'transition' 作为事件类型传递
            ModifyObserver.handleAnimationEvent(event, 'transition');
        };
        
        // 添加监听器
        document.body.addEventListener('animationstart', ModifyObserver.animationStartHandler, true);
        document.body.addEventListener('transitionstart', ModifyObserver.transitionStartHandler, true);
        
        ModifyObserver.animationListenerAdded = true;
        Log.info('CSS动画事件监听器添加成功', ModifyObserver.TAG);
    }

    /**
     * animationstart 和 transitionstart 事件的通用处理逻辑。
     * @param event - 动画或过渡事件。
     * @param eventType - 用于日志和状态管理的事件类型 ('animation' | 'transition')。
     */
    private static handleAnimationEvent(event: AnimationEvent | TransitionEvent, eventType: 'animation' | 'transition'): void {
        const target = event.target as HTMLElement;

        // 打印特定的日志
        if (eventType === 'animation' && event instanceof AnimationEvent) {
            Log.d(`🎬 CSS动画开始: ${target.tagName}.${target.className}, 动画名: ${event.animationName}`, ModifyObserver.TAG);
        } else if (eventType === 'transition' && event instanceof TransitionEvent) {
            Log.d(`🎬 CSS过渡开始: ${target.tagName}.${target.className}, 属性: ${event.propertyName}`, ModifyObserver.TAG);
        }

        const popupRoot = ModifyObserver.findPopupRoot(target);
        
        // 更新弹窗状态
        ModifyObserver.updatePopupStateOnAnimationStart(popupRoot, eventType);
        
        const duration = ModifyObserver.getDurationFromElement(target);
        
        // 设置超时
        ModifyObserver.createAnimationTimeout(target, popupRoot, duration, eventType);
    }

    /**
     * 【辅助函数】 根据动画事件更新弹窗状态。
     * @param popupRoot - 弹窗根节点。
     * @param eventType - 事件类型 ('animation' | 'transition')。
     */
    private static updatePopupStateOnAnimationStart(popupRoot: HTMLElement | null, eventType: 'animation' | 'transition'): void {
        if (!popupRoot) {
            return;
        }
        
        const currentState = PopupStateManager.getState(popupRoot);
        // 只有在IDLE状态才设置为WAITING_ANIMATION，避免覆盖正在进行的布局流程
        if (currentState === PopupLayoutState.IDLE) {
            const reason = eventType === 'animation' ? '检测到动画开始' : '检测到过渡开始';
            PopupStateManager.setState(popupRoot, PopupLayoutState.WAITING_ANIMATION, reason);
        } else {
            Log.d(`弹窗当前状态为 ${PopupLayoutState[currentState]}，跳过设置WAITING_ANIMATION`, ModifyObserver.TAG);
        }
    }

    /**
     * 【辅助函数】 为检测到的动画创建延迟回调，以在动画结束后解锁布局。
     * @param target - 动画的目标元素。
     * @param popupRoot - 弹窗根节点。
     * @param duration - 动画时长。
     * @param eventType - 事件类型 ('animation' | 'transition')。
     */
    private static createAnimationTimeout(
        target: HTMLElement, 
        popupRoot: HTMLElement | null, 
        duration: number, 
        eventType: 'animation' | 'transition'
    ): void {
        
        if (duration <= 0) {
            return;
        }

        Log.d(`锁定布局，延迟 ${duration}ms`, ModifyObserver.TAG);

        const timeoutId: NodeJS.Timeout = setTimeout(() => {
            ModifyObserver.activeAnimationTimeouts.delete(timeoutId);
            
            const logMessage = eventType === 'animation' ? 'CSS动画延迟结束' : 'CSS过渡延迟结束';
            Log.d(`${logMessage}，解锁布局`, ModifyObserver.TAG);
            
            if (popupRoot && PopupStateManager.getState(popupRoot) === PopupLayoutState.WAITING_ANIMATION) {
                const reason = eventType === 'animation' ? '动画完成' : '过渡完成';
                PopupStateManager.setState(popupRoot, PopupLayoutState.IDLE, reason);
            }
            
            ObserverHandler.postTask();
        }, duration);
        
        ModifyObserver.activeAnimationTimeouts.set(timeoutId, target);
    }
    
    /**
     * 查找元素所属的弹窗根节点
     * @private
     */
    private static findPopupRoot(element: HTMLElement): HTMLElement | null {
        const popupInfo = IntelligentLayout.getActivePopupWindowInfo();
        if (popupInfo?.root_node && popupInfo.root_node.contains(element)) {
            return popupInfo.root_node;
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
        
        const isMaskNodeExistence = ModifyObserver.checkMaskNodeExistence();

        if (!isMaskNodeExistence) {
            Log.d('检测到遮罩节点失效，已重置弹窗', ModifyObserver.TAG);
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
            const timeoutId: NodeJS.Timeout = setTimeout(() => {
                // 从Map中移除已完成的超时任务
                ModifyObserver.activeAnimationTimeouts.delete(timeoutId);
                Log.d(`动画延迟结束，解锁布局并触发任务`, ModifyObserver.TAG);
                ObserverHandler.postTask();
            }, animationDuration);
            // 注册超时任务到Map中
            ModifyObserver.activeAnimationTimeouts.set(timeoutId, document.body);
        }

        // STEP 3: 通用处理属性变更
        // 只要 class 或 style 发生变化，都视为可能的布局/显隐改变
        const hasValidAttr = ModifyObserver.handleAttributeChanges(attrRecords);

        // STEP 4: 延迟处理节点添加，只在有添加记录时才设置定时器
        Log.d('开始处理节点添加', ModifyObserver.TAG);
        const hasValidAdd = ModifyObserver.handleAddedNodes(addRecords);
        // 如果animationDuration不为0，前面就已经posttask，所以此处只需要处理为0，且存在validchange的情况
        if(animationDuration === 0 && (hasValidRemove || hasValidAdd || hasValidAttr)) {
            Log.d(`立即触发任务 (无动画): 移除变更=${hasValidRemove}, 添加变更=${hasValidAdd}, 属性变更=${hasValidAttr}`, ModifyObserver.TAG);
            ObserverHandler.postTask();
        } else {
            Log.d(`跳过任务触发: 动画延迟=${animationDuration}ms, 移除变更=${hasValidRemove}, 添加变更=${hasValidAdd}, 属性变更=${hasValidAttr}`, ModifyObserver.TAG);
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

    /**
     * 通用属性变更处理
     * 只要涉及到 class 或 style 的变化，都标记为有效变更，触发后续检查
     */
    private static handleAttributeChanges(attrRecords: MutationRecord[]): boolean {
        let hasValidChange = false;
        if (attrRecords.length === 0) {
            return false;
        }

        let dirtyCount = 0;
        for (const record of attrRecords) {
            // 关注 style 的变化，其会影响显隐和布局
            if (record.attributeName === 'style') {
                const target = record.target as HTMLElement;
                // 标记为脏节点，通知 IntelligentLayout 该节点可能需要重新评估
                IntelligentLayout.markDirty(target);
                hasValidChange = true;
                dirtyCount++;
            }
        }
        
        if (hasValidChange) {
             Log.d(`属性变更处理完成: 标记${dirtyCount}个涉及class/style变化的节点`, ModifyObserver.TAG);
        }
        
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

    /**
     * 检查单个新增节点，提取其动画时长，并更新最大时长。
     *
     * @param node - (来自 MutationRecord.addedNodes) 需要检查的 DOM 节点。
     * @param processedElements - 一个 Set，用于防止重复处理同一个 HTMLElement。
     * @param currentMaxDuration - 到目前为止在所有节点中发现的最长动画时长。
     * @returns 一个对象，包含：
     * `newDuration`: 更新后的最大动画时长（比较 currentMaxDuration 和当前节点时长后的结果）。
     * `detected`: 一个布尔值，如果这个节点包含大于0的动画时长，则为 true。
     */
    private static processAddedNode(
        node: Node,
        processedElements: Set<HTMLElement>,
        currentMaxDuration: number
    ): { newDuration: number, detected: boolean } {

        // 卫语句：如果不是 HTMLElement，或者已经处理过，则提前返回
        if (!(node instanceof HTMLElement) || processedElements.has(node)) {
            return { newDuration: currentMaxDuration, detected: false };
        }

        // 标记为已处理
        processedElements.add(node);
        
        const duration = ModifyObserver.getDurationFromElement(node);
        let detected: boolean = false;

        if (duration > 0) {
            Log.d(`新增节点动画: ${node.tagName}.${node.className} = ${duration}ms`, ModifyObserver.TAG);
            detected = true;
        }
        
        // 返回更新后的最大时长，以及是否在本节点检测到了动画
        return { 
            newDuration: Math.max(duration, currentMaxDuration), 
            detected: detected 
        };
    }

    /**
     * 遍历所有 MutationRecord 中的 addedNodes，找出其中包含的最长 CSS 动画/过渡时长。
     *
     * @param addRecords - MutationObserver 捕获到的所有新增记录。
     * @param processedElements - 一个 Set，用于在整个批次中跟踪已处理的节点，防止重复计算。
     * @param animationDuration - 初始的动画时长（通常为0或上一个批次的值），用于比较。
     * @returns 在所有新增节点中找到的最长动画时长。
     */
    private static getAnimationDurationInAddNodes(addRecords: MutationRecord[], processedElements: Set<HTMLElement>, animationDuration: number): number {
        let detectedCount: number = 0;
        
        for (let i = 0; i < addRecords.length; i++) {
            const addedNodes: NodeList = addRecords[i].addedNodes;

            for (let j = 0; j < addedNodes.length; j++) {
                
                const result = ModifyObserver.processAddedNode(
                    addedNodes[j], 
                    processedElements, 
                    animationDuration
                );

                // 更新当前已知的最大时长
                animationDuration = result.newDuration;
                
                if (result.detected) {
                    detectedCount++;
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
        const timeParts = str.split(',')
                            .map(part => part.trim())
                            .filter(part => part.length > 0);
        const timeRegex = /^([\d.]+)(s|ms)$/; 
        let maxTime: number = 0;

        for (const part of timeParts) {
            const match = part.trim().match(timeRegex);
            if (!match) {
                Log.d(`无效的CSS时间格式: ${part}`, ModifyObserver.TAG);
                return 0;
            }
            const [, num, unit] = match;
            const msUnitTime = unit === 's' ? parseFloat(num) * 1000 : parseFloat(num);
            maxTime = Math.max(maxTime, msUnitTime);
        }

        return maxTime;
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

    /**
     * 检查Mask节点是否存在
     * 
     * @returns Mask节点存在则返回 true，否则返回 false
     */
    private static checkMaskNodeExistence(): boolean {
        const popupInfo = IntelligentLayout.getActivePopupWindowInfo();
        if (!popupInfo) {
            return true;
        }

        if (!ModifyObserver.isMaskNodeValid(popupInfo)) {
            const maskClass = popupInfo?.mask_node?.className || 'unknown-mask';
            const reason = `检测到遮罩节点失效: ${maskClass}`;
            ModifyObserver.resetPopup(reason);
            ObserverHandler.postTask();
            return false;
        }

        return true;
    }

    private static isMaskNodeValid(popupInfo: PopupInfo): boolean {
        if (!popupInfo) {
            return false;
        }

        if (!popupInfo.root_node || !popupInfo.root_node.isConnected) {
            Log.d('弹窗根节点已脱离文档结构', ModifyObserver.TAG);
            return false;
        }

        if (!PopupWindowDetector.isMaskNodeActive(popupInfo.mask_node)) {
            return false;
        }

        return true;
    }

    private static resetPopup(reason: string): void {
        Log.d(`重置弹窗: ${reason}`, ModifyObserver.TAG);

        const popupInfo = IntelligentLayout.getActivePopupWindowInfo();
        const component = IntelligentLayout.getActivePopupWindowComponent();

        if (component instanceof PopupWindowRelayout) {
            component.cancelPendingValidation();
            component.restoreStyles();
        }

        if (popupInfo?.root_node) {
            PopupStateManager.resetState(popupInfo.root_node, reason);
        }

        IntelligentLayout.clearActivePopupWindow();
    }
}
