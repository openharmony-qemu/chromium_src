/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_NWEB_SRC_CAPI_NWEB_HITTEST_RESULT_H
#define OHOS_NWEB_SRC_CAPI_NWEB_HITTEST_RESULT_H

#include <string>

#include "ohos_nweb/include/nweb_hit_testresult.h"
namespace ohos_extension_hittest {
enum HitTestType {
  /**
   * Default HitTestResult, where the target is unknown.
   */
  UNKNOWN_TYPE = 0,
  /**
   * This type is no longer used.
   */
  ANCHOR_TYPE = 1,
  /**
   * HitTestResult for hitting a phone number.
   */
  PHONE_TYPE = 2,
  /**
   * HitTestResult for hitting a map address.
   */
  GEO_TYPE = 3,
  /**
   * HitTestResult for hitting an email address.
   */
  EMAIL_TYPE = 4,
  /**
   * HitTestResult for hitting an HTML::img tag.
   */
  IMAGE_TYPE = 5,
  /**
   * This type is no longer used.
   */
  IMAGE_ANCHOR_TYPE = 6,
  /**
   * HitTestResult for hitting a HTML::a tag with src=http.
   */
  SRC_ANCHOR_TYPE = 7,
  /**
   * HitTestResult for hitting a HTML::a tag with src=http + HTML::img.
   */
  SRC_IMAGE_ANCHOR_TYPE = 8,
  /**
   * HitTestResult for hitting an edit text area.
   */
  EDIT_TEXT_TYPE = 9,
};

enum WebHitTestType {
  EDIT = 0,
  EMAIL,
  HTTP,
  HTTP_IMG,
  IMG,
  MAP,
  PHONE,
  UNKNOWN
};
}  // namespace ohos_extension_hittest
#endif  // OHOS_NWEB_SRC_CAPI_NWEB_HITTEST_RESULT_H
