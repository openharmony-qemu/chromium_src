// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import JsBindingUtils from '../utils/JsBindingUtils';
import Inject from "../common/InjectModule";
import IMFAdapter from '../adapter/IMFAdapter';
import {
  IMFAdapterCursorInfo,
  IMFAdapterInputAttribute,
  IMFAdapterTextConfig } from '../interface/CommonInterface';

function implIMFAdapter(): IMFAdapter {
  return Inject.get(IMFAdapter);
}

function attachTextInput(textConfigIMF: IMFAdapterTextConfig, callback: (ret: boolean) => void)  {
  implIMFAdapter().attachTextInput(textConfigIMF, callback);
}

function cursorUpdate(cursorInfoIMF: IMFAdapterCursorInfo) {
  implIMFAdapter().cursorUpdate(cursorInfoIMF);
}

function detachTextInput(callback: (ret: boolean) => void) {
  implIMFAdapter().detachTextInput(callback);
}

function updateAttribute(inputAttributeIMF: IMFAdapterInputAttribute) {
  implIMFAdapter().updateAttribute(inputAttributeIMF);
}

function showTextInput() {
  implIMFAdapter().showTextInput();
}

export default class IMFAdapterBind {
  static bind() {
    JsBindingUtils.bindFunction("IMFAdapter.AttachTextInput", attachTextInput);
    JsBindingUtils.bindFunction("IMFAdapter.CursorUpdate", cursorUpdate);
    JsBindingUtils.bindFunction("IMFAdapter.DetachTextInput", detachTextInput);
    JsBindingUtils.bindFunction("IMFAdapter.UpdateAttribute", updateAttribute);
    JsBindingUtils.bindFunction("IMFAdapter.ShowTextInput", showTextInput);
  }
}