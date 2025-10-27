// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import JsBindingUtils from '../utils/JsBindingUtils';
import Inject from '../common/InjectModule';
import FileManagerAdapter from '../adapter/FileManagerAdapter';

function implFileManagerAdapter(): FileManagerAdapter {
  return Inject.get(FileManagerAdapter);
}

function openItemInFolder(filePath: string) {
  implFileManagerAdapter().openItemInFolder(filePath);
}

function openVerifiedItem(filePath: string) {
  implFileManagerAdapter().openVerifiedItem(filePath);
}

export default class FileManagerAdapterBind {
  static bind() {
    JsBindingUtils.bindFunction("FileManagerAdapter.OpenItemInFolder", openItemInFolder);
    JsBindingUtils.bindFunction("FileManagerAdapter.OpenVerifiedItem", openVerifiedItem);
  }
}