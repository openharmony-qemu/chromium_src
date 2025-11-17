export interface LayoutMetrics {
    top: number;
    left: number;
    width: number;
    height: number;
    right: number;
    bottom: number;
}

export class DetectorInst {
    // 最小安全间隙
    public static readonly minGap: number = -1;
    public static readonly minInterWH: number = Number((48 / Math.max(window.devicePixelRatio, 1)).toFixed(2));

    public static readonly empiricalCoefficient: number = 0.022;
    public static readonly maxProportion: number = 0;
    public static readonly minAlignment: number = Number((20 / Math.max(window.devicePixelRatio, 1)).toFixed(2));
    public static readonly tolerance: number = Number((20 / Math.max(window.devicePixelRatio, 1)).toFixed(2));
    public static readonly relativeOffset: number = 0.05;
    public static readonly maxOffset: number = 0.5;
    
    // 单例实例引用
    private static instance: DetectorInst;

    // 原始布局状态（包含屏幕尺寸记录）
    private originalLayoutState: {
        metrics: Map<HTMLElement, LayoutMetrics>;
        screenSize: { width: number; height: number };
    } | null = null;

    // 私有构造函数确保外部无法实例化
    private constructor() {
    }

    /**
     * 获取单例实例
     */
    public static getInstance(): DetectorInst {
        if (!DetectorInst.instance) {
            DetectorInst.instance = new DetectorInst();
        }
        return DetectorInst.instance;
    }

    /**
     * 记录原始布局位置和屏幕尺寸
     */
    public recordOriginalPosition(rootNode: HTMLElement): void {
        if (this.originalLayoutState && this.originalLayoutState.metrics.has(rootNode)) {
            return;
        }

        const metrics = new Map<HTMLElement, LayoutMetrics>();
        metrics.set(rootNode, this.getLayoutMetrics(rootNode));
        
        // 记录所有子元素
        const allElements = rootNode.querySelectorAll('*') as NodeListOf<HTMLElement>;
        allElements.forEach(el => {
            metrics.set(el, this.getLayoutMetrics(el));
        });
        
        // 保存屏幕尺寸
        this.originalLayoutState = {
            metrics,
            screenSize: {
                width: window.innerWidth,
                height: window.innerHeight
            }
        };
    }

    /**
     * 获取元素的布局指标
     */
    public getLayoutMetrics(element: HTMLElement): LayoutMetrics {
        const rect = element.getBoundingClientRect();
        return {
            top: rect.top,
            left: rect.left,
            width: rect.width,
            height: rect.height,
            right: rect.right,
            bottom: rect.bottom
        };
    }
}