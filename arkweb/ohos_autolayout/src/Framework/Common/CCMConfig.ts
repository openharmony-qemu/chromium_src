import Log from '../../Debug/Log';
import Tag from '../../Debug/Tag';

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

export interface AppRuleInfo {
  'id': string;
  'pg': string[];
  'strategy' ?: number;
  'alphabetIdentificationMinSize' ?: number;
  'alphabetHeightWidthMinRatio' ?: number;
}

export enum AutoLayoutStrategyType {
  popupScale = 1,
  alphabetNavigator = 1 << 1
}

export enum CheckRuleStateResult {
  skipCheck = 0,
  initial = 1,
  inWhiteList = 2,
  outOfWhiteList = 3
}

export interface ICCMConfigBase {
  /**
   * 蒙版最小屏占比阈值
   * 例如: 100% 对应的值是 100
   */
  minMaskAreaRatioThreshold: number;

  /**
   * 透明度筛选范围
   * 一个包含最小值和最大值的元组, 例如: [0, 100] 表示透明度在 0% 到 100% 之间
   */
  opacityFilter: [number, number];

  /**
   * 弹窗内容节点最小屏占比阈值
   * 例如: 15% 对应的值是 15
   */
  minContentAreaRatioThreshold: number;

  /**
   * 弹窗缩放动画时长 (单位: 毫秒)，50到400之间
   */
  scaleAnimationDuration: number;

  /**
   * 不同产品的窗口缩放系数
   * 例如: 55 对应的值是 55%
   */
  minScaleFactor: number;

  /**
   * 识别为字母导航条所需最长单调递增字串长度的最小值
   * 即最长单调递增字串长度 > alphabetIdentificationMinSize 才判断为字母导航条
   */
  alphabetIdentificationMinSize?: number;

  /**
   * 识别为字母导航条元素高宽比的最小值
   * 即元素 高 > 宽 * alphabetHeightWidthMinRatio 才判断为字母导航条
   */
  alphabetHeightWidthMinRatio?: number;

  /**
   * 是否需要校验id和page，为true时为小程序逻辑，为false时为H5网页url校验
   */
  needCheckIdAndPage: boolean;

  /**
   * 自动调整布局策略
   * 0x1表示弹窗缩放， 0x2表示字母导航条截断修复，0x3 = 0x1 | 0x2
   */
  strategy?: number;

  /**
   * 小程序白名单
   */
  appRuleInfos: AppRuleInfo[];
}

export interface ICCMConfig extends ICCMConfigBase {
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

  buttonPattern: string[];

  /* 吸顶吸底元素最小高度，默认值5
  */
  minSARTofStickyComponent:number,

  /* 吸顶吸底元素最大高度，默认值45
  */
  maxSARTofStickyComponent:number,

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
  opacityFilter: [0, 100],
  minContentAreaRatioThreshold: 15,
  scrollNodePattern: ['srcoll', 'List', 'swiper'],
  closeButtonPattern: ['close', 'guanbi', 'dele'],
  buttonPattern: ['button', 'btn'],
  scaleAnimationDuration: 100,
  minScaleFactor: 55,
  breakpoints: [
    { widthRange: { min: 320, max: 500 }, aspectRatioRange: { min: 0.61, max: 1.63 } },
    { widthRange: { min: 580, max: 950 }, aspectRatioRange: { min: 0.7, max: 2.0 } },
    { widthRange: { min: 1000, max: 1150 }, aspectRatioRange: { min: 1.3, max: 1.45 } }
  ],
  appRuleInfos: [{'id':'000','pg':['home']}],
  needCheckIdAndPage: false,
  strategy: 3,
  minSARTofStickyComponent:5,
  maxSARTofStickyComponent:45,
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
  private _buttonPattern: string[];
  private _scaleAnimationDuration: number;
  private _minScaleFactor: number;
  private _alphabetIdentificationMinSize: number;
  private _alphabetHeightWidthMinRatio: number;
  private _breakpoints: Breakpoint[];
  private _appRuleInfos:AppRuleInfo[];
  private _needCheckIdAndPage:boolean;

  private _minSARTofStickyComponent:number;
  private _maxSARTofStickyComponent:number;

  private appId:string;
  private page:string;

  private checkRuleStateResult:CheckRuleStateResult;

  // 默认是弹窗缩放
  private strategy: number = AutoLayoutStrategyType.popupScale;
  // 记录是否已执行字母导航条修复
  private isAlphabetNavigatorFixExecuted: boolean = false;

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
    this._buttonPattern = data.buttonPattern;
    this._scaleAnimationDuration = data.scaleAnimationDuration;
    this._minScaleFactor = data.minScaleFactor;
    this._alphabetIdentificationMinSize = data.alphabetIdentificationMinSize;
    this._alphabetHeightWidthMinRatio = data.alphabetHeightWidthMinRatio;
    this._breakpoints = data.breakpoints;
    this._appRuleInfos = data.appRuleInfos;
    this._needCheckIdAndPage = data.needCheckIdAndPage;
    // 若needCheckIdAndPage为false，跳过id和page的校验
    if (data.needCheckIdAndPage) {
      this.checkRuleStateResult = CheckRuleStateResult.initial;
    } else {
      this.checkRuleStateResult = CheckRuleStateResult.skipCheck;
    }
    this.appId = '';
    this.page = '';
    this._minSARTofStickyComponent = data.minSARTofStickyComponent;
    this._maxSARTofStickyComponent = data.maxSARTofStickyComponent;
  }

  public getMinMaskAreaRatioThreshold(): number {
    return this._minMaskAreaRatioThreshold;
  }

  public getMinSARTofStickyComponent(): number {
    return this._minSARTofStickyComponent;
  }

  public getMaxSARTofStickyComponent(): number {
    return this._maxSARTofStickyComponent;
  } 

  public getOpacityFilter(): [number, number] {
    return this._opacityFilter;
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

  public getcloseButtonPattern(): string[] {
    return this._closeButtonPattern;
  }

  public getButtonPattern(): string[] {
    return this._buttonPattern;
  }

  public getScaleAnimationDuration(): number {
    return this._scaleAnimationDuration;
  }

  public getMinScaleFactor(): number {
    return this._minScaleFactor;
  }

  public getAppID(): string {
    return this.appId;
  }

  public getPage(): string {
    return this.page;
  }

  public getAutoLayoutStrategyType(): number {
    return this.strategy;
  }

  public getAlphabetIdentificationMinSize(): number {
    return this._alphabetIdentificationMinSize;
  }

  public getAlphabetHeightWidthMinRatio(): number {
    return this._alphabetHeightWidthMinRatio;
  }

  public setAlphabetNavigatorFixExecuted(isExecuted: boolean): void {
    this.isAlphabetNavigatorFixExecuted = isExecuted;
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

  /**
   * 使用新的数据对象更新实例的属性
   * @param data - 包含新配置的 IProductConfigData 对象
   */
  public update(data: ICCMConfigBase): void {
      this._minMaskAreaRatioThreshold = data.minMaskAreaRatioThreshold;
      this._opacityFilter = data.opacityFilter;
      this._minContentAreaRatioThreshold = data.minContentAreaRatioThreshold;
      this._scaleAnimationDuration = data.scaleAnimationDuration;
      this._minScaleFactor = data.minScaleFactor ;
      this._alphabetIdentificationMinSize = data.alphabetIdentificationMinSize;
      this._alphabetHeightWidthMinRatio = data.alphabetHeightWidthMinRatio;
      this._appRuleInfos = typeof data.appRuleInfos === 'string' ? JSON.parse(data.appRuleInfos) : data.appRuleInfos;
      this._needCheckIdAndPage = data.needCheckIdAndPage;
  }

  /**
   * (静态方法) 解析 JSON 字符串并创建一个 ProductConfig 实例
   * @param jsonString - 包含配置数据的 JSON 字符串
   * @returns 返回一个新的 ProductConfig 实例
   * @throws 如果 JSON 格式错误，则会抛出异常
   */
  fromJson(jsonString: string): boolean {
    try {
      const dataObject: ICCMConfigBase = JSON.parse(jsonString);
      this.update(dataObject);
    } catch (error) {
      Log.e('Failed to parse ProductConfig from JSON', Tag.ccmConfig, error as Error);
      return false;
    }
    return true
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

  /**
   * 匹配任何一条 AppRuleInfo 规则,匹配一下三种情况
   * case 1: 未能获取Appid和page，周期性监听
   * case 2：获取appid 和page，检查结果是在白名单之外，关闭整个特性
   * case 3：获取appid和page，检查结果在白名单内，下次无需检查
   * @returns 如果找到匹配的规则，则返回 true，否则返回 false。
   */
  public checkRule(): CheckRuleStateResult {
    if (this.checkRuleStateResult === CheckRuleStateResult.skipCheck) {
      Log.info('H5网页修复，跳过id和page校验', Tag.ccmConfig);
    }
    if(this.checkRuleStateResult !== CheckRuleStateResult.initial) {
      return this.checkRuleStateResult;
    }
    // 要检查的App ID。
    // @ts-ignore
    const idAsString = String(typeof window._getAppId_ === 'function' ? window._getAppId_() : '');

    // @ts-ignore
    const pg:string = String(typeof window._getPage_ === 'function' ? window._getPage_() : '');
    if(idAsString && idAsString !== '' && pg && pg !== '') {
      this.appId = idAsString;
      this.page = pg;
    } else {
      Log.info('未能获取Appid和page，周期性监听', Tag.ccmConfig);
      return CheckRuleStateResult.initial;
    }

    // 遍历所有规则
    for (const rule of this._appRuleInfos) {
      // 检查 id 是否匹配
      // 匹配条件：规则 id 是 '*' 或与输入的 id 字符串相等
      const isIdMatch = rule.id === '*' || rule.id === idAsString;

      // 检查 pg 是否匹配
      // 匹配条件：规则 pg 数组包含 '*' 或包含输入的 pg 字符串
      const isPgMatch = rule.pg.includes('*') || rule.pg.includes(pg);

      // 如果 id 和 pg 同时匹配，则表示规则匹配成功
      if (isIdMatch && isPgMatch) {
        // 立即返回 true，因为已经找到了一个匹配项
        this.checkRuleStateResult = CheckRuleStateResult.inWhiteList;
        Log.info('id, page 检查成功，查询策略...', Tag.ccmConfig);
        if (rule.strategy) {
          this.strategy = rule.strategy;
          Log.info(`当前页面策略为：${this.strategy}`, Tag.ccmConfig);
        }
        if (rule.alphabetIdentificationMinSize) {
          this._alphabetIdentificationMinSize = rule.alphabetIdentificationMinSize;
        }
        if (rule.alphabetHeightWidthMinRatio) {
          this._alphabetHeightWidthMinRatio = rule.alphabetHeightWidthMinRatio;
        }
        return this.checkRuleStateResult;
      }
    }
    this.checkRuleStateResult = CheckRuleStateResult.outOfWhiteList;
    return this.checkRuleStateResult;
  }

  /**
   * 判断是否应用弹窗缩放策略
   * @returns 是否应用弹窗缩放策略
   */
  public isPopupScaleEnable(strategy: number): boolean {
    return (strategy & AutoLayoutStrategyType.popupScale) > 0;
  }
  
  /**
   * 判断是否应用字母导航条修复策略
   * @returns 是否应用字母导航条修复策略
   */
  public isAlphabetNavigatorEnable(strategy: number): boolean {
    let strategyEnable: boolean = (strategy & AutoLayoutStrategyType.alphabetNavigator) > 0;
    return strategyEnable && !this.isAlphabetNavigatorFixExecuted;
  }
}