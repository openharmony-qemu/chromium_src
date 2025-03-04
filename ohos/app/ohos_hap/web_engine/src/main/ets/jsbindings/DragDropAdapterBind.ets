// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import JsBindingUtils from '../utils/JsBindingUtils';
import Inject from "../common/InjectModule";
import DragDropAdapter from '../adapter/DragDropAdapter';
import { OhosDragParamToJs } from '../interface/CommonInterface';

function implDragDropAdapter(): DragDropAdapter {
  return Inject.get(DragDropAdapter);
}

function startDrag(dragParams: OhosDragParamToJs) {
  implDragDropAdapter().startDrag(dragParams);
}

export default class DragDropAdapterBind {
  static bind() {
    JsBindingUtils.bindFunction('DragDropAdapter.StartDrag', startDrag);
  }
}