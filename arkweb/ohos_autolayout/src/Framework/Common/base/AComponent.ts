
export abstract class AComponent {
    protected mDOMSnapshot: Node;
    protected mComponent: HTMLElement;
    protected mDirty: boolean = true;
    constructor(protected root: HTMLElement) {
        this.mComponent = root;
    }
    static recognize(root: HTMLElement): boolean {
        throw new Error(`FATAL ERROR:Please Implement tis function in subclass`);
    }
    abstract intelligenceLayout(): void;
    abstract printTree(color: boolean): void;
    abstract resetStyle(): void;
    isDirty(): boolean {
        return this.mDirty;
    }
    setDirty(dirty: boolean): void {
        this.mDirty = dirty;
    }
    // 判断node是否是子节点
    contains(node: Node): boolean {
        return this.mComponent.contains(node);
    }
}
