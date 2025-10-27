// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import { injectable } from 'inversify';
import { BusinessError } from '@ohos.base';
import common from '@ohos.app.ability.common';
import { Want } from '@kit.AbilityKit';

import { BaseAdapter } from '../common/BaseAdapter';
import LogUtil from '../utils/LogUtil';
import LogMethod from '../common/LogDecorator';

@injectable()
export default class ExternalProtocolAdapter extends BaseAdapter {
  private static TAG: string = 'ExternalProtocolAdapter';

  @LogMethod
  openExternal(urlStr: string) {
    let context = getContext() as common.UIAbilityContext;
    const want: Want = {
      uri: urlStr
    }
    context.startAbility(want).catch((err: BusinessError) => {
      LogUtil.error(ExternalProtocolAdapter.TAG, `Failed to start ability:` + JSON.stringify(err));
    });
  }
}