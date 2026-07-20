// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import { ChildProcessArgs } from '@kit.AbilityKit';
import { WebChildProcess } from 'web_engine/childProcess';

export default class CustomChildProcess extends WebChildProcess {
  onStart(args?: ChildProcessArgs): void {
    super.onStart(args);
  }

  static toString(): string {
    return "CustomChildProcess";
  }
}
