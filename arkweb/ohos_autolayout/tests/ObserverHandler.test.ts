/**
 * Unit tests for ObserverHandler.ts
 * 测试观察者处理器模块的功能
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
    observerHandler: 'ObserverHandler',
    resizeObserver: 'ResizeObserver',
  },
}));

jest.mock('../src/Framework/Framework', () => ({
  __esModule: true,
  default: {
    mainTask: jest.fn(),
    stopFlag: false,
  },
}));

jest.mock('../src/Framework/Utils/Throttle', () => {
  return jest.fn().mockImplementation((timeout, callback) => {
    return {
      postTask: jest.fn(() => callback()),
    };
  });
});

jest.mock('../src/Framework/Observer/Observers/ModifyObserver', () => ({
  __esModule: true,
  default: {
    reInit: jest.fn(),
    disconnect: jest.fn(),
  },
}));

jest.mock('../src/Framework/Observer/Observers/PageContentObserver', () => ({
  __esModule: true,
  default: {
    reInit: jest.fn(),
    disconnect: jest.fn(),
  },
}));

jest.mock('../src/Framework/Observer/Observers/ResizeObserver', () => ({
  __esModule: true,
  default: {
    init_: jest.fn(),
    removeListener: jest.fn(),
  },
}));

jest.mock('../src/Framework/Common/Utils/Utils', () => ({
  __esModule: true,
  default: {
    isWideScreen: jest.fn(() => false),
  },
}));

jest.mock('../src/Framework/Common/Utils/Store', () => ({
  __esModule: true,
  default: {
    setValue: jest.fn(),
    nextRoundCache: [] as HTMLElement[],
  },
}));

import ObserverHandler from '../src/Framework/Observer/ObserverHandler';
import Framework from '../src/Framework/Framework';
import ModifyObserver from '../src/Framework/Observer/Observers/ModifyObserver';
import PageContentObserver from '../src/Framework/Observer/Observers/PageContentObserver';
import ResizeObserver from '../src/Framework/Observer/Observers/ResizeObserver';
import Utils from '../src/Framework/Common/Utils/Utils';
import Store from '../src/Framework/Common/Utils/Store';
import Throttle from '../src/Framework/Utils/Throttle';

describe('ObserverHandler Module', () => {
  beforeEach(() => {
    jest.clearAllMocks();
    ObserverHandler.relayoutHandler = null;
    Store.nextRoundCache = [];
  });

  describe('postTask', () => {
    test('should create relayoutHandler if not exists', () => {
      ObserverHandler.postTask();
      
      expect(ObserverHandler.relayoutHandler).not.toBeNull();
      expect(Throttle).toHaveBeenCalled();
    });

    test('should call postTask on relayoutHandler', () => {
      ObserverHandler.postTask();
      
      expect(ObserverHandler.relayoutHandler.postTask).toHaveBeenCalled();
    });

    test('should reuse existing relayoutHandler', () => {
      ObserverHandler.postTask();
      const firstHandler = ObserverHandler.relayoutHandler;
      
      ObserverHandler.postTask();
      
      expect(ObserverHandler.relayoutHandler).toBe(firstHandler);
    });

    test('should trigger Framework.mainTask through throttle', () => {
      ObserverHandler.postTask();
      
      expect(Framework.mainTask).toHaveBeenCalled();
    });
  });

  describe('reInit', () => {
    test('should call reInit on all observers', () => {
      ObserverHandler.reInit();
      
      expect(ModifyObserver.reInit).toHaveBeenCalled();
      expect(PageContentObserver.reInit).toHaveBeenCalled();
      expect(ResizeObserver.init_).toHaveBeenCalled();
    });

    test('should post task after reinit', () => {
      ObserverHandler.reInit();
      
      expect(Throttle).toHaveBeenCalled();
    });

    test('should be callable multiple times', () => {
      ObserverHandler.reInit();
      ObserverHandler.reInit();
      
      expect(ModifyObserver.reInit).toHaveBeenCalledTimes(2);
      expect(PageContentObserver.reInit).toHaveBeenCalledTimes(2);
      expect(ResizeObserver.init_).toHaveBeenCalledTimes(2);
    });
  });

  describe('updateObserver', () => {
    test('should remove listener when stopFlag is true', () => {
      Framework.stopFlag = true;
      
      ObserverHandler.updateObserver();
      
      expect(ResizeObserver.removeListener).toHaveBeenCalled();
    });

    test('should not remove listener when stopFlag is false and not wide screen', () => {
      Framework.stopFlag = false;
      (Utils.isWideScreen as jest.Mock).mockReturnValue(false);
      
      ObserverHandler.updateObserver();
      
      expect(ModifyObserver.disconnect).toHaveBeenCalled();
      expect(PageContentObserver.disconnect).toHaveBeenCalled();
    });

    test('should return early when is wide screen', () => {
      (Utils.isWideScreen as jest.Mock).mockReturnValue(true);
      
      ObserverHandler.updateObserver();
      
      expect(ModifyObserver.disconnect).not.toHaveBeenCalled();
      expect(PageContentObserver.disconnect).not.toHaveBeenCalled();
    });

    test('should disconnect observers when not wide screen', () => {
      (Utils.isWideScreen as jest.Mock).mockReturnValue(false);
      
      ObserverHandler.updateObserver();
      
      expect(ModifyObserver.disconnect).toHaveBeenCalled();
      expect(PageContentObserver.disconnect).toHaveBeenCalled();
    });
  });

  describe('nextRound', () => {
    test('should add element to nextRoundCache', () => {
      const element = document.createElement('div');
      
      ObserverHandler.nextRound(element);
      
      expect(Store.nextRoundCache).toContain(element);
    });

    test('should set LAYOUT_TAG on element', () => {
      const element = document.createElement('div');
      
      ObserverHandler.nextRound(element);
      
      expect(Store.setValue).toHaveBeenCalled();
    });

    test('should handle multiple elements', () => {
      const element1 = document.createElement('div');
      const element2 = document.createElement('div');
      
      ObserverHandler.nextRound(element1);
      ObserverHandler.nextRound(element2);
      
      expect(Store.nextRoundCache).toHaveLength(2);
      expect(Store.setValue).toHaveBeenCalledTimes(2);
    });
  });

  describe('nextRoundAndPost', () => {
    test('should call nextRound and postTask', () => {
      const element = document.createElement('div');
      
      ObserverHandler.nextRoundAndPost(element);
      
      expect(Store.nextRoundCache).toContain(element);
      expect(Throttle).toHaveBeenCalled();
    });

    test('should process element before posting', () => {
      const element = document.createElement('div');
      
      ObserverHandler.nextRoundAndPost(element);
      
      expect(Store.setValue).toHaveBeenCalled();
      expect(Framework.mainTask).toHaveBeenCalled();
    });

    test('should handle null element gracefully', () => {
      expect(() => {
        ObserverHandler.nextRoundAndPost(null);
      }).not.toThrow();
    });
  });

  describe('Integration Tests', () => {
    test('should handle full lifecycle', () => {
      // Initialize
      ObserverHandler.reInit();
      
      // Post task
      ObserverHandler.postTask();
      
      // Update observer
      ObserverHandler.updateObserver();
      
      expect(ModifyObserver.reInit).toHaveBeenCalled();
      expect(PageContentObserver.reInit).toHaveBeenCalled();
      expect(ResizeObserver.init_).toHaveBeenCalled();
      expect(ModifyObserver.disconnect).toHaveBeenCalled();
      expect(PageContentObserver.disconnect).toHaveBeenCalled();
    });

    test('should handle multiple elements in sequence', () => {
      const elements = [
        document.createElement('div'),
        document.createElement('span'),
        document.createElement('p'),
      ];
      
      elements.forEach(el => ObserverHandler.nextRound(el));
      
      expect(Store.nextRoundCache).toHaveLength(3);
      expect(Store.setValue).toHaveBeenCalledTimes(3);
    });

    test('should reinit after update', () => {
      ObserverHandler.updateObserver();
      ObserverHandler.reInit();
      
      expect(ModifyObserver.disconnect).toHaveBeenCalled();
      expect(ModifyObserver.reInit).toHaveBeenCalled();
    });

    test('should handle rapid postTask calls', () => {
      ObserverHandler.postTask();
      ObserverHandler.postTask();
      ObserverHandler.postTask();
      
      expect(ObserverHandler.relayoutHandler).not.toBeNull();
      expect(ObserverHandler.relayoutHandler.postTask).toHaveBeenCalledTimes(3);
    });
  });

  describe('Edge Cases', () => {
    test('should handle stopFlag changes', () => {
      Framework.stopFlag = false;
      ObserverHandler.updateObserver();
      
      Framework.stopFlag = true;
      ObserverHandler.updateObserver();
      
      expect(ResizeObserver.removeListener).toHaveBeenCalled();
    });

    test('should handle screen size changes', () => {
      (Utils.isWideScreen as jest.Mock).mockReturnValue(false);
      ObserverHandler.updateObserver();
      
      (Utils.isWideScreen as jest.Mock).mockReturnValue(true);
      ObserverHandler.updateObserver();
      
      expect(ModifyObserver.disconnect).toHaveBeenCalledTimes(1);
    });

    test('should handle empty nextRoundCache', () => {
      Store.nextRoundCache = [];
      const element = document.createElement('div');
      
      ObserverHandler.nextRound(element);
      
      expect(Store.nextRoundCache).toHaveLength(1);
    });
  });
});
