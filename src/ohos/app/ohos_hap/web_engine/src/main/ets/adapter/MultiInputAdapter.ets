// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import common from '@ohos.app.ability.common';
import { inject, injectable } from 'inversify';
import { BaseAdapter } from '../common/BaseAdapter';
import { PanAction } from '../common/Constants';
import LogMethod from '../common/LogDecorator';
import { MODULE_TYPE } from "../common/ModuleType";

@injectable()
export default class MultiInputAdapter extends BaseAdapter {
  constructor(
    @inject(MODULE_TYPE.Context) ctx: common.UIAbilityContext
  ) {
    super(ctx);
  }

  private isPanEventValid(event: GestureEvent | undefined): boolean {
    if (!event || event.sourceTool === SourceTool.Unknown) {
      return false;
    }
    // When sourceTool is mouse, only pass mouse wheel event trigger
    // by mouse middle button, ignore wheel event trigger by long
    // pressing and moving
    if (event.sourceTool === SourceTool.MOUSE) {
      if (event.fingerList.length > 0 && event.fingerList[0].id === 0) {
        return true;
      }
      return false;
    }
    return true;
  }

  OnPanEvent(action: number, id: string, event: GestureEvent | undefined) {
    let isValidEvent = false;
    switch (action) {
      case PanAction.kCancel:
        isValidEvent = true;
        break;
      default:
        isValidEvent = this.isPanEventValid(event);
    }

    if (isValidEvent) {
      this.nativeContext.OnPanEventCB(action, id, event);
    }
  }

  OnPinchEvent(pinch_step: string, id: string, event: GestureEvent) {
    this.nativeContext.OnPinchEventCB(pinch_step, id, event);
  }
}