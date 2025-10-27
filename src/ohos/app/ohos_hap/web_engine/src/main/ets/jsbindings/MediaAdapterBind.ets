// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import JsBindingUtils from '../utils/JsBindingUtils';
import Inject from "../common/InjectModule";
import MediaAdapter from '../adapter/MediaAdapter';

function implMediaAdapter(): MediaAdapter {
  return Inject.get(MediaAdapter);
}

function getPreviewSurfaceId(callback?: Function) {
  implMediaAdapter().getPreviewSurfaceId(callback);
}

function getImageReceiver() {
  implMediaAdapter().getImageReceiver();
}

export default class MediaAdapterBind {
  static bind() {
    JsBindingUtils.bindFunction("mediaAdapter.getPreviewSurfaceId", getPreviewSurfaceId);
    JsBindingUtils.bindFunction("mediaAdapter.getImageReceiver", getImageReceiver);
  }
}
