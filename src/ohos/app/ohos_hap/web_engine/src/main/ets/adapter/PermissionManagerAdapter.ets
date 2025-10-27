// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import { injectable, inject } from 'inversify';
import abilityAccessCtrl from '@ohos.abilityAccessCtrl';
import { Permissions } from '@ohos.abilityAccessCtrl';
import { BusinessError } from '@ohos.base';
import bundleManager from '@ohos.bundle.bundleManager';
import { AbilityManager } from '../common/AbilityManager';
import { BaseAdapter } from '../common/BaseAdapter';
import LogUtil from '../utils/LogUtil';
import LogMethod from '../common/LogDecorator';
import { MODULE_TYPE } from '../common/ModuleType';
import prompt from '@ohos.promptAction'
import { common, Want } from '@kit.AbilityKit';

const CONFIRM: number = 0;
const ERROR: number = -1;

@injectable()
export default class PermissionManagerAdapter extends BaseAdapter {
  private static TAG: string = 'PermissionManagerAdapter';
  private readonly needPermissions: Map<string, Array<Permissions>> = new Map([
    ['location', [
      'ohos.permission.APPROXIMATELY_LOCATION',
      'ohos.permission.LOCATION',
    ]],
    ['microphone', [
      'ohos.permission.MICROPHONE',
    ]],
    ['camera', [
      'ohos.permission.CAMERA',
    ]],
    ['pasteboard', [
      'ohos.permission.READ_PASTEBOARD'
    ]],
    ['bluetooth', [
      'ohos.permission.ACCESS_BLUETOOTH',
    ]],
    ['directory_download', [
      'ohos.permission.READ_WRITE_DOWNLOAD_DIRECTORY',
    ]],
    ['directory_desktop', [
      'ohos.permission.READ_WRITE_DESKTOP_DIRECTORY',
    ]],
    ['directory_document', [
      'ohos.permission.READ_WRITE_DOCUMENTS_DIRECTORY',
    ]]
  ]);
  private abilityManager: AbilityManager;
  private atManager: abilityAccessCtrl.AtManager;
  private bundleFlags = bundleManager.BundleFlag.GET_BUNDLE_INFO_WITH_APPLICATION;
  private tokenId: number = 0;
  private titleTextId: number;
  private contentTextId: number;
  private confirmTextId: number;
  private cancelTextId: number;
  private appLabelId: number;

  constructor(
    @inject(MODULE_TYPE.Context) ctx: common.UIAbilityContext,
    @inject(AbilityManager) abilityManager: AbilityManager,
  ) {
    super(ctx);
    this.abilityManager = abilityManager;
    this.atManager = abilityAccessCtrl.createAtManager();
    this.titleTextId = $r("app.string.download_dialog_title").id;
    this.contentTextId = $r("app.string.download_dialog_content").id;
    this.confirmTextId = $r("app.string.download_dialog_confirm_button").id;
    this.cancelTextId = $r("app.string.download_dialog_cancel_button").id;
    this.appLabelId = ctx.abilityInfo.labelId;

    try {
      bundleManager.getBundleInfoForSelf(this.bundleFlags).then((data) => {
        let appInfo: bundleManager.ApplicationInfo = data.appInfo;
        this.tokenId = appInfo.accessTokenId;
      }).catch((err: BusinessError) => {
        LogUtil.error(PermissionManagerAdapter.TAG, `getBundleInfoForSelf failed. Cause: ${err.message}`);
      });
    } catch (err) {
      let message = (err as BusinessError).message;
      LogUtil.error(PermissionManagerAdapter.TAG, `getBundleInfoForSelf failed.: ${message}`);
    }
  }

  @LogMethod
  requestPermissions(permissionType: string, callback: (granted: boolean) => void) {
    const permissions = this.needPermissions.get(permissionType);
    if (!permissions) {
      LogUtil.error(PermissionManagerAdapter.TAG, 'unsupported permission:' + permissionType);
      callback(false);
      return;
    }
    let activeContext = this.abilityManager.getActiveContext();
    if (!activeContext) {
      LogUtil.error(PermissionManagerAdapter.TAG, 'no active context');
      callback(false);
      return;
    }
    
    try {
      this.atManager.requestPermissionsFromUser(activeContext, permissions || []).then((data) => {
        let grantStatus: Array<number> = data.authResults;
        for (let i = 0; i < grantStatus.length; i++) {
          if (grantStatus[i] !== 0) {
            LogUtil.error(PermissionManagerAdapter.TAG, 'err:' + JSON.stringify(data));
            callback(false);
            return;
          }
        }
        callback(true);
      }).catch((err: BusinessError) => {
        LogUtil.error(PermissionManagerAdapter.TAG, 'err:' + JSON.stringify(err));
        callback(false);
      });
    } catch (err) {
      LogUtil.error(PermissionManagerAdapter.TAG, 'err:' + JSON.stringify(err));
      callback(false);
    }
  }

  @LogMethod
  checkPermissions(permissionType: string, callback: (granted: boolean) => void): void {
    const permissions = this.needPermissions.get(permissionType) as Array<Permissions>;
    if (!permissions) {
      LogUtil.error(PermissionManagerAdapter.TAG, 'unsupported permission:' + permissionType);
      callback(false);
      return;
    }
    let grantStatus: abilityAccessCtrl.GrantStatus = abilityAccessCtrl.GrantStatus.PERMISSION_DENIED;
    for (let i = 0; i < permissions.length; i++) {
      try {
        grantStatus = this.atManager.checkAccessTokenSync(this.tokenId, permissions[i]);
        if (grantStatus !== abilityAccessCtrl.GrantStatus.PERMISSION_GRANTED) {
          LogUtil.error(PermissionManagerAdapter.TAG, 'checkPermission error');
          callback(false);
          return;
        }
      } catch (err) {
        LogUtil.error(PermissionManagerAdapter.TAG,
          `checkAccessToken failed, code is ${err.code}, message is ${err.message}`);
        callback(false);
      }
    }
    callback(true);
  }

  @LogMethod
  async openPermissionConfirm(callback: Function) {
    let context = getContext() as common.UIAbilityContext;
    let title = context.resourceManager.getStringSync(this.titleTextId);
    let label = context.resourceManager.getStringSync(this.appLabelId);
    let content = context.resourceManager.getStringSync(this.contentTextId);
    content = content.replace("label", label);
    let confirm = context.resourceManager.getStringSync(this.confirmTextId);
    let cancel = context.resourceManager.getStringSync(this.cancelTextId);
    prompt.showDialog({
      title: title,
      message: content,
      buttons: [
        {
          text: confirm,
          color: '#000000'
        },
        {
          text: cancel,
          color: '#000000'
        }
      ],
    }).then(data => {
      if (data.index === CONFIRM) {
        let wantInfo: Want = {
          bundleName: 'com.huawei.hmos.settings',
          abilityName: 'com.huawei.hmos.settings.MainAbility',
          uri: 'privacy_settings',
          parameters: {
            pushParams: 'com.huawei.ohos_chromium'
          }
        }
        context.startAbility(wantInfo);
      }
      callback(data.index);
    }).catch((err: Error) => {
      callback(ERROR);
      LogUtil.error(PermissionManagerAdapter.TAG, `openPermissionConfirm failed, caused by:${JSON.stringify(err)}`);
    });
  }
}
