// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import { injectable } from 'inversify';
import inputMethod from '@ohos.inputMethod';

import { BaseAdapter } from '../common/BaseAdapter';
import { BusinessError } from '@ohos.base';
import LogMethod from '../common/LogDecorator';
import LogUtil from '../utils/LogUtil';

import {
  IMFAdapterInputAttribute,
  IMFAdapterCursorInfo,
  IMFAdapterTextConfig } from '../interface/CommonInterface';

const TAG: string = 'IMFAdapter';

@injectable()
export default class IMFAdapter extends BaseAdapter {
  private inputMethodController = inputMethod.getController();

  @LogMethod
  attachTextInput(textConfigIMF: IMFAdapterTextConfig, callback: (ret: boolean) => void) {
    this.attachTextInputHelper(textConfigIMF, callback)
  }

  @LogMethod
  cursorUpdate(cursorInfoIMF: IMFAdapterCursorInfo) {
    let cursorInfo: inputMethod.CursorInfo = {
      left: cursorInfoIMF.left as number,
      top: cursorInfoIMF.top as number,
      width: cursorInfoIMF.width as number,
      height: cursorInfoIMF.height as number
    };
    this.inputMethodController.updateCursor(cursorInfo, (err: BusinessError) => {
      if (err) {
        LogUtil.error(TAG, `Failed to updateCursor:` + JSON.stringify(err));
        return;
      }
    });
  }

  @LogMethod
  detachTextInput(callback: (ret: boolean) => void) {
    this.detachTextInputHelper(callback);
  }

  @LogMethod
  updateAttribute(inputAttributeIMF: IMFAdapterInputAttribute) {
    let inputAttribute: inputMethod.InputAttribute = {
      textInputType: inputAttributeIMF.inputPattern as inputMethod.TextInputType,
      enterKeyType: inputAttributeIMF.enterKeyType as inputMethod.EnterKeyType
    };
    this.inputMethodController.updateAttribute(inputAttribute, (err: BusinessError) => {
      if (err) {
        LogUtil.error(TAG, `Failed to updateAttribute:` + JSON.stringify(err));
        return;
      }
    });
  }

  @LogMethod
  showTextInput() {
    this.inputMethodController.showTextInput((err: BusinessError) => {
      if (err) {
        LogUtil.error(TAG, `Failed to showTextInput:` + JSON.stringify(err));
        return;
      }
    });
  }

  private attachTextInputHelper(textConfigIMF: IMFAdapterTextConfig, callback: (ret: boolean) => void) {
    let textConfig: inputMethod.TextConfig = {
      inputAttribute: {
        textInputType: textConfigIMF.inputAttribute.inputPattern as inputMethod.TextInputType,
        enterKeyType: textConfigIMF.inputAttribute.enterKeyType as inputMethod.EnterKeyType
      },
      cursorInfo: {
        left: textConfigIMF.cursorInfo.left as number,
        top: textConfigIMF.cursorInfo.top as number,
        width: textConfigIMF.cursorInfo.width as number,
        height: textConfigIMF.cursorInfo.height as number
      }
    };
    this.inputMethodController.attach(true, textConfig).then(() => {
      this.showTextInputHelper(callback);
    }).catch((err: BusinessError) => {
      LogUtil.error(TAG, `Failed to attach:` + JSON.stringify(err));
    });
  }

  private showTextInputHelper(callback: (ret: boolean) => void) {
    this.inputMethodController.showTextInput().then(() => {
      this.onListenIME();
      callback(true);
    }).catch((err: BusinessError) => {
      LogUtil.error(TAG, `Failed to showTextInput:` + JSON.stringify(err));
      callback(false);
    });
  }

  private onListenIME() {
    this.inputMethodController.on('insertText', (text: string) => {
      this.nativeContext.InsertTextCallback(text);
    });
    this.inputMethodController.on('deleteLeft', (length: number) => {
      this.nativeContext.DeleteBackCallback(length);
    });
    this.inputMethodController.on('deleteRight', (length: number) => {
      this.nativeContext.DeleteForwardCallback(length);
    });
    this.inputMethodController.on('sendFunctionKey', () => {
      this.nativeContext.SendEnterKeyEventCallback();
    });
    this.inputMethodController.on('moveCursor', (direction: inputMethod.Direction) => {
      this.nativeContext.MoveCursorCallback(direction);
    });
  }

  private detachTextInputHelper(callback: (ret: boolean) => void) {
    this.inputMethodController.off('insertText');
    this.inputMethodController.off('deleteLeft');
    this.inputMethodController.off('deleteRight');
    this.inputMethodController.off('sendFunctionKey');
    this.inputMethodController.off('moveCursor');
    this.inputMethodController.hideTextInput().then(() => {
      this.inputMethodController.detach().then(() => {
        callback(true);
      }).catch((err: BusinessError) => {
        LogUtil.error(TAG, `Failed to detach:` + JSON.stringify(err));
        callback(false);
      });
    }).catch((err: BusinessError) => {
      LogUtil.error(TAG, `Failed to hideTextInput:` + JSON.stringify(err));
      callback(false);
    });
  }
}
