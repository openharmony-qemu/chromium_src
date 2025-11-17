/**
 * Unit tests for StyleCommon.ts
 */
import StyleCommon from '../src/Framework/Common/Style/Common/StyleCommon';

describe('StyleCommon', () => {
  beforeEach(() => {
    // Reset all static properties before each test
    StyleCommon.styleCache.clear();
    StyleCommon.styleFlushed.clear();
    StyleCommon.uniqueVirIdx = 0;
    StyleCommon.indexMappingList = [];
    StyleCommon.styleDiv = null;
  });

  describe('Static constants', () => {
    test('should have correct INDEX_IDX value', () => {
      expect(StyleCommon.INDEX_IDX).toBe(0);
    });

    test('should have correct STYLE_MAP_IDX value', () => {
      expect(StyleCommon.STYLE_MAP_IDX).toBe(1);
    });

    test('INDEX_IDX and STYLE_MAP_IDX should be different', () => {
      expect(StyleCommon.INDEX_IDX).not.toBe(StyleCommon.STYLE_MAP_IDX);
    });
  });

  describe('styleCache Map', () => {
    test('should be initialized as empty Map', () => {
      expect(StyleCommon.styleCache).toBeInstanceOf(Map);
      expect(StyleCommon.styleCache.size).toBe(0);
    });

    test('should allow setting element-value pairs', () => {
      const element = document.createElement('div');
      const styleMap = new Map<string, string>([
        ['color', 'red'],
        ['fontSize', '16px']
      ]);

      StyleCommon.styleCache.set(element, styleMap);

      expect(StyleCommon.styleCache.has(element)).toBe(true);
      expect(StyleCommon.styleCache.get(element)).toBe(styleMap);
    });

    test('should handle multiple elements', () => {
      const element1 = document.createElement('div');
      const element2 = document.createElement('span');
      const styleMap1 = new Map<string, string>([['color', 'red']]);
      const styleMap2 = new Map<string, string>([['color', 'blue']]);

      StyleCommon.styleCache.set(element1, styleMap1);
      StyleCommon.styleCache.set(element2, styleMap2);

      expect(StyleCommon.styleCache.size).toBe(2);
      expect(StyleCommon.styleCache.get(element1)).toBe(styleMap1);
      expect(StyleCommon.styleCache.get(element2)).toBe(styleMap2);
    });

    test('should support delete operation', () => {
      const element = document.createElement('div');
      const styleMap = new Map<string, string>([['color', 'red']]);

      StyleCommon.styleCache.set(element, styleMap);
      expect(StyleCommon.styleCache.size).toBe(1);

      StyleCommon.styleCache.delete(element);
      expect(StyleCommon.styleCache.size).toBe(0);
      expect(StyleCommon.styleCache.has(element)).toBe(false);
    });

    test('should support clear operation', () => {
      const element1 = document.createElement('div');
      const element2 = document.createElement('span');

      StyleCommon.styleCache.set(element1, new Map());
      StyleCommon.styleCache.set(element2, new Map());
      expect(StyleCommon.styleCache.size).toBe(2);

      StyleCommon.styleCache.clear();
      expect(StyleCommon.styleCache.size).toBe(0);
    });
  });

  describe('styleFlushed Map', () => {
    test('should be initialized as empty Map', () => {
      expect(StyleCommon.styleFlushed).toBeInstanceOf(Map);
      expect(StyleCommon.styleFlushed.size).toBe(0);
    });

    test('should store element with index and style map tuple', () => {
      const element = document.createElement('div');
      const index = 42;
      const styleMap = new Map<string, string>([
        ['width', '100px'],
        ['height', '50px']
      ]);
      const tuple: [number, Map<string, string>] = [index, styleMap];

      StyleCommon.styleFlushed.set(element, tuple);

      expect(StyleCommon.styleFlushed.has(element)).toBe(true);
      const stored = StyleCommon.styleFlushed.get(element);
      expect(stored).toBeDefined();
      expect(stored![StyleCommon.INDEX_IDX]).toBe(index);
      expect(stored![StyleCommon.STYLE_MAP_IDX]).toBe(styleMap);
    });

    test('should access tuple values by constants', () => {
      const element = document.createElement('div');
      const index = 10;
      const styleMap = new Map<string, string>([['display', 'block']]);
      const tuple: [number, Map<string, string>] = [index, styleMap];

      StyleCommon.styleFlushed.set(element, tuple);

      const stored = StyleCommon.styleFlushed.get(element);
      expect(stored![StyleCommon.INDEX_IDX]).toBe(10);
      expect(stored![StyleCommon.STYLE_MAP_IDX]).toBe(styleMap);
      expect(stored![0]).toBe(10);
      expect(stored![1]).toBe(styleMap);
    });

    test('should handle multiple elements with different indices', () => {
      const element1 = document.createElement('div');
      const element2 = document.createElement('span');
      const tuple1: [number, Map<string, string>] = [1, new Map([['color', 'red']])];
      const tuple2: [number, Map<string, string>] = [2, new Map([['color', 'blue']])];

      StyleCommon.styleFlushed.set(element1, tuple1);
      StyleCommon.styleFlushed.set(element2, tuple2);

      expect(StyleCommon.styleFlushed.size).toBe(2);
      expect(StyleCommon.styleFlushed.get(element1)![StyleCommon.INDEX_IDX]).toBe(1);
      expect(StyleCommon.styleFlushed.get(element2)![StyleCommon.INDEX_IDX]).toBe(2);
    });

    test('should update existing element entry', () => {
      const element = document.createElement('div');
      const tuple1: [number, Map<string, string>] = [5, new Map([['color', 'red']])];
      const tuple2: [number, Map<string, string>] = [10, new Map([['color', 'blue']])];

      StyleCommon.styleFlushed.set(element, tuple1);
      expect(StyleCommon.styleFlushed.get(element)![StyleCommon.INDEX_IDX]).toBe(5);

      StyleCommon.styleFlushed.set(element, tuple2);
      expect(StyleCommon.styleFlushed.get(element)![StyleCommon.INDEX_IDX]).toBe(10);
      expect(StyleCommon.styleFlushed.size).toBe(1);
    });
  });

  describe('uniqueVirIdx', () => {
    test('should be initialized to 0', () => {
      expect(StyleCommon.uniqueVirIdx).toBe(0);
    });

    test('should be incrementable', () => {
      expect(StyleCommon.uniqueVirIdx).toBe(0);
      StyleCommon.uniqueVirIdx++;
      expect(StyleCommon.uniqueVirIdx).toBe(1);
      StyleCommon.uniqueVirIdx++;
      expect(StyleCommon.uniqueVirIdx).toBe(2);
    });

    test('should maintain uniqueness when incremented', () => {
      const values = new Set<number>();
      for (let i = 0; i < 10; i++) {
        values.add(StyleCommon.uniqueVirIdx);
        StyleCommon.uniqueVirIdx++;
      }
      expect(values.size).toBe(10);
    });

    test('should be assignable', () => {
      StyleCommon.uniqueVirIdx = 100;
      expect(StyleCommon.uniqueVirIdx).toBe(100);
    });
  });

  describe('indexMappingList', () => {
    test('should be initialized as empty array', () => {
      expect(Array.isArray(StyleCommon.indexMappingList)).toBe(true);
      expect(StyleCommon.indexMappingList.length).toBe(0);
    });

    test('should store index mappings', () => {
      StyleCommon.indexMappingList.push(10);
      StyleCommon.indexMappingList.push(20);
      StyleCommon.indexMappingList.push(30);

      expect(StyleCommon.indexMappingList.length).toBe(3);
      expect(StyleCommon.indexMappingList[0]).toBe(10);
      expect(StyleCommon.indexMappingList[1]).toBe(20);
      expect(StyleCommon.indexMappingList[2]).toBe(30);
    });

    test('should support array operations', () => {
      StyleCommon.indexMappingList = [1, 2, 3, 4, 5];
      
      expect(StyleCommon.indexMappingList.length).toBe(5);
      
      const doubled = StyleCommon.indexMappingList.map(x => x * 2);
      expect(doubled).toEqual([2, 4, 6, 8, 10]);
    });

    test('should allow index-based access', () => {
      StyleCommon.indexMappingList = [100, 200, 300];
      
      expect(StyleCommon.indexMappingList[0]).toBe(100);
      expect(StyleCommon.indexMappingList[2]).toBe(300);
    });

    test('should allow modification by index', () => {
      StyleCommon.indexMappingList = [1, 2, 3];
      StyleCommon.indexMappingList[1] = 999;
      
      expect(StyleCommon.indexMappingList).toEqual([1, 999, 3]);
    });
  });

  describe('styleDiv', () => {
    test('should be initialized to null', () => {
      expect(StyleCommon.styleDiv).toBeNull();
    });

    test('should accept HTMLStyleElement', () => {
      const styleElement = document.createElement('style');
      styleElement.textContent = '.test { color: red; }';
      
      StyleCommon.styleDiv = styleElement;
      
      expect(StyleCommon.styleDiv).toBe(styleElement);
      expect(StyleCommon.styleDiv).toBeInstanceOf(HTMLStyleElement);
    });

    test('should store style content', () => {
      const styleElement = document.createElement('style');
      const cssContent = '.my-class { background-color: blue; }';
      styleElement.textContent = cssContent;
      
      StyleCommon.styleDiv = styleElement;
      
      expect(StyleCommon.styleDiv.textContent).toBe(cssContent);
    });

    test('should be reassignable', () => {
      const style1 = document.createElement('style');
      const style2 = document.createElement('style');
      
      StyleCommon.styleDiv = style1;
      expect(StyleCommon.styleDiv).toBe(style1);
      
      StyleCommon.styleDiv = style2;
      expect(StyleCommon.styleDiv).toBe(style2);
    });

    test('should support null assignment', () => {
      const styleElement = document.createElement('style');
      StyleCommon.styleDiv = styleElement;
      expect(StyleCommon.styleDiv).not.toBeNull();
      
      StyleCommon.styleDiv = null;
      expect(StyleCommon.styleDiv).toBeNull();
    });
  });

  describe('Integration scenarios', () => {
    test('should handle complete cache flow', () => {
      const element = document.createElement('div');
      const styleMap = new Map<string, string>([
        ['color', 'red'],
        ['fontSize', '14px']
      ]);

      // Cache style
      StyleCommon.styleCache.set(element, styleMap);
      expect(StyleCommon.styleCache.has(element)).toBe(true);

      // Flush with index
      const index = StyleCommon.uniqueVirIdx++;
      StyleCommon.styleFlushed.set(element, [index, styleMap]);
      expect(StyleCommon.styleFlushed.get(element)![StyleCommon.INDEX_IDX]).toBe(0);

      // Add to mapping list
      StyleCommon.indexMappingList.push(index);
      expect(StyleCommon.indexMappingList[0]).toBe(0);
    });

    test('should handle multiple elements with unique indices', () => {
      const elements = [
        document.createElement('div'),
        document.createElement('span'),
        document.createElement('p')
      ];

      elements.forEach(element => {
        const styleMap = new Map<string, string>([['color', 'red']]);
        const index = StyleCommon.uniqueVirIdx++;
        
        StyleCommon.styleCache.set(element, styleMap);
        StyleCommon.styleFlushed.set(element, [index, styleMap]);
        StyleCommon.indexMappingList.push(index);
      });

      expect(StyleCommon.styleCache.size).toBe(3);
      expect(StyleCommon.styleFlushed.size).toBe(3);
      expect(StyleCommon.indexMappingList.length).toBe(3);
      expect(StyleCommon.uniqueVirIdx).toBe(3);
      expect(StyleCommon.indexMappingList).toEqual([0, 1, 2]);
    });

    test('should handle style element creation and assignment', () => {
      const styleElement = document.createElement('style');
      document.head.appendChild(styleElement);
      
      StyleCommon.styleDiv = styleElement;
      
      expect(StyleCommon.styleDiv).toBe(styleElement);
      expect(document.head.contains(styleElement)).toBe(true);
      
      // Cleanup
      document.head.removeChild(styleElement);
    });

    test('should support clearing all state', () => {
      // Setup some state
      StyleCommon.styleCache.set(document.createElement('div'), new Map());
      StyleCommon.styleFlushed.set(document.createElement('span'), [1, new Map()]);
      StyleCommon.uniqueVirIdx = 10;
      StyleCommon.indexMappingList = [1, 2, 3];
      StyleCommon.styleDiv = document.createElement('style');

      // Clear all
      StyleCommon.styleCache.clear();
      StyleCommon.styleFlushed.clear();
      StyleCommon.uniqueVirIdx = 0;
      StyleCommon.indexMappingList = [];
      StyleCommon.styleDiv = null;

      // Verify
      expect(StyleCommon.styleCache.size).toBe(0);
      expect(StyleCommon.styleFlushed.size).toBe(0);
      expect(StyleCommon.uniqueVirIdx).toBe(0);
      expect(StyleCommon.indexMappingList.length).toBe(0);
      expect(StyleCommon.styleDiv).toBeNull();
    });
  });
});
