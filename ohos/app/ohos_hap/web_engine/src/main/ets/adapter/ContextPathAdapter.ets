// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import { injectable, inject } from 'inversify';
import environment from '@ohos.file.environment';

import { BaseAdapter } from '../common/BaseAdapter';
import LogUtil from '../utils/LogUtil';
import LogMethod from '../common/LogDecorator';

const TAG: string = 'ContextPath';

@injectable()
export default class ContextPathAdapter extends BaseAdapter {
  @LogMethod
  getCacheDir(): string {
    return this.context.getApplicationContext().cacheDir;
  }

  @LogMethod
  getTempDir(): string {
    return this.context.getApplicationContext().tempDir;
  }

  @LogMethod
  getFilesDir(): string {
    return this.context.getApplicationContext().filesDir;
  }

  @LogMethod
  getBundleDir(): string {
    return this.context.bundleCodeDir;
  }

  @LogMethod
  getUserDocumentDir(): string {
    let userDir: string = this.getFilesDir() + '/UserDocument';
    try {
      userDir = environment.getUserDocumentDir();
    } catch (error) {
      LogUtil.error(TAG, `getUserDocumentDir failed, caused by:${JSON.stringify(error)}, rollback to sandbox
      dir`);
    }
    return userDir;
  }

  @LogMethod
  getUserDesktopDir(): string {
    let userDir: string = this.getFilesDir() + '/UserDesktop';
    try {
      userDir = environment.getUserDesktopDir();
    } catch (error) {
      LogUtil.error(TAG, `GetUserDesktopDir failed, caused by:${JSON.stringify(error)}, rollback to sandbox`);
    }
    return userDir;
  }

  @LogMethod
  getUserDownloadDir(): string {
    let userDir: string = this.getFilesDir() + '/UserDownload';
    try {
      userDir = environment.getUserDownloadDir();
    } catch (error) {
      LogUtil.error(TAG, `GetUserDownloadDir failed, caused by:${JSON.stringify(error)}, rollback to sandbox`);
    }
    return userDir;
  }
}