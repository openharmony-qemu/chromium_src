// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import JsBindingUtils from '../utils/JsBindingUtils';
import Inject from "../common/InjectModule";
import TracingAdapter from '../adapter/TracingAdapter';

function implTracingAdapter(): TracingAdapter {
  return Inject.get(TracingAdapter);;
}

function registerTracingEventCallback(callback: Function): void {
  implTracingAdapter().registerTracingEventCallback(callback);
}

function unregisterTracingEventCallback(): void {
  implTracingAdapter().unregisterTracingEventCallback();
}

export default class TracingAdapterBind {
  static bind() {
    JsBindingUtils.bindFunction('TracingAdapter.RegisterTracingCallback', registerTracingEventCallback);
    JsBindingUtils.bindFunction('TracingAdapter.UnregisterTracingCallback', unregisterTracingEventCallback);
  }
}
