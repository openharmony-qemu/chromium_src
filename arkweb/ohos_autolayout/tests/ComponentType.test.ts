/**
 * Unit tests for ComponentType.ts
 */
import { ComponentType } from '../src/Framework/Common/ComponentType';

describe('ComponentType Enum', () => {
  describe('Enum values', () => {
    test('should have TabBar type', () => {
      expect(ComponentType.TabBar).toBe('tab-bar');
    });

    test('should have List type', () => {
      expect(ComponentType.List).toBe('list');
    });

    test('should have ScrollList type', () => {
      expect(ComponentType.ScrollList).toBe('scroll-list');
    });

    test('should have OneRowList type', () => {
      expect(ComponentType.OneRowList).toBe('onerow-list');
    });

    test('should have WaterFall type', () => {
      expect(ComponentType.WaterFall).toBe('water-fall');
    });

    test('should have GridList type', () => {
      expect(ComponentType.GridList).toBe('grid-list');
    });

    test('should have AsideBar type', () => {
      expect(ComponentType.AsideBar).toBe('aside-bar');
    });

    test('should have Swiper type', () => {
      expect(ComponentType.Swiper).toBe('swiper');
    });

    test('should have Image type', () => {
      expect(ComponentType.Image).toBe('image');
    });

    test('should have ObjectNode type', () => {
      expect(ComponentType.ObjectNode).toBe('object');
    });

    test('should have BackgroundImage type', () => {
      expect(ComponentType.BackgroundImage).toBe('back-image');
    });

    test('should have ServiceCard type', () => {
      expect(ComponentType.ServiceCard).toBe('service-card');
    });

    test('should have Container type', () => {
      expect(ComponentType.Container).toBe('container');
    });

    test('should have FlexBox type', () => {
      expect(ComponentType.FlexBox).toBe('flexbox');
    });

    test('should have Unknown type', () => {
      expect(ComponentType.Unknown).toBe('unknown');
    });
  });

  describe('Enum completeness', () => {
    test('should have exactly 15 component types', () => {
      const typeCount = Object.keys(ComponentType).length;
      expect(typeCount).toBe(15);
    });

    test('should have all unique values', () => {
      const values = Object.values(ComponentType);
      const uniqueValues = new Set(values);
      expect(uniqueValues.size).toBe(values.length);
    });

    test('should be accessible by key', () => {
      expect(ComponentType.TabBar).toBe('tab-bar');
      expect(ComponentType.List).toBe('list');
      expect(ComponentType.Unknown).toBe('unknown');
    });
  });

  describe('Type comparison', () => {
    test('should allow strict equality comparison', () => {
      const getType = (name: string): ComponentType => {
        return name === 'tab' ? ComponentType.TabBar : ComponentType.List;
      };
      
      const type1 = getType('tab');
      const type2 = getType('list');
      
      expect(type1 === ComponentType.TabBar).toBe(true);
      expect(type2 === ComponentType.List).toBe(true);
      expect(type1 === type2).toBe(false);
    });

    test('should allow string comparison', () => {
      const typeString = 'tab-bar';
      expect(typeString === ComponentType.TabBar).toBe(true);
    });

    test('should work in switch statements', () => {
      const getTypeName = (type: ComponentType): string => {
        switch (type) {
          case ComponentType.TabBar:
            return 'Tab Bar Component';
          case ComponentType.List:
            return 'List Component';
          case ComponentType.Unknown:
            return 'Unknown Component';
          default:
            return 'Other Component';
        }
      };

      expect(getTypeName(ComponentType.TabBar)).toBe('Tab Bar Component');
      expect(getTypeName(ComponentType.List)).toBe('List Component');
      expect(getTypeName(ComponentType.Swiper)).toBe('Other Component');
    });
  });

  describe('Practical usage', () => {
    test('should work as function parameter type', () => {
      const checkComponentType = (type: ComponentType): boolean => {
        return type === ComponentType.List || type === ComponentType.ScrollList;
      };

      expect(checkComponentType(ComponentType.List)).toBe(true);
      expect(checkComponentType(ComponentType.ScrollList)).toBe(true);
      expect(checkComponentType(ComponentType.Image)).toBe(false);
    });

    test('should work in object properties', () => {
      interface Component {
        type: ComponentType;
        name: string;
      }

      const component: Component = {
        type: ComponentType.Image,
        name: 'Hero Image',
      };

      expect(component.type).toBe(ComponentType.Image);
      expect(component.type).toBe('image');
    });

    test('should work with array filtering', () => {
      const listTypes = [
        ComponentType.List,
        ComponentType.ScrollList,
        ComponentType.GridList,
        ComponentType.OneRowList,
      ];

      const isListType = (type: ComponentType): boolean => {
        return listTypes.includes(type);
      };

      expect(isListType(ComponentType.List)).toBe(true);
      expect(isListType(ComponentType.Image)).toBe(false);
    });
  });

  describe('Type safety', () => {
    test('should be type-safe at compile time', () => {
      // This test ensures TypeScript enforces the enum type
      const validType: ComponentType = ComponentType.TabBar;
      expect(validType).toBeDefined();
    });

    test('should allow iteration over enum values', () => {
      const allTypes = Object.values(ComponentType);
      
      expect(allTypes).toContain('tab-bar');
      expect(allTypes).toContain('list');
      expect(allTypes).toContain('unknown');
      expect(allTypes.length).toBe(15);
    });

    test('should allow iteration over enum keys', () => {
      const allKeys = Object.keys(ComponentType);
      
      expect(allKeys).toContain('TabBar');
      expect(allKeys).toContain('List');
      expect(allKeys).toContain('Unknown');
      expect(allKeys.length).toBe(15);
    });
  });
});
