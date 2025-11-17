/**
 * 统一的日志 TAG 管理类
 * 所有模块的 TAG 定义都应该在这里集中管理
 */
export default class Tag {
    // Framework 相关
    static framework = 'Framework';
    static intelligentLayout = 'IntelligentLayout';
    static observerHandler = 'ObserverHandler';
    static resizeObserver = 'ResizeObserver';
    static modifyObserver = 'ModifyObserver';
    static pageContentObserver = 'PageContentObserver';
    static throttle = 'Throttle';
    
    // Popup 相关
    static popupDecisionTree = 'PopupDecisionTree';
    static popupRecog = 'PopupRecog';
    static popupDetector = 'PopupDetector';
    static popupRelayout = 'PopupRelayout';
    static popupStateManager = 'PopupStateManager';
    
    // Layout 相关
    static layoutConstraintDetector = 'LayoutConstraintDetector';
    static layoutUtils = 'LayoutUtils';
    
    // Common 相关
    static util = 'Utils';
    static ccmConfig = 'CCMConfig';
    
    // Style 相关
    static styleSetter = 'StyleSetter';
    static styleGetter = 'StyleGetter';
    static styleCleaner = 'StyleCleaner';
    static cssSheetManage = 'CSSSheetManage';
    static domReader = 'DomReader';
    static originStyleCache = 'OriginStyleCache';
    
    // Data 相关
    static store = 'Store';
    static record = 'Record';
    static diffRecorder = 'DiffRecorder';
    
    // Font 相关
    static zoomFont = 'ZoomFont';
    
    // Debug 辅助 TAG
    static performance = 'Performance';
    static visual = 'Visual';
    static state = 'State';
    static cache = 'Cache';
    static debug = 'Debug';
    static component = 'Component';
    static node = 'Node';
}
