// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import JsBindingUtils from '../utils/JsBindingUtils';
import Inject from "../common/InjectModule";
import FontAdapter from '../adapter/FontAdapter';

function implFontAdapter(): FontAdapter {
  return Inject.get(FontAdapter);
}

function getSystemFontList(): Array<string> {
  return implFontAdapter().getSystemFontList();
}

function getConfigFontList(): Array<string> {
  return implFontAdapter().getConfigFontList();
}

function getFontInfo(name: string): string {
  return implFontAdapter().getFontInfo(name);
}

export default class FontAdapterBind {
  static bind() {
    JsBindingUtils.bindFunction("FontAdapter.GetSystemFontList", getSystemFontList);
    JsBindingUtils.bindFunction("FontAdapter.GetConfigFontList", getConfigFontList);
    JsBindingUtils.bindFunction("FontAdapter.GetFontInfo", getFontInfo);
  }
}