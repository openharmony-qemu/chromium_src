// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import ConfigurationConstant from '@ohos.app.ability.ConfigurationConstant';
import JsBindingUtils from '../utils/JsBindingUtils';
import Inject from "../common/InjectModule";
import NativeThemeAdapter from '../adapter/NativeThemeAdapter';

function implNativeThemeAdapter(): NativeThemeAdapter {
  return Inject.get(NativeThemeAdapter);
}

function getSystemNativeTheme(): ConfigurationConstant.ColorMode {
  return implNativeThemeAdapter().getSystemNativeTheme();
}

export default class NativeThemeAdapterBind {
  static bind() {
    JsBindingUtils.bindFunction("NativeThemeAdapter.GetSystemNativeTheme", getSystemNativeTheme);
  }
}