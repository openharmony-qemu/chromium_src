// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import JsBindingUtils from '../utils/JsBindingUtils';
import Inject from "../common/InjectModule";
import RunningLockAdapter from '../adapter/RunningLockAdapter';

function implRunningLockAdapter(): RunningLockAdapter {
  return Inject.get(RunningLockAdapter);
}

function start() {
  implRunningLockAdapter().start();
}

function stop() {
  implRunningLockAdapter().stop();
}

export default class RunningLockAdapterBind {
  static bind() {
    JsBindingUtils.bindFunction("RunningLock.Start", start);
    JsBindingUtils.bindFunction("RunningLock.Stop", stop);
  }
}