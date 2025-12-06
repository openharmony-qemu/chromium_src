import Log from '../../../Debug/Log';
import Tag from '../../../Debug/Tag';
import { HwTag, LayoutKey, LayoutValue } from '../Constant';
import CacheStyleGetter from '../Style/Common/CacheStyleGetter';

export default class Store {
    static valueMap = new Map<string, WeakMap<HTMLElement, string>>();
    static nextRoundCache: HTMLElement[] = [];
    static setValue(ele: HTMLElement, key: string, value: string): void {
        if (!ele) {
            return;
        }
        Log.i(ele, 'setValue key: {' + key + '}' + ' value {' + value + '}', Tag.store);
        let tempMap = this.valueMap.get(key);
        if (!tempMap) {
            tempMap = new Map<HTMLElement, string>();
            this.valueMap.set(key, tempMap);
        }
        tempMap.set(ele, value);

        Log.showValue(ele, key, value);

        // 这段代码移动到其他模块会导致循环引用
        Store.saveOriginHeight(ele, key);
    }

    static removeValue(ele: HTMLElement, key: string): void {
        if (!ele) {
            return;
        }
        Log.i(ele, 'removeValue key: {' + key + '}', Tag.store);

        const tempMap = this.valueMap.get(key);
        if (!tempMap) {
            return;
        }
        tempMap.delete(ele);

        Log.deleteValue(ele, key);
    }

    static getValue(ele: HTMLElement, key: string): string {
        if (!ele) {
            return null;
        }

        const tempMap = this.valueMap.get(key);
        if (!tempMap) {
            return null;
        }

        const value = tempMap.get(ele);
        if (!value) {
            return null;
        }
        return value;
    }

    static clearTag(key: string): void {
        Log.clearTag(this.valueMap, key);

        this.valueMap.delete(key);
    }

    static clearAllTag(ele: HTMLElement, isDelete: boolean): void {
        const persistentTags = [
            HwTag.ORIGIN_WIDTH,
            HwTag.ELE_NUM,
            HwTag.HEIGHT_0,
            HwTag.MULTI_IMGS,
            HwTag.HEIGHT_0,
            LayoutValue.HAS_TEXT_CHILD,
            LayoutValue.IS_IMG,
            LayoutKey.DIFF_ELE,
        ];
        const persistentValue: Map<string, string> = new Map().set(LayoutKey.CONFIG_LAYOUT_TAG, LayoutValue.DISABLE);

        for (let [k, map] of this.valueMap) {
            if (!isDelete && persistentTags.includes(k)) {
                continue;
            }

            const perValue = persistentValue.get(k);
            if (!isDelete && perValue && Store.getValue(ele, k) === perValue) {
                continue;
            }
            map.delete(ele);
            Log.deleteValue(ele, k);
        }
    }

    static removeNextRoundTags(): void {
        Store.nextRoundCache.forEach((ele) => {
            Store.removeValue(ele, LayoutKey.LAYOUT_TAG);
        });

        Store.nextRoundCache = [];
    }

    private static saveOriginHeight(ele: HTMLElement, key: string): void {
        if (key === LayoutKey.LAYOUT_TAG && Store.getValue(ele, HwTag.ORIGIN_HEIGHT)) {
            Store.setValue(ele, HwTag.ORIGIN_HEIGHT, CacheStyleGetter.offsetH(ele).toString());
        }
    }
}
