// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import JsBindingUtils from '../utils/JsBindingUtils';
import Inject from '../common/InjectModule';
import PermissionManagerAdapter from '../adapter/PermissionManagerAdapter';

function implPermissionManagerAdapter(): PermissionManagerAdapter {
  return Inject.get(PermissionManagerAdapter);
}

function requestPermissions(permissionType: string, callback: (permissionGranted: boolean) => void) {
  implPermissionManagerAdapter().requestPermissions(permissionType, callback);
}

function checkPermissions(permissionType: string, callback: (permissionGranted: boolean) => void) {
  implPermissionManagerAdapter().checkPermissions(permissionType, callback);
}

async function openPermissionConfirm(callback: (index: number) => void) {
  await implPermissionManagerAdapter().openPermissionConfirm(callback);
}

export default class PermissionManagerAdapterBind {
  static bind() {
    JsBindingUtils.bindFunction('PermissionManagerAdapter.RequestPermissions', requestPermissions);
    JsBindingUtils.bindFunction('PermissionManagerAdapter.CheckPermissions', checkPermissions);
    JsBindingUtils.bindFunction('PermissionManagerAdapter.OpenPermissionConfirm', openPermissionConfirm);
  }
}