
/**
 * 用于描述一个数值范围，包含最小值和最大值
 */
export interface CCMRange {
    min: number;
    max: number;
  }
  
  /**
   * 用于描述响应式布局的断点规则
   */
  export interface Breakpoint {
    widthRange: CCMRange;
    aspectRatioRange: CCMRange;
  }
  
  /**
   * 用于描述 CSS 样式的键值对结构
   */
  export interface CssPattern {
    property: string;
    value: string;
  }
  
  // ===================================
  //  1. 接口定义
  // ===================================
  export interface ICCMConfig {
    /**
     * 蒙版最小屏占比阈值
     * 例如: 100% 对应的值是 1
     */
    minMaskAreaRatioThreshold: number;
  
    /**
     * 透明度筛选范围
     * 一个包含最小值和最大值的元组, 例如: [0, 1]
     */
    opacityFilter: [number, number];
  
    /**
     * 弹窗内容节点最小屏占比阈值
     * 例如: 15% 对应的值是 0.15
     */
    minContentAreaRatioThreshold: number;
  
    /**
     * 弹窗内容节点滚动特征值 (用于识别可滚动区域)
     * 包含多个关键词，忽略大小写
     */
    scrollNodePattern: string[];
  
    /**
     * 关闭按钮特征值 (用于识别关闭按钮)
     * 包含多个关键词，忽略大小写
     */
    closeButtonPattern: string[];
  
    /**
     * 特殊关闭按钮特征值 (用于识别关闭按钮)
     * 包含多个关键词
     */
    specialCloseButtonPattern: string[];
  
    /**
     * 按钮特征值 (用于识别按钮)
     * 包含多个关键词，忽略大小写
     */
    buttonPattern: string[];
  
    /**
     * 弹窗贴底特征值 (JSON 结构)
     */
    SemiModalBottomPattern: CssPattern;
  
    /**
     * 弹窗缩放动画时长 (单位: 毫秒)
     */
    scaleAnimationDuration: number;
  
    /**
     * 弹窗动画检测间隔 (单位: 毫秒)
     */
    animationCheckInterval: number;
  
    /**
     * 不同产品的窗口缩放系数
     * 键是产品名，值是包含最小和最大缩放值的元组
     */
    desScale: {
      HOPE: [number, number];
      VERDE: [number, number];
      ICL: [number, number];
      GRE: [number, number];
      // 可以根据需要添加更多产品
      [key: string]: [number, number];
    };
  
    /**
     * 响应式断点配置
     * 每个对象定义了一个宽度范围及其对应的宽高比范围
     */
    breakpoints: Breakpoint[];
  
  }
  
  // ===================================
  //  2. CCM 默认配置
  // ===================================
  
  const defaultCCMConfig: ICCMConfig = {
   minMaskAreaRatioThreshold: 1,
    opacityFilter: [0, 1],
    minContentAreaRatioThreshold: 0.15,
    scrollNodePattern: ["srcoll", "List", "swiper"],
    closeButtonPattern: ["close", "guanbi", "dele"],
    // 无法根据关键字识别的关闭按钮
    specialCloseButtonPattern: ["https://gw.alicdn.com/imgextra/i3/O1CN01UTLFnj1Fwrca4IXRL_!!6000000000552-2-tps-48-48.png", 
                                "https://m.hellobike.com/resource/aliss/bike/gENrrooPRRSKS2nmR644j.png", 
                                "https://gw.alicdn.com/imgextra/i3/O1CN01KKAlCs258LnUjyD30_!!6000000007481-2-tps-72-72.png", 
                                "https://bjyoss.oss-cn-hangzhou.aliyuncs.com/image/ali/pearl/peal_new/zzs_jzbg.png",
                                "https://fmy90.oss-cn-beijing.aliyuncs.com/h5/2022/achievement/colse.png",
                                "https://images.qmai.cn/s16/images/2019/06/04/046c63cc3b568864.png",
                                "https://oss.huizustore.com/92c65a788007401fa393a3b22b430099.png"],
    buttonPattern: ["button", 'btn'],
    SemiModalBottomPattern: { property: 'bottom', value: '0px' },
    scaleAnimationDuration: 100,
    animationCheckInterval: 200,
    desScale: {
      HOPE: [0.65, 1.2],
      VERDE: [0.65, 1.2],
      ICL: [0.6, 1.1],
      GRE: [0.5, 1.1],
    },
    breakpoints: [
      { widthRange: { min: 320, max: 500 }, aspectRatioRange: { min: 0.61, max: 1.63 } },
      { widthRange: { min: 660, max: 730 }, aspectRatioRange: { min: 0.89, max: 2.0 } },
      { widthRange: { min: 1000, max: 1150 }, aspectRatioRange: { min: 1.3, max: 1.45 } }
    ]
  };
  
  // ===================================
  //  3. CCMConfig 类定义
  // ===================================
  
  export class CCMConfig {
  // 静态私有实例，用于保存唯一的实例
    private static instance: CCMConfig;
  
    // 私有属性，用于存储配置数据
    private _minMaskAreaRatioThreshold: number;
    private _opacityFilter: [number, number];
    private _minContentAreaRatioThreshold: number;
    private _scrollNodePattern: string[];
    private _closeButtonPattern: string[];
    private _specialCloseButtonPattern: string[];
    private _buttonPattern: string[];
    private _SemiModalBottomPattern: CssPattern;
    private _scaleAnimationDuration: number;
    private _animationCheckInterval: number;
    private _desScale: { [key: string]: [number, number] };
    private _breakpoints: Breakpoint[];
  
    /**
     * 构造函数，用于初始化 ProductConfig 实例
     * @param data - 符合 IProductConfigData 接口的数据对象
     */
    constructor(data: ICCMConfig) {
      this._minMaskAreaRatioThreshold = data.minMaskAreaRatioThreshold;
      this._opacityFilter = data.opacityFilter;
      this._minContentAreaRatioThreshold = data.minContentAreaRatioThreshold;
      this._scrollNodePattern = data.scrollNodePattern;
      this._closeButtonPattern = data.closeButtonPattern;
      this._specialCloseButtonPattern = data.specialCloseButtonPattern;
      this._buttonPattern = data.buttonPattern;
      this._SemiModalBottomPattern = data.SemiModalBottomPattern;
      this._scaleAnimationDuration = data.scaleAnimationDuration;
      this._animationCheckInterval = data.animationCheckInterval;
      this._desScale = data.desScale;
      this._breakpoints = data.breakpoints;
    }
  
    // --- Getters and Setters ---
  
    public getMinMaskAreaRatioThreshold(): number {
      return this._minMaskAreaRatioThreshold;
    }
    public setMinMaskAreaRatioThreshold(value: number): void {
      this._minMaskAreaRatioThreshold = value;
    }
  
    public getOpacityFilter(): [number, number] {
      return this._opacityFilter;
    }
    public setOpacityFilter(value: [number, number]): void {
      this._opacityFilter = value;
    }
  
    public getMinContentAreaRatioThreshold(): number {
      return this._minContentAreaRatioThreshold;
    }
    public setMinContentAreaRatioThreshold(value: number): void {
      this._minContentAreaRatioThreshold = value;
    }
    
    public getscrollNodePattern(): string[] {
      return this._scrollNodePattern;
    }
    public setscrollNodePattern(value: string[]): void {
      this._scrollNodePattern = value;
    }
  
    public getcloseButtonPattern(): string[] {
      return this._closeButtonPattern;
    }
    public setcloseButtonPattern(value: string[]): void {
      this._closeButtonPattern = value;
    }
  
    public getSpecialCloseButtonPattern(): string[] {
      return this._specialCloseButtonPattern;
    }
    public setSpecialCloseButtonPattern(value: string[]): void {
      this._specialCloseButtonPattern = value;
    }
  
    public getButtonPattern(): string[] {
      return this._buttonPattern;
    }
    public setButtonPattern(value: string[]): void {
      this._buttonPattern = value;
    }
  
    public getSemiModalBottomPattern(): CssPattern {
      return this._SemiModalBottomPattern;
    }
    public setSemiModalBottomPattern(value: CssPattern): void {
      this._SemiModalBottomPattern = value;
    }
  
    public getScaleAnimationDuration(): number {
      return this._scaleAnimationDuration;
    }
    public setScaleAnimationDuration(value: number): void {
      this._scaleAnimationDuration = value;
    }
  
    public getAnimationCheckInterval(): number {
      return this._animationCheckInterval;
    }
    public setAnimationCheckInterval(value: number): void {
      this._animationCheckInterval = value;
    }
  
    public getDesScale(): { [key: string]: [number, number] } {
      return this._desScale;
    }
    public setDesScale(value: { [key: string]: [number, number] }): void {
      this._desScale = value;
    }
  
    public setBreakpoints(value: Breakpoint[]): void {
      this._breakpoints = value;
    }
  
  
    /**
     * 根据输入的宽度值，在断点配置中查找匹配的宽高比范围
     * @param width - 当前的宽度值
     * @returns 如果找到匹配的范围，则返回 Range 对象；否则返回 undefined
     */
    public findAspectRatioRange(width: number): CCMRange | undefined {
      for (const breakpoint of this._breakpoints) {
        if (width >= breakpoint.widthRange.min && width <= breakpoint.widthRange.max) {
          return breakpoint.aspectRatioRange;
        }
      }
      return undefined; // 没有找到匹配的断点
    }
  
    // --- JSON 解析函数 ---
      /**
       * 使用新的数据对象更新实例的属性
       * @param data - 包含新配置的 IProductConfigData 对象
       */
      public update(data: ICCMConfig): void {
          this._minMaskAreaRatioThreshold = data.minMaskAreaRatioThreshold;
          this._opacityFilter = data.opacityFilter;
          this._minContentAreaRatioThreshold = data.minContentAreaRatioThreshold;
          this._scrollNodePattern = data.scrollNodePattern;
          this._closeButtonPattern = data.closeButtonPattern;
          this._specialCloseButtonPattern = data.specialCloseButtonPattern;
          this._buttonPattern = data.buttonPattern;
          this._SemiModalBottomPattern = data.SemiModalBottomPattern;
          this._scaleAnimationDuration = data.scaleAnimationDuration;
          this._animationCheckInterval = data.animationCheckInterval;
          this._desScale = data.desScale;
          this._breakpoints = data.breakpoints;
      }
  
    /**
     * (静态方法) 解析 JSON 字符串并创建一个 ProductConfig 实例
     * @param jsonString - 包含配置数据的 JSON 字符串
     * @returns 返回一个新的 ProductConfig 实例
     * @throws 如果 JSON 格式错误，则会抛出异常
     */
    public static fromJson(jsonString: string): CCMConfig {
      try {
        const dataObject: ICCMConfig = JSON.parse(jsonString);
        const instance = CCMConfig.getInstance(); // 获取或创建实例
        instance.update(dataObject); // 使用新数据更新实例
        return instance;
      } catch (error) {
        console.error("Failed to parse ProductConfig from JSON:", error);
        throw new Error("Invalid JSON format for ProductConfig.");
      }
    }
  
      /**
     * 静态方法，用于获取类的唯一实例。
     * 如果实例不存在，则使用默认配置创建它。
     * @returns ProductConfig 的单例
     */
    public static getInstance(): CCMConfig {
      if (!CCMConfig.instance) {
        CCMConfig.instance = new CCMConfig(defaultCCMConfig);
      }
      return CCMConfig.instance;
    }
  }