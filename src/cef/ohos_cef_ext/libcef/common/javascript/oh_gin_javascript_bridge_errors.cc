// Copyright (c) 2022 Huawei Device Co., Ltd.
// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cef/ohos_cef_ext/libcef/common/javascript/oh_gin_javascript_bridge_errors.h"

#include "base/notreached.h"
namespace NWEB {
const char* OhGinJavascriptBridgeErrorToString(
    OhGinJavascriptBridgeError error) {
  switch (error) {
    case kOhGinJavascriptBridgeNoError:
      return "No error";
    case kOhGinJavascriptBridgeUnknownObjectId:
      return "Unknown Ace object ID";
    case kOhGinJavascriptBridgeObjectIsGone:
      return "Ace object is gone";
    case kOhGinJavascriptBridgeMethodNotFound:
      return "Method not found";
    case kOhGinJavascriptBridgeAccessToObjectGetClassIsBlocked:
      return "Access to Ace is blocked";
    case kOhGinJavascriptBridgeJavaExceptionRaised:
      return "Ace exception was raised during method invocation";
    case kOhGinJavascriptBridgeNonAssignableTypes:
      return "The type of the object passed to the method is incompatible "
             "with the type of method's argument";
    case kOhGinJavascriptBridgeRenderFrameDeleted:
      return "RenderFrame has been deleted";
    case kOhGinJavascriptBridgePermissionDenied:
      return "Jsb Permission Denied";
  }
  NOTREACHED();
}
}  // namespace NWEB
