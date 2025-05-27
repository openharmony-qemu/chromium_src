/* Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_HIT_TEST_RESULT_UTILS_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_HIT_TEST_RESULT_UTILS_H_

#include "third_party/blink/renderer/core/layout/hit_test_result.h"

namespace blink {
class CORE_EXPORT HitTestResultUtils {
 public:
  HitTestResult* hit_test_result_;
  HitTestResultUtils(HitTestResult* impl);

#if BUILDFLAG(ARKWEB_DRAG_DROP)
  gfx::Rect GetReplacedContentRect() const;
  gfx::Rect GetLinkElementRect() const;
#endif
};
}  // namespace blink
#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_LAYOUT_HIT_TEST_RESULT_UTILS_H_
