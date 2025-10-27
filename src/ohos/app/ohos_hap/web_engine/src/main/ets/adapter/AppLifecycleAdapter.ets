// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import common from '@ohos.app.ability.common';
import { inject, injectable } from 'inversify';
import { AbilityManager } from '../common/AbilityManager';
import { BaseAdapter } from '../common/BaseAdapter';
import LogMethod from '../common/LogDecorator';
import { MODULE_TYPE } from "../common/ModuleType";
import LogUtil from '../utils/LogUtil';

enum WebStatus {
  kInit = 0,
  kStart,
  kDestroy,
}

const TAG: string = 'AppLifecycleAdapter';

@injectable()
export default class AppLifecycleAdapter extends BaseAdapter {
  private abilityManager: AbilityManager;
  private webStatus: WebStatus;

  constructor(
    @inject(AbilityManager) abilityManager: AbilityManager,
    @inject(MODULE_TYPE.Context) ctx: common.UIAbilityContext
  ) {
    super(ctx);
    this.abilityManager = abilityManager;
    this.webStatus = WebStatus.kInit;
  }

  @LogMethod
  onWebCreate() {
    this.webStatus = WebStatus.kStart;
  }

  @LogMethod
  onWebDestroy() {
    this.webStatus = WebStatus.kDestroy;
    let context = this.abilityManager.getLastContext();
    context?.terminateSelf((err) => {
      if (err.code) {
        LogUtil.error(TAG, `Failed to terminate Self. Code is ${err.code}, message is ${err.message}`);
        return;
      }
      LogUtil.info(TAG, "terminateSelf ok");
    });
  }

  getWebStatus(): number {
    return this.webStatus;
  }
}
