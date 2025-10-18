import Log from '../../Debug/Log';
import Tag from '../../Debug/Tag';

/**
 * Throttle 节流器: 限制任务执行频率，确保在指定时间间隔内最多执行一次
 * 使用场景：
 * - DOM 事件节流（resize, scroll, input 等）
 * - 频繁的 API 调用限流
 * - 高频状态更新的性能优化
 */
export default class Throttle {
    private static readonly TAG = Tag.throttle;
    private static readonly MIN_TIMEOUT = 10; // 最小节流时间 10ms
    
    private taskCount = 0;          // 待执行任务计数
    private isLocked = false;       // 任务锁状态
    private timerId: number | null = null;  // 定时器 ID
    
    private readonly timeout: number;       // 节流时间间隔
    private readonly taskToRun: Function;   // 要执行的任务
    
    /**
     * 构造函数
     * @param timeout 节流时间间隔（毫秒），最小值 10ms
     * @param task 要执行的任务函数
     */
    constructor(timeout: number, task: Function) {
        // 参数验证
        if (typeof task !== 'function') {
            throw new TypeError('Throttle: task must be a function');
        }
        
        if (typeof timeout !== 'number' || timeout < Throttle.MIN_TIMEOUT) {
            Log.w(`Throttle: timeout must be >= ${Throttle.MIN_TIMEOUT}ms, got ${timeout}. Using ${Throttle.MIN_TIMEOUT}ms`, Throttle.TAG);
            this.timeout = Throttle.MIN_TIMEOUT;
        } else {
            this.timeout = timeout;
        }
        
        this.taskToRun = task;
        
        Log.d(`Throttle created with timeout: ${this.timeout}ms`, Throttle.TAG);
    }

    /**
     * 提交任务到节流队列
     * 如果当前没有任务在执行，立即执行
     * 如果有任务在执行，累加计数，等待解锁后执行
     */
    postTask(): void {
        this.taskCount++;
        
        Log.d(`postTask called, taskCount: ${this.taskCount}, isLocked: ${this.isLocked}`, Throttle.TAG);
        
        // 如果未锁定，立即执行
        if (!this.isLocked) {
            this.executeTask();
        }
        // 否则等待解锁后自动执行
    }

    /**
     * 取消所有待执行的任务
     * 清空任务计数，但不会中断正在执行的任务
     */
    cancel(): void {
        Log.d('cancel called', Throttle.TAG);
        this.taskCount = 0;
        
        // 如果有待执行的定时器，清除它
        if (this.timerId !== null) {
            clearTimeout(this.timerId);
            this.timerId = null;
        }
    }

    /**
     * 销毁节流器
     * 清理所有资源，取消待执行任务
     */
    destroy(): void {
        Log.d('destroy called', Throttle.TAG);
        this.cancel();
        this.isLocked = false;
    }

    /**
     * 执行任务
     * 私有方法，负责实际的任务执行和节流控制
     */
    private executeTask(): void {
        const startTime = Date.now();
        
        // 加锁，防止重复执行
        this.lock();
        
        try {
            // 执行任务
            this.taskToRun();
            
            const executionTime = Date.now() - startTime;
            Log.d(`Task executed in ${executionTime}ms`, Throttle.TAG);
            
            // 调度解锁
            this.scheduleUnlock(startTime);
            
        } catch (error) {
            // 错误处理：确保即使任务失败，也能解锁
            Log.e(`Task execution failed: ${error}`, Throttle.TAG, error as Error);
            
            // 立即解锁，避免死锁
            this.unlock();
        }
    }

    /**
     * 加锁并清空任务计数
     */
    private lock(): void {
        Log.d(`Lock at ${Date.now()}`, Throttle.TAG);
        this.isLocked = true;
        this.taskCount = 0;
    }

    /**
     * 调度解锁操作
     * 修复：使用固定的 timeout，从任务开始时间计算
     * @param startTime 任务开始时间
     */
    private scheduleUnlock(startTime: number): void {
        // 计算应该在何时解锁
        const unlockTime = startTime + this.timeout;
        const now = Date.now();
        const delay = Math.max(0, unlockTime - now);
        
        Log.d(`Schedule unlock in ${delay}ms (timeout: ${this.timeout}ms)`, Throttle.TAG);
        
        // 清除之前的定时器（如果有）
        if (this.timerId !== null) {
            clearTimeout(this.timerId);
        }
        
        // 设置新的定时器
        this.timerId = window.setTimeout(() => {
            this.timerId = null;
            this.unlock();
        }, delay);
    }

    /**
     * 解锁
     * 如果有待执行任务，继续执行
     */
    private unlock(): void {
        Log.d(`Unlock at ${Date.now()}, pending tasks: ${this.taskCount}`, Throttle.TAG);
        
        this.isLocked = false;
        
        // 如果有待执行的任务，继续执行
        if (this.taskCount > 0) {
            // 使用 setTimeout(0) 确保在下一个事件循环执行
            // 避免递归调用栈过深
            setTimeout(() => {
                if (this.taskCount > 0 && !this.isLocked) {
                    this.executeTask();
                }
            }, 0);
        }
    }

    /**
     * 获取当前状态（用于调试）
     */
    getState(): { isLocked: boolean; taskCount: number; timeout: number } {
        return {
            isLocked: this.isLocked,
            taskCount: this.taskCount,
            timeout: this.timeout,
        };
    }
}
