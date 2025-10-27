// Copyright (c) 2022 Huawei Device Co., Ltd.
// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// IPC messages for injected Javascript objects (Gin-based implementation).
#include <stdint.h>

#include "cef/ohos_cef_ext/libcef/common/javascript/oh_gin_javascript_bridge_errors.h"
#include "content/common/content_export.h"
#include "ipc/ipc_message_macros.h"
#include "ipc/ipc_message_start.h"

#undef IPC_MESSAGE_EXPORT
#define IPC_MESSAGE_EXPORT CONTENT_EXPORT
#define IPC_MESSAGE_START OhGinJavascriptBridgeMsgStart

IPC_ENUM_TRAITS_MAX_VALUE(NWEB::OhGinJavascriptBridgeError,
                          NWEB::kOhGinJavascriptBridgeErrorLast)

// Sent from browser to renderer to add a Javascript object with the given
// name.

IPC_MESSAGE_ROUTED4(OhGinJavascriptBridgeMsg_AddNamedObject,
                    std::string /* name */,
                    int32_t /* object_id */,
                    base::Value::List /* async_method_list */,
                    bool /* update_method*/)

IPC_MESSAGE_ROUTED1(OhGinJavascriptBridgeMsg_RemoveNamedObject,
                    std::string /* name */)

IPC_MESSAGE_ROUTED4(OhGinJavascriptBridgeMsg_DoCallH5Function,
                    int32_t /* routing_id */,
                    int32_t /* h5_object_id */,
                    std::string /* method_name */,
                    base::Value::List /* args */)

IPC_SYNC_MESSAGE_ROUTED1_1(OhGinJavascriptBridgeHostMsg_GetMethods,
                           int32_t /* object_id */,
                           std::set<std::string> /* returned_method_names */)

// Sent from renderer to browser to find out, if an object has a method with
// the given name.
IPC_SYNC_MESSAGE_ROUTED2_1(OhGinJavascriptBridgeHostMsg_HasMethod,
                           int32_t /* object_id */,
                           std::string /* method_name */,
                           bool /* result */)

IPC_SYNC_MESSAGE_ROUTED4_2(OhGinJavascriptBridgeHostMsg_InvokeMethod,
                           int32_t /* object_id */,
                           std::string /* document url */,
                           std::string /* method_name */,
                           base::Value::List /* arguments */,
                           base::Value::List /* result */,
                           NWEB::OhGinJavascriptBridgeError /* error_code */)

#define IPC_ASYNC_MESSAGE_ROUTED(msg_class, in) \
  IPC_MESSAGE_DECL(msg_class, ROUTED, IPC_TUPLE in, void)

#define IPC_ASYNC_MESSAGE_ROUTED4_0(msg, a, b, c, d) \
  IPC_ASYNC_MESSAGE_ROUTED(msg, (a, b, c, d))

IPC_ASYNC_MESSAGE_ROUTED4_0(OhGinJavascriptBridgeHostMsg_InvokeMethod_Async,
                            int32_t /* object_id */,
                            std::string /* document url */,
                            std::string /* method_name */,
                            base::Value::List /* arguments */)

IPC_MESSAGE_ROUTED1(OhGinJavascriptBridgeHostMsg_ObjectWrapperDeleted,
                    int32_t /* object_id */)
