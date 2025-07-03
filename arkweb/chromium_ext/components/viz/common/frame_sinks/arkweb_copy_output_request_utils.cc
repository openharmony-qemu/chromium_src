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

#include "components/viz/common/frame_sinks/copy_output_request.h"
#include "arkweb/chromium_ext/components/viz/common/frame_sinks/arkweb_copy_output_request_utils.h"

namespace viz {

ArkwebCopyOutputRequestUtils::ArkwebCopyOutputRequestUtils(CopyOutputRequest* CopyOutputRequest)
    : copyOutputRequest_(CopyOutputRequest) {}

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
void ArkwebCopyOutputRequestUtils::SetBlanklessKey(uint64_t key) {
  blankless_key_ = key;
}

void ArkwebCopyOutputRequestUtils::SetLcpTime(int32_t lcp_time) {
  lcp_time_ = lcp_time;
}

void ArkwebCopyOutputRequestUtils::SetPreferenceHash(int64_t pref_hash)
{
  pref_hash_ = pref_hash;
}

void ArkwebCopyOutputRequestUtils::SetQuadList(const std::vector<gfx::Rect>& quad_list) {
  quad_list_ = quad_list;
}

uint64_t ArkwebCopyOutputRequestUtils::GetBlanklessKey() const {
  return blankless_key_;
}

int32_t ArkwebCopyOutputRequestUtils::GetLcpTime() const {
  return lcp_time_;
}

int64_t ArkwebCopyOutputRequestUtils::GetPreferenceHash() const
{
  return pref_hash_;
}

const std::vector<gfx::Rect>& ArkwebCopyOutputRequestUtils::GetQuadList() const {
  return quad_list_;
}
#endif
}  // namespace viz