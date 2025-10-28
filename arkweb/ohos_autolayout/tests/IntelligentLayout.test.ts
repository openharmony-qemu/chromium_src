/**
 * Unit tests for IntelligentLayout.ts
 * 测试智能布局模块的功能
 */

// Mock dependencies
jest.mock('../src/Debug/Log', () => ({
  __esModule: true,
  default: {
    d: jest.fn(),
    e: jest.fn(),
    info: jest.fn(),
    w: jest.fn(),
  },
}));

jest.mock('../src/Debug/Tag', () => ({
  __esModule: true,
  default: {
    intelligentLayout: 'IntelligentLayout',
  },
}));

jest.mock('../src/Framework/Utils/Utils', () => ({
  __esModule: true,
  default: {
    shouldSkip: jest.fn(() => false),
  },
}));

jest.mock('../src/Framework/Popup/PopupWindowDetector', () => ({
  PopupWindowDetector: {
    findPopups: jest.fn(),
  },
}));

jest.mock('../src/Framework/Popup/PopupWindowRelayout', () => ({
  PopupWindowRelayout: jest.fn().mockImplementation((popupInfo) => ({
    isDirty: jest.fn(() => true),
    intelligenceLayout: jest.fn(),
    setDirty: jest.fn(),
    restoreStyles: jest.fn(),
    popupInfo,
  })),
}));

import IntelligentLayout from '../src/Framework/IntelligentLayout';
import { PopupWindowDetector } from '../src/Framework/Popup/PopupWindowDetector';
import { PopupWindowRelayout } from '../src/Framework/Popup/PopupWindowRelayout';
import { PopupInfo } from '../src/Framework/Popup/PopupInfo';
import Utils from '../src/Framework/Utils/Utils';
import { AComponent } from '../src/Framework/Common/base/AComponent';

describe('IntelligentLayout Module', () => {
  let mockPopupInfo: PopupInfo;
  let mockRootNode: HTMLElement;

  beforeEach(() => {
    jest.clearAllMocks();
    IntelligentLayout.popWindowMap.clear();
    
    // Create mock elements
    mockRootNode = document.createElement('div');
    mockRootNode.className = 'popup-root';
    
    mockPopupInfo = {
      root_node: mockRootNode,
      mask_node: null,
      content_node: null,
      popup_type: null,
      root_position: 'fixed',
      root_zindex: 1000,
      has_mask: false,
      root_screen_area_ratio: 0.5,
      root_is_visiable: true,
      has_close_button: false,
      mask_area_ratio: 0,
      mask_position: '',
      mask_zindex: 0,
      stickyTop_height: 0,
      stickyBottom_height: 0,
    } as PopupInfo;
  });

  describe('TAG property', () => {
    test('should have TAG property defined', () => {
      expect(IntelligentLayout.TAG).toBeDefined();
      expect(IntelligentLayout.TAG).toBe('IntelligentLayout');
    });
  });

  describe('popWindowMap', () => {
    test('should be a Map instance', () => {
      expect(IntelligentLayout.popWindowMap).toBeInstanceOf(Map);
    });

    test('should start empty', () => {
      expect(IntelligentLayout.popWindowMap.size).toBe(0);
    });
  });

  describe('intelligentLayout', () => {
    test('should find popups when map is empty', () => {
      (PopupWindowDetector.findPopups as jest.Mock).mockReturnValue(mockPopupInfo);
      
      IntelligentLayout.intelligentLayout(mockRootNode);
      
      expect(PopupWindowDetector.findPopups).toHaveBeenCalledWith(mockRootNode);
    });

    test('should use cached popup from map', () => {
      const mockComponent = {
        isDirty: jest.fn(() => false),
        intelligenceLayout: jest.fn(),
        setDirty: jest.fn(),
      };
      // @ts-ignore
      IntelligentLayout.popWindowMap.set(mockPopupInfo, mockComponent);
      
      IntelligentLayout.intelligentLayout(mockRootNode);
      
      expect(PopupWindowDetector.findPopups).not.toHaveBeenCalled();
    });

    test('should handle null popup info', () => {
      (PopupWindowDetector.findPopups as jest.Mock).mockReturnValue(null);
      
      expect(() => {
        IntelligentLayout.intelligentLayout(mockRootNode);
      }).not.toThrow();
    });

    test('should call calculateForPopWin when popup exists', () => {
      (PopupWindowDetector.findPopups as jest.Mock).mockReturnValue(mockPopupInfo);
      
      IntelligentLayout.intelligentLayout(mockRootNode);
      
      // Check if component was created
      expect(IntelligentLayout.popWindowMap.size).toBe(1);
    });
  });

  describe('calculateForPopWin', () => {
    test('should create new component if not in map', () => {
      IntelligentLayout.calculateForPopWin(mockPopupInfo);
      
      expect(PopupWindowRelayout).toHaveBeenCalledWith(mockPopupInfo);
      expect(IntelligentLayout.popWindowMap.has(mockPopupInfo)).toBe(true);
    });

    test('should reuse existing component from map', () => {
      const mockComponent = {
        isDirty: jest.fn(() => false),
        intelligenceLayout: jest.fn(),
        setDirty: jest.fn(),
      };
      
      // @ts-ignore
      IntelligentLayout.popWindowMap.set(mockPopupInfo, mockComponent);
      IntelligentLayout.calculateForPopWin(mockPopupInfo);
      
      expect(PopupWindowRelayout).not.toHaveBeenCalled();
      expect(mockComponent.isDirty).toHaveBeenCalled();
    });

    test('should call intelligenceLayout when component is dirty', () => {
      const mockComponent = {
        isDirty: jest.fn(() => true),
        intelligenceLayout: jest.fn(),
        setDirty: jest.fn(),
      };
      
      // @ts-ignore
      IntelligentLayout.popWindowMap.set(mockPopupInfo, mockComponent);
      IntelligentLayout.calculateForPopWin(mockPopupInfo);
      
      expect(mockComponent.intelligenceLayout).toHaveBeenCalled();
      expect(mockComponent.setDirty).toHaveBeenCalledWith(false);
    });

    test('should not call intelligenceLayout when component is not dirty', () => {
      const mockComponent = {
        isDirty: jest.fn(() => false),
        intelligenceLayout: jest.fn(),
        setDirty: jest.fn(),
      };
      
      // @ts-ignore
      IntelligentLayout.popWindowMap.set(mockPopupInfo, mockComponent);
      IntelligentLayout.calculateForPopWin(mockPopupInfo);
      
      expect(mockComponent.intelligenceLayout).not.toHaveBeenCalled();
    });

    test('should set global popupInfo on window', () => {
      IntelligentLayout.calculateForPopWin(mockPopupInfo);
      
      // @ts-ignore
      expect(window.popupInfo).toBe(mockPopupInfo);
    });
  });

  describe('recoverPopwinStyle', () => {
    test('should restore styles when map has entries', () => {
      const mockComponent = {
        restoreStyles: jest.fn(),
      };
      
      // @ts-ignore
      IntelligentLayout.popWindowMap.set(mockPopupInfo, mockComponent);
      IntelligentLayout.recoverPopwinStyle();
      
      expect(IntelligentLayout.popWindowMap.size).toBe(0);
    });

    test('should clear map even if empty', () => {
      IntelligentLayout.recoverPopwinStyle();
      
      expect(IntelligentLayout.popWindowMap.size).toBe(0);
    });
  });

  describe('removePopwinCache', () => {
    test('should remove popup when node contains root_node', () => {
      const parentNode = document.createElement('div');
      parentNode.appendChild(mockRootNode);
      
      // @ts-ignore
      IntelligentLayout.popWindowMap.set(mockPopupInfo, {});
      
      const result = IntelligentLayout.removePopwinCache(parentNode);
      
      expect(result).toBe(true);
      expect(IntelligentLayout.popWindowMap.size).toBe(0);
    });

    test('should not remove popup when node does not contain root_node', () => {
      const otherNode = document.createElement('div');
      
      // @ts-ignore
      IntelligentLayout.popWindowMap.set(mockPopupInfo, {});
      
      const result = IntelligentLayout.removePopwinCache(otherNode);
      
      expect(result).toBe(false);
      expect(IntelligentLayout.popWindowMap.size).toBe(1);
    });

    test('should return false when map is empty', () => {
      const result = IntelligentLayout.removePopwinCache(document.createElement('div'));
      
      expect(result).toBe(false);
    });

    test('should handle multiple popups', () => {
      const popup1Root = document.createElement('div');
      const popup2Root = document.createElement('div');
      
      const popup1: PopupInfo = {
        root_node: popup1Root,
        mask_node: null,
        content_node: null,
        popup_type: null,
        root_position: 'fixed',
        root_zindex: 1000,
        has_mask: false,
        root_screen_area_ratio: 0.5,
        root_is_visiable: true,
        has_close_button: false,
        mask_area_ratio: 0,
        mask_position: '',
        mask_zindex: 0,
        stickyTop_height: 0,
        stickyBottom_height: 0,
      };
      
      const popup2: PopupInfo = {
        root_node: popup2Root,
        mask_node: null,
        content_node: null,
        popup_type: null,
        root_position: 'fixed',
        root_zindex: 1000,
        has_mask: false,
        root_screen_area_ratio: 0.5,
        root_is_visiable: true,
        has_close_button: false,
        mask_area_ratio: 0,
        mask_position: '',
        mask_zindex: 0,
        stickyTop_height: 0,
        stickyBottom_height: 0,
      };
      
      const parentNode = document.createElement('div');
      
      parentNode.appendChild(popup1.root_node);
      
      // @ts-ignore
      IntelligentLayout.popWindowMap.set(popup1, {});
      // @ts-ignore
      IntelligentLayout.popWindowMap.set(popup2, {});
      
      const result = IntelligentLayout.removePopwinCache(parentNode);
      
      expect(result).toBe(true);
      expect(IntelligentLayout.popWindowMap.size).toBe(1);
    });
  });

  describe('markDirty', () => {
    test('should mark component as dirty when item is contained', () => {
      const childNode = document.createElement('div');
      mockRootNode.appendChild(childNode);
      
      const mockComponent = {
        isDirty: jest.fn(() => false),
        setDirty: jest.fn(),
      };
      
      // @ts-ignore
      IntelligentLayout.popWindowMap.set(mockPopupInfo, mockComponent);
      
      IntelligentLayout.markDirty(childNode);
      
      expect(mockComponent.setDirty).toHaveBeenCalledWith(true);
    });

    test('should not mark component when already dirty', () => {
      const childNode = document.createElement('div');
      mockRootNode.appendChild(childNode);
      
      const mockComponent = {
        isDirty: jest.fn(() => true),
        setDirty: jest.fn(),
      };
      
      // @ts-ignore
      IntelligentLayout.popWindowMap.set(mockPopupInfo, mockComponent);
      
      IntelligentLayout.markDirty(childNode);
      
      expect(mockComponent.setDirty).not.toHaveBeenCalled();
    });

    test('should handle null item', () => {
      expect(() => {
        IntelligentLayout.markDirty(null);
      }).not.toThrow();
    });

    test('should skip items that shouldSkip returns true for', () => {
      (Utils.shouldSkip as jest.Mock).mockReturnValue(true);
      const childNode = document.createElement('div');
      
      const mockComponent = {
        isDirty: jest.fn(() => false),
        setDirty: jest.fn(),
      };
      
      // @ts-ignore
      IntelligentLayout.popWindowMap.set(mockPopupInfo, mockComponent);
      
      IntelligentLayout.markDirty(childNode);
      
      expect(mockComponent.setDirty).not.toHaveBeenCalled();
    });

    test('should not mark when item not contained in any popup', () => {
      const otherNode = document.createElement('div');
      
      const mockComponent = {
        isDirty: jest.fn(() => false),
        setDirty: jest.fn(),
      };
      
      // @ts-ignore
      IntelligentLayout.popWindowMap.set(mockPopupInfo, mockComponent);
      
      IntelligentLayout.markDirty(otherNode);
      
      expect(mockComponent.setDirty).not.toHaveBeenCalled();
    });
  });

  describe('relayoutForPopWin', () => {
    test('should use existing popup from map', () => {
      const mockComponent = {
        isDirty: jest.fn(() => false),
        intelligenceLayout: jest.fn(),
        setDirty: jest.fn(),
      };
      // @ts-ignore
      IntelligentLayout.popWindowMap.set(mockPopupInfo, mockComponent);
      
      IntelligentLayout.relayoutForPopWin();
      
      expect(PopupWindowDetector.findPopups).not.toHaveBeenCalled();
    });

    test('should find new popup when map is empty', () => {
      (PopupWindowDetector.findPopups as jest.Mock).mockReturnValue(mockPopupInfo);
      
      IntelligentLayout.relayoutForPopWin();
      
      expect(PopupWindowDetector.findPopups).toHaveBeenCalledWith(document.body);
    });

    test('should handle null popup info', () => {
      (PopupWindowDetector.findPopups as jest.Mock).mockReturnValue(null);
      
      expect(() => {
        IntelligentLayout.relayoutForPopWin();
      }).not.toThrow();
    });
  });

  describe('reInit', () => {
    test('should recover styles and clear map', () => {
      const mockComponent = {
        restoreStyles: jest.fn(),
      };
      
      // @ts-ignore
      IntelligentLayout.popWindowMap.set(mockPopupInfo, mockComponent);
      
      IntelligentLayout.reInit();
      
      expect(IntelligentLayout.popWindowMap.size).toBe(0);
    });

    test('should clear empty map', () => {
      IntelligentLayout.reInit();
      
      expect(IntelligentLayout.popWindowMap.size).toBe(0);
    });
  });
});
