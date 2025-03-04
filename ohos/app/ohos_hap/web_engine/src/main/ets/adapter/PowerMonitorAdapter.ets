// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import { injectable, inject } from 'inversify';
import Base from "@ohos.base";
import BatteryInfo from '@ohos.batteryInfo';
import CommonEventManager from '@ohos.commonEventManager';
import { BaseAdapter } from '../common/BaseAdapter';
import LogUtil from '../utils/LogUtil';
import common from '@ohos.app.ability.common';
import { MODULE_TYPE } from "../common/ModuleType";
import LogMethod from '../common/LogDecorator';

const TAG: string = 'PowerMonitor';
const ENTER_FORCE_SLEEP: string = "usual.event.ENTER_FORCE_SLEEP";
const EXIT_FORCE_SLEEP: string = "usual.event.EXIT_FORCE_SLEEP";
const POWER_CONNECTED: string = "usual.event.POWER_CONNECTED";
const POWER_DISCONNECTED: string = "usual.event.POWER_DISCONNECTED";
const subscribeInfo: CommonEventManager.CommonEventSubscribeInfo = {
  events: [
    ENTER_FORCE_SLEEP,
    EXIT_FORCE_SLEEP,
    POWER_CONNECTED,
    POWER_DISCONNECTED,
  ],
};

let adapterOwner : PowerMonitorAdapter | undefined = undefined;

function callbackFunc(err: Base.BusinessError, data: CommonEventManager.CommonEventData) {
  if (err) {
    LogUtil.error(TAG, `subscribe failed, code is ${err.code}, message is ${err.message}, data is ${data}`);
    return;
  }
  const evt = data.event;
  switch (evt) {
    case ENTER_FORCE_SLEEP:
      adapterOwner?.getNativeContext().PowerMonitor.OnSuspend();
      break;
    case EXIT_FORCE_SLEEP:
      adapterOwner?.getNativeContext().PowerMonitor.OnResume();
      break;
    case POWER_CONNECTED:
    case POWER_DISCONNECTED:
      adapterOwner?.getNativeContext().PowerMonitor.OnPowerStateChanged();
      break;
    default:
      LogUtil.warn(TAG, `PowerMonitor unknown event: ${evt}`);
      return;
  }
}

@injectable()
export default class PowerMonitorAdapter extends BaseAdapter {
  private subscriber: CommonEventManager.CommonEventSubscriber | undefined = undefined;

  constructor(@inject(MODULE_TYPE.Context) ctx: common.UIAbilityContext) {
    super(ctx);
    this.subscribePowerEvent();
    adapterOwner = this;
  }

  private subscribePowerEvent() {
    try {
      this.subscriber = CommonEventManager.createSubscriberSync(subscribeInfo);
      CommonEventManager.subscribe(this.subscriber, callbackFunc)
    } catch (error) {
      let err: Base.BusinessError = error as Base.BusinessError;
      LogUtil.error(TAG, `create PowerMonitor SubscriberSync failed, code is ${err.code}, message is ${err.message}`);
    }
  }

  @LogMethod
  isOnBatteryPower(): boolean {
    return BatteryInfo.pluggedType == BatteryInfo.BatteryPluggedType.NONE;
  }
}
