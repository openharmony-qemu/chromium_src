// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import defaultAppMgr from '@ohos.bundle.defaultAppManager';
import { Want } from '@kit.AbilityKit';
import { BusinessError } from '@ohos.base';
import { injectable } from 'inversify';
import { BaseAdapter } from '../common/BaseAdapter';
import LogMethod from '../common/LogDecorator';
import LogUtil from '../utils/LogUtil';

const TAG: string = 'DefaultApplicationAdapter';

@injectable()
export default class DefaultApplicationAdapter extends BaseAdapter {
  @LogMethod
  isDefaultApplication(): boolean {
    try {
      let data = defaultAppMgr.isDefaultApplicationSync(defaultAppMgr.ApplicationType.BROWSER)
      return data;
    } catch (error) {
      LogUtil.error(TAG, 'Operation failed. Cause: ' + JSON.stringify(error));
      return false;
    }
  }

  @LogMethod
  startSettingsAbility(): boolean {
    const want: Want = {
      bundleName: 'com.huawei.hmos.settings',
      abilityName: 'com.huawei.hmos.settings.MainAbility',
      uri: 'application_info_entry',
      parameters: {
        pushParams: 'com.huawei.hmos.browser'
      }
    };

    try {
      this.getContext().startAbility(want, (err: BusinessError) => {
        if (err.code) {
          LogUtil.error(TAG, `startAbility failed, code is ${err.code}, message is ${err.message}`);
          return false;
        }
        return true;
      });
    } catch (err) {
      let code = (err as BusinessError).code;
      let message = (err as BusinessError).message;
      LogUtil.error(TAG, `startAbility failed, code is ${code}, message is ${message}`);
      return false;
    }
    return true;
  }
}
