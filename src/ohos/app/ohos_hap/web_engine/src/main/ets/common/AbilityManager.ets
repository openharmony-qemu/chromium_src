// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import window from '@ohos.window';
import common from '@ohos.app.ability.common';
import { injectable } from 'inversify';

interface IAbilityParams {
  windowStage: window.WindowStage,
  context: common.UIAbilityContext,
  id: string
};

AppStorage.setOrCreate('abilityParamsList', []);

@injectable()
export class AbilityManager {
  private appStorage: IAbilityParams[];

  private activeContext: common.UIAbilityContext | undefined;

  constructor() {
    let appStorageLink: SubscribedAbstractProperty<IAbilityParams[]> = AppStorage.link('abilityParamsList');
    this.appStorage = appStorageLink.get();
  }

  private paramNormalization(abilityId: string | number): string {
    if (typeof abilityId === 'number') {
      return 'browser' + abilityId;
    }
    return abilityId;
  }

  public addAbilityData(data: IAbilityParams) {
    for (let i = 0; i < this.appStorage.length; i++) {
      if (data.id === this.appStorage[i].id) {
        return;
      }
    }
    this.appStorage.push(data);
  }

  public removeAbilityData(abilityId: string | number) {
    let id = this.paramNormalization(abilityId);
    for (let i = 0; i < this.appStorage.length; i++) {
      if (id === this.appStorage[i].id) {
        this.appStorage.splice(i, 1);
        break;
      }
    }
  }

  public getContext(abilityId: string | number): common.UIAbilityContext | undefined {
    let id = this.paramNormalization(abilityId);
    for (let i = 0; i < this.appStorage.length; i++) {
      if (id === this.appStorage[i].id) {
        return this.appStorage[i].context;
      }
    }
    return undefined;
  }

  public getWindowStage(abilityId: string | number): window.WindowStage | undefined {
    let id = this.paramNormalization(abilityId);
    for (let i = 0; i < this.appStorage.length; i++) {
      if (id === this.appStorage[i].id) {
        return this.appStorage[i].windowStage;
      }
    }
    return undefined;
  }

  public getLastContext(): common.UIAbilityContext | undefined {
    if (this.isEmpty()) {
      return undefined;
    }
    return this.appStorage[0].context;
  }

  public getLastWindowStage(): window.WindowStage | undefined {
    if (this.isEmpty()) {
      return undefined;
    }
    return this.appStorage[0].windowStage;
  }

  public setActiveContext(context: common.UIAbilityContext) {
    this.activeContext = context;
  }

  public getActiveContext(): common.UIAbilityContext | undefined {
    if (this.isEmpty()) {
      return undefined;
    }
    return this.activeContext;
  }

  public getMainWindow(abilityId: string | number | common.UIAbilityContext): window.Window | undefined {
    if (typeof(abilityId) === "string" || typeof(abilityId) === "number") {
      let id = this.paramNormalization(abilityId);
      for (let i = 0; i < this.appStorage.length; i++) {
        if (id === this.appStorage[i].id) {
          return this.appStorage[i].windowStage.getMainWindowSync();
        }
      }
    } else {
      for (let item of this.appStorage) {
        if (item.context === abilityId) {
          return item.windowStage.getMainWindowSync();
        }
      }
    }
    return undefined;
  }

  public isEmpty(): boolean {
    return this.appStorage.length === 0;
  }

  public getAbilityCount(): number {
    return this.appStorage.length;
  }
};
