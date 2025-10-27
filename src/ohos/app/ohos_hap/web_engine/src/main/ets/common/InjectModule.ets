// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import common from '@ohos.app.ability.common';
import { Container, interfaces } from 'inversify';
import { MODULE_TYPE } from "./ModuleType";
import { adapterModule } from './AdapterModule';
import { commonModule } from './CommonModule';

export default class InjectModule {
  private static container = new Container({ defaultScope: "Singleton" });
  private static context: common.UIAbilityContext;
  private static isInject: boolean = false;

  static inject(ctx: common.UIAbilityContext) {
    if (InjectModule.isInject) {
      return;
    }

    try {
      InjectModule.context = ctx;
      InjectModule.container.bind<common.Context>(MODULE_TYPE.Context).toDynamicValue(
        () => {
          return InjectModule.context;
        });
      InjectModule.container.load(commonModule, adapterModule);
      InjectModule.isInject = true;
    } catch(err) {
      throw new Error('inject Module failed: ' + JSON.stringify(err));
    }
  }

  static get<T>(serviceIdentifier: interfaces.ServiceIdentifier<T>): T {
    return InjectModule.container.get<T>(serviceIdentifier);
  }

  static destroy() {
    InjectModule.container.unbindAll();
  }
}
