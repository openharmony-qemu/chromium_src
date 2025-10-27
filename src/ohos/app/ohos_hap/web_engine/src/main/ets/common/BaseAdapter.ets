// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import common from '@ohos.app.ability.common';
import { ContextType } from './Constants';
import { injectable, inject } from 'inversify';
import JsBindingUtils from '../utils/JsBindingUtils';
import { MODULE_TYPE } from "../common/ModuleType";
import { NativeContext } from '../interface/CommonInterface';
import * as ns from "reflect-metadata"
ns

@injectable()
export abstract class BaseAdapter {
    protected context: common.UIAbilityContext;
    protected nativeContext: NativeContext;

    constructor(@inject(MODULE_TYPE.Context) ctx: common.UIAbilityContext) {
        this.context = ctx;
        this.nativeContext = JsBindingUtils.getNativeContext(ContextType.kMainProcess);
        this.init();
    }
    init(): void {}
    unInit(): void {}
    getNativeContext(): NativeContext {
        return this.nativeContext;
    }
    getContext(): common.UIAbilityContext {
        if (!this.context.isTerminating()) {
            return this.context;
        }
        return getContext(this) as common.UIAbilityContext;
    }
}