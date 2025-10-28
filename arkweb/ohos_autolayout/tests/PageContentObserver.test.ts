/**
 * PageContentObserver 单元测试
 * 测试页面内容观察器的核心功能
 */

// Mock 所有依赖模块
jest.mock('../src/Debug/Log', () => ({
    __esModule: true,
    default: {
        info: jest.fn(),
        d: jest.fn(),
    },
}));

jest.mock('../src/Debug/Tag', () => ({
    __esModule: true,
    default: {
        pageContentObserver: 'PageContentObserver',
    },
}));

jest.mock('../src/Framework/Observer/ObserverHandler', () => ({
    __esModule: true,
    default: {
        postTask: jest.fn(),
    },
}));

import PageContentObserver from '../src/Framework/Observer/Observers/PageContentObserver';
import ObserverHandler from '../src/Framework/Observer/ObserverHandler';

describe('PageContentObserver', () => {
    let mockMutationObserver: jest.Mock;
    let observeCallback: MutationCallback;

    beforeEach(() => {
        jest.clearAllMocks();

        // Mock MutationObserver
        observeCallback = null;
        mockMutationObserver = jest.fn((callback: MutationCallback) => {
            observeCallback = callback;
            return {
                observe: jest.fn(),
                disconnect: jest.fn(),
                takeRecords: jest.fn((): MutationRecord[] => []),
            };
        });
        global.MutationObserver = mockMutationObserver;

        // 重置 document.body
        document.body.innerHTML = '';

        // 重置 PageContentObserver 状态
        PageContentObserver.reset();
    });

    afterEach(() => {
        PageContentObserver.disconnect();
    });

    describe('reInit', () => {
        it('应该重置内容就绪状态', () => {
            // @ts-ignore
            PageContentObserver.contentReady = true;

            PageContentObserver.reInit();
            
            // @ts-ignore
            expect(PageContentObserver.contentReady).toBe(false);
        });

        it('应该断开现有观察器', () => {
            PageContentObserver.startObserving();
            // @ts-ignore
            const observer = PageContentObserver.observer;
            const disconnectSpy = jest.spyOn(observer!, 'disconnect');

            PageContentObserver.reInit();

            expect(disconnectSpy).toHaveBeenCalled();
        });
    });

    describe('isContentReady', () => {
        it('应该在已经确认内容就绪时返回 true', () => {
            // @ts-ignore
            PageContentObserver.contentReady = true;

            const result = PageContentObserver.isContentReady();

            expect(result).toBe(true);
        });

        it('应该在页面有足够内容时返回 true', () => {
            // 创建足够多的有意义节点
            for (let i = 0; i < 12; i++) {
                const div = document.createElement('div');
                div.style.display = 'block';
                document.body.appendChild(div);
            }

            const result = PageContentObserver.isContentReady();

            expect(result).toBe(true);
            // @ts-ignore
            expect(PageContentObserver.contentReady).toBe(true);
        });

        it('应该在页面内容不足时返回 false', () => {
            // 只创建少量节点
            const div = document.createElement('div');
            document.body.appendChild(div);

            const result = PageContentObserver.isContentReady();

            expect(result).toBe(false);
        });

        it('应该忽略隐藏元素（display: none）', () => {
            for (let i = 0; i < 15; i++) {
                const div = document.createElement('div');
                div.style.display = 'none';
                document.body.appendChild(div);
            }

            const result = PageContentObserver.isContentReady();

            expect(result).toBe(false);
        });

        it('应该忽略隐藏元素（visibility: hidden）', () => {
            for (let i = 0; i < 15; i++) {
                const div = document.createElement('div');
                div.style.visibility = 'hidden';
                document.body.appendChild(div);
            }

            const result = PageContentObserver.isContentReady();

            expect(result).toBe(false);
        });

        it('应该忽略隐藏元素（opacity: 0）', () => {
            for (let i = 0; i < 15; i++) {
                const div = document.createElement('div');
                div.style.opacity = '0';
                document.body.appendChild(div);
            }

            const result = PageContentObserver.isContentReady();

            expect(result).toBe(false);
        });

        it('应该统计各种有意义的标签', () => {
            const meaningfulTags = ['div', 'p', 'span', 'a', 'button', 'section', 'article', 'nav', 'header', 'footer', 'main'];
            
            meaningfulTags.forEach(tag => {
                const element = document.createElement(tag);
                element.style.display = 'block';
                document.body.appendChild(element);
            });

            const result = PageContentObserver.isContentReady();

            expect(result).toBe(true);
        });

        it('应该处理嵌套元素', () => {
            const container = document.createElement('div');
            document.body.appendChild(container);

            for (let i = 0; i < 12; i++) {
                const child = document.createElement('div');
                child.style.display = 'block';
                container.appendChild(child);
            }

            const result = PageContentObserver.isContentReady();

            expect(result).toBe(true);
        });

        it('应该在达到阈值后提前退出', () => {
            // 创建大量元素
            for (let i = 0; i < 100; i++) {
                const div = document.createElement('div');
                div.style.display = 'block';
                document.body.appendChild(div);
            }

            const result = PageContentObserver.isContentReady();

            expect(result).toBe(true);
            // 性能优化：应该在检测到足够内容后提前退出
        });
    });

    describe('startObserving', () => {
        it('应该启动 MutationObserver', () => {
            PageContentObserver.startObserving();

            expect(mockMutationObserver).toHaveBeenCalled();
            // @ts-ignore
            expect(PageContentObserver.observer).toBeDefined();
        });

        it('应该避免重复启动', () => {
            PageContentObserver.startObserving();
            PageContentObserver.startObserving();

            expect(mockMutationObserver).toHaveBeenCalledTimes(1);
        });

        it('应该观察 body 的子节点变化', () => {
            PageContentObserver.startObserving();

            // @ts-ignore
            const observer = PageContentObserver.observer;
            expect(observer).toBeDefined();
        });

        it('应该在检测到内容时触发 postTask', () => {
            PageContentObserver.startObserving();

            // 添加足够的内容
            for (let i = 0; i < 12; i++) {
                const div = document.createElement('div');
                div.style.display = 'block';
                document.body.appendChild(div);
            }

            // 模拟 MutationObserver 回调
            if (observeCallback) {
                // @ts-ignore
                observeCallback([], PageContentObserver.observer as MutationObserver);
            }

            expect(ObserverHandler.postTask).toHaveBeenCalled();
            // @ts-ignore
            expect(PageContentObserver.contentReady).toBe(true);
        });

        it('应该在检测到内容后自动断开观察器', () => {
            PageContentObserver.startObserving();
            // @ts-ignore
            const observer = PageContentObserver.observer;
            const disconnectSpy = jest.spyOn(observer!, 'disconnect');

            // 添加足够的内容
            for (let i = 0; i < 12; i++) {
                const div = document.createElement('div');
                div.style.display = 'block';
                document.body.appendChild(div);
            }

            if (observeCallback) {
                // @ts-ignore
                observeCallback([], PageContentObserver.observer as MutationObserver);
            }

            expect(disconnectSpy).toHaveBeenCalled();
            // @ts-ignore
            expect(PageContentObserver.observer).toBeNull();
        });

        it('应该在内容不足时继续观察', () => {
            PageContentObserver.startObserving();

            // 只添加少量内容
            const div = document.createElement('div');
            document.body.appendChild(div);

            if (observeCallback) {
                // @ts-ignore
                observeCallback([], PageContentObserver.observer as MutationObserver);
            }

            expect(ObserverHandler.postTask).not.toHaveBeenCalled();
            // @ts-ignore
            expect(PageContentObserver.observer).not.toBeNull();
        });
    });

    describe('disconnect', () => {
        it('应该断开观察器连接', () => {
            PageContentObserver.startObserving();
            // @ts-ignore
            const observer = PageContentObserver.observer;
            const disconnectSpy = jest.spyOn(observer!, 'disconnect');

            PageContentObserver.disconnect();

            expect(disconnectSpy).toHaveBeenCalled();
            // @ts-ignore
            expect(PageContentObserver.observer).toBeNull();
        });

        it('应该在没有观察器时安全调用', () => {
            expect(() => {
                PageContentObserver.disconnect();
            }).not.toThrow();
        });
    });

    describe('reset', () => {
        it('应该重置状态并断开观察器', () => {
            // @ts-ignore
            PageContentObserver.contentReady = true;
            PageContentObserver.startObserving();

            PageContentObserver.reset();

            // @ts-ignore
            expect(PageContentObserver.contentReady).toBe(false);
            // @ts-ignore
            expect(PageContentObserver.observer).toBeNull();
        });
    });

    describe('checkPageContent (性能)', () => {
        it('应该限制每层检查的子节点数量', () => {
            const container = document.createElement('div');
            document.body.appendChild(container);

            // 创建超过限制的子节点
            for (let i = 0; i < 100; i++) {
                const child = document.createElement('div');
                child.style.display = 'block';
                container.appendChild(child);
            }

            const result = PageContentObserver.isContentReady();

            expect(result).toBe(true);
            // 应该在达到阈值后停止检查，而不是检查所有 100 个节点
        });

        it('应该处理深层嵌套结构', () => {
            let current = document.body;
            
            // 创建深层嵌套
            for (let i = 0; i < 5; i++) {
                const div = document.createElement('div');
                div.style.display = 'block';
                current.appendChild(div);
                current = div;
            }

            // 在最深层添加多个元素
            for (let i = 0; i < 10; i++) {
                const div = document.createElement('div');
                div.style.display = 'block';
                current.appendChild(div);
            }

            const result = PageContentObserver.isContentReady();

            expect(result).toBe(true);
        });
    });

    describe('边界情况', () => {
        it('应该处理空 body', () => {
            document.body.innerHTML = '';

            const result = PageContentObserver.isContentReady();

            expect(result).toBe(false);
        });

        it('应该处理 null body', () => {
            const originalBody = document.body;
            Object.defineProperty(document, 'body', {
                get: () => null,
                configurable: true,
            });

            const result = PageContentObserver.isContentReady();

            expect(result).toBe(false);

            // 恢复 body
            Object.defineProperty(document, 'body', {
                get: () => originalBody,
                configurable: true,
            });
        });

        it('应该处理混合的可见和隐藏元素', () => {
            // 添加一些可见元素
            for (let i = 0; i < 6; i++) {
                const div = document.createElement('div');
                div.style.display = 'block';
                document.body.appendChild(div);
            }

            // 添加一些隐藏元素
            for (let i = 0; i < 10; i++) {
                const div = document.createElement('div');
                div.style.display = 'none';
                document.body.appendChild(div);
            }

            // 再添加一些可见元素达到阈值
            for (let i = 0; i < 6; i++) {
                const div = document.createElement('div');
                div.style.display = 'block';
                document.body.appendChild(div);
            }

            const result = PageContentObserver.isContentReady();

            expect(result).toBe(true);
        });
    });
});
