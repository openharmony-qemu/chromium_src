/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ARK_DISPLAY_MANAGER_ADAPTER_H
#define ARK_DISPLAY_MANAGER_ADAPTER_H
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=web core)--*/
class ArkDisplayListenerAdapter : public virtual ArkWebBaseRefCounted {
public:
  /*--ark web()--*/
  virtual void OnCreate(uint64_t displayId) = 0;

  /*--ark web()--*/
  virtual void OnDestroy(uint64_t displayId) = 0;

  /*--ark web()--*/
  virtual void OnChange(uint64_t displayId) = 0;
};

/*--ark web(source=library)--*/
class ArkDisplayAdapter : public virtual ArkWebBaseRefCounted {
public:
  /*--ark web()--*/
  virtual uint64_t GetId() = 0;

  /*--ark web()--*/
  virtual int32_t GetWidth() = 0;

  /*--ark web()--*/
  virtual int32_t GetHeight() = 0;

  /*--ark web()--*/
  virtual float GetVirtualPixelRatio() = 0;

  /*--ark web()--*/
  virtual uint32_t GetRotation() = 0;

  /*--ark web()--*/
  virtual uint32_t GetOrientation() = 0;

  /*--ark web()--*/
  virtual int32_t GetDpi() = 0;

  /*--ark web()--*/
  virtual uint32_t GetDisplayOrientation() = 0;
};

/*--ark web(source=library)--*/
class ArkDisplayManagerAdapter : public virtual ArkWebBaseRefCounted {
public:
  /*--ark web()--*/
  virtual uint64_t GetDefaultDisplayId() = 0;

  /*--ark web()--*/
  virtual ArkWebRefPtr<ArkDisplayAdapter> GetDefaultDisplay() = 0;

  /*--ark web()--*/
  virtual uint32_t
  RegisterDisplayListener(ArkWebRefPtr<ArkDisplayListenerAdapter> listener) = 0;

  /*--ark web()--*/
  virtual bool UnregisterDisplayListener(uint32_t id) = 0;

  /*--ark web()--*/
  virtual bool IsDefaultPortrait() = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_DISPLAY_MANAGER_ADAPTER_H
