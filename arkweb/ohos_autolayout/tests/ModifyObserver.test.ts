import ModifyObserver from '../src/Framework/Observer/Observers/ModifyObserver';
import IntelligentLayout from '../src/Framework/IntelligentLayout';
import ObserverHandler from '../src/Framework/Observer/ObserverHandler';
import StyleCleaner from '../src/Framework/Common/Style/Setter/StyleCleaner';
import Utils from '../src/Framework/Common/Utils/Utils';
import Constant from '../src/Framework/Common/Constant';

// 1. 定义 Mock MutationObserver 的接口
interface MockMutationObserver {
  observe: jest.Mock;
  disconnect: jest.Mock;
  takeRecords: jest.Mock;
}

jest.mock('../src/Debug/Log', () => ({
  __esModule: true,
  default: {
    info: jest.fn(),
    d: jest.fn(),
    i: jest.fn(),
  },
}));

jest.mock('../src/Framework/IntelligentLayout', () => ({
  __esModule: true,
  default: {
    removePopwinCache: jest.fn(),
    markDirty: jest.fn(),
  },
}));

jest.mock('../src/Framework/Observer/ObserverHandler', () => ({
  __esModule: true,
  default: {
    postTask: jest.fn(),
  },
}));

jest.mock('../src/Framework/Common/Style/Setter/StyleCleaner', () => ({
  __esModule: true,
  default: {
    resetEle: jest.fn(),
    resetParent: jest.fn(),
  },
}));

jest.mock('../src/Framework/Common/Utils/Utils', () => ({
  __esModule: true,
  default: {
    ignoreEle: jest.fn().mockReturnValue(false),
  },
}));

jest.mock('../src/Framework/Common/Constant', () => ({
  __esModule: true,
  default: {
    none: 'none',
    discrepancy: 1,
  },
}));

// 使用 fake timers
jest.useFakeTimers();

// Mock MutationObserver globally
const mockMutationObserver = jest.fn();
global.MutationObserver = mockMutationObserver;

// Mock queueMicrotask globally - 放在这里确保在模块导入时就生效
const microtasksGlobal: (() => void)[] = [];
const originalQueueMicrotask = global.queueMicrotask;
global.queueMicrotask = jest.fn((callback: () => void) => {
  microtasksGlobal.push(callback);
});

// MutationRecord factory function to create complete objects
const createMutationRecord = (options: {
  type?: MutationRecordType;
  target?: Node;
  addedNodes?: Node[];
  removedNodes?: Node[];
  attributeName?: string | null;
  oldValue?: string | null;
  previousSibling?: Node | null;
  nextSibling?: Node | null;
  attributeNamespace?: string | null;
}): MutationRecord => {
  const nodeListFromArray = (nodes: Node[]): NodeList => {
    // Create a simple object that behaves like NodeList
    const nodeList = {
      ...nodes,
      length: nodes.length,
      item: (index: number) => nodes[index] || null,
      forEach: Array.prototype.forEach.bind(nodes),
      [Symbol.iterator]: nodes[Symbol.iterator].bind(nodes)
    };
    
    // Set the prototype to make instanceof work
    Object.setPrototypeOf(nodeList, NodeList.prototype);
    return nodeList as NodeList;
  };

  return {
    type: options.type || 'childList',
    target: options.target || document.createElement('div'),
    addedNodes: nodeListFromArray(options.addedNodes || []),
    removedNodes: nodeListFromArray(options.removedNodes || []),
    previousSibling: options.previousSibling || null,
    nextSibling: options.nextSibling || null,
    attributeName: options.attributeName || null,
    attributeNamespace: options.attributeNamespace || null,
    oldValue: options.oldValue || null
  } as MutationRecord;
};

describe('ModifyObserver', () => {
  let mutationCallback: MutationCallback;
  let microtasks: (() => void)[] = microtasksGlobal; // 引用全局的 microtasks
  let observer: MockMutationObserver;

  const runMicrotasks = (): void => {
    while (microtasks.length > 0) {
      const task = microtasks.shift();
      task?.();
    }
  };

  beforeEach(() => {
    jest.clearAllTimers();
    
    // 清空 microtasks
    microtasks.length = 0;
    
    // 手动清除各个 mock
    mockMutationObserver.mockClear();
    (IntelligentLayout.removePopwinCache as jest.Mock).mockClear();
    (IntelligentLayout.markDirty as jest.Mock).mockClear();
    (ObserverHandler.postTask as jest.Mock).mockClear();
    (StyleCleaner.resetEle as jest.Mock).mockClear();
    (StyleCleaner.resetParent as jest.Mock).mockClear();
    (Utils.ignoreEle as jest.Mock).mockClear().mockReturnValue(false);
    (global.queueMicrotask as jest.Mock).mockClear();

    // 重置 ModifyObserver 内部状态
    ModifyObserver.modifyObserver = null;
    // @ts-ignore
    ModifyObserver.pendingRecords = [];
    // @ts-ignore
    ModifyObserver.scheduledWork = false;

    // 初始化 MutationObserver mock
    observer = {
      observe: jest.fn(),
      disconnect: jest.fn(),
      takeRecords: jest.fn(),
    };
    
    mockMutationObserver.mockImplementation((callback) => {
      mutationCallback = callback;
      return observer;
    });

    // 重置 getComputedStyle mock
    Object.defineProperty(window, 'getComputedStyle', {
      value: () => ({
        animationDuration: '0s',
        transitionDuration: '0s',
      }),
      configurable: true,
      writable: true
    });
  });

  afterEach(() => {
    // 清空所有未执行的 microtasks
    microtasks.length = 0;
  });

  describe('reInit and disconnect', () => {
    it('should create and observe a new MutationObserver on reInit', () => {
      ModifyObserver.reInit();
      expect(MutationObserver).toHaveBeenCalled();
      expect(observer.observe).toHaveBeenCalledWith(document.body, {
        childList: true,
        subtree: true,
        attributes: true,
        attributeOldValue: true,
      });
    });

    it('should disconnect existing observer on reInit', () => {
      ModifyObserver.reInit();
      const oldObserver = ModifyObserver.modifyObserver;
      ModifyObserver.reInit();
      expect(oldObserver.disconnect).toHaveBeenCalled();
    });

    it('should disconnect the observer and clear pending records on disconnect', () => {
      ModifyObserver.reInit();
      const obs = ModifyObserver.modifyObserver;
      // @ts-ignore
      ModifyObserver.pendingRecords = [createMutationRecord({ type: 'childList' })];
      
      ModifyObserver.disconnect();

      expect(obs.disconnect).toHaveBeenCalled();
      expect(ModifyObserver.modifyObserver).toBeNull();
      // @ts-ignore
      expect(ModifyObserver.pendingRecords).toEqual([]);
      // @ts-ignore
      expect(ModifyObserver.scheduledWork).toBe(false);
    });
  });

  describe('onElementModify and batch processing', () => {
    it('should queue records and schedule a microtask', () => {
      ModifyObserver.reInit();
      const records = [createMutationRecord({
        type: 'childList',
        addedNodes: [],
        removedNodes: []
      })];
      
      mutationCallback(records, observer);

      // @ts-ignore
      expect(ModifyObserver.pendingRecords).toEqual(records);
      // @ts-ignore
      expect(ModifyObserver.scheduledWork).toBe(true);
      expect(microtasks.length).toBe(1);
    });

    it('should ignore hidden or meaningless nodes', () => {
      ModifyObserver.reInit();
      const node = document.createElement('div');
      node.style.display = 'none';
      const records = [createMutationRecord({ type: 'childList', addedNodes: [node] })];

      mutationCallback(records, observer);
      runMicrotasks();

      expect(ObserverHandler.postTask).not.toHaveBeenCalled();
    });
  });

  describe('cssTimeToMs', () => {
    it('should convert seconds to milliseconds', () => {
      expect(ModifyObserver.cssTimeToMs('1.5s')).toBe(1500);
      expect(ModifyObserver.cssTimeToMs('0.5s')).toBe(500);
    });

    it('should convert milliseconds to milliseconds', () => {
      expect(ModifyObserver.cssTimeToMs('200ms')).toBe(200);
    });

    it('should return 0 for invalid or zero values', () => {
      expect(ModifyObserver.cssTimeToMs('0s')).toBe(0);
      expect(ModifyObserver.cssTimeToMs('0ms')).toBe(0);
      expect(ModifyObserver.cssTimeToMs('')).toBe(0);
      expect(ModifyObserver.cssTimeToMs('invalid')).toBe(0);
    });
  });

  describe('getAnimDurations', () => {
    it('should get animation and transition durations from computed style', () => {
      const el = document.createElement('div');
      Object.defineProperty(window, 'getComputedStyle', {
        value: () => ({
          animationDuration: '0.3s',
          transitionDuration: '500ms',
        }),
      });

      const durations = ModifyObserver.getAnimDurations(el);
      expect(durations.animationDur).toBe(300);
      expect(durations.transitionDur).toBe(500);
      expect(durations.total).toBe(800);
    });
  });

  describe('Advanced Batch Processing Tests', () => {
    beforeEach(() => {
      ModifyObserver.reInit();
    });

    describe('processBatch - Empty and Basic Cases', () => {
      it('should handle empty records array', () => {
        // Trigger batch processing with empty array
        const emptyRecords: MutationRecord[] = [];
        mutationCallback(emptyRecords, observer);
        runMicrotasks();

        // Should not call any handlers
        expect(ObserverHandler.postTask).not.toHaveBeenCalled();
        expect(IntelligentLayout.markDirty).not.toHaveBeenCalled();
      });

      it('should filter non-element nodes', () => {
        // Create text node and comment node
        const textNode = document.createTextNode('test text');
        const commentNode = document.createComment('test comment');
        
        const records = [createMutationRecord({
          type: 'childList',
          addedNodes: [textNode, commentNode],
          removedNodes: []
        })];

        mutationCallback(records, observer);
        runMicrotasks();

        // Should not trigger any processing since no element nodes
        expect(ObserverHandler.postTask).not.toHaveBeenCalled();
      });
    });

    describe('processBatch - Node Addition Processing', () => {
      it('should process added element nodes', () => {
        const addedDiv = document.createElement('div');
        addedDiv.style.display = 'block'; // Ensure it's visible
        (Utils.ignoreEle as jest.Mock).mockReturnValue(false);

        const records = [createMutationRecord({
          type: 'childList',
          addedNodes: [addedDiv],
          removedNodes: []
        })];

        // Mock getComputedStyle for animation
        Object.defineProperty(window, 'getComputedStyle', {
          value: () => ({
            animationDuration: '0s',
            transitionDuration: '0s',
          }),
          configurable: true
        });

        mutationCallback(records, observer);
        runMicrotasks();
        
        // Fast-forward timers to trigger setTimeout
        jest.runAllTimers();

        expect(Utils.ignoreEle).toHaveBeenCalledWith(addedDiv);
        expect(ObserverHandler.postTask).toHaveBeenCalled();
        expect(IntelligentLayout.markDirty).toHaveBeenCalledWith(addedDiv);
      });

      it('should ignore hidden added nodes', () => {
        const hiddenDiv = document.createElement('div');
        hiddenDiv.style.display = 'none'; // Hidden element
        
        const records = [createMutationRecord({
          type: 'childList',
          addedNodes: [hiddenDiv],
          removedNodes: []
        })];

        mutationCallback(records, observer);
        runMicrotasks();
        jest.runAllTimers();

        // Should not trigger processing for hidden elements
        expect(ObserverHandler.postTask).not.toHaveBeenCalled();
        expect(IntelligentLayout.markDirty).not.toHaveBeenCalled();
      });

      it('should ignore nodes marked by Utils.ignoreEle', () => {
        const ignoredDiv = document.createElement('div');
        ignoredDiv.style.display = 'block';
        (Utils.ignoreEle as jest.Mock).mockReturnValue(true); // Mark as ignored
        
        const records = [createMutationRecord({
          type: 'childList',
          addedNodes: [ignoredDiv],
          removedNodes: []
        })];

        mutationCallback(records, observer);
        runMicrotasks();
        jest.runAllTimers();

        expect(Utils.ignoreEle).toHaveBeenCalledWith(ignoredDiv);
        expect(ObserverHandler.postTask).not.toHaveBeenCalled();
        expect(IntelligentLayout.markDirty).not.toHaveBeenCalled();
      });
    });

    describe('processBatch - Node Removal Processing', () => {
      it('should process removed element nodes', () => {
        const removedDiv = document.createElement('div');
        const parentDiv = document.createElement('div');
        
        const records = [createMutationRecord({
          type: 'childList',
          addedNodes: [],
          removedNodes: [removedDiv],
          target: parentDiv
        })];

        mutationCallback(records, observer);
        runMicrotasks();

        // Verify removal handling
        expect(IntelligentLayout.removePopwinCache).toHaveBeenCalledWith(removedDiv);
        expect(StyleCleaner.resetEle).toHaveBeenCalledWith(removedDiv, true);
        expect(StyleCleaner.resetParent).toHaveBeenCalledWith(parentDiv);
      });

      it('should process multiple removed nodes', () => {
        const removedDiv1 = document.createElement('div');
        const removedDiv2 = document.createElement('span');
        const parentDiv = document.createElement('div');
        
        const records = [createMutationRecord({
          type: 'childList',
          addedNodes: [],
          removedNodes: [removedDiv1, removedDiv2],
          target: parentDiv
        })];

        mutationCallback(records, observer);
        runMicrotasks();

        // Verify both nodes are processed
        expect(IntelligentLayout.removePopwinCache).toHaveBeenCalledTimes(2);
        expect(IntelligentLayout.removePopwinCache).toHaveBeenCalledWith(removedDiv1);
        expect(IntelligentLayout.removePopwinCache).toHaveBeenCalledWith(removedDiv2);
        expect(StyleCleaner.resetEle).toHaveBeenCalledTimes(2);
        expect(StyleCleaner.resetParent).toHaveBeenCalledTimes(2);
      });
    });

    describe('processBatch - Attribute Change Processing', () => {
      it('should process attribute changes on element nodes', () => {
        const changedDiv = document.createElement('div');
        changedDiv.style.display = 'block';
        (Utils.ignoreEle as jest.Mock).mockReturnValue(false);
        
        const records = [createMutationRecord({
          type: 'attributes',
          target: changedDiv,
          attributeName: 'class',
          oldValue: 'old-class'
        })];

        // Mock getComputedStyle for animation
        Object.defineProperty(window, 'getComputedStyle', {
          value: () => ({
            animationDuration: '0s',
            transitionDuration: '0s',
          }),
          configurable: true
        });

        mutationCallback(records, observer);
        runMicrotasks();
        jest.runAllTimers();

        expect(Utils.ignoreEle).toHaveBeenCalledWith(changedDiv);
        expect(ObserverHandler.postTask).toHaveBeenCalled();
        expect(IntelligentLayout.markDirty).toHaveBeenCalledWith(changedDiv);
      });

      it('should ignore attribute changes on hidden elements', () => {
        const hiddenDiv = document.createElement('div');
        hiddenDiv.style.display = 'none';
        
        const records = [createMutationRecord({
          type: 'attributes',
          target: hiddenDiv,
          attributeName: 'class'
        })];

        mutationCallback(records, observer);
        runMicrotasks();
        jest.runAllTimers();

        expect(ObserverHandler.postTask).not.toHaveBeenCalled();
        expect(IntelligentLayout.markDirty).not.toHaveBeenCalled();
      });
    });

    describe('processBatch - Mixed Operations', () => {
      it('should handle mixed add/remove/attribute changes', () => {
        const addedDiv = document.createElement('div');
        addedDiv.style.display = 'block';
        const removedDiv = document.createElement('div');
        const changedDiv = document.createElement('div');
        changedDiv.style.display = 'block';
        const parentDiv = document.createElement('div');
        
        (Utils.ignoreEle as jest.Mock).mockReturnValue(false);
        
        const records = [
          createMutationRecord({
            type: 'childList',
            addedNodes: [addedDiv],
            removedNodes: [removedDiv],
            target: parentDiv
          }),
          createMutationRecord({
            type: 'attributes',
            target: changedDiv,
            attributeName: 'style'
          })
        ];

        // Mock getComputedStyle for animation
        Object.defineProperty(window, 'getComputedStyle', {
          value: () => ({
            animationDuration: '0s',
            transitionDuration: '0s',
          }),
          configurable: true
        });

        mutationCallback(records, observer);
        runMicrotasks();
        jest.runAllTimers();

        // Verify removal processing
        expect(IntelligentLayout.removePopwinCache).toHaveBeenCalledWith(removedDiv);
        expect(StyleCleaner.resetEle).toHaveBeenCalledWith(removedDiv, true);
        expect(StyleCleaner.resetParent).toHaveBeenCalledWith(parentDiv);
        
        // Verify addition and attribute change processing
        expect(Utils.ignoreEle).toHaveBeenCalledWith(addedDiv);
        expect(Utils.ignoreEle).toHaveBeenCalledWith(changedDiv);
        expect(ObserverHandler.postTask).toHaveBeenCalled();
        expect(IntelligentLayout.markDirty).toHaveBeenCalled();
      });

      it('should deduplicate nodes in mixed operations', () => {
        const div = document.createElement('div');
        div.style.display = 'block';
        (Utils.ignoreEle as jest.Mock).mockReturnValue(false);
        
        // Same element appears in multiple records
        const records = [
          createMutationRecord({
            type: 'childList',
            addedNodes: [div],
            removedNodes: []
          }),
          createMutationRecord({
            type: 'attributes',
            target: div,
            attributeName: 'class'
          })
        ];

        // Mock getComputedStyle for animation
        Object.defineProperty(window, 'getComputedStyle', {
          value: () => ({
            animationDuration: '0s',
            transitionDuration: '0s',
          }),
          configurable: true
        });

        mutationCallback(records, observer);
        runMicrotasks();
        jest.runAllTimers();

        // Should only process the element once due to Set deduplication
        expect(Utils.ignoreEle).toHaveBeenCalledTimes(1);
        expect(Utils.ignoreEle).toHaveBeenCalledWith(div);
        expect(IntelligentLayout.markDirty).toHaveBeenCalledWith(div);
      });
    });
  });

  describe('Animation Duration Calculation Tests', () => {
    beforeEach(() => {
      ModifyObserver.reInit();
    });

    describe('getDurationFromElement and calculateAnimationDuration', () => {
      it('should calculate max duration from multiple elements', () => {
        const div1 = document.createElement('div');
        const div2 = document.createElement('div');
        const div3 = document.createElement('div');
        
        div1.style.display = 'block';
        div2.style.display = 'block';
        div3.style.display = 'block';
        (Utils.ignoreEle as jest.Mock).mockReturnValue(false);
        
        // Mock different animation durations for each element
        const mockGetComputedStyle = jest.fn()
          .mockReturnValueOnce({ animationDuration: '0.2s', transitionDuration: '0.1s' })
          .mockReturnValueOnce({ animationDuration: '0.5s', transitionDuration: '0.3s' })
          .mockReturnValueOnce({ animationDuration: '0.1s', transitionDuration: '0.4s' });
        
        Object.defineProperty(window, 'getComputedStyle', {
          value: mockGetComputedStyle,
          configurable: true
        });
        
        const records = [createMutationRecord({
          type: 'childList',
          addedNodes: [div1, div2, div3],
          removedNodes: []
        })];

        mutationCallback(records, observer);
        runMicrotasks();
        
        // Should use the maximum duration (0.5s from div2)
        expect(mockGetComputedStyle).toHaveBeenCalledTimes(3);
        
        // Fast-forward by the max duration
        jest.advanceTimersByTime(500);
        
        expect(ObserverHandler.postTask).toHaveBeenCalled();
        expect(IntelligentLayout.markDirty).toHaveBeenCalled();
      });

      it('should handle zero animation duration', () => {
        const div = document.createElement('div');
        div.style.display = 'block';
        (Utils.ignoreEle as jest.Mock).mockReturnValue(false);
        
        Object.defineProperty(window, 'getComputedStyle', {
          value: () => ({ animationDuration: '0s', transitionDuration: '0s' }),
          configurable: true
        });
        
        const records = [createMutationRecord({
          type: 'childList',
          addedNodes: [div],
          removedNodes: []
        })];

        mutationCallback(records, observer);
        runMicrotasks();
        
        // Should execute immediately with no delay
        jest.advanceTimersByTime(0);
        
        expect(ObserverHandler.postTask).toHaveBeenCalled();
        expect(IntelligentLayout.markDirty).toHaveBeenCalledWith(div);
      });

      it('should prefer transition over animation duration', () => {
        const div = document.createElement('div');
        div.style.display = 'block';
        (Utils.ignoreEle as jest.Mock).mockReturnValue(false);
        
        // Transition longer than animation
        Object.defineProperty(window, 'getComputedStyle', {
          value: () => ({ animationDuration: '0.2s', transitionDuration: '0.8s' }),
          configurable: true
        });
        
        const records = [createMutationRecord({
          type: 'childList',
          addedNodes: [div],
          removedNodes: []
        })];

        mutationCallback(records, observer);
        runMicrotasks();
        
        // Should not execute until transition duration passes
        jest.advanceTimersByTime(200);
        expect(ObserverHandler.postTask).not.toHaveBeenCalled();
        
        jest.advanceTimersByTime(600); // Total 800ms
        expect(ObserverHandler.postTask).toHaveBeenCalled();
      });

      it('should handle empty changed nodes set', () => {
        // Trigger with only removed nodes (no changed nodes)
        const removedDiv = document.createElement('div');
        const parentDiv = document.createElement('div');
        
        const records = [createMutationRecord({
          type: 'childList',
          addedNodes: [],
          removedNodes: [removedDiv],
          target: parentDiv
        })];

        mutationCallback(records, observer);
        runMicrotasks();
        
        // Should process removal but not schedule tasks
        expect(IntelligentLayout.removePopwinCache).toHaveBeenCalledWith(removedDiv);
        expect(StyleCleaner.resetEle).toHaveBeenCalledWith(removedDiv, true);
        expect(StyleCleaner.resetParent).toHaveBeenCalledWith(parentDiv);
        
        // No tasks should be scheduled since no valid changes
        jest.runAllTimers();
        expect(ObserverHandler.postTask).not.toHaveBeenCalled();
      });
    });
  });

  describe('Microtask Batching and Performance Tests', () => {
    beforeEach(() => {
      ModifyObserver.reInit();
    });

    describe('Batch scheduling and execution', () => {
      it('should batch multiple rapid mutations into single microtask', () => {
        const div1 = document.createElement('div');
        const div2 = document.createElement('div');
        div1.style.display = 'block';
        div2.style.display = 'block';
        (Utils.ignoreEle as jest.Mock).mockReturnValue(false);
        
        Object.defineProperty(window, 'getComputedStyle', {
          value: () => ({ animationDuration: '0s', transitionDuration: '0s' }),
          configurable: true
        });
        
        // Multiple rapid mutations
        const records1 = [createMutationRecord({
          type: 'childList',
          addedNodes: [div1],
          removedNodes: []
        })];
        
        const records2 = [createMutationRecord({
          type: 'childList',
          addedNodes: [div2],
          removedNodes: []
        })];

        // First mutation
        mutationCallback(records1, observer);
        expect(microtasks.length).toBe(1);
        
        // Second mutation before microtask runs
        mutationCallback(records2, observer);
        expect(microtasks.length).toBe(1); // Still only one microtask
        
        // Run microtasks
        runMicrotasks();
        jest.runAllTimers();
        
        // Both elements should be processed in single batch
        expect(Utils.ignoreEle).toHaveBeenCalledWith(div1);
        expect(Utils.ignoreEle).toHaveBeenCalledWith(div2);
        expect(ObserverHandler.postTask).toHaveBeenCalledTimes(1);
      });

      it('should reset scheduledWork flag after batch processing', () => {
        const div = document.createElement('div');
        div.style.display = 'block';
        (Utils.ignoreEle as jest.Mock).mockReturnValue(false);
        
        Object.defineProperty(window, 'getComputedStyle', {
          value: () => ({ animationDuration: '0s', transitionDuration: '0s' }),
          configurable: true
        });
        
        const records = [createMutationRecord({
          type: 'childList',
          addedNodes: [div],
          removedNodes: []
        })];

        // First mutation
        mutationCallback(records, observer);
        // @ts-ignore
        expect(ModifyObserver.scheduledWork).toBe(true);
        
        // Process microtask
        runMicrotasks();
        // @ts-ignore
        expect(ModifyObserver.scheduledWork).toBe(false);
        
        // Second mutation should be able to schedule again
        mutationCallback(records, observer);
        // @ts-ignore
        expect(ModifyObserver.scheduledWork).toBe(true);
      });

      it('should snapshot and clear pending records during processing', () => {
        const div = document.createElement('div');
        div.style.display = 'block';
        (Utils.ignoreEle as jest.Mock).mockReturnValue(false);
        
        const records = [createMutationRecord({
          type: 'childList',
          addedNodes: [div],
          removedNodes: []
        })];

        // Add records
        mutationCallback(records, observer);
        // @ts-ignore
        expect(ModifyObserver.pendingRecords.length).toBe(1);
        
        // Process should clear pending records
        runMicrotasks();
        // @ts-ignore
        expect(ModifyObserver.pendingRecords.length).toBe(0);
      });
    });
  });

  describe('Edge Cases and Error Handling', () => {
    beforeEach(() => {
      ModifyObserver.reInit();
    });

    describe('Robustness tests', () => {
      it('should handle null observer gracefully in disconnect', () => {
        // First disconnect normally
        ModifyObserver.disconnect();
        expect(ModifyObserver.modifyObserver).toBeNull();
        
        // Second disconnect should not throw
        expect(() => ModifyObserver.disconnect()).not.toThrow();
      });

      it('should handle records with null targets', () => {
        const records = [createMutationRecord({
          type: 'attributes',
          target: null,
          attributeName: 'class',
          addedNodes: [],
          removedNodes: []
        })];

        // Should not throw when processing null target
        expect(() => {
          mutationCallback(records, observer);
          runMicrotasks();
        }).not.toThrow();
      });

      it('should handle malformed computed style', () => {
        const div = document.createElement('div');
        div.style.display = 'block';
        (Utils.ignoreEle as jest.Mock).mockReturnValue(false);
        
        // Mock malformed getComputedStyle
        Object.defineProperty(window, 'getComputedStyle', {
          value: () => ({
            animationDuration: 'invalid',
            transitionDuration: 'also-invalid',
          }),
          configurable: true
        });
        
        const records = [createMutationRecord({
          type: 'childList',
          addedNodes: [div],
          removedNodes: []
        })];

        expect(() => {
          mutationCallback(records, observer);
          runMicrotasks();
          jest.runAllTimers();
        }).not.toThrow();
        
        // Should still execute with 0 delay
        expect(ObserverHandler.postTask).toHaveBeenCalled();
      });

      it('should handle large batch sizes efficiently', () => {
        const elements = [];
        const records: MutationRecord[] = [];
        
        // Create 100 elements
        for (let i = 0; i < 100; i++) {
          const div = document.createElement('div');
          div.style.display = 'block';
          elements.push(div);
          
          records.push(createMutationRecord({
            type: 'childList',
            addedNodes: [div],
            removedNodes: []
          }));
        }
        
        (Utils.ignoreEle as jest.Mock).mockReturnValue(false);
        Object.defineProperty(window, 'getComputedStyle', {
          value: () => ({ animationDuration: '0s', transitionDuration: '0s' }),
          configurable: true
        });

        const startTime = performance.now();
        mutationCallback(records, observer);
        runMicrotasks();
        jest.runAllTimers();
        const endTime = performance.now();
        
        // Should complete in reasonable time
        expect(endTime - startTime).toBeLessThan(100); // Less than 100ms
        expect(Utils.ignoreEle).toHaveBeenCalledTimes(100);
        expect(ObserverHandler.postTask).toHaveBeenCalled();
      });
    });
  });

  describe('Integration and Real-world Scenarios', () => {
    beforeEach(() => {
      ModifyObserver.reInit();
    });

    describe('Complete workflow tests', () => {
      it('should handle DOM manipulation lifecycle', () => {
        // Simulate real DOM operations: add element, modify attributes, remove element
        const parent = document.createElement('div');
        const child = document.createElement('span');
        child.style.display = 'block';
        (Utils.ignoreEle as jest.Mock).mockReturnValue(false);
        
        Object.defineProperty(window, 'getComputedStyle', {
          value: () => ({ animationDuration: '0.1s', transitionDuration: '0.2s' }),
          configurable: true
        });
        
        // Step 1: Add element
        const addRecord = [createMutationRecord({
          type: 'childList',
          addedNodes: [child],
          removedNodes: [],
          target: parent
        })];
        
        mutationCallback(addRecord, observer);
        runMicrotasks();
        jest.advanceTimersByTime(200); // Wait for animation
        
        expect(ObserverHandler.postTask).toHaveBeenCalledTimes(1);
        expect(IntelligentLayout.markDirty).toHaveBeenCalledWith(child);
        
        // Step 2: Modify attributes
        const attrRecord = [createMutationRecord({
          type: 'attributes',
          target: child,
          attributeName: 'class',
          oldValue: null
        })];
        
        mutationCallback(attrRecord, observer);
        runMicrotasks();
        jest.advanceTimersByTime(200);
        
        expect(ObserverHandler.postTask).toHaveBeenCalledTimes(2);
        
        // Step 3: Remove element
        const removeRecord = [createMutationRecord({
          type: 'childList',
          addedNodes: [],
          removedNodes: [child],
          target: parent
        })];
        
        mutationCallback(removeRecord, observer);
        runMicrotasks();
        
        expect(IntelligentLayout.removePopwinCache).toHaveBeenCalledWith(child);
        expect(StyleCleaner.resetEle).toHaveBeenCalledWith(child, true);
        expect(StyleCleaner.resetParent).toHaveBeenCalledWith(parent);
      });

      it('should handle complex nested DOM changes', () => {
        const grandParent = document.createElement('div');
        const parent1 = document.createElement('div');
        const parent2 = document.createElement('div');
        const child1 = document.createElement('span');
        const child2 = document.createElement('span');
        
        [parent1, parent2, child1, child2].forEach(el => {
          el.style.display = 'block';
        });
        
        (Utils.ignoreEle as jest.Mock).mockReturnValue(false);
        Object.defineProperty(window, 'getComputedStyle', {
          value: () => ({ animationDuration: '0s', transitionDuration: '0s' }),
          configurable: true
        });
        
        // Complex nested changes
        const records = [
          createMutationRecord({
            type: 'childList',
            addedNodes: [parent1],
            removedNodes: [],
            target: grandParent
          }),
          createMutationRecord({
            type: 'childList',
            addedNodes: [child1],
            removedNodes: [],
            target: parent1
          }),
          createMutationRecord({
            type: 'childList',
            addedNodes: [child2],
            removedNodes: [parent2],
            target: grandParent
          }),
          createMutationRecord({
            type: 'attributes',
            target: child1,
            attributeName: 'style'
          })
        ];
        
        mutationCallback(records, observer);
        runMicrotasks();
        jest.runAllTimers();
        
        // Should handle all operations correctly
        expect(IntelligentLayout.removePopwinCache).toHaveBeenCalledWith(parent2);
        expect(Utils.ignoreEle).toHaveBeenCalledWith(parent1);
        expect(Utils.ignoreEle).toHaveBeenCalledWith(child1);
        expect(Utils.ignoreEle).toHaveBeenCalledWith(child2);
        expect(ObserverHandler.postTask).toHaveBeenCalled();
      });
    });
  });

  describe('Private Method Tests', () => {
    beforeEach(() => {
      ModifyObserver.reInit();
    });

    describe('handleElementChange', () => {
      it('should return false for elements with display none', () => {
        const hiddenDiv = document.createElement('div');
        hiddenDiv.style.display = 'none';
        
        // @ts-ignore
        const result = ModifyObserver.handleElementChange(hiddenDiv);
        expect(result).toBe(false);
      });

      it('should return false for ignored elements', () => {
        const ignoredDiv = document.createElement('div');
        ignoredDiv.style.display = 'block';
        (Utils.ignoreEle as jest.Mock).mockReturnValue(true);
        
        // @ts-ignore
        const result = ModifyObserver.handleElementChange(ignoredDiv);
        expect(result).toBe(false);
      });

      it('should return true for valid elements', () => {
        const validDiv = document.createElement('div');
        validDiv.style.display = 'block';
        (Utils.ignoreEle as jest.Mock).mockReturnValue(false);
        
        // @ts-ignore
        const result = ModifyObserver.handleElementChange(validDiv);
        expect(result).toBe(true);
      });
    });

    describe('handleRemove', () => {
      it('should call all removal methods correctly', () => {
        const removedNode = document.createElement('div');
        const parentNode = document.createElement('div');
        
        // @ts-ignore
        ModifyObserver.handleRemove(removedNode, parentNode);
        
        expect(IntelligentLayout.removePopwinCache).toHaveBeenCalledWith(removedNode);
        expect(StyleCleaner.resetEle).toHaveBeenCalledWith(removedNode, true);
        expect(StyleCleaner.resetParent).toHaveBeenCalledWith(parentNode);
      });
    });

    describe('getDurationFromElement', () => {
      it('should return animation duration when it is higher', () => {
        const element = document.createElement('div');
        Object.defineProperty(window, 'getComputedStyle', {
          value: () => ({ animationDuration: '0.8s', transitionDuration: '0.3s' }),
          configurable: true
        });
        
        // @ts-ignore
        const duration = ModifyObserver.getDurationFromElement(element);
        expect(duration).toBe(800);
      });

      it('should return transition duration when it is higher', () => {
        const element = document.createElement('div');
        Object.defineProperty(window, 'getComputedStyle', {
          value: () => ({ animationDuration: '0.2s', transitionDuration: '0.9s' }),
          configurable: true
        });
        
        // @ts-ignore
        const duration = ModifyObserver.getDurationFromElement(element);
        expect(duration).toBe(900);
      });

      it('should return 0 when both durations are 0', () => {
        const element = document.createElement('div');
        Object.defineProperty(window, 'getComputedStyle', {
          value: () => ({ animationDuration: '0s', transitionDuration: '0s' }),
          configurable: true
        });
        
        // @ts-ignore
        const duration = ModifyObserver.getDurationFromElement(element);
        expect(duration).toBe(0);
      });
    });

    describe('calculateAnimationDuration', () => {
      it('should return 0 for empty node set', () => {
        const emptySet = new Set<HTMLElement>();
        // @ts-ignore
        const duration = ModifyObserver.calculateAnimationDuration(emptySet);
        expect(duration).toBe(0);
      });

      it('should calculate max duration correctly', () => {
        const div1 = document.createElement('div');
        const div2 = document.createElement('div');
        const nodeSet = new Set([div1, div2]);
        
        const mockGetComputedStyle = jest.fn()
          .mockReturnValueOnce({ animationDuration: '0.3s', transitionDuration: '0.1s' })
          .mockReturnValueOnce({ animationDuration: '0.1s', transitionDuration: '0.7s' });
        
        Object.defineProperty(window, 'getComputedStyle', {
          value: mockGetComputedStyle,
          configurable: true
        });
        
        // @ts-ignore
        const duration = ModifyObserver.calculateAnimationDuration(nodeSet);
        expect(duration).toBe(700); // Max of 300ms and 700ms
      });
    });
  });

  describe('Additional Edge Cases', () => {
    beforeEach(() => {
      ModifyObserver.reInit();
    });

    describe('Complex batch scenarios', () => {
      it('should handle mixed element and non-element nodes', () => {
        const element = document.createElement('div');
        element.style.display = 'block';
        const textNode = document.createTextNode('text');
        const commentNode = document.createComment('comment');
        
        const records = [createMutationRecord({
          type: 'childList',
          addedNodes: [element, textNode, commentNode],
          removedNodes: []
        })];

        (Utils.ignoreEle as jest.Mock).mockReturnValue(false);
        Object.defineProperty(window, 'getComputedStyle', {
          value: () => ({ animationDuration: '0s', transitionDuration: '0s' }),
          configurable: true
        });

        mutationCallback(records, observer);
        runMicrotasks();
        jest.runAllTimers();

        // Only element node should be processed
        expect(Utils.ignoreEle).toHaveBeenCalledTimes(1);
        expect(Utils.ignoreEle).toHaveBeenCalledWith(element);
        expect(ObserverHandler.postTask).toHaveBeenCalled();
      });

      it('should handle mutation records with characterData type', () => {
        const textNode = document.createTextNode('test');
        const records = [createMutationRecord({
          type: 'characterData',
          target: textNode,
          oldValue: 'old text'
        })];

        mutationCallback(records, observer);
        runMicrotasks();

        // Should not trigger any processing for characterData changes
        expect(ObserverHandler.postTask).not.toHaveBeenCalled();
      });

      it('should handle records with invalid node types', () => {
        const invalidNode = { nodeType: 999 }; // Invalid node type
        const records = [createMutationRecord({
          type: 'childList',
          // @ts-ignore
          addedNodes: [invalidNode],
          removedNodes: []
        })];

        expect(() => {
          mutationCallback(records, observer);
          runMicrotasks();
        }).not.toThrow();

        expect(ObserverHandler.postTask).not.toHaveBeenCalled();
      });

      it('should handle attribute changes with null attributeName', () => {
        const element = document.createElement('div');
        element.style.display = 'block';
        (Utils.ignoreEle as jest.Mock).mockReturnValue(false);

        const records = [createMutationRecord({
          type: 'attributes',
          target: element,
          attributeName: null
        })];

        Object.defineProperty(window, 'getComputedStyle', {
          value: () => ({ animationDuration: '0s', transitionDuration: '0s' }),
          configurable: true
        });

        mutationCallback(records, observer);
        runMicrotasks();
        jest.runAllTimers();

        expect(Utils.ignoreEle).toHaveBeenCalledWith(element);
        expect(ObserverHandler.postTask).toHaveBeenCalled();
      });

      it('should handle observer reInit while microtask is pending', () => {
        const element = document.createElement('div');
        element.style.display = 'block';
        (Utils.ignoreEle as jest.Mock).mockReturnValue(false);

        const records = [createMutationRecord({
          type: 'childList',
          addedNodes: [element],
          removedNodes: []
        })];

        mutationCallback(records, observer);
        // @ts-ignore
        expect(ModifyObserver.scheduledWork).toBe(true);

        // ReInit while microtask is pending
        const oldObserver = ModifyObserver.modifyObserver;
        ModifyObserver.reInit();

        expect(oldObserver.disconnect).toHaveBeenCalled();
        // After reInit, the observer should be a new instance
        expect(ModifyObserver.modifyObserver).toBeDefined();
        expect(ModifyObserver.modifyObserver).not.toBeNull();
      });

      it('should handle simultaneous add and remove of same element type', () => {
        const addedDiv = document.createElement('div');
        const removedDiv = document.createElement('div');
        const parentDiv = document.createElement('div');
        
        addedDiv.style.display = 'block';
        (Utils.ignoreEle as jest.Mock).mockReturnValue(false);

        const records = [createMutationRecord({
          type: 'childList',
          addedNodes: [addedDiv],
          removedNodes: [removedDiv],
          target: parentDiv
        })];

        Object.defineProperty(window, 'getComputedStyle', {
          value: () => ({ animationDuration: '0s', transitionDuration: '0s' }),
          configurable: true
        });

        mutationCallback(records, observer);
        runMicrotasks();
        jest.runAllTimers();

        // Both operations should be handled
        expect(IntelligentLayout.removePopwinCache).toHaveBeenCalledWith(removedDiv);
        expect(StyleCleaner.resetEle).toHaveBeenCalledWith(removedDiv, true);
        expect(StyleCleaner.resetParent).toHaveBeenCalledWith(parentDiv);
        expect(Utils.ignoreEle).toHaveBeenCalledWith(addedDiv);
        expect(ObserverHandler.postTask).toHaveBeenCalled();
      });
    });
  });
});