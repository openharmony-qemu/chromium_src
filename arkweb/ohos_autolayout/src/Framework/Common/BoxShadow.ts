/**
 * @interface BoxShadow
 * @description 解析后的 box-shadow 属性结构
 */
export interface BoxShadow {
    /** X轴偏移量 */
    offsetX: string;
    /** Y轴偏移量 */
    offsetY: string;
    /** 模糊半径 */
    blurRadius: string;
    /** 扩展半径 */
    spreadRadius: string;
    /** 阴影颜色 */
    color: string;
  }