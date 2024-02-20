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

#ifndef ARK_WEB_FIND_CALLBACK_H_
#define ARK_WEB_FIND_CALLBACK_H_
#pragma once

#include "base/include/ark_web_base_ref_counted.h"

namespace OHOS::ArkWeb {

/*--ark web(source=library)--*/
class ArkWebFindCallback : public virtual ArkWebBaseRefCounted {
public:
  /**
   * @brief Notify the host application that OnFindResultReceived
   *
   * @param active_match_ordinal int: the zero-based ordinal of the currently
   *        selected match
   * @param number_of_matches int: how many matches have been found
   * @param is_done_counting bool: whether the find operation has actually
   *        completed. The listener may be notified multiple times while the
   *        operation is underway, and the number_of_matches value should not be
   *        considered final unless is_done_counting is true.
   */
  /*--ark web()--*/
  virtual void OnFindResultReceived(const int active_match_ordinal,
                                    const int number_of_matches,
                                    const bool is_done_counting) = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_FIND_CALLBACK_H_
