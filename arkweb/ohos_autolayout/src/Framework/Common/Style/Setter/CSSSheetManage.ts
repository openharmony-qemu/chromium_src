import Utils from '../../Utils/Utils';
import StyleCommon from '../Common/StyleCommon';
import Log from '../../../../Debug/Log';
import Tag from '../../../../Debug/Tag';
import Constant from '../../Constant';

export default class CSSSheetManage {
    static initFlag = false;

    // 生成CSS样式表
    static reInit(): void {
        if (CSSSheetManage.initFlag) {
            return;
        }
        StyleCommon.styleDiv = this.creatStyleDiv();

        CSSSheetManage.initFlag = true;
    }

    // 创建的style节点缓存列表
    static styleNodeList: HTMLElement[] = [];
    static creatStyleDiv(): HTMLStyleElement {
        const style = document.createElement(Constant.style) as HTMLStyleElement;
        // 对于webkit类型浏览器需要添加style.appendChild(document.createTextNode(''))代码（如safari）
        // 放大模式且clientHeight高度大于500px的页面，需onResize监控来设置CSSSheet是否生效
        const styleNode = document.head.appendChild(<Node>style) as HTMLStyleElement;
        CSSSheetManage.styleNodeList.push(styleNode);

        CSSSheetManage.updateState();
        return styleNode;
    }

    static enableMedia(): void {
        if (!this.styleNodeList.length) {
            return;
        }

        let setMedia: string;

        // 放大模式且页面高度大于500px的网页，CSSSheet强制生效
        const mediaIsActive = window.matchMedia(Constant.WIDE_SCREEN_MEDIA_EN_DEFAULT).matches;
        if (mediaIsActive) {
            setMedia = Constant.WIDE_SCREEN_MEDIA_EN_DEFAULT;
        } else {
            setMedia = '';
        }

        Log.d(`设置 media 属性: ${setMedia}`, Tag.cssSheetManage);
        for (let i = 0; i < this.styleNodeList.length; i++) {
            this.styleNodeList[i].setAttribute(Constant.media_, setMedia);
        }
    }

    static disableMedia(force: boolean = false): void {
        if (!this.styleNodeList.length) {
            return;
        }

        const hasEmptyMedia = CSSSheetManage.styleNodeList.some((node) => node.getAttribute(Constant.media_) === '');
        const mediaIsActive = window.matchMedia(Constant.WIDE_SCREEN_MEDIA_EN_DEFAULT).matches;
        if (force || hasEmptyMedia || mediaIsActive) {
            for (let i = 0; i < this.styleNodeList.length; i++) {
                this.styleNodeList[i].setAttribute(Constant.media_, Constant.WIDE_SCREEN_MEDIA_DIS);
            }
        }
    }

    static updateState(): void {
        // 放大模式且clientHeight高度大于500px的页面，需onResize监控来设置CSSSheet是否生效
        if (Utils.isWideScreen()) {
            CSSSheetManage.enableMedia();
        } else {
            CSSSheetManage.disableMedia();
        }
    }
}
