import Tag from '../../Debug/Tag';
import { AdjacentImageInfo } from '../List/List/AdjacentImageAction';


export interface LargeImgInfo {
    imgWrapper: HTMLElement;
    backIndex: number;
    isTopImg: boolean;
    wrapperWidth: number;
    adjacentImageInfo?: AdjacentImageInfo;
}

export default class LargeImgAction {
    private static TAG = Tag.largeImg;

    static largeImgsList: HTMLElement[] = [];

    static handledImgMap = new Map<HTMLElement, LargeImgInfo>();
    static backImgIncreaseId = 0;
}