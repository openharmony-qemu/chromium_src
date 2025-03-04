// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import JsBindingUtils from '../utils/JsBindingUtils';
import Inject from "../common/InjectModule";
import FilePickerAdapter from '../adapter/FilePickerAdapter';

function implFilePickerAdapter(): FilePickerAdapter {
  return Inject.get(FilePickerAdapter);
}

function showDocumentViewPicker(multi_files: boolean, filters: Array<Array<string>>, callback: (path: string) => void) {
  implFilePickerAdapter().showDocumentViewPicker(multi_files, filters, callback);
}

function showDirDocumentViewPicker(file_access_persist: boolean, callback: (path: string) => void) {
  implFilePickerAdapter().showDirDocumentViewPicker(file_access_persist, callback);
}

function showSaveAsDocumentViewPicker(file_name: string, dir_name: string, filters: Array<Array<string>>,
  callback: (path: string) => void) {
  implFilePickerAdapter().showSaveAsDocumentViewPicker(file_name, dir_name, filters,callback);
}

export default class FilePickerAdapterBind {
  static bind() {
    JsBindingUtils.bindFunction("FilePickerAdapter.ShowDocumentViewPicker", showDocumentViewPicker);
    JsBindingUtils.bindFunction("FilePickerAdapter.ShowDirDocumentViewPicker", showDirDocumentViewPicker);
    JsBindingUtils.bindFunction("FilePickerAdapter.ShowSaveAsDocumentViewPicker", showSaveAsDocumentViewPicker);
  }
}