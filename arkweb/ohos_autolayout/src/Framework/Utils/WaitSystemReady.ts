import Tag from '../../Debug/Tag';
import Log from '../../Debug/Log';
import { Main } from '../../Main';

export default class WaitSystemReady {
    private static tryTimes = 1000;
    static hasBodyReady = false;
    private static tryHead = 100;
    static sysReady:boolean = false;
    static elementCount:number = 0;

    static bodyReady(task: Function): void {
        Log.d('========== 等待Body就绪 ==========', Tag.framework);
        
        (function wait(): void {
            if (!document.body || innerWidth <= 100) {
                const reason = !document.body ? 'document.body不存在' : `窗口宽度过小(${innerWidth}px)`;
                Log.d(`⏳ Body未就绪: ${reason}, 剩余尝试次数: ${WaitSystemReady.tryTimes}`, Tag.framework);
                
                if (WaitSystemReady.tryTimes >= 0) {
                    setTimeout(wait, 50);
                } else {
                    Log.e('❌ Body就绪检测超时，已达最大尝试次数', Tag.framework);
                }
                WaitSystemReady.tryTimes--;
                return;
            }
            
            Log.info(`✅ Body就绪: innerWidth=${innerWidth}px`, Tag.framework);
            WaitSystemReady.hasBodyReady = true;
            
            Log.d('执行Body就绪回调任务', Tag.framework);
            task();
        })();
    }

    static viewportReady(task: Function): void {
        Log.d('========== 等待Viewport就绪 ==========', Tag.framework);
        
        let waitViewport: MutationObserver;
        if (viewportCheck()) {
            return;
        }

        Log.d('Viewport不存在，启动MutationObserver监听...', Tag.framework);
        waitViewport = new MutationObserver(() => {
            Log.d('检测到head子节点变化，检查viewport', Tag.framework);
            viewportCheck();
        });
        waitViewport.observe(document.head, {
            childList: true,
        });

        function viewportCheck(): boolean {
            let viewport = document.querySelector('meta[name="viewport"]');
            if (!viewport) {
                Log.d('⏳ Viewport meta标签仍不存在', Tag.framework);
                return false;
            }

            const content = viewport.getAttribute('content');
            Log.info(`✅ Viewport就绪: content="${content}"`, Tag.framework);

            if (waitViewport) {
                Log.d('断开Viewport观察器', Tag.framework);
                waitViewport.disconnect();
                waitViewport = null;
            }

            Log.d('执行Viewport就绪回调任务', Tag.framework);
            task();
            return true;
        }
    }

    private static checkDOMReady(root:HTMLElement): boolean {
        if (!root) {
            Log.d('DOM检查: root不存在', Tag.framework);
            return false;
        }
        
        const childCount = root.children.length;
        Log.d(`DOM检查: root有${childCount}个子节点`, Tag.framework);
        return true;
    }

    static waitForSystemReady(): void {
        Log.d('========== 等待系统就绪 ==========', Tag.framework);
        
        // check DOM ready?
        WaitSystemReady.sysReady = WaitSystemReady.checkDOMReady(document.body);

        if (WaitSystemReady.sysReady === false) {
            Log.d('⏳ 系统未就绪，200ms后重试', Tag.framework);
            setTimeout(WaitSystemReady.waitForSystemReady, 200);
        } else {
            Log.info('✅ 系统就绪，启动Main.start_', Tag.framework);
            Main.start_();
        }
    }

    static headReady(task: Function): void {
        Log.d('========== 等待Head就绪 ==========', Tag.framework);
        
        WaitSystemReady.tryHead--;
        
        if (WaitSystemReady.tryHead < 0) {
            Log.e('❌ Head就绪检测超时，已达最大尝试次数', Tag.framework);
            return;
        }

        if (!document.head) {
            Log.d(`⏳ Head不存在，10ms后重试 (剩余尝试次数: ${WaitSystemReady.tryHead})`, Tag.framework);
            setTimeout(() => {
                this.headReady(task);
            }, 10);
            return;
        }

        const linkCount = document.head.querySelectorAll('link').length;
        const styleCount = document.head.querySelectorAll('style').length;
        Log.info(`✅ Head就绪: ${linkCount}个link, ${styleCount}个style`, Tag.framework);
        
        Log.d('执行Head就绪回调任务', Tag.framework);
        task();
    }

    static configReady(task: Function):void {
        Log.d('========== 配置就绪 ==========', Tag.framework);
        Log.d('执行配置就绪回调任务', Tag.framework);
        task();
    }
}
