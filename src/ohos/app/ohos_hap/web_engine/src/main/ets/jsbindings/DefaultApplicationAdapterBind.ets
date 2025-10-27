// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import JsBindingUtils from '../utils/JsBindingUtils';
import Inject from "../common/InjectModule";
import DefaultApplicationAdapter from '../adapter/DefaultApplicationAdapter';

function implDefaultApplication(): DefaultApplicationAdapter {
  return Inject.get(DefaultApplicationAdapter);
}

function isDefaultApplication(): boolean {
  return implDefaultApplication().isDefaultApplication();
}

function startSettingsAbility(): boolean {
  return implDefaultApplication().startSettingsAbility()
}

export default class DefaultApplicationAdapterBind {
  static bind() {
    JsBindingUtils.bindFunction("DefaultApplicationAdapter.isDefaultApplication", isDefaultApplication);
    JsBindingUtils.bindFunction("DefaultApplicationAdapter.StartSettingsAbility", startSettingsAbility);
  }
}
