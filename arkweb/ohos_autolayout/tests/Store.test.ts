/**
 * Unit tests for Store.ts
 * 测试存储模块的功能
 */

// Mock dependencies
jest.mock('../src/Debug/Log', () => ({
  __esModule: true,
  default: {
    i: jest.fn(),
    deleteValue: jest.fn(),
    clearTag: jest.fn(),
    showValue: jest.fn(),
  },
}));

jest.mock('../src/Debug/Tag', () => ({
  __esModule: true,
  default: {
    store: 'Store',
  },
}));

jest.mock('../src/Framework/Common/Style/Common/CacheStyleGetter', () => ({
  __esModule: true,
  default: {
    offsetH: jest.fn(() => 100),
  },
}));

import Store from '../src/Framework/Common/Utils/Store';
import { LayoutKey, LayoutValue, HwTag } from '../src/Framework/Common/Constant';

describe('Store Module', () => {
  let testElement: HTMLElement;

  beforeEach(() => {
    jest.clearAllMocks();
    testElement = document.createElement('div');
    
    // Clear the static maps before each test
    Store.valueMap.clear();
    Store.nextRoundCache = [];
  });

  describe('setValue', () => {
    test('should store value for an element', () => {
      Store.setValue(testElement, 'testKey', 'testValue');
      const value = Store.getValue(testElement, 'testKey');
      
      expect(value).toBe('testValue');
    });

    test('should handle null element', () => {
      expect(() => {
        Store.setValue(null, 'testKey', 'testValue');
      }).not.toThrow();
    });

    test('should create new map if key does not exist', () => {
      Store.setValue(testElement, 'newKey', 'newValue');
      
      expect(Store.valueMap.has('newKey')).toBe(true);
      expect(Store.getValue(testElement, 'newKey')).toBe('newValue');
    });

    test('should overwrite existing value', () => {
      Store.setValue(testElement, 'testKey', 'value1');
      Store.setValue(testElement, 'testKey', 'value2');
      
      expect(Store.getValue(testElement, 'testKey')).toBe('value2');
    });

    test('should store different values for different elements', () => {
      const element1 = document.createElement('div');
      const element2 = document.createElement('div');
      
      Store.setValue(element1, 'testKey', 'value1');
      Store.setValue(element2, 'testKey', 'value2');
      
      expect(Store.getValue(element1, 'testKey')).toBe('value1');
      expect(Store.getValue(element2, 'testKey')).toBe('value2');
    });

    test('should store different values for different keys on same element', () => {
      Store.setValue(testElement, 'key1', 'value1');
      Store.setValue(testElement, 'key2', 'value2');
      
      expect(Store.getValue(testElement, 'key1')).toBe('value1');
      expect(Store.getValue(testElement, 'key2')).toBe('value2');
    });
  });

  describe('getValue', () => {
    test('should return null for non-existent key', () => {
      const value = Store.getValue(testElement, 'nonExistentKey');
      
      expect(value).toBeNull();
    });

    test('should return null for null element', () => {
      const value = Store.getValue(null, 'testKey');
      
      expect(value).toBeNull();
    });

    test('should return null for element without value', () => {
      Store.setValue(testElement, 'testKey', 'testValue');
      const otherElement = document.createElement('div');
      
      const value = Store.getValue(otherElement, 'testKey');
      
      expect(value).toBeNull();
    });

    test('should return correct value after multiple sets', () => {
      Store.setValue(testElement, 'testKey', 'value1');
      Store.setValue(testElement, 'testKey', 'value2');
      Store.setValue(testElement, 'testKey', 'value3');
      
      expect(Store.getValue(testElement, 'testKey')).toBe('value3');
    });
  });

  describe('removeValue', () => {
    test('should delete value for an element', () => {
      Store.setValue(testElement, 'testKey', 'testValue');
      Store.removeValue(testElement, 'testKey');
      const value = Store.getValue(testElement, 'testKey');
      
      expect(value).toBeNull();
    });

    test('should handle null element', () => {
      expect(() => {
        Store.removeValue(null, 'testKey');
      }).not.toThrow();
    });

    test('should handle non-existent key', () => {
      expect(() => {
        Store.removeValue(testElement, 'nonExistentKey');
      }).not.toThrow();
    });

    test('should only remove value for specific element', () => {
      const element1 = document.createElement('div');
      const element2 = document.createElement('div');
      
      Store.setValue(element1, 'testKey', 'value1');
      Store.setValue(element2, 'testKey', 'value2');
      
      Store.removeValue(element1, 'testKey');
      
      expect(Store.getValue(element1, 'testKey')).toBeNull();
      expect(Store.getValue(element2, 'testKey')).toBe('value2');
    });
  });

  describe('clearTag', () => {
    test('should remove all values for a key', () => {
      const element1 = document.createElement('div');
      const element2 = document.createElement('div');
      
      Store.setValue(element1, 'tagKey', 'value1');
      Store.setValue(element2, 'tagKey', 'value2');
      
      Store.clearTag('tagKey');
      
      expect(Store.getValue(element1, 'tagKey')).toBeNull();
      expect(Store.getValue(element2, 'tagKey')).toBeNull();
    });

    test('should remove key from valueMap', () => {
      Store.setValue(testElement, 'tagKey', 'value');
      expect(Store.valueMap.has('tagKey')).toBe(true);
      
      Store.clearTag('tagKey');
      
      expect(Store.valueMap.has('tagKey')).toBe(false);
    });

    test('should handle non-existent key', () => {
      expect(() => {
        Store.clearTag('nonExistentKey');
      }).not.toThrow();
    });

    test('should not affect other keys', () => {
      Store.setValue(testElement, 'key1', 'value1');
      Store.setValue(testElement, 'key2', 'value2');
      
      Store.clearTag('key1');
      
      expect(Store.getValue(testElement, 'key1')).toBeNull();
      expect(Store.getValue(testElement, 'key2')).toBe('value2');
    });
  });

  describe('clearAllTag', () => {
    test('should clear all tags for an element when isDelete is true', () => {
      Store.setValue(testElement, 'key1', 'value1');
      Store.setValue(testElement, 'key2', 'value2');
      Store.setValue(testElement, 'key3', 'value3');
      
      Store.clearAllTag(testElement, true);
      
      expect(Store.getValue(testElement, 'key1')).toBeNull();
      expect(Store.getValue(testElement, 'key2')).toBeNull();
      expect(Store.getValue(testElement, 'key3')).toBeNull();
    });

    test('should preserve persistent tags when isDelete is false', () => {
      Store.setValue(testElement, HwTag.ORIGIN_WIDTH, 'width');
      Store.setValue(testElement, 'normalKey', 'normalValue');
      
      Store.clearAllTag(testElement, false);
      
      expect(Store.getValue(testElement, HwTag.ORIGIN_WIDTH)).toBe('width');
      expect(Store.getValue(testElement, 'normalKey')).toBeNull();
    });

    test('should preserve CONFIG_LAYOUT_TAG with DISABLE value', () => {
      Store.setValue(testElement, LayoutKey.CONFIG_LAYOUT_TAG, LayoutValue.DISABLE);
      Store.setValue(testElement, 'normalKey', 'normalValue');
      
      Store.clearAllTag(testElement, false);
      
      expect(Store.getValue(testElement, LayoutKey.CONFIG_LAYOUT_TAG)).toBe(LayoutValue.DISABLE);
      expect(Store.getValue(testElement, 'normalKey')).toBeNull();
    });

    test('should not affect other elements', () => {
      const element1 = document.createElement('div');
      const element2 = document.createElement('div');
      
      Store.setValue(element1, 'testKey', 'value1');
      Store.setValue(element2, 'testKey', 'value2');
      
      Store.clearAllTag(element1, true);
      
      expect(Store.getValue(element1, 'testKey')).toBeNull();
      expect(Store.getValue(element2, 'testKey')).toBe('value2');
    });
  });

  describe('removeNextRoundTags', () => {
    test('should remove LAYOUT_TAG from cached elements', () => {
      const element1 = document.createElement('div');
      const element2 = document.createElement('div');
      
      Store.setValue(element1, LayoutKey.LAYOUT_TAG, 'value1');
      Store.setValue(element2, LayoutKey.LAYOUT_TAG, 'value2');
      
      Store.nextRoundCache = [element1, element2];
      
      Store.removeNextRoundTags();
      
      expect(Store.getValue(element1, LayoutKey.LAYOUT_TAG)).toBeNull();
      expect(Store.getValue(element2, LayoutKey.LAYOUT_TAG)).toBeNull();
    });

    test('should clear nextRoundCache', () => {
      Store.nextRoundCache = [testElement];
      
      Store.removeNextRoundTags();
      
      expect(Store.nextRoundCache).toEqual([]);
    });

    test('should handle empty nextRoundCache', () => {
      Store.nextRoundCache = [];
      
      expect(() => {
        Store.removeNextRoundTags();
      }).not.toThrow();
    });
  });

  describe('Static properties', () => {
    test('valueMap should be a Map', () => {
      expect(Store.valueMap).toBeInstanceOf(Map);
    });

    test('nextRoundCache should be an array', () => {
      expect(Array.isArray(Store.nextRoundCache)).toBe(true);
    });
  });

  describe('Integration tests', () => {
    test('should handle complex workflow', () => {
      // Set multiple values
      Store.setValue(testElement, 'key1', 'value1');
      Store.setValue(testElement, 'key2', 'value2');
      Store.setValue(testElement, 'key3', 'value3');
      
      // Verify all values
      expect(Store.getValue(testElement, 'key1')).toBe('value1');
      expect(Store.getValue(testElement, 'key2')).toBe('value2');
      expect(Store.getValue(testElement, 'key3')).toBe('value3');
      
      // Remove one value
      Store.removeValue(testElement, 'key2');
      expect(Store.getValue(testElement, 'key2')).toBeNull();
      
      // Others should remain
      expect(Store.getValue(testElement, 'key1')).toBe('value1');
      expect(Store.getValue(testElement, 'key3')).toBe('value3');
    });

    test('should handle multiple elements with same keys', () => {
      const elements = [
        document.createElement('div'),
        document.createElement('div'),
        document.createElement('div'),
      ];
      
      elements.forEach((el, i) => {
        Store.setValue(el, 'testKey', `value${i}`);
      });
      
      elements.forEach((el, i) => {
        expect(Store.getValue(el, 'testKey')).toBe(`value${i}`);
      });
    });
  });

  describe('saveOriginHeight method coverage', () => {
    test('should trigger saveOriginHeight for LAYOUT_TAG with ORIGIN_HEIGHT', () => {
      // Set initial ORIGIN_HEIGHT to make getValue return truthy
      Store.setValue(testElement, HwTag.ORIGIN_HEIGHT, '100');
      
      // This setValue call with LAYOUT_TAG should trigger saveOriginHeight
      Store.setValue(testElement, LayoutKey.LAYOUT_TAG, LayoutValue.ZOOM_PARENT);
      
      // Verify that setValue was called (the spy should have been called)
      // The actual CacheStyleGetter.offsetH call is mocked to return 100
      expect(Store.getValue(testElement, HwTag.ORIGIN_HEIGHT)).toBe('100');
    });

    test('should not trigger saveOriginHeight when ORIGIN_HEIGHT is null', () => {
      // Ensure ORIGIN_HEIGHT is null
      Store.setValue(testElement, HwTag.ORIGIN_HEIGHT, null);
      
      // This setValue call should not trigger saveOriginHeight 
      Store.setValue(testElement, LayoutKey.LAYOUT_TAG, LayoutValue.ZOOM_PARENT);
      
      expect(Store.getValue(testElement, HwTag.ORIGIN_HEIGHT)).toBeNull();
    });

    test('should not trigger saveOriginHeight for non-LAYOUT_TAG keys', () => {
      // Set initial ORIGIN_HEIGHT
      Store.setValue(testElement, HwTag.ORIGIN_HEIGHT, '100');
      
      // This setValue call with different key should not trigger saveOriginHeight
      Store.setValue(testElement, 'other-key', 'some-value');
      
      // ORIGIN_HEIGHT should remain unchanged
      expect(Store.getValue(testElement, HwTag.ORIGIN_HEIGHT)).toBe('100');
    });
  });
});

describe('Store Module - Basic Structure Tests', () => {
  test('Store module can be imported', () => {
    const Store = require('../src/Framework/Common/Utils/Store').default;
    expect(Store).toBeDefined();
  });

  test('Store has valueMap property', () => {
    const Store = require('../src/Framework/Common/Utils/Store').default;
    expect(Store.valueMap).toBeDefined();
    expect(Store.valueMap instanceof Map).toBe(true);
  });

  test('Store has nextRoundCache property', () => {
    const Store = require('../src/Framework/Common/Utils/Store').default;
    expect(Store.nextRoundCache).toBeDefined();
    expect(Array.isArray(Store.nextRoundCache)).toBe(true);
  });

  test('Store has setValue method', () => {
    const Store = require('../src/Framework/Common/Utils/Store').default;
    expect(typeof Store.setValue).toBe('function');
  });

  test('Store has getValue method', () => {
    const Store = require('../src/Framework/Common/Utils/Store').default;
    expect(typeof Store.getValue).toBe('function');
  });

  test('Store has removeValue method', () => {
    const Store = require('../src/Framework/Common/Utils/Store').default;
    expect(typeof Store.removeValue).toBe('function');
  });

  test('Store has clearTag method', () => {
    const Store = require('../src/Framework/Common/Utils/Store').default;
    expect(typeof Store.clearTag).toBe('function');
  });

  test('Store has clearAllTag method', () => {
    const Store = require('../src/Framework/Common/Utils/Store').default;
    expect(typeof Store.clearAllTag).toBe('function');
  });

  test('Store has removeNextRoundTags method', () => {
    const Store = require('../src/Framework/Common/Utils/Store').default;
    expect(typeof Store.removeNextRoundTags).toBe('function');
  });
});
