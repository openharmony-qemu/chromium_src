// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import JsBindingUtils from '../utils/JsBindingUtils';
import Inject from "../common/InjectModule";
import AccessibilityAdapter from '../adapter/AccessibilityAdapter';


function implAccessibilityAdapter(): AccessibilityAdapter {
  return Inject.get(AccessibilityAdapter);
}

function speech(text: string, id: string) {
  implAccessibilityAdapter().speech(text, id);
}

function shutDown() {
  implAccessibilityAdapter().shutDown();
}

export default class AccessibilityAdapterBind {
  static bind() {
    JsBindingUtils.bindFunction("AccessibilityAdapter.Speech", speech);
    JsBindingUtils.bindFunction("AccessibilityAdapter.ShutDown", shutDown);
  }
}