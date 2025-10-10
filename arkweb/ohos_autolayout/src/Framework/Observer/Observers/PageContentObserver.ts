/**
 * PageContentObserver - 页面内容观察器
 * 
 * 功能：响应式监听页面内容加载，避免在骨架屏/白屏阶段进行弹窗检测
 * 
 * 优化说明：
 * 1. 避免死循环：不使用轮询，直接使用 MutationObserver 响应式等待
 * 2. 真正检测内容：检查可见 DOM 节点数量，而非仅依赖时间
 * 3. 统一管控：集成到 ObserverHandler 体系中
 * 4. 性能优化：只在必要时启动观察器，检测到内容后立即停止
 */

import Log from '../../../Debug/Log';
import Tag from '../../../Debug/Tag';
import ObserverHandler from '../ObserverHandler';

export default class PageContentObserver {
    private static readonly TAG = Tag.pageContentObserver;
    private static contentReady: boolean = false;  // 页面内容是否已就绪
    private static observer: MutationObserver | null = null;  // 内容变化观察器
    private static readonly MIN_NODE_THRESHOLD = 10;  // 最小节点数阈值
    private static readonly MAX_CHECK_CHILDREN = 50;  // 每层最多检查的子节点数

    /**
     * 初始化/重新初始化
     */
    static reInit(): void {
        Log.info('reInit', PageContentObserver.TAG);
        PageContentObserver.contentReady = false;
        PageContentObserver.disconnect();
    }

    /**
     * 检测页面内容是否就绪
     * @returns {boolean} 内容是否就绪
     */
    static isContentReady(): boolean {
        // 场景1: 已经确认过内容就绪，直接返回
        if (PageContentObserver.contentReady) {
            return true;
        }

        // 场景2: 检测页面是否有实际内容
        const hasContent = PageContentObserver.checkPageContent();
        if (hasContent) {
            Log.info('检测到页面内容就绪', PageContentObserver.TAG);
            PageContentObserver.contentReady = true;
            PageContentObserver.disconnect();
            return true;
        }

        // 场景3: 无内容，返回 false
        Log.d('页面内容未就绪（骨架屏/白屏）', PageContentObserver.TAG);
        return false;
    }

    /**
     * 启动内容观察器，响应式等待页面内容加载
     * 当检测到 DOM 变化且内容就绪时，自动触发弹窗检测
     */
    static startObserving(): void {
        if (PageContentObserver.observer) {
            return;  // 已经启动
        }

        Log.info('启动观察器，响应式等待页面内容加载...', PageContentObserver.TAG);
        
        PageContentObserver.observer = new MutationObserver(() => {
            // 检测到 DOM 变化，验证是否有实际内容
            if (PageContentObserver.checkPageContent()) {
                Log.info('观察器检测到页面内容就绪，触发弹窗检测', PageContentObserver.TAG);
                PageContentObserver.contentReady = true;
                PageContentObserver.disconnect();
                // 重新触发弹窗检测
                ObserverHandler.postTask();
            }
        });

        // 观察 body 的子节点变化
        if (document.body) {
            PageContentObserver.observer.observe(document.body, {
                childList: true,
                subtree: true,
            });
        }
    }

    /**
     * 停止内容观察器
     */
    static disconnect(): void {
        if (PageContentObserver.observer) {
            Log.d('停止观察器', PageContentObserver.TAG);
            PageContentObserver.observer.disconnect();
            PageContentObserver.observer = null;
        }
    }

    /**
     * 检测页面是否有实际内容
     * 策略：统计可见的、有意义的 DOM 节点数量
     * @returns {boolean} 是否有内容
     */
    private static checkPageContent(): boolean {
        const body = document.body;
        if (!body) {
            return false;
        }

        // 统计有效节点数量
        let meaningfulNodeCount = 0;

        // 递归检查节点
        const checkNode = (node: Element): void => {
            // 跳过隐藏元素
            if (node instanceof HTMLElement) {
                const style = getComputedStyle(node);
                if (style.display === 'none' || style.visibility === 'hidden' || style.opacity === '0') {
                    return;
                }
            }

            // 有意义的节点类型
            const meaningfulTags = [
                'DIV', 'P', 'SPAN', 'A', 'IMG', 'BUTTON', 'INPUT', 
                'TEXTAREA', 'UL', 'LI', 'H1', 'H2', 'H3', 'H4', 'H5', 'H6',
                'SECTION', 'ARTICLE', 'NAV', 'HEADER', 'FOOTER', 'MAIN'
            ];
            
            if (meaningfulTags.includes(node.tagName)) {
                meaningfulNodeCount++;
                
                // 提前退出优化：如果已经超过阈值，不需要继续统计
                if (meaningfulNodeCount > PageContentObserver.MIN_NODE_THRESHOLD) {
                    return;
                }
            }

            // 递归检查子节点（限制数量，避免性能问题）
            if (node.children.length > 0 && meaningfulNodeCount <= PageContentObserver.MIN_NODE_THRESHOLD) {
                const maxChildren = Math.min(node.children.length, PageContentObserver.MAX_CHECK_CHILDREN);
                for (let i = 0; i < maxChildren; i++) {
                    checkNode(node.children[i]);
                    if (meaningfulNodeCount > PageContentObserver.MIN_NODE_THRESHOLD) {
                        break;
                    }
                }
            }
        };

        checkNode(body);
        
        const hasContent = meaningfulNodeCount > PageContentObserver.MIN_NODE_THRESHOLD;
        Log.d(`页面节点统计: ${meaningfulNodeCount} 个有效节点, 阈值: ${PageContentObserver.MIN_NODE_THRESHOLD}, 判断: ${hasContent ? '有内容' : '无内容'}`, PageContentObserver.TAG);
        
        return hasContent;
    }

    /**
     * 重置状态（用于测试或特殊场景）
     */
    static reset(): void {
        PageContentObserver.contentReady = false;
        PageContentObserver.disconnect();
    }
}
