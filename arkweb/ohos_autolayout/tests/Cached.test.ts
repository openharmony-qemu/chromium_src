/**
 * Unit tests for Cached.ts
 */

// Mock dependencies BEFORE imports
jest.mock('../src/Framework/Common/Style/Common/CacheStyleGetter', () => ({
  __esModule: true,
  default: {
    clearSpecCache: jest.fn(),
  },
}));

jest.mock('../src/Framework/Common/Utils/Store', () => ({
  __esModule: true,
  default: {
    removeNextRoundTags: jest.fn(),
  },
}));

import Cached from '../src/Framework/Common/Cached';
import CacheStyleGetter from '../src/Framework/Common/Style/Common/CacheStyleGetter';
import Store from '../src/Framework/Common/Utils/Store';

describe('Cached Module', () => {
  beforeEach(() => {
    jest.clearAllMocks();
    
    // Reset cached values
    // @ts-ignore
    Cached.bodyCache = undefined;
    // @ts-ignore
    Cached.documentCache = undefined;
  });

  describe('body_', () => {
    test('should return document.body', () => {
      const result = Cached.body_();
      
      expect(result).toBe(document.body);
    });

    test('should cache document.body on first call', () => {
      const result1 = Cached.body_();
      const result2 = Cached.body_();
      
      expect(result1).toBe(result2);
      expect(result1).toBe(document.body);
    });

    test('should return cached value without re-accessing document.body', () => {
      // First call caches the value
      const firstResult = Cached.body_();
      
      // Mock document.body to return something different
      const originalBody = document.body;
      const mockBody = document.createElement('body');
      Object.defineProperty(document, 'body', {
        writable: true,
        configurable: true,
        value: mockBody,
      });
      
      // Second call should return cached value
      const secondResult = Cached.body_();
      expect(secondResult).toBe(firstResult);
      expect(secondResult).not.toBe(mockBody);
      
      // Restore
      Object.defineProperty(document, 'body', {
        writable: true,
        configurable: true,
        value: originalBody,
      });
    });
  });

  describe('documentElement_', () => {
    test('should return document.documentElement', () => {
      const result = Cached.documentElement_();
      
      expect(result).toBe(document.documentElement);
    });

    test('should cache document.documentElement on first call', () => {
      const result1 = Cached.documentElement_();
      const result2 = Cached.documentElement_();
      
      expect(result1).toBe(result2);
      expect(result1).toBe(document.documentElement);
    });

    test('should return cached value without re-accessing document.documentElement', () => {
      // First call caches the value
      const firstResult = Cached.documentElement_();
      
      // Mock document.documentElement to return something different
      const originalDocumentElement = document.documentElement;
      const mockDocumentElement = document.createElement('html');
      Object.defineProperty(document, 'documentElement', {
        writable: true,
        configurable: true,
        value: mockDocumentElement,
      });
      
      // Second call should return cached value
      const secondResult = Cached.documentElement_();
      expect(secondResult).toBe(firstResult);
      expect(secondResult).not.toBe(mockDocumentElement);
      
      // Restore
      Object.defineProperty(document, 'documentElement', {
        writable: true,
        configurable: true,
        value: originalDocumentElement,
      });
    });
  });

  describe('clearStyleCache', () => {
    test('should call CacheStyleGetter.clearSpecCache', () => {
      Cached.clearStyleCache();
      
      expect(CacheStyleGetter.clearSpecCache).toHaveBeenCalled();
    });

    test('should be callable multiple times', () => {
      Cached.clearStyleCache();
      Cached.clearStyleCache();
      
      expect(CacheStyleGetter.clearSpecCache).toHaveBeenCalledTimes(2);
    });
  });

  describe('clearAllCache', () => {
    test('should call Store.removeNextRoundTags', () => {
      Cached.clearAllCache();
      
      expect(Store.removeNextRoundTags).toHaveBeenCalled();
    });

    test('should clear body cache', () => {
      // First cache the value
      Cached.body_();
      // @ts-ignore
      expect(Cached.bodyCache).toBeDefined();
      
      // Clear cache
      Cached.clearAllCache();
      
      // Cache should be undefined
      // @ts-ignore
      expect(Cached.bodyCache).toBeUndefined();
    });

    test('should clear documentElement cache', () => {
      // First cache the value
      Cached.documentElement_();
      // @ts-ignore
      expect(Cached.documentCache).toBeDefined();
      
      // Clear cache
      Cached.clearAllCache();
      
      // Cache should be undefined
      // @ts-ignore
      expect(Cached.documentCache).toBeUndefined();
    });

    test('should allow re-caching after clearAllCache', () => {
      // Cache values
      const firstBody = Cached.body_();
      const firstDoc = Cached.documentElement_();
      
      // Clear cache
      Cached.clearAllCache();
      
      // Re-cache values
      const secondBody = Cached.body_();
      const secondDoc = Cached.documentElement_();
      
      // Values should be the same DOM elements but re-fetched
      expect(secondBody).toBe(document.body);
      expect(secondDoc).toBe(document.documentElement);
    });

    test('caching should work for basic DOM elements', () => {
      // Cache values
      const firstBody = Cached.body_();
      const firstDoc = Cached.documentElement_();
      
      // Second calls should return cached values
      const secondBody = Cached.body_();
      const secondDoc = Cached.documentElement_();
      
      // Both should equal document elements
      expect(secondBody).toBe(document.body);
      expect(secondDoc).toBe(document.documentElement);
      expect(firstBody).toBe(secondBody);
      expect(firstDoc).toBe(secondDoc);
    });
  });

  describe('Cache behavior', () => {
    test('should maintain separate caches for body and documentElement', () => {
      const body = Cached.body_();
      const doc = Cached.documentElement_();
      
      expect(body).not.toBe(doc);
      // @ts-ignore
      expect(Cached.bodyCache).toBe(body);
      // @ts-ignore
      expect(Cached.documentCache).toBe(doc);
    });

    test('should not affect one cache when clearing the other conceptually', () => {
      Cached.body_();
      Cached.documentElement_();
      
      // There's no method to clear only one cache, but we test the isolation
      // @ts-ignore
      const bodyCache = Cached.bodyCache;
      // @ts-ignore
      const docCache = Cached.documentCache;
      
      expect(bodyCache).toBeDefined();
      expect(docCache).toBeDefined();
      expect(bodyCache).not.toBe(docCache);
    });
  });

  describe('Integration with other modules', () => {
    test('clearStyleCache method should exist', () => {
      expect(typeof Cached.clearStyleCache).toBe('function');
    });

    test('clearAllCache method should exist', () => {
      expect(typeof Cached.clearAllCache).toBe('function');
    });

    test('should have body_ and documentElement_ as separate methods', () => {
      expect(typeof Cached.body_).toBe('function');
      expect(typeof Cached.documentElement_).toBe('function');
      expect(Cached.body_).not.toBe(Cached.documentElement_);
    });
  });

  describe('Performance optimization', () => {
    test('should cache DOM elements for reuse', () => {
      // First call caches the value
      const firstBody = Cached.body_();
      const secondBody = Cached.body_();
      
      // Should return the same instance
      expect(firstBody).toBe(secondBody);
      expect(firstBody).toBe(document.body);
    });
  });

  describe('Edge cases', () => {
    test('should handle null document.body gracefully', () => {
      const originalBody = document.body;
      
      Object.defineProperty(document, 'body', {
        writable: true,
        configurable: true,
        value: null,
      });
      
      const result = Cached.body_();
      expect(result).toBeNull();
      
      // Restore
      Object.defineProperty(document, 'body', {
        writable: true,
        configurable: true,
        value: originalBody,
      });
    });

    test('should handle null document.documentElement gracefully', () => {
      const originalDocumentElement = document.documentElement;
      
      Object.defineProperty(document, 'documentElement', {
        writable: true,
        configurable: true,
        value: null,
      });
      
      const result = Cached.documentElement_();
      expect(result).toBeNull();
      
      // Restore
      Object.defineProperty(document, 'documentElement', {
        writable: true,
        configurable: true,
        value: originalDocumentElement,
      });
    });
  });
});
