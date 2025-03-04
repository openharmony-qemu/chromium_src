// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import JsBindingUtils from '../utils/JsBindingUtils';
import Inject from "../common/InjectModule";
import DisplayAdapter from '../adapter/DisplayAdapter';

function implDisplayAdapter(): DisplayAdapter {
  return Inject.get(DisplayAdapter);
}

function getDefaultDisplay(callback?: Function) {
  implDisplayAdapter().getDefaultDisplay(callback);
}

function getALLDisplays(callback?: Function) {
  implDisplayAdapter().getALLDisplays(callback);
}

function registerDisplayMonitor() {
  implDisplayAdapter().registerDisplayMonitor();
}

export default class DisplayAdapterBind {
  static bind() {
    JsBindingUtils.bindFunction('OhosDisplayAdapter.GetDefaultDisplay', getDefaultDisplay);
    JsBindingUtils.bindFunction('OhosDisplayAdapter.GetALLDisplays', getALLDisplays);
    JsBindingUtils.bindFunction('OhosDisplayAdapter.registerDisplayMonitor', registerDisplayMonitor);
  }
}