import { PopupType } from './PopupType';
import { PopupLayoutState } from './PopupLayoutState';

/**
 * 弹窗信息接口
 */
export interface PopupInfo {
    root_node: HTMLElement;
    mask_node: HTMLElement;
    content_node: HTMLElement;
    popup_type: PopupType;
    root_position: string;
    root_zindex: number;
    has_mask: boolean;
    root_screen_area_ratio: number;
    root_is_visiable: boolean;
    has_close_button: boolean;
    mask_area_ratio: number;
    mask_position: string;
    mask_zindex: number;
    stickyTop_height:number;
    stickyBottom_height:number;
}

export interface PotentialElements {
    potentialMasks: Array<Element>;
    potentialStickyTop:Element;
    potentialStickyBottom:Element;
}
