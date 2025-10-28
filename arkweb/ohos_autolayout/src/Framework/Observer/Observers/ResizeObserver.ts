import ObserverHandler from '../ObserverHandler';
import WaitSystemReady from '../../Utils/WaitSystemReady';
import CSSSheetManage from '../../Common/Style/Setter/CSSSheetManage';
import Framework from '../../Framework';
import Log from '../../../Debug/Log';
import Tag from '../../../Debug/Tag';
import { Main } from '../../../Main';
import Constant from '../../Common/Constant';
import IntelligentLayout from '../../IntelligentLayout';
import ModifyObserver from './ModifyObserver';
 
export default class ResizeObserver {
    private static readonly TAG = Tag.resizeObserver;
    static init: boolean = false;

    static init_(): void {
        Log.info('========== 初始化窗口尺寸监听器 ==========', ResizeObserver.TAG);
        if (ResizeObserver.init) {
            Log.d('监听器已初始化，跳过重复初始化', ResizeObserver.TAG);
            return;
        }
        ResizeObserver.init = true;

        Log.d(`当前窗口尺寸: ${window.innerWidth}×${window.innerHeight}`, ResizeObserver.TAG);
        Log.d('注册resize事件监听器', ResizeObserver.TAG);
        window.addEventListener(Constant.resize, ResizeObserver.resizeCallback);
        Log.info('窗口尺寸监听器启动成功', ResizeObserver.TAG);
    }

    private static resizeCallback(): void {
        Log.info('========== 收到窗口尺寸变化事件 ==========', ResizeObserver.TAG);
        Log.d(`新窗口尺寸: ${window.innerWidth}×${window.innerHeight}`, ResizeObserver.TAG);
        Log.d(`可视区域: ${window.innerWidth}×${window.innerHeight}, 屏幕尺寸: ${screen.width}×${screen.height}`, ResizeObserver.TAG);
        
        if (Main.initFlag) {
            Log.d('系统已初始化，执行resize处理', ResizeObserver.TAG);
            ResizeObserver.onResize();
            return;
        }
        Log.w('系统未初始化，触发重启', ResizeObserver.TAG);
        Main.restart();
    }
 
    static onResize(): void {
        Log.info('========== 处理窗口尺寸变化 ==========', ResizeObserver.TAG);
        
        if (!WaitSystemReady.hasBodyReady) {
            Log.w('document.body未就绪，跳过resize处理', ResizeObserver.TAG);
            return;
        }
        
        // 步骤1: 取消所有动画超时任务
        Log.d('步骤1: 取消所有活跃的动画超时任务', ResizeObserver.TAG);
        ModifyObserver.cancelAllAnimationTimeouts();
        
        // 步骤2: 重置所有弹窗状态（取消异步验证、重置状态为IDLE、标记为dirty）
        Log.d('步骤2: 重置所有弹窗窗口状态', ResizeObserver.TAG);
        IntelligentLayout.resetAllPopWindows();
            
        Log.d('  - 执行头部就绪任务', ResizeObserver.TAG);
        Framework.headReadyTask();
        
        Log.d('  - 更新观察器状态', ResizeObserver.TAG);
        ObserverHandler.updateObserver();
        Log.checkState('after updateObserver');
        
        Log.d('  - 更新CSS样式表状态', ResizeObserver.TAG);
        CSSSheetManage.updateState();
        
        Log.d('  - 触发弹窗检测任务', ResizeObserver.TAG);
        ObserverHandler.postTask();
        
        Log.info('窗口尺寸变化处理完成', ResizeObserver.TAG);
    }

    static removeListener(): void {
        Log.info('========== 移除窗口尺寸监听器 ==========', ResizeObserver.TAG);
        if (!ResizeObserver.init) {
            Log.d('监听器未初始化，无需移除', ResizeObserver.TAG);
            return;
        }
        Log.d('移除resize事件监听器', ResizeObserver.TAG);
        ResizeObserver.init = false;
        removeEventListener(Constant.resize, ResizeObserver.resizeCallback);
        Log.info('窗口尺寸监听器已移除', ResizeObserver.TAG);
    }
}