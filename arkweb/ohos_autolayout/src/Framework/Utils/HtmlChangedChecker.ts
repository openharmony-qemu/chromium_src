import WaitSystemReady from './WaitSystemReady';
import Framework from '../Framework';

export default class HtmlChangedChecker {
    static initHtml: HTMLElement = null;
    static tryTimes = 7;
    static TIMEOUT = 200;

    private static checkAndRestart(): boolean {
        if (!HtmlChangedChecker.initHtml) {
            HtmlChangedChecker.initHtml = document.documentElement;
            return false;
        }

        if (HtmlChangedChecker.initHtml !== document.documentElement) {
            WaitSystemReady.configReady(Framework.configReady);
            return true;
        }
        return false;
    }

    static startCheckHtml():void {
        HtmlChangedChecker.tryTimes--;
        if (HtmlChangedChecker.tryTimes < 0) {
            return;
        }
        console.log('startCheckHtml');
        if (!HtmlChangedChecker.checkAndRestart()) {
            setTimeout(HtmlChangedChecker.startCheckHtml, HtmlChangedChecker.TIMEOUT);
        }
    }
}
