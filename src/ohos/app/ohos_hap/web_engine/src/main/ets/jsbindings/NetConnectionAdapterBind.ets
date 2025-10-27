// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import JsBindingUtils from '../utils/JsBindingUtils';
import Inject from "../common/InjectModule";
import NetConnectionAdapter from '../adapter/NetConnectionAdapter';

function implNetConnectionAdapter(): NetConnectionAdapter {
  return Inject.get(NetConnectionAdapter);
}

function netAvailable(callback: Function): void {
  implNetConnectionAdapter().netAvailable(callback);
}

function netCapabilitiesChange(callback: Function): void {
  implNetConnectionAdapter().netCapabilitiesChange(callback);
}

function netConnectionPropertiesChange(callback: Function): void {
  implNetConnectionAdapter().netConnectionPropertiesChange(callback);
}

function netUnavailable(callback: Function): void {
  implNetConnectionAdapter().netUnavailable(callback);
}

function netLost(callback: Function): void {
  implNetConnectionAdapter().netLost(callback);
}

function unregisterAll(): void {
  implNetConnectionAdapter().unregisterAll();
}

function showSystemSettings(uri: string, callback: (ret: boolean) => void, subUri?: string): void {
  implNetConnectionAdapter().showSystemSettings(uri, callback, subUri);
}

export default class NetConnectionAdapterBind {
  static bind() {
    JsBindingUtils.bindFunction("NetConnection.NetAvailable", netAvailable);
    JsBindingUtils.bindFunction("NetConnection.NetCapabilitiesChange", netCapabilitiesChange);
    JsBindingUtils.bindFunction("NetConnection.NetConnectionPropertiesChange", netConnectionPropertiesChange);
    JsBindingUtils.bindFunction("NetConnection.NetUnavailable", netUnavailable);
    JsBindingUtils.bindFunction("NetConnection.NetLost", netLost);
    JsBindingUtils.bindFunction("NetConnection.UnregisterAll", unregisterAll);
    JsBindingUtils.bindFunction("NetConnection.ShowSystemSettings", showSystemSettings);
  }
}