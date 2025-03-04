// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import { injectable } from 'inversify';
import { BusinessError } from '@ohos.base';
import display from '@ohos.display';
import { BaseAdapter } from '../common/BaseAdapter';
import LogUtil from '../utils/LogUtil';
import LogMethod from '../common/LogDecorator';

@injectable()
export default class DisplayAdapter extends BaseAdapter {
  private static TAG: string = 'DisplayAdapter';
  private static VIRTUAL_ID: number = 1000;

  @LogMethod
  getDefaultDisplay(callback?: Function) {
    let displayClass: display.Display | null = null;
    try {
      displayClass = display.getDefaultDisplaySync();
      if (callback) {
        callback(displayClass);
      }
    } catch (exception) {
      LogUtil.error(DisplayAdapter.TAG, 'Failed to obtain the default display object. Code: ' + JSON.stringify(exception));
    }

    return displayClass;
  }

  @LogMethod
  getALLDisplays(callback?: Function) {
    try {
      display.getAllDisplays((err: BusinessError, data: Array<display.Display>) => {
        const errCode: number = err.code;
        if (errCode) {
          LogUtil.error(DisplayAdapter.TAG, 'Failed to obtain all the display objects. Code: ' + JSON.stringify(err));
          if (callback) {
            callback([], 0);
          }
          return;
        }
        if (callback) {
          callback(data, data.length);
        }
      });
    } catch (exception) {
      if (callback) {
        callback([], 0);
      }
      LogUtil.error(DisplayAdapter.TAG, 'Failed to obtain all the display object. Code: ' + JSON.stringify(exception));
    }
  }

  @LogMethod
  registerDisplayMonitor() {
    try {
      display.on("add", this.onAddCallback);
      display.on("remove", this.onRemoveCallback);
      display.on("change", this.onChangeCallback);
    } catch (exception) {
      LogUtil.error(DisplayAdapter.TAG, 'Failed to register callback. Code: ' + JSON.stringify(exception));
    }
  }

  private checkVirtualId(displayId: number) : boolean {
    if(displayId >= DisplayAdapter.VIRTUAL_ID){
        return true;
    }
    return false;
  }

  private onAddCallback = (data: number) => {
    try {
      if (this.checkVirtualId(data)) {
        LogUtil.warn(DisplayAdapter.TAG, JSON.stringify(data) + ' is a virtual display in onAddCallback');
        return;
      }
      this.nativeContext.OnDisplayChangeCallback("add", data);
    } catch (exception) {
      LogUtil.error(DisplayAdapter.TAG, 'Add event callback fail. Code: ' + JSON.stringify(exception));
    }
  }

  private onRemoveCallback = (data: number) => {
    try {
      if (this.checkVirtualId(data)) {
        LogUtil.warn(DisplayAdapter.TAG, JSON.stringify(data) + ' is a virtual display in onRemoveCallback');
        return;
      }
      this.nativeContext.OnDisplayChangeCallback("remove", data);
    } catch (exception) {
      LogUtil.error(DisplayAdapter.TAG, 'remove event callback fail. Code: ' + JSON.stringify(exception));
    }
  }

  private onChangeCallback = (data: number) => {
    try {
      if (this.checkVirtualId(data)) {
        LogUtil.warn(DisplayAdapter.TAG, JSON.stringify(data) + ' is a virtual display in onChangeCallback');
        return;
      }
      this.nativeContext.OnDisplayChangeCallback("change", data);
    } catch (exception) {
      LogUtil.error(DisplayAdapter.TAG, 'Change event callback fail. Code: ' + JSON.stringify(exception));
    }
  }

  onSystemFontSizeChange(fontSizeZoom: number) {
    this.nativeContext.OnFontSizeChangeCallback(fontSizeZoom);
  }
}