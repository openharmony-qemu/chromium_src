import Log from '../../../../Debug/Log';
import Utils from '../../Utils/Utils';
import StyleCommon from '../Common/StyleCommon';
import Tag from '../../../../Debug/Tag';
import Cached from '../../Cached';
import Store from '../../Utils/Store';
import CacheStyleGetter from '../Common/CacheStyleGetter';
import Constant, { HwTag, LayoutKey, LayoutValue } from '../../Constant';

export default class StyleSetter {
    static insertRuleSelectorArr: [string, string[], string][] = [];

    // 当前状态假如有元素和设置的选择器匹配，则通过forceCSS设置的优先
    // 但是由于这个方法和flush绑定 所以会有flush的时候不匹配但是，下一个瞬间匹配但是不flush的情况
    // 所以设置的时候可以把所以的状态都通过forcecss insert rule的方法覆盖到
    static queryInsertRuleEle(): void {
        for (let [selector, attrs] of this.insertRuleSelectorArr) {
            const eles = document.querySelectorAll(selector);
            for (let i = 0; i < eles.length; i++) {
                const ele = eles[i] as HTMLElement;

                for (let j = 0; j < attrs.length; j++) {
                    const attr = attrs[j];
                }
            }
        }
    }

    static forceCSSAppendStyle: HTMLStyleElement;
    static appendRule(selector: string, cssList: [string, string][]): void {
        if (!this.forceCSSAppendStyle) {
        }

        let cssStr = '';
        let keys = [];
        for (let i = 0; i < cssList.length; i++) {
            const item = cssList[i];
            keys.push(item[0]);
            cssStr += `${item[0]}: ${item[1]}!important;`;
        }

        const index = this.insertRuleSelectorArr.findIndex((item: [string, string[], string]) => {
            const [oldSelector] = item;

            return oldSelector === selector;
        });

        if (index !== -1) {
            this.forceCSSAppendStyle.sheet.deleteRule(index);
            this.insertRuleSelectorArr.splice(index, 1);
        }

        this.insertRuleSelectorArr.push([selector, keys, cssStr]);

        this.forceCSSAppendStyle.sheet.insertRule(`${selector}{${cssStr}} `, this.insertRuleSelectorArr.length - 1);
    }

    private static deleteList: ChildNode[] = [];
    static observeList: HTMLElement[] = [];

    // 添加ele节点的缓存样式
    static setStyle(ele: HTMLElement, key: string, value: string): void {
        // 检查本次设置的样式是否已被强制设置
        if (this.hasForceCSS(ele, key)) {
            Log.i(ele, `${key} 已做强制更改`, Tag.styleSetter);
            return;
        }
        Log.debug(ele, `设置样式 ${key}: ${value}`, Tag.styleSetter);
        const eleStyleMap = StyleCommon.styleCache.get(ele);
        if (!eleStyleMap) {
            StyleCommon.styleCache.set(ele, new Map().set(key, value));
        } else {
            eleStyleMap.set(key, value);
        }
    }

    static setStyleByInsertRule(ele: HTMLElement, key: string, value: string): void {
        if (this.hasForceCSS(ele, key)) {
            Log.i(ele, key + '已做强制更改', Tag.styleSetter);
            return;
        }


        let selector = `[class='${ele.className}']`;

        if (ele.style.position === Constant.fixed) {
            selector += '[style*="position: fixed;"]';
        }

        if (ele.style.top) {
            selector += `[style*='top: ${ele.style.top};']`;
        }

        if (ele.id) {
            selector = `#${ele.id}` + selector;
        }

        let indexValue;
        if (!ele.getAttribute(LayoutKey.INSERT_RULE)) {
            indexValue = 'LayoutKey.INSERT_RULE' + StyleCommon.uniqueVirIdx++;
        } else {
            indexValue = ele.getAttribute(LayoutKey.INSERT_RULE);
        }

        ele.setAttribute(LayoutKey.INSERT_RULE, indexValue);

        selector += `[${LayoutKey.INSERT_RULE}='${indexValue}']`;

        ele.setAttribute(LayoutKey.INSERT_RULE, indexValue);
        this.appendRule(selector, [[key, value]]);
    }

    // 查看指定元素的指定样式是否已被强制修改
    private static hasForceCSS(ele: HTMLElement, key: string): boolean {
        const forceCSS = Store.getValue(ele, HwTag.FORCE_CSS);
        if (!forceCSS) {
            return false;
        }
        const forceCSSList = forceCSS.split(',');
        return forceCSSList.includes(key);
    }

    // 将缓存样式刷新至元素上，触发回流
    static flushAllStyles(): void {
        if (StyleCommon.styleDiv.parentElement !== document.head) {
            StyleCommon.styleDiv = document.head.appendChild(<Node>StyleCommon.styleDiv) as HTMLStyleElement;
        }

        this.queryInsertRuleEle();

        this.flushRemove();
        this.flushCacheToCSSSheet();
        // 若hw_a等相关属性被删除，重新设置
        this.checkAttr();
        StyleCommon.styleCache.clear();
    }

    private static flushRemove(): void {
        for (let i = 0; i < StyleSetter.deleteList.length; i++) {
            // 使用parent.removeChild会出现parent为空的情况
            StyleSetter.deleteList[i].remove();
        }
        StyleSetter.deleteList = [];
    }

    static removeEle(ele: ChildNode): void  {
        if (!ele) {
            return;
        }
        StyleSetter.deleteList.push(ele);
    }

    // 刷新当前缓存样式
    private static flushCacheToCSSSheet(): void  {
        for (const [ele, style] of StyleCommon.styleCache.entries()) {
            if(!ele) {
                continue;
            }
            let styleFlushArr = StyleCommon.styleFlushed.get(ele);
            // 删除样式
            if (style.delete(Constant.delete)) {
                if (styleFlushArr) {
                    const index = styleFlushArr[StyleCommon.INDEX_IDX];
                    this.deleteOldStyle(ele, index);
                    styleFlushArr = undefined;
                }
                if (!style.size) {
                    continue;
                }
            }
            // StoreMap中没有该ele信息，新增StoreMap的键值对，并刷新样式至CSSSheet
            if (!styleFlushArr) {
                styleFlushArr = [StyleCommon.uniqueVirIdx, new Map()];
                StyleCommon.styleFlushed.set(ele, styleFlushArr);
                StyleCommon.uniqueVirIdx++;
            }
            // 判断CacheMap中该ele的样式是否与StoreMap中的一致，若一致则不处理
            if (Utils.MapAContainsMapB(styleFlushArr[StyleCommon.STYLE_MAP_IDX], style)) {
                Log.i(ele, 'Cache样式与Store一致，不处理', Tag.styleSetter);
                continue;
            }

            // 刷新Cache样式至Store，并将全部样式设置至CSSSheet
            const mergedStyle: Map<string, string> = StyleSetter.mergeStyle(
                style,
                styleFlushArr[StyleCommon.STYLE_MAP_IDX]
            );
            const index = styleFlushArr[StyleCommon.INDEX_IDX];

            StyleCommon.styleFlushed.set(ele, [index, mergedStyle]);
            this.setStoreStyleToCSSSheet(ele, index, mergedStyle);
        }
    }

    private static mergeStyle(styleCache: Map<string, string>, styleFlush: Map<string, string>): Map<string, string> {
        for (let [attr, value] of styleCache.entries()) {
            if (value === Constant.delete) {
                styleFlush.delete(attr);
            } else {
                styleFlush.set(attr, value);
            }
        }
        return styleFlush;
    }

    // 删除ele在storeMap和CSSSheet中的原样式
    private static deleteOldStyle(ele: HTMLElement, index: number): void {
        StyleCommon.styleFlushed.delete(ele);
        StyleSetter.delAttr(ele);
        // 从indexMappingList中获取CSSRule对应的序号
        const ruleIndex = StyleSetter.findRuleIdxInList(index);
        if (ruleIndex !== -1) {
            StyleCommon.styleDiv.sheet.deleteRule(ruleIndex);
            StyleSetter.delRuleIdxInList(ruleIndex);
        }
    }

    private static setStoreStyleToCSSSheet(ele: HTMLElement, index: number, styleMap: Map<string, string>): void {
        const rulesStr = StyleSetter.getRulesStr(styleMap);
        const attrSelect = StyleSetter.getSelectStr(ele, index);

        StyleSetter.setAttr(ele, index);
        let ruleIdx = StyleSetter.findRuleIdxInList(index);

        if (ruleIdx !== -1) {
            // 该index已有对应的Rule，删除旧规则，添加新规则
            StyleCommon.styleDiv.sheet.deleteRule(ruleIdx);
        } else {
            // 该index需添加新规则，插入
            ruleIdx = StyleSetter.addRuleIdxInList(index);
        }
        // 新增样式表的元素：元素增加对应类名
        StyleCommon.styleDiv.sheet.insertRule(attrSelect + '{' + rulesStr + '}', ruleIdx);

        Log.i(ele, rulesStr, Tag.styleSetter);
    }

    // 获取CSS样式表规则字符串
    private static getRulesStr(styleMap: Map<string, string>): string {
        let rulesStr = '';
        for (const [attr, attrValue] of styleMap) {
            rulesStr += attr + ':' + attrValue + ' ' + Constant.important + ';\n';
        }
        return rulesStr;
    }

    /**
     * id选择器+属性选择器
     * @param ele 元素
     * @param index
     */
    private static getSelectStr(ele: HTMLElement, index: number): string {
        const attrSelect = StyleSetter.getAttrSelectStr(index);
        const idSelectStr = StyleSetter.getIdSelect(ele, attrSelect);

        return idSelectStr + attrSelect;
    }

    private static getAttrSelectStr(index: number): string {
        let selectStr = '';
        const attrVal = Constant.a_ + index;

        for (let i = 0; i < Constant.ATTR_NUM; i++) {
            const attrKey = StyleSetter.generateAttrKey(i);
            selectStr += `[${attrKey}=${attrVal}]`;
        }
        return selectStr;
    }

    /**
     * 最终字符串格式例如： #aaa #bbb #ccc[a=a1][b=a1],
     */
    private static getIdSelect(ele: HTMLElement, attrSelect: string): string {
        let idSlcStr = '';
        // 若元素本身/祖先元素有id，则构造id后代属性选择器，与属性选择器并集; id不能以数字开头，否则选择器会报错
        const regID = /^[a-zA-Z_.-][0-9a-zA-Z_.-]*$/g;
        if (regID.test(ele.id)) {
            idSlcStr += `#${ele.id}`.replace(/\./g, '\\.');
        }

        ele = ele.parentElement;
        while (ele && ele !== Cached.body_()) {
            if (regID.test(ele.id)) {
                idSlcStr = `#${ele.id}`.replace(/\./g, '\\.') + ' ' + idSlcStr;
            }
            ele = ele.parentElement;
        }
        if (idSlcStr) {
            idSlcStr += attrSelect + ',';
        }
        return idSlcStr;
    }

    private static setAttr(ele: HTMLElement, index: number): void {
        const attrVal = Constant.a_ + index;

        for (let i = 0; i < Constant.ATTR_NUM; i++) {
            const attrKey = StyleSetter.generateAttrKey(i);
            ele.setAttribute(attrKey, attrVal);
        }
    }

    static delAttr(ele: HTMLElement): void {
        for (let i = 0; i < Constant.ATTR_NUM; i++) {
            const attrKey = StyleSetter.generateAttrKey(i);
            ele.removeAttribute(attrKey);
        }
    }

    private static checkAttr(): void {
        for (let i = 0; i < this.observeList.length; i++) {
            const ele = this.observeList[i];
            const flushedMsg = StyleCommon.styleFlushed.get(ele);
            if (flushedMsg) {
                this.setAttr(ele, flushedMsg[StyleCommon.INDEX_IDX]);
            }
        }
        this.observeList = [];
    }

    // 返回映射表最后一个元素的下标（即当前eleIdx对应的序号）
    private static addRuleIdxInList(eleIdx: number): number {
        StyleCommon.indexMappingList.push(eleIdx);
        return StyleCommon.indexMappingList.length - 1;
    }

    private static findRuleIdxInList(eleIdx: number): number {
        return StyleCommon.indexMappingList.findIndex((x) => x === eleIdx);
    }

    private static delRuleIdxInList(ruleIdx: number): void {
        if (ruleIdx < 0 || ruleIdx >= StyleCommon.indexMappingList.length) {
            return;
        }
        StyleCommon.indexMappingList.splice(ruleIdx, 1);
    }

    static disableHandle(ele: HTMLElement): void {
        Store.setValue(ele, LayoutKey.CONFIG_LAYOUT_TAG, LayoutValue.DISABLE);
        Utils.setParentTag(ele.parentElement, LayoutKey.LAYOUT_TAG, LayoutValue.ZOOM_PARENT);
        Utils.setParentTag(ele.parentElement, LayoutKey.CONFIG_LAYOUT_TAG, LayoutValue.ZOOM_PARENT);
    }

    static checkAndSetStyle(ele: HTMLElement, style: string, value: string): void {
        if (!ele) {
            return;
        }

        const currentValue = CacheStyleGetter.computedStyle(ele, style);
        // 当前元素已经被网页本身设置过style且不为空值 或 已经设置过目标value时，不再设置value
        if (currentValue === null || currentValue === Constant.num0px) {
            StyleSetter.setStyle(ele, style, value);
        }
    }

    private static generateAttrKey(attrKey: number): string {
        return Constant.hw_ + String.fromCharCode(attrKey + Constant.a_.charCodeAt(0));
    }
}
