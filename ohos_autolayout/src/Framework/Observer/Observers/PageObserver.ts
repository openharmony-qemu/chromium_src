import { Txt } from "../../../Common/Txt";
 
export default class PageObserver {
    static hadScroll: boolean = false;
 
    static reInit() {
        PageObserver.checkFirstScroll();
    }
 
    static checkFirstScroll() {
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
 
    static scrollHandle() {
        PageObserver.hadScroll = true;
 
        window.removeEventListener(Txt.scroll_, PageObserver.scrollHandle);
    }
}