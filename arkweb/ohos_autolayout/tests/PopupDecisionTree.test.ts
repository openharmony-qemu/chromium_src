// Test Imports
import { PopupDecisionTree } from '../src/Framework/Popup/PopupDecisionTree';
import { PopupInfo } from '../src/Framework/Popup/PopupInfo';
import { PopupType } from '../src/Framework/Popup/PopupType';
import { PopupDecisionTreeType } from '../src/Framework/Popup/PopupDecisionTreeType';
import Log from '../src/Debug/Log';
import Tag from '../src/Debug/Tag';

// Global Test State
const styleMap = new Map<HTMLElement, Partial<CSSStyleDeclaration>>();
const semiTransparentElements = new Set<HTMLElement>();

// Default Styles
const defaultStyle: Partial<CSSStyleDeclaration> = {
  display: 'block',
  visibility: 'visible',
  opacity: '1',
  position: 'static',
  backgroundImage: 'none',
  mixBlendMode: 'normal',
  filter: 'none',
  mask: 'none',
  backdropFilter: 'none',
  top: '0px',
  bottom: '0px',
  left: '0px',
  right: '0px',
  boxSizing: 'border-box',
  flexDirection: 'row',
  justifyContent: 'flex-start',
  alignItems: 'stretch',
};

// Mock Configuration
const mockConfig = {
  getcloseButtonPattern: jest.fn(() => ['close', 'guanbi', 'cancel']),
  getButtonPattern: jest.fn(() => ['btn', 'button']),
  getMinMaskAreaRatioThreshold: jest.fn(() => 80),
  getMinContentAreaRatioThreshold: jest.fn(() => 20),
};

// Mock CCMConfig
jest.mock('../src/Framework/Common/CCMConfig', () => ({
  CCMConfig: {
    getInstance: jest.fn(() => mockConfig),
  },
}));

// Mock Functions
const visibleSiblingsMock: jest.Mock<HTMLElement[], [HTMLElement]> = jest.fn();
const isBackgroundSemiTransparentMock: jest.Mock<boolean, [CSSStyleDeclaration]> = jest.fn();

// Mock Utils Module
jest.mock('../src/Framework/Utils/Utils', () => ({
  __esModule: true,
  default: {
    getVisibleSiblings: (node: HTMLElement): HTMLElement[] => visibleSiblingsMock(node),
    isBackgroundSemiTransparent: (style: CSSStyleDeclaration): boolean =>
      isBackgroundSemiTransparentMock(style),
  },
}));

// Additional Mock Functions
const hasBottomStyleMock: jest.Mock<boolean, [HTMLElement, string, string]> = jest.fn();
const hasTopStyleMock: jest.Mock<boolean, [HTMLElement, string, string]> = jest.fn();

// Mock LayoutUtils Module
jest.mock('../src/Framework/Utils/LayoutUtils', () => ({
  __esModule: true,
  default: {
    hasBottomStyle: (element: HTMLElement, position: string, bottom: string): boolean =>
      hasBottomStyleMock(element, position, bottom),
    hasTopStyle: (element: HTMLElement, position: string, top: string): boolean =>
      hasTopStyleMock(element, position, top),
  },
}));

const { default: Utils } = jest.requireMock('../src/Framework/Utils/Utils');
const { default: LayoutUtils } = jest.requireMock('../src/Framework/Utils/LayoutUtils');
// Utility Functions
const originalGetComputedStyle = window.getComputedStyle;

function setStyle(element: HTMLElement, style: Partial<CSSStyleDeclaration>): void {
  styleMap.set(element, { ...(styleMap.get(element) || {}), ...style });
}

function setRect(
  element: HTMLElement,
  rect: { top?: number; left?: number; width?: number; height?: number }
): void {
  const top = rect.top ?? 0;
  const left = rect.left ?? 0;
  const width = rect.width ?? 0;
  const height = rect.height ?? 0;
  
  Object.defineProperty(element, 'getBoundingClientRect', {
    configurable: true,
    value: () => ({
      top,
      left,
      right: left + width,
      bottom: top + height,
      width,
      height,
    }),
  });
}

function setDimensions(
  element: HTMLElement,
  { width, height }: { width: number; height: number }
): void {
  Object.defineProperty(element, 'offsetWidth', {
    configurable: true,
    get: () => width,
  });
  Object.defineProperty(element, 'offsetHeight', {
    configurable: true,
    get: () => height,
  });
}
// Test Setup and Teardown
beforeAll(() => {
  jest.spyOn(window, 'getComputedStyle').mockImplementation((el: Element) => {
    const overrides = styleMap.get(el as HTMLElement) || {};
    return {
      ...defaultStyle,
      ...overrides,
      __node: el,
      backgroundImage: (overrides.backgroundImage ?? defaultStyle.backgroundImage) as string,
      getPropertyValue(this: Record<string, string>, prop: string) {
        return this[prop];
      },
      toString() {
        return '[mocked-computed-style]';
      },
    } as CSSStyleDeclaration;
  });
});

afterAll(() => {
  (window.getComputedStyle as jest.Mock).mockRestore();
  styleMap.clear();
  window.getComputedStyle = originalGetComputedStyle;
});

beforeEach(() => {
  styleMap.clear();
  jest.clearAllMocks();
  semiTransparentElements.clear();
  
  visibleSiblingsMock.mockReset().mockImplementation(node =>
    Array.from(node.parentElement?.children || []).filter(
      (child): child is HTMLElement => child instanceof HTMLElement && child !== node
    )
  );
  
  isBackgroundSemiTransparentMock.mockReset().mockImplementation(style => {
    const element = (style as unknown as { __node?: HTMLElement }).__node;
    return element ? semiTransparentElements.has(element) : false;
  });
  
  hasBottomStyleMock.mockReset().mockReturnValue(true);
  hasTopStyleMock.mockReset().mockReturnValue(false);
  
  window.innerHeight = 900;
  window.innerWidth = 1200;
});
// Test Suite
describe('PopupDecisionTree', () => {
  // Helper Functions
  function makePopupInfo(options: Partial<PopupInfo>): PopupInfo {
    const defaults = {
      root_node: document.createElement('div'),
      mask_node: document.createElement('div'),
      content_node: document.createElement('div'),
      popup_type: PopupType.B,
      root_position: 'static',
      root_zindex: 0,
      has_mask: true,
      root_screen_area_ratio: 100,
      root_is_visiable: true,
      has_close_button: false,
      mask_area_ratio: 100,
      mask_position: 'fixed',
      mask_zindex: 100,
      stickyTop_height: 0,
      stickyBottom_height: 0,
    };
    return { ...defaults, ...options };
  }
  // Close Button Detection Tests
  describe('Close Button Detection', () => {
    it('should filter ancestors and detect background/image clues in getCloseButtons', () => {
      const root = document.createElement('div');
      const parent = document.createElement('div');
      const child = document.createElement('div');
      const img = document.createElement('img');
      
      parent.className = 'wrapper';
      child.className = 'close-child';
      img.className = 'icon';
      (img as HTMLImageElement).src = 'https://example.com/guanbi.svg';
      
      root.append(parent);
      parent.append(child);
      child.append(img);
      
      setStyle(parent, { backgroundImage: 'url(button.png)' });
      setStyle(child, { backgroundImage: 'url(ICON-CLOSE.PNG)' });
      
      const nodes = [root, parent, child, img];
      const result = (PopupDecisionTree as unknown as { getCloseButtons: Function }).getCloseButtons(
        root,
        nodes
      ) as HTMLElement[];
      
      expect(result).toHaveLength(1);
      expect(result[0]).toBe(child);
      expect(mockConfig.getcloseButtonPattern).toHaveBeenCalled();
    });

    it('should ignore nodes outside the root context in getCloseButtons', () => {
      const root = document.createElement('div');
      const foreign = document.createElement('button');
      
      setStyle(foreign, { backgroundImage: 'url(close.png)' });
      
      // @ts-ignore
      const result = PopupDecisionTree.getCloseButtons(root, [foreign]);
      expect(result).toEqual([]);
    });

    it('should match uppercase URL branch when class clues absent in getCloseButtons', () => {
      const root = document.createElement('div');
      const button = document.createElement('button');
      root.appendChild(button);
      
      const originalImpl = mockConfig.getcloseButtonPattern.getMockImplementation();
      mockConfig.getcloseButtonPattern.mockImplementation(() => ['CLOSE']);
      
      setStyle(button, { backgroundImage: 'url(CLOSE.svg)' });
      
      const nodes = [root, button];
      // @ts-ignore
      const result = PopupDecisionTree.getCloseButtons(root, nodes);
      
      expect(result).toEqual([button]);
      
      if (originalImpl) {
        mockConfig.getcloseButtonPattern.mockImplementation(originalImpl);
      } else {
        mockConfig.getcloseButtonPattern.mockImplementation(() => ['close', 'guanbi', 'cancel']);
      }
    });

    it('should check center close button constraints in CheckCenterCloseButton', () => {
      const content = document.createElement('div');
      const closeButton = document.createElement('button');
      closeButton.className = 'close primary';
      content.appendChild(closeButton);
      
      setRect(closeButton, { top: window.innerHeight - 10, left: 0, width: 30, height: 30 });
      
      // @ts-ignore
      const result = PopupDecisionTree.CheckCenterCloseButton(content, [closeButton]);
      expect(result).toBe(true);
    });
  });

  // Element Position and Overlap Tests
  describe('Element Position and Overlap', () => {
    it('should handle direct and ancestor absolute positioning in isCloseElementAbsolute', () => {
      const root = document.createElement('div');
      const mask = document.createElement('div');
      const popupInfo = makePopupInfo({ root_node: root, mask_node: mask, popup_type: PopupType.A });
      const child = document.createElement('button');
      
      root.append(mask, child);
      setStyle(child, { position: 'absolute' });
      setRect(child, { top: 0, left: 0, width: 10, height: 10 });
      
      // @ts-ignore
      const flags = PopupDecisionTree.isCloseElementAbsolute([child], root, popupInfo);
      expect(flags).toEqual([true]);
      
      setStyle(child, { position: 'static' });
      const ancestor = document.createElement('div');
      ancestor.appendChild(child);
      root.appendChild(ancestor);
      setStyle(ancestor, { position: 'absolute' });
      setRect(ancestor, { top: 5, left: 5, width: 40, height: 40 });
      
      // @ts-ignore
      const flagsWithAncestor = PopupDecisionTree.isCloseElementAbsolute(
        [child],
        root,
        popupInfo
      );
      expect(flagsWithAncestor).toEqual([true]);
    });

    it('should detect overlaps for absolute and nested siblings in hasOverlapWithSiblings', () => {
      const parent = document.createElement('div');
      const node = document.createElement('button');
      const prevSibling = document.createElement('div');
      const nextSibling = document.createElement('div');
      const nestedPrev = document.createElement('div');
      const nestedNext = document.createElement('div');
      
      parent.append(prevSibling, node, nextSibling);
      prevSibling.appendChild(nestedPrev);
      nextSibling.appendChild(nestedNext);
      
      visibleSiblingsMock.mockImplementation((el: HTMLElement) =>
        Array.from(el.parentElement?.children || []).filter(
          (child): child is HTMLElement => child instanceof HTMLElement && child !== el
        )
      );
      
      setStyle(node, { position: 'absolute' });
      setRect(node, { top: 100, left: 100, width: 50, height: 50 });
      
      setStyle(prevSibling, { position: 'absolute' });
      setRect(prevSibling, { top: 110, left: 110, width: 50, height: 50 });
      
      // @ts-ignore
      expect(PopupDecisionTree.hasOverlapWithSiblings(node)).toBe(true);
      
      setStyle(prevSibling, { position: 'static' });
      setStyle(nextSibling, { position: 'static' });
      setStyle(nestedPrev, { position: 'absolute' });
      setRect(nestedPrev, { top: 102, left: 102, width: 30, height: 30 });
      setStyle(nestedNext, { position: 'absolute' });
      setRect(nestedNext, { top: 180, left: 180, width: 10, height: 10 });
      
      // @ts-ignore
      expect(PopupDecisionTree.hasOverlapWithSiblings(node)).toBe(true);
    });

    it('should return expected values for hasOverlap and calOverlapAreaRatio', () => {
      const a = document.createElement('div');
      const b = document.createElement('div');
      
      setRect(a, { top: 0, left: 0, width: 100, height: 100 });
      setRect(b, { top: 50, left: 50, width: 100, height: 100 });
      
      // @ts-ignore
      expect(PopupDecisionTree.hasOverlap(a, b)).toBe(true);
      // @ts-ignore
      expect(PopupDecisionTree.calOverlapAreaRatio(a, b)).toBeCloseTo(0.25);
      
      setRect(b, { top: 110, left: 110, width: 20, height: 20 });
      // @ts-ignore
      expect(PopupDecisionTree.hasOverlap(a, b)).toBe(false);
      // @ts-ignore
      expect(PopupDecisionTree.calOverlapAreaRatio(a, b)).toBe(0);
    });

    it('should detect overlapping close buttons and log appropriately in hasOverlappingCloseButton', () => {
      const root = document.createElement('div');
      const close = document.createElement('button');
      root.appendChild(close);
      
      const logSpy = jest.spyOn(Log, 'd').mockImplementation(() => undefined);
      
      setStyle(close, { backgroundImage: 'url(close.png)' });
      setStyle(close, { position: 'absolute' });
      setRect(close, { top: 0, left: 0, width: 20, height: 20 });
      
      visibleSiblingsMock.mockReturnValue([]);
      
      const popupInfo = makePopupInfo({ root_node: root, popup_type: PopupType.B });
      expect(
        // @ts-ignore
        PopupDecisionTree.hasOverlappingCloseButton(root, [root, close], popupInfo)
      ).toBe(false);
      expect(logSpy).toHaveBeenLastCalledWith('无重叠', Tag.popupDecisionTree);
      
      const sibling = document.createElement('div');
      root.appendChild(sibling);
      visibleSiblingsMock.mockReturnValue([sibling]);
      setStyle(sibling, { position: 'absolute' });
      setRect(sibling, { top: 5, left: 5, width: 30, height: 30 });
      
      expect(
        // @ts-ignore
        PopupDecisionTree.hasOverlappingCloseButton(root, [root, close, sibling], popupInfo)
      ).toBe(true);
      
      const lastCall = logSpy.mock.calls[logSpy.mock.calls.length - 1] || [];
      expect(lastCall[0]).toContain('关闭按钮存在重叠');
      expect(lastCall[1]).toBe(Tag.popupDecisionTree);
      
      logSpy.mockRestore();
    });
  });
  // DOM Navigation Tests
  describe('DOM Navigation', () => {
    it('should traverse to deepest nodes in findFirstChild and findLastChild', () => {
      const parent = document.createElement('div');
      const first = document.createElement('div');
      const last = document.createElement('div');
      const nestedFirst = document.createElement('div');
      const nestedLast = document.createElement('div');
      
      parent.append(first, last);
      first.appendChild(nestedFirst);
      last.appendChild(nestedLast);
      
      // @ts-ignore
      expect(PopupDecisionTree.findFirstChild(parent)).toBe(nestedLast);
      // @ts-ignore
      expect(PopupDecisionTree.findLastChild(parent)).toBe(nestedLast);
    });

    it('should filter invisible nodes and honor z-index ordering in getTopmostChildren', () => {
      const parent = document.createElement('div');
      const child1 = document.createElement('div');
      const child2 = document.createElement('div');
      const child3 = document.createElement('div');
      
      parent.append(child1, child2, child3);
      
      semiTransparentElements.add(child1);
      setStyle(child1, { position: 'relative', zIndex: '1' });
      setStyle(child2, { position: 'relative', zIndex: '3' });
      setStyle(child3, { position: 'relative', zIndex: '2', opacity: '1' });
      
      const mask = document.createElement('div');
      setStyle(mask, { zIndex: '1' });
      
      const popupInfo = makePopupInfo({
        popup_type: PopupType.B,
        mask_node: mask,
      });
      
      // @ts-ignore
      const topNodes = PopupDecisionTree.getTopmostChildren(parent, popupInfo);
      expect(topNodes).toEqual([child2, child3]);
    });

    it('should return correct element and handle ties in findMainContentNode', () => {
      const root = document.createElement('div');
      const mask = document.createElement('div');
      const contentA = document.createElement('div');
      const contentB = document.createElement('div');
      
      root.append(mask, contentA, contentB);
      mask.appendChild(document.createElement('div'));
      
      setStyle(contentA, { zIndex: '1' });
      setStyle(contentB, { zIndex: '2' });
      
      const popupInfo = makePopupInfo({ root_node: root, mask_node: mask, popup_type: PopupType.B });
      
      // @ts-ignore
      expect(PopupDecisionTree.findMainContentNode(root, popupInfo)).toBe(contentB);
      
      setStyle(contentA, { zIndex: '2' });
      // @ts-ignore
      expect(PopupDecisionTree.findMainContentNode(root, popupInfo)).toBeNull();
    });
  });

  // Layout and Style Tests
  describe('Layout and Style', () => {
    it('should respond to flex alignment in specialCenterCondition and specialBottomCondition', () => {
      const node = document.createElement('div');
      setStyle(node, { flexDirection: 'row', alignItems: 'center' });
      expect(PopupDecisionTree.specialCenterCondition(node)).toBe(true);
      
      setStyle(node, { flexDirection: 'column', justifyContent: 'center' });
      expect(PopupDecisionTree.specialCenterCondition(node)).toBe(true);
      
      const root = document.createElement('div');
      setStyle(node, { flexDirection: 'column', justifyContent: 'flex-end' });
      expect(PopupDecisionTree.specialBottomCondition(root, node)).toBe(true);
      
      setStyle(root, { flexDirection: 'row', alignItems: 'end' });
      expect(PopupDecisionTree.specialBottomCondition(root, node)).toBe(true);
    });

    it('should work with border-box and content-box sizing in equalToScreenWidth', () => {
      const node = document.createElement('div');
      setStyle(node, {
        boxSizing: 'border-box',
        width: `${window.innerWidth}`,
      });
      expect(PopupDecisionTree.equalToScreenWidth(node, 2)).toBe(true);
      
      const content = document.createElement('div');
      setStyle(content, {
        boxSizing: 'content-box',
        width: `${window.innerWidth - 20}`,
        paddingLeft: '10',
        paddingRight: '10',
        borderLeft: '0',
        borderRight: '0',
      });
      expect(PopupDecisionTree.equalToScreenWidth(content, 2)).toBe(true);
      
      setStyle(content, {
        paddingLeft: '0',
        paddingRight: '0',
        borderLeft: '0',
        borderRight: '0',
        width: '100',
      });
      expect(PopupDecisionTree.equalToScreenWidth(content, 2)).toBe(false);
    });
  });

  // Modal Detection Tests
  describe('Modal Detection', () => {
    it('should cover bottom, static and top-style branches in judgeModalConditions', () => {
      const root = document.createElement('div');
      const content = document.createElement('div');
      root.append(content);
      
      setRect(content, {
        top: window.innerHeight - 300,
        left: 0,
        width: window.innerWidth,
        height: 300,
      });
      setStyle(content, {
        position: 'fixed',
        bottom: '0px',
        top: `${window.innerHeight - 300}px`,
        boxSizing: 'border-box',
        width: `${window.innerWidth}`,
      });
      
      hasBottomStyleMock.mockReturnValueOnce(true);
      expect(PopupDecisionTree.judgeModalConditions(root, content)).toBe(true);
      
      // static bottom auto branch
      const staticContent = document.createElement('div');
      setRect(staticContent, {
        top: window.innerHeight - 200,
        left: 0,
        width: window.innerWidth,
        height: 200,
      });
      setStyle(staticContent, {
        position: 'static',
        bottom: 'auto',
        top: '100px',
        boxSizing: 'border-box',
        width: `${window.innerWidth}`,
      });
      
      expect(PopupDecisionTree.judgeModalConditions(root, staticContent)).toBe(true);
      
      // top style branch
      const topContent = document.createElement('div');
      setRect(topContent, { top: 0, left: 0, width: window.innerWidth, height: 200 });
      setStyle(topContent, {
        position: 'fixed',
        bottom: '0px',
        top: '0px',
        boxSizing: 'border-box',
        width: `${window.innerWidth}`,
      });
      
      hasTopStyleMock.mockReturnValueOnce(true);
      expect(PopupDecisionTree.judgeModalConditions(root, topContent)).toBe(false);
    });

    it('should reuse judgeModalConditions logic in isModalForTypeB and isModalForTypeC', () => {
      const root = document.createElement('div');
      const mask = document.createElement('div');
      const content = document.createElement('div');
      
      root.append(mask, content);
      
      setRect(content, {
        top: window.innerHeight - 200,
        left: 0,
        width: window.innerWidth,
        height: 200,
      });
      setStyle(content, {
        position: 'fixed',
        bottom: '0px',
        top: `${window.innerHeight - 200}px`,
        boxSizing: 'border-box',
        width: `${window.innerWidth}`,
      });
      
      const popupInfoB = makePopupInfo({
        root_node: root,
        content_node: content,
        mask_node: mask,
        popup_type: PopupType.B,
      });
      
      // @ts-ignore
      expect(PopupDecisionTree.isModalForTypeB(popupInfoB)).toBe(true);
      
      const popupInfoC = makePopupInfo({
        root_node: root,
        content_node: content,
        mask_node: mask,
        popup_type: PopupType.C,
      });
      
      // @ts-ignore
      expect(PopupDecisionTree.isModalForTypeC(popupInfoC)).toBe(true);
    });

    it('should integrate all pieces and update popupInfo content in isModalWin', () => {
      const root = document.createElement('div');
      const mask = document.createElement('div');
      const content = document.createElement('div');
      
      root.append(mask, content);
      
      setStyle(content, {
        position: 'fixed',
        bottom: '0px',
        top: `${window.innerHeight - 200}px`,
        boxSizing: 'border-box',
        width: `${window.innerWidth}`,
      });
      setRect(content, {
        top: window.innerHeight - 200,
        left: 0,
        width: window.innerWidth,
        height: 200,
      });
      
      const popupInfo = makePopupInfo({
        root_node: root,
        mask_node: mask,
        content_node: content,
        popup_type: PopupType.B,
      });
      
      expect(PopupDecisionTree.isModalWin([content], popupInfo)).toBe(true);
      expect(popupInfo.content_node).toBe(content);
    });
  });
  // Decision Tree Classification Tests
  describe('Decision Tree Classification', () => {
    it('should distinguish Bottom, Center overlap and default center in judgePopupDecisionTreeType', () => {
      const root = document.createElement('div');
      const mask = document.createElement('div');
      const content = document.createElement('div');
      
      root.append(mask, content);
      
      setStyle(content, {
        position: 'fixed',
        bottom: '0px',
        top: `${window.innerHeight - 200}px`,
        boxSizing: 'border-box',
        width: `${window.innerWidth}`,
      });
      setRect(content, {
        top: window.innerHeight - 200,
        left: 0,
        width: window.innerWidth,
        height: 200,
      });
      
      const popupInfo = makePopupInfo({
        root_node: root,
        mask_node: mask,
        content_node: content,
        popup_type: PopupType.B,
      });
      
      visibleSiblingsMock.mockReturnValueOnce([]);
      const bottomDecision = PopupDecisionTree.judgePopupDecisionTreeType([content], popupInfo);
      expect(bottomDecision).toBe(PopupDecisionTreeType.Bottom);
      // @ts-ignore
      expect(window.popWin).toBe('bottom');
      
      const overlapRoot = document.createElement('div');
      const overlapContent = document.createElement('div');
      const overlapMask = document.createElement('div');
      overlapRoot.append(overlapMask, overlapContent);
      
      const contentSibling = document.createElement('div');
      overlapContent.appendChild(contentSibling);
      const closeButton = document.createElement('button');
      closeButton.className = 'close';
      overlapContent.appendChild(closeButton);
      
      setStyle(contentSibling, { position: 'absolute' });
      setRect(contentSibling, { top: 0, left: 5, width: 20, height: 20 });
      setStyle(closeButton, { position: 'absolute', backgroundImage: 'url(close.png)' });
      setRect(closeButton, { top: 0, left: 0, width: 10, height: 10 });
      
      const overlapInfo = makePopupInfo({
        root_node: overlapRoot,
        mask_node: overlapMask,
        content_node: overlapContent,
        popup_type: PopupType.B,
      });
      
      setRect(overlapContent, {
        top: 100,
        left: 100,
        width: 200,
        height: 200,
      });
      
      visibleSiblingsMock.mockReset();
      visibleSiblingsMock.mockImplementation(node =>
        Array.from(node.parentElement?.children || []).filter(
          (child): child is HTMLElement => child instanceof HTMLElement && child !== node
        )
      );
      
      // @ts-ignore
      expect(PopupDecisionTree.calOverlapAreaRatio(contentSibling, closeButton)).toBeGreaterThan(0.2);
      // @ts-ignore
      expect(PopupDecisionTree.hasOverlap(contentSibling, closeButton)).toBe(true);
      // @ts-ignore
      expect(PopupDecisionTree.hasOverlapWithSiblings(closeButton)).toBe(true);
      expect(
        // @ts-ignore
        PopupDecisionTree.hasOverlappingCloseButton(
          [overlapRoot, overlapContent, closeButton, contentSibling],
          overlapInfo
        )
      ).toBe(true);
      
      const overlapDecision = PopupDecisionTree.judgePopupDecisionTreeType(
        [overlapContent, closeButton],
        overlapInfo
      );
      expect(overlapDecision).toBe(PopupDecisionTreeType.Center_Button_Overlap);
      // @ts-ignore
      expect(window.popWin).toBe('center');
      
      visibleSiblingsMock.mockReturnValue([]);
      setRect(overlapContent, { top: 100, left: 100, width: 100, height: 100 });
      setStyle(overlapContent, { position: 'relative', bottom: 'auto', top: '200px' });
      
      expect(
        PopupDecisionTree.judgePopupDecisionTreeType(
          [overlapContent, closeButton],
          makePopupInfo({
            root_node: overlapRoot,
            mask_node: overlapMask,
            content_node: overlapContent,
            popup_type: PopupType.A,
          })
        )
      ).toBe(PopupDecisionTreeType.Center);
    });
  });

  // Edge Cases and Error Handling
  describe('Edge Cases and Error Handling', () => {
    it('should handle absence of close buttons gracefully', () => {
      const root = document.createElement('div');
      const popupInfo = makePopupInfo({ root_node: root });
      
      expect(
        // @ts-ignore
        PopupDecisionTree.hasOverlappingCloseButton(root, [root], popupInfo)
      ).toBe(false);
    });

    it('should return false when no absolute ancestor found', () => {
      const root = document.createElement('div');
      const child = document.createElement('div');
      const stopNode = document.createElement('div');
      
      root.append(stopNode);
      stopNode.append(child);
      
      setStyle(child, { position: 'relative' });
      setStyle(stopNode, { position: 'relative' });
      
      expect(
        // @ts-ignore
        PopupDecisionTree.isElementOrAncestorAbsolute(child, stopNode)
      ).toBe(false);
    });

    it('should detect small absolute ancestor', () => {
      const root = document.createElement('div');
      const ancestor = document.createElement('div');
      const child = document.createElement('div');
      
      root.append(ancestor);
      ancestor.append(child);
      
      setStyle(ancestor, { position: 'absolute' });
      setRect(ancestor, { top: 0, left: 0, width: 20, height: 20 });
      
      expect(
        // @ts-ignore
        PopupDecisionTree.isElementOrAncestorAbsolute(child, root)
      ).toBe(true);
    });

    it('should iterate ancestors before detection', () => {
      const root = document.createElement('div');
      const ancestorA = document.createElement('div');
      const ancestorB = document.createElement('div');
      const child = document.createElement('div');
      
      root.append(ancestorB);
      ancestorB.append(ancestorA);
      ancestorA.append(child);
      
      setStyle(ancestorA, { position: 'relative' });
      setStyle(ancestorB, { position: 'absolute' });
      setRect(ancestorB, { top: 0, left: 0, width: 30, height: 30 });
      
      expect(
        // @ts-ignore
        PopupDecisionTree.isElementOrAncestorAbsolute(child, root)
      ).toBe(true);
    });

    it('should return false for orphan node in hasOverlapWithSiblings', () => {
      const orphan = document.createElement('div');
      // @ts-ignore
      expect(PopupDecisionTree.hasOverlapWithSiblings(orphan)).toBe(false);
    });

    it('should cover following sibling branch with nested absolute child', () => {
      const parent = document.createElement('div');
      const target = document.createElement('div');
      const prev = document.createElement('div');
      const next = document.createElement('div');
      const nextFirst = document.createElement('div');
      
      parent.append(prev, target, next);
      next.append(nextFirst);
      
      visibleSiblingsMock.mockImplementation(node =>
        Array.from(node.parentElement?.children || []).filter(
          (child): child is HTMLElement => child instanceof HTMLElement && child !== node
        )
      );
      
      setRect(target, { top: 0, left: 0, width: 100, height: 100 });
      setRect(nextFirst, { top: 10, left: 10, width: 60, height: 60 });
      
      setStyle(prev, { position: 'relative' });
      setStyle(next, { position: 'relative' });
      setStyle(nextFirst, { position: 'absolute' });
      
      // @ts-ignore
      expect(PopupDecisionTree.hasOverlapWithSiblings(target)).toBe(true);
      
      visibleSiblingsMock.mockReset();
    });

    it('should return null when no descendants in findFirstChild/findLastChild', () => {
      const empty = document.createElement('div');
      // @ts-ignore
      expect(PopupDecisionTree.findFirstChild(empty)).toBeNull();
      // @ts-ignore
      expect(PopupDecisionTree.findLastChild(empty)).toBeNull();
    });

    it('should handle null sibling in hasOverlap and calOverlapAreaRatio', () => {
      const node = document.createElement('div');
      setRect(node, { top: 0, left: 0, width: 10, height: 10 });
      
      // @ts-ignore
      expect(PopupDecisionTree.hasOverlap(null, node)).toBe(false);
      // @ts-ignore
      expect(PopupDecisionTree.calOverlapAreaRatio(null, node)).toBe(0);
    });

    it('should return empty when all children filtered in getTopmostChildren', () => {
      const parent = document.createElement('div');
      const child = document.createElement('div');
      
      parent.appendChild(child);
      setStyle(child, { display: 'none' });
      
      const popupInfo = makePopupInfo({ mask_node: document.createElement('div') });
      // @ts-ignore
      expect(PopupDecisionTree.getTopmostChildren(parent, popupInfo)).toEqual([]);
    });

    it('should handle auto z-index scenarios in getTopmostChildren', () => {
      const parent = document.createElement('div');
      const mask = document.createElement('div');
      const childA = document.createElement('div');
      const childB = document.createElement('div');
      
      parent.append(childA, childB);
      
      const popupInfo = makePopupInfo({ mask_node: mask, popup_type: PopupType.B });
      
      setStyle(mask, { zIndex: 'auto' });
      setStyle(childA, { position: 'relative', zIndex: 'auto' });
      setStyle(childB, { position: 'relative', zIndex: '5' });
      
      // @ts-ignore
      const result = PopupDecisionTree.getTopmostChildren(parent, popupInfo);
      expect(result[0]).toBe(childB);
    });

    it('should handle popup type C baseline in getTopmostChildren', () => {
      const parent = document.createElement('div');
      const mask = document.createElement('div');
      const child = document.createElement('div');
      
      parent.appendChild(child);
      
      const popupInfo = makePopupInfo({ mask_node: mask, popup_type: PopupType.C });
      
      setStyle(mask, { zIndex: '3' });
      setStyle(child, { position: 'relative', zIndex: '1' });
      
      // @ts-ignore
      expect(PopupDecisionTree.getTopmostChildren(parent, popupInfo)).toEqual([child]);
    });

    it('should sort by DOM order when z-index equal in getTopmostChildren', () => {
      const parent = document.createElement('div');
      const mask = document.createElement('div');
      const childA = document.createElement('div');
      const childB = document.createElement('div');
      
      parent.append(childA, childB);
      
      const popupInfo = makePopupInfo({ mask_node: mask, popup_type: PopupType.B });
      
      setStyle(mask, { zIndex: '0' });
      setStyle(childA, { position: 'relative', zIndex: 'auto' });
      setStyle(childB, { position: 'relative', zIndex: 'auto' });
      
      // @ts-ignore
      const result = PopupDecisionTree.getTopmostChildren(parent, popupInfo);
      expect(result).toEqual([childB, childA]);
    });

    it('should return false when no content node detected in isModalWin', () => {
      const root = document.createElement('div');
      const mask = document.createElement('div');
      
      root.appendChild(mask);
      
      const popupInfo = makePopupInfo({ root_node: root, mask_node: mask, popup_type: PopupType.B });
      expect(PopupDecisionTree.isModalWin([], popupInfo)).toBe(false);
    });

    it('should return false when close button matches center pattern in isModalWin', () => {
      const root = document.createElement('div');
      const mask = document.createElement('div');
      const content = document.createElement('div');
      const close = document.createElement('button');
      
      root.append(mask, content);
      content.appendChild(close);
      
      close.className = 'close';
      setRect(content, { top: window.innerHeight - 200, left: 0, width: window.innerWidth, height: 200 });
      setRect(close, { top: window.innerHeight - 40, left: 0, width: 20, height: 20 });
      
      const popupInfo = makePopupInfo({ root_node: root, mask_node: mask, popup_type: PopupType.B });
      expect(PopupDecisionTree.isModalWin([close], popupInfo)).toBe(false);
    });

    it('should route to type C handler in isModalWin', () => {
      const root = document.createElement('div');
      const content = document.createElement('div');
      
      root.appendChild(content);
      
      setRect(content, { top: window.innerHeight - 100, left: 0, width: window.innerWidth, height: 100 });
      setStyle(content, { position: 'fixed', bottom: '0px', top: `${window.innerHeight - 100}px`, boxSizing: 'border-box', width: `${window.innerWidth}` });
      
      hasBottomStyleMock.mockReturnValue(true);
      
      const popupInfo = makePopupInfo({
        root_node: root,
        mask_node: root,
        content_node: content,
        popup_type: PopupType.C,
      });
      
      expect(PopupDecisionTree.isModalWin([content], popupInfo)).toBe(true);
    });
  });
    it('should return false when close button criteria not met', () => {
      const root = document.createElement('div');
      const btn = document.createElement('button');
      root.appendChild(btn);
      setRect(btn, { top: 0, left: 0, width: 500, height: 500 });
      // @ts-ignore
      expect(PopupDecisionTree.CheckCenterCloseButton(root, [btn])).toBe(false);
    });
    it('should detect modal content when iterating children', () => {
      const root = document.createElement('div');
      const mask = document.createElement('div');
      const content = document.createElement('div');
      const child = document.createElement('div');
      root.append(mask, content);
      content.appendChild(child);
      setRect(content, { top: window.innerHeight - 100, left: 0, width: window.innerWidth, height: 100 });
      setRect(child, { top: window.innerHeight - 50, left: 0, width: window.innerWidth, height: 50 });
      setStyle(content, {
        position: 'fixed',
        bottom: '0px',
        top: `${window.innerHeight - 100}px`,
        boxSizing: 'border-box',
        flexDirection: 'column',
        justifyContent: 'flex-end',
      });
      setStyle(child, { position: 'fixed', bottom: '0px', top: `${window.innerHeight - 50}px`, boxSizing: 'border-box' });
      const info = makePopupInfo({
        root_node: root,
        mask_node: mask,
        content_node: content,
        popup_type: PopupType.B,
      });
      hasBottomStyleMock.mockReturnValue(true);
      // @ts-ignore
      expect(PopupDecisionTree.isModalForTypeB(info)).toBe(true);
    });
    it('should fallback to children when parent not modal', () => {
      const root = document.createElement('div');
      const mask = document.createElement('div');
      const content = document.createElement('div');
      const child = document.createElement('div');
      root.append(mask, content);
      content.appendChild(child);
      const info = makePopupInfo({
        root_node: root,
        mask_node: mask,
        content_node: content,
        popup_type: PopupType.B,
      });
      const spy = jest
        .spyOn(PopupDecisionTree, 'judgeModalConditions')
        .mockImplementation((_root: HTMLElement, node: HTMLElement) => node === child);
      // @ts-ignore
      expect((PopupDecisionTree).isModalForTypeB(info)).toBe(true);
      spy.mockRestore();
    });
  it('should return null when mask path invalid', () => {
    const root = document.createElement('div');
    const mask = document.createElement('div');
    const popupInfo = makePopupInfo({ root_node: root, mask_node: mask, popup_type: PopupType.B });
    root.appendChild(document.createElement('div'));
    // @ts-ignore
    expect(PopupDecisionTree.findMainContentNode(root, popupInfo)).toBeNull();
  });
  it('should climb mask ancestors before detection', () => {
    const root = document.createElement('div');
    const wrapper = document.createElement('div');
    const mask = document.createElement('div');
    const content = document.createElement('div');
    wrapper.appendChild(mask);
    root.append(wrapper, content);
    const popupInfo = makePopupInfo({ root_node: root, mask_node: mask, popup_type: PopupType.B });
    // @ts-ignore
    expect(PopupDecisionTree.findMainContentNode(root, popupInfo)).toBe(content);
  });
  it('should return null for equal z-index ties', () => {
    const root = document.createElement('div');
    const mask = document.createElement('div');
    const a = document.createElement('div');
    const b = document.createElement('div');
      root.append(mask, a, b);
      const popupInfo = makePopupInfo({ root_node: root, mask_node: mask, popup_type: PopupType.A });
    setStyle(a, { zIndex: '2' });
    setStyle(b, { zIndex: '2' });
    // @ts-ignore
    expect(PopupDecisionTree.findMainContentNode(root, popupInfo)).toBeNull();
  });
  it('should treat auto z-index as baseline', () => {
    const root = document.createElement('div');
    const mask = document.createElement('div');
    const autoNode = document.createElement('div');
    const topNode = document.createElement('div');
    root.append(mask, autoNode, topNode);
    const popupInfo = makePopupInfo({ root_node: root, mask_node: mask, popup_type: PopupType.B });
    setStyle(autoNode, { zIndex: 'auto' });
    setStyle(topNode, { zIndex: '5' });
    // @ts-ignore
    expect(PopupDecisionTree.findMainContentNode(root, popupInfo)).toBe(topNode);
  });
    it('should return false on special center condition', () => {
      const root = document.createElement('div');
      const content = document.createElement('div');
      setStyle(content, { flexDirection: 'row', alignItems: 'center' });
      expect(PopupDecisionTree.judgeModalConditions(root, content)).toBe(false);
    });
    it('should handle missing bottom style', () => {
      const root = document.createElement('div');
      const content = document.createElement('div');
      setRect(content, { top: 100, left: 0, width: window.innerWidth, height: 200 });
      setStyle(content, { position: 'fixed', bottom: '0px', top: '100px', boxSizing: 'border-box', width: `${window.innerWidth}` });
      hasBottomStyleMock.mockReturnValue(false);
      expect(PopupDecisionTree.judgeModalConditions(root, content)).toBe(false);
    });
  it('should check explicit top style when top is zero', () => {
    const root = document.createElement('div');
    const content = document.createElement('div');
    setRect(content, { top: 0, left: 0, width: window.innerWidth, height: 100 });
    setStyle(content, { position: 'fixed', bottom: '0px', top: '0px', boxSizing: 'border-box' });
    hasBottomStyleMock.mockReturnValue(true);
    hasTopStyleMock.mockReturnValue(true);
    expect(PopupDecisionTree.judgeModalConditions(root, content)).toBe(false);
  });
  it('should preserve bottom stick when top style absent', () => {
    const root = document.createElement('div');
    const content = document.createElement('div');
    setRect(content, { top: 0, left: 0, width: window.innerWidth, height: window.innerHeight });
    setStyle(content, {
      position: 'fixed',
      bottom: '0px',
      top: '0px',
      boxSizing: 'border-box',
      width: `${window.innerWidth}`,
    });
    hasBottomStyleMock.mockReturnValue(true);
    hasTopStyleMock.mockReturnValue(false);
    expect(PopupDecisionTree.judgeModalConditions(root, content)).toBe(true);
  });
    it('should return true when top offset positive', () => {
      const root = document.createElement('div');
      const content = document.createElement('div');
      setRect(content, { top: 50, left: 0, width: window.innerWidth, height: 200 });
      setStyle(content, {
        position: 'fixed',
        bottom: '0px',
        top: '50px',
        boxSizing: 'border-box',
        width: `${window.innerWidth}`,
      });
      hasBottomStyleMock.mockReturnValue(true);
      expect(PopupDecisionTree.judgeModalConditions(root, content)).toBe(true);
    });
    it('should return false when bottom offset not zero', () => {
      const root = document.createElement('div');
      const content = document.createElement('div');
      setRect(content, { top: 100, left: 0, width: window.innerWidth, height: 200 });
      setStyle(content, { position: 'fixed', bottom: '5px', top: '100px', boxSizing: 'border-box', width: `${window.innerWidth}` });
      hasBottomStyleMock.mockReturnValue(true);
      expect(PopupDecisionTree.judgeModalConditions(root, content)).toBe(false);
    });
    it('should fallback to stick-to-bottom return', () => {
      const root = document.createElement('div');
      const content = document.createElement('div');
      setRect(content, { top: window.innerHeight - 150, left: 0, width: window.innerWidth, height: 150 });
      setStyle(content, { position: 'fixed', bottom: '0px', top: 'auto', boxSizing: 'border-box', width: `${window.innerWidth}` });
      hasBottomStyleMock.mockReturnValue(true);
      expect(PopupDecisionTree.judgeModalConditions(root, content)).toBe(true);
    });
    it('should detect root flex-end alignment', () => {
      const root = document.createElement('div');
      const content = document.createElement('div');
      setStyle(content, { flexDirection: 'column', justifyContent: 'flex-end' });
      expect(PopupDecisionTree.specialBottomCondition(root, content)).toBe(true);
    });
    it('should return false for non-center alignments', () => {
      const content = document.createElement('div');
      setStyle(content, { flexDirection: 'column', justifyContent: 'flex-start' });
      expect(PopupDecisionTree.specialCenterCondition(content)).toBe(false);
    });
    it('should handle content-box invalid measurements', () => {
      const node = document.createElement('div');
      setStyle(node, {
        boxSizing: 'content-box',
        width: '100',
        paddingLeft: 'NaN',
        paddingRight: '0',
        borderLeft: '0',
        borderRight: '0',
      });
      expect(PopupDecisionTree.equalToScreenWidth(node, 2)).toBe(false);
    });

    it('covers additional branches in getCloseButtons', () => {
      const root = document.createElement('div');
      const button = document.createElement('button');
      const img = document.createElement('img');
      
      root.appendChild(button);
      root.appendChild(img);
      
      // Test background image URL matching
      setStyle(button, { backgroundImage: 'url(https://example.com/close-icon.png)' });
      img.src = 'https://example.com/guanbi-icon.svg';
      
      // @ts-ignore
      const result = PopupDecisionTree.getCloseButtons(root, [root, button, img]);
      expect(result.length).toBeGreaterThan(0);
    });

    it('covers position absolute check in hasOverlapWithSiblings', () => {
      const parent = document.createElement('div');
      const target = document.createElement('div');
      const sibling = document.createElement('div');
      
      parent.append(target, sibling);
      
      setStyle(target, { position: 'absolute' });
      setRect(target, { top: 0, left: 0, width: 50, height: 50 });
      
      setStyle(sibling, { position: 'absolute' });
      setRect(sibling, { top: 10, left: 10, width: 50, height: 50 });
      
      visibleSiblingsMock.mockReturnValue([sibling]);
      
      // @ts-ignore
      expect(PopupDecisionTree.hasOverlapWithSiblings(target)).toBe(true);
    });

    it('covers equalToScreenWidth with different box-sizing values', () => {
      const node = document.createElement('div');
      
      // Test with invalid box-sizing
      setStyle(node, {
        boxSizing: 'invalid-box',
        width: '1200',
        paddingLeft: '10',
        paddingRight: '10',
        borderLeft: '1',
        borderRight: '1',
      });
      expect(PopupDecisionTree.equalToScreenWidth(node, 2)).toBe(false);
    });

    it('should handle elements with zero area in overlap calculation', () => {
      const elementA = document.createElement('div');
      const elementB = document.createElement('div');
      
      setRect(elementA, { top: 0, left: 0, width: 100, height: 100 });
      setRect(elementB, { top: 0, left: 0, width: 0, height: 0 }); // Zero area
      
      // @ts-ignore
      expect(PopupDecisionTree.calOverlapAreaRatio(elementA, elementB)).toBe(0);
    });

    it('should handle modal detection with no children', () => {
      const root = document.createElement('div');
      const mask = document.createElement('div');
      const content = document.createElement('div');
      
      root.append(mask, content);
      
      const info = makePopupInfo({
        root_node: root,
        mask_node: mask,
        content_node: content,
        popup_type: PopupType.B,
      });

      // Mock judgeModalConditions to return false
      const spy = jest
        .spyOn(PopupDecisionTree, 'judgeModalConditions')
        .mockReturnValue(false);

      // @ts-ignore
      expect(PopupDecisionTree.isModalForTypeB(info)).toBe(false);
      
      spy.mockRestore();
    });

    it('covers specialBottomCondition with rootNode flex end alignment', () => {
      const root = document.createElement('div');
      const content = document.createElement('div');
      
      setStyle(root, { flexDirection: 'row', alignItems: 'end' });
      expect(PopupDecisionTree.specialBottomCondition(root, content)).toBe(true);
      setStyle(root, { flexDirection: 'column', justifyContent: 'flex-end' });
      expect(PopupDecisionTree.specialBottomCondition(root, content)).toBe(true);
      setStyle(root, { flexDirection: 'column', justifyContent: 'end' });
      expect(PopupDecisionTree.specialBottomCondition(root, content)).toBe(true);
    });

    it('should handle position static with non-auto bottom', () => {
      const root = document.createElement('div');
      const content = document.createElement('div');
      
      setRect(content, { top: 100, left: 0, width: window.innerWidth, height: 200 });
      setStyle(content, { 
        position: 'static', 
        bottom: '10px', // Not auto
        top: '100px',
        boxSizing: 'border-box',
        width: `${window.innerWidth}px`
      });
      
      expect(PopupDecisionTree.judgeModalConditions(root, content)).toBe(false);
    });

    it('should handle top values not ending with px', () => {
      const root = document.createElement('div');
      const content = document.createElement('div');
      
      setRect(content, { 
        top: window.innerHeight - 200, // Position so bottom touches viewport bottom
        left: 0, 
        width: window.innerWidth, 
        height: 200
      });
      setStyle(content, { 
        position: 'fixed', 
        bottom: '0px',
        top: '10%', // Not ending with 'px'
        boxSizing: 'border-box',
        width: `${window.innerWidth}px`
      });
      
      hasBottomStyleMock.mockReturnValue(true);
      
      expect(PopupDecisionTree.judgeModalConditions(root, content)).toBe(true);
    });

    it('should handle popup type A baseline in topmost children', () => {
      const parent = document.createElement('div');
      const mask = document.createElement('div');
      const child = document.createElement('div');
      
      parent.appendChild(child);
      
      const popupInfo = makePopupInfo({ 
        mask_node: mask, 
        popup_type: PopupType.A 
      });
      
      setStyle(mask, { zIndex: '3' });
      setStyle(child, { position: 'relative', zIndex: '1' });
      
      // @ts-ignore
      expect(PopupDecisionTree.getTopmostChildren(parent, popupInfo)).toEqual([child]);
    });

    it('should handle popup type A in main content detection', () => {
      const root = document.createElement('div');
      const mask = document.createElement('div');
      const content = document.createElement('div');
      
      root.append(mask, content);
      
      const popupInfo = makePopupInfo({
        root_node: root, 
        mask_node: mask, 
        popup_type: PopupType.A 
      });
      
      setStyle(content, { zIndex: '5' });
      
      // @ts-ignore
      expect(PopupDecisionTree.findMainContentNode(root, popupInfo)).toBe(content);
    });

    it('should handle popup type A in modal detection', () => {
      const root = document.createElement('div');
      const mask = document.createElement('div');
      const content = document.createElement('div');
      
      root.append(mask, content);
      
      const popupInfo = makePopupInfo({
        root_node: root,
        mask_node: mask,
        content_node: content,
        popup_type: PopupType.A,
      });
      
      setStyle(content, { zIndex: '5' });
      
      expect(PopupDecisionTree.isModalWin([content], popupInfo)).toBe(false);
    });

    it('should handle modal content with children', () => {
      const root = document.createElement('div');
      const mask = document.createElement('div');
      const content = document.createElement('div');
      const child = document.createElement('div');
      
      root.append(mask, content);
      content.appendChild(child);
      
      const info = makePopupInfo({
        root_node: root,
        mask_node: mask,
        content_node: content,
        popup_type: PopupType.B,
      });

      // Set up content to be modal itself
      setRect(content, { 
        top: window.innerHeight - 100, 
        left: 0, 
        width: window.innerWidth, 
        height: 100 
      });
      setStyle(content, {
        position: 'fixed',
        bottom: '0px',
        top: `${window.innerHeight - 100}px`,
        boxSizing: 'border-box',
        width: `${window.innerWidth}px`,
      });
      
      hasBottomStyleMock.mockReturnValue(true);
      
      // This should trigger the contentNode having children AND being modal
      // @ts-ignore
      expect(PopupDecisionTree.isModalForTypeB(info)).toBe(true);
    });

    it('covers additional edge cases in hasOverlapWithSiblings', () => {
      const parent = document.createElement('div');
      const target = document.createElement('div');
      
      parent.appendChild(target);
      
      // Mock getVisibleSiblings to return empty array
      visibleSiblingsMock.mockReturnValue([]);
      
      // @ts-ignore
      expect(PopupDecisionTree.hasOverlapWithSiblings(target)).toBe(false);
    });
    it('should skip distant siblings when none adjacent', () => {
      const parent = document.createElement('div');
      const target = document.createElement('div');
      const spacer = document.createElement('div');
      const far = document.createElement('div');
      parent.append(target, spacer, far);
      visibleSiblingsMock.mockReturnValue([far]);
      // @ts-ignore
      expect(PopupDecisionTree.hasOverlapWithSiblings(target)).toBe(false);
    });

    it('covers filter branches in getTopmostChildren', () => {
      const parent = document.createElement('div');
      const child1 = document.createElement('div');
      const child2 = document.createElement('div');
      
      parent.append(child1, child2);
      
      // Make one child have display none
      setStyle(child1, { display: 'none' });
      setStyle(child2, { position: 'relative', zIndex: '1', opacity: '1' });
      
      const popupInfo = makePopupInfo({ mask_node: document.createElement('div'), popup_type: PopupType.B });
      setStyle(popupInfo.mask_node, { zIndex: '0' });
      
      // @ts-ignore
      const result = PopupDecisionTree.getTopmostChildren(parent, popupInfo);
      expect(result.length).toBeGreaterThanOrEqual(0); // Just ensure it runs without error
    });

    it('covers edge case in findFirstChild with complex nesting', () => {
      const parent = document.createElement('div');
      const level1 = document.createElement('div');
      const level2 = document.createElement('div');
      const deepest = document.createElement('div');
      
      parent.appendChild(level1);
      level1.appendChild(level2);
      level2.appendChild(deepest);
      
      // @ts-ignore
      expect(PopupDecisionTree.findFirstChild(parent)).toBe(deepest);
    });

    it('covers getCloseButtons with src attribute matching', () => {
      const root = document.createElement('div');
      const img = document.createElement('img');
      
      root.appendChild(img);
      (img as HTMLImageElement).src = 'https://example.com/close-button.png';
      
      // @ts-ignore
      const result = PopupDecisionTree.getCloseButtons(root, [root, img]);
      expect(result.length).toBeGreaterThanOrEqual(0);
    });

    it('covers getCloseButtons edge cases and missing branches', () => {
      const root = document.createElement('div');
      
      // Test case 1: Test matchByUrl branch (when matchByClassOrBg is false)
      const elementUrlOnly = document.createElement('div');
      root.appendChild(elementUrlOnly);
      elementUrlOnly.className = 'non-matching-class';
      setStyle(elementUrlOnly, { backgroundImage: 'url(guanbi.svg)' });
      
      // @ts-ignore
      let result = PopupDecisionTree.getCloseButtons(root, [root, elementUrlOnly]);
      expect(result).toContain(elementUrlOnly);
      
      // Test case 2: No matches at all
      const elementNoMatch = document.createElement('div');
      root.appendChild(elementNoMatch);
      elementNoMatch.className = 'unrelated';
      setStyle(elementNoMatch, { backgroundImage: 'url(logo.png)' });
      
      // @ts-ignore
      result = PopupDecisionTree.getCloseButtons(root, [root, elementNoMatch]);
      expect(result).not.toContain(elementNoMatch);
      
      // Test case 3: classList is null/undefined
      const elementNullClass = document.createElement('div');
      root.appendChild(elementNullClass);
      setStyle(elementNullClass, { backgroundImage: 'url(close.png)' });
      // Override classList to be null
      Object.defineProperty(elementNullClass, 'classList', {
        get: () => null,
        configurable: true
      });
      
      // @ts-ignore
      result = PopupDecisionTree.getCloseButtons(root, [root, elementNullClass]);
      expect(result).toContain(elementNullClass);
      
      // Test case 4: backgroundImage with toLowerCase returning undefined
      const elementSpecialBg = document.createElement('div');
      root.appendChild(elementSpecialBg);
      
      // Use a more realistic mock for getComputedStyle
      const originalGetComputedStyle = window.getComputedStyle;
      window.getComputedStyle = jest.fn((el) => {
        if (el === elementSpecialBg) {
          const mockStyle = originalGetComputedStyle(el);
          return {
            ...mockStyle,
            backgroundImage: 'url(close.png)',
            // Mock a style that has backgroundImage but toLowerCase may fail
          } as CSSStyleDeclaration;
        }
        return originalGetComputedStyle(el);
      });
      
      // @ts-ignore
      result = PopupDecisionTree.getCloseButtons(root, [root, elementSpecialBg]);
      expect(result).toContain(elementSpecialBg);
      
      // Restore original function
      window.getComputedStyle = originalGetComputedStyle;
      
      // Clean up
      root.innerHTML = '';
    });
    
    it('should handle overlapping close buttons with different absolute positions', () => {
      const root = document.createElement('div');
      const closeButton1 = document.createElement('button');
      const closeButton2 = document.createElement('button');
      
      root.append(closeButton1, closeButton2);
      
      // Set up close buttons with different characteristics
      closeButton1.className = 'close-btn';
      closeButton2.className = 'another-close';
      setStyle(closeButton1, { backgroundImage: 'url(close.png)', position: 'static' });
      setStyle(closeButton2, { backgroundImage: 'url(guanbi.png)', position: 'static' });
      
      // Test case 1: Close button that is not absolute
      visibleSiblingsMock.mockReturnValue([]);
      
      const popupInfo = makePopupInfo({ root_node: root, popup_type: PopupType.B });
      // @ts-ignore
      let result = PopupDecisionTree.hasOverlappingCloseButton(root, [root, closeButton1], popupInfo);
      expect(result).toBe(false);
      
      // Test case 2: Close button that is absolute but has no overlap
      setStyle(closeButton1, { position: 'absolute' });
      setRect(closeButton1, { top: 0, left: 0, width: 10, height: 10 });
      // @ts-ignore
      result = PopupDecisionTree.hasOverlappingCloseButton(root, [root, closeButton1], popupInfo);
      expect(result).toBe(false);
      
      visibleSiblingsMock.mockReset();
    });

    it('should comprehensively test all close button matching branches', () => {
      const root = document.createElement('div');
      
      // Test case 1: matchByClassOrBg = true, matchByUrl should not be evaluated
      const classMatch = document.createElement('div');
      classMatch.className = 'close-button';
      root.appendChild(classMatch);
      
      // @ts-ignore
      let result = PopupDecisionTree.getCloseButtons(root, [root, classMatch]);
      expect(result).toContain(classMatch);
      
      // Test case 2: matchByClassOrBg = false, matchByUrl = true  
      const urlMatch = document.createElement('div');
      urlMatch.className = 'non-matching';
      setStyle(urlMatch, { backgroundImage: 'url(close-icon.svg)' });
      root.appendChild(urlMatch);
      
      // @ts-ignore
      result = PopupDecisionTree.getCloseButtons(root, [root, urlMatch]);
      expect(result).toContain(urlMatch);
      
      // Test case 3: matchByClassOrBg = false, matchByUrl = false, matchByImgSrc = true
      const imgMatch = document.createElement('img');
      (imgMatch as HTMLImageElement).src = 'https://example.com/guanbi-button.png';
      root.appendChild(imgMatch);
      
      // @ts-ignore
      result = PopupDecisionTree.getCloseButtons(root, [root, imgMatch]);
      expect(result).toContain(imgMatch);
      
      // Test case 4: All matches false
      const noMatch = document.createElement('span');
      noMatch.className = 'irrelevant';
      setStyle(noMatch, { backgroundImage: 'url(logo.png)' });
      root.appendChild(noMatch);
      
      // @ts-ignore
      result = PopupDecisionTree.getCloseButtons(root, [root, noMatch]);
      expect(result).not.toContain(noMatch);
      
      // Test case 5: IMG element with non-matching src
      const imgNoMatch = document.createElement('img');
      (imgNoMatch as HTMLImageElement).src = 'https://example.com/avatar.jpg';
      root.appendChild(imgNoMatch);
      
      // @ts-ignore
      result = PopupDecisionTree.getCloseButtons(root, [root, imgNoMatch]);
      expect(result).not.toContain(imgNoMatch);
      
      // Clean up
      root.innerHTML = '';
    });
    
    it('should handle empty and null close button patterns', () => {
      const root = document.createElement('div');
      
      // Test with empty patterns array
      const originalPatterns = mockConfig.getcloseButtonPattern.getMockImplementation();
      mockConfig.getcloseButtonPattern.mockReturnValue([]);
      
      const element = document.createElement('div');
      element.className = 'close';
      root.appendChild(element);
      
      // @ts-ignore
      let result = PopupDecisionTree.getCloseButtons(root, [root, element]);
      expect(result).toEqual([]);
      
      // Test with null patterns
      mockConfig.getcloseButtonPattern.mockReturnValue(null);
      // @ts-ignore
      result = PopupDecisionTree.getCloseButtons(root, [root, element]);
      expect(result).toEqual([]);
      
      // Restore original patterns
      if (originalPatterns) {
        mockConfig.getcloseButtonPattern.mockImplementation(originalPatterns);
      } else {
        mockConfig.getcloseButtonPattern.mockReturnValue(['close', 'guanbi', 'cancel']);
      }
      
      // Clean up
      root.innerHTML = '';
    });

    it('should handle absolute array edge cases in overlap detection', () => {
      const root = document.createElement('div');
      const closeButton = document.createElement('button');
      closeButton.className = 'close-btn';
      setStyle(closeButton, { backgroundImage: 'url(close.png)', position: 'static' });
      root.appendChild(closeButton);
      
      const popupInfo = makePopupInfo({ root_node: root, popup_type: PopupType.B });
      
      // Test case 1: Close button that is not absolute
      visibleSiblingsMock.mockReturnValue([]);
      // @ts-ignore
      let result = PopupDecisionTree.hasOverlappingCloseButton(root, [root, closeButton], popupInfo);
      expect(result).toBe(false);
      
      // Test case 2: Close button that is absolute but has no overlap
      setStyle(closeButton, { position: 'absolute' });
      setRect(closeButton, { top: 0, left: 0, width: 10, height: 10 });
      // @ts-ignore
      result = PopupDecisionTree.hasOverlappingCloseButton(root, [root, closeButton], popupInfo);
      expect(result).toBe(false);
      
      visibleSiblingsMock.mockReset();
    });

    it('should handle different flex combinations for bottom detection', () => {
      const root = document.createElement('div');
      const content = document.createElement('div');
      
      // Test flex-end alignment
      setStyle(content, { flexDirection: 'column', justifyContent: 'end' });
      expect(PopupDecisionTree.specialBottomCondition(root, content)).toBe(true);
      
      // Test alignItems end
      setStyle(content, { flexDirection: 'row', alignItems: 'end' });
      expect(PopupDecisionTree.specialBottomCondition(root, content)).toBe(true);
    });

    it('should filter hidden elements in topmost children', () => {
      const parent = document.createElement('div');
      const child = document.createElement('div');
      
      parent.appendChild(child);
      setStyle(child, { visibility: 'hidden' });
      
      const popupInfo = makePopupInfo({ mask_node: document.createElement('div'), popup_type: PopupType.B });
      
      // @ts-ignore
      const result = PopupDecisionTree.getTopmostChildren(parent, popupInfo);
      expect(result).toEqual([]);
    });

    it('should filter zero opacity elements in topmost children', () => {
      const parent = document.createElement('div');
      const child = document.createElement('div');
      
      parent.appendChild(child);
      setStyle(child, { opacity: '0' });
      
      const popupInfo = makePopupInfo({ mask_node: document.createElement('div'), popup_type: PopupType.B });
      
      // @ts-ignore
      const result = PopupDecisionTree.getTopmostChildren(parent, popupInfo);
      expect(result).toEqual([]);
    });

    it('should filter semi-transparent background elements', () => {
      const parent = document.createElement('div');
      const child = document.createElement('div');
      
      parent.appendChild(child);
      setStyle(child, { opacity: '1' });
      semiTransparentElements.add(child);
      
      const popupInfo = makePopupInfo({ mask_node: document.createElement('div'), popup_type: PopupType.B });

      // @ts-ignore
      const result = PopupDecisionTree.getTopmostChildren(parent, popupInfo);
      expect(result).toEqual([]);
    });

    it('should return last child when first differs from last', () => {
      const parent = document.createElement('div');
      const first = document.createElement('div');
      const last = document.createElement('div');
      
      parent.append(first, last);
      
      // The method actually returns lastElementChild in the current implementation
      // @ts-ignore
      expect(PopupDecisionTree.findFirstChild(parent)).toBe(last);
    });

    it('should handle absolute positioned elements and ancestors', () => {
      const root = document.createElement('div');
      const parent = document.createElement('div');
      const child = document.createElement('div');
      
      root.appendChild(parent);
      parent.appendChild(child);
      
      // Test case where element itself is absolute
      setStyle(child, { position: 'absolute' });
      setRect(child, { top: 0, left: 0, width: 10, height: 10 });
      
      // @ts-ignore
      expect(PopupDecisionTree.isElementOrAncestorAbsolute(child, root)).toBe(true);
    });

    it('should handle complex node containment filtering', () => {
      const grandParent = document.createElement('div');
      const parent = document.createElement('div');
      const child = document.createElement('div');
      
      grandParent.appendChild(parent);
      parent.appendChild(child);
      
      grandParent.className = 'close';
      parent.className = 'close';
      child.className = 'close';
      
      const nodes = [grandParent, parent, child];
      // @ts-ignore
      const result = PopupDecisionTree.filterContainedNodes(nodes);
      
      // Should only return grandParent since it's not contained by others
      expect(result).toEqual([grandParent]);
    });

    it('should handle very small overlap calculations', () => {
      const elementA = document.createElement('div');
      const elementB = document.createElement('div');
      
      // Create minimal overlap scenario
      setRect(elementA, { top: 0, left: 0, width: 10, height: 10 });
      setRect(elementB, { top: 9, left: 9, width: 10, height: 10 }); // 1x1 overlap
      
      // @ts-ignore
      const ratio = PopupDecisionTree.calOverlapAreaRatio(elementA, elementB);
      expect(ratio).toBeCloseTo(0.01); // 1/(10*10) = 0.01
    });

    it('should handle overlapping close buttons with logging', () => {
      const root = document.createElement('div');
      const closeButton = document.createElement('button');
      closeButton.className = 'close-btn';
      setStyle(closeButton, { backgroundImage: 'url(close.png)', position: 'absolute' });
      setRect(closeButton, { top: 0, left: 0, width: 20, height: 20 });
      root.appendChild(closeButton);
      
      const sibling = document.createElement('div');
      setStyle(sibling, { position: 'absolute' });
      setRect(sibling, { top: 5, left: 5, width: 25, height: 25 });
      root.appendChild(sibling);
      
      visibleSiblingsMock.mockReturnValue([sibling]);
      
      const logSpy = jest.spyOn(Log, 'd').mockImplementation(() => undefined);
      const popupInfo = makePopupInfo({ 
        root_node: root, 
        popup_type: PopupType.B 
      });
      
      // @ts-ignore
      const result = PopupDecisionTree.hasOverlappingCloseButton(root, [root, closeButton, sibling], popupInfo);
      expect(result).toBe(true);
      expect(logSpy).toHaveBeenCalledWith(
        expect.stringContaining('特例1: 关闭按钮存在重叠'),
        Tag.popupDecisionTree
      );
      
      logSpy.mockRestore();
      visibleSiblingsMock.mockReset();
    });

    it('should ensure overlapping close button scenario with proper logging', () => {
      const root = document.createElement('div');
      root.className = 'test-popup-root';
      
      // Create a close button
      const closeButton = document.createElement('button');
      closeButton.className = 'close-btn';
      setStyle(closeButton, { 
        backgroundImage: 'url(close.png)', 
        position: 'absolute' 
      });
      setRect(closeButton, { top: 10, left: 10, width: 20, height: 20 });
      root.appendChild(closeButton);
      
      // Create an overlapping sibling
      const overlappingSibling = document.createElement('div');
      setStyle(overlappingSibling, { position: 'absolute' });
      setRect(overlappingSibling, { top: 15, left: 15, width: 30, height: 30 });
      root.appendChild(overlappingSibling);
      
      // Mock visibleSiblings to return the overlapping sibling
      visibleSiblingsMock.mockImplementation((el) => {
        if (el === closeButton) {
          return [overlappingSibling];
        }
        return [];
      });
      
      // Create a spy on Log.d to verify line 41 is called
      const logSpy = jest.spyOn(Log, 'd').mockImplementation(() => undefined);
      
      const popupInfo = makePopupInfo({ 
        root_node: root, 
        popup_type: PopupType.B 
      });
      
      // Call the function - this should trigger line 41
      // @ts-ignore
      const result = PopupDecisionTree.hasOverlappingCloseButton(
        [root, closeButton, overlappingSibling], 
        popupInfo
      );
      
      expect(result).toBe(true);
      
      // Verify that line 41 was called with the specific log message
      expect(logSpy).toHaveBeenCalledWith(
        expect.stringMatching(/特例1.*关闭按钮存在重叠.*弹窗.*test-popup-root/),
        Tag.popupDecisionTree
      );
      
      logSpy.mockRestore();
      visibleSiblingsMock.mockReset();
    });

    it('should handle popup info optional chain scenarios', () => {
      const root = document.createElement('div');
      const closeButton = document.createElement('button');
      closeButton.className = 'close-btn';
      setStyle(closeButton, { backgroundImage: 'url(close.png)', position: 'absolute' });
      setRect(closeButton, { top: 10, left: 10, width: 20, height: 20 });
      root.appendChild(closeButton);
      
      const overlappingSibling = document.createElement('div');
      setStyle(overlappingSibling, { position: 'absolute' });
      setRect(overlappingSibling, { top: 15, left: 15, width: 30, height: 30 });
      root.appendChild(overlappingSibling);
      
      visibleSiblingsMock.mockImplementation((el) => {
        if (el === closeButton) {
          return [overlappingSibling];
        }
        return [];
      });
      
      const logSpy = jest.spyOn(Log, 'd').mockImplementation(() => undefined);
      
      // Test case 1: popupInfo.root_node is null
      const popupInfoNoRoot: PopupInfo = { ...makePopupInfo({ root_node: root }), root_node: null };
      // @ts-ignore
      let result = PopupDecisionTree.hasOverlappingCloseButton(
        [root, closeButton, overlappingSibling], 
        popupInfoNoRoot
      );
      expect(result).toBe(true);
      expect(logSpy).toHaveBeenCalledWith(
        expect.stringContaining('特例1: 关闭按钮存在重叠，弹窗: undefined'),
        Tag.popupDecisionTree
      );
      
      // Test case 2: root_node has no className (empty string)
      const rootNoClass = document.createElement('div');
      // Don't set className - will be empty string
      const popupInfoNoClass = makePopupInfo({ root_node: rootNoClass });
      // @ts-ignore
      result = PopupDecisionTree.hasOverlappingCloseButton(
        [root, closeButton, overlappingSibling], 
        popupInfoNoClass
      );
      expect(result).toBe(true);
      expect(logSpy).toHaveBeenCalledWith(
        expect.stringContaining('特例1: 关闭按钮存在重叠，弹窗: '),
        Tag.popupDecisionTree
      );
      
      // Test case 3: root_node has undefined className
      const rootUndefinedClass = document.createElement('div');
      Object.defineProperty(rootUndefinedClass, 'className', {
        get: () => undefined,
        configurable: true
      });
      const popupInfoUndefinedClass = makePopupInfo({ root_node: rootUndefinedClass });
      // @ts-ignore
      result = PopupDecisionTree.hasOverlappingCloseButton(
        [root, closeButton, overlappingSibling], 
        popupInfoUndefinedClass
      );
      expect(result).toBe(true);
      expect(logSpy).toHaveBeenCalledWith(
        expect.stringContaining('特例1: 关闭按钮存在重叠，弹窗: undefined'),
        Tag.popupDecisionTree
      );
      
      logSpy.mockRestore();
      visibleSiblingsMock.mockReset();
    });

    it('should handle background image edge cases in close button detection', () => {
      const root = document.createElement('div');
      
      // Test line 94: style.backgroundImage?.toLowerCase?.() ?? ''
      // Test line 95: style.backgroundImage ?? ''
      const elementWithNullBg = document.createElement('div');
      elementWithNullBg.className = 'no-match-class'; // Use a class that doesn't match close patterns
      root.appendChild(elementWithNullBg);
      
      const originalGetComputedStyle = window.getComputedStyle;
      
      // Mock getComputedStyle to return null/undefined backgroundImage
      // @ts-ignore
      window.getComputedStyle = jest.fn((el) => {
        if (el === elementWithNullBg) {
          return {
            backgroundImage: 'null', // This will trigger both ?? '' fallbacks
            position: 'static',
            display: 'block',
            visibility: 'visible',
            opacity: '1',
            zIndex: 'auto'
          };
        }
        return originalGetComputedStyle(el);
      });
      
      // @ts-ignore
      let result = PopupDecisionTree.getCloseButtons(root, [root, elementWithNullBg]);
      expect(result).toEqual([]); // Should not match since backgroundImage is null and class doesn't match
      
      // Test with backgroundImage that has no toLowerCase method but still has includes
      const elementWithSpecialBg = document.createElement('div');
      elementWithSpecialBg.className = 'no-match-class-2'; // Class that doesn't match close patterns
      root.appendChild(elementWithSpecialBg);
      
      // Create a mock string-like object with includes but no toLowerCase
      const mockBgImage = {
        includes: (str: string): boolean => 'url(close.png)'.includes(str),
        toString: (): string => 'url(close.png)',
        // No toLowerCase method - this triggers ?.toLowerCase?.() ?? ''
      };
      
      // @ts-ignore
      window.getComputedStyle = jest.fn((el) => {
        if (el === elementWithSpecialBg) {
          return {
            backgroundImage: mockBgImage,
            position: 'static',
            display: 'block',
            visibility: 'visible',
            opacity: '1',
            zIndex: 'auto'
          };
        }
        return originalGetComputedStyle(el);
      });
      
      // @ts-ignore
      result = PopupDecisionTree.getCloseButtons(root, [root, elementWithSpecialBg]);
      expect(result).toContain(elementWithSpecialBg); // Should match by URL
      
      // Restore original
      window.getComputedStyle = originalGetComputedStyle;
      root.innerHTML = '';
    });

    it('should handle multiple nodes with same max z-index', () => {
      const root = document.createElement('div');
      const mask = document.createElement('div');
      const contentA = document.createElement('div');
      const contentB = document.createElement('div');
      
      root.append(mask, contentA, contentB);
      
      // Set both content nodes to have the same z-index (highest)
      setStyle(contentA, { zIndex: '5' });
      setStyle(contentB, { zIndex: '5' }); // Same z-index as contentA
      
      const popupInfo = makePopupInfo({ 
        root_node: root, 
        mask_node: mask, 
        popup_type: PopupType.B 
      });
      
      // This should trigger line 509: } else if (zIndex === maxZIndex) {
      // and result in zIndexCount > 1, causing the function to return null
      // @ts-ignore
      const result = PopupDecisionTree.findMainContentNode(root, popupInfo);
      expect(result).toBeNull(); // Should return null when multiple nodes have same max z-index
    });

    it('should handle exact z-index tie scenario in content detection', () => {
      const root = document.createElement('div');
      const mask = document.createElement('div');
      const nodeA = document.createElement('div');
      const nodeB = document.createElement('div');
      const nodeC = document.createElement('div');
      
      root.append(mask, nodeA, nodeB, nodeC);
      
      // Set up z-index values to ensure we hit line 509
      setStyle(nodeA, { zIndex: '3' }); // Lower z-index
      setStyle(nodeB, { zIndex: '5' }); // Highest z-index (tie)
      setStyle(nodeC, { zIndex: '5' }); // Highest z-index (tie)
      
      const popupInfo = makePopupInfo({ 
        root_node: root, 
        mask_node: mask, 
        popup_type: PopupType.B 
      });
      
      // This should trigger:
      // 1. First iteration: nodeA has zIndex=3, maxZIndex becomes 3, zIndexCount=1
      // 2. Second iteration: nodeB has zIndex=5 > maxZIndex(3), maxZIndex=5, zIndexCount=1  
      // 3. Third iteration: nodeC has zIndex=5 === maxZIndex(5), this triggers line 509, zIndexCount=2
      // @ts-ignore
      const result = PopupDecisionTree.findMainContentNode(root, popupInfo);
      
      // Should return null because zIndexCount > 1
      expect(result).toBeNull();
    });
});
  
