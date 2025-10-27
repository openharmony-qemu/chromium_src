// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import account_osAccount from '@ohos.account.osAccount';
import { injectable } from 'inversify';
import certManager from '@ohos.security.certManager';
import { BusinessError } from '@ohos.base';
import fs, { Filter } from '@ohos.file.fs';

import { BaseAdapter } from '../common/BaseAdapter';
import LogUtil from '../utils/LogUtil';
import LogMethod from '../common/LogDecorator';

class ListFileOption {
  public recursion: boolean = false;
  public listNum: number = 0;
  public filter: Filter = {};
}

interface OhosCertInfo {
  cert: string;
  uri: string;
}

@injectable()
export default class CertManagerAdapter extends BaseAdapter {
  private static TAG: string = 'CertManagerAdapter';
  private static uri_separator: string = '\n';

  get uriPath(): string {
    return `${this.context.filesDir}/uris`;
  }

  writeFile(filePath: string, str: string) {
    let file = fs.openSync(filePath, fs.OpenMode.READ_WRITE | fs.OpenMode.CREATE);
    fs.writeSync(file.fd, str);
    fs.closeSync(file);
  }

  readFile(filePath: string) : string {
    let res = fs.accessSync(filePath);
    if (!res) {
      return '';
    }
    let fileStat = fs.lstatSync(filePath);
    let file = fs.openSync(filePath, fs.OpenMode.READ_ONLY | fs.OpenMode.CREATE);
    let buf = new ArrayBuffer(fileStat.size);
    fs.readSync(file.fd, buf);
    fs.closeSync(file);
    return this.uint8ArrayToString(new Uint8Array(buf));
  }

  replaceFileContent(filePath: string, strDelete: string) {
    let content: string = this.readFile(filePath);
    fs.unlinkSync(filePath);
    this.writeFile(filePath, content.replace(strDelete, ''));
  }

  @LogMethod
  installPersonalCert(certData: ArrayBuffer, certPass: string, alias: string, callback: (ready: number) => void) {
    const keystore: Uint8Array =  new Uint8Array(certData);
    const keystorePwd: string = certPass;
    // TODO The alias can contain only letters and digits. Special characters will be supported in later versions.
    certManager.installPrivateCertificate(keystore, keystorePwd, alias).then((cmResult) => {
      let uri: string = cmResult.uri || '';
      let uris: string = this.readFile(uri);
      let index = uris.indexOf(uri);
      if (index === -1) {
        uri += CertManagerAdapter.uri_separator;
        this.writeFile(this.uriPath, uri);
      }
      callback(0);
    }).catch((err: BusinessError) => {
      LogUtil.error(CertManagerAdapter.TAG, `installPrivateCertificate failed, code: ${err.code}`);
      callback(err.code);
    });
  }

  private uint8ArrayToString(data: Uint8Array) {
    return data.reduce((prev, current) => prev + String.fromCharCode(current), '');
  }

  @LogMethod
  async getAllPrivateCertificates(callback: Function) {
    let uris: string = this.readFile(this.uriPath);
    let urisArray = uris.split(CertManagerAdapter.uri_separator) || [];
    const certInfos: OhosCertInfo[] = [];
    if (urisArray.length < 1) {
      callback(null, 0);
      return;
    }
    for (let i = 0; i < urisArray.length - 1; i++) {
      let cmResult = await certManager.getPrivateCertificate(urisArray[i]);
      if (cmResult.credential != undefined && cmResult.credential.keyNum > 0) {
        const certInfo: OhosCertInfo = {
          uri: urisArray[i],
          cert: this.uint8ArrayToString(cmResult.credential.credentialData)
        };
        certInfos.push(certInfo);
      }
    }
    callback(certInfos, certInfos.length);
  }

  @LogMethod
  removeCertByUri(uriparam: string, callback: Function) {
    certManager.uninstallPrivateCertificate(uriparam).then(() => {
      this.replaceFileContent(this.uriPath, uriparam + CertManagerAdapter.uri_separator);
      callback(0);
    }).catch((err: BusinessError) => {
      LogUtil.error(CertManagerAdapter.TAG, `uninstallPrivateCertificate failed, code: ${err.code}`);
      callback(err.code);
    });
  }

  @LogMethod
  signByCertUri(uri: string, inputData: ArrayBuffer, callback: Function): void {
    const input: Uint8Array =  new Uint8Array(inputData);
    const buffer_return = new ArrayBuffer(0);
    const SignReq: certManager.CMSignatureSpec = {
      purpose: certManager.CmKeyPurpose.CM_KEY_PURPOSE_SIGN,
      padding: certManager.CmKeyPadding.CM_PADDING_PKCS1_V1_5,
      digest: certManager.CmKeyDigest.CM_DIGEST_SHA256
    }
    certManager.init(uri, SignReq, (err: BusinessError, SignHandle: certManager.CMHandle) => {
      if (err) {
        LogUtil.error(CertManagerAdapter.TAG, `certManager init sign failed, code: ${err.code}`);
        callback(buffer_return);
        return;
      }
      const SignHandle_const = SignHandle.handle;
      certManager.update(SignHandle_const, input, (err: BusinessError) => {
        if (err) {
          LogUtil.error(CertManagerAdapter.TAG, `certManager update sign failed, code: ${err.code}`);
          callback(buffer_return);
          return;
        }
        certManager.finish(SignHandle_const).then((cmResult) => {
          callback(cmResult.outData || buffer_return);
        }).catch((err: BusinessError) => {
          LogUtil.error(CertManagerAdapter.TAG, `certManager finish sign failed, code: ${err.code}`);
          callback(buffer_return);
        });
      });
    })
  }

  async getUserId(): Promise<number> {
    let accountManager: account_osAccount.AccountManager = account_osAccount.getAccountManager();
    let localId: number = await accountManager.getOsAccountLocalId();
    return await Promise.resolve(localId);
  }

  @LogMethod
  async returnUserId(callback: Function) {
    let userid: number = await this.getUserId();
    callback(userid);
  }

  @LogMethod
  async getCertCrl(pathDirPrefix:string, getUserIdFlag: boolean, callback: Function) {
    let option = new ListFileOption();
    let pathDir: string;
    if (getUserIdFlag) {
      let userid: number = await this.getUserId();
      pathDir = pathDirPrefix + userid;
    } else {
      pathDir = pathDirPrefix;
    }

    let cert_crl = new Array<string>();
    let res = fs.accessSync(pathDir);
    if (!res) {
      callback(cert_crl, 0);
      return;
    }
    let filenames = fs.listFileSync(pathDir, option);

    for (let i = 0; i < filenames.length; i++) {
      let filePath: string = pathDir + '/' + filenames[i];
      let buf: string = this.readFile(filePath);
      cert_crl.push(buf);
    }
    callback(cert_crl, cert_crl.length);
  }

}
