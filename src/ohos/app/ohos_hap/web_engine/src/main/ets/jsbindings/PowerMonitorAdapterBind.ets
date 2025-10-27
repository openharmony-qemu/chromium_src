// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import JsBindingUtils from '../utils/JsBindingUtils';
import Inject from "../common/InjectModule";
import PowerMonitorAdapter from '../adapter/PowerMonitorAdapter';

function implPowerMonitorAdapter(): PowerMonitorAdapter {
  return Inject.get(PowerMonitorAdapter);
}

function isOnBatteryPower(): boolean {
  return implPowerMonitorAdapter().isOnBatteryPower();
}

export default class PowerMonitorAdapterBind {
  static bind() {
    JsBindingUtils.bindFunction('PowerMonitor.IsOnBatteryPower', isOnBatteryPower);
  }
}