import Log from '../../Debug/Log';
import Tag from '../../Debug/Tag';
import ObserverHandler from '../Observer/ObserverHandler';
import { PopupLayoutState, isValidTransition, getStateDescription, getStateIcon } from './PopupLayoutState';

/**
 * 弹窗状态管理器
 * 负责管理弹窗的布局状态，确保状态转换的合法性，避免重复布局
 */
export class PopupStateManager {
    private static TAG = Tag.popupStateManager;
    
    // 每个弹窗元素的状态
    private static stateMap = new WeakMap<HTMLElement, PopupLayoutState>();
    
    // 状态变化的时间戳
    private static timestampMap = new WeakMap<HTMLElement, number>();
    
    /**
     * 获取弹窗的当前状态
     */
    static getState(popupRoot: HTMLElement): PopupLayoutState {
        if (!popupRoot) {
            Log.w('获取状态失败：弹窗根节点为空', PopupStateManager.TAG);
            return PopupLayoutState.IDLE;
        }
        
        const state = this.stateMap.get(popupRoot) || PopupLayoutState.IDLE;
        return state;
    }
    
    /**
     * 设置弹窗的状态
     * 会验证状态转换的合法性
     */
    static setState(popupRoot: HTMLElement, newState: PopupLayoutState, reason?: string): boolean {
        if (!popupRoot) {
            Log.w('设置状态失败：弹窗根节点为空', PopupStateManager.TAG);
            return false;
        }
        
        const oldState = this.getState(popupRoot);
        
        // 如果状态没有变化，跳过
        if (oldState === newState) {
            Log.d(`状态未变化: ${getStateIcon(newState)} ${getStateDescription(newState)}`, PopupStateManager.TAG);
            return true;
        }
        
        // 验证状态转换的合法性
        if (!isValidTransition(oldState, newState)) {
            Log.e(`❌ 非法状态转换: ${getStateIcon(oldState)} ${getStateDescription(oldState)} -> ${getStateIcon(newState)} ${getStateDescription(newState)}`, PopupStateManager.TAG);
            return false;
        }
        
        // 执行状态转换
        this.stateMap.set(popupRoot, newState);
        this.timestampMap.set(popupRoot, Date.now());
        
        const reasonText = reason ? ` (原因: ${reason})` : '';
        Log.info(`🔄 状态转换: ${getStateIcon(oldState)} ${getStateDescription(oldState)} -> ${getStateIcon(newState)} ${getStateDescription(newState)}${reasonText}`, PopupStateManager.TAG);
        
        if (newState !== PopupLayoutState.COMPLETED && newState !== PopupLayoutState.FAILED && newState !== PopupLayoutState.RESTORED) {
            ObserverHandler.postTask();
        }
        return true;
    }
    
    /**
     * 重置弹窗状态为 IDLE
     * 用于 resize 或需要重新布局的场景
     */
    static resetState(popupRoot: HTMLElement, reason: string): void {
        if (!popupRoot) {
            return;
        }
        
        const currentState = this.getState(popupRoot);
        Log.info(`♻️ 重置状态: ${getStateIcon(currentState)} ${getStateDescription(currentState)} -> ${getStateIcon(PopupLayoutState.IDLE)} ${getStateDescription(PopupLayoutState.IDLE)} (原因: ${reason})`, PopupStateManager.TAG);
        
        this.stateMap.set(popupRoot, PopupLayoutState.IDLE);
        this.timestampMap.set(popupRoot, Date.now());
    }
    
    /**
     * 清除弹窗状态
     * 用于弹窗被移除时
     */
    static clearState(popupRoot: HTMLElement): void {
        if (!popupRoot) {
            return;
        }
        
        const currentState = this.getState(popupRoot);
        Log.d(`🗑️ 清除状态: ${getStateIcon(currentState)} ${getStateDescription(currentState)}`, PopupStateManager.TAG);
        
        this.stateMap.delete(popupRoot);
        this.timestampMap.delete(popupRoot);
    }
    
    /**
     * 检查是否可以开始布局
     */
    static canStartLayout(popupRoot: HTMLElement): boolean {
        const state = this.getState(popupRoot);
        const canStart = (state === PopupLayoutState.IDLE || state === PopupLayoutState.PREPROCESSING);
        
        if (!canStart) {
            Log.d(`⛔ 不能开始布局: 当前状态 ${getStateIcon(state)} ${getStateDescription(state)}`, PopupStateManager.TAG);
        }
        
        return canStart;
    }
    
    /**
     * 检查是否正在处理中
     */
    static isProcessing(popupRoot: HTMLElement): boolean {
        const state = this.getState(popupRoot);
        return state === PopupLayoutState.WAITING_ANIMATION ||
               state === PopupLayoutState.PREPROCESSING ||
               state === PopupLayoutState.LAYOUTING ||
               state === PopupLayoutState.WAITING_VALIDATION ||
               state === PopupLayoutState.VALIDATING;
    }
    
    /**
     * 检查是否已完成（终态）
     */
    static isCompleted(popupRoot: HTMLElement): boolean {
        const state = this.getState(popupRoot);
        return state === PopupLayoutState.COMPLETED || 
               state === PopupLayoutState.RESTORED;
    }
    
    /**
     * 获取状态持续时间（毫秒）
     */
    static getStateDuration(popupRoot: HTMLElement): number {
        if (!popupRoot) {
            return 0;
        }
        
        const timestamp = this.timestampMap.get(popupRoot);
        if (!timestamp) {
            return 0;
        }
        
        return Date.now() - timestamp;
    }
    
    /**
     * 打印当前状态（调试用）
     */
    static printState(popupRoot: HTMLElement, label?: string): void {
        if (!popupRoot) {
            return;
        }
        
        const state = this.getState(popupRoot);
        const duration = this.getStateDuration(popupRoot);
        const prefix = label ? `[${label}] ` : '';
        
        Log.d(`${prefix}当前状态: ${getStateIcon(state)} ${getStateDescription(state)}, 持续时间: ${duration}ms`, PopupStateManager.TAG);
    }
}
