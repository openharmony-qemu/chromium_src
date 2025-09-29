import { LayoutValue } from '../../../Common/Constant';
import Store from '../../../Common/Utils/Store';
 
export default class SwiperAction {
    private static childNumberChangeList: Set<HTMLElement>;
 
    static collectEle(parent: Node): void {
        if (parent instanceof HTMLElement && Store.getValue(parent, LayoutValue.SWIPER)) {
            if (!SwiperAction.childNumberChangeList) {
                SwiperAction.childNumberChangeList = new Set();
            }
            SwiperAction.childNumberChangeList.add(parent);
        }
    }
}