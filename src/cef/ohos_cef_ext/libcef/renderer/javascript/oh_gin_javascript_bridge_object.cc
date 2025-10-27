// Copyright (c) 2022 Huawei Device Co., Ltd.
// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libcef/renderer/javascript/oh_gin_javascript_bridge_object.h"

#include "base/functional/bind.h"
#include "cef/libcef/renderer/v8_impl.h"
#include "content/public/renderer/render_frame.h"
#include "content/public/renderer/render_thread.h"
#include "gin/function_template.h"
#include "libcef/common/javascript/oh_gin_javascript_bridge_messages.h"
#include "libcef/renderer/javascript/oh_gin_javascript_bridge_dispatcher.h"
#include "libcef/renderer/javascript/oh_gin_javascript_function_invocation_helper.h"
#include "third_party/blink/public/platform/scheduler/web_agent_group_scheduler.h"
#include "third_party/blink/public/web/blink.h"
#include "third_party/blink/public/web/web_local_frame.h"
#include "v8/include/v8-function.h"

namespace NWEB {
gin::WrapperInfo OhGinJavascriptBridgeObject::kWrapperInfo = {
    gin::kEmbedderNativeGin};

// static
OhGinJavascriptBridgeObject* OhGinJavascriptBridgeObject::InjectNamed(
    blink::WebLocalFrame* frame,
    const base::WeakPtr<OhGinJavascriptBridgeDispatcher>& dispatcher,
    const std::string& object_name,
    OhGinJavascriptBridgeDispatcher::ObjectID object_id) {
  v8::Isolate* isolate = frame->GetAgentGroupScheduler()->Isolate();
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Context> context = frame->MainWorldScriptContext();
  if (context.IsEmpty()) {
    return nullptr;
  }

  OhGinJavascriptBridgeObject* object =
      new OhGinJavascriptBridgeObject(isolate, dispatcher, object_id);

  v8::Context::Scope context_scope(context);
  v8::Local<v8::Object> global = context->Global();
  gin::Handle<OhGinJavascriptBridgeObject> controller =
      gin::CreateHandle(isolate, object);
  // WrappableBase instance deletes itself in case of a wrapper
  // creation failure, thus there is no need to delete |object|.
  if (controller.IsEmpty()) {
    return nullptr;
  }

  global->Set(context, gin::StringToV8(isolate, object_name), controller.ToV8())
      .Check();

  return object;
}

// static
OhGinJavascriptBridgeObject* OhGinJavascriptBridgeObject::InjectAnonymous(
    const base::WeakPtr<OhGinJavascriptBridgeDispatcher>& dispatcher,
    OhGinJavascriptBridgeDispatcher::ObjectID object_id) {
  return new OhGinJavascriptBridgeObject(dispatcher->render_frame()
                                             ->GetWebFrame()
                                             ->GetAgentGroupScheduler()
                                             ->Isolate(),
                                         dispatcher, object_id);
}

OhGinJavascriptBridgeObject::OhGinJavascriptBridgeObject(
    v8::Isolate* isolate,
    const base::WeakPtr<OhGinJavascriptBridgeDispatcher>& dispatcher,
    OhGinJavascriptBridgeDispatcher::ObjectID object_id)
    : gin::NamedPropertyInterceptor(isolate, this),
      dispatcher_(dispatcher),
      object_id_(object_id),
#if BUILDFLAG(CONTENT_ENABLE_LEGACY_IPC)
      frame_routing_id_(dispatcher_->routing_id()),
#endif  // CONTENT_ENABLE_LEGACY_IPC
      template_cache_(isolate) {
}

OhGinJavascriptBridgeObject::~OhGinJavascriptBridgeObject() {
  if (dispatcher_) {
    LOG(DEBUG)
        << "OhGinJavascriptBridgeObject::~OhGinJavascriptBridgeObject called";
    dispatcher_->OnOhGinJavascriptBridgeObjectDeleted(this);
  } else {
    LOG(DEBUG)
        << "OhGinJavascriptBridgeObject::~OhGinJavascriptBridgeObject called";
    // A wrapper can outlive a render frame, and thus the dispatcher.
    // Note that we intercept GinJavaBridgeHostMsg messages in a browser filter
    // thus it's OK to send the message with a routing id of a ceased frame.
#if BUILDFLAG(CONTENT_ENABLE_LEGACY_IPC)
    content::RenderThread::Get()->Send(
        new OhGinJavascriptBridgeHostMsg_ObjectWrapperDeleted(frame_routing_id_,
                                                              object_id_));
#endif  // CONTENT_ENABLE_LEGACY_IPC
  }
}

gin::ObjectTemplateBuilder
OhGinJavascriptBridgeObject::GetObjectTemplateBuilder(v8::Isolate* isolate) {
  return gin::Wrappable<OhGinJavascriptBridgeObject>::GetObjectTemplateBuilder(
             isolate)
      .AddNamedPropertyInterceptor();
}

v8::Local<v8::Value> OhGinJavascriptBridgeObject::GetNamedProperty(
    v8::Isolate* isolate,
    const std::string& property) {
  std::map<std::string, bool>::iterator method_pos =
      known_methods_.find(property);
  if (method_pos == known_methods_.end()) {
    if (!dispatcher_) {
      return v8::Local<v8::Value>();
    }
    known_methods_[property] =
        dispatcher_->HasJavascriptMethod(object_id_, property);
  }
  if (known_methods_[property]) {
    return GetFunctionTemplate(isolate, property)
        ->GetFunction(isolate->GetCurrentContext())
        .FromMaybe(v8::Local<v8::Value>());
  } else {
    return v8::Local<v8::Value>();
  }
}

std::vector<std::string> OhGinJavascriptBridgeObject::EnumerateNamedProperties(
    v8::Isolate* isolate) {
  std::set<std::string> method_names;
  if (dispatcher_) {
    dispatcher_->GetJavascriptMethods(object_id_, &method_names);
  }
  return std::vector<std::string>(method_names.begin(), method_names.end());
}

v8::Local<v8::FunctionTemplate>
OhGinJavascriptBridgeObject::GetFunctionTemplate(v8::Isolate* isolate,
                                                 const std::string& name) {
  v8::Local<v8::FunctionTemplate> function_template = template_cache_.Get(name);
  if (!function_template.IsEmpty()) {
    return function_template;
  }
  function_template = gin::CreateFunctionTemplate(
      isolate, base::BindRepeating(
                   &OhGinJavascriptFunctionInvocationHelper::Invoke,
                   base::Owned(new OhGinJavascriptFunctionInvocationHelper(
                       name, dispatcher_))));
  template_cache_.Set(name, function_template);
  return function_template;
}
}  // namespace NWEB
