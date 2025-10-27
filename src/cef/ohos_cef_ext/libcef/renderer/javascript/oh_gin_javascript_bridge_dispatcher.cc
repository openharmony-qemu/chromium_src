// Copyright (c) 2022 Huawei Device Co., Ltd.
// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "oh_gin_javascript_bridge_dispatcher.h"

#include "base/auto_reset.h"
#include "cef/ohos_cef_ext/libcef/common/javascript/oh_gin_javascript_bridge_messages.h"
#include "cef/ohos_cef_ext/libcef/renderer/javascript/oh_gin_javascript_bridge_object.h"
#include "content/public/renderer/render_frame.h"
#include "ipc/ipc_platform_file_attachment_posix.h"
#include "libcef/common/javascript/oh_gin_javascript_bridge_value.h"
#include "libcef/renderer/javascript/oh_gin_javascript_bridge_value_converter.h"
#include "third_party/blink/public/web/blink.h"
#include "third_party/blink/public/web/web_local_frame.h"
#include "third_party/blink/public/web/web_view.h"
#include "v8/include/v8.h"

namespace NWEB {
OhGinJavascriptBridgeDispatcher::OhGinJavascriptBridgeDispatcher(
    content::RenderFrame* render_frame)
    : content::RenderFrameObserver(render_frame),
      inside_did_clear_window_object_(false) {
  converter_ =
      std::make_unique<OhGinJavascriptBridgeValueConverter>(AsWeakPtr());
}

OhGinJavascriptBridgeDispatcher::~OhGinJavascriptBridgeDispatcher() {
  for (auto item : h5_object_map_) {
    item.second->Reset();
    delete (item.second);
  }
}

#if BUILDFLAG(CONTENT_ENABLE_LEGACY_IPC)
bool OhGinJavascriptBridgeDispatcher::OnMessageReceived(
    const IPC::Message& msg) {
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(OhGinJavascriptBridgeDispatcher, msg)
    IPC_MESSAGE_HANDLER(OhGinJavascriptBridgeMsg_AddNamedObject,
                        OnAddNamedObject)
    IPC_MESSAGE_HANDLER(OhGinJavascriptBridgeMsg_RemoveNamedObject,
                        OnRemoveNamedObject)
    IPC_MESSAGE_HANDLER(OhGinJavascriptBridgeMsg_DoCallH5Function,
                        OnDoCallH5Function)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()
  return handled;
}
#endif  // CONTENT_ENABLE_LEGACY_IPC

void OhGinJavascriptBridgeDispatcher::DidClearWindowObject() {
  if (inside_did_clear_window_object_) {
    return;
  }
  base::AutoReset<bool> flag_entry(&inside_did_clear_window_object_, true);
  for (NamedObjectMap::const_iterator iter = named_objects_.begin();
       iter != named_objects_.end(); ++iter) {
    if (objects_.Lookup(iter->second)) {
      objects_.Remove(iter->second);
    }

    OhGinJavascriptBridgeObject* object =
        OhGinJavascriptBridgeObject::InjectNamed(render_frame()->GetWebFrame(),
                                                 AsWeakPtr(), iter->first,
                                                 iter->second);
    if (object) {
      objects_.AddWithID(object, iter->second);
    } else {
      LOG(DEBUG) << "OhGinJavascriptBridgeDispatcher::DidClearWindowObject "
                    "ObjectWrapperDeleted called";
      // Inform the host about wrapper creation failure.
#if BUILDFLAG(CONTENT_ENABLE_LEGACY_IPC)
      render_frame()->Send(
          new OhGinJavascriptBridgeHostMsg_ObjectWrapperDeleted(routing_id(),
                                                                iter->second));
#endif  // CONTENT_ENABLE_LEGACY_IPC
    }
  }
}

void OhGinJavascriptBridgeDispatcher::OnAddNamedObject(
    const std::string& name,
    ObjectID object_id,
    const base::Value::List& async_method_list,
    bool need_update) {
  int size = async_method_list.size();
  if (need_update && size > 0) {
    // update async_methods_map_ to new methods
    LOG(DEBUG) << "OhGinJavascriptBridgeDispatcher::OnAddNamedObject "
                  "register async methods, object_id = "
               << object_id;
    std::unordered_set<std::string> async_method_set;
    for (int i = 0; i < size; ++i) {
      const base::Value& method_value = async_method_list[i];
      if (method_value.is_string()) {
        async_method_set.emplace(*method_value.GetIfString());
      } else {
        LOG(ERROR) << "OhGinJavascriptBridgeDispatcher::OnAddNamedObject "
                      "method name is not a string";
      }
    }
    async_methods_map_[object_id] = async_method_set;
  } else if (need_update) {
    // remove this obj in async_methods_map_ methods
    auto it = async_methods_map_.find(object_id);
    if (it != async_methods_map_.end()) {
      LOG(DEBUG) << "OhGinJavascriptBridgeDispatcher::OnAddNamedObject "
                    "clear obj's async methods, object_id = "
                 << object_id;
      async_methods_map_.erase(it);
    }
  }
  named_objects_[name] = object_id;
}

void OhGinJavascriptBridgeDispatcher::OnRemoveNamedObject(
    const std::string& name) {
  // Removal becomes in effect on next reload. We simply removing the entry
  // from the map here.
  NamedObjectMap::iterator iter = named_objects_.find(name);
  if (iter == named_objects_.end()) {
    return;
  }
  auto object_id = iter->second;
  auto it = async_methods_map_.find(object_id);
  if (it != async_methods_map_.end()) {
    LOG(DEBUG) << "OhGinJavascriptBridgeDispatcher::OnAddNamedObject "
                  "clear obj's async methods, object_id = "
               << object_id;
    async_methods_map_.erase(it);
  }
  named_objects_.erase(iter);
}

void OhGinJavascriptBridgeDispatcher::GetJavascriptMethods(
    ObjectID object_id,
    std::set<std::string>* methods) {
#if BUILDFLAG(CONTENT_ENABLE_LEGACY_IPC)
  render_frame()->Send(new OhGinJavascriptBridgeHostMsg_GetMethods(
      routing_id(), object_id, methods));
#endif  // CONTENT_ENABLE_LEGACY_IPC
}

bool OhGinJavascriptBridgeDispatcher::HasJavascriptMethod(
    ObjectID object_id,
    const std::string& method_name) {
  bool result = false;
#if BUILDFLAG(CONTENT_ENABLE_LEGACY_IPC)
  render_frame()->Send(new OhGinJavascriptBridgeHostMsg_HasMethod(
      routing_id(), object_id, method_name, &result));
#endif  // CONTENT_ENABLE_LEGACY_IPC
  return result;
}

std::unique_ptr<base::Value>
OhGinJavascriptBridgeDispatcher::InvokeJavascriptMethod(
    ObjectID object_id,
    const std::string& method_name,
    const base::Value::List& arguments,
    OhGinJavascriptBridgeError* error) {
  std::string url;
  blink::WebLocalFrame* frame = render_frame()->GetWebFrame();
  url = frame->GetDocument().Url().GetString().Utf8();
  base::Value::List result_wrapper;
#if BUILDFLAG(CONTENT_ENABLE_LEGACY_IPC)
  render_frame()->Send(new OhGinJavascriptBridgeHostMsg_InvokeMethod(
      routing_id(), object_id, url, method_name, arguments, &result_wrapper,
      error));
#endif  // CONTENT_ENABLE_LEGACY_IPC
  if (result_wrapper.empty()) {
    return nullptr;
  }
  return base::Value::ToUniquePtrValue(result_wrapper[0].Clone());
}

std::unique_ptr<base::Value>
OhGinJavascriptBridgeDispatcher::InvokeJavascriptMethodAsync(
    ObjectID object_id,
    const std::string& method_name,
    const base::Value::List& arguments) {
  std::string url;
  blink::WebLocalFrame* frame = render_frame()->GetWebFrame();
  url = frame->GetDocument().Url().GetString().Utf8();
#if BUILDFLAG(CONTENT_ENABLE_LEGACY_IPC)
  render_frame()->Send(new OhGinJavascriptBridgeHostMsg_InvokeMethod_Async(
      routing_id(), object_id, url, method_name, arguments));
#endif  // CONTENT_ENABLE_LEGACY_IPC
  return base::Value::ToUniquePtrValue(base::Value(0).Clone());
}

std::unique_ptr<base::Value>
OhGinJavascriptBridgeDispatcher::InvokeJavascriptMethodFlowbuf(
    ObjectID object_id,
    const std::string& method_name,
    const base::Value::List& arguments,
    int fd,
    OhGinJavascriptBridgeError* error) {
  std::string url;
  blink::WebLocalFrame* frame = render_frame()->GetWebFrame();
  url = frame->GetDocument().Url().GetString().Utf8();
  base::Value::List result_wrapper;
#if BUILDFLAG(CONTENT_ENABLE_LEGACY_IPC)
  IPC::Message* msg = new OhGinJavascriptBridgeHostMsg_InvokeMethod(
      routing_id(), object_id, url, method_name, arguments, &result_wrapper,
      error);
  msg->WriteAttachment(new IPC::internal::PlatformFileAttachment(fd));
  render_frame()->Send(msg);
#endif  // CONTENT_ENABLE_LEGACY_IPC
  if (result_wrapper.empty()) {
    return nullptr;
  }
  return base::Value::ToUniquePtrValue(result_wrapper[0].Clone());
}

OhGinJavascriptBridgeObject* OhGinJavascriptBridgeDispatcher::GetObject(
    const ObjectID object_id) {
  OhGinJavascriptBridgeObject* result = objects_.Lookup(object_id);
  if (!result) {
    result =
        OhGinJavascriptBridgeObject::InjectAnonymous(AsWeakPtr(), object_id);
    if (result) {
      objects_.AddWithID(result, object_id);
    }
  }
  return result;
}

void OhGinJavascriptBridgeDispatcher::OnOhGinJavascriptBridgeObjectDeleted(
    const OhGinJavascriptBridgeObject* object) {
  int object_id = object->object_id();
  LOG(DEBUG) << "OhGinJavascriptBridgeDispatcher::"
                "OnOhGinJavascriptBridgeObjectDeleted called, object_id = "
             << object_id;
  // Ignore cleaning up of old object wrappers.
  if (objects_.Lookup(object_id) != object) {
    return;
  }
  LOG(DEBUG)
      << "OhGinJavascriptBridgeDispatcher::"
         "OnOhGinJavascriptBridgeObjectDeleted called, object_id = "
      << object_id
      << " ,new OhGinJavascriptBridgeHostMsg_ObjectWrapperDeleted called";
  objects_.Remove(object_id);
#if BUILDFLAG(CONTENT_ENABLE_LEGACY_IPC)
  render_frame()->Send(new OhGinJavascriptBridgeHostMsg_ObjectWrapperDeleted(
      routing_id(), object_id));
#endif  // CONTENT_ENABLE_LEGACY_IPC
}

void OhGinJavascriptBridgeDispatcher::OnDestruct() {
  delete this;
}

OhGinJavascriptBridgeDispatcher::H5ObjectID
OhGinJavascriptBridgeDispatcher::AddH5Object(v8::Local<v8::Object>& value) {
  LOG(DEBUG) << "OhGinJavascriptBridgeDispatcher::AddH5Object called";

  if (!render_frame() || !render_frame()->GetWebFrame()) {
    LOG(ERROR)
        << "OhGinJavascriptBridgeDispatcher::GetH5ObjectMethodNames frame null";
    return 0;
  }
  v8::Isolate* isolate =
      render_frame()->GetWebFrame()->GetAgentGroupScheduler()->Isolate();
  v8::HandleScope handle_scope(isolate);

  v8::Persistent<v8::Value>* persistent =
      new v8::Persistent<v8::Value>(isolate, value);

  H5ObjectID object_id = 0;
  {
    base::AutoLock locker(h5_objects_lock_);
    object_id = next_h5_object_id_++;
    h5_object_map_[object_id] = persistent;
  }

  return object_id;
}

std::vector<std::string>
OhGinJavascriptBridgeDispatcher::GetH5ObjectMethodNames(
    v8::Local<v8::Object> object,
    int h5_object_id,
    bool is_promise) {
  if (!render_frame() || !render_frame()->GetWebFrame()) {
    LOG(ERROR)
        << "OhGinJavascriptBridgeDispatcher::GetH5ObjectMethodNames frame null";
    return std::vector<std::string>();
  }
  v8::Isolate* isolate =
      render_frame()->GetWebFrame()->GetAgentGroupScheduler()->Isolate();
  v8::HandleScope handle_scope(isolate);

  v8::Local<v8::Context> context =
      render_frame()->GetWebFrame()->MainWorldScriptContext();

  if (context.IsEmpty()) {
    LOG(ERROR) << "OhGinJavascriptBridgeDispatcher::GetH5ObjectMethodNames "
                  "context empty";
    return std::vector<std::string>();
  }

  v8::Context::Scope context_scope(context);

  if (is_promise) {
    LOG(DEBUG) << "OhGinJavascriptBridgeDispatcher::GetH5ObjectMethodNames "
                  "promise case";
    H5ObjectMethodsMap_[h5_object_id] =
        std::vector<std::string>{"then", "catch", "finally"};
    return H5ObjectMethodsMap_[h5_object_id];
  }

  v8::Local<v8::String> annotate_string =
      v8::String::NewFromUtf8(isolate, "methodNameListForJsProxy")
          .ToLocalChecked();
  v8::Local<v8::Value> value;

  v8::TryCatch try_catch(isolate);
  v8::MaybeLocal<v8::Value> maybe_value = object->Get(context, annotate_string);
  if (try_catch.HasCaught() || !maybe_value.ToLocal(&value)) {
    LOG(ERROR) << "OhGinJavascriptBridgeDispatcher::GetH5ObjectMethodNames "
                  "Getter property fail";
    return std::vector<std::string>();
  } else if (value->IsArray()) {
    v8::Local<v8::Array> keys = value.As<v8::Array>();
    v8::Local<v8::Value> key;
    for (uint32_t i = 0; i < keys->Length(); ++i) {
      if (!keys->Get(context, i).ToLocal(&key)) {
        LOG(DEBUG) << "OhGinJavascriptBridgeDispatcher::GetH5ObjectMethodNames "
                      "key error";
        continue;
      }

      int len = key.As<v8::String>()->Utf8Length(isolate);
      if (len == 0) {
        LOG(DEBUG) << "OhGinJavascriptBridgeDispatcher::GetH5ObjectMethodNames "
                      "key len error";
        continue;
      }

      char* buf = new char[len];
      key.As<v8::String>()->WriteUtf8(isolate, buf, len, nullptr,
                                      v8::String::REPLACE_INVALID_UTF8);
      std::string method_name(buf, len);
      LOG(DEBUG) << "OhGinJavascriptBridgeDispatcher::GetH5ObjectMethodNames "
                    "method_name = "
                 << method_name;
      H5ObjectMethodsMap_[h5_object_id].push_back(method_name);
      delete[] buf;
    }
    return H5ObjectMethodsMap_[h5_object_id];
  }

  return std::vector<std::string>();
}

bool OhGinJavascriptBridgeDispatcher::HasH5ObjectMethod(
    v8::Local<v8::Object> object) {
  if (!render_frame() || !render_frame()->GetWebFrame()) {
    LOG(ERROR)
        << "OhGinJavascriptBridgeDispatcher::HasH5ObjectMethod frame null";
    return false;
  }
  v8::Isolate* isolate =
      render_frame()->GetWebFrame()->GetAgentGroupScheduler()->Isolate();
  v8::HandleScope handle_scope(isolate);

  v8::Local<v8::Context> context =
      render_frame()->GetWebFrame()->MainWorldScriptContext();
  if (context.IsEmpty()) {
    LOG(ERROR)
        << "OhGinJavascriptBridgeDispatcher::HasH5ObjectMethod context empty";
    return false;
  }
  v8::Context::Scope context_scope(context);

  v8::Local<v8::Array> keys;
  v8::Local<v8::Value> key, value;
  LOG(DEBUG) << "OhGinJavascriptBridgeDispatcher::HasH5ObjectMethod called";
  if (!object->GetOwnPropertyNames(context).ToLocal(&keys)) {
    LOG(ERROR) << "OhGinJavascriptBridgeDispatcher::HasH5ObjectMethod "
                  "GetOwnPropertyNames fail";
    return false;
  }

  for (uint32_t i = 0; i < keys->Length(); ++i) {
    if (!keys->Get(context, i).ToLocal(&key)) {
      LOG(DEBUG) << "OhGinJavascriptBridgeDispatcher::GetH5ObjectMethodNames "
                    "key error";
      continue;
    }

    if (!key->IsString() && !key->IsNumber()) {
      continue;
    }

    v8::String::Utf8Value name_utf8(isolate, key);
    if (std::string(*name_utf8) != "methodNameListForJsProxy") {
      continue;
    }

    v8::TryCatch try_catch(isolate);
    v8::MaybeLocal<v8::Value> maybe_value = object->Get(context, key);
    if (try_catch.HasCaught() || !maybe_value.ToLocal(&value)) {
      LOG(WARNING) << "OhGinJavascriptBridgeDispatcher::HasH5ObjectMethod "
                      "Getter for property "
                   << *name_utf8 << " threw an exception.";
      continue;
    }

    if (value->IsArray()) {
      LOG(DEBUG)
          << "OhGinJavascriptBridgeDispatcher::HasH5ObjectMethod return true";
      return true;
    }
  }
  LOG(DEBUG)
      << "OhGinJavascriptBridgeDispatcher::HasH5ObjectMethod return false";
  return false;
}

bool OhGinJavascriptBridgeDispatcher::HasH5ObjectMethod(
    int h5_object_id,
    const std::string& method_name) {
  if (H5ObjectMethodsMap_.find(h5_object_id) == H5ObjectMethodsMap_.end()) {
    return false;
  }

  for (std::vector<std::string>::iterator iter =
           H5ObjectMethodsMap_[h5_object_id].begin();
       iter != H5ObjectMethodsMap_[h5_object_id].end(); ++iter) {
    if (*iter == method_name) {
      LOG(DEBUG)
          << "OhGinJavascriptBridgeDispatcher::HasH5ObjectMethod return true";
      return true;
    }
  }
  LOG(DEBUG)
      << "OhGinJavascriptBridgeDispatcher::HasH5ObjectMethod return false";
  return false;
}

void OhGinJavascriptBridgeDispatcher::OnDoCallAnonymousH5Function(
    int32_t h5_object_id,
    const base::Value::List& args) {
  std::map<H5ObjectID, v8::Persistent<v8::Value>*>::iterator iter;
  {
    base::AutoLock locker(h5_objects_lock_);
    iter = h5_object_map_.find(h5_object_id);
    if (iter == h5_object_map_.end() || !(iter->second)) {
      LOG(DEBUG) << "OhGinJavascriptBridgeDispatcher::"
                    "OnDoCallAnonymousH5Function obj id not found";
      return;
    }
  }

  if (!render_frame() || !render_frame()->GetWebFrame()) {
    LOG(ERROR) << "OhGinJavascriptBridgeDispatcher::"
                  "OnDoCallAnonymousH5Function frame null";
    return;
  }
  v8::Isolate* isolate =
      render_frame()->GetWebFrame()->GetAgentGroupScheduler()->Isolate();
  v8::HandleScope handle_scope(isolate);

  v8::Local<v8::Context> context =
      render_frame()->GetWebFrame()->MainWorldScriptContext();
  if (context.IsEmpty()) {
    LOG(ERROR) << "OhGinJavascriptBridgeDispatcher::"
                  "OnDoCallAnonymousH5Function context empty";
    return;
  }
  v8::Context::Scope context_scope(context);

  v8::Local<v8::Object> object = v8::Local<v8::Object>::New(
      isolate, *reinterpret_cast<v8::Persistent<v8::Object>*>(iter->second));

  v8::Local<v8::Value>* v8_args = nullptr;
  int size = args.size();
  if (size > 0) {
    LOG(DEBUG) << "OhGinJavascriptBridgeDispatcher::"
                  "OnDoCallAnonymousH5Function args size = "
               << size;
    v8_args = new v8::Local<v8::Value>[size];
  }

  for (int i = 0; i < size; ++i) {
    const base::Value& base_arg = args[i];
    if (!base_arg.is_blob()) {
      LOG(DEBUG) << "OhGinJavascriptBridgeDispatcher::"
                    "OnDoCallAnonymousH5Function not blob";
      v8_args[i] = converter_->ToV8Value(&base_arg, context);
      continue;
    }

    std::unique_ptr<const OhGinJavascriptBridgeValue> gin_value =
        OhGinJavascriptBridgeValue::FromValue(&base_arg);
    if (gin_value->IsType(OhGinJavascriptBridgeValue::TYPE_OBJECT_ID)) {
      OhGinJavascriptBridgeObject* object_result = NULL;
      OhGinJavascriptBridgeDispatcher::ObjectID object_id;
      if (gin_value->GetAsObjectID(&object_id)) {
        LOG(DEBUG) << "OhGinJavascriptBridgeDispatcher::"
                      "OnDoCallAnonymousH5Function blob";
        object_result = GetObject(object_id);
      }
      if (object_result) {
        LOG(DEBUG) << "OhGinJavascriptBridgeDispatcher::"
                      "OnDoCallAnonymousH5Function object_result ok";
        gin::Handle<OhGinJavascriptBridgeObject> controller =
            gin::CreateHandle(isolate, object_result);
        if (controller.IsEmpty()) {
          LOG(DEBUG) << "OhGinJavascriptBridgeDispatcher::"
                        "OnDoCallAnonymousH5Function controller empty";
          v8_args[i] = v8::Undefined(isolate);
          continue;
        }
        v8_args[i] = controller.ToV8();
        continue;
      }
    } else if (gin_value->IsType(OhGinJavascriptBridgeValue::TYPE_NONFINITE)) {
      float float_value;
      gin_value->GetAsNonFinite(&float_value);
      v8_args[i] = v8::Number::New(isolate, float_value);
      continue;
    }
    v8_args[i] = v8::Undefined(isolate);
  }

  v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(object);
  v8::MaybeLocal<v8::Value> function_call_result =
      func->Call(context, context->Global(), size, v8_args);
  LOG(DEBUG) << "OhGinJavascriptBridgeDispatcher::OnDoCallAnonymousH5Function "
                "func call end";
  if (!function_call_result.IsEmpty()) {
    function_call_result.ToLocalChecked();
  }

  if (v8_args != nullptr) {
    delete[] v8_args;
  }
}

void OhGinJavascriptBridgeDispatcher::convertToV8Value(
    const base::Value::List& base_value,
    v8::Local<v8::Value>*& v8_value) {
  if (!v8_value || !render_frame() || !render_frame()->GetWebFrame()) {
    return;
  }
  v8::Isolate* isolate =
      render_frame()->GetWebFrame()->GetAgentGroupScheduler()->Isolate();
  v8::HandleScope handle_scope(isolate);

  v8::Local<v8::Context> context =
      render_frame()->GetWebFrame()->MainWorldScriptContext();
  if (context.IsEmpty()) {
    return;
  }
  v8::Context::Scope context_scope(context);

  int size = base_value.size();
  for (int i = 0; i < size; ++i) {
    const base::Value& base_arg = base_value[i];
    if (!base_arg.is_blob()) {
      v8_value[i] = converter_->ToV8Value(&base_arg, context);
      continue;
    }

    std::unique_ptr<const OhGinJavascriptBridgeValue> gin_value =
        OhGinJavascriptBridgeValue::FromValue(&base_arg);
    if (gin_value->IsType(OhGinJavascriptBridgeValue::TYPE_OBJECT_ID)) {
      OhGinJavascriptBridgeObject* object_result = NULL;
      OhGinJavascriptBridgeDispatcher::ObjectID object_id;
      if (gin_value->GetAsObjectID(&object_id)) {
        object_result = GetObject(object_id);
      }
      if (object_result) {
        gin::Handle<OhGinJavascriptBridgeObject> controller =
            gin::CreateHandle(isolate, object_result);
        if (controller.IsEmpty()) {
          v8_value[i] = v8::Undefined(isolate);
          continue;
        }
        v8_value[i] = controller.ToV8();
        continue;
      }
    } else if (gin_value->IsType(OhGinJavascriptBridgeValue::TYPE_NONFINITE)) {
      float float_value;
      gin_value->GetAsNonFinite(&float_value);
      v8_value[i] = v8::Number::New(isolate, float_value);
      continue;
    }
    v8_value[i] = v8::Undefined(isolate);
  }
}

void OhGinJavascriptBridgeDispatcher::OnDoCallH5Function(
    int32_t frame_routing_id,
    int32_t h5_object_id,
    const std::string& method_name,
    const base::Value::List& args) {
#if BUILDFLAG(CONTENT_ENABLE_LEGACY_IPC)
  if (frame_routing_id != routing_id()) {
    return;
  }
#endif  // CONTENT_ENABLE_LEGACY_IPC

  std::map<H5ObjectID, v8::Persistent<v8::Value>*>::iterator iter;
  {
    base::AutoLock locker(h5_objects_lock_);
    iter = h5_object_map_.find(h5_object_id);
    if (iter == h5_object_map_.end() || !(iter->second)) {
      LOG(DEBUG) << "OhGinJavascriptBridgeDispatcher::OnDoCallH5Function obj "
                    "id not found";
      return;
    }
  }

  if (method_name
          .empty()) {  // IF h5_method_name empty, call anonymous function
    OnDoCallAnonymousH5Function(h5_object_id, args);
  }

  v8::Isolate* isolate =
      render_frame()->GetWebFrame()->GetAgentGroupScheduler()->Isolate();
  v8::HandleScope handle_scope(isolate);

  if (!render_frame() || !render_frame()->GetWebFrame()) {
    LOG(ERROR)
        << "OhGinJavascriptBridgeDispatcher::OnDoCallH5Function frame null";
    return;
  }

  v8::Local<v8::Context> context =
      render_frame()->GetWebFrame()->MainWorldScriptContext();

  if (context.IsEmpty()) {
    LOG(ERROR)
        << "OhGinJavascriptBridgeDispatcher::OnDoCallH5Function context empty";
    return;
  }
  v8::Context::Scope context_scope(context);

  if (!HasH5ObjectMethod(h5_object_id, method_name)) {
    return;
  }

  v8::Local<v8::Object> object = v8::Local<v8::Object>::New(
      isolate, *reinterpret_cast<v8::Persistent<v8::Object>*>(iter->second));
  v8::Local<v8::Value> key =
      v8::String::NewFromUtf8(isolate, method_name.c_str()).ToLocalChecked();

  v8::Local<v8::Value>* v8_args = nullptr;
  int size = args.size();
  if (size > 0) {
    LOG(DEBUG)
        << "OhGinJavascriptBridgeDispatcher::OnDoCallH5Function args size = "
        << size;
    v8_args = new v8::Local<v8::Value>[size];
  }

  for (int i = 0; i < size; ++i) {
    const base::Value& base_arg = args[i];
    if (!base_arg.is_blob()) {
      LOG(DEBUG)
          << "OhGinJavascriptBridgeDispatcher::OnDoCallH5Function not blob";
      v8_args[i] = converter_->ToV8Value(&base_arg, context);
      continue;
    }

    std::unique_ptr<const OhGinJavascriptBridgeValue> gin_value =
        OhGinJavascriptBridgeValue::FromValue(&base_arg);
    if (gin_value->IsType(OhGinJavascriptBridgeValue::TYPE_OBJECT_ID)) {
      OhGinJavascriptBridgeObject* object_result = NULL;
      OhGinJavascriptBridgeDispatcher::ObjectID object_id;
      if (gin_value->GetAsObjectID(&object_id)) {
        LOG(DEBUG)
            << "OhGinJavascriptBridgeDispatcher::OnDoCallH5Function blob";
        object_result = GetObject(object_id);
      }
      if (object_result) {
        LOG(DEBUG) << "OhGinJavascriptBridgeDispatcher::OnDoCallH5Function "
                      "object_result ok";
        gin::Handle<OhGinJavascriptBridgeObject> controller =
            gin::CreateHandle(isolate, object_result);
        if (controller.IsEmpty()) {
          LOG(DEBUG) << "OhGinJavascriptBridgeDispatcher::OnDoCallH5Function "
                        "controller empty";
          v8_args[i] = v8::Undefined(isolate);
          continue;
        }
        v8_args[i] = controller.ToV8();
        continue;
      }
    } else if (gin_value->IsType(OhGinJavascriptBridgeValue::TYPE_NONFINITE)) {
      float float_value;
      gin_value->GetAsNonFinite(&float_value);
      v8_args[i] = v8::Number::New(isolate, float_value);
      continue;
    }
    v8_args[i] = v8::Undefined(isolate);
  }

  v8::TryCatch try_catch(isolate);
  v8::MaybeLocal<v8::Value> result_value = object->Get(context, key);

  if (!try_catch.HasCaught() && !result_value.IsEmpty()) {
    v8::Local<v8::Value> value = result_value.ToLocalChecked();
    v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(value);
    v8::MaybeLocal<v8::Value> function_call_result =
        func->Call(context, object, size, v8_args);
    LOG(DEBUG)
        << "OhGinJavascriptBridgeDispatcher::OnDoCallH5Function func call end";
    if (!function_call_result.IsEmpty()) {
      function_call_result.ToLocalChecked();
    }
  }

  if (v8_args != nullptr) {
    delete[] v8_args;
  }
}

bool OhGinJavascriptBridgeDispatcher::IsAsyncMethod(
    ObjectID object_id,
    const std::string& method_name) {
  auto it = async_methods_map_.find(object_id);
  if (it != async_methods_map_.end()) {
    auto async_methods = it->second;
    if (async_methods.find(method_name) != async_methods.end()) {
      return true;
    }
  }
  return false;
}

void OhGinJavascriptBridgeDispatcher::AddNamedObject(
    const std::string& name,
    int32_t object_id,
    base::Value::List& async_method_list,
    bool need_update) {
  OnAddNamedObject(name, object_id, async_method_list, need_update);
}

}  // namespace NWEB
