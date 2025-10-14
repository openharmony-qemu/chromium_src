export default class StyleCommon {
    static INDEX_IDX = 0;
    static STYLE_MAP_IDX = 1;
    // styleCache<ele: HTMLElement, Map<attr: string, attrValue: string>>
    static styleCache = new Map();
    // styleStore<ele: HTMLElement, [index: number, Map<attr: string, attrValue: string>]>
    static styleFlushed = new Map();
    // 递增，唯一
    static uniqueVirIdx = 0;
    // uniqueVirIdx与CSSRule序号的映射表；List的下标对应CSSRule序号，内容为uniqueVirIdx
    static indexMappingList: number[] = [];
    // 自建自维护的CSS样式表（两张表）
    static styleDiv: HTMLStyleElement = null;
}
