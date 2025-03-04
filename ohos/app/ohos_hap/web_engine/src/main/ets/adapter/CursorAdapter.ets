// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import common from '@ohos.app.ability.common';
import { inject, injectable } from 'inversify';
import { AbilityManager } from '../common/AbilityManager';
import { BaseAdapter } from '../common/BaseAdapter';
import LogMethod from '../common/LogDecorator';
import pointer from '@ohos.multimodalInput.pointer';
import { image } from '@kit.ImageKit';
import { MODULE_TYPE } from "../common/ModuleType";
import LogUtil from '../utils/LogUtil';
import { BusinessError } from '@kit.BasicServicesKit';

const TAG: string = 'Cursor';

@injectable()
export default class CursorAdapter extends BaseAdapter {
  private abilityManager: AbilityManager;

  constructor(
    @inject(AbilityManager) abilityManager: AbilityManager,
    @inject(MODULE_TYPE.Context) ctx: common.UIAbilityContext
  ) {
    super(ctx);
    this.abilityManager = abilityManager;
  }

  @LogMethod
  setCursor(id: number, type: number) {
    let window = this.abilityManager.getMainWindow(id);
    if (window) {
      let windowId = window.getWindowProperties().id;
      pointer.setPointerStyle(windowId, type).catch((error: BusinessError) => {
        LogUtil.error(TAG, "Failed to set cursor. Cause: " + JSON.stringify(error));
      });
    }
  }

  @LogMethod
  setCursorVisible(visible: boolean) {
    pointer.setPointerVisible(visible).catch((error: BusinessError) => {
      LogUtil.error(TAG, "Failed to set cursor visible. Cause: " + JSON.stringify(error));
    });
  }

  @LogMethod
  setCustomCursor(id: string, width: number, height: number, hotspotX: number, hotspotY: number, buff: ArrayBuffer) {
    let opts: image.InitializationOptions =
      { editable: false, pixelFormat: image.PixelMapFormat.BGRA_8888, size: { height: height, width: width } };
    image.createPixelMap(buff, opts).then((cursorImage: image.PixelMap) => {
      let win = this.abilityManager.getMainWindow(id);
      if (win !== undefined) {
        let windowId = win.getWindowProperties().id;
        pointer.setCustomCursorSync(windowId, cursorImage, hotspotX, hotspotY);
      }
    });
  }
}
