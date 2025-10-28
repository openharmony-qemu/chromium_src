/**
 * 弹窗布局状态枚举
 * 用于管理弹窗在布局流水线中的状态，避免重复布局
 */
export enum PopupLayoutState {
    /** 未布局：初始状态，弹窗尚未进行布局处理 */
    IDLE = 'IDLE',
    
    /** 等待动画完成：弹窗正在播放动画，需要等待动画结束后再进行布局 */
    WAITING_ANIMATION = 'WAITING_ANIMATION',
    
    /** 布局中：正在执行布局操作（检测、计算、应用样式） */
    LAYOUTING = 'LAYOUTING',
    
    /** 等待验证：布局已完成，等待16帧后进行约束验证 */
    WAITING_VALIDATION = 'WAITING_VALIDATION',
    
    /** 验证中：正在进行布局约束验证 */
    VALIDATING = 'VALIDATING',
    
    /** 已完成：布局成功并通过验证 */
    COMPLETED = 'COMPLETED',
    
    /** 已恢复：布局验证失败，已恢复到原始状态 */
    RESTORED = 'RESTORED',
    
    /** 失败：布局过程中发生错误 */
    FAILED = 'FAILED'
}

/**
 * 状态转换规则
 * 定义了哪些状态可以转换到哪些状态
 */
export const StateTransitionRules: Record<PopupLayoutState, PopupLayoutState[]> = {
    [PopupLayoutState.IDLE]: [
        PopupLayoutState.WAITING_ANIMATION,
        PopupLayoutState.LAYOUTING,
        PopupLayoutState.FAILED
    ],
    [PopupLayoutState.WAITING_ANIMATION]: [
        PopupLayoutState.LAYOUTING,
        PopupLayoutState.IDLE,
        PopupLayoutState.FAILED
    ],
    [PopupLayoutState.LAYOUTING]: [
        PopupLayoutState.WAITING_VALIDATION,
        PopupLayoutState.IDLE, // 允许 resize 中断
        PopupLayoutState.COMPLETED,
        PopupLayoutState.FAILED
    ],
    [PopupLayoutState.WAITING_VALIDATION]: [
        PopupLayoutState.VALIDATING,
        PopupLayoutState.IDLE, // 允许 resize 中断
        PopupLayoutState.FAILED
    ],
    [PopupLayoutState.VALIDATING]: [
        PopupLayoutState.COMPLETED,
        PopupLayoutState.RESTORED,
        PopupLayoutState.IDLE, // 允许 resize 中断
        PopupLayoutState.FAILED
    ],
    [PopupLayoutState.COMPLETED]: [
        PopupLayoutState.IDLE // resize 时重置
    ],
    [PopupLayoutState.RESTORED]: [
        PopupLayoutState.IDLE // resize 时重置
    ],
    [PopupLayoutState.FAILED]: [
        PopupLayoutState.IDLE // 允许重试
    ]
};

/**
 * 判断当前状态是否允许进行新的布局操作
 */
export function canStartLayout(state: PopupLayoutState): boolean {
    return state === PopupLayoutState.IDLE;
}

/**
 * 判断当前状态是否是终态（不会再变化）
 */
export function isTerminalState(state: PopupLayoutState): boolean {
    return state === PopupLayoutState.COMPLETED || 
           state === PopupLayoutState.RESTORED;
}

/**
 * 判断当前状态是否正在处理中（不应该重复触发）
 */
export function isProcessing(state: PopupLayoutState): boolean {
    return state === PopupLayoutState.WAITING_ANIMATION ||
           state === PopupLayoutState.LAYOUTING ||
           state === PopupLayoutState.WAITING_VALIDATION ||
           state === PopupLayoutState.VALIDATING;
}

/**
 * 验证状态转换是否合法
 */
export function isValidTransition(from: PopupLayoutState, to: PopupLayoutState): boolean {
    const allowedStates = StateTransitionRules[from];
    return allowedStates.includes(to);
}

/**
 * 获取状态的中文描述
 */
export function getStateDescription(state: PopupLayoutState): string {
    const descriptions: Record<PopupLayoutState, string> = {
        [PopupLayoutState.IDLE]: '未布局',
        [PopupLayoutState.WAITING_ANIMATION]: '等待动画完成',
        [PopupLayoutState.LAYOUTING]: '布局中',
        [PopupLayoutState.WAITING_VALIDATION]: '等待验证',
        [PopupLayoutState.VALIDATING]: '验证中',
        [PopupLayoutState.COMPLETED]: '已完成',
        [PopupLayoutState.RESTORED]: '已恢复',
        [PopupLayoutState.FAILED]: '失败'
    };
    return descriptions[state];
}

/**
 * 获取状态的图标
 */
export function getStateIcon(state: PopupLayoutState): string {
    const icons: Record<PopupLayoutState, string> = {
        [PopupLayoutState.IDLE]: '⚪',
        [PopupLayoutState.WAITING_ANIMATION]: '⏳',
        [PopupLayoutState.LAYOUTING]: '🔄',
        [PopupLayoutState.WAITING_VALIDATION]: '⏰',
        [PopupLayoutState.VALIDATING]: '🔍',
        [PopupLayoutState.COMPLETED]: '✅',
        [PopupLayoutState.RESTORED]: '↩️',
        [PopupLayoutState.FAILED]: '❌'
    };
    return icons[state];
}
