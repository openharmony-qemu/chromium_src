// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_NWEB_SRC_CAPI_COMMON_COMMON_H_
#define OHOS_NWEB_SRC_CAPI_COMMON_COMMON_H_

template <class T, typename MethodPointer>
bool CheckValid(T* obj, MethodPointer* pointer) {
  if (!obj || !pointer) {
    return false;
  }
  typename T::Base* base = static_cast<typename T::Base*>(obj);
  uintptr_t start_pos = reinterpret_cast<intptr_t>(base);
  uintptr_t method_pos = reinterpret_cast<intptr_t>(pointer);
  if (start_pos + obj->struct_size < method_pos + sizeof(MethodPointer)) {
    return false;
  }
  if (!(*pointer)) {
    return false;
  }
  return true;
}

#endif // OHOS_NWEB_SRC_CAPI_COMMON_COMMON_H_