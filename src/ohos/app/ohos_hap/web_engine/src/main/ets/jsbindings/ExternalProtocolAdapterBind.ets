// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import JsBindingUtils from '../utils/JsBindingUtils';
import Inject from "../common/InjectModule";
import ExternalProtocolAdapter from '../adapter/ExternalProtocolAdapter';

function implExternalProtocolAdapter(): ExternalProtocolAdapter {
  return Inject.get(ExternalProtocolAdapter);
}

function openExternal(urlStr: string) {
  implExternalProtocolAdapter().openExternal(urlStr);
}

export default class ExternalProtocolAdapterBind {
  static bind() {
    JsBindingUtils.bindFunction("ExternalProtocolAdapter.OpenExternal", openExternal);
  }
}