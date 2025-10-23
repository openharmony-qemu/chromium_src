/**
 * ResizeObserver 单元测试
 * 测试窗口大小变化观察器的核心功能
 */

// Mock 所有依赖模块
jest.mock('../src/Framework/Observer/ObserverHandler', () => ({
    __esModule: true,
    default: {
        updateObserver: jest.fn(),
        postTask: jest.fn(),
    },
}));

jest.mock('../src/Framework/Utils/WaitSystemReady', () => ({
    __esModule: true,
    default: {
        hasBodyReady: true,
    },
}));

jest.mock('../src/Framework/Common/Style/Setter/CSSSheetManage', () => ({
    __esModule: true,
    default: {
        updateState: jest.fn(),
    },
}));

jest.mock('../src/Framework/Framework', () => ({
    __esModule: true,
    default: {
        headReadyTask: jest.fn(),
    },
}));

jest.mock('../src/Debug/Log', () => ({
    __esModule: true,
    default: {
        info: jest.fn(),
        d: jest.fn(),
        checkState: jest.fn(),
    },
}));

jest.mock('../src/Debug/Tag', () => ({
    __esModule: true,
    default: {
        resizeObserver: 'ResizeObserver',
    },
}));

jest.mock('../src/Main', () => ({
    Main: {
        initFlag: true,
        restart: jest.fn(),
    },
}));

jest.mock('../src/Framework/Common/Constant', () => ({
    __esModule: true,
    default: {
        resize: 'resize',
    },
}));

import ResizeObserver from '../src/Framework/Observer/Observers/ResizeObserver';
import ObserverHandler from '../src/Framework/Observer/ObserverHandler';
import WaitSystemReady from '../src/Framework/Utils/WaitSystemReady';
import CSSSheetManage from '../src/Framework/Common/Style/Setter/CSSSheetManage';
import Framework from '../src/Framework/Framework';
import { Main } from '../src/Main';

describe('ResizeObserver', () => {
    beforeEach(() => {
        jest.clearAllMocks();
        jest.useFakeTimers();
        
        // 重置初始化标志
        ResizeObserver.init = false;
        
        // 重置 Main.initFlag
        Main.initFlag = true;
        WaitSystemReady.hasBodyReady = true;
    });

    afterEach(() => {
        jest.useRealTimers();
        ResizeObserver.removeListener();
    });

    describe('init_', () => {
        it('应该初始化并添加 resize 事件监听器', () => {
            const addEventListenerSpy = jest.spyOn(window, 'addEventListener');

            ResizeObserver.init_();

            expect(ResizeObserver.init).toBe(true);
            expect(addEventListenerSpy).toHaveBeenCalledWith('resize', expect.any(Function));
        });

        it('应该避免重复初始化', () => {
            const addEventListenerSpy = jest.spyOn(window, 'addEventListener');

            ResizeObserver.init_();
            ResizeObserver.init_();

            // 应该只添加一次监听器
            expect(addEventListenerSpy).toHaveBeenCalledTimes(1);
        });
    });

    describe('resizeCallback', () => {
        it('应该在已初始化时调用 onResize', () => {
            ResizeObserver.init_();
            Main.initFlag = true;

            // 触发 resize 事件
            window.dispatchEvent(new Event('resize'));

            jest.advanceTimersByTime(100);

            expect(Framework.headReadyTask).toHaveBeenCalled();
            expect(ObserverHandler.updateObserver).toHaveBeenCalled();
            expect(CSSSheetManage.updateState).toHaveBeenCalled();
            expect(ObserverHandler.postTask).toHaveBeenCalled();
        });

        it('应该在未初始化时调用 Main.restart', () => {
            ResizeObserver.init_();
            Main.initFlag = false;

            window.dispatchEvent(new Event('resize'));

            expect(Main.restart).toHaveBeenCalled();
            expect(Framework.headReadyTask).not.toHaveBeenCalled();
        });
    });

    describe('onResize', () => {
        it('应该执行完整的 resize 处理流程', () => {
            ResizeObserver.onResize();

            expect(Framework.headReadyTask).toHaveBeenCalled();
            expect(ObserverHandler.updateObserver).toHaveBeenCalled();
            expect(CSSSheetManage.updateState).toHaveBeenCalled();

            jest.advanceTimersByTime(100);
            expect(ObserverHandler.postTask).toHaveBeenCalled();
        });

        it('应该在 body 未就绪时直接返回', () => {
            WaitSystemReady.hasBodyReady = false;

            ResizeObserver.onResize();

            expect(Framework.headReadyTask).not.toHaveBeenCalled();
            expect(ObserverHandler.updateObserver).not.toHaveBeenCalled();
        });

        it('应该延迟 100ms 后调用 postTask', () => {
            ResizeObserver.onResize();

            expect(ObserverHandler.postTask).not.toHaveBeenCalled();

            jest.advanceTimersByTime(50);
            expect(ObserverHandler.postTask).not.toHaveBeenCalled();

            jest.advanceTimersByTime(50);
            expect(ObserverHandler.postTask).toHaveBeenCalled();
        });
    });

    describe('removeListener', () => {
        it('应该移除事件监听器并重置状态', () => {
            const removeEventListenerSpy = jest.spyOn(window, 'removeEventListener');

            ResizeObserver.init_();
            ResizeObserver.removeListener();

            expect(ResizeObserver.init).toBe(false);
            expect(removeEventListenerSpy).toHaveBeenCalledWith('resize', expect.any(Function));
        });

        it('应该在移除监听器后不再响应 resize 事件', () => {
            ResizeObserver.init_();
            ResizeObserver.removeListener();

            jest.clearAllMocks();
            window.dispatchEvent(new Event('resize'));

            expect(Framework.headReadyTask).not.toHaveBeenCalled();
        });
    });

    describe('集成测试', () => {
        it('应该正确处理多次 resize 事件', () => {
            ResizeObserver.init_();

            window.dispatchEvent(new Event('resize'));
            window.dispatchEvent(new Event('resize'));
            window.dispatchEvent(new Event('resize'));

            jest.advanceTimersByTime(100);

            expect(ObserverHandler.postTask).toHaveBeenCalledTimes(3);
        });

        it('应该在 resize 时按顺序执行所有任务', () => {
            const callOrder: string[] = [];

            (Framework.headReadyTask as jest.Mock).mockImplementation(() => {
                callOrder.push('headReadyTask');
            });

            (ObserverHandler.updateObserver as jest.Mock).mockImplementation(() => {
                callOrder.push('updateObserver');
            });

            (CSSSheetManage.updateState as jest.Mock).mockImplementation(() => {
                callOrder.push('updateState');
            });

            ResizeObserver.onResize();

            expect(callOrder).toEqual(['headReadyTask', 'updateObserver', 'updateState']);
        });
    });
});
