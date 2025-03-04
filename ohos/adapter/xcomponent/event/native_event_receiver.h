// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_ADAPTER_XCOMPONENT_EVENT_NATIVE_EVENT_RECEIVER_H_
#define OHOS_ADAPTER_XCOMPONENT_EVENT_NATIVE_EVENT_RECEIVER_H_

#include <ace/xcomponent/native_interface_xcomponent.h>

#include <memory>
#include <mutex>
#include <unordered_map>

#include "ohos/adapter/xcomponent/event/native_event_listener.h"

namespace ohos::adapter::xcomponent {

class Hash_ {
 public:
  size_t operator()(OH_NativeXComponent* val) const {
    return std::hash<unsigned long>()(reinterpret_cast<unsigned long>(val));
  }
};

class Equal_ {
 public:
  bool operator()(OH_NativeXComponent* val1, OH_NativeXComponent* val2) const {
    return val1 == val2;
  }
};

class NativeEventReceiver {
 public:
  ~NativeEventReceiver();
  NativeEventReceiver(NativeEventReceiver& other) = delete;
  void operator=(const NativeEventReceiver&) = delete;

  static NativeEventReceiver& GetInstance();

  NativeEventListener* GetCallBack(OH_NativeXComponent* component);
  void RegisterCallBack(OH_NativeXComponent* component,
                        std::shared_ptr<NativeEventListener> listener);
  void UnRegisterCallBack(OH_NativeXComponent* component);

 private:
  NativeEventReceiver();

  // XComponent callbacks
  OH_NativeXComponent_Callback surface_callback_;
  OH_NativeXComponent_MouseEvent_Callback mouse_callback_;

  std::unordered_map<OH_NativeXComponent*,
                     std::shared_ptr<NativeEventListener>,
                     Hash_,
                     Equal_>
      callback_map_;

  std::mutex mutex_;
};

}  // namespace ohos::adapter::xcomponent

#endif  // OHOS_ADAPTER_XCOMPONENT_EVENT_NATIVE_EVENT_RECEIVER_H_
