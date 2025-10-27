// Copyright (c) 2022 Huawei Device Co., Ltd.
// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OH_GIN_JAVASCRIPT_BRIDGE_ERRORS_H
#define OH_GIN_JAVASCRIPT_BRIDGE_ERRORS_H

namespace NWEB {
enum OhGinJavascriptBridgeError {
  kOhGinJavascriptBridgeNoError = 0,
  kOhGinJavascriptBridgeUnknownObjectId,
  kOhGinJavascriptBridgeObjectIsGone,
  kOhGinJavascriptBridgeMethodNotFound,
  kOhGinJavascriptBridgeAccessToObjectGetClassIsBlocked,
  kOhGinJavascriptBridgeJavaExceptionRaised,
  kOhGinJavascriptBridgeNonAssignableTypes,
  kOhGinJavascriptBridgeRenderFrameDeleted,
  kOhGinJavascriptBridgePermissionDenied,
  kOhGinJavascriptBridgeErrorLast = kOhGinJavascriptBridgePermissionDenied
};

__attribute__((visibility("default"))) const char*
OhGinJavascriptBridgeErrorToString(OhGinJavascriptBridgeError error);
}  // namespace NWEB
#endif
