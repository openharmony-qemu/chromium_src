// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import UIAbility from '@ohos.app.ability.UIAbility';
import LogUtil from '../utils/LogUtil';
import window from '@ohos.window';
import Want from '@ohos.app.ability.Want';
import AbilityConstant from '@ohos.app.ability.AbilityConstant';
import NativeThemeAdapter from '../adapter/NativeThemeAdapter';
import { AbilityManager } from '../common/AbilityManager';
import { Configuration } from '@ohos.app.ability.Configuration';
import { UIContext } from '@kit.ArkUI';
import Inject from "../common/InjectModule";
import { DragParamManager } from '../common/DragParamManager';
import JsBindingUtils from '../utils/JsBindingUtils';
import { NativeContext, WindowBound } from '../interface/CommonInterface';
import { ConfigData, ContextType } from '../common/Constants';
import { GlobalThisHelper } from '../utils/GlobalThisHelper';
import CheckEmptyUtils from '../utils/CheckEmptyUtils';
import FilePickerAdapter from '../adapter/FilePickerAdapter';
import DisplayAdapter from  '../adapter/DisplayAdapter';

const TAG: string = 'WebAbility';

export class WebAbility extends UIAbility {
  private xcomponentId: string = '';
  private startUri: string = '';
  private abilityManager: AbilityManager | undefined = undefined;
  private displayAdapter : DisplayAdapter | undefined = undefined;
  private dragParamManager: DragParamManager | undefined = undefined;
  private nativeThemeAdapter: NativeThemeAdapter | undefined = undefined;
  private filePickerAdapter: FilePickerAdapter | undefined = undefined;  
  private nativeContext: NativeContext = JsBindingUtils.getNativeContext(ContextType.kMainProcess);
  private hideTitleBar: boolean = true;

  onConfigurationUpdated(config: Configuration) {
    this.nativeThemeAdapter?.setSystemNativeTheme(config.colorMode);
    if (config.fontSizeScale !== undefined) {
      this.displayAdapter?.onSystemFontSizeChange(config.fontSizeScale);
    } else {
      this.displayAdapter?.onSystemFontSizeChange(1.0);
    }
  }

  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam) {
    this.initParameters(want);

    if (!GlobalThisHelper.isLaunched()) {
      GlobalThisHelper.appInit(this.context);
      if (CheckEmptyUtils.isEmpty(this.xcomponentId)) {
        LogUtil.info(TAG, 'onCreate, set the xcomponent default value when start the app');
        this.xcomponentId = ConfigData.DEFAULT_WINDOW_ID;
      }
    }

    this.abilityManager = Inject.get(AbilityManager);
    this.dragParamManager = Inject.get(DragParamManager);
    this.nativeThemeAdapter = Inject.get(NativeThemeAdapter);
    this.displayAdapter = Inject.get(DisplayAdapter);
  }

  onWindowStageCreate(windowStage: window.WindowStage) {
    if (this.checkSingleInstance()) {
      LogUtil.warn(TAG, 'repeat launch from desktop, need terminate');
      return;
    }

    windowStage.getMainWindow((err, data) => {
      if (err.code) {
        LogUtil.error(TAG, 'Failed to obtain the main window. Cause: ' + JSON.stringify(err));
        return;
      }

      try {
        data.on('windowSizeChange', (size) => {
          const prop = data.getWindowProperties();
          let windowBound: WindowBound = {
            left: prop.windowRect.left + prop.drawableRect.left,
            top: prop.windowRect.top + prop.drawableRect.top,
            width: size.width - prop.drawableRect.left - prop.drawableRect.left,
            height: size.height - prop.drawableRect.left - prop.drawableRect.top,
          };
          this.nativeContext.OnWindowSizeChange(this.xcomponentId, windowBound);
        });
      } catch (exception) {
        LogUtil.error(TAG,'Failed to enable the listener for window size changes. Cause: ' + JSON.stringify(exception));
      }
 
      try {
        data.on("windowRectChange", (option) => {
          this.nativeContext.OnWindowRectChange(this.xcomponentId, option.rect, option.reason.valueOf());
        });
      } catch (exception) {
        LogUtil.error(TAG,'Failed to enable the listener for window rect changes. Cause: ' + JSON.stringify(exception));
      }
 
      try {
        data.on('windowEvent', (type) => {
          this.updateAbilityData(type);
          this.nativeContext.OnWindowEvent(this.xcomponentId, type.valueOf());
        });
      } catch (exception) {
        LogUtil.error(TAG, 'Failed to register callback. Cause: ' + JSON.stringify(exception));
      }
      
      try {
        data.on("windowVisibilityChange", (visible) => {
          this.nativeContext.OnWindowVisibilityChange(this.xcomponentId, visible);
        });
      } catch (exception) {
        LogUtil.error(TAG, 'Failed to register callback. Cause: ' + JSON.stringify(exception));
      }
    });

    this.abilityManager?.addAbilityData({
      id: this.xcomponentId,
      context: this.context,
      windowStage: windowStage
    });

    // Bug fix: OH does not report the window show event when the first window start
    if (this.xcomponentId == ConfigData.DEFAULT_WINDOW_ID) {
      this.abilityManager?.setActiveContext(this.context);
    }
    
    this.dragParamManager?.addDragContextData({
      id: this.xcomponentId,
      uiContext: undefined,
    });
    const para: Record<string, string> = {
      'xcomponentId': this.xcomponentId,
      'startUri': this.startUri
    };
    let storage: LocalStorage = new LocalStorage(para);

    windowStage.loadContent('pages/Index', storage, (err, data) => {
      if (err.code) {
        LogUtil.error(TAG, 'Failed to load the content. Cause: ' + JSON.stringify(err) ?? '');
        return;
      }

      let window: window.Window = windowStage.getMainWindowSync();
      // hide title bar
      window.setWindowDecorVisible(!this.hideTitleBar);
 
      let uiContext: UIContext = window.getUIContext();
      this.dragParamManager?.setUiContext(this.xcomponentId, uiContext);
    });
  }

  onWindowStageDestroy() {
    if (CheckEmptyUtils.isEmpty(this.xcomponentId)) {
      LogUtil.warn(TAG, 'onWindowStageDestroy component is not instantiated');
      return;
    }
    // Main window is destroyed, release UI related resources
    this.abilityManager?.removeAbilityData(this.xcomponentId);
    this.dragParamManager?.removeDragContextData(this.xcomponentId);
  }

  onNewWant(want: Want, launchParam: AbilityConstant.LaunchParam): void {
    if (want["uri"] !== undefined && want["uri"] !== '') {
      let uri = want['uri'] as string;
      this.nativeContext.startNewWindow(uri.replace("file://docs/", "file:///"));
    }
  }

  onPrepareToTerminate(): boolean {
    this.nativeContext.OnRequestCloseWindow(this.xcomponentId);
    return true;
  }

  private initParameters(want: Want) {
    if (want !== undefined && want.parameters !== undefined) {
      if (want.parameters['xcomponentId'] !== undefined) {
        this.xcomponentId = want.parameters['xcomponentId'] as string;
      }
      if (want.parameters["hideTitleBar"] !== undefined) {
        this.hideTitleBar = want.parameters['hideTitleBar'] as boolean;
      }
    }

    if (want["uri"] != undefined) {
      let storePath = want['uri'] as string;
      // convert storage path to uri
      this.startUri = storePath.replace("file://docs/", "file:///");
    }
  }

  private checkSingleInstance(): boolean {
    if (!CheckEmptyUtils.isEmpty(this.xcomponentId)) {
      return false;
    }
    this.nativeContext.startNewWindow(this.startUri);
    this.context.terminateSelf((err) => {
      if (err.code) {
        LogUtil.error(TAG, `Failed to terminate Self. Code is ${err.code}, message is ${err.message}`);
        return;
      }
    });
    return true;
  }

  private updateAbilityData(type: window.WindowEventType) {
    switch (type) {
      case window.WindowEventType.WINDOW_SHOWN:
      case window.WindowEventType.WINDOW_ACTIVE:
        this.abilityManager?.setActiveContext(this.context);
      default:
        break;
    }
  }
};
