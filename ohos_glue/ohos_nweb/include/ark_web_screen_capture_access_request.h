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

#ifndef ARK_WEB_SCREEN_CAPTURE_ACCESS_REQUEST_H_
#define ARK_WEB_SCREEN_CAPTURE_ACCESS_REQUEST_H_
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"
#include "ohos_nweb/include/ark_web_nweb_structs.h"

namespace OHOS::ArkWeb {

/*--ark web(source=web core)--*/
class ArkWebScreenCaptureAccessRequest : public virtual ArkWebBaseRefCounted {
public:
  /**
   * @brief Agree the origin to access the given resources. The granted access
   *        is only valid for this WebView.
   *
   * @param config screen capture config.
   */
  /*--ark web()--*/
  virtual void Agree(const ArkWebScreenCaptureConfig &config) = 0;

  /**
   * @brief Refuse the request.
   */
  /*--ark web()--*/
  virtual void Refuse() = 0;

  /**
   * @brief Get the origin of the web page which is trying to access the screen
   *        capture resource.
   *
   * @return the origin of the web page which is trying to access the resource.
   */
  /*--ark web()--*/
  virtual ArkWebString Origin() = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_SCREEN_CAPTURE_ACCESS_REQUEST_H_
