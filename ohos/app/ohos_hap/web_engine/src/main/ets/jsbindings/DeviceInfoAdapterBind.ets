// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import DeviceInfoAdapter from '../adapter/DeviceInfoAdapter';
import Inject from "../common/InjectModule";
import JsBindingUtils from '../utils/JsBindingUtils';

function implDeviceInfoAdapter(): DeviceInfoAdapter {
  return Inject.get(DeviceInfoAdapter);
}

function get(prop:string): string {
  return implDeviceInfoAdapter().Get(prop);
}

export default class DeviceInfoAdapterBind {
  static bind() {
    JsBindingUtils.bindFunction('DeviceInfoAdapter.Get', get);
  }
}