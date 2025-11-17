/**
 * Unit tests for PopupInfo.ts
 */
import { PopupInfo, PotentialElements } from '../src/Framework/Popup/PopupInfo';
import { PopupType } from '../src/Framework/Popup/PopupType';

describe('PopupInfo Interface', () => {
  describe('Interface structure', () => {
    test('should allow creating complete PopupInfo object', () => {
      const rootNode = document.createElement('div');
      const maskNode = document.createElement('div');
      const contentNode = document.createElement('div');

      const popupInfo: PopupInfo = {
        root_node: rootNode,
        mask_node: maskNode,
        content_node: contentNode,
        popup_type: PopupType.A,
        root_position: 'fixed',
        root_zindex: 1000,
        has_mask: true,
        root_screen_area_ratio: 0.8,
        root_is_visiable: true,
        has_close_button: true,
        mask_area_ratio: 0.9,
        mask_position: 'fixed',
        mask_zindex: 999,
        stickyTop_height: 50,
        stickyBottom_height: 60,
      };

      expect(popupInfo).toBeDefined();
      expect(popupInfo.root_node).toBe(rootNode);
      expect(popupInfo.popup_type).toBe(PopupType.A);
    });

    test('should have all required properties', () => {
      const popupInfo: PopupInfo = {
        root_node: document.createElement('div'),
        mask_node: document.createElement('div'),
        content_node: document.createElement('div'),
        popup_type: PopupType.B,
        root_position: 'absolute',
        root_zindex: 100,
        has_mask: false,
        root_screen_area_ratio: 0.5,
        root_is_visiable: true,
        has_close_button: false,
        mask_area_ratio: 0,
        mask_position: 'static',
        mask_zindex: 0,
        stickyTop_height: 0,
        stickyBottom_height: 0,
      };

      expect(popupInfo).toHaveProperty('root_node');
      expect(popupInfo).toHaveProperty('mask_node');
      expect(popupInfo).toHaveProperty('content_node');
      expect(popupInfo).toHaveProperty('popup_type');
      expect(popupInfo).toHaveProperty('root_position');
      expect(popupInfo).toHaveProperty('root_zindex');
      expect(popupInfo).toHaveProperty('has_mask');
      expect(popupInfo).toHaveProperty('root_screen_area_ratio');
      expect(popupInfo).toHaveProperty('root_is_visiable');
      expect(popupInfo).toHaveProperty('has_close_button');
      expect(popupInfo).toHaveProperty('mask_area_ratio');
      expect(popupInfo).toHaveProperty('mask_position');
      expect(popupInfo).toHaveProperty('mask_zindex');
      expect(popupInfo).toHaveProperty('stickyTop_height');
      expect(popupInfo).toHaveProperty('stickyBottom_height');
    });
  });

  describe('DOM node properties', () => {
    test('should accept valid HTMLElement for root_node', () => {
      const rootNode = document.createElement('div');
      rootNode.id = 'popup-root';

      const popupInfo: PopupInfo = {
        root_node: rootNode,
        mask_node: document.createElement('div'),
        content_node: document.createElement('div'),
        popup_type: PopupType.A,
        root_position: 'fixed',
        root_zindex: 1000,
        has_mask: true,
        root_screen_area_ratio: 0.8,
        root_is_visiable: true,
        has_close_button: true,
        mask_area_ratio: 0.9,
        mask_position: 'fixed',
        mask_zindex: 999,
        stickyTop_height: 50,
        stickyBottom_height: 60,
      };

      expect(popupInfo.root_node.id).toBe('popup-root');
    });

    test('should accept different elements for mask and content nodes', () => {
      const maskNode = document.createElement('div');
      maskNode.className = 'mask';
      const contentNode = document.createElement('section');
      contentNode.className = 'content';

      const popupInfo: PopupInfo = {
        root_node: document.createElement('div'),
        mask_node: maskNode,
        content_node: contentNode,
        popup_type: PopupType.C,
        root_position: 'fixed',
        root_zindex: 100,
        has_mask: true,
        root_screen_area_ratio: 0.6,
        root_is_visiable: true,
        has_close_button: false,
        mask_area_ratio: 0.8,
        mask_position: 'absolute',
        mask_zindex: 99,
        stickyTop_height: 0,
        stickyBottom_height: 0,
      };

      expect(popupInfo.mask_node.className).toBe('mask');
      expect(popupInfo.content_node.className).toBe('content');
    });
  });

  describe('PopupType integration', () => {
    test('should work with PopupType.A', () => {
      const popupInfo: PopupInfo = {
        root_node: document.createElement('div'),
        mask_node: document.createElement('div'),
        content_node: document.createElement('div'),
        popup_type: PopupType.A,
        root_position: 'fixed',
        root_zindex: 1000,
        has_mask: true,
        root_screen_area_ratio: 0.8,
        root_is_visiable: true,
        has_close_button: true,
        mask_area_ratio: 0.9,
        mask_position: 'fixed',
        mask_zindex: 999,
        stickyTop_height: 50,
        stickyBottom_height: 60,
      };

      expect(popupInfo.popup_type).toBe(PopupType.A);
      expect(popupInfo.popup_type).toBe(1);
    });

    test('should work with all PopupType values', () => {
      const types = [PopupType.A, PopupType.B, PopupType.C, PopupType.Unknown];

      types.forEach(type => {
        const popupInfo: PopupInfo = {
          root_node: document.createElement('div'),
          mask_node: document.createElement('div'),
          content_node: document.createElement('div'),
          popup_type: type,
          root_position: 'fixed',
          root_zindex: 100,
          has_mask: true,
          root_screen_area_ratio: 0.5,
          root_is_visiable: true,
          has_close_button: false,
          mask_area_ratio: 0.5,
          mask_position: 'fixed',
          mask_zindex: 99,
          stickyTop_height: 0,
          stickyBottom_height: 0,
        };

        expect(popupInfo.popup_type).toBe(type);
      });
    });
  });

  describe('Boolean properties', () => {
    test('should handle has_mask property', () => {
      const withMask: PopupInfo = {
        root_node: document.createElement('div'),
        mask_node: document.createElement('div'),
        content_node: document.createElement('div'),
        popup_type: PopupType.A,
        root_position: 'fixed',
        root_zindex: 100,
        has_mask: true,
        root_screen_area_ratio: 0.5,
        root_is_visiable: true,
        has_close_button: false,
        mask_area_ratio: 0.8,
        mask_position: 'fixed',
        mask_zindex: 99,
        stickyTop_height: 0,
        stickyBottom_height: 0,
      };

      const withoutMask: PopupInfo = { ...withMask, has_mask: false };

      expect(withMask.has_mask).toBe(true);
      expect(withoutMask.has_mask).toBe(false);
    });

    test('should handle root_is_visiable property', () => {
      const visible: PopupInfo = {
        root_node: document.createElement('div'),
        mask_node: document.createElement('div'),
        content_node: document.createElement('div'),
        popup_type: PopupType.B,
        root_position: 'fixed',
        root_zindex: 100,
        has_mask: true,
        root_screen_area_ratio: 0.5,
        root_is_visiable: true,
        has_close_button: false,
        mask_area_ratio: 0.5,
        mask_position: 'fixed',
        mask_zindex: 99,
        stickyTop_height: 0,
        stickyBottom_height: 0,
      };

      const hidden: PopupInfo = { ...visible, root_is_visiable: false };

      expect(visible.root_is_visiable).toBe(true);
      expect(hidden.root_is_visiable).toBe(false);
    });

    test('should handle has_close_button property', () => {
      const withButton: PopupInfo = {
        root_node: document.createElement('div'),
        mask_node: document.createElement('div'),
        content_node: document.createElement('div'),
        popup_type: PopupType.C,
        root_position: 'fixed',
        root_zindex: 100,
        has_mask: true,
        root_screen_area_ratio: 0.5,
        root_is_visiable: true,
        has_close_button: true,
        mask_area_ratio: 0.5,
        mask_position: 'fixed',
        mask_zindex: 99,
        stickyTop_height: 0,
        stickyBottom_height: 0,
      };

      expect(withButton.has_close_button).toBe(true);
    });
  });

  describe('Numeric properties', () => {
    test('should handle z-index values', () => {
      const popupInfo: PopupInfo = {
        root_node: document.createElement('div'),
        mask_node: document.createElement('div'),
        content_node: document.createElement('div'),
        popup_type: PopupType.A,
        root_position: 'fixed',
        root_zindex: 9999,
        has_mask: true,
        root_screen_area_ratio: 0.5,
        root_is_visiable: true,
        has_close_button: false,
        mask_area_ratio: 0.5,
        mask_position: 'fixed',
        mask_zindex: 9998,
        stickyTop_height: 0,
        stickyBottom_height: 0,
      };

      expect(popupInfo.root_zindex).toBe(9999);
      expect(popupInfo.mask_zindex).toBe(9998);
      expect(popupInfo.root_zindex).toBeGreaterThan(popupInfo.mask_zindex);
    });

    test('should handle area ratio values', () => {
      const popupInfo: PopupInfo = {
        root_node: document.createElement('div'),
        mask_node: document.createElement('div'),
        content_node: document.createElement('div'),
        popup_type: PopupType.B,
        root_position: 'fixed',
        root_zindex: 100,
        has_mask: true,
        root_screen_area_ratio: 0.75,
        root_is_visiable: true,
        has_close_button: false,
        mask_area_ratio: 0.85,
        mask_position: 'fixed',
        mask_zindex: 99,
        stickyTop_height: 0,
        stickyBottom_height: 0,
      };

      expect(popupInfo.root_screen_area_ratio).toBe(0.75);
      expect(popupInfo.mask_area_ratio).toBe(0.85);
    });

    test('should handle sticky height values', () => {
      const popupInfo: PopupInfo = {
        root_node: document.createElement('div'),
        mask_node: document.createElement('div'),
        content_node: document.createElement('div'),
        popup_type: PopupType.C,
        root_position: 'fixed',
        root_zindex: 100,
        has_mask: false,
        root_screen_area_ratio: 0.5,
        root_is_visiable: true,
        has_close_button: false,
        mask_area_ratio: 0,
        mask_position: 'static',
        mask_zindex: 0,
        stickyTop_height: 60,
        stickyBottom_height: 80,
      };

      expect(popupInfo.stickyTop_height).toBe(60);
      expect(popupInfo.stickyBottom_height).toBe(80);
    });
  });

  describe('String properties', () => {
    test('should handle position values', () => {
      const positions = ['fixed', 'absolute', 'relative', 'static', 'sticky'];

      positions.forEach(position => {
        const popupInfo: PopupInfo = {
          root_node: document.createElement('div'),
          mask_node: document.createElement('div'),
          content_node: document.createElement('div'),
          popup_type: PopupType.A,
          root_position: position,
          root_zindex: 100,
          has_mask: true,
          root_screen_area_ratio: 0.5,
          root_is_visiable: true,
          has_close_button: false,
          mask_area_ratio: 0.5,
          mask_position: position,
          mask_zindex: 99,
          stickyTop_height: 0,
          stickyBottom_height: 0,
        };

        expect(popupInfo.root_position).toBe(position);
        expect(popupInfo.mask_position).toBe(position);
      });
    });
  });
});

describe('PotentialElements Interface', () => {
  describe('Interface structure', () => {
    test('should allow creating PotentialElements object', () => {
      const mask1 = document.createElement('div');
      const mask2 = document.createElement('div');
      const stickyTop = document.createElement('header');
      const stickyBottom = document.createElement('footer');

      const potentialElements: PotentialElements = {
        potentialMasks: [mask1, mask2],
        potentialStickyTop: stickyTop,
        potentialStickyBottom: stickyBottom,
      };

      expect(potentialElements).toBeDefined();
      expect(potentialElements.potentialMasks.length).toBe(2);
    });

    test('should have all required properties', () => {
      const potentialElements: PotentialElements = {
        potentialMasks: [],
        potentialStickyTop: document.createElement('div'),
        potentialStickyBottom: document.createElement('div'),
      };

      expect(potentialElements).toHaveProperty('potentialMasks');
      expect(potentialElements).toHaveProperty('potentialStickyTop');
      expect(potentialElements).toHaveProperty('potentialStickyBottom');
    });
  });

  describe('potentialMasks array', () => {
    test('should accept empty array', () => {
      const potentialElements: PotentialElements = {
        potentialMasks: [],
        potentialStickyTop: document.createElement('div'),
        potentialStickyBottom: document.createElement('div'),
      };

      expect(potentialElements.potentialMasks).toEqual([]);
      expect(potentialElements.potentialMasks.length).toBe(0);
    });

    test('should accept multiple mask elements', () => {
      const masks = [
        document.createElement('div'),
        document.createElement('div'),
        document.createElement('div'),
      ];

      const potentialElements: PotentialElements = {
        potentialMasks: masks,
        potentialStickyTop: document.createElement('div'),
        potentialStickyBottom: document.createElement('div'),
      };

      expect(potentialElements.potentialMasks.length).toBe(3);
      expect(potentialElements.potentialMasks).toEqual(masks);
    });

    test('should allow array operations', () => {
      const potentialElements: PotentialElements = {
        potentialMasks: [document.createElement('div')],
        potentialStickyTop: document.createElement('div'),
        potentialStickyBottom: document.createElement('div'),
      };

      potentialElements.potentialMasks.push(document.createElement('div'));

      expect(potentialElements.potentialMasks.length).toBe(2);
    });
  });

  describe('Sticky elements', () => {
    test('should accept different element types', () => {
      const header = document.createElement('header');
      const footer = document.createElement('footer');

      const potentialElements: PotentialElements = {
        potentialMasks: [],
        potentialStickyTop: header,
        potentialStickyBottom: footer,
      };

      expect(potentialElements.potentialStickyTop.tagName).toBe('HEADER');
      expect(potentialElements.potentialStickyBottom.tagName).toBe('FOOTER');
    });

    test('should allow same element type for both sticky positions', () => {
      const div1 = document.createElement('div');
      const div2 = document.createElement('div');

      const potentialElements: PotentialElements = {
        potentialMasks: [],
        potentialStickyTop: div1,
        potentialStickyBottom: div2,
      };

      expect(potentialElements.potentialStickyTop).toBe(div1);
      expect(potentialElements.potentialStickyBottom).toBe(div2);
    });
  });

  describe('Practical usage', () => {
    test('should work with filter operations on masks', () => {
      const mask1 = document.createElement('div');
      mask1.className = 'visible';
      const mask2 = document.createElement('div');
      mask2.className = 'hidden';

      const potentialElements: PotentialElements = {
        potentialMasks: [mask1, mask2],
        potentialStickyTop: document.createElement('div'),
        potentialStickyBottom: document.createElement('div'),
      };

      const visibleMasks = (potentialElements.potentialMasks as HTMLElement[])
        .filter(el => el.className === 'visible');

      expect(visibleMasks.length).toBe(1);
    });

    test('should support object destructuring', () => {
      const elements: PotentialElements = {
        potentialMasks: [document.createElement('div')],
        potentialStickyTop: document.createElement('header'),
        potentialStickyBottom: document.createElement('footer'),
      };

      const { potentialMasks, potentialStickyTop, potentialStickyBottom } = elements;

      expect(potentialMasks).toBeDefined();
      expect(potentialStickyTop).toBeDefined();
      expect(potentialStickyBottom).toBeDefined();
    });
  });
});
