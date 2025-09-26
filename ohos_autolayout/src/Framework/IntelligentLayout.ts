import { AComponent } from "./Common/base/AComponent";
import Logger from "./Common/Logger";
import Utils from "./Common/Utils";
import { PerfExecution, Level } from "./Common/Perf";
import { PopWindow } from "./PopWindow/PopWindow";
import { PopupWindow } from "./Popup/PopupWindow";
import { PopupInfo } from "./Popup/PopupInfo";

export default class IntelligentLayout {
    static TAG = "IntelligentLayout";
    static ComponentMap = new Map<HTMLElement, AComponent>();

    // 布局参数硬编码的节点缓存
    static HardCodeElementsCache = new Map<HTMLElement,Boolean>();
    // 不需要检查布局参数硬编码的节点缓存
    static DiscardElementsCache = new Set<HTMLElement>();
    static imageCache = new Map<HTMLElement,Boolean>();
    static mEnabled = false; // 默认不生效
    static isAIEnable = false;   // 是否走AI识别的开关
    static screenWidth = window.innerWidth;
    static screenHeight = window.innerHeight;
    static rootNode: HTMLElement = null;

    static popWindowMap = new Map<PopupInfo, AComponent>();
    // 0: init  1:relayout  2:restorelayout
    static operation_state:number = 0;

    private static hasProcessed(element: HTMLElement, candidates: Map<HTMLElement, boolean>): boolean {
        if(!element || !candidates) {
            return false;
        }
        for (const [container,outOfFlow] of candidates) {
            if(container.contains(element)) {
                return true;
            }
        }
        return false;
    }

    static discardElements(comp:HTMLElement) {
        if(!comp || Utils.shouldSkip(comp)) return;
        if( IntelligentLayout.HardCodeElementsCache.has(comp)) {
            IntelligentLayout.HardCodeElementsCache.delete(comp);
        }
        if( IntelligentLayout.DiscardElementsCache.has(comp)) {
            IntelligentLayout.DiscardElementsCache.delete(comp);
        }
    }

    /**
     * 响应开启重布局的按钮事件，局部刷新节点
     */
    public static relayoutForPopWin(): void {
        Logger.printComInfo(`relayoutForPopWin run`);
        let popupInfo: PopupInfo = null;
        if (this.popWindowMap.size > 0) {
            popupInfo = this.popWindowMap.keys().next().value;
        } else {
            popupInfo = PopupWindow.findPopups(document.body);
        }

        if (popupInfo != null) {
            this.calculateForPopWin(popupInfo);
        }
    }
    
    @PerfExecution({ level: Level.INFO })
    public static intelligentLayout(root: HTMLElement): void {
        Logger.printComInfo(`intelligentLayout run`);

        this.rootNode = root;
        let popupInfo: PopupInfo = null;
        if (this.popWindowMap.size > 0) {
            popupInfo = this.popWindowMap.keys().next().value;
            // @ts-ignore
            window.popupInfo = popupInfo;
        } else {
            popupInfo = PopupWindow.findPopups(root);
            // @ts-ignore
            window.popupInfo = popupInfo;
        }
        // @ts-ignore
        console.log('intelligentLayout: print popupInfo root_node className = ' + window.popupInfo?.root_node?.className);

        if (popupInfo != null) {
            this.calculateForPopWin(popupInfo);
        }
    }


    public static removePopwinCache(node: HTMLElement) {
        this.popWindowMap.forEach((popupWindow, popupInfo)=>{
            if (node.contains(popupInfo.root_node)) {
                this.popWindowMap.delete(popupInfo);
            }
        });
    }

    private static calculateForPopWin(popupInfo: PopupInfo) {
        Logger.printDebugMsg(`calculate for popWindow ${popupInfo?.root_node?.className} `);
        const component:AComponent = this.popWindowMap.has(popupInfo) ? this.popWindowMap.get(popupInfo) : new PopWindow(popupInfo);
        if (component && component.isDirty()) {
            component.intelligenceLayout();
            // 清除标记
            component.setDirty(false);
            // this.operation_state = 1; // 已经重布局过，避免多次
        } else {
            // fallback,兜底方案
        }

        if(component && !IntelligentLayout.popWindowMap.has(popupInfo)) {
            IntelligentLayout.popWindowMap.set(popupInfo, component);
        }
    }

    // 新增节点是某个组件下的节点，标记这个组件为脏，下一次布局的时候，只需要布局这个组件就可以了
    // todo FIXME: 节点需要精细化处理
    static markDirty(item: MutationRecord) {
        if(!item) return;
        for (let i = 0; i < item.addedNodes.length; i++) {
            if(Utils.shouldSkip(item.addedNodes[i] as HTMLElement)) {
                continue;
            }
            // workaround,只要发生节点变化，就重新刷新界面
            IntelligentLayout.ComponentMap.forEach(comp => comp.setDirty(true));
            break;
        }
        for (let i = 0; i < item.removedNodes.length; i++) {
            if(Utils.shouldSkip(item.addedNodes[i] as HTMLElement)) {
                continue;
            }
            IntelligentLayout.discardElements(item.addedNodes[i]  as HTMLElement);
        }
    }

    @PerfExecution({ level: Level.INFO })
    static reInit() {
        IntelligentLayout.HardCodeElementsCache.clear();
        for (const [ele, comp] of IntelligentLayout.ComponentMap) {
            if (!comp) continue;
            comp.setDirty(true);
        }
    }

    static injectSwitchButton() {
        Logger.printDebugMsg(`injectSwitchButton`);

        const button = document.createElement('button');
        button.id = 'SmartSwitch';
        Object.assign(button.style, {
            position: 'fixed',
            innerWidth: '40px',
            innerHeight: '20px',
            right: '20px',
            bottom: screen.availHeight < 500 ? '150px' : '240px',
            zIndex: '9999999999',
            backgroundColor: '#007dff',
            color: 'white',
            border: 'none',
            borderRadius: '10px',
            cursor: 'pointer',
            fontSize: '15px'
        });

        let isIntelligentLayoutEnabled = sessionStorage.getItem('intelligentLayoutEnable');
        if (isIntelligentLayoutEnabled == "true") {
            button.textContent = 'Off';
        } else {
            button.textContent = 'On';
        }

        // 添加点击事件监听
        button.addEventListener('click', () => {
            if (button.textContent == 'On') {
                button.textContent = 'Off'
                this.mEnabled = true;
                this.relayoutForPopWin();
            } else {
                button.textContent = 'On'
                this.mEnabled = false;
                location.reload();
            }
            sessionStorage.setItem('intelligentLayoutEnable', JSON.stringify(this.mEnabled));
        });

        document.body.appendChild(button);
      }

    static injectBorderButton() {
        Logger.printDebugMsg(`injectBorderButton`);

        const button = document.createElement('button');
        button.id = 'BorderSwitch';
        Object.assign(button.style, {
            position: 'fixed',
            innerWidth: '40px',
            innerHeight: '20px',
            right: '20px',
            bottom: '200px',
            zIndex: '9999',
            backgroundColor: '#007dff',
            color: 'white',
            border: 'none',
            borderRadius: '10px',
            cursor: 'pointer',
            fontSize: '15px'
        });

        let isBorderShow = sessionStorage.getItem('ShowBorder');
        if (isBorderShow == "true" || isBorderShow == null) {
            button.textContent = 'Hid';
        } else {
            button.textContent = 'Show';
        }

        // 添加点击事件监听
        button.addEventListener('click', () => {
            if (button.textContent == 'Show') {
                button.textContent = 'Hid'
                sessionStorage.setItem('ShowBorder', JSON.stringify(true));
            } else {
                button.textContent = 'Show'
                sessionStorage.setItem('ShowBorder', JSON.stringify(false));
            }
            location.reload();
        });

        document.body.appendChild(button);
    }

    static injectSaveButton() {
        Logger.printDebugMsg(`InjectSaveBorderButton`);

        const button = document.createElement('button');
        button.id = 'SaveSwitch';
        button.textContent = 'Save';
        Object.assign(button.style, {
            position: 'fixed',
            innerWidth: '40px',
            innerHeight: '20px',
            right: '20px',
            bottom: screen.availHeight < 500 ? '100px' : '200px',
            zIndex: '9999999999',
            backgroundColor: '#007dff',
            color: 'white',
            border: 'none',
            borderRadius: '10px',
            cursor: 'pointer',
            fontSize: '15px'
        });


        // 添加点击事件监听
        button.addEventListener('click', () => {
            this.saveHtml();
        });

        document.body.appendChild(button);
    }

    /**
     * 抓取页面内容和样式
     */
    static saveHtml(): void {
        console.log("开始处理页面，请稍候...");

        // =================================================================
        // 步骤 1: 提取页面所有元素的计算样式
        // =================================================================
        const allElements: NodeListOf<Element> = document.body.querySelectorAll('*');
        const cssRules: string[] = [];
        let elementIndex: number = 0;

        allElements.forEach((element: Element) => {
            const htmlElement = element as HTMLElement;
            if (htmlElement.tagName === 'SCRIPT' || htmlElement.tagName === 'STYLE') {
                return;
            }
            const uniqueId: string = `styled-element-${elementIndex++}`;
            htmlElement.setAttribute('data-style-id', uniqueId);
            const computedStyle: CSSStyleDeclaration = window.getComputedStyle(htmlElement);
            let styleString: string = '';
            for (const prop of computedStyle) {
                styleString += `${prop}: ${computedStyle.getPropertyValue(prop)};\n`;
            }
            if (styleString) {
                cssRules.push(`[data-style-id="${uniqueId}"] {\n${styleString}}`);
            }
        });

        const cssContent: string = cssRules.join('\n\n');
        console.log("CSS样式提取完成。");

        // =================================================================
        // 步骤 2: 生成包含样式链接的HTML内容
        // =================================================================
        const clonedDocument: Document = document.cloneNode(true) as Document;
        const selectorToRemove = '#SmartSwitch, #SaveSwitch';
        console.log(`正在从导出的HTML中移除匹配选择器 "${selectorToRemove}" 的元素...`);
        
        const elementsToRemove: NodeListOf<Element> = clonedDocument.querySelectorAll(selectorToRemove);
        
        if (elementsToRemove.length > 0) {
            elementsToRemove.forEach(element => {
                element.remove();
            });
            console.log(`已成功移除 ${elementsToRemove.length} 个匹配的元素。`);
        } else {
            console.log(`未在页面中找到匹配 "${selectorToRemove}" 的元素。`);
        }
        const head: HTMLHeadElement | null = clonedDocument.querySelector('head');
        if (head) {
            const linkTag: HTMLLinkElement = document.createElement('link');
            linkTag.rel = 'stylesheet';
            // 注意：此处先使用一个临时文件名，后续会替换为动态生成的文件名
            linkTag.href = 'placeholder-for-styles.css'; 
            head.appendChild(linkTag);
        }
        const rawHtmlContent: string = clonedDocument.documentElement?.outerHTML || '';
        console.log("HTML内容准备完成。");

        // =================================================================
        // 步骤 3: 生成文件名并保存文件
        // =================================================================
        let appTitle: string = '';
        let appPage: string = '';

        try {
            // @ts-ignore
            if (typeof $AppxMeta !== 'undefined' && $AppxMeta.window.defaultTitle) {
                // @ts-ignore
                appTitle = $AppxMeta.window.defaultTitle;
            }
            if (appTitle == '' || appTitle == undefined || appTitle == 'undefined') {
                // @ts-ignore
                appTitle = AlipayJSBridge.startupParams.defaultTitle;
                if (appTitle == '' || appTitle == undefined || appTitle == 'undefined') {
                    appTitle = '' + Date.now();
                }
            }
        } catch (e) {
            console.error("获取 appTitle 失败", e);
        }
        appTitle = this.replayceIllegalChars(appTitle, '_');

        try {
            // @ts-ignore
            appPage = AlipayJSBridge.startupParams.page?.replaceAll('/', '-');
            if (appPage == undefined || appPage == '') {
                appPage = '' + Date.now();
            }
        } catch (e) {
            console.error("获取 appPage 失败", e);
        }
        appPage = this.replayceIllegalChars(appPage, '_');
        let timeStamp = '' + Date.now();
        
        const baseFilename = `${appTitle}_${appPage}_${timeStamp}_${screen.availWidth}x${screen.availHeight}`;
        const htmlFilename = `${baseFilename}.html`;
        const cssFilename = `${baseFilename}.css`;

        // 将HTML中的临时CSS链接替换为最终的正确文件名
        const finalHtmlContent = rawHtmlContent.replace('href="placeholder-for-styles.css"', `href="${cssFilename}"`);

        try {
            console.log(`正在保存 CSS 文件: ${cssFilename}`);
            // @ts-ignore
            ndkPopUp.saveHTML(cssFilename, cssContent);

            console.log(`正在保存 HTML 文件: ${htmlFilename}`);
            // @ts-ignore
            ndkPopUp.saveHTML(htmlFilename, finalHtmlContent);

            this.showToast(`成功保存文件:\n${htmlFilename}\n${cssFilename}`);
        } catch (error) {
            console.error("文件保存失败:", error);
        }

        // =================================================================
        // 步骤 4: 清理工作
        // =================================================================
        allElements.forEach((element: Element) => {
            element.removeAttribute('data-style-id');
        });

        console.log("处理完成！");
    }

    static replayceIllegalChars(input: string, replacement = '_'): string {
        // 1.替换文件系统非法字符
        // Windows: \ / : * ? " < > |
        // HarmonyOS: / \0 : * ? " < > | \
        const illegalCharsRegex = /[\\/:*?"<>|\0]/g;
        let sanitized = input.trim().replace(illegalCharsRegex, replacement);

        // 2. 处理开头/结尾的点和空格
        sanitized = sanitized.replace(/^[. ]+/, '').replace(/[. ]+$/, '');

        // 3. 处理长度限制
        if (sanitized.length > 100) {
            sanitized = sanitized.slice(0, 100);
        }

        return sanitized;
    }

    static showToast(message: string, duration = 3000) {
        const toast = document.createElement("div");
        toast.textContent = message;
        toast.style.position = "fixed";
        toast.style.bottom = "60px";
        toast.style.left = "50%";
        toast.style.fontSize = '10px';
        toast.style.transform = "translateX(-50%)";
        toast.style.backgroundColor = "#333";
        toast.style.color = "#fff";
        toast.style.padding = "10px 20px";
        toast.style.borderRadius = "5px";
        toast.style.zIndex = "99999999999";
        toast.style.animation = "fadein 0.5s, fadeout 0.5s " + (duration / 1000 - 0.5) + "s";
      
        document.body.appendChild(toast);
      
        setTimeout(() => {
          toast.remove();
        }, duration);
    }
}
