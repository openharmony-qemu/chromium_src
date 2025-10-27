// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import { injectable } from 'inversify';
import common from '@ohos.app.ability.common';
import { Want, wantConstant } from '@kit.AbilityKit';

import { BaseAdapter } from '../common/BaseAdapter';
import LogUtil from '../utils/LogUtil';
import LogMethod from '../common/LogDecorator';

@injectable()
export default class FileManagerAdapter extends BaseAdapter {
  private static TAG: string = 'FileManagerAdapter';

  @LogMethod
  openItemInFolder(filePath: string) {
    let context = getContext() as common.UIAbilityContext;
    let encodedFilePath = encodeURI(filePath);
    this.openFolder(encodedFilePath, context);
  }

  private async openFolder(fileUri: string, context?: common.UIAbilityContext): Promise<void> {
    let want: Want = {
      bundleName: 'com.huawei.hmos.filemanager',
      abilityName: 'MainAbility',
      parameters: {
        'fileUri': fileUri
      }
    };

    try {
      context?.startAbilityForResult(want);
    } catch (error) {
      LogUtil.error(FileManagerAdapter.TAG,
        'try to open url fail openFolder err = ' + JSON.stringify(error));
    }
  }

  @LogMethod
  openVerifiedItem(filePath: string) {
    let encodedFilePath = encodeURI(filePath);
    let context = getContext() as common.UIAbilityContext;
    const want: Want = {
      // Configure read and write permissions for shared files,
      // such as granting read and write authorization to shared applications
      flags: wantConstant.Flags.FLAG_AUTH_WRITE_URI_PERMISSION | wantConstant.Flags.FLAG_AUTH_READ_URI_PERMISSION,
      action: "ohos.want.action.viewData",
      uri: encodedFilePath
    }
    context.startAbility(want);
  }
}