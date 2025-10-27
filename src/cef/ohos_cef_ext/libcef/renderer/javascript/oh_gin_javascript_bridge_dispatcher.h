// Copyright (c) 2022 Huawei Device Co., Ltd.
// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OH_GIN_JAVASCRIPT_BRIDGE_DISPATCHER_H
#define OH_GIN_JAVASCRIPT_BRIDGE_DISPATCHER_H

#include <map>
#include <memory>
#include <set>

#include "base/containers/id_map.h"
#include "base/memory/weak_ptr.h"
#include "base/values.h"
#include "cef/ohos_cef_ext/libcef/common/javascript/oh_gin_javascript_bridge_errors.h"
#include "content/public/renderer/render_frame_observer.h"
#include "content/public/renderer/v8_value_converter.h"

namespace NWEB {
class OhGinJavascriptBridgeObject;
class OhGinJavascriptBridgeValueConverter;

class OhGinJavascriptBridgeDispatcher : public content::RenderFrameObserver {
 public:
  OhGinJavascriptBridgeDispatcher(const OhGinJavascriptBridgeDispatcher&) =
      delete;
  OhGinJavascriptBridgeDispatcher& operator=(
      const OhGinJavascriptBridgeDispatcher&) = delete;

  using ObjectMap = base::IDMap<OhGinJavascriptBridgeObject*>;
  using ObjectID = ObjectMap::KeyType;
  typedef int32_t H5ObjectID;

  explicit OhGinJavascriptBridgeDispatcher(content::RenderFrame* render_frame);

  ~OhGinJavascriptBridgeDispatcher() override;

  // RenderFrameObserver override:
#if BUILDFLAG(CONTENT_ENABLE_LEGACY_IPC)
  bool OnMessageReceived(const IPC::Message& message) override;
#endif  // CONTENT_ENABLE_LEGACY_IPC
  void DidClearWindowObject() override;

  void GetJavascriptMethods(ObjectID object_id, std::set<std::string>* methods);
  bool HasJavascriptMethod(ObjectID object_id, const std::string& method_name);

  std::unique_ptr<base::Value> InvokeJavascriptMethod(
      ObjectID object_id,
      const std::string& method_name,
      const base::Value::List& arguments,
      OhGinJavascriptBridgeError* error);

  std::unique_ptr<base::Value> InvokeJavascriptMethodAsync(
      ObjectID object_id,
      const std::string& method_name,
      const base::Value::List& arguments);

  std::unique_ptr<base::Value> InvokeJavascriptMethodFlowbuf(
      ObjectID object_id,
      const std::string& method_name,
      const base::Value::List& arguments,
      int fd,
      OhGinJavascriptBridgeError* error);

  OhGinJavascriptBridgeObject* GetObject(const ObjectID object_id);
  void OnOhGinJavascriptBridgeObjectDeleted(
      const OhGinJavascriptBridgeObject* object);

  H5ObjectID AddH5Object(v8::Local<v8::Object>& value);

  bool HasH5ObjectMethod(v8::Local<v8::Object> object);

  std::vector<std::string> GetH5ObjectMethodNames(v8::Local<v8::Object> object,
                                                  int h5_object_id,
                                                  bool is_promise);

  bool IsAsyncMethod(ObjectID object_id, const std::string& method_name);

  base::WeakPtr<OhGinJavascriptBridgeDispatcher> AsWeakPtr() {
    return weak_ptr_factory_.GetWeakPtr();
  }

  void AddNamedObject(const std::string& name,
                      int32_t object_id,
                      base::Value::List& async_method_list,
                      bool need_update) override;

 private:
  // RenderFrameObserver implementation.
  void OnDestruct() override;
  void OnAddNamedObject(const std::string& name,
                        ObjectID object_id,
                        const base::Value::List& async_method_list,
                        bool need_update);
  void OnRemoveNamedObject(const std::string& name);

  bool HasH5ObjectMethod(int h5_object_id, const std::string& method_name);

  void convertToV8Value(const base::Value::List& base_value,
                        v8::Local<v8::Value>*& v8_value);

  void OnDoCallAnonymousH5Function(int32_t h5_object_id,
                                   const base::Value::List& args);

  void OnDoCallH5Function(int32_t frame_routing_id,
                          int32_t h5_object_id,
                          const std::string& method_name,
                          const base::Value::List& args);

  typedef std::map<std::string, ObjectID> NamedObjectMap;
  typedef std::map<ObjectID, std::unordered_set<std::string>> MethodsMap;
  MethodsMap async_methods_map_;
  NamedObjectMap named_objects_;
  ObjectMap objects_;
  bool inside_did_clear_window_object_;

  H5ObjectID next_h5_object_id_ = 1;
  std::map<H5ObjectID, v8::Persistent<v8::Value>*> h5_object_map_
      GUARDED_BY(h5_objects_lock_);
  std::map<H5ObjectID, std::vector<std::string>> H5ObjectMethodsMap_;
  std::unique_ptr<OhGinJavascriptBridgeValueConverter> converter_;
  base::Lock h5_objects_lock_;
  base::WeakPtrFactory<OhGinJavascriptBridgeDispatcher> weak_ptr_factory_{this};
};
}  // namespace NWEB
#endif
