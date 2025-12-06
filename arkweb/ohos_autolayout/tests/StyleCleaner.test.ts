
import StyleCleaner from '../src/Framework/Common/Style/Setter/StyleCleaner';
import StyleSetter from '../src/Framework/Common/Style/Setter/StyleSetter';
import Store from '../src/Framework/Common/Utils/Store';
import Constant, { LayoutKey, LayoutValue } from '../src/Framework/Common/Constant';
import StyleCommon from '../src/Framework/Common/Style/Common/StyleCommon';
import Cached from '../src/Framework/Common/Cached';
import ObserverHandler from '../src/Framework/Observer/ObserverHandler';
import Utils from '../src/Framework/Common/Utils/Utils';

jest.mock('../src/Framework/Common/Utils/Utils', () => ({
  __esModule: true,
  default: {
    isZoomParent: jest.fn(),
    hadZoomLayout: jest.fn(),
    hadStretchedWidth: jest.fn(),
  },
}));

jest.mock('../src/Debug/Log', () => ({
  __esModule: true,
  default: {
    i: jest.fn(),
    d: jest.fn(),
    w: jest.fn(),
    e: jest.fn(),
    debug: jest.fn(),
    info: jest.fn(),
    warn: jest.fn(),
    error: jest.fn(),
    deleteValue: jest.fn(),
    clearTag: jest.fn(),
  },
}));

jest.mock('../src/Framework/Common/Style/Setter/StyleSetter', () => ({
  __esModule: true,
  default: {
    setStyle: jest.fn(),
    removeAllStyle: jest.fn(),
  },
}));

jest.mock('../src/Framework/Common/Utils/Store', () => ({
  __esModule: true,
  default: {
    getValue: jest.fn(),
    clearAllTag: jest.fn(),
    setValue: jest.fn(),
  },
}));

jest.mock('../src/Framework/Common/Cached', () => ({
  __esModule: true,
  default: {
    documentElement_: jest.fn(() => document.documentElement),
  },
}));

jest.mock('../src/Framework/Observer/ObserverHandler', () => ({
  __esModule: true,
  default: {
    nextRoundAndPost: jest.fn(),
  },
}));

describe('StyleCleaner', () => {
  let ele: HTMLElement;
  let child: HTMLElement;
  let parent: HTMLElement;

  beforeEach(() => {
    document.body.innerHTML = `
      <div id="parent">
        <div id="ele">
          <div id="child"></div>
        </div>
      </div>
    `;
    parent = document.getElementById('parent') as HTMLElement;
    ele = document.getElementById('ele') as HTMLElement;
    child = document.getElementById('child') as HTMLElement;

    jest.clearAllMocks();
  });

  describe('removeAllStyle', () => {
    it('should set a delete marker in StyleCommon.styleCache', () => {
      StyleCleaner.removeAllStyle(ele);
      const expectedMap = new Map().set(Constant.delete, Constant.true);
      expect(StyleCommon.styleCache.get(ele)).toEqual(expectedMap);
    });
  });

  describe('removeSingleStyle', () => {
    it('should call StyleSetter.setStyle with delete marker', () => {
      const attr = 'width';
      StyleCleaner.removeSingleStyle(ele, attr);
      expect(StyleSetter.setStyle).toHaveBeenCalledWith(ele, attr, Constant.delete);
    });
  });

  describe('resetEle', () => {
    it('should call resetChild and resetParent', () => {
      // @ts-ignore
      const resetChildSpy = jest.spyOn(StyleCleaner, 'resetChild').mockImplementation();
      // @ts-ignore
      const resetParentSpy = jest.spyOn(StyleCleaner, 'resetParent').mockImplementation();

      StyleCleaner.resetEle(ele);

      expect(resetChildSpy).toHaveBeenCalledWith(ele, false);
      expect(resetParentSpy).toHaveBeenCalledWith(ele.parentElement);

      resetChildSpy.mockRestore();
      resetParentSpy.mockRestore();
    });

    it('should pass isDelete to resetChild', () => {
        // @ts-ignore
        const resetChildSpy = jest.spyOn(StyleCleaner, 'resetChild').mockImplementation();
        // @ts-ignore
        const resetParentSpy = jest.spyOn(StyleCleaner, 'resetParent').mockImplementation();
  
        StyleCleaner.resetEle(ele, true);
  
        expect(resetChildSpy).toHaveBeenCalledWith(ele, true);
  
        resetChildSpy.mockRestore();
        resetParentSpy.mockRestore();
      });
  });

  describe('resetParent', () => {
    it('should do nothing if element is null or documentElement', () => {
      const removeAllStyleSpy = jest.spyOn(StyleCleaner, 'removeAllStyle');
      StyleCleaner.resetParent(null);
      StyleCleaner.resetParent(document.documentElement);
      expect(removeAllStyleSpy).not.toHaveBeenCalled();
      removeAllStyleSpy.mockRestore();
    });

    it('should do nothing if element does not have ZOOM_PARENT tag', () => {
      (Store.getValue as jest.Mock).mockReturnValue(null);
      const removeAllStyleSpy = jest.spyOn(StyleCleaner, 'removeAllStyle');
      StyleCleaner.resetParent(parent);
      expect(removeAllStyleSpy).not.toHaveBeenCalled();
      removeAllStyleSpy.mockRestore();
    });

    it('should do nothing if any child has a layout tag', () => {
        (Store.getValue as jest.Mock).mockImplementation((el, key) => {
            if (el === parent && (key === LayoutKey.LAYOUT_TAG || key === LayoutKey.CONFIG_LAYOUT_TAG)) {
              return LayoutValue.ZOOM_PARENT;
            }
            if (el === ele && (key === LayoutKey.LAYOUT_TAG || key === LayoutKey.CONFIG_LAYOUT_TAG)) {
              return 'some-layout';
            }
            return null;
          });
      const removeAllStyleSpy = jest.spyOn(StyleCleaner, 'removeAllStyle');
      StyleCleaner.resetParent(parent);
      expect(removeAllStyleSpy).not.toHaveBeenCalled();
      removeAllStyleSpy.mockRestore();
    });

    it('should reset styles and tags if conditions are met', () => {
        (Store.getValue as jest.Mock).mockImplementation((el, key) => {
            if (el === parent && (key === LayoutKey.LAYOUT_TAG || key === LayoutKey.CONFIG_LAYOUT_TAG)) {
              return LayoutValue.ZOOM_PARENT;
            }
            return null;
          });
      const removeAllStyleSpy = jest.spyOn(StyleCleaner, 'removeAllStyle');
      const clearAllTagSpy = jest.spyOn(Store, 'clearAllTag');
      const resetParentSpy = jest.spyOn(StyleCleaner, 'resetParent');

      StyleCleaner.resetParent(parent);

      expect(removeAllStyleSpy).toHaveBeenCalledWith(parent);
      expect(clearAllTagSpy).toHaveBeenCalledWith(parent, false);
      expect(resetParentSpy).toHaveBeenCalledWith(parent.parentElement);

      removeAllStyleSpy.mockRestore();
      resetParentSpy.mockRestore();
    });
  });

  describe('removeWidth', () => {
    it('should remove width, maxWidth, and flexBasis styles', () => {
      const removeSingleStyleSpy = jest.spyOn(StyleCleaner, 'removeSingleStyle');
      StyleCleaner.removeWidth(ele);
      expect(removeSingleStyleSpy).toHaveBeenCalledWith(ele, Constant.width);
      expect(removeSingleStyleSpy).toHaveBeenCalledWith(ele, Constant.max_width);
      expect(removeSingleStyleSpy).toHaveBeenCalledWith(ele, Constant.flex_basis);
      removeSingleStyleSpy.mockRestore();
    });
  });

  describe('resetChild', () => {
    beforeEach(() => {
      // Mock the resetChild method to test it indirectly
      jest.clearAllMocks();
    });

    it('should handle null element gracefully', () => {
      // @ts-ignore
      expect(() => (StyleCleaner).resetChild(null, false)).not.toThrow();
    });

    it('should remove swiper-box elements', () => {
      (Store.getValue as jest.Mock).mockImplementation((el, key) => {
        if (key === LayoutKey.INSERT_SWIPER) {
          return 'swiper';
        }
        return null;
      });

      const swiperDiv = document.createElement('div');
      const parent = document.createElement('div');
      parent.appendChild(swiperDiv);
      document.body.appendChild(parent);

      const removeSpy = jest.spyOn(swiperDiv, 'remove');
      // @ts-ignore
      StyleCleaner.resetChild(swiperDiv, false);

      expect(removeSpy).toHaveBeenCalled();

      document.body.removeChild(parent);
      removeSpy.mockRestore();
    });

    it('should process element and its children recursively', () => {
      const grandChild = document.createElement('div');
      child.appendChild(grandChild);

      const removeAllStyleSpy = jest.spyOn(StyleCleaner, 'removeAllStyle');
      const clearAllTagSpy = jest.spyOn(Store, 'clearAllTag');

      (Store.getValue as jest.Mock).mockReturnValue(null);

      // @ts-ignore
      StyleCleaner.resetChild(child, false);

      expect(removeAllStyleSpy).toHaveBeenCalledWith(child);
      expect(clearAllTagSpy).toHaveBeenCalledWith(child, false);
      expect(removeAllStyleSpy).toHaveBeenCalledWith(grandChild);
      expect(clearAllTagSpy).toHaveBeenCalledWith(grandChild, false);

      removeAllStyleSpy.mockRestore();
      clearAllTagSpy.mockRestore();
    });

    it('should pass isDelete flag correctly', () => {
      const clearAllTagSpy = jest.spyOn(Store, 'clearAllTag');
      (Store.getValue as jest.Mock).mockReturnValue(null);

      // @ts-ignore
      StyleCleaner.resetChild(ele, true);

      expect(clearAllTagSpy).toHaveBeenCalledWith(ele, true);

      clearAllTagSpy.mockRestore();
    });
  });

  describe('resetParentRelayout', () => {
    beforeEach(() => {
      jest.clearAllMocks();
    });

    it('should return false for null element', () => {
      const result = StyleCleaner.resetParentRelayout(null);
      expect(result).toBe(false);
    });

    it('should return false for zoom parent element', () => {
      (Utils.isZoomParent as jest.Mock).mockReturnValue(true);
      
      const result = StyleCleaner.resetParentRelayout(ele);
      
      expect(result).toBe(false);
    });

    it('should return true for element with NEXT_ROUND tag', () => {
      (Utils.isZoomParent as jest.Mock).mockReturnValue(false);
      (Store.getValue as jest.Mock).mockReturnValue(LayoutValue.NEXT_ROUND);
      
      const result = StyleCleaner.resetParentRelayout(ele);
      
      expect(result).toBe(true);
    });

    it('should handle element with zoom layout but no stretched width', () => {
      (Utils.isZoomParent as jest.Mock).mockReturnValue(false);
      (Store.getValue as jest.Mock).mockReturnValue(null);
      (Utils.hadZoomLayout as jest.Mock).mockReturnValue(true);
      (Utils.hadStretchedWidth as jest.Mock).mockReturnValue(false);
      
      const resetEleSpy = jest.spyOn(StyleCleaner, 'resetEle');
      
      const result = StyleCleaner.resetParentRelayout(ele);
      
      expect(resetEleSpy).toHaveBeenCalledWith(ele);
      expect(result).toBe(false);
      
      resetEleSpy.mockRestore();
    });

    it('should handle element with zoom layout and stretched width', () => {
      (Utils.isZoomParent as jest.Mock).mockReturnValue(false);
      (Store.getValue as jest.Mock).mockReturnValue(null);
      (Utils.hadZoomLayout as jest.Mock).mockReturnValue(true);
      (Utils.hadStretchedWidth as jest.Mock).mockReturnValue(true);
      
      const resetEleSpy = jest.spyOn(StyleCleaner, 'resetEle');
      const nextRoundSpy = jest.spyOn(ObserverHandler, 'nextRoundAndPost');
      
      const result = StyleCleaner.resetParentRelayout(ele);
      
      expect(resetEleSpy).toHaveBeenCalledWith(ele);
      expect(nextRoundSpy).toHaveBeenCalledWith(ele);
      expect(result).toBe(true);
      
      resetEleSpy.mockRestore();
      nextRoundSpy.mockRestore();
    });

    it('should recurse to parent element when no zoom layout', () => {
      const parentDiv = document.createElement('div');
      parentDiv.appendChild(ele);
      document.body.appendChild(parentDiv);
      
      (Utils.isZoomParent as jest.Mock).mockReturnValue(false);
      (Store.getValue as jest.Mock).mockReturnValue(null);
      (Utils.hadZoomLayout as jest.Mock).mockReturnValue(false);
      
      const recursiveSpy = jest.spyOn(StyleCleaner, 'resetParentRelayout');
      
      StyleCleaner.resetParentRelayout(ele);
      
      expect(recursiveSpy).toHaveBeenCalledWith(parentDiv);
      
      document.body.removeChild(parentDiv);
      recursiveSpy.mockRestore();
    });
  });

  describe('edge cases and error handling', () => {
    it('should handle removeAllStyle with null element', () => {
      expect(() => StyleCleaner.removeAllStyle(null)).not.toThrow();
    });

    it('should handle removeSingleStyle with null element', () => {
      expect(() => StyleCleaner.removeSingleStyle(null, 'width')).not.toThrow();
    });

    it('should handle resetEle with null element', () => {
      expect(() => StyleCleaner.resetEle(null)).toThrow(TypeError);
      expect(() => StyleCleaner.resetEle(null)).toThrow('Cannot read properties of null');
    });

    it('should handle removeWidth with null element', () => {
      expect(() => StyleCleaner.removeWidth(null)).not.toThrow();
    });
  });

  describe('integration tests', () => {
    it('should complete full reset cycle', () => {
      // Setup complex DOM structure
      const grandParent = document.createElement('div');
      const parentWithZoom = document.createElement('div');
      const childElement = document.createElement('div');
      const grandChildElement = document.createElement('div');
      
      grandParent.appendChild(parentWithZoom);
      parentWithZoom.appendChild(childElement);
      childElement.appendChild(grandChildElement);
      document.body.appendChild(grandParent);
      
      // Mock store values
      (Store.getValue as jest.Mock).mockImplementation((el, key) => {
        if (el === parentWithZoom && (key === LayoutKey.LAYOUT_TAG || key === LayoutKey.CONFIG_LAYOUT_TAG)) {
          return LayoutValue.ZOOM_PARENT;
        }
        return null;
      });
      
      const removeAllStyleSpy = jest.spyOn(StyleCleaner, 'removeAllStyle');
      const clearAllTagSpy = jest.spyOn(Store, 'clearAllTag');
      
      StyleCleaner.resetEle(childElement);
      
      expect(removeAllStyleSpy).toHaveBeenCalledWith(childElement);
      expect(clearAllTagSpy).toHaveBeenCalledWith(childElement, false);
      
      document.body.removeChild(grandParent);
      removeAllStyleSpy.mockRestore();
      clearAllTagSpy.mockRestore();
    });

    it('should handle delete flag propagation', () => {
      const childElement = document.createElement('div');
      ele.appendChild(childElement);
      
      const clearAllTagSpy = jest.spyOn(Store, 'clearAllTag');
      
      StyleCleaner.resetEle(ele, true);
      
      expect(clearAllTagSpy).toHaveBeenCalledWith(ele, true);
      expect(clearAllTagSpy).toHaveBeenCalledWith(childElement, true);
      
      clearAllTagSpy.mockRestore();
    });

    it('should handle complex parent reset with multiple children', () => {
      const childElement1 = document.createElement('div');
      const childElement2 = document.createElement('div');
      parent.appendChild(childElement1);
      parent.appendChild(childElement2);
      
      // Mock first child has layout tag, second doesn't
      (Store.getValue as jest.Mock).mockImplementation((el, key) => {
        if (el === parent && (key === LayoutKey.LAYOUT_TAG || key === LayoutKey.CONFIG_LAYOUT_TAG)) {
          return LayoutValue.ZOOM_PARENT;
        }
        if (el === childElement1 && (key === LayoutKey.LAYOUT_TAG || key === LayoutKey.CONFIG_LAYOUT_TAG)) {
          return 'some-layout';
        }
        return null;
      });
      
      const removeAllStyleSpy = jest.spyOn(StyleCleaner, 'removeAllStyle');
      
      StyleCleaner.resetParent(parent);
      
      // Should not remove styles because one child has layout tag
      expect(removeAllStyleSpy).not.toHaveBeenCalledWith(parent);
      
      removeAllStyleSpy.mockRestore();
    });
  });
});
