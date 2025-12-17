/**
 * Unit tests for Constant.ts
 */
import Constant, { LayoutValue, LayoutKey, HwTag } from '../src/Framework/Common/Constant';

describe('Constant Module', () => {
  describe('Layout constants', () => {
    test('should have display constant', () => {
      expect(Constant.display).toBe('display');
    });

    test('should have width constant', () => {
      expect(Constant.width).toBe('width');
    });

    test('should have position constant', () => {
      expect(Constant.position).toBe('position');
    });

    test('should have transform constant', () => {
      expect(Constant.transform).toBe('transform');
    });

    test('should have flex constant', () => {
      expect(Constant.flex).toBe('flex');
    });
  });

  describe('Position constants', () => {
    test('should have fixed constant', () => {
      expect(Constant.fixed).toBe('fixed');
    });

    test('should have sticky constant', () => {
      expect(Constant.sticky).toBe('sticky');
    });

    test('should have absolute constant', () => {
      expect(Constant.absolute).toBe('absolute');
    });

    test('should have static constant', () => {
      expect(Constant.static).toBe('static');
    });
  });

  describe('Display values', () => {
    test('should have none constant', () => {
      expect(Constant.none).toBe('none');
    });

    test('should have grid constant', () => {
      expect(Constant.grid).toBe('grid');
    });
  });

  describe('Numeric constants', () => {
    test('should have minMargin', () => {
      expect(Constant.minMargin).toBe(5);
    });

    test('should have ATTR_NUM', () => {
      expect(Constant.ATTR_NUM).toBe(5);
    });

    test('should have discrepancy', () => {
      expect(Constant.discrepancy).toBe(1);
    });

    test('should have maxCloseButtonSizeRatio', () => {
      expect(Constant.maxCloseButtonSizeRatio).toBe(0.15);
    });

    test('should have error code constants', () => {
      expect(Constant.ERR_CODE_GAPS).toBe(100);
      expect(Constant.ERR_CODE_OVERFLOW).toBe(50);
      expect(Constant.ERR_CODE_OFFSET).toBe(10);
      expect(Constant.ERR_CODE_CLOSE_BTN_OVERLAP).toBe(2);
      expect(Constant.ERR_CODE_INTERACTIVE).toBe(1);
    });
  });

  describe('Element names', () => {
    test('should have script constant', () => {
      expect(Constant.script).toBe('script');
    });

    test('should have style constant', () => {
      expect(Constant.style).toBe('style');
    });

    test('should have link constant', () => {
      expect(Constant.link).toBe('link');
    });

    test('should have meta constant', () => {
      expect(Constant.meta).toBe('meta');
    });
  });
});

describe('LayoutValue Constants', () => {
  test('should have DISABLE value', () => {
    expect(LayoutValue.DISABLE).toBe('disable');
  });

  test('should have VERTICAL_GRID value', () => {
    expect(LayoutValue.VERTICAL_GRID).toBe('vertical_grid');
  });

  test('should have ASIDE_BAR value', () => {
    expect(LayoutValue.ASIDE_BAR).toBe('asideBar');
  });

  test('should have BACK_IMG value', () => {
    expect(LayoutValue.BACK_IMG).toBe('backImg');
  });

  test('should have ONE_LINE value', () => {
    expect(LayoutValue.ONE_LINE).toBe('oneLine');
  });

  test('should have ZOOM_PARENT value', () => {
    expect(LayoutValue.ZOOM_PARENT).toBe('zoomParent');
  });

  test('should have IS_IMG value', () => {
    expect(LayoutValue.IS_IMG).toBe('isImg');
  });

  test('should have HAS_TEXT_CHILD value', () => {
    expect(LayoutValue.HAS_TEXT_CHILD).toBe('hasTextChild');
  });

  test('should have WATERFALL value', () => {
    expect(LayoutValue.WATERFALL).toBe('waterfall');
  });

  test('should have SWIPER value', () => {
    expect(LayoutValue.SWIPER).toBe('swiper');
  });

  test('should have SCROLL_LIST value', () => {
    expect(LayoutValue.SCROLL_LIST).toBe('scrollList');
  });

  test('should have COMMENT value', () => {
    expect(LayoutValue.COMMENT).toBe('comment');
  });

  test('should have HEADER value', () => {
    expect(LayoutValue.HEADER).toBe('header');
  });

  test('should have ACROSS_COLUMN value', () => {
    expect(LayoutValue.ACROSS_COLUMN).toBe('across_column');
  });

  test('should have NEXT_ROUND value', () => {
    expect(LayoutValue.NEXT_ROUND).toBe('next_round');
  });

  test('should have BOTTOM value', () => {
    expect(LayoutValue.BOTTOM).toBe('bottom');
  });

  test('should have EMPTY value', () => {
    expect(LayoutValue.EMPTY).toBe('empty');
  });
});

describe('LayoutKey Constants', () => {
  test('should have LAYOUT_TAG key', () => {
    expect(LayoutKey.LAYOUT_TAG).toBe('hwLayout');
  });

  test('should have CONFIG_LAYOUT_TAG key', () => {
    expect(LayoutKey.CONFIG_LAYOUT_TAG).toBe('thirdLayout');
  });

  test('should have DIFF_ELE key', () => {
    expect(LayoutKey.DIFF_ELE).toBe('diffEle');
  });

  test('should have INSERT_RULE key', () => {
    expect(LayoutKey.INSERT_RULE).toBe('insertRule');
  });

  test('should have INSERT_SWIPER key', () => {
    expect(LayoutKey.INSERT_SWIPER).toBe('insertSwiper');
  });
});

describe('HwTag Constants', () => {
  test('should have MULTI_IMGS tag', () => {
    expect(HwTag.MULTI_IMGS).toBe('multi_items');
  });

  test('should have ELE_NUM tag', () => {
    expect(HwTag.ELE_NUM).toBe('ele_num');
  });

  test('should have ORIGIN_WIDTH tag', () => {
    expect(HwTag.ORIGIN_WIDTH).toBe('origin_width');
  });

  test('should have HEIGHT_0 tag', () => {
    expect(HwTag.HEIGHT_0).toBe('height0');
  });

  test('should have FORCE_CSS tag', () => {
    expect(HwTag.FORCE_CSS).toBe('force_css');
  });

  test('should have ORIGIN_HEIGHT tag', () => {
    expect(HwTag.ORIGIN_HEIGHT).toBe('origin_height');
  });
});
