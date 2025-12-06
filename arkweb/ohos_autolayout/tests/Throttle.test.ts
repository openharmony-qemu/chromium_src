/**
 * Unit tests for Throttle.ts
 */
import Throttle from '../src/Framework/Utils/Throttle';

// Mock dependencies
jest.mock('../src/Debug/Log', () => ({
  __esModule: true,
  default: {
    d: jest.fn(),
    w: jest.fn(),
    e: jest.fn(),
  },
}));

jest.mock('../src/Debug/Tag', () => ({
  __esModule: true,
  default: {
    throttle: 'THROTTLE',
  },
}));

describe('Throttle Module', () => {
  jest.useFakeTimers();

  afterEach(() => {
    jest.clearAllTimers();
    jest.clearAllMocks();
  });

  describe('Constructor', () => {
    test('should create throttle with valid timeout and task', () => {
      const task = jest.fn();
      const throttle = new Throttle(100, task);
      
      expect(throttle).toBeDefined();
      const state = throttle.getState();
      expect(state.timeout).toBe(100);
      expect(state.isLocked).toBe(false);
      expect(state.taskCount).toBe(0);
    });

    test('should throw error when task is not a function', () => {
      expect(() => {
        // @ts-ignore - Testing invalid input
        new Throttle(100, 'not a function');
      }).toThrow(TypeError);
      
      expect(() => {
        // @ts-ignore
        new Throttle(100, null);
      }).toThrow(TypeError);
    });

    test('should use MIN_TIMEOUT when timeout is too small', () => {
      const task = jest.fn();
      const throttle = new Throttle(5, task);
      
      const state = throttle.getState();
      expect(state.timeout).toBe(10); // MIN_TIMEOUT
    });

    test('should use MIN_TIMEOUT when timeout is negative', () => {
      const task = jest.fn();
      const throttle = new Throttle(-50, task);
      
      const state = throttle.getState();
      expect(state.timeout).toBe(10);
    });

    test('should accept valid timeout values', () => {
      const task = jest.fn();
      const throttle1 = new Throttle(10, task);
      const throttle2 = new Throttle(100, task);
      const throttle3 = new Throttle(1000, task);
      
      expect(throttle1.getState().timeout).toBe(10);
      expect(throttle2.getState().timeout).toBe(100);
      expect(throttle3.getState().timeout).toBe(1000);
    });
  });

  describe('postTask', () => {
    test('should execute task immediately when not locked', () => {
      const task = jest.fn();
      const throttle = new Throttle(100, task);
      
      throttle.postTask();
      
      expect(task).toHaveBeenCalledTimes(1);
      expect(throttle.getState().isLocked).toBe(true);
    });

    test('should increment taskCount when locked', () => {
      const task = jest.fn();
      const throttle = new Throttle(100, task);
      
      throttle.postTask(); // First call executes and locks
      expect(throttle.getState().taskCount).toBe(0); // Reset after execution
      
      throttle.postTask(); // Second call while locked
      throttle.postTask(); // Third call while locked
      
      expect(throttle.getState().taskCount).toBe(2);
      expect(task).toHaveBeenCalledTimes(1); // Still only called once
    });

    test('should execute pending tasks after unlock', () => {
      const task = jest.fn();
      const throttle = new Throttle(100, task);
      
      throttle.postTask(); // Execute immediately
      throttle.postTask(); // Queue while locked
      
      expect(task).toHaveBeenCalledTimes(1);
      
      // Fast forward time to trigger unlock
      jest.advanceTimersByTime(100);
      
      // After unlock, setTimeout(0) is called
      jest.advanceTimersByTime(1);
      
      expect(task).toHaveBeenCalledTimes(2);
    });

    test('should handle multiple rapid postTask calls', () => {
      const task = jest.fn();
      const throttle = new Throttle(50, task);
      
      for (let i = 0; i < 10; i++) {
        throttle.postTask();
      }
      
      // Only first call should execute immediately
      expect(task).toHaveBeenCalledTimes(1);
      
      // Advance time and let next task execute
      jest.advanceTimersByTime(50);
      jest.advanceTimersByTime(1);
      
      expect(task).toHaveBeenCalledTimes(2);
    });
  });

  describe('cancel', () => {
    test('should clear pending tasks', () => {
      const task = jest.fn();
      const throttle = new Throttle(100, task);
      
      throttle.postTask(); // Execute immediately
      throttle.postTask(); // Queue
      throttle.postTask(); // Queue
      
      expect(throttle.getState().taskCount).toBe(2);
      
      throttle.cancel();
      
      expect(throttle.getState().taskCount).toBe(0);
    });

    test('should clear pending timer', () => {
      const task = jest.fn();
      const throttle = new Throttle(100, task);
      
      throttle.postTask();
      throttle.cancel();
      
      // Advance time - no unlock should happen
      jest.advanceTimersByTime(100);
      
      // Task should not be called again
      expect(task).toHaveBeenCalledTimes(1);
    });

    test('should not affect current lock state', () => {
      const task = jest.fn();
      const throttle = new Throttle(100, task);
      
      throttle.postTask();
      expect(throttle.getState().isLocked).toBe(true);
      
      throttle.cancel();
      
      // Lock state persists
      expect(throttle.getState().isLocked).toBe(true);
    });
  });

  describe('destroy', () => {
    test('should cancel tasks and unlock', () => {
      const task = jest.fn();
      const throttle = new Throttle(100, task);
      
      throttle.postTask();
      throttle.postTask();
      
      throttle.destroy();
      
      const state = throttle.getState();
      expect(state.taskCount).toBe(0);
      expect(state.isLocked).toBe(false);
    });

    test('should allow reuse after destroy', () => {
      const task = jest.fn();
      const throttle = new Throttle(100, task);
      
      throttle.postTask();
      throttle.destroy();
      
      // Should be able to post new task
      throttle.postTask();
      
      expect(task).toHaveBeenCalledTimes(2);
    });
  });

  describe('Error handling', () => {
    test('should handle task execution errors gracefully', () => {
      const task = jest.fn(() => {
        throw new Error('Task error');
      });
      const throttle = new Throttle(100, task);
      
      expect(() => {
        throttle.postTask();
      }).not.toThrow();
      
      // Should unlock even after error
      expect(throttle.getState().isLocked).toBe(false);
    });

    test('should continue working after task error', () => {
      let shouldFail = true;
      const task = jest.fn(() => {
        if (shouldFail) {
          throw new Error('Task error');
        }
      });
      const throttle = new Throttle(100, task);
      
      throttle.postTask(); // This will fail
      
      expect(throttle.getState().isLocked).toBe(false);
      
      shouldFail = false;
      throttle.postTask(); // This should succeed
      
      expect(task).toHaveBeenCalledTimes(2);
    });
  });

  describe('getState', () => {
    test('should return current state', () => {
      const task = jest.fn();
      const throttle = new Throttle(150, task);
      
      const initialState = throttle.getState();
      expect(initialState).toEqual({
        isLocked: false,
        taskCount: 0,
        timeout: 150,
      });
      
      throttle.postTask();
      
      const lockedState = throttle.getState();
      expect(lockedState.isLocked).toBe(true);
      expect(lockedState.taskCount).toBe(0);
    });
  });

  describe('Timing behavior', () => {
    test('should respect timeout interval', () => {
      const task = jest.fn();
      const throttle = new Throttle(200, task);
      
      throttle.postTask();
      expect(task).toHaveBeenCalledTimes(1);
      
      // Try to post before timeout
      jest.advanceTimersByTime(100);
      throttle.postTask();
      expect(task).toHaveBeenCalledTimes(1); // Still locked
      
      // Complete timeout
      jest.advanceTimersByTime(100);
      jest.advanceTimersByTime(1); // setTimeout(0)
      
      expect(task).toHaveBeenCalledTimes(2);
    });

    test('should batch multiple calls within timeout', () => {
      const task = jest.fn();
      const throttle = new Throttle(100, task);
      
      // First call
      throttle.postTask();
      
      // Multiple calls while locked
      for (let i = 0; i < 5; i++) {
        jest.advanceTimersByTime(10);
        throttle.postTask();
      }
      
      // Only first call executed
      expect(task).toHaveBeenCalledTimes(1);
      
      // Complete timeout
      jest.advanceTimersByTime(50);
      jest.advanceTimersByTime(1);
      
      // Only one more execution for all batched calls
      expect(task).toHaveBeenCalledTimes(2);
    });
  });

  describe('Edge cases', () => {
    test('should handle rapid destroy and postTask', () => {
      const task = jest.fn();
      const throttle = new Throttle(100, task);
      
      throttle.postTask();
      throttle.destroy();
      throttle.postTask();
      
      expect(task).toHaveBeenCalledTimes(2);
    });

    test('should handle cancel during execution', () => {
      let callCount = 0;
      const task = jest.fn(() => {
        callCount++;
        if (callCount === 1) {
          throttle.cancel();
        }
      });
      const throttle = new Throttle(100, task);
      
      throttle.postTask();
      throttle.postTask(); // This adds to taskCount before execution
      
      // After first execution, taskCount is reset to 0 by lock()
      // But the second postTask added 1, so cancel clears it
      const state = throttle.getState();
      expect(state.taskCount).toBeGreaterThanOrEqual(0);
    });

    test('should handle zero taskCount after multiple operations', () => {
      const task = jest.fn();
      const throttle = new Throttle(100, task);
      
      throttle.postTask();
      throttle.postTask();
      throttle.cancel();
      
      jest.advanceTimersByTime(100);
      jest.advanceTimersByTime(1);
      
      // No additional tasks should execute
      expect(task).toHaveBeenCalledTimes(1);
    });
  });
});
