/**
 * Unit tests for PopupDecisionTreeType.ts
 */
import { PopupDecisionTreeType } from '../src/Framework/Popup/PopupDecisionTreeType';

describe('PopupDecisionTreeType Enum', () => {
  describe('Enum values', () => {
    test('should have Center type with value 1', () => {
      expect(PopupDecisionTreeType.Center).toBe(1);
    });

    test('should have Center_Button_Overlap type with value 2', () => {
      expect(PopupDecisionTreeType.Center_Button_Overlap).toBe(2);
    });

    test('should have Bottom type with value 3', () => {
      expect(PopupDecisionTreeType.Bottom).toBe(3);
    });
  });

  describe('Enum structure', () => {
    test('should have exactly 3 decision tree types', () => {
      const keys = Object.keys(PopupDecisionTreeType).filter(key => isNaN(Number(key)));
      expect(keys.length).toBe(3);
    });

    test('should support reverse mapping', () => {
      expect(PopupDecisionTreeType[1]).toBe('Center');
      expect(PopupDecisionTreeType[2]).toBe('Center_Button_Overlap');
      expect(PopupDecisionTreeType[3]).toBe('Bottom');
    });

    test('should allow accessing by string key', () => {
      // @ts-ignore
      expect(PopupDecisionTreeType.Center).toBe(1);
      expect(PopupDecisionTreeType.Center_Button_Overlap).toBe(2);
      expect(PopupDecisionTreeType.Bottom).toBe(3);
    });

    test('should have sequential numeric values', () => {
      expect(PopupDecisionTreeType.Center).toBe(1);
      expect(PopupDecisionTreeType.Center_Button_Overlap).toBe(2);
      expect(PopupDecisionTreeType.Bottom).toBe(3);
    });
  });

  describe('Type comparison', () => {
    test('should allow strict equality comparison', () => {
      const type = PopupDecisionTreeType.Center;
      expect(type === PopupDecisionTreeType.Center).toBe(true);
      expect(type === 1).toBe(true);
    });

    test('should work in switch statements', () => {
      const getTypeName = (type: PopupDecisionTreeType): string => {
        switch (type) {
          case PopupDecisionTreeType.Center:
            return 'Center Popup';
          case PopupDecisionTreeType.Center_Button_Overlap:
            return 'Center with Button Overlap';
          case PopupDecisionTreeType.Bottom:
            return 'Bottom Popup';
          default:
            return 'Unknown';
        }
      };

      expect(getTypeName(PopupDecisionTreeType.Center)).toBe('Center Popup');
      expect(getTypeName(PopupDecisionTreeType.Center_Button_Overlap)).toBe('Center with Button Overlap');
      expect(getTypeName(PopupDecisionTreeType.Bottom)).toBe('Bottom Popup');
    });

    test('should support numeric comparison', () => {
      expect(PopupDecisionTreeType.Center < PopupDecisionTreeType.Center_Button_Overlap).toBe(true);
      expect(PopupDecisionTreeType.Center_Button_Overlap < PopupDecisionTreeType.Bottom).toBe(true);
    });
  });

  describe('Practical usage', () => {
    test('should work as function parameter', () => {
      const isCenterType = (type: PopupDecisionTreeType): boolean => {
        return type === PopupDecisionTreeType.Center || 
               type === PopupDecisionTreeType.Center_Button_Overlap;
      };

      expect(isCenterType(PopupDecisionTreeType.Center)).toBe(true);
      expect(isCenterType(PopupDecisionTreeType.Center_Button_Overlap)).toBe(true);
      expect(isCenterType(PopupDecisionTreeType.Bottom)).toBe(false);
    });

    test('should work in object properties', () => {
      interface PopupConfig {
        type: PopupDecisionTreeType;
        title: string;
      }

      const config: PopupConfig = {
        type: PopupDecisionTreeType.Center,
        title: 'Test Popup',
      };

      expect(config.type).toBe(PopupDecisionTreeType.Center);
      expect(config.type).toBe(1);
    });

    test('should work with array operations', () => {
      const centerTypes = [
        PopupDecisionTreeType.Center,
        PopupDecisionTreeType.Center_Button_Overlap,
      ];

      expect(centerTypes).toContain(PopupDecisionTreeType.Center);
      expect(centerTypes).toContain(PopupDecisionTreeType.Center_Button_Overlap);
      expect(centerTypes).not.toContain(PopupDecisionTreeType.Bottom);
    });

    test('should allow conditional logic', () => {
      const getPosition = (type: PopupDecisionTreeType): string => {
        if (type === PopupDecisionTreeType.Bottom) {
          return 'bottom';
        }
        return 'center';
      };

      expect(getPosition(PopupDecisionTreeType.Center)).toBe('center');
      expect(getPosition(PopupDecisionTreeType.Center_Button_Overlap)).toBe('center');
      expect(getPosition(PopupDecisionTreeType.Bottom)).toBe('bottom');
    });
  });

  describe('Type safety', () => {
    test('should maintain type safety', () => {
      const validType: PopupDecisionTreeType = PopupDecisionTreeType.Center;
      expect(validType).toBe(1);
    });

    test('should iterate over values', () => {
      const allTypes = [
        PopupDecisionTreeType.Center,
        PopupDecisionTreeType.Center_Button_Overlap,
        PopupDecisionTreeType.Bottom,
      ];
      expect(allTypes).toEqual([1, 2, 3]);
    });

    test('should support bitwise operations', () => {
      const combined = PopupDecisionTreeType.Center | PopupDecisionTreeType.Bottom;
      expect(combined).toBe(3); // 1 | 3 = 3
      
      const hasCenter = (combined & PopupDecisionTreeType.Center) === PopupDecisionTreeType.Center;
      expect(hasCenter).toBe(true);
    });
  });

  describe('Edge cases', () => {
    test('should handle all enum keys', () => {
      const keys = Object.keys(PopupDecisionTreeType);
      expect(keys).toContain('Center');
      expect(keys).toContain('Center_Button_Overlap');
      expect(keys).toContain('Bottom');
      expect(keys).toContain('1');
      expect(keys).toContain('2');
      expect(keys).toContain('3');
    });

    test('should handle all enum values', () => {
      const values = Object.values(PopupDecisionTreeType);
      expect(values).toContain('Center');
      expect(values).toContain('Center_Button_Overlap');
      expect(values).toContain('Bottom');
      expect(values).toContain(1);
      expect(values).toContain(2);
      expect(values).toContain(3);
    });
  });
});
