// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import JsBindingUtils from '../utils/JsBindingUtils';
import Inject from "../common/InjectModule";
import BatteryAdapter from '../adapter/BatteryAdapter';
import { BatteryInfo } from '../interface/CommonInterface'

function implBatteryAdapter(): BatteryAdapter {
  return Inject.get(BatteryAdapter);
}

function getBatteryInfo(callback: (batteryStatus: BatteryInfo) => void): boolean {
  return implBatteryAdapter().GetBatteryInfo(callback);
}

function registerBatteryUpdateCallback(callback: (batteryStatus: BatteryInfo) => void): boolean {
  return implBatteryAdapter().registerBatteryUpdateCallback(callback);
}

function unregisterBatteryUpdateCallback(): void {
  implBatteryAdapter().unregisterBatteryUpdateCallback();
}

export default class BatteryAdapterBind {
  static bind() {
    JsBindingUtils.bindFunction('BatteryAdapter.GetBatteryInfo', getBatteryInfo);
    JsBindingUtils.bindFunction('BatteryAdapter.RegisterBatteryUpdateCallback', registerBatteryUpdateCallback);
    JsBindingUtils.bindFunction('BatteryAdapter.UnregisterBatteryUpdateCallback', unregisterBatteryUpdateCallback);
  }
}