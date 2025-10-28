/**
 * Unit tests for AComponent.ts
 */
import { AComponent } from '../src/Framework/Common/base/AComponent';

// Create a concrete implementation for testing the abstract class
class TestComponent extends AComponent {
  constructor(root: HTMLElement) {
    super(root);
  }

  intelligenceLayout(): void {
    // Test implementation
  }

  printTree(color: boolean): void {
    // Test implementation
  }

  resetStyle(): void {
    // Test implementation
  }

  // Override static method
  static recognize(root: HTMLElement): boolean {
    return root.classList.contains('test-component');
  }
}

describe('AComponent Abstract Class', () => {
  let rootElement: HTMLElement;
  let component: TestComponent;

  beforeEach(() => {
    rootElement = document.createElement('div');
    rootElement.className = 'test-component';
    document.body.appendChild(rootElement);
    component = new TestComponent(rootElement);
  });

  afterEach(() => {
    document.body.removeChild(rootElement);
  });

  describe('Constructor', () => {
    test('should initialize with root element', () => {
      expect(component).toBeDefined();
      // @ts-ignore
      expect(component.mComponent).toBe(rootElement);
    });

    test('should initialize mDirty as true', () => {
      expect(component.isDirty()).toBe(true);
    });

    test('should set mDOMSnapshot property', () => {
      // @ts-ignore
      expect(component.mDOMSnapshot).toBeUndefined(); // Not initialized in constructor
    });
  });

  describe('isDirty method', () => {
    test('should return initial dirty state as true', () => {
      expect(component.isDirty()).toBe(true);
    });

    test('should return false after setDirty(false)', () => {
      component.setDirty(false);
      expect(component.isDirty()).toBe(false);
    });

    test('should return true after setDirty(true)', () => {
      component.setDirty(false);
      component.setDirty(true);
      expect(component.isDirty()).toBe(true);
    });
  });

  describe('setDirty method', () => {
    test('should set dirty state to false', () => {
      component.setDirty(false);
      // @ts-ignore
      expect(component.mDirty).toBe(false);
    });

    test('should set dirty state to true', () => {
      component.setDirty(false);
      component.setDirty(true);
      // @ts-ignore
      expect(component.mDirty).toBe(true);
    });

    test('should allow multiple state changes', () => {
      component.setDirty(false);
      expect(component.isDirty()).toBe(false);
      
      component.setDirty(true);
      expect(component.isDirty()).toBe(true);
      
      component.setDirty(false);
      expect(component.isDirty()).toBe(false);
    });
  });

  describe('contains method', () => {
    test('should return true for root element itself', () => {
      expect(component.contains(rootElement)).toBe(true);
    });

    test('should return true for child node', () => {
      const child = document.createElement('span');
      rootElement.appendChild(child);
      
      expect(component.contains(child)).toBe(true);
    });

    test('should return true for deeply nested child', () => {
      const div1 = document.createElement('div');
      const div2 = document.createElement('div');
      const span = document.createElement('span');
      
      rootElement.appendChild(div1);
      div1.appendChild(div2);
      div2.appendChild(span);
      
      expect(component.contains(span)).toBe(true);
    });

    test('should return false for external node', () => {
      const externalNode = document.createElement('div');
      document.body.appendChild(externalNode);
      
      expect(component.contains(externalNode)).toBe(false);
      
      document.body.removeChild(externalNode);
    });

    test('should return false for text node outside component', () => {
      const textNode = document.createTextNode('test');
      document.body.appendChild(textNode);
      
      expect(component.contains(textNode)).toBe(false);
      
      document.body.removeChild(textNode);
    });

    test('should return true for text node inside component', () => {
      const textNode = document.createTextNode('test content');
      rootElement.appendChild(textNode);
      
      expect(component.contains(textNode)).toBe(true);
    });
  });

  describe('Static recognize method', () => {
    test('should throw error for base class', () => {
      expect(() => {
        AComponent.recognize(rootElement);
      }).toThrow('FATAL ERROR:Please Implement tis function in subclass');
    });

    test('should work for concrete implementation', () => {
      const testElement = document.createElement('div');
      testElement.className = 'test-component';
      
      expect(TestComponent.recognize(testElement)).toBe(true);
    });

    test('should return false for non-matching element', () => {
      const otherElement = document.createElement('div');
      otherElement.className = 'other-component';
      
      expect(TestComponent.recognize(otherElement)).toBe(false);
    });
  });

  describe('Abstract methods', () => {
    test('should have intelligenceLayout method', () => {
      expect(typeof component.intelligenceLayout).toBe('function');
      expect(() => component.intelligenceLayout()).not.toThrow();
    });

    test('should have printTree method', () => {
      expect(typeof component.printTree).toBe('function');
      expect(() => component.printTree(true)).not.toThrow();
      expect(() => component.printTree(false)).not.toThrow();
    });

    test('should have resetStyle method', () => {
      expect(typeof component.resetStyle).toBe('function');
      expect(() => component.resetStyle()).not.toThrow();
    });
  });

  describe('Protected properties', () => {
    test('should have mComponent property', () => {
      // @ts-ignore
      expect(component.mComponent).toBeDefined();
      // @ts-ignore
      expect(component.mComponent).toBe(rootElement);
    });

    test('should have mDirty property', () => {
      // @ts-ignore
      expect(component.mDirty).toBeDefined();
      // @ts-ignore
      expect(typeof component.mDirty).toBe('boolean');
    });

    test('should have root property', () => {
      // @ts-ignore
      expect(component.root).toBeDefined();
      // @ts-ignore
      expect(component.root).toBe(rootElement);
    });

    test('mComponent and root should reference the same element', () => {
      // @ts-ignore
      expect(component.mComponent).toBe(component.root);
    });
  });

  describe('Edge cases', () => {
    test('should handle component with no children', () => {
      const emptyDiv = document.createElement('div');
      const emptyComponent = new TestComponent(emptyDiv);
      
      expect(emptyComponent.contains(emptyDiv)).toBe(true);
      expect(emptyComponent.isDirty()).toBe(true);
    });

    test('should handle rapid dirty state changes', () => {
      for (let i = 0; i < 100; i++) {
        component.setDirty(i % 2 === 0);
      }
      // Last iteration is i=99, which is odd, so i % 2 === 0 is false
      expect(component.isDirty()).toBe(false);
    });

    test('should maintain state after DOM modifications', () => {
      component.setDirty(false);
      
      const child = document.createElement('div');
      rootElement.appendChild(child);
      
      expect(component.isDirty()).toBe(false);
      expect(component.contains(child)).toBe(true);
    });
  });

  describe('Inheritance', () => {
    test('should allow multiple subclass instances', () => {
      const root1 = document.createElement('div');
      const root2 = document.createElement('div');
      
      const comp1 = new TestComponent(root1);
      const comp2 = new TestComponent(root2);
      
      comp1.setDirty(false);
      comp2.setDirty(true);
      
      expect(comp1.isDirty()).toBe(false);
      expect(comp2.isDirty()).toBe(true);
    });

    test('should maintain separate state for each instance', () => {
      const anotherRoot = document.createElement('div');
      const anotherComponent = new TestComponent(anotherRoot);
      
      component.setDirty(false);
      anotherComponent.setDirty(true);
      
      expect(component.isDirty()).toBe(false);
      expect(anotherComponent.isDirty()).toBe(true);
    });
  });
});
