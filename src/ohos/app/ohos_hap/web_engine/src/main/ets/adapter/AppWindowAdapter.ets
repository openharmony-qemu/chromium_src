// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import common from '@ohos.app.ability.common';
import bundleManager from '@ohos.bundle.bundleManager';
import { inject, injectable } from 'inversify';
import { BusinessError } from '@ohos.base';
import { Size, window } from '@kit.ArkUI';
import Want from '@ohos.app.ability.Want';
import { BaseAdapter } from '../common/BaseAdapter';
import LogUtil from '../utils/LogUtil';
import { AbilityManager } from '../common/AbilityManager';
import LogMethod from '../common/LogDecorator';
import { MODULE_TYPE } from "../common/ModuleType";
import { StartOptions } from '@kit.AbilityKit';
import { NewWindowParam, WindowBound } from '../interface/CommonInterface';

const TAG: string = 'AppWindow';

@injectable()
export default class AppWindowAdapter extends BaseAdapter {
  private abilityManager: AbilityManager;
  private defaultScreenSize: Size = { width: 2560, height: 1600 };
  private bundleName: string = 'com.huawei.ohos_chromium';
  private bundleFlags = bundleManager.BundleFlag.GET_BUNDLE_INFO_WITH_APPLICATION |
                        bundleManager.BundleFlag.GET_BUNDLE_INFO_WITH_METADATA;

  constructor(
    @inject(AbilityManager) abilityManager: AbilityManager,
    @inject(MODULE_TYPE.Context) ctx: common.UIAbilityContext
  ) {
    super(ctx);
    this.abilityManager = abilityManager;

    try {
      bundleManager.getBundleInfoForSelf(this.bundleFlags).then((data) => {
        this.bundleName = data?.name;
      }).catch((err: BusinessError) => {
        LogUtil.error(TAG, `getBundleInfoForSelf failed. Cause: ${err.message}`);
      });
    } catch (err) {
      let message = (err as BusinessError).message;
      LogUtil.error(TAG, `getBundleInfoForSelf failed.: ${message}`);
    }
  }

  @LogMethod
  createWindow(param: NewWindowParam) {
    const want: Want = {
      bundleName: this.bundleName,
      abilityName: 'EntryAbility',
      parameters: {
        'xcomponentId': param.id,
        'hideTitleBar': param.hide_title_bar,
        instanceKey: param.id
      }
    };

    let context = this.getContext();
    if (!context) {
      LogUtil.warn(TAG, 'ability is in terminating state');
      context = this.abilityManager.getLastContext() as common.UIAbilityContext;
    }

    let windowClass = this.abilityManager.getMainWindow(context);
    const mainWindowProp = windowClass?.getWindowProperties();
    let xComBorder: number = mainWindowProp?.drawableRect.left || 0;
    let xComTop: number = mainWindowProp?.drawableRect.top || 0;
    let options: StartOptions = {
      windowLeft: param.left - xComBorder,
      windowTop: param.top - xComTop,
      windowWidth: param.width + xComBorder + xComBorder,
      windowHeight: param.height + xComBorder + xComTop
    }

    context?.startAbility(want, options)
      .catch((err: BusinessError) => {
        LogUtil.error(TAG, 'Failed to start ability. Cause:' + JSON.stringify(err));
      });
  }

  @LogMethod
  getScreenWidth() {
    return this.defaultScreenSize.width;
  }

  @LogMethod
  getScreenHeight() {
    return this.defaultScreenSize.height;
  }

  @LogMethod
  closeWindow(id: number) {
    if (this.abilityManager.getAbilityCount() === 1) {
      LogUtil.warn(TAG, 'When there is only one tab page, we must waiting for the browser thread to exit safely');
      return;
    }
    let context = this.abilityManager.getContext(id);
    context?.terminateSelf();
  }

  @LogMethod
  showWindow(id: number) {
    let windowClass = this.abilityManager.getMainWindow(id);
    windowClass?.showWindow((err: BusinessError) => {
      LogUtil.info(TAG, 'show the window: ' + JSON.stringify(err));
    });
  }

  @LogMethod
  activateWindow(id: number) {
    this.showWindow(id);
  }

  @LogMethod
  setFullscreen(id: number) {
    let windowClass = this.abilityManager.getMainWindow(id);
    if (!windowClass) {
      LogUtil.error(TAG, 'Failed to obtain the main window.');
      return;
    }

    windowClass.maximize(window.MaximizePresentation.ENTER_IMMERSIVE)
      .then(() => {
        LogUtil.info(TAG, 'Succeeded in maximizing the window.');
      }).catch((err: BusinessError) => {
      LogUtil.error(TAG, 'Failed to maximize the window. Cause:' + JSON.stringify(err));
    });
  }

  @LogMethod
  setBounds(id: number,
            bounds: WindowBound,
            callback?: Function) {
    let windowClass = this.abilityManager.getMainWindow(id);
    if (!windowClass) {
      LogUtil.error(TAG, 'Failed to obtain the main window.');
      return;
    }

    const mainWindowProp = windowClass?.getWindowProperties();

    // xComponent bounds need to add a border
    let xComBorder: number = mainWindowProp?.drawableRect.left || 0;
    let xComTop: number = mainWindowProp?.drawableRect.top || 0;
    bounds.left -= xComBorder;
    bounds.top -= xComTop;
    bounds.width = bounds.width + xComBorder + xComBorder;
    bounds.height = bounds.height + xComBorder + xComTop;

    try {
      windowClass.moveWindowToAsync(bounds.left, bounds.top)
        .then(() => {
          windowClass?.resizeAsync(bounds.width, bounds.height)
            .catch((err: BusinessError) => {
              LogUtil.error(TAG, 'Failed to change the window size. Cause:' + JSON.stringify(err));
              callback && callback();
            });
        }).catch((err: BusinessError) => {
        LogUtil.error(TAG, 'Failed to move the window. Cause:' + JSON.stringify(err));
      });
    } catch (exception) {
      LogUtil.error(TAG, 'move the window exception. Cause:' + JSON.stringify(exception));
    }
  }

  @LogMethod
  setEnabled(enable: boolean, id: number) {
    let windowClass = this.abilityManager.getMainWindow(id);
    windowClass?.setWindowTouchable(enable, (err: BusinessError) => {
      if (err.code) {
        LogUtil.error(TAG, 'Failed to set the window to be touchable. Cause:'+ JSON.stringify(err));
      }
    });
  }

  @LogMethod
  setWindowLimits(minWidth: number,
    minHeight: number,
    maxWidth: number,
    maxHeight: number,
    id: number) {
    let windowClass = this.abilityManager.getMainWindow(id);
    const mainWindowProp = windowClass?.getWindowProperties();
    let xComBorder: number = mainWindowProp?.drawableRect.left || 0;
    let xComTop: number = mainWindowProp?.drawableRect.top || 0;

    try {
      let windowLimits: window.WindowLimits = {
        minWidth: minWidth === 0 ? minWidth : minWidth + xComBorder + xComBorder,
        minHeight: minHeight === 0 ? minHeight : minHeight + xComTop + xComBorder,
        maxWidth: maxWidth === 0 ? maxWidth : maxWidth + xComBorder + xComBorder,
        maxHeight: maxHeight === 0 ? maxHeight : maxHeight + xComTop + xComBorder,
      };
      windowClass?.setWindowLimits(windowLimits)
        .catch((err: BusinessError) => {
          LogUtil.error(TAG, 'Failed to change the window limits. Cause: ' + JSON.stringify(err));
        });
    } catch (exception) {
      LogUtil.error(TAG, 'change the window limits exception. Cause:' + JSON.stringify(exception));
    }
  }

  @LogMethod
  maximize(id: number) {
    let windowClass = this.abilityManager.getMainWindow(id);
    if (!windowClass) {
      LogUtil.error(TAG, 'Failed to obtain the main window.');
      return;
    }

    try {
      windowClass.maximize(window.MaximizePresentation.EXIT_IMMERSIVE).then(() => {
        LogUtil.info(TAG, 'Succeeded in maximizing the window.');
      }).catch((err: BusinessError) => {
        LogUtil.error(TAG, 'Failed to maximize the window. Cause:' + JSON.stringify(err));
      });
    } catch (exception) {
      LogUtil.error(TAG, 'maximize the window exception. Cause:' + JSON.stringify(exception));
    }
  }

  @LogMethod
  unMaximize(id: number) {
    let windowClass = this.abilityManager.getMainWindow(id);
    if (!windowClass) {
      LogUtil.error(TAG, 'Failed to obtain the main window.');
      return;
    }

    try {
      windowClass.recover().then(() => {
        LogUtil.info(TAG, 'Succeeded in setting the system bar to be invisible.');
      }).catch((err: BusinessError) => {
        LogUtil.error(TAG, 'Failed to recover the window. Cause: ' + JSON.stringify(err));
      });
    } catch (exception) {
      LogUtil.error(TAG, 'recover the window exception. Cause:' + JSON.stringify(exception));
    }
  }

  @LogMethod
  minimize(id: number) {
    let windowClass = this.abilityManager.getMainWindow(id);
    windowClass?.minimize().catch((err: BusinessError) => {
      LogUtil.error(TAG, 'Failed to minimize the window. Cause: ' + JSON.stringify(err));
    });
  }

  @LogMethod
  setTitle(title:string, id: number) {
    let context = this.abilityManager.getContext(id);
    context?.setMissionLabel(title, (result: BusinessError) => {
      LogUtil.info(TAG, `set title result: ${JSON.stringify(result)}`);
    });
  }
};

