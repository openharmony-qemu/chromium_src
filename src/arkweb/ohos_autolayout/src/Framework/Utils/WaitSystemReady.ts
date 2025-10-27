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
        (function wait(): void {
            if (!document.body || innerWidth <= 100) {
                Log.d('body null' + innerWidth, Tag.framework);
                if (WaitSystemReady.tryTimes >= 0) {
                    setTimeout(wait, 50);
                }
                WaitSystemReady.tryTimes--;
                return;
            }
            WaitSystemReady.hasBodyReady = true;
            task();
        })();
    }

    static viewportReady(task: Function): void {
        let waitViewport: MutationObserver;
        if (viewportCheck()) {
            return;
        }

        Log.d('viewport observe');
        waitViewport = new MutationObserver(viewportCheck);
        waitViewport.observe(document.head, {
            childList: true,
        });

        function viewportCheck(): boolean {
            let viewport = document.querySelector('meta[name="viewport"]');
            if (!viewport) {
                Log.d('viewport null');
                return false;
            }

            Log.d('viewport ready');

            if (waitViewport) {
                Log.d('viewportCheck disconnect');
                waitViewport.disconnect();
                waitViewport = null;
            }

            task();
            return true;
        }
    }

    private static checkDOMReady(root:HTMLElement): boolean {
        return true;
    }

    static waitForSystemReady(): void {
        Log.d('waitForSystemReady');
        // check DOM ready?
        WaitSystemReady.sysReady = WaitSystemReady.checkDOMReady(document.body);

        if (WaitSystemReady.sysReady === false) {
            setTimeout(WaitSystemReady.waitForSystemReady, 200);
        } else {
            Log.d('waitForSystemReady run main.start');
            Main.start_();
        }
    }

    static headReady(task: Function): void {
        WaitSystemReady.tryHead--;
        if (WaitSystemReady.tryHead < 0) {
            return;
        }

        if (!document.head) {
            setTimeout(() => {
                this.headReady(task);
            }, 10);
            return;
        }

        task();
    }

    static configReady(task: Function):void {
        task();
    }
}
