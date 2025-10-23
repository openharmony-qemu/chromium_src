/**
 * Unit tests for PopupType.ts
 */
import { PopupType } from '../src/Framework/Popup/PopupType';

describe('PopupType Enum', () => {
  describe('Enum values', () => {
    test('should have type A with value 1', () => {
      expect(PopupType.A).toBe(1);
    });

    test('should have type B with value 2', () => {
      expect(PopupType.B).toBe(2);
    });

    test('should have type C with value 3', () => {
      expect(PopupType.C).toBe(3);
    });

    test('should have Unknown type with value 20', () => {
      expect(PopupType.Unknown).toBe(20);
    });
  });

  describe('Enum structure', () => {
    test('should have exactly 4 popup types', () => {
      // Numeric enums create reverse mappings, so we need to filter
      const keys = Object.keys(PopupType).filter(key => isNaN(Number(key)));
      expect(keys.length).toBe(4);
    });

    test('should support reverse mapping', () => {
      expect(PopupType[1]).toBe('A');
      expect(PopupType[2]).toBe('B');
      expect(PopupType[3]).toBe('C');
      expect(PopupType[20]).toBe('Unknown');
    });

    test('should allow accessing by string key', () => {
      expect(PopupType.A).toBe(1);
      expect(PopupType.B).toBe(2);
      expect(PopupType.C).toBe(3);
      expect(PopupType.Unknown).toBe(20);
    });
  });

  describe('Type comparison', () => {
    test('should allow strict equality comparison', () => {
      const type = PopupType.A;
      expect(type === PopupType.A).toBe(true);
      expect(type === 1).toBe(true);
    });

    test('should work in switch statements', () => {
      const getTypeName = (type: PopupType): string => {
        switch (type) {
          case PopupType.A:
            return 'Type A';
          case PopupType.B:
            return 'Type B';
          case PopupType.C:
            return 'Type C';
          case PopupType.Unknown:
            return 'Unknown Type';
          default:
            return 'Invalid';
        }
      };

      expect(getTypeName(PopupType.A)).toBe('Type A');
      expect(getTypeName(PopupType.B)).toBe('Type B');
      expect(getTypeName(PopupType.C)).toBe('Type C');
      expect(getTypeName(PopupType.Unknown)).toBe('Unknown Type');
    });

    test('should support numeric comparison', () => {
      expect(PopupType.A < PopupType.B).toBe(true);
      expect(PopupType.B < PopupType.C).toBe(true);
      expect(PopupType.C < PopupType.Unknown).toBe(true);
    });
  });

  describe('Practical usage', () => {
    test('should work as function parameter', () => {
      const isKnownType = (type: PopupType): boolean => {
        return type !== PopupType.Unknown;
      };

      expect(isKnownType(PopupType.A)).toBe(true);
      expect(isKnownType(PopupType.B)).toBe(true);
      expect(isKnownType(PopupType.C)).toBe(true);
      expect(isKnownType(PopupType.Unknown)).toBe(false);
    });

    test('should work in conditional logic', () => {
      const popup = { type: PopupType.B };
      
      if (popup.type === PopupType.A) {
        fail('Should not match A');
      } else if (popup.type === PopupType.B) {
        expect(popup.type).toBe(2);
      }
    });

    test('should work with array operations', () => {
      const knownTypes = [PopupType.A, PopupType.B, PopupType.C];
      
      expect(knownTypes).toContain(PopupType.A);
      expect(knownTypes).toContain(PopupType.B);
      expect(knownTypes).toContain(PopupType.C);
      expect(knownTypes).not.toContain(PopupType.Unknown);
    });

    test('should allow range checking', () => {
      const isValidPopup = (type: number): boolean => {
        return type >= PopupType.A && type <= PopupType.C;
      };

      expect(isValidPopup(1)).toBe(true);
      expect(isValidPopup(2)).toBe(true);
      expect(isValidPopup(3)).toBe(true);
      expect(isValidPopup(20)).toBe(false);
    });
  });

  describe('Type safety', () => {
    test('should maintain type safety', () => {
      const validType: PopupType = PopupType.B;
      expect(validType).toBe(2);
    });

    test('should support bitwise operations', () => {
      const combined = PopupType.A | PopupType.B;
      expect(combined).toBe(3);
      
      const hasA = (combined & PopupType.A) === PopupType.A;
      expect(hasA).toBe(true);
    });

    test('should iterate over values', () => {
      const values = [PopupType.A, PopupType.B, PopupType.C, PopupType.Unknown];
      expect(values).toEqual([1, 2, 3, 20]);
    });
  });
});
