import Log from '../../Debug/Log';
import Tag from '../../Debug/Tag';

export default class Throttle {
    private taskCount = 0;
    private taskLock = false;

    private readonly timeOut;
    private readonly taskToRun;

    constructor(timeOut: number, task: Function) {
        this.timeOut = timeOut;
        this.taskToRun = task;
    }

    postTask(): void {
        // 缓冲区添加一个任务
        this.taskCount++;
        // 当任务未锁定时，执行任务
        if (!this.taskLock) {
            this.lockAndRun();
        }
    }

    private taskUnlock(from: this): () => void {
        return function () {
            // 解锁任务
            from.taskLock = false;
            Log.i(null, '解锁: ' + Date.now(), Tag.framework);
            // 缓冲区有任务，则触发mainTask
            if (from.taskCount > 0) {
                from.lockAndRun();
            }
        };
    }

    private lockAndPost(): void {
        Log.i(null, '加锁: ' + Date.now(), Tag.framework);
        this.taskLock = true; // 锁定任务
        this.taskCount = 0; // 清空缓冲区
    }

    private lockAndRun(): void {
        const start = new Date().getTime();
        this.lockAndPost();

        this.taskToRun();
        const end = new Date().getTime();
        const time = end - start;
        window.setTimeout(this.taskUnlock(this), Math.min(this.timeOut,time)); // 延迟一段时间后解锁
    }
}
