import Log from '../../Debug/Log';
import Tag from '../../Debug/Tag';
import StyleSetter from '../Style/Setter/StyleSetter';
import { LayoutKey, LayoutValue } from '../Constant';
import Store from '../Utils/Store';

const CHANGE_COUNT_IDX = 0;
const UPPER_LIMIT = 3;
const STATE_IDX = 1;
const OBSERVER_CONT_LIMIT = 100;
const COUNT_MAP_SIZE_LIMIT = 100;

export const enum recordType {
    ATTR = 0,
    TRANSITION,
    IMGLOAD,
    ADD,
    CHARACTER,
}

export class ObserverRecord {
    // 记录触发元素 Map<Observer触发类型, Set<元素>>
    private static changeEleCache: Map<recordType, Set<HTMLElement>> = new Map();
    // 记录元素触发但无变化的次数 Map<Observer触发类型, Map<元素, 触发次数>>
    private static ignoreUselessChangeEle: Map<recordType, Map<HTMLElement, number>> = new Map();

    static ignoreChange(ele: HTMLElement, observerType: recordType): boolean {
        // AddObserver的changeEle为父元素（可能为NULL，需判断），其他都是触发元素本身
        const changeEle = ObserverRecord.getChangeEle(ele, observerType);
        if (!changeEle) {
            return true;
        }
        // 过滤多次触发变动但无重排的元素
        if (ObserverRecord.checkCount(changeEle, observerType)) {
            return true;
        }

        ObserverRecord.setChangeEleCache(changeEle, observerType);

        return false;
    }

    // 清除保存的changeEle计数
    static clearChangeCount(): void {
        for (let [observerType, changeEleSet] of ObserverRecord.changeEleCache.entries()) {
            // 找到当前Observer类型对应的次数
            let changeEleCountMap: Map<HTMLElement, recordType> =
                ObserverRecord.ignoreUselessChangeEle.get(observerType);
            if (!changeEleCountMap) {
                continue;
            }
            for (let changeEle of changeEleSet.values()) {
                changeEleCountMap.delete(changeEle);
            }
            if (changeEleCountMap.size === 0) {
                ObserverRecord.ignoreUselessChangeEle.delete(observerType);
            }
        }
        ObserverRecord.changeEleCache.clear();
    }

    // 保存的changeEle计数+1
    static addChangeCount(): void {
        for (let [observerType, changeEleSet] of ObserverRecord.changeEleCache.entries()) {
            // 找到变动元素在某Observer类型触发下的对应次数
            let changeEleCountMap: Map<HTMLElement, recordType> =
                ObserverRecord.ignoreUselessChangeEle.get(observerType);
            if (!changeEleCountMap) {
                changeEleCountMap = new Map();
                ObserverRecord.ignoreUselessChangeEle.set(observerType, changeEleCountMap);
            }

            for (let changeEle of changeEleSet.values()) {
                let count = changeEleCountMap.get(changeEle);
                count = count ? count : 0;
                changeEleCountMap.set(changeEle, count + 1);
            }
        }
        ObserverRecord.changeEleCache.clear();
    }

    private static getChangeEle(ele: HTMLElement, observerType: recordType): HTMLElement {
        if (observerType === recordType.ADD) {
            return ele.parentElement;
        }
        return ele;
    }

    private static setChangeEleCache(changeEle: HTMLElement, observerType: recordType): void {
        let changeEleSet = ObserverRecord.changeEleCache.get(observerType);
        if (!changeEleSet) {
            changeEleSet = new Set<HTMLElement>();
            ObserverRecord.changeEleCache.set(observerType, changeEleSet);
        }
        changeEleSet.add(changeEle);
    }

    private static checkCount(changeEle: HTMLElement, observerType: recordType): boolean {
        const changeEleMap = ObserverRecord.ignoreUselessChangeEle.get(observerType);
        if (!changeEleMap) {
            return false;
        }

        const count = changeEleMap.get(changeEle);
        if (count && count >= UPPER_LIMIT) {
            return true;
        }
        return false;
    }
}

export class StyleRecord {
    // styleCount: Map<父元素, [变动次数, 状态标记]>
    private static styleCount: Map<HTMLElement, [number, boolean]> = new Map();
    static ignoreStyleChange(ele: HTMLElement, isReset: boolean): void {
        // 被配置的大图不进行缓存
        if (Store.getValue(ele, LayoutKey.CONFIG_LAYOUT_TAG) === LayoutValue.LARGE_IMG) {
            return;
        }

        StyleRecord.clear(StyleRecord.styleCount);

        const value = StyleRecord.styleCount.get(ele);
        if (!value) {
            if (!isReset) {
                StyleRecord.styleCount.set(ele, [0, isReset]);
            }

            return;
        }

        const oldState = value[STATE_IDX];
        let count = value[CHANGE_COUNT_IDX];

        if (isReset === oldState) {
            return;
        }

        if (count > UPPER_LIMIT) {
            Log.i(ele, '过滤乒乓变动的元素', Tag.record);
            StyleSetter.disableHandle(ele);
            return;
        }

        if (isReset) {
            count++;
        }

        StyleRecord.styleCount.set(ele, [count, isReset]);
    }

    private static count = 0;
    // 第100次调用该函数且countMap规模大于100时，清理CountMap
    private static clear(countMap: Map<HTMLElement | string, any[]>): void {
        if (StyleRecord.count < OBSERVER_CONT_LIMIT) {
            StyleRecord.count++;
            return;
        }

        if (countMap.size >= COUNT_MAP_SIZE_LIMIT) {
            for (let [key, value] of countMap.entries()) {
                if (value[0] === 1) {
                    countMap.delete(key);
                }
            }
        }
        StyleRecord.count = 1;
    }
}