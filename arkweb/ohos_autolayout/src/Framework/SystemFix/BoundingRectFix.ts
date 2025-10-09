import Utils from '../../Common/Utils/Utils';
 
export default class BoundingRectFix {
    private static doubles: () => DOMRect;
    private static isActive: boolean = true;
 
    static updateState(): void {
        BoundingRectFix.isActive = Utils.isWideScreen();
    }
 
    static getRectBeforeZoom(ele: HTMLElement): DOMRect {
        if (BoundingRectFix.doubles) {
            return BoundingRectFix.doubles.call(ele);
        }
 
        return ele.getBoundingClientRect();
    }
}