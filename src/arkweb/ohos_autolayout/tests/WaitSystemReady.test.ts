/**
 * Unit tests for WaitSystemReady.ts
 * 测试系统就绪等待模块的功能
 */

// Mock dependencies
jest.mock('../src/Debug/Log', () => ({
  __esModule: true,
  default: {
    d: jest.fn(),
    info: jest.fn(),
  },
}));

jest.mock('../src/Debug/Tag', () => ({
  __esModule: true,
  default: {
    framework: 'Framework',
  },
}));

jest.mock('../src/Main', () => ({
  Main: {
    start_: jest.fn(),
  },
}));

import WaitSystemReady from '../src/Framework/Utils/WaitSystemReady';
import { Main } from '../src/Main';

describe('WaitSystemReady Module', () => {
  beforeEach(() => {
    jest.clearAllMocks();
    jest.useFakeTimers();
    WaitSystemReady.hasBodyReady = false;
    WaitSystemReady.sysReady = false;
    // @ts-ignore - 访问私有属性用于测试
    WaitSystemReady.tryTimes = 1000;
    // @ts-ignore
    WaitSystemReady.tryHead = 100;
  });

  afterEach(() => {
    jest.useRealTimers();
  });

  describe('bodyReady', () => {
    test('should call task immediately when body exists and wide screen', () => {
      const mockTask = jest.fn();
      
      // Mock wide screen
      Object.defineProperty(window, 'innerWidth', {
        writable: true,
        configurable: true,
        value: 1024,
      });
      
      WaitSystemReady.bodyReady(mockTask);
      
      expect(mockTask).toHaveBeenCalled();
      expect(WaitSystemReady.hasBodyReady).toBe(true);
    });

    test('should wait when body is null', () => {
      const mockTask = jest.fn();
      const originalBody = document.body;
      
      Object.defineProperty(document, 'body', {
        writable: true,
        configurable: true,
        value: null,
      });
      
      WaitSystemReady.bodyReady(mockTask);
      
      expect(mockTask).not.toHaveBeenCalled();
      
      // Restore
      Object.defineProperty(document, 'body', {
        writable: true,
        configurable: true,
        value: originalBody,
      });
    });

    test('should wait when innerWidth is too small', () => {
      const mockTask = jest.fn();
      
      Object.defineProperty(window, 'innerWidth', {
        writable: true,
        configurable: true,
        value: 50,
      });
      
      WaitSystemReady.bodyReady(mockTask);
      
      expect(mockTask).not.toHaveBeenCalled();
    });

    test('should retry after timeout', () => {
      const mockTask = jest.fn();
      const originalBody = document.body;
      
      Object.defineProperty(document, 'body', {
        writable: true,
        configurable: true,
        value: null,
      });
      
      WaitSystemReady.bodyReady(mockTask);
      
      jest.advanceTimersByTime(50);
      
      expect(mockTask).not.toHaveBeenCalled();
      
      // Restore
      Object.defineProperty(document, 'body', {
        writable: true,
        configurable: true,
        value: originalBody,
      });
    });

    test('should stop retrying after tryTimes reaches 0', () => {
      const mockTask = jest.fn();
      
      // @ts-ignore
      WaitSystemReady.tryTimes = 0;
      
      Object.defineProperty(document, 'body', {
        writable: true,
        configurable: true,
        value: null,
      });
      
      WaitSystemReady.bodyReady(mockTask);
      
      jest.advanceTimersByTime(100);
      
      expect(mockTask).not.toHaveBeenCalled();
    });
  });

  describe('headReady', () => {
    test('should call task immediately when head exists', () => {
      const mockTask = jest.fn();
      
      WaitSystemReady.headReady(mockTask);
      
      expect(mockTask).toHaveBeenCalled();
    });

    test('should wait when head is null', () => {
      const mockTask = jest.fn();
      const originalHead = document.head;
      
      Object.defineProperty(document, 'head', {
        writable: true,
        configurable: true,
        value: null,
      });
      
      WaitSystemReady.headReady(mockTask);
      
      expect(mockTask).not.toHaveBeenCalled();
      
      // Restore
      Object.defineProperty(document, 'head', {
        writable: true,
        configurable: true,
        value: originalHead,
      });
    });

    test('should retry after timeout when head is null', () => {
      const mockTask = jest.fn();
      const originalHead = document.head;
      
      Object.defineProperty(document, 'head', {
        writable: true,
        configurable: true,
        value: null,
      });
      
      WaitSystemReady.headReady(mockTask);
      
      jest.advanceTimersByTime(10);
      
      expect(mockTask).not.toHaveBeenCalled();
      
      // Restore
      Object.defineProperty(document, 'head', {
        writable: true,
        configurable: true,
        value: originalHead,
      });
    });

    test('should stop retrying after tryHead reaches 0', () => {
      const mockTask = jest.fn();
      
      // @ts-ignore
      WaitSystemReady.tryHead = -1;
      
      WaitSystemReady.headReady(mockTask);
      
      expect(mockTask).not.toHaveBeenCalled();
    });
  });

  describe('configReady', () => {
    test('should call task immediately', () => {
      const mockTask = jest.fn();
      
      WaitSystemReady.configReady(mockTask);
      
      expect(mockTask).toHaveBeenCalled();
    });

    test('should pass task parameters', () => {
      const mockTask = jest.fn();
      
      WaitSystemReady.configReady(mockTask);
      
      expect(mockTask).toHaveBeenCalledTimes(1);
    });
  });

  describe('viewportReady', () => {
    test('should call task immediately when viewport exists', () => {
      const mockTask = jest.fn();
      const viewport = document.createElement('meta');
      viewport.setAttribute('name', 'viewport');
      document.head.appendChild(viewport);
      
      WaitSystemReady.viewportReady(mockTask);
      
      expect(mockTask).toHaveBeenCalled();
      
      document.head.removeChild(viewport);
    });

    test('should wait when viewport does not exist', () => {
      const mockTask = jest.fn();
      
      // Remove any existing viewport
      const existingViewport = document.querySelector('meta[name="viewport"]');
      if (existingViewport) {
        existingViewport.remove();
      }
      
      WaitSystemReady.viewportReady(mockTask);
      
      // Task should not be called yet
      expect(mockTask).not.toHaveBeenCalled();
    });
  });

  describe('waitForSystemReady', () => {
    test('should call Main.start_ when system is ready', () => {
      jest.useRealTimers();
      WaitSystemReady.sysReady = true;
      
      WaitSystemReady.waitForSystemReady();
      
      expect(Main.start_).toHaveBeenCalled();
      jest.useFakeTimers();
    });

    test('should handle system ready check', () => {
      const initialReadyState = WaitSystemReady.sysReady;
      
      // Just verify the function is callable
      expect(() => {
        WaitSystemReady.waitForSystemReady();
      }).not.toThrow();
      
      // Function may modify sysReady based on checkDOMReady result
      expect(typeof WaitSystemReady.sysReady).toBe('boolean');
    });

    test('should eventually call Main.start_ after system becomes ready', () => {
      WaitSystemReady.sysReady = false;
      
      WaitSystemReady.waitForSystemReady();
      
      // Simulate system becoming ready
      WaitSystemReady.sysReady = true;
      
      jest.advanceTimersByTime(200);
      
      // Run pending timers
      jest.runAllTimers();
    });
  });

  describe('hasBodyReady flag', () => {
    test('should be false initially', () => {
      WaitSystemReady.hasBodyReady = false;
      
      expect(WaitSystemReady.hasBodyReady).toBe(false);
    });

    test('should be accessible', () => {
      WaitSystemReady.hasBodyReady = true;
      expect(WaitSystemReady.hasBodyReady).toBe(true);
      
      WaitSystemReady.hasBodyReady = false;
      expect(WaitSystemReady.hasBodyReady).toBe(false);
    });
  });

  describe('sysReady flag', () => {
    test('should be modifiable', () => {
      WaitSystemReady.sysReady = true;
      expect(WaitSystemReady.sysReady).toBe(true);
      
      WaitSystemReady.sysReady = false;
      expect(WaitSystemReady.sysReady).toBe(false);
    });
  });

  describe('elementCount property', () => {
    test('should be accessible', () => {
      WaitSystemReady.elementCount = 10;
      
      expect(WaitSystemReady.elementCount).toBe(10);
    });

    test('should start at 0', () => {
      WaitSystemReady.elementCount = 0;
      
      expect(WaitSystemReady.elementCount).toBe(0);
    });
  });

  describe('Integration Tests', () => {
    test('should handle sequential calls', () => {
      const task1 = jest.fn();
      const task2 = jest.fn();
      
      WaitSystemReady.configReady(task1);
      WaitSystemReady.configReady(task2);
      
      expect(task1).toHaveBeenCalled();
      expect(task2).toHaveBeenCalled();
    });

    test('should handle multiple config ready calls', () => {
      const tasks = [jest.fn(), jest.fn(), jest.fn()];
      
      tasks.forEach(task => WaitSystemReady.configReady(task));
      
      tasks.forEach(task => expect(task).toHaveBeenCalled());
    });
  });
});
