// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import common from '@ohos.app.ability.common';
import { inject, injectable } from 'inversify';
import window from '@ohos.window';
import { IParams, WindowBound } from '../interface/CommonInterface';
import { AbilityManager } from '../common/AbilityManager';
import { BaseAdapter } from '../common/BaseAdapter';
import LogUtil from '../utils/LogUtil';
import LogMethod from '../common/LogDecorator';
import { DragParamManager } from '../common/DragParamManager';
import { MODULE_TYPE } from '../common/ModuleType';
import { BusinessError } from '@kit.BasicServicesKit';

interface ISubWindowInfo {
  id: string,
  parentId: string,
  subWindow: window.Window,
  localStorage: LocalStorage,
}

const TAG: string = 'SubWindow';
const FEEDBACK_X: number = 2;
const MAX_REUSABLE_WINDOW_NUM: number = 5;

@injectable()
export default class SubWindowAdapter extends BaseAdapter {
  private abilityManager: AbilityManager;
  private subWindowList: (ISubWindowInfo)[] = [];
  private reusableSubWindow: Map<string, (ISubWindowInfo)[]> = new Map<string, (ISubWindowInfo)[]>();
  private dragParamManager: DragParamManager;

  constructor(
    @inject(AbilityManager) abilityManager: AbilityManager,
    @inject(DragParamManager) dragManager: DragParamManager,
    @inject(MODULE_TYPE.Context) ctx: common.UIAbilityContext
  ) {
    super(ctx);
    this.abilityManager = abilityManager;
    this.dragParamManager = dragManager;
  }

  @LogMethod
  async createSubWindow(
    parentId: string,
    id: string,
    r: WindowBound,
    initColorRgb: string,
    callback: (ready: boolean, id: string) => void) {
    let fixedBounds = r;
    // Conversion is required if there is no parent window
    if (parentId === 'browser0') {
      parentId = 'browser1';
    }

    // TODO: add x border size to menu
    let mainWindow = this.abilityManager.getMainWindow(parentId);
    const mainProp = mainWindow?.getWindowProperties();
    if (mainProp && (mainProp.drawableRect.left !== 0)) {
      fixedBounds.top += mainProp ? mainProp.drawableRect.left : 0;
    } else {
      // In full-screen mode, when a sub-window is created, the window is
      // moved left by two pixels to adjust its position relative to the cursor point.
      fixedBounds.top += FEEDBACK_X;
    }

    const para: Record<string, IParams> = {
      'subWindowParams': {
        id: id,
        callback: callback,
        size: [fixedBounds.width, fixedBounds.height],
        initColorRgb: initColorRgb,
      }
    };
    const reusableWindow = this.findReuseableSubWindow(parentId, fixedBounds.width, fixedBounds.height);
    if (reusableWindow) {
      try {
        let paramsLink: SubscribedAbstractProperty<IParams> = reusableWindow.localStorage.link('subWindowParams');
        let lastParams = paramsLink.get();
        lastParams.initColorRgb = initColorRgb;
        paramsLink.set(lastParams);
        this.resizeSubWindow(reusableWindow.subWindow, fixedBounds.width, fixedBounds.height);
        this.moveSubWindow(reusableWindow.subWindow, fixedBounds.left, fixedBounds.top);
        reusableWindow.subWindow.showWindow(() => {
          LogUtil.error(TAG,'reuseSubWindow ' + reusableWindow.id);
          return;
        });
        callback(true, reusableWindow.id);
        return;
      } catch (e) {
        LogUtil.error(TAG,'reuseSubWindow error' + reusableWindow.id);
      }
    }
    let subStorage: LocalStorage = new LocalStorage(para);
    this.createNewWindow(parentId, id, fixedBounds, subStorage);
  }

  private findSubWindow(id: string): ISubWindowInfo | undefined {
    for (let i = 0; i < this.subWindowList.length; i++) {
      const item = this.subWindowList[i];
      if (item && item.id === id) {
        return item;
      }
    }
    return undefined;
  }

  private findReuseableSubWindow(parentId: string, width: number, height: number): ISubWindowInfo | undefined {
    let reusableSubWindowList = this.reusableSubWindow.get(parentId);
    if (!reusableSubWindowList) {
      return undefined;
    }
    for (let i = 0; i < reusableSubWindowList.length; i++) {
      const reusableWindow = reusableSubWindowList[i];
      if (!reusableWindow) {
        continue;
      }
      const rect = reusableWindow.subWindow.getWindowProperties().windowRect;
      const maxDistance:number = 2;
      if (Math.abs(rect.width - width) <= maxDistance && Math.abs(rect.height - height) <= maxDistance) {
        reusableSubWindowList.splice(i, 1);
        LogUtil.error(TAG, `reusableWindow ${reusableWindow.id} size ${reusableSubWindowList.length}`);
        this.subWindowList.push(reusableWindow);
        return reusableWindow;
      }
    }
    return undefined;
  }

  private createNewWindow(parentId: string, id: string, r: WindowBound, subStorage: LocalStorage) {
    let windowStage = this.abilityManager.getWindowStage(parentId);

    windowStage?.createSubWindow(id, (err, data) => {
      if (err.code) {
        LogUtil.error(TAG, 'Failed to create the subWindow. Cause: ' + JSON.stringify(err));
        return;
      }
      let windowClass = data;
      windowClass.keepKeyboardOnFocus(true);
      this.subWindowList.push({
        id: id,
        parentId: parentId,
        subWindow: windowClass,
        localStorage: subStorage
      });
      this.dragParamManager.addDragContextData({
        id: id,
        uiContext: undefined,
      });

      windowClass.on('windowSizeChange', (size) => {
        const prop = data.getWindowProperties();
        let windowBound: WindowBound = {
          left: prop.windowRect.left,
          top: prop.windowRect.top,
          width: size.width,
          height: size.height
        };
        this.nativeContext.OnWindowSizeChange(id, windowBound);
      });

      this.moveSubWindow(windowClass, r.left, r.top);
      if (r.width > 0 && r.height > 0) {
        this.resizeSubWindow(windowClass,r.width, r.height);
      }
      windowClass.loadContent('pages/SubWindow', subStorage, (err) => {
        if (err.code) {
          LogUtil.error(TAG, 'Failed to load the content. Cause:' + JSON.stringify(err));
          return;
        }
        let uiContext: UIContext = windowClass.getUIContext();
        this.dragParamManager.setUiContext(id, uiContext);
        windowClass.setWindowFocusable(false).then(() => {
          windowClass.showWindow((err) => {
            if (err.code) {
              LogUtil.error(TAG, 'Failed to show the subWindow. Cause: ' + JSON.stringify(err));
              return;
            }
          });
        }).catch(() => {
          LogUtil.error(TAG, 'set focusable false failed')
        });
      });
    });
  }

  private async resizeSubWindow(windowClass: window.Window, width: number, height: number) {
    return await windowClass.resize(width, height);
  }

  private moveSubWindow(windowClass: window.Window, x: number, y: number) {
    windowClass.moveWindowTo(x, y, (err) => {
      if (err.code) {
        LogUtil.error(TAG, 'Failed to move the subWindow. Cause:' + JSON.stringify(err));
        return;
      }
    });
  }

  @LogMethod
  cancelSubWindow(id: string) {
    for (let i = 0; i < this.subWindowList.length; i++) {
      const item = this.subWindowList[i];
      if (item && item.id === id) {
        item.subWindow.minimize();
        this.addReuseWindow(item);
        this.subWindowList.splice(i, 1);
        break;
      }
    }
  }

  private addReuseWindow(item: ISubWindowInfo) {
    if (!item) {
      LogUtil.error(TAG, 'addReuseWindow fail');
    }
    let reuseWindowList = this.reusableSubWindow.get(item.parentId);
    if (reuseWindowList) {
      reuseWindowList.push(item);
      if (reuseWindowList.length > MAX_REUSABLE_WINDOW_NUM) {
        reuseWindowList.pop();
        this.dragParamManager.removeDragContextData(item.id);
      }
    } else {
      this.reusableSubWindow.set(item.parentId, [item]);
    }
  }

  @LogMethod
  hideSubWindow(id: string) {
    const item = this.findSubWindow(id);
    item?.subWindow.minimize().catch((err: BusinessError) => {
      LogUtil.error(TAG, 'Failed to hide SubWindow. Cause: ' + JSON.stringify(err));
    });
  }

  @LogMethod
  showSubWindow(id: string) {
    const item = this.findSubWindow(id);
    item?.subWindow.showWindow().catch((err: BusinessError) => {
      LogUtil.error(TAG, 'Failed to show SubWindow. Cause: ' + JSON.stringify(err));
    });
  }

  @LogMethod
  setSubWindowBounds(id: string, r: WindowBound) {
    try {
      let windowClass = window.findWindow(id);
      windowClass?.resize(r.width, r.height, (err) => {
        if (err.code) {
          LogUtil.error(TAG, 'Failed to change the subWindow size. Cause:' + JSON.stringify(err));
          return;
        }
      });
      windowClass?.moveWindowTo(r.left, r.top, (err) => {
        if (err.code) {
          LogUtil.error(TAG, 'Failed to move the subwindow. Cause:' + JSON.stringify(err));
          return;
        }
      });
    } catch (exception) {
      LogUtil.error(TAG, 'Failed to find the Window. Cause: ' + JSON.stringify(exception));
    }
  }
}
