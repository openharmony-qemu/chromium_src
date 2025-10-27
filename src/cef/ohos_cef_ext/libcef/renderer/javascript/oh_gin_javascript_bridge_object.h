// Copyright (c) 2022 Huawei Device Co., Ltd.
// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OH_GIN_JAVASCRIPT_BRIDGE_OBJECT_H
#define OH_GIN_JAVASCRIPT_BRIDGE_OBJECT_H

#include <map>

#include "base/memory/weak_ptr.h"
#include "cef/ohos_cef_ext/libcef/renderer/javascript/oh_gin_javascript_bridge_dispatcher.h"
#include "gin/handle.h"
#include "gin/interceptor.h"
#include "gin/object_template_builder.h"
#include "gin/wrappable.h"
#include "v8/include/v8-util.h"

namespace blink {
class WebLocalFrame;
}

namespace NWEB {
class OhGinJavascriptBridgeObject
    : public gin::Wrappable<OhGinJavascriptBridgeObject>,
      public gin::NamedPropertyInterceptor {
 public:
  OhGinJavascriptBridgeObject(const OhGinJavascriptBridgeObject&) = delete;
  OhGinJavascriptBridgeObject& operator=(const OhGinJavascriptBridgeObject&) =
      delete;

  static gin::WrapperInfo kWrapperInfo;

  OhGinJavascriptBridgeDispatcher::ObjectID object_id() const {
    return object_id_;
  }

  // gin::Wrappable.
  gin::ObjectTemplateBuilder GetObjectTemplateBuilder(
      v8::Isolate* isolate) override;

  // gin::NamedPropertyInterceptor
  v8::Local<v8::Value> GetNamedProperty(v8::Isolate* isolate,
                                        const std::string& property) override;
  std::vector<std::string> EnumerateNamedProperties(
      v8::Isolate* isolate) override;

  static OhGinJavascriptBridgeObject* InjectNamed(
      blink::WebLocalFrame* frame,
      const base::WeakPtr<OhGinJavascriptBridgeDispatcher>& dispatcher,
      const std::string& object_name,
      OhGinJavascriptBridgeDispatcher::ObjectID object_id);

  static OhGinJavascriptBridgeObject* InjectAnonymous(
      const base::WeakPtr<OhGinJavascriptBridgeDispatcher>& dispatcher,
      OhGinJavascriptBridgeDispatcher::ObjectID object_id);

 private:
  OhGinJavascriptBridgeObject(
      v8::Isolate* isolate,
      const base::WeakPtr<OhGinJavascriptBridgeDispatcher>& dispatcher,
      OhGinJavascriptBridgeDispatcher::ObjectID object_id);

  ~OhGinJavascriptBridgeObject() override;

  v8::Local<v8::FunctionTemplate> GetFunctionTemplate(v8::Isolate* isolate,
                                                      const std::string& name);

  base::WeakPtr<OhGinJavascriptBridgeDispatcher> dispatcher_;
  OhGinJavascriptBridgeDispatcher::ObjectID object_id_;
  int frame_routing_id_;
  std::map<std::string, bool> known_methods_;
  v8::StdGlobalValueMap<std::string, v8::FunctionTemplate> template_cache_;
};
}  // namespace NWEB
#endif
