// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import AbilityStage from '@ohos.app.ability.AbilityStage';
import { ContextType } from '../common/Constants';
import JsBindingMethod from '../jsbindings/JsBindingMethod';
import JsBindingUtils from '../utils/JsBindingUtils';
import LogUtil from '../utils/LogUtil';
import Want from '@ohos.app.ability.Want';
import { GlobalThisHelper } from '../utils/GlobalThisHelper';
import { NativeContext } from '../interface/CommonInterface';

export class WebAbilityStage extends AbilityStage {
  private static TAG: string = 'WebAbilityStage';

  private nativeContext: NativeContext | undefined = undefined;

  onCreate(): void {
    JsBindingUtils.initNativeContext(ContextType.kMainProcess);
    this.nativeContext = JsBindingUtils.getNativeContext(ContextType.kMainProcess);
    this.runTaskAsync();
  }

  onAcceptWant(want: Want): string {
    let instanceKey = want.parameters?.instanceKey;
    if (instanceKey) {
      return instanceKey.toString();
    }
    if (GlobalThisHelper.isLaunched()) {
      let lastActive: number | undefined = this.nativeContext?.GetLastActive();
      if (lastActive) {
        return 'browser' + lastActive;
      }
    }
    return 'browser1';
  }

  private runTaskAsync() {
    setTimeout(() => {
      JsBindingMethod.bind();
    }, 0)
  }
}