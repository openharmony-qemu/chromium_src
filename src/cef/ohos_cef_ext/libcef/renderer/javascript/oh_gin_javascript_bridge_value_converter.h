// Copyright (c) 2022 Huawei Device Co., Ltd.
// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OH_GIN_JAVASCRIPT_BRIDGE_VALUE_CONVERTER_H
#define OH_GIN_JAVASCRIPT_BRIDGE_VALUE_CONVERTER_H

#include <memory>

#include "cef/ohos_cef_ext/libcef/renderer/javascript/oh_gin_javascript_bridge_dispatcher.h"
#include "content/public/renderer/v8_value_converter.h"

namespace NWEB {
class OhGinJavascriptBridgeValueConverter
    : public content::V8ValueConverter::Strategy {
 public:
  OhGinJavascriptBridgeValueConverter(
      const base::WeakPtr<OhGinJavascriptBridgeDispatcher>& dispatcher);

  OhGinJavascriptBridgeValueConverter();

  OhGinJavascriptBridgeValueConverter(
      const OhGinJavascriptBridgeValueConverter&) = delete;
  OhGinJavascriptBridgeValueConverter& operator=(
      const OhGinJavascriptBridgeValueConverter&) = delete;

  ~OhGinJavascriptBridgeValueConverter() override;

  v8::Local<v8::Value> ToV8Value(const base::Value* value,
                                 v8::Local<v8::Context> context) const;
  std::unique_ptr<base::Value> FromV8Value(
      v8::Local<v8::Value> value,
      v8::Local<v8::Context> context) const;

  // content::V8ValueConverter::Strategy
  bool FromV8Object(v8::Local<v8::Object> value,
                    std::unique_ptr<base::Value>* out,
                    v8::Isolate* isolate) override;
  bool FromV8Object(v8::Local<v8::Object> value,
                    std::unique_ptr<base::Value>* out,
                    v8::Isolate* isolate,
                    bool is_function,
                    bool is_promise) override;
  bool FromV8ArrayBuffer(v8::Local<v8::Object> value,
                         std::unique_ptr<base::Value>* out,
                         v8::Isolate* isolate) override;
  bool FromV8Number(v8::Local<v8::Number> value,
                    std::unique_ptr<base::Value>* out) override;
  bool FromV8Undefined(std::unique_ptr<base::Value>* out) override;

 private:
  std::unique_ptr<content::V8ValueConverter> converter_;
  base::WeakPtr<OhGinJavascriptBridgeDispatcher> dispatcher_;
};
}  // namespace NWEB
#endif
