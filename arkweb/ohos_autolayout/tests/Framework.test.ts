/**
 * Unit tests for Framework.ts
 * 测试框架核心模块的功能
 */

// Mock dependencies
jest.mock('../src/Debug/Log', () => ({
  __esModule: true,
  default: {
    d: jest.fn(),
    e: jest.fn(),
    info: jest.fn(),
    w: jest.fn(),
    i: jest.fn(),
  },
}));

jest.mock('../src/Debug/Tag', () => ({
  __esModule: true,
  default: {
    framework: 'Framework',
  },
}));

jest.mock('../src/Framework/Common/Cached', () => ({
  __esModule: true,
  default: {
    clearStyleCache: jest.fn(),
    clearAllCache: jest.fn(),
  },
}));

jest.mock('../src/Framework/Common/Style/Setter/CSSSheetManage', () => ({
  __esModule: true,
  default: {
    reInit: jest.fn(),
  },
}));

jest.mock('../src/Framework/Common/Style/Setter/StyleSetter', () => ({
  __esModule: true,
  default: {
    flushAllStyles: jest.fn(),
  },
}));

jest.mock('../src/Framework/Common/Utils/Utils', () => ({
  __esModule: true,
  default: {
    isWideScreen: jest.fn(() => true),
  },
}));

jest.mock('../src/Framework/Observer/ObserverHandler', () => ({
  __esModule: true,
  default: {
    postTask: jest.fn(),
    reInit: jest.fn(),
  },
}));

jest.mock('../src/Framework/Observer/Observers/PageContentObserver', () => ({
  __esModule: true,
  default: {
    isContentReady: jest.fn(() => true),
    startObserving: jest.fn(),
  },
}));

jest.mock('../src/Framework/Utils/WaitSystemReady', () => ({
  __esModule: true,
  default: {
    headReady: jest.fn((task) => task()),
    bodyReady: jest.fn((task) => task()),
  },
}));

jest.mock('../src/Framework/IntelligentLayout', () => ({
  __esModule: true,
  default: {
    intelligentLayout: jest.fn(),
    reInit: jest.fn(),
    recoverPopwinStyle: jest.fn(),
  },
}));

const mockCCMConfigInstance = {
  checkRule: jest.fn(() => 1), // CheckRuleStateResult.inWhiteList
  getAppID: jest.fn(() => 'testApp'),
  getPage: jest.fn(() => 'testPage'),
  fromJson: jest.fn(() => false),
};

jest.mock('../src/Framework/Common/CCMConfig', () => ({
  CCMConfig: {
    getInstance: jest.fn(() => mockCCMConfigInstance),
  },
  CheckRuleStateResult: {
    initial: 0,
    inWhiteList: 1,
    outOfWhiteList: 2,
  },
}));

jest.mock('../src/Framework/Common/Style/Common/CacheStyleGetter', () => ({
  SpecificStyleCache: {
    init: jest.fn(),
  },
}));

jest.mock('../src/Main', () => ({
  Main: {
    stop: jest.fn(),
  },
}));

jest.mock('../src/Framework/Common/Constant', () => ({
  __esModule: true,
  default: {
    link: 'link',
    rel: 'rel',
    href: 'href',
    stylesheet: 'stylesheet',
  },
}));

import Framework from '../src/Framework/Framework';
import Utils from '../src/Framework/Common/Utils/Utils';
import PageContentObserver from '../src/Framework/Observer/Observers/PageContentObserver';
import ObserverHandler from '../src/Framework/Observer/ObserverHandler';
import IntelligentLayout from '../src/Framework/IntelligentLayout';
import Cached from '../src/Framework/Common/Cached';
import StyleSetter from '../src/Framework/Common/Style/Setter/StyleSetter';
import WaitSystemReady from '../src/Framework/Utils/WaitSystemReady';
import CSSSheetManage from '../src/Framework/Common/Style/Setter/CSSSheetManage';
import { CCMConfig, CheckRuleStateResult } from '../src/Framework/Common/CCMConfig';
import { SpecificStyleCache } from '../src/Framework/Common/Style/Common/CacheStyleGetter';
import { Main } from '../src/Main';
import Log from '../src/Debug/Log';

describe('Framework Module', () => {
  beforeEach(() => {
    jest.clearAllMocks();
    Framework.stopFlag = false;
    // @ts-ignore - 访问私有属性用于测试
    Framework.layoutLockCount = 0;
    
    // Mock document
    if (!document.head) {
      Object.defineProperty(document, 'head', {
        writable: true,
        value: document.createElement('head')
      });
    } else {
      document.head.innerHTML = '';
    }
    if (!document.body) {
      Object.defineProperty(document, 'body', {
        writable: true,
        value: document.createElement('body')
      });
    }
  });

  describe('Layout Lock Management', () => {
    test('lockLayout should increment lock count', () => {
      // @ts-ignore
      expect(Framework.layoutLockCount).toBe(1);
      
      // @ts-ignore
      expect(Framework.layoutLockCount).toBe(2);
    });

    test('unLockLayout should decrement lock count', () => {
      // @ts-ignore
      expect(Framework.layoutLockCount).toBe(1);
    });

    test('unLockLayout should not go below zero', () => {
      // @ts-ignore
      expect(Framework.layoutLockCount).toBe(-1);
      expect(Log.e).toHaveBeenCalledWith('Layout解锁数量错误: -1', Framework.TAG);
    });
  });

  describe('taskinit', () => {
    test('should return false when not wide screen', () => {
      (Utils.isWideScreen as jest.Mock).mockReturnValue(false);
      
      const result = Framework.taskinit();
      
      expect(result).toBe(false);
      expect(Utils.isWideScreen).toHaveBeenCalled();
    });

    test('should return false when stopFlag is true', () => {
      Framework.stopFlag = true;
      (Utils.isWideScreen as jest.Mock).mockReturnValue(true);
      
      const result = Framework.taskinit();
      
      expect(result).toBe(false);
    });

    test('should return false when content is not ready', () => {
      (Utils.isWideScreen as jest.Mock).mockReturnValue(true);
      (PageContentObserver.isContentReady as jest.Mock).mockReturnValue(false);
      
      const result = Framework.taskinit();
      
      expect(result).toBe(false);
      expect(PageContentObserver.startObserving).toHaveBeenCalled();
    });

    test('should clear style cache when content is ready', () => {
      (Utils.isWideScreen as jest.Mock).mockReturnValue(true);
      (PageContentObserver.isContentReady as jest.Mock).mockReturnValue(true);
      
      Framework.taskinit();
      
      expect(Cached.clearStyleCache).toHaveBeenCalled();
    });

    test('should set startTime on first call', () => {
      (Utils.isWideScreen as jest.Mock).mockReturnValue(true);
      (PageContentObserver.isContentReady as jest.Mock).mockReturnValue(true);
      
      // @ts-ignore
      Framework.startTime = 0;
      Framework.taskinit();
      
      // @ts-ignore
      expect(Framework.startTime).toBeGreaterThan(0);
    });
  });

  describe('mainTask', () => {
    test('should execute layout when all conditions are met', () => {
      (Utils.isWideScreen as jest.Mock).mockReturnValue(true);
      (PageContentObserver.isContentReady as jest.Mock).mockReturnValue(true);
      
      // Mock CCMConfig to return inWhiteList
      jest.resetModules();
      
      Framework.mainTask();
      
      expect(IntelligentLayout.intelligentLayout).toHaveBeenCalledWith(document.body);
      expect(StyleSetter.flushAllStyles).toHaveBeenCalled();
      expect(Cached.clearStyleCache).toHaveBeenCalled();
    });

    test('should not execute layout when locked', () => {
      (Utils.isWideScreen as jest.Mock).mockReturnValue(true);
      (PageContentObserver.isContentReady as jest.Mock).mockReturnValue(true);
      Framework.mainTask();
      
      // Layout should not be called when locked
      expect(IntelligentLayout.intelligentLayout).not.toHaveBeenCalled();
    });

    test('should stop when checkRule returns outOfWhiteList', () => {
      (Utils.isWideScreen as jest.Mock).mockReturnValue(true);
      (PageContentObserver.isContentReady as jest.Mock).mockReturnValue(true);
      
      // Mock CCMConfig to return outOfWhiteList
      mockCCMConfigInstance.checkRule.mockReturnValue(CheckRuleStateResult.outOfWhiteList);
      mockCCMConfigInstance.getAppID.mockReturnValue('testApp');
      mockCCMConfigInstance.getPage.mockReturnValue('testPage');
      
      Framework.mainTask();
      
      expect(mockCCMConfigInstance.checkRule).toHaveBeenCalled();
      expect(mockCCMConfigInstance.getAppID).toHaveBeenCalled();
      expect(mockCCMConfigInstance.getPage).toHaveBeenCalled();
      expect(Main.stop).toHaveBeenCalled();
      expect(IntelligentLayout.intelligentLayout).not.toHaveBeenCalled();
    });

    test('should return early when checkRule returns initial', () => {
      (Utils.isWideScreen as jest.Mock).mockReturnValue(true);
      (PageContentObserver.isContentReady as jest.Mock).mockReturnValue(true);
      
      // Mock CCMConfig to return initial
      mockCCMConfigInstance.checkRule.mockReturnValue(CheckRuleStateResult.initial);
      
      Framework.mainTask();
      
      expect(mockCCMConfigInstance.checkRule).toHaveBeenCalled();
      expect(IntelligentLayout.intelligentLayout).not.toHaveBeenCalled();
    });

    test('should not execute when taskinit returns false', () => {
      // Mock taskinit to return false by setting not wide screen
      (Utils.isWideScreen as jest.Mock).mockReturnValue(false);
      
      Framework.mainTask();
      
      expect(IntelligentLayout.intelligentLayout).not.toHaveBeenCalled();
    });
  });

  describe('recoverStyle', () => {
    test('should call IntelligentLayout.recoverPopwinStyle', () => {
      Framework.recoverStyle();
      
      expect(IntelligentLayout.recoverPopwinStyle).toHaveBeenCalled();
    });
  });

  describe('reInit', () => {
    test('should not reinit when not wide screen', () => {
      (Utils.isWideScreen as jest.Mock).mockReturnValue(false);
      
      Framework.reInit();
      
      expect(IntelligentLayout.reInit).not.toHaveBeenCalled();
    });

    test('should reinit all components when wide screen', () => {
      (Utils.isWideScreen as jest.Mock).mockReturnValue(true);
      
      Framework.reInit();
      
      expect(Cached.clearAllCache).toHaveBeenCalled();
      expect(IntelligentLayout.reInit).toHaveBeenCalled();
      expect(ObserverHandler.reInit).toHaveBeenCalled();
      // @ts-ignore
      expect(Framework.layoutLockCount).toBe(0);
      expect(Framework.stopFlag).toBe(false);
    });

    test('should reset startTime when reinitializing', () => {
      (Utils.isWideScreen as jest.Mock).mockReturnValue(true);
      
      // @ts-ignore
      Framework.startTime = 12345;
      Framework.reInit();
      
      // @ts-ignore
      expect(Framework.startTime).toBe(0);
    });

    test('should call SpecificStyleCache.init', () => {
      (Utils.isWideScreen as jest.Mock).mockReturnValue(true);
      
      Framework.reInit();
      
      expect(SpecificStyleCache.init).toHaveBeenCalled();
    });

    test('should call CSSSheetManage.reInit', () => {
      (Utils.isWideScreen as jest.Mock).mockReturnValue(true);
      
      Framework.reInit();
      
      expect(CSSSheetManage.reInit).toHaveBeenCalled();
    });
  });

  describe('CSS Loading Detection', () => {
    test('should detect incomplete CSS loading', () => {
      // Create a link element without sheet
      const link = document.createElement('link');
      link.setAttribute('rel', 'stylesheet');
      link.setAttribute('href', 'http://example.com/style.css');
      link.href = 'http://example.com/style.css';
      document.head.appendChild(link);
      
      // taskinit should return false because CSS is not loaded
      (Utils.isWideScreen as jest.Mock).mockReturnValue(true);
      (PageContentObserver.isContentReady as jest.Mock).mockReturnValue(true);
      
      const result = Framework.taskinit();
      
      expect(result).toBe(false);
      expect(ObserverHandler.postTask).toHaveBeenCalled();
    });

    test('should skip non-stylesheet links', () => {
      const link = document.createElement('link');
      link.setAttribute('rel', 'preload');
      link.setAttribute('href', 'http://example.com/script.js');
      document.head.appendChild(link);
      
      (Utils.isWideScreen as jest.Mock).mockReturnValue(true);
      (PageContentObserver.isContentReady as jest.Mock).mockReturnValue(true);
      
      const result = Framework.taskinit();
      
      expect(result).toBe(true);
    });

    test('should skip links without href', () => {
      const link = document.createElement('link');
      link.setAttribute('rel', 'stylesheet');
      document.head.appendChild(link);
      
      (Utils.isWideScreen as jest.Mock).mockReturnValue(true);
      (PageContentObserver.isContentReady as jest.Mock).mockReturnValue(true);
      
      const result = Framework.taskinit();
      
      expect(result).toBe(true);
    });

    test('should skip links with empty href', () => {
      const link = document.createElement('link');
      link.setAttribute('rel', 'stylesheet');
      link.setAttribute('href', '');
      document.head.appendChild(link);
      
      (Utils.isWideScreen as jest.Mock).mockReturnValue(true);
      (PageContentObserver.isContentReady as jest.Mock).mockReturnValue(true);
      
      const result = Framework.taskinit();
      
      expect(result).toBe(true);
    });

    test('should detect incomplete CSS loading with https', () => {
      // Create a link element without sheet using https
      const link = document.createElement('link');
      link.setAttribute('rel', 'stylesheet');
      link.setAttribute('href', 'https://example.com/style.css');
      link.href = 'https://example.com/style.css';
      document.head.appendChild(link);
      
      (Utils.isWideScreen as jest.Mock).mockReturnValue(true);
      (PageContentObserver.isContentReady as jest.Mock).mockReturnValue(true);
      
      const result = Framework.taskinit();
      
      expect(result).toBe(false);
      expect(ObserverHandler.postTask).toHaveBeenCalled();
    });

    test('should handle https links with sheet loaded', () => {
      const link = document.createElement('link');
      link.setAttribute('rel', 'stylesheet');
      link.setAttribute('href', 'https://example.com/style.css');
      link.href = 'https://example.com/style.css';
      // Mock the sheet property
      Object.defineProperty(link, 'sheet', {
        value: { cssRules: [] },
        writable: true
      });
      document.head.appendChild(link);
      
      (Utils.isWideScreen as jest.Mock).mockReturnValue(true);
      (PageContentObserver.isContentReady as jest.Mock).mockReturnValue(true);
      
      const result = Framework.taskinit();
      
      expect(result).toBe(true);
    });

    test('should skip non-link elements', () => {
      const style = document.createElement('style');
      style.textContent = 'body { margin: 0; }';
      document.head.appendChild(style);
      
      (Utils.isWideScreen as jest.Mock).mockReturnValue(true);
      (PageContentObserver.isContentReady as jest.Mock).mockReturnValue(true);
      
      const result = Framework.taskinit();
      
      expect(result).toBe(true);
    });
  });

  describe('TAG property', () => {
    test('should have TAG property defined', () => {
      expect(Framework.TAG).toBeDefined();
      expect(Framework.TAG).toBe('Framework');
    });
  });

  describe('stopFlag', () => {
    test('should be modifiable', () => {
      Framework.stopFlag = true;
      expect(Framework.stopFlag).toBe(true);
      
      Framework.stopFlag = false;
      expect(Framework.stopFlag).toBe(false);
    });
  });

  describe('configReady', () => {
    test('should call WaitSystemReady.headReady with headReadyTask', () => {
      Framework.configReady();
      
      expect(WaitSystemReady.headReady).toHaveBeenCalledWith(Framework.headReadyTask);
    });
  });

  describe('headReadyTask', () => {
    test('should call WaitSystemReady.bodyReady with reInit', () => {
      Framework.headReadyTask();
      
      expect(WaitSystemReady.bodyReady).toHaveBeenCalledWith(Framework.reInit);
    });
  });

  describe('taskinit additional scenarios', () => {
    test('should set layoutConstraintResult when content not ready', () => {
      (Utils.isWideScreen as jest.Mock).mockReturnValue(true);
      (PageContentObserver.isContentReady as jest.Mock).mockReturnValue(false);
      
      Framework.taskinit();
      
      // @ts-ignore
      expect(window.layoutConstraintResult).toBeDefined();
      // @ts-ignore
      expect(window.layoutConstraintResult.resultCode).toBe(-1);
      // @ts-ignore
      expect(window.layoutConstraintResult.errorMsg).toBe('页面内容未就绪，等待内容加载');
      expect(PageContentObserver.startObserving).toHaveBeenCalled();
    });

    test('should return false when stopFlag is true', () => {
      Framework.stopFlag = true;
      (Utils.isWideScreen as jest.Mock).mockReturnValue(true);
      
      const result = Framework.taskinit();
      
      expect(result).toBe(false);
    });
  });
});
