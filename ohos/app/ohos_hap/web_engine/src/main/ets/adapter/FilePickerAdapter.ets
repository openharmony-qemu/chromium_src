// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import fileUri from '@ohos.file.fileuri';
import fs from '@ohos.file.fs';
import LogUtil from '../utils/LogUtil';
import LogMethod from '../common/LogDecorator';
import picker from '@ohos.file.picker';

import { BaseAdapter } from '../common/BaseAdapter';
import { BusinessError } from '@kit.BasicServicesKit';
import { common } from '@kit.AbilityKit';
import { fileShare } from '@kit.CoreFileKit';
import { inject, injectable } from 'inversify';
import { MODULE_TYPE } from '../common/ModuleType';
import { relationalStore } from '@kit.ArkData';

const STORE_CONFIG: relationalStore.StoreConfig = {
  name: "RdbPicker.db",
  securityLevel: relationalStore.SecurityLevel.S1
}

@injectable()
export default class FilePickerAdapter extends BaseAdapter {
  private static TAG: string = "FilePicker";
  private static PREFIX: string = "file://docs";
  private static MAX_SELECT_NUMBER: number = 500;
  private static VERSION_EMPTY: number = 0;
  private static VERSION_CHANGE: number = 1;
  private static CREATE_TABLE =
    'CREATE TABLE IF NOT EXISTS URIS (ID INTEGER PRIMARY KEY AUTOINCREMENT, URI TEXT NOT NULL)';
  private static DELETE_DUPLICATE_DATA =
    'DELETE FROM URIS WHERE ID NOT IN (SELECT MIN(ID)  FROM URIS GROUP BY URI)';

  constructor(@inject(MODULE_TYPE.Context) ctx: common.UIAbilityContext) {
    super(ctx);
    relationalStore.getRdbStore(ctx, STORE_CONFIG, (err, store) => {
      if (store !== undefined) {
        if (store.version === FilePickerAdapter.VERSION_EMPTY) {
          store.executeSql(FilePickerAdapter.CREATE_TABLE);
          store.version = FilePickerAdapter.VERSION_CHANGE;
        }
        let predicates = new relationalStore.RdbPredicates('URIS');
        let deleteIds: Array<number> = [];
        store.executeSql(FilePickerAdapter.DELETE_DUPLICATE_DATA, (err: BusinessError) => {
          if (err) {
            LogUtil.error(FilePickerAdapter.TAG,
              `Failed to delete duplicate data. Code:${err.code}, message:${err.message}`);
            return
          }
        });
        (store as relationalStore.RdbStore).query(predicates, ['ID', 'URI'], (err: BusinessError, resultSet) => {
          if (err) {
            LogUtil.error(FilePickerAdapter.TAG, `Failed to query data. Code:${err.code}, message:${err.message}`);
            return;
          }
          let uris: Array<string> = [];
          while (resultSet.goToNextRow()) {
            const uri = resultSet.getString(resultSet.getColumnIndex('URI'));
            const id = resultSet.getLong(resultSet.getColumnIndex("ID"));
            let fileUriObject = new fileUri.FileUri(uri);
            let res = fs.accessSync(fileUriObject.path, fs.AccessModeType.EXIST);
            if (res) {
              uris.push(uri);
            } else {
              deleteIds.push(id);
            }
          }
          if (uris.length != 0) {
            this.activeFileAccessPersist(uris);
          }
          resultSet.close();
          if (deleteIds.length > 0) {
            let predicateDelete = new relationalStore.RdbPredicates('URIS');
            predicateDelete.in("ID", deleteIds);
            store.deleteSync(predicateDelete);
          }
        })
      }
    });
  }

  @LogMethod
  showDocumentViewPicker(multi_files: boolean, filters: Array<Array<string>>, callback: (path: string) => void) {
    let DocumentSelectOptions = new picker.DocumentSelectOptions();
    if (multi_files) {
      DocumentSelectOptions.maxSelectNumber = FilePickerAdapter.MAX_SELECT_NUMBER;
    }
    if (filters.length > 0) {
      DocumentSelectOptions.fileSuffixFilters = this.getFilterArray(filters);
    }
    let documentPicker = new picker.DocumentViewPicker();
    documentPicker.select(DocumentSelectOptions).then((selectResult) => {
      selectResult = this.dirFilter(selectResult);
      callback(JSON.stringify(selectResult));
    }).catch((err: Error) => {
      callback('');
      LogUtil.error(FilePickerAdapter.TAG, `DocumentViewPicker.select failed with err: ${JSON.stringify(err)}`);
    });
  }

  @LogMethod
  showDirDocumentViewPicker(file_access_persist: boolean, callback: (path: string) => void) {
    let DocumentSelectOptions = new picker.DocumentSelectOptions();
    DocumentSelectOptions.selectMode = picker.DocumentSelectMode.FOLDER;
    let documentPicker = new picker.DocumentViewPicker();
    documentPicker.select(DocumentSelectOptions).then((selectResult) => {
      try {
        if (file_access_persist) {
          this.fileAccessPersist(selectResult);
        }
        selectResult = this.dirFilter(selectResult);
        callback(JSON.stringify(selectResult));
      } catch (err) {
        callback('');
        LogUtil.error(FilePickerAdapter.TAG,
          `DocumentViewPicker.select Foloder failed with err: ${JSON.stringify(err)}`);
      }
    }).catch((err: Error) => {
      callback('');
      LogUtil.error(FilePickerAdapter.TAG, `DocumentViewPicker.select failed with err: ${JSON.stringify(err)}`);
    });
  }

  @LogMethod
  showSaveAsDocumentViewPicker(file_name: string, dir_name: string, filters: Array<Array<string>>,
    callback: (path: string) => void) {
    let DocumentSaveOptions = new picker.DocumentSaveOptions();
    DocumentSaveOptions.newFileNames = [`${file_name}`];
    DocumentSaveOptions.defaultFilePathUri = FilePickerAdapter.PREFIX + dir_name;
    if (filters.length > 0) {
      DocumentSaveOptions.fileSuffixChoices = ["." + filters[0].join(",.")];
    }
    let documentPicker = new picker.DocumentViewPicker();
    documentPicker.save(DocumentSaveOptions).then((saveResult) => {
      this.fileAccessPersist(saveResult);
      saveResult = this.dirFilter(saveResult);
      callback(JSON.stringify(saveResult));
    }).catch((err: Error) => {
      callback('');
      LogUtil.error(FilePickerAdapter.TAG, `DocumentViewPicker.save failed with err: ${JSON.stringify(err)}`);
    });
  }

  private dirFilter(docs: string[]): string[] {
    let copy: string[] = [];
    docs.forEach((path) => {
      // Convert to uri get systeam path
      let uri = new fileUri.FileUri(path);
      if (uri.authority === 'media') {
        let file: fs.File | undefined;
        let copyFile: fs.File | undefined;
        try {
          let filePath = this.context.tempDir;
          file = fs.openSync(path, fs.OpenMode.READ_ONLY);
          copyFile = fs.openSync(filePath + '/' + uri.name, fs.OpenMode.READ_WRITE | fs.OpenMode.CREATE);
          fs.copyFileSync(file.fd, copyFile.fd);
          copy.push(filePath + '/' + uri.name);
        } catch (err) {
          LogUtil.error(FilePickerAdapter.TAG, `copy failed, code is ${err.code}, message is ${err.message}`);
        } finally {
          try {
            if (file) {
              fs.closeSync(file);
            }
          } catch (err1) {
            LogUtil.error(FilePickerAdapter.TAG, `close file failed, code is ${err1.code}, message is ${err1.message}`);
          }
          try {
            if (copyFile) {
              fs.closeSync(copyFile);
            }
          } catch (err2) {
            LogUtil.error(FilePickerAdapter.TAG,
              `close copyFile failed, code is ${err2.code}, message is ${err2.message}`);
          }
        }
      } else {
        copy.push(uri.path);
      }
    });
    return copy;
  }

  private getFilterArray(filters: Array<Array<String>>): Array<string> {
    let copy = new Array<string>();
    filters.forEach((item) => {
      copy.push("." + item.join(",."));
    })
    return copy;
  }

  private insertValue(path: string[]) {
    let uriBuckets: Array<relationalStore.ValuesBucket> = new Array<relationalStore.ValuesBucket>();
    path.forEach((uri) => {
      let bucket: relationalStore.ValuesBucket = {
        'URI': uri
      };
      uriBuckets.push(bucket);
    });

    let context = getContext() as common.UIAbilityContext;
    relationalStore.getRdbStore(context, STORE_CONFIG, (err, store) => {
      if (store !== undefined) {
        try {
          (store as relationalStore.RdbStore).batchInsertSync('URIS', uriBuckets);
        } catch (err) {
          LogUtil.error(FilePickerAdapter.TAG,
            `insertValue failed, code is ${err.code}, message is ${err.message}`);
        }
      }
    });
  }

  private activeFileAccessPersist(uris: string[]) {
    let policies = new Array<fileShare.PolicyInfo>();
    uris.forEach((uri) => {
      let policyRead: fileShare.PolicyInfo = {
        uri: uri,
        operationMode: fileShare.OperationMode.READ_MODE | fileShare.OperationMode.WRITE_MODE,
      };
      policies.push(policyRead);
    });
    fileShare.activatePermission(policies).then(() => {
      LogUtil.info(FilePickerAdapter.TAG, ` activeFileAccessPersist success`);
    }).catch((err: BusinessError<Array<fileShare.PolicyErrorResult>>) => {
      LogUtil.error(FilePickerAdapter.TAG,
        `activeFileAccessPersist failed, code is ${err.code}, message is ${err.message}`);
    });
  }

  private fileAccessPersist(uris: string[]) {
    let policies = new Array<fileShare.PolicyInfo>();
    uris.forEach((uri) => {
      let policyRead: fileShare.PolicyInfo = {
        uri: uri,
        operationMode: fileShare.OperationMode.READ_MODE | fileShare.OperationMode.WRITE_MODE,
      };
      policies.push(policyRead);
    });
    fileShare.persistPermission(policies).then(() => {
      LogUtil.info(FilePickerAdapter.TAG, ` fileAccessPersist success`);
      this.insertValue(uris);
    }).catch((err: BusinessError<Array<fileShare.PolicyErrorResult>>) => {
      LogUtil.error(FilePickerAdapter.TAG,
        `fileAccessPersist failed, code is ${err.code}, message is ${err.message}`);
    });
  }
}