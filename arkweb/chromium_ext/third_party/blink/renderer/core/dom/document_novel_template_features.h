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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_DOM_DOCUMENT_NOVEL_TEMPLATE_FEATURES_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_DOM_DOCUMENT_NOVEL_TEMPLATE_FEATURES_H_

#include "base/containers/flat_map.h"
#include "third_party/blink/renderer/platform/allow_discouraged_type.h"
#include "third_party/blink/renderer/platform/wtf/text/atomic_string.h"

#include <array>

namespace blink {

enum PageTemplates {
  PAGE_CONTENT = 0,
  PAGE_CONTROL = 1,
  PAGE_MAX_IDX = 2
};

struct NovelContentFeatures {
  bool has_prev_btn;
  bool has_next_btn;
  bool has_chapterlist_btn;
  bool has_bookmark_btn;
};

class NovelTemplateFeatures {
 public:
  static NovelTemplateFeatures& GetInstance();
  const base::flat_map<int32_t, std::vector<std::string>>* GetContentPageTemplates();
  const base::flat_map<int32_t, std::vector<std::string>>* GetDetailsPageTemplates();
  const std::vector<std::string>& GetLikelyPrevCandidates();
  const std::vector<std::string>& GetLikelyNextCandidates();
  const std::vector<std::string>& GetLikelyChapterListCandidates();
  const std::vector<std::string>& GetLikelyBookMarkCandidates();
  const std::vector<std::string>& GetPreNextRegexes();

  bool MustHaveCatalog();
  int GetMinimumContentLength();

 private:
  NovelTemplateFeatures();
  std::vector<std::string> likely_prev_candidates_
      ALLOW_DISCOURAGED_TYPE("mojo mapped type");
  std::vector<std::string> likely_next_candidates_
      ALLOW_DISCOURAGED_TYPE("mojo mapped type");
  std::vector<std::string> likely_chapterlist_candidates_
      ALLOW_DISCOURAGED_TYPE("mojo mapped type");
  std::vector<std::string> likely_bookmark_candidates_
      ALLOW_DISCOURAGED_TYPE("mojo mapped type");
  std::vector<std::string> pre_next_regexes_
      ALLOW_DISCOURAGED_TYPE("mojo mapped type");
};
}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_DOM_DOCUMENT_NOVEL_TEMPLATE_FEATURES_H_
