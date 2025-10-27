import { PopupInfo } from '../Popup/PopupInfo';
import Constant from './Constant';
import {DetectorInst, LayoutMetrics} from './DetectorInst';
import LayoutUtils from '../Utils/LayoutUtils';
import Log from '../../Debug/Log';
import Tag from '../../Debug/Tag';

// 布局约束指标检测结果接口
export interface LayoutConstraintMetrics {
    resultCode: number;               // 错误码，0表示通过
    errorMsg: string;                 // 错误描述（简略版）
    duration: number;                 // 报告耗时
    report: string;                   // 结构化报告文本（详细版）
}

// 布局约束指标检测器
export class LayoutConstraintMetricsDetector {
    /**
     * 执行所有布局约束指标检测并生成报告
     */
    public static detectLayoutConstraintMetrics(popupInfo: PopupInfo, rootNodes: Set<HTMLElement>): LayoutConstraintMetrics {        
        Log.d('========== 开始弹窗修复结果检测 ==========', Tag.layoutConstraintDetector);
        // 确保根节点列表有效
        if (!rootNodes || rootNodes.size === 0) {
            Log.e('无效的根节点列表', Tag.layoutConstraintDetector);
            return this.createErrorReport('无效的根节点列表');
        }

        Log.d(`待检测根节点数量: ${rootNodes.size}`, Tag.layoutConstraintDetector);
        const reportLines: string[] = [];
        const startTime = performance.now();
        let resultCode = 0;
        let errorMsg = '';
        let allGapsPassed = true;
        let allOverflowRatePassed = true;
        
        for (const rootNode of rootNodes) {
            Log.d(`检测根节点: ${rootNode.tagName}.${rootNode.className}`, Tag.layoutConstraintDetector);
            // 确保单个根节点有效
            if (!rootNode || !(rootNode instanceof HTMLElement)) {
                Log.e(`无效的根节点: ${rootNode}`, Tag.layoutConstraintDetector);
                continue;
            }

            const layoutMap = this.collectLayoutMetrics(rootNode);
            Log.d(`收集到 ${layoutMap.size} 个节点的布局指标`, Tag.layoutConstraintDetector);
            // 确保根节点布局数据存在
            if (!layoutMap.has(rootNode)) {
                Log.w('根节点布局数据缺失，重新获取', Tag.layoutConstraintDetector);
                this.addRootNodeMetrics(rootNode, layoutMap);
            }
            
            // 1. 四边安全间隙检测（使用安全区域版本）
            Log.d('开始四边安全间隙检测', Tag.layoutConstraintDetector);
            const safeGaps = this.detectSafeAreaGaps(rootNode, layoutMap, reportLines);
            const gapsPassed = Object.values(safeGaps).every(gap => gap >= DetectorInst.minGap);
            if (!gapsPassed)  {
                Log.w(`安全间隙检测失败: 左${safeGaps.left.toFixed(1)} 上${safeGaps.top.toFixed(1)} 右${safeGaps.right.toFixed(1)} 下${safeGaps.bottom.toFixed(1)}`, Tag.layoutConstraintDetector);
                resultCode = Math.max(resultCode, Constant.ERR_CODE_GAPS);
                allGapsPassed = false;
            } else {
                Log.d('安全间隙检测通过', Tag.layoutConstraintDetector);
            }

            // 2. 交互元素溢出率检测
            Log.d('开始交互元素溢出率检测', Tag.layoutConstraintDetector);
            const overflowRate = this.detectContainerOverflowRate(rootNode, layoutMap, reportLines);
            if (overflowRate > 0) {
                Log.w(`元素溢出率检测失败: ${overflowRate.toFixed(1)}%`, Tag.layoutConstraintDetector);
                resultCode = Math.max(resultCode, Constant.ERR_CODE_OVERFLOW);
                allOverflowRatePassed = false;
            } else {
                Log.d('元素溢出率检测通过', Tag.layoutConstraintDetector);
            }
        }
        
        if (!allGapsPassed) {
            errorMsg += '四边安全间隙检测 FAIL.';
        }
        if (!allOverflowRatePassed) {
            errorMsg += '元素溢出率检测 FAIL.';
        }
        
        // 5. 底部关闭按钮与弹窗主体重叠检测
        Log.d('开始底部关闭按钮重叠检测', Tag.layoutConstraintDetector);
        if (LayoutUtils.isBottomCloseButtonOverlap(popupInfo)) {
            Log.w('关闭按钮重叠检测失败', Tag.layoutConstraintDetector);
            resultCode = Math.max(resultCode, Constant.ERR_CODE_CLOSE_BTN_OVERLAP);
            errorMsg += '关闭按钮重叠检测 FAIL.';
        } else {
            Log.d('关闭按钮重叠检测通过', Tag.layoutConstraintDetector);
        }
        
        const finalReport = this.generateFinalReport(reportLines, resultCode);
        const duration = performance.now() - startTime;
        
        Log.d(`布局约束检测完成，耗时: ${duration.toFixed(2)}ms`, Tag.layoutConstraintDetector);
        Log.d(`最终结果码: ${resultCode}, 错误信息: ${errorMsg || '无'}`, Tag.layoutConstraintDetector);
        Log.d('========== 弹窗修复结果检测结束 ==========', Tag.layoutConstraintDetector);
        
        return {
            resultCode,
            errorMsg,
            duration,
            report: finalReport
        };
    }
    
    /**
     * 生成最终报告
     */
    private static generateFinalReport(reportLines: string[], resultCode: number): string {
        const status = resultCode === 0 ? '✅ 通过' : '❌ 未通过';
        const summary = `# 布局约束指标检测报告\n\n## 总体状态: ${status}\n\n`;
        
        const details = reportLines.join('\n\n');
        
        const footer = '\n\n---\n*检测标准:*\n' + 
                       '- 安全间隙 ≥ ' + DetectorInst.minGap + '\n' +
                       '- 元素溢出率 ≤ ' + DetectorInst.maxProportion + '.\n';
        return summary + details + footer;
    }

    /**
     * 创建错误报告
     */
    private static createErrorReport(errorMessage: string): LayoutConstraintMetrics {
        const report = `### 布局约束指标检测\n❌ 错误: ${errorMessage}`;
        return {
            resultCode: -1,
            errorMsg: '',
            duration: 0,
            report
        };
    }

    /**
     * 添加根节点布局指标
     */
    private static addRootNodeMetrics(
        rootNode: HTMLElement,
        layoutMap: Map<HTMLElement, LayoutMetrics>
    ): void {
        const rect = rootNode.getBoundingClientRect();
        layoutMap.set(rootNode, {
            top: rect.top,
            left: rect.left,
            width: rect.width,
            height: rect.height,
            right: rect.right,
            bottom: rect.bottom
        });
    }

    /**
     * 收集所有节点的布局指标
     */
    private static collectLayoutMetrics(rootNode: HTMLElement): Map<HTMLElement, LayoutMetrics> {
        Log.d(`开始收集布局指标: ${rootNode.tagName}.${rootNode.className}`, Tag.layoutConstraintDetector);
        const layoutMap = new Map<HTMLElement, LayoutMetrics>();
        
        // 首先确保添加根节点
        this.addRootNodeMetrics(rootNode, layoutMap);
        
        // 然后添加所有后代元素
        const walker = document.createTreeWalker(
            rootNode,
            NodeFilter.SHOW_ELEMENT,
            { 
                acceptNode: () => NodeFilter.FILTER_ACCEPT 
            }
        );
        
        let currentNode: Node | null = walker.nextNode(); // 从第一个子节点开始
        let descendantCount = 0;
        while (currentNode) {
            const el = currentNode as HTMLElement;
            // 确保不重复添加根节点
            if (el !== rootNode) {
                const rect = el.getBoundingClientRect();
                layoutMap.set(el, {
                    top: rect.top,
                    left: rect.left,
                    width: rect.width,
                    height: rect.height,
                    right: rect.right,
                    bottom: rect.bottom
                });
                descendantCount++;
            }
            currentNode = walker.nextNode();
        }
        
        Log.d(`布局指标收集完成: 根节点1个 + 后代${descendantCount}个 = 总计${layoutMap.size}个`, Tag.layoutConstraintDetector);
        return layoutMap;
    }

    /**
     * 1、安全间隙检测 （考虑安全区域）
    **/
    private static detectSafeAreaGaps(
        rootNode: HTMLElement,
        layoutMap: Map<HTMLElement, LayoutMetrics>,
        reportLines: string[]
    ): { left: number; top: number; right: number; bottom: number } {
        // 确保获取根节点指标
        const rootMetrics = layoutMap.get(rootNode);
        if (!rootMetrics) {
            Log.e('无法获取根节点布局指标', Tag.layoutConstraintDetector);
            reportLines.push('### 安全区域间隙检测\n❌ 错误: 无法获取根节点布局指标');
            return { left: 0, top: 0, right: 0, bottom: 0 };
        }

        Log.d(`根节点位置: L${rootMetrics.left.toFixed(1)} T${rootMetrics.top.toFixed(1)} R${rootMetrics.right.toFixed(1)} B${rootMetrics.bottom.toFixed(1)}`, Tag.layoutConstraintDetector);
        Log.d(`根节点尺寸: ${rootMetrics.width.toFixed(1)}×${rootMetrics.height.toFixed(1)}`, Tag.layoutConstraintDetector);
        
        // 获取安全区域（兼容不同设备和浏览器）
        const safeArea = {
            top: 0,
            left: 0,
            bottom: 0,
            right: 0,
            ...(window as any).safeAreaInsets // 兼容iOS安全区域
        };
        
        Log.d(`安全区域: L${safeArea.left} T${safeArea.top} R${safeArea.right} B${safeArea.bottom}`, Tag.layoutConstraintDetector);
        
        // 计算有效屏幕边界（考虑安全区域）
        const effectiveBounds = {
            left: safeArea.left,
            top: safeArea.top,
            right: window.innerWidth - safeArea.right,
            bottom: window.innerHeight - safeArea.bottom
        };
        
        // 计算四边间隙（相对于安全区域）
        const gaps = {
            left: rootMetrics.left - safeArea.left,
            top: rootMetrics.top - safeArea.top,
            right: effectiveBounds.right - rootMetrics.right,
            bottom: effectiveBounds.bottom - rootMetrics.bottom
        };
        
        Log.d(`计算间隙 (要求≥${DetectorInst.minGap}): 左${gaps.left.toFixed(1)} 上${gaps.top.toFixed(1)} 右${gaps.right.toFixed(1)} 下${gaps.bottom.toFixed(1)}`, Tag.layoutConstraintDetector);
        
        const passed = {
            left: gaps.left >= DetectorInst.minGap,
            top: gaps.top >= DetectorInst.minGap,
            right: gaps.right >= DetectorInst.minGap,
            bottom: gaps.bottom >= DetectorInst.minGap
        };
        
        const failedSides = Object.entries(passed)
            .filter(([_, pass]) => !pass)
            .map(([side, _]) => side);
        if (failedSides.length > 0) {
            Log.d(`间隙检测未通过的方向: ${failedSides.join('、')}`, Tag.layoutConstraintDetector);
        }
        
        // 生成详细报告
        const statusSymbol = (pass: boolean): string => pass ? '✅ 通过' : '❌ 未通过';
        
        const report = `### 1. 安全间隙检测(弹窗距离屏幕)\n` +
                    `- **要求**: 弹窗四边到安全区域边缘 ≥ ${DetectorInst.minGap}\n` +
                    `- **屏幕尺寸**: ${window.innerWidth}×${window.innerHeight}\n` +
                    `- **安全区域**: [T:${safeArea.top}, R:${safeArea.right}, B:${safeArea.bottom}, L:${safeArea.left}]\n` +
                    `- **有效区域**: [L:${effectiveBounds.left.toFixed(1)}, T:${effectiveBounds.top.toFixed(1)}, ` +
                    `R:${effectiveBounds.right.toFixed(1)}, B:${effectiveBounds.bottom.toFixed(1)}]\n` +
                    `- **弹窗位置**: [L:${rootMetrics.left.toFixed(1)}, T:${rootMetrics.top.toFixed(1)}, ` +
                    `R:${rootMetrics.right.toFixed(1)}, B:${rootMetrics.bottom.toFixed(1)}]\n\n` +
                    `| 方向 | 要求 | 实际值 | 状态 |\n` +
                    `|------|------|--------|------|\n` +
                    `| 左侧 | ≥ ${DetectorInst.minGap} | ${gaps.left.toFixed(1)} | ${statusSymbol(passed.left)} |\n` +
                    `| 顶部 | ≥ ${DetectorInst.minGap} | ${gaps.top.toFixed(1)} | ${statusSymbol(passed.top)} |\n` +
                    `| 右侧 | ≥ ${DetectorInst.minGap} | ${gaps.right.toFixed(1)} | ${statusSymbol(passed.right)} |\n` +
                    `| 底部 | ≥ ${DetectorInst.minGap} | ${gaps.bottom.toFixed(1)} | ${statusSymbol(passed.bottom)} |\n\n` +
                    `- **整体状态**: ${Object.values(passed).every(p => p) ? '✅ 全部通过' : '⚠️ 部分未通过'}\n`;
        reportLines.push(report);
        return gaps;
    }

    /**
     * 2. 元素溢出率检测
     */
    private static detectContainerOverflowRate(
        rootNode: HTMLElement,
        layoutMap: Map<HTMLElement, LayoutMetrics>,
        reportLines: string[]
    ): number {

        // 获取安全区域（兼容不同设备和浏览器）
        const safeArea = {
            top: 0,
            left: 0,
            bottom: 0,
            right: 0,
            ...(window as any).safeAreaInsets // 兼容iOS安全区域
        };

        // 计算有效屏幕边界（考虑安全区域）
        const effectiveBounds = {
            left: safeArea.left,
            top: safeArea.top,
            right: window.innerWidth - safeArea.right,
            bottom: window.innerHeight - safeArea.bottom
        };

        const interactiveElements = this.collectInteractiveElements(rootNode);
        Log.d(`找到 ${interactiveElements.length} 个交互元素`, Tag.layoutConstraintDetector);
        
        let overflowCount = 0;
        const overflowDetails: string[] = [];
        
        interactiveElements.forEach((el, index) => {
            // 排除根节点自身
            if (el === rootNode) {
                return;
            }
            const metrics = layoutMap.get(el)!;
            if (!metrics || metrics.width === 0 || metrics.height === 0) {
                return;
            }
            // 检查四边是否在屏幕内
            const overflowLeft = Math.round(metrics.left) < Math.round(effectiveBounds.left);
            const overflowRight = Math.round(metrics.right) > Math.round(effectiveBounds.right);
            const overflowTop = Math.round(metrics.top) < Math.round(effectiveBounds.top);
            const overflowBottom = Math.round(metrics.bottom) > Math.round(effectiveBounds.bottom);
            
            const isOverflowing = overflowLeft || overflowRight || overflowTop || overflowBottom;
            
            if (isOverflowing) {
                // 滚动列表、Swiper组件不检测
                if (this.isScrollableList(el, rootNode) || this.isSwiperComponent(el)) {
                    Log.d(`跳过可滚动元素: ${el.tagName}.${el.className}`, Tag.layoutConstraintDetector);
                    return;
                }
                overflowCount++;
                const directions = [];
                if (overflowLeft) {
                    directions.push('左');
                }
                if (overflowRight) {
                    directions.push('右');
                }
                if (overflowTop) {
                    directions.push('上');
                }
                if (overflowBottom) {
                    directions.push('下');
                }
                const overflowInfo = `${el.tagName}.${el.className}: ${directions.join('、')}侧溢出`;
                Log.d(`检测到溢出元素[${overflowCount}]: ${overflowInfo}`, Tag.layoutConstraintDetector);
                overflowDetails.push(`  - ${overflowInfo}`);
            }
        });
        
        const overflowRate = interactiveElements.length > 0 
            ? (overflowCount / interactiveElements.length * 100) 
            : 0;
        const passed = overflowRate === 0;
        const status = passed ? '✅ 通过' : '❌ 未通过';
        
        Log.d(`溢出率统计: ${overflowCount}/${interactiveElements.length} = ${overflowRate.toFixed(1)}%`, Tag.layoutConstraintDetector);
        
        const report = `### 3.  元素溢出率检测(弹窗内控件是否溢出弹窗)\n` +
                       `- **要求**: 元素溢出率 ≤ ${DetectorInst.maxProportion}}\n` +
                       `- **实际值**: ${overflowRate.toFixed(1)}%\n` +
                       `- **状态**: ${status}\n` +
                       `- **详情**:\n` +
                       `  - 总元素数: ${interactiveElements.length}\n` +
                       `  - 溢出元素数: ${overflowCount}\n`;
        
        if (overflowDetails.length > 0) {
            reportLines.push(report + `  - **溢出元素**:\n${overflowDetails.join('\n')}`);
        } else {
            reportLines.push(report);
        }
        
        return overflowRate;
    }

    /**
     * 收集所有可交互元素
     * 该函数可以放到组件识别中
     */
    private static collectInteractiveElements(root: HTMLElement): HTMLElement[] {
        Log.d(`开始收集交互元素: ${root.tagName}.${root.className}`, Tag.layoutConstraintDetector);
        // 可配置的选择器列表
        const SELECTORS = [
            // ======== 标准交互元素 ========
            'button:not([disabled])',
            'a[href]:not([disabled])',
            'input:not([disabled]):not([type="hidden"])',
            'select:not([disabled])',
            'textarea:not([disabled])',
            
            // ======== ARIA 兼容元素 ========
            '[role="button"]:not([disabled])',
            '[role="link"]:not([disabled])',
            '[role="menuitem"]',
            '[role="option"]',
            '[role="img"]',
            
            // ======== 可聚焦元素 ========
            '[tabindex]:not([tabindex="-1"])',
            
            // ======== 图标类关闭按钮 ========
            // 方案1: 关键类组合
            '.a-text.iconfont', 
            '.a-view.iconfont',
            
            // // 方案2: 带交互属性的图标
            '.iconfont[data-action="close"]',
            '.iconfont[data-role="button"]',
            '.iconfont[onclick]',
            '.iconfont[role="button"]',
            
            // // 方案3: 特定关闭图标
            '.icon-close',
            '.icon-close-circle',
            '.icon-close_circle',
            '.icon-guanbi',
            '.icon-shutdown',
            
            // // 方案4: 类名包含关闭关键词
            '[class*="close"]', // 类名包含"close"
            '[class*="guanbi"]' // 类名包含"guanbi"
        ];
    
        // 使用 Set 自动去重
        const selectorSet = new Set(SELECTORS);
        const selectorString = Array.from(selectorSet).join(',');
        
        // 查询所有匹配的交互元素
        const interactiveElements = Array.from(root.querySelectorAll(selectorString)) as HTMLElement[];

        // 检查根元素自身是否符合交互元素的条件
        const matchesSelector = (element: HTMLElement, selectors: string[]): boolean => {
            return selectors.some(selector => element.matches(selector));
        };

        if (matchesSelector(root, Array.from(selectorSet))) {
            interactiveElements.unshift(root);
            Log.d('根元素本身也是交互元素', Tag.layoutConstraintDetector);
        }

        Log.d(`交互元素收集完成: 共${interactiveElements.length}个`, Tag.layoutConstraintDetector);
        return interactiveElements;
    }

    /**
     * 判断元素是否是 滚动 组件
     * @param {HTMLElement} element - 需要判断的元素
     * @returns {boolean} - 是否是 滚动 组件
     */
    private static isScrollableList(element: HTMLElement, rootNode:HTMLElement): boolean {
        const checkOverflow = (el: HTMLElement): boolean => {
            const style = window.getComputedStyle(el);
            const overflow = style.overflow;
            const overflowY = style.overflowY;
            return overflow === 'auto' || overflow === 'scroll' ||
                    overflowY === 'auto' || overflowY === 'scroll';
        };
    
        if (checkOverflow(element)) {
            return true;
        }
    
        let parent = element.parentElement;
        while (parent !== rootNode.parentElement) {
            if (checkOverflow(parent)) {
                return true;
            }
            parent = parent.parentElement;
        }
    
        return false;
    }

    /**
     * 判断元素是否是 swiper 组件
     * @param {HTMLElement} element - 需要判断的元素
     * @returns {boolean} - 是否是 swiper 组件
     */
    private static isSwiperComponent(element: HTMLElement): boolean {
        // 使用正则表达式匹配 swiper 字符串
        const swiperRegex = /swiper/;

        // 检查元素自身的类名
        const isSwiperClass = swiperRegex.test(element.className);

        // 检查元素的父节点是否是 swiper 容器
        let parent = element.parentElement;
        while (parent) {
            if (swiperRegex.test(parent.className)) {
                return true;
            }
            parent = parent.parentElement; // 继续检查上一级父元素
        }
        return isSwiperClass;
    }
}