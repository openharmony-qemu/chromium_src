// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import { injectable, inject } from 'inversify';
import ConfigurationConstant from '@ohos.app.ability.ConfigurationConstant';
import { BaseAdapter } from '../common/BaseAdapter';
import common from '@ohos.app.ability.common';
import { MODULE_TYPE } from "../common/ModuleType";
import LogMethod from '../common/LogDecorator';

@injectable()
export default class NativeThemeAdapter extends BaseAdapter {
  private themeSource: ConfigurationConstant.ColorMode = ConfigurationConstant.ColorMode.COLOR_MODE_NOT_SET;

  constructor(@inject(MODULE_TYPE.Context) ctx: common.UIAbilityContext) {
    super(ctx);
    if (this.context.config.colorMode !== undefined) {
      this.themeSource = this.context.config.colorMode;
    }
  }

  @LogMethod
  getSystemNativeTheme(): ConfigurationConstant.ColorMode {
    this.context.getApplicationContext().setColorMode(ConfigurationConstant.ColorMode.COLOR_MODE_NOT_SET);
    return this.themeSource;
  }

  setSystemNativeTheme(themeSource: ConfigurationConstant.ColorMode = ConfigurationConstant.ColorMode.COLOR_MODE_NOT_SET) {
    this.nativeContext.SetThemeSource(themeSource);
  }
};