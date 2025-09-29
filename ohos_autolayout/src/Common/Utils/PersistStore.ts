import Store from './Store';

export default class PersistStore extends Store {
    static valueMap = new Map<string, WeakMap<HTMLElement, string>>();

    static clearAllTag(ele: HTMLElement, isDelete: boolean): void {
        if (!isDelete) {
            return;
        }

        super.clearAllTag(ele, isDelete);
    }
}
