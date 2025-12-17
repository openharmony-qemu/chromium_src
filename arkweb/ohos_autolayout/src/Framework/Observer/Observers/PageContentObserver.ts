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
import ObserverHandler from '../ObserverHandler';

export default class PageContentObserver {
    private static readonly TAG = 'PageContentObserver';
    private static contentReady: boolean = false;  // 页面内容是否已就绪
    private static observer: MutationObserver | null = null;  // 内容变化观察器
    private static readonly MIN_NODE_THRESHOLD = 9;  // 最小节点数阈值
    private static readonly MAX_CHECK_CHILDREN = 50;  // 每层最多检查的子节点数

    /**
     * 初始化/重新初始化
     */
    static reInit(): void {
        Log.info('========== 初始化页面内容观察器 ==========', PageContentObserver.TAG);
        Log.d('重置内容就绪状态为false', PageContentObserver.TAG);
        PageContentObserver.contentReady = false;
        PageContentObserver.disconnect();
        Log.info('页面内容观察器初始化完成', PageContentObserver.TAG);
    }

    /**
     * 检测页面内容是否就绪
     * @returns {boolean} 内容是否就绪
     */
    static isContentReady(): boolean {
        Log.d('开始检查页面内容就绪状态', PageContentObserver.TAG);
        
        // 场景1: 已经确认过内容就绪，直接返回
        if (PageContentObserver.contentReady) {
            Log.d('内容已就绪（缓存状态）', PageContentObserver.TAG);
            return true;
        }

        // 场景2: 检测页面是否有实际内容
        Log.d('执行页面内容检测...', PageContentObserver.TAG);
        const hasContent = PageContentObserver.checkPageContent();
        if (hasContent) {
            Log.info('✅ 检测到页面内容就绪', PageContentObserver.TAG);
            PageContentObserver.contentReady = true;
            PageContentObserver.disconnect();
            return true;
        }

        // 场景3: 无内容，返回 false
        Log.w('⚠️ 页面内容未就绪（骨架屏/白屏状态）', PageContentObserver.TAG);
        return false;
    }

    /**
     * 启动内容观察器，响应式等待页面内容加载
     * 当检测到 DOM 变化且内容就绪时，自动触发弹窗检测
     */
    static startObserving(): void {
        if (PageContentObserver.observer) {
            Log.d('观察器已在运行，跳过重复启动', PageContentObserver.TAG);
            return;  // 已经启动
        }

        Log.info('========== 启动页面内容观察器 ==========', PageContentObserver.TAG);
        Log.d('配置: 监听childList和subtree变化', PageContentObserver.TAG);
        
        let mutationCount = 0;
        PageContentObserver.observer = new MutationObserver(() => {
            mutationCount++;
            Log.d(`收到DOM变化通知 (第${mutationCount}次)`, PageContentObserver.TAG);
            
            // 检测到 DOM 变化，验证是否有实际内容
            if (PageContentObserver.checkPageContent()) {
                Log.info(`✅ 观察器检测到页面内容就绪 (共监听${mutationCount}次变化)`, PageContentObserver.TAG);
                PageContentObserver.contentReady = true;
                PageContentObserver.disconnect();
                // 重新触发弹窗检测
                Log.d('触发弹窗检测任务', PageContentObserver.TAG);
                ObserverHandler.postTask();
            } else {
                Log.d('内容仍未就绪，继续观察...', PageContentObserver.TAG);
            }
        });

        // 观察 body 的子节点变化
        if (document.body) {
            PageContentObserver.observer.observe(document.body, {
                childList: true,
                subtree: true,
            });
            Log.info('页面内容观察器已启动，等待内容加载...', PageContentObserver.TAG);
        } else {
            Log.e('document.body不存在，无法启动观察器', PageContentObserver.TAG);
        }
    }

    /**
     * 停止内容观察器
     */
    static disconnect(): void {
        if (PageContentObserver.observer) {
            Log.info('停止页面内容观察器', PageContentObserver.TAG);
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
        Log.d('开始统计页面有效节点...', PageContentObserver.TAG);
        const body = document.body;
        if (!body) {
            Log.e('document.body不存在', PageContentObserver.TAG);
            return false;
        }

        const startTime = performance.now();
        // 统计有效节点数量
        let meaningfulNodeCount = PageContentObserver.checkMeaningfulNodeRecursive(body);
        const duration = (performance.now() - startTime).toFixed(2);
        
        const hasContent = meaningfulNodeCount > PageContentObserver.MIN_NODE_THRESHOLD;
        Log.d(`页面节点统计完成: ${meaningfulNodeCount}个有效节点 (阈值${PageContentObserver.MIN_NODE_THRESHOLD}, 耗时${duration}ms) => ${hasContent ? '✅有内容' : '❌无内容'}`, PageContentObserver.TAG);
        
        return hasContent;
    }

     // 递归检查节点
     private static checkMeaningfulNodeRecursive(node: Element, depth: number = 0): number {
        let meaningfulNodeCount = 0;
        
        // 跳过隐藏元素
        if (node instanceof HTMLElement) {
            const style = getComputedStyle(node);
            if (style.display === 'none' || style.visibility === 'hidden' || style.opacity === '0') {
                if (depth === 0) {
                    Log.d(`跳过隐藏的根元素: ${node.tagName}.${node.className}`, PageContentObserver.TAG);
                }
                return meaningfulNodeCount;
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
                Log.d(`提前退出: 已统计${meaningfulNodeCount}个节点，超过阈值`, PageContentObserver.TAG);
                return meaningfulNodeCount;
            }
        }

        // 递归检查子节点（限制数量，避免性能问题）
        if (node.children.length > 0 && meaningfulNodeCount <= PageContentObserver.MIN_NODE_THRESHOLD) {
            const maxChildren = Math.min(node.children.length, PageContentObserver.MAX_CHECK_CHILDREN);
            if (node.children.length > maxChildren) {
                Log.d(`节点${node.tagName}有${node.children.length}个子节点，限制检查前${maxChildren}个`, PageContentObserver.TAG);
            }
            
            for (let i = 0; i < maxChildren; i++) {
                meaningfulNodeCount += PageContentObserver.checkMeaningfulNodeRecursive(node.children[i], depth + 1);
                if (meaningfulNodeCount > PageContentObserver.MIN_NODE_THRESHOLD) {
                    break;
                }
            }
        }
        
        return meaningfulNodeCount;
    }

    /**
     * 重置状态（用于测试或特殊场景）
     */
    static reset(): void {
        Log.info('重置页面内容观察器状态', PageContentObserver.TAG);
        PageContentObserver.contentReady = false;
        PageContentObserver.disconnect();
    }
}
