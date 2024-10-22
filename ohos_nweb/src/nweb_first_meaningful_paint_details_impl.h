/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef NWEB_FIRST_MEANINGFUL_PAINT_DETAILS_IMPL_H
#define NWEB_FIRST_MEANINGFUL_PAINT_DETAILS_IMPL_H

#include "nweb_first_meaningful_paint_details.h"

namespace OHOS::NWeb {
class NWebFirstMeaningfulPaintDetailsImpl : public NWebFirstMeaningfulPaintDetails {
 public:
  NWebFirstMeaningfulPaintDetailsImpl(int64_t navigationStartTime,
                                      int64_t firstMeaningfulPaintTime);
  ~NWebFirstMeaningfulPaintDetailsImpl() = default;

  /**
   * @brief Get start time of navigation.
   *
   * @return Start time of navigation.
   */
  int64_t GetNavigationStartTime() override;

  /**
   * @brief Get paint time of first meaningful content.
   *
   * @return Paint time of first meaningful content.
   */
  int64_t GetFirstMeaningfulPaintTime() override;

 private:
  int64_t navigationStartTime_;
  int64_t firstMeaningfulPaintTime_;
};
}
#endif // NWEB_FIRST_MEANINGFUL_PAINT_DETAILS_IMPL_H