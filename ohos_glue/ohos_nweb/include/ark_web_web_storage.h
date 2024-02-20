/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef ARK_WEB_WEB_STORAGE_H_
#define ARK_WEB_WEB_STORAGE_H_
#pragma once

#include "base/include/ark_web_types.h"
#include "ohos_nweb/include/ark_web_long_value_callback.h"
#include "ohos_nweb/include/ark_web_web_storage_origin_vector_value_callback.h"

namespace OHOS::ArkWeb {

/*--ark web(source=web core)--*/
class ArkWebWebStorage : public virtual ArkWebBaseRefCounted {
public:
  /*--ark web()--*/
  virtual ArkWebWebStorageOriginVector GetOrigins() = 0;

  /*--ark web()--*/
  virtual void GetOrigins(
      ArkWebRefPtr<ArkWebWebStorageOriginVectorValueCallback> callback) = 0;

  /*--ark web()--*/
  virtual long GetOriginQuota(const ArkWebString &origin) = 0;

  /*--ark web()--*/
  virtual void
  GetOriginQuota(const ArkWebString &origin,
                 ArkWebRefPtr<ArkWebLongValueCallback> callback) = 0;

  /*--ark web()--*/
  virtual long GetOriginUsage(const ArkWebString &origin) = 0;

  /*--ark web()--*/
  virtual void
  GetOriginUsage(const ArkWebString &origin,
                 ArkWebRefPtr<ArkWebLongValueCallback> callback) = 0;

  /*--ark web()--*/
  virtual int DeleteOrigin(const ArkWebString &origin) = 0;

  /*--ark web()--*/
  virtual void DeleteAllData(bool incognito_mode) = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_WEB_STORAGE_H_
