// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import { injectable } from 'inversify';
import connection from '@ohos.net.connection';
import { BusinessError } from '@ohos.base';
import Want from '@ohos.app.ability.Want';

import { BaseAdapter } from '../common/BaseAdapter';
import LogUtil from '../utils/LogUtil';
import LogMethod from '../common/LogDecorator';

const TAG: string = 'net';

@injectable()
export default class NetConnectionAdapter extends BaseAdapter {
  private netCon: connection.NetConnection | undefined = undefined;
  private registered: boolean = false;

  private getNetConnect(): connection.NetConnection {
    if (!this.netCon) {
      this.netCon = connection.createNetConnection();
    }
    if (!this.registered) {
      this.netCon.register((error: BusinessError) => {
        if (error != null) {
          LogUtil.error(TAG, `register error: errcode: ${error.code} message: ${error.message}.`);
        }
      });
      this.registered = true;
    }
    return this.netCon;
  }

  @LogMethod
  netAvailable(callback: Function): void {
    this.getNetConnect().on('netAvailable', () => {
      callback();
    });
  }

  @LogMethod
  netCapabilitiesChange(callback: Function): void {
    this.getNetConnect().on('netCapabilitiesChange', (data: connection.NetCapabilityInfo) => {
      const bearerTypes: number[] = data.netCap.bearerTypes;
      const type = bearerTypes.length > 0 ? bearerTypes[0] : 0;
      callback(type, 0);
    });
  }

  @LogMethod
  netConnectionPropertiesChange(callback: Function): void {
    this.getNetConnect().on('netConnectionPropertiesChange', () => {
      callback();
    });
  }

  @LogMethod
  netUnavailable(callback: Function): void {
    this.getNetConnect().on('netUnavailable', () => {
      callback();
    });
  }

  @LogMethod
  netLost(callback: Function): void {
    this.getNetConnect().on('netLost', () => {
      callback();
    });
  }

  @LogMethod
  unregisterAll(): void {
    this.getNetConnect().unregister((error: BusinessError) => {
      if (error != null) {
        LogUtil.error(TAG, `unregister error, errcode: ${error.code} message: ${error.message}.`);
      }
    });
  }

  @LogMethod
  showSystemSettings(uri_value: string, callback: (ret: boolean) => void, subUri?: string): void {
    let want: Want = {
      bundleName: 'com.huawei.hmos.settings',
      abilityName: 'com.huawei.hmos.settings.MainAbility',
      uri: uri_value
    }
    if (subUri !== undefined) {
      want.parameters = {
        'subUri': subUri
      }
    }
    this.getContext().startAbility(want).then(() => {
      callback(true);
    }).catch((error: BusinessError) => {
      callback(false);
      LogUtil.error(TAG, `showSystemSettings Failed. Cause: ${JSON.stringify(error)}`);
    });
  }
}
