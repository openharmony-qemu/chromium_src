/**
 * Unit tests for Tag.ts
 */
import Tag from '../src/Debug/Tag';

describe('Tag Module', () => {
  describe('Framework related tags', () => {
    test('should have framework tag', () => {
      expect(Tag.framework).toBe('Framework');
    });

    test('should have intelligentLayout tag', () => {
      expect(Tag.intelligentLayout).toBe('IntelligentLayout');
    });

    test('should have observerHandler tag', () => {
      expect(Tag.observerHandler).toBe('ObserverHandler');
    });

    test('should have resizeObserver tag', () => {
      expect(Tag.resizeObserver).toBe('ResizeObserver');
    });

    test('should have modifyObserver tag', () => {
      expect(Tag.modifyObserver).toBe('ModifyObserver');
    });

    test('should have pageContentObserver tag', () => {
      expect(Tag.pageContentObserver).toBe('PageContentObserver');
    });

    test('should have throttle tag', () => {
      expect(Tag.throttle).toBe('Throttle');
    });
  });

  describe('Popup related tags', () => {
    test('should have popupDecisionTree tag', () => {
      expect(Tag.popupDecisionTree).toBe('PopupDecisionTree');
    });

    test('should have popupRecog tag', () => {
      expect(Tag.popupRecog).toBe('PopupRecog');
    });
  });

  describe('Layout related tags', () => {
    test('should have layoutConstraintDetector tag', () => {
      expect(Tag.layoutConstraintDetector).toBe('LayoutConstraintDetector');
    });

    test('should have layoutUtils tag', () => {
      expect(Tag.layoutUtils).toBe('LayoutUtils');
    });
  });

  describe('Common related tags', () => {
    test('should have util tag', () => {
      expect(Tag.util).toBe('Utils');
    });

    test('should have ccmConfig tag', () => {
      expect(Tag.ccmConfig).toBe('CCMConfig');
    });
  });

  describe('Style related tags', () => {
    test('should have styleSetter tag', () => {
      expect(Tag.styleSetter).toBe('StyleSetter');
    });

    test('should have styleGetter tag', () => {
      expect(Tag.styleGetter).toBe('StyleGetter');
    });

    test('should have styleCleaner tag', () => {
      expect(Tag.styleCleaner).toBe('StyleCleaner');
    });

    test('should have cssSheetManage tag', () => {
      expect(Tag.cssSheetManage).toBe('CSSSheetManage');
    });

    test('should have domReader tag', () => {
      expect(Tag.domReader).toBe('DomReader');
    });

    test('should have originStyleCache tag', () => {
      expect(Tag.originStyleCache).toBe('OriginStyleCache');
    });
  });

  describe('Data related tags', () => {
    test('should have store tag', () => {
      expect(Tag.store).toBe('Store');
    });

    test('should have record tag', () => {
      expect(Tag.record).toBe('Record');
    });

    test('should have diffRecorder tag', () => {
      expect(Tag.diffRecorder).toBe('DiffRecorder');
    });
  });

  describe('Font related tags', () => {
    test('should have zoomFont tag', () => {
      expect(Tag.zoomFont).toBe('ZoomFont');
    });
  });

  describe('Debug helper tags', () => {
    test('should have performance tag', () => {
      expect(Tag.performance).toBe('Performance');
    });

    test('should have visual tag', () => {
      expect(Tag.visual).toBe('Visual');
    });

    test('should have state tag', () => {
      expect(Tag.state).toBe('State');
    });

    test('should have cache tag', () => {
      expect(Tag.cache).toBe('Cache');
    });

    test('should have debug tag', () => {
      expect(Tag.debug).toBe('Debug');
    });

    test('should have component tag', () => {
      expect(Tag.component).toBe('Component');
    });

    test('should have node tag', () => {
      expect(Tag.node).toBe('Node');
    });
  });

  describe('Tag properties', () => {
    test('all tags should be strings', () => {
      const tags = Object.values(Tag);
      tags.forEach(tag => {
        expect(typeof tag).toBe('string');
      });
    });

    test('all tags should be unique', () => {
      const tags = Object.values(Tag);
      const uniqueTags = new Set(tags);
      expect(uniqueTags.size).toBe(tags.length);
    });
    
    test('should have multiple tag categories', () => {
      // Just verify that tags exist and are accessible
      expect(Tag.framework).toBeDefined();
      expect(Tag.util).toBeDefined();
      expect(Tag.performance).toBeDefined();
    });
  });
});
