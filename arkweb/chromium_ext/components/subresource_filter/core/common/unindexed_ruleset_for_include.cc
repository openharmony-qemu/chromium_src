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

#include "arkweb/build/features/features.h"

#if BUILDFLAG(ARKWEB_ADBLOCK)
#include "base/logging.h"
#endif

namespace subresource_filter {

#if BUILDFLAG(ARKWEB_ADBLOCK)
bool UnindexedRulesetWriter::AddCssRule(
    const url_pattern_index::proto::CssRule& rule) {
  DCHECK(!had_error());
  pending_chunk_.add_css_rules()->CopyFrom(rule);
  if (pending_chunk_.css_rules_size() >= max_rules_per_chunk_) {
    DCHECK_EQ(pending_chunk_.css_rules_size(), max_rules_per_chunk_);
    return WritePendingChunk();
  }

  return true;
}
#endif

}  // namespace subresource_filter