/**
 * Unit tests for DetectorInst.ts
 */
import { DetectorInst, LayoutMetrics } from '../src/Framework/Common/DetectorInst';

describe('DetectorInst Class', () => {
  let detectorInst: DetectorInst;
  let testElement: HTMLElement;

  beforeEach(() => {
    detectorInst = DetectorInst.getInstance();
    testElement = document.createElement('div');
    testElement.style.width = '100px';
    testElement.style.height = '50px';
    testElement.style.position = 'absolute';
    testElement.style.top = '10px';
    testElement.style.left = '20px';
    document.body.appendChild(testElement);
  });

  afterEach(() => {
    document.body.removeChild(testElement);
    // Reset singleton instance for testing
    // @ts-ignore
    DetectorInst.instance = null;
  });

  describe('Static constants', () => {
    test('should have minGap constant', () => {
      expect(DetectorInst.minGap).toBe(-1);
    });

    test('should have minInterWH constant based on devicePixelRatio', () => {
      const expected = Number((48 / Math.max(window.devicePixelRatio, 1)).toFixed(2));
      expect(DetectorInst.minInterWH).toBe(expected);
    });

    test('should have empiricalCoefficient constant', () => {
      expect(DetectorInst.empiricalCoefficient).toBe(0.022);
    });

    test('should have maxProportion constant', () => {
      expect(DetectorInst.maxProportion).toBe(0);
    });

    test('should have minAlignment constant based on devicePixelRatio', () => {
      const expected = Number((20 / Math.max(window.devicePixelRatio, 1)).toFixed(2));
      expect(DetectorInst.minAlignment).toBe(expected);
    });

    test('should have tolerance constant based on devicePixelRatio', () => {
      const expected = Number((20 / Math.max(window.devicePixelRatio, 1)).toFixed(2));
      expect(DetectorInst.tolerance).toBe(expected);
    });

    test('should have relativeOffset constant', () => {
      expect(DetectorInst.relativeOffset).toBe(0.05);
    });

    test('should have maxOffset constant', () => {
      expect(DetectorInst.maxOffset).toBe(0.5);
    });

    test('constants should be accessible', () => {
      // TypeScript enforces readonly at compile time
      // At runtime, we verify they exist and have correct values
      expect(DetectorInst.minGap).toBeDefined();
      expect(DetectorInst.minInterWH).toBeDefined();
      expect(DetectorInst.empiricalCoefficient).toBeDefined();
    });
  });

  describe('Singleton pattern', () => {
    test('should return the same instance', () => {
      const instance1 = DetectorInst.getInstance();
      const instance2 = DetectorInst.getInstance();
      
      expect(instance1).toBe(instance2);
    });

    test('should create instance on first call', () => {
      const instance = DetectorInst.getInstance();
      expect(instance).toBeDefined();
      expect(instance).toBeInstanceOf(DetectorInst);
    });

    test('should have private constructor', () => {
      // TypeScript enforces private constructor at compile time
      // At runtime, we can only verify the singleton pattern works
      const instance1 = DetectorInst.getInstance();
      const instance2 = DetectorInst.getInstance();
      expect(instance1).toBe(instance2);
    });

    test('should maintain state across getInstance calls', () => {
      const instance1 = DetectorInst.getInstance();
      instance1.recordOriginalPosition(testElement);
      
      const instance2 = DetectorInst.getInstance();
      // Both should reference the same state
      expect(instance1).toBe(instance2);
    });
  });

  describe('getLayoutMetrics', () => {
    test('should return LayoutMetrics object', () => {
      const metrics = detectorInst.getLayoutMetrics(testElement);
      
      expect(metrics).toBeDefined();
      expect(metrics).toHaveProperty('top');
      expect(metrics).toHaveProperty('left');
      expect(metrics).toHaveProperty('width');
      expect(metrics).toHaveProperty('height');
      expect(metrics).toHaveProperty('right');
      expect(metrics).toHaveProperty('bottom');
    });

    test('should return correct dimensions', () => {
      const metrics = detectorInst.getLayoutMetrics(testElement);
      
      expect(typeof metrics.width).toBe('number');
      expect(typeof metrics.height).toBe('number');
      expect(metrics.width).toBeGreaterThanOrEqual(0);
      expect(metrics.height).toBeGreaterThanOrEqual(0);
    });

    test('should return position values', () => {
      const metrics = detectorInst.getLayoutMetrics(testElement);
      
      expect(typeof metrics.top).toBe('number');
      expect(typeof metrics.left).toBe('number');
      expect(typeof metrics.right).toBe('number');
      expect(typeof metrics.bottom).toBe('number');
    });

    test('should calculate right and bottom correctly', () => {
      const metrics = detectorInst.getLayoutMetrics(testElement);
      
      // right should be left + width
      expect(metrics.right).toBeCloseTo(metrics.left + metrics.width, 1);
      // bottom should be top + height
      expect(metrics.bottom).toBeCloseTo(metrics.top + metrics.height, 1);
    });

    test('should handle element at origin', () => {
      const originElement = document.createElement('div');
      originElement.style.position = 'absolute';
      originElement.style.top = '0px';
      originElement.style.left = '0px';
      originElement.style.width = '50px';
      originElement.style.height = '50px';
      document.body.appendChild(originElement);
      
      const metrics = detectorInst.getLayoutMetrics(originElement);
      
      expect(metrics.top).toBeGreaterThanOrEqual(0);
      expect(metrics.left).toBeGreaterThanOrEqual(0);
      
      document.body.removeChild(originElement);
    });

    test('should handle element with zero dimensions', () => {
      const zeroElement = document.createElement('div');
      document.body.appendChild(zeroElement);
      
      const metrics = detectorInst.getLayoutMetrics(zeroElement);
      
      expect(metrics).toBeDefined();
      expect(typeof metrics.width).toBe('number');
      expect(typeof metrics.height).toBe('number');
      
      document.body.removeChild(zeroElement);
    });
  });

  describe('recordOriginalPosition', () => {
    test('should record position for root element', () => {
      detectorInst.recordOriginalPosition(testElement);
      
      // @ts-ignore
      const state = detectorInst.originalLayoutState;
      expect(state).not.toBeNull();
      expect(state.metrics).toBeInstanceOf(Map);
      expect(state.metrics.has(testElement)).toBe(true);
    });

    test('should record screen size', () => {
      detectorInst.recordOriginalPosition(testElement);
      
      // @ts-ignore
      const state = detectorInst.originalLayoutState;
      expect(state.screenSize).toBeDefined();
      expect(state.screenSize.width).toBe(window.innerWidth);
      expect(state.screenSize.height).toBe(window.innerHeight);
    });

    test('should record all child elements', () => {
      const child1 = document.createElement('span');
      const child2 = document.createElement('div');
      testElement.appendChild(child1);
      testElement.appendChild(child2);
      
      detectorInst.recordOriginalPosition(testElement);
      
      // @ts-ignore
      const state = detectorInst.originalLayoutState;
      expect(state.metrics.has(child1)).toBe(true);
      expect(state.metrics.has(child2)).toBe(true);
    });

    test('should not re-record if already recorded', () => {
      detectorInst.recordOriginalPosition(testElement);
      // @ts-ignore
      const firstState = detectorInst.originalLayoutState;
      
      // Try to record again
      detectorInst.recordOriginalPosition(testElement);
      // @ts-ignore
      const secondState = detectorInst.originalLayoutState;
      
      expect(firstState).toBe(secondState);
    });

    test('should store LayoutMetrics for each element', () => {
      const child = document.createElement('div');
      testElement.appendChild(child);
      
      detectorInst.recordOriginalPosition(testElement);
      
      // @ts-ignore
      const state = detectorInst.originalLayoutState;
      const rootMetrics = state.metrics.get(testElement);
      const childMetrics = state.metrics.get(child);
      
      expect(rootMetrics).toBeDefined();
      expect(childMetrics).toBeDefined();
      expect(rootMetrics).toHaveProperty('top');
      expect(childMetrics).toHaveProperty('top');
    });

    test('should handle nested elements', () => {
      const level1 = document.createElement('div');
      const level2 = document.createElement('div');
      const level3 = document.createElement('span');
      
      testElement.appendChild(level1);
      level1.appendChild(level2);
      level2.appendChild(level3);
      
      detectorInst.recordOriginalPosition(testElement);
      
      // @ts-ignore
      const state = detectorInst.originalLayoutState;
      expect(state.metrics.has(level1)).toBe(true);
      expect(state.metrics.has(level2)).toBe(true);
      expect(state.metrics.has(level3)).toBe(true);
    });

    test('should handle element with no children', () => {
      const emptyElement = document.createElement('div');
      document.body.appendChild(emptyElement);
      
      detectorInst.recordOriginalPosition(emptyElement);
      
      // @ts-ignore
      const state = detectorInst.originalLayoutState;
      expect(state.metrics.has(emptyElement)).toBe(true);
      
      document.body.removeChild(emptyElement);
    });
  });

  describe('LayoutMetrics interface', () => {
    test('should have all required properties', () => {
      const metrics: LayoutMetrics = {
        top: 10,
        left: 20,
        width: 100,
        height: 50,
        right: 120,
        bottom: 60,
      };

      expect(metrics.top).toBe(10);
      expect(metrics.left).toBe(20);
      expect(metrics.width).toBe(100);
      expect(metrics.height).toBe(50);
      expect(metrics.right).toBe(120);
      expect(metrics.bottom).toBe(60);
    });

    test('should work with destructuring', () => {
      const metrics = detectorInst.getLayoutMetrics(testElement);
      const { top, left, width, height } = metrics;

      expect(typeof top).toBe('number');
      expect(typeof left).toBe('number');
      expect(typeof width).toBe('number');
      expect(typeof height).toBe('number');
    });
  });

  describe('Edge cases', () => {
    test('should handle multiple root elements separately', () => {
      const element2 = document.createElement('div');
      document.body.appendChild(element2);
      
      const instance1 = DetectorInst.getInstance();
      instance1.recordOriginalPosition(testElement);
      
      // Reset to test second element
      // @ts-ignore
      DetectorInst.instance = null;
      const instance2 = DetectorInst.getInstance();
      instance2.recordOriginalPosition(element2);
      
      document.body.removeChild(element2);
    });

    test('should handle elements added after recording', () => {
      detectorInst.recordOriginalPosition(testElement);
      
      const newChild = document.createElement('span');
      testElement.appendChild(newChild);
      
      // @ts-ignore
      const state = detectorInst.originalLayoutState;
      // New child should not be in the recorded state
      expect(state.metrics.has(newChild)).toBe(false);
    });

    test('should work with devicePixelRatio variations', () => {
      const originalRatio = window.devicePixelRatio;
      
      // Constants should be calculated based on current ratio
      expect(DetectorInst.minInterWH).toBe(
        Number((48 / Math.max(originalRatio, 1)).toFixed(2))
      );
      expect(DetectorInst.minAlignment).toBe(
        Number((20 / Math.max(originalRatio, 1)).toFixed(2))
      );
    });
  });

  describe('Integration', () => {
    test('should support full workflow', () => {
      // Get instance
      const instance = DetectorInst.getInstance();
      expect(instance).toBeDefined();
      
      // Get metrics
      const metrics = instance.getLayoutMetrics(testElement);
      expect(metrics).toBeDefined();
      
      // Record position
      instance.recordOriginalPosition(testElement);
      // @ts-ignore
      const state = instance.originalLayoutState;
      expect(state).not.toBeNull();
    });
  });
});
