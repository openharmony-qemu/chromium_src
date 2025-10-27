// Copyright (c) 2022 Huawei Device Co., Ltd.
// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OH_GIN_JAVASCRIPT_FUNCTION_INVOCATION_HELPER_H
#define OH_GIN_JAVASCRIPT_FUNCTION_INVOCATION_HELPER_H

#include <memory>

#include "base/memory/weak_ptr.h"
#include "cef/ohos_cef_ext/libcef/renderer/javascript/oh_gin_javascript_bridge_dispatcher.h"
#include "gin/arguments.h"
#include "gin/handle.h"

namespace NWEB {
class OhGinJavascriptBridgeValueConverter;

class OhGinJavascriptFunctionInvocationHelper {
 public:
  OhGinJavascriptFunctionInvocationHelper(
      const std::string& method_name,
      const base::WeakPtr<OhGinJavascriptBridgeDispatcher>& dispatcher);

  OhGinJavascriptFunctionInvocationHelper(
      const OhGinJavascriptFunctionInvocationHelper&) = delete;
  OhGinJavascriptFunctionInvocationHelper& operator=(
      const OhGinJavascriptFunctionInvocationHelper&) = delete;

  ~OhGinJavascriptFunctionInvocationHelper();

  v8::Local<v8::Value> Invoke(gin::Arguments* args);

 private:
  std::unique_ptr<base::Value> InvokeJavascriptMethod(
      base::Value::List& arguments,
      OhGinJavascriptBridgeError& error,
      gin::Arguments* args,
      OhGinJavascriptBridgeObject* object);

  std::unique_ptr<base::Value> InvokeJavascriptMethodFlowbuf(
      base::Value::List& arguments,
      OhGinJavascriptBridgeError& error,
      gin::Arguments* args,
      OhGinJavascriptBridgeObject* object);

  bool StoreString(int index, void* mem, const char* str);

  static int32_t maxFdNum_;
  static std::atomic<int32_t> usedFd_;
  std::string method_name_;
  base::WeakPtr<OhGinJavascriptBridgeDispatcher> dispatcher_;
  std::unique_ptr<OhGinJavascriptBridgeValueConverter> converter_;
};
}  // namespace NWEB
#endif
