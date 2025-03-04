// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import { injectable } from 'inversify';
import { BaseAdapter } from '../common/BaseAdapter';

import deviceInfo from '@ohos.deviceInfo';

const DefaultValue = deviceInfo.versionId;
const DeviceInfo: Map<string, string> = new Map([
  ["manufacture", deviceInfo.manufacture],
  ["productModel", deviceInfo.productModel],
  ["distributionOSName", deviceInfo.distributionOSName],
  ["distributionOSVersion", deviceInfo.distributionOSVersion],
  ["deviceType", deviceInfo.deviceType]
])

@injectable()
export default class DeviceInfoAdapter extends BaseAdapter {
  Get(prop: string): string {
    const info: string = DeviceInfo.get(prop) || DefaultValue;
    return info;
  }
}