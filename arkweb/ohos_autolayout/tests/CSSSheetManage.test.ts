import CSSSheetManage from '../src/Framework/Common/Style/Setter/CSSSheetManage';
import StyleCommon from '../src/Framework/Common/Style/Common/StyleCommon';
import Constant from '../src/Framework/Common/Constant';

describe('CSSSheetManage', () => {
    beforeEach(() => {
        document.head.innerHTML = '';
        CSSSheetManage.initFlag = false;
        CSSSheetManage.styleNodeList = [];
        StyleCommon.styleDiv = null;

        // Mock window.matchMedia
        Object.defineProperty(window, 'matchMedia', {
            writable: true,
            configurable: true,
            value: jest.fn().mockImplementation(query => ({
                matches: false,
                media: query,
                onchange: null,
                addListener: jest.fn(),
                removeListener: jest.fn(),
                addEventListener: jest.fn(),
                removeEventListener: jest.fn(),
                dispatchEvent: jest.fn(),
            })),
        });
    });

    afterEach(() => {
        document.head.innerHTML = '';
        CSSSheetManage.initFlag = false;
        CSSSheetManage.styleNodeList = [];
        jest.restoreAllMocks();
    });

    describe('reInit', () => {
        it('应该初始化样式表并设置标志位', () => {
            expect(CSSSheetManage.initFlag).toBe(false);
            
            CSSSheetManage.reInit();
            
            expect(CSSSheetManage.initFlag).toBe(true);
            expect(StyleCommon.styleDiv).toBeDefined();
            expect(StyleCommon.styleDiv).toBeInstanceOf(HTMLStyleElement);
        });

        it('应该在已初始化时不重复初始化', () => {
            CSSSheetManage.reInit();
            const firstStyleDiv = StyleCommon.styleDiv;
            
            CSSSheetManage.reInit();
            const secondStyleDiv = StyleCommon.styleDiv;
            
            expect(firstStyleDiv).toBe(secondStyleDiv);
        });

        it('应该将样式节点添加到 document.head', () => {
            CSSSheetManage.reInit();
            
            const styleNodes = document.head.querySelectorAll('style');
            expect(styleNodes.length).toBeGreaterThan(0);
        });
    });

    describe('creatStyleDiv', () => {
        it('应该创建一个 HTMLStyleElement', () => {
            const styleElement = CSSSheetManage.creatStyleDiv();
            
            expect(styleElement).toBeInstanceOf(HTMLStyleElement);
            expect(styleElement.tagName.toLowerCase()).toBe('style');
        });

        it('应该将创建的样式节点添加到 styleNodeList', () => {
            const initialLength = CSSSheetManage.styleNodeList.length;
            
            CSSSheetManage.creatStyleDiv();
            
            expect(CSSSheetManage.styleNodeList.length).toBe(initialLength + 1);
        });

        it('应该将样式节点附加到 document.head', () => {
            const initialHeadChildren = document.head.children.length;
            
            CSSSheetManage.creatStyleDiv();
            
            expect(document.head.children.length).toBe(initialHeadChildren + 1);
        });

        it('应该调用 updateState 方法', () => {
            const updateStateSpy = jest.spyOn(CSSSheetManage, 'updateState');
            
            CSSSheetManage.creatStyleDiv();
            
            expect(updateStateSpy).toHaveBeenCalled();
        });
    });

    describe('enableMedia', () => {
        it('在没有样式节点时应该直接返回', () => {
            CSSSheetManage.styleNodeList = [];
            
            expect(() => CSSSheetManage.enableMedia()).not.toThrow();
        });

        it('应该在媒体查询匹配时设置媒体属性', () => {
            const mockMatchMedia = jest.fn().mockImplementation(query => ({
                matches: true,
                media: query,
                onchange: null,
                addListener: jest.fn(),
                removeListener: jest.fn(),
                addEventListener: jest.fn(),
                removeEventListener: jest.fn(),
                dispatchEvent: jest.fn(),
            }));
            window.matchMedia = mockMatchMedia;

            const styleNode = document.createElement('style');
            document.head.appendChild(styleNode);
            CSSSheetManage.styleNodeList.push(styleNode);
            
            CSSSheetManage.enableMedia();
            
            expect(styleNode.getAttribute(Constant.media_)).toBe(Constant.WIDE_SCREEN_MEDIA_EN_DEFAULT);
        });

        it('应该在媒体查询不匹配时清空媒体属性', () => {
            const mockMatchMedia = jest.fn().mockImplementation(query => ({
                matches: false,
                media: query,
                onchange: null,
                addListener: jest.fn(),
                removeListener: jest.fn(),
                addEventListener: jest.fn(),
                removeEventListener: jest.fn(),
                dispatchEvent: jest.fn(),
            }));
            window.matchMedia = mockMatchMedia;

            const styleNode = document.createElement('style');
            document.head.appendChild(styleNode);
            CSSSheetManage.styleNodeList.push(styleNode);
            
            CSSSheetManage.enableMedia();
            
            expect(styleNode.getAttribute(Constant.media_)).toBe('');
        });

        it('应该为所有样式节点设置媒体属性', () => {
            const styleNode1 = document.createElement('style');
            const styleNode2 = document.createElement('style');
            document.head.appendChild(styleNode1);
            document.head.appendChild(styleNode2);
            CSSSheetManage.styleNodeList.push(styleNode1, styleNode2);
            
            CSSSheetManage.enableMedia();
            
            expect(styleNode1.hasAttribute(Constant.media_)).toBe(true);
            expect(styleNode2.hasAttribute(Constant.media_)).toBe(true);
        });
    });

    describe('disableMedia', () => {
        it('在没有样式节点时应该直接返回', () => {
            CSSSheetManage.styleNodeList = [];
            
            expect(() => CSSSheetManage.disableMedia()).not.toThrow();
        });

        it('应该在强制模式下禁用所有媒体查询', () => {
            const styleNode = document.createElement('style');
            styleNode.setAttribute(Constant.media_, 'screen');
            document.head.appendChild(styleNode);
            CSSSheetManage.styleNodeList.push(styleNode);
            
            CSSSheetManage.disableMedia(true);
            
            expect(styleNode.getAttribute(Constant.media_)).toBe(Constant.WIDE_SCREEN_MEDIA_DIS);
        });

        it('应该在有空媒体属性时禁用', () => {
            const styleNode = document.createElement('style');
            styleNode.setAttribute(Constant.media_, '');
            document.head.appendChild(styleNode);
            CSSSheetManage.styleNodeList.push(styleNode);
            
            CSSSheetManage.disableMedia(false);
            
            expect(styleNode.getAttribute(Constant.media_)).toBe(Constant.WIDE_SCREEN_MEDIA_DIS);
        });

        it('应该在媒体查询活跃时禁用', () => {
            const mockMatchMedia = jest.fn().mockImplementation(query => ({
                matches: true,
                media: query,
                onchange: null,
                addListener: jest.fn(),
                removeListener: jest.fn(),
                addEventListener: jest.fn(),
                removeEventListener: jest.fn(),
                dispatchEvent: jest.fn(),
            }));
            window.matchMedia = mockMatchMedia;

            const styleNode = document.createElement('style');
            document.head.appendChild(styleNode);
            CSSSheetManage.styleNodeList.push(styleNode);
            
            CSSSheetManage.disableMedia(false);
            
            expect(styleNode.getAttribute(Constant.media_)).toBe(Constant.WIDE_SCREEN_MEDIA_DIS);
        });

        it('应该处理多个样式节点', () => {
            const styleNode1 = document.createElement('style');
            const styleNode2 = document.createElement('style');
            styleNode1.setAttribute(Constant.media_, '');
            styleNode2.setAttribute(Constant.media_, '');
            document.head.appendChild(styleNode1);
            document.head.appendChild(styleNode2);
            CSSSheetManage.styleNodeList.push(styleNode1, styleNode2);
            
            CSSSheetManage.disableMedia(false);
            
            expect(styleNode1.getAttribute(Constant.media_)).toBe(Constant.WIDE_SCREEN_MEDIA_DIS);
            expect(styleNode2.getAttribute(Constant.media_)).toBe(Constant.WIDE_SCREEN_MEDIA_DIS);
        });
    });

    describe('updateState', () => {
        it('应该在宽屏模式下启用媒体查询', () => {
            const isWideScreenSpy = jest.spyOn(require('../src/Framework/Common/Utils/Utils').default, 'isWideScreen')
                .mockReturnValue(true);
            const enableMediaSpy = jest.spyOn(CSSSheetManage, 'enableMedia');
            
            CSSSheetManage.updateState();
            
            expect(enableMediaSpy).toHaveBeenCalled();
            
            isWideScreenSpy.mockRestore();
            enableMediaSpy.mockRestore();
        });

        it('应该在非宽屏模式下禁用媒体查询', () => {
            const isWideScreenSpy = jest.spyOn(require('../src/Framework/Common/Utils/Utils').default, 'isWideScreen')
                .mockReturnValue(false);
            const disableMediaSpy = jest.spyOn(CSSSheetManage, 'disableMedia');
            
            CSSSheetManage.updateState();
            
            expect(disableMediaSpy).toHaveBeenCalled();
            
            isWideScreenSpy.mockRestore();
            disableMediaSpy.mockRestore();
        });
    });

    describe('集成测试', () => {
        it('应该完整初始化并管理样式节点', () => {
            CSSSheetManage.reInit();
            
            expect(CSSSheetManage.initFlag).toBe(true);
            expect(CSSSheetManage.styleNodeList.length).toBeGreaterThan(0);
            expect(StyleCommon.styleDiv).toBeDefined();
            expect(document.head.contains(StyleCommon.styleDiv)).toBe(true);
        });

        it('应该正确切换媒体查询状态', () => {
            const styleNode = document.createElement('style');
            document.head.appendChild(styleNode);
            CSSSheetManage.styleNodeList.push(styleNode);
            
            // 启用
            CSSSheetManage.enableMedia();
            const enabledValue = styleNode.getAttribute(Constant.media_);
            
            // 禁用
            CSSSheetManage.disableMedia(true);
            const disabledValue = styleNode.getAttribute(Constant.media_);
            
            expect(enabledValue).not.toBe(disabledValue);
            expect(disabledValue).toBe(Constant.WIDE_SCREEN_MEDIA_DIS);
        });
    });

    describe('边界情况测试', () => {
        it('应该处理空的 styleNodeList', () => {
            CSSSheetManage.styleNodeList = [];
            
            expect(() => CSSSheetManage.enableMedia()).not.toThrow();
            expect(() => CSSSheetManage.disableMedia()).not.toThrow();
        });

        it('应该处理已从 DOM 移除的样式节点', () => {
            const styleNode = document.createElement('style');
            CSSSheetManage.styleNodeList.push(styleNode);
            // 不添加到 DOM
            
            expect(() => CSSSheetManage.enableMedia()).not.toThrow();
            expect(() => CSSSheetManage.disableMedia()).not.toThrow();
        });
    });
});
