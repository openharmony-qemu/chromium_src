import { Txt } from '../../../Common/Txt';
 
export default class PageObserver {
    static hadScroll: boolean = false;
 
    static reInit():void {
        PageObserver.checkFirstScroll();
    }
 
    static checkFirstScroll(): void {
        if (PageObserver.hadScroll) {
            return;
        }
 
        const currentTop = document.documentElement.scrollTop || document.body.scrollTop;
 
        if (currentTop > 0) {
            PageObserver.hadScroll = true;
            return;
        }
 
        window.addEventListener(Txt.scroll_, PageObserver.scrollHandle);
    }
 
    static scrollHandle(): void {
        PageObserver.hadScroll = true;
 
        window.removeEventListener(Txt.scroll_, PageObserver.scrollHandle);
    }
}