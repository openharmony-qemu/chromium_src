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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/dom/document_novel_template_features.h"

#include "arkweb/chromium_ext/third_party/blink/public/mojom/dom_distiller/reader_mode_config.mojom.h"
#include "third_party/blink/public/platform/platform.h"

namespace blink {
NovelTemplateFeatures& NovelTemplateFeatures::GetInstance() {
  DEFINE_STATIC_LOCAL(NovelTemplateFeatures, instance, ());
  return instance;
}

const base::flat_map<int32_t, std::vector<std::string>>*
NovelTemplateFeatures::GetContentPageTemplates() {
  if (const auto* config = blink::Platform::Current()->GetReaderModeConfig()) {
    return &config->content_templates;
  }
  return nullptr;
}

const base::flat_map<int32_t, std::vector<std::string>>*
NovelTemplateFeatures::GetDetailsPageTemplates() {
  if (const auto* config = blink::Platform::Current()->GetReaderModeConfig()) {
    return &config->detail_templates;
  }
  return nullptr;
}

const std::vector<std::string>&
NovelTemplateFeatures::GetLikelyPrevCandidates() {
  if (const auto* config = blink::Platform::Current()->GetReaderModeConfig()) {
    if (config->content_features.find(
            blink::mojom::ContentFeatureIndex::PREV) !=
        config->content_features.end()) {
      return config->content_features.at(
          blink::mojom::ContentFeatureIndex::PREV);
    }
  }
  return likely_prev_candidates_;
}

const std::vector<std::string>&
NovelTemplateFeatures::GetLikelyNextCandidates() {
  if (const auto* config = blink::Platform::Current()->GetReaderModeConfig()) {
    if (config->content_features.find(
            blink::mojom::ContentFeatureIndex::NEXT) !=
        config->content_features.end()) {
      return config->content_features.at(
          blink::mojom::ContentFeatureIndex::NEXT);
    }
  }
  return likely_next_candidates_;
}

const std::vector<std::string>&
NovelTemplateFeatures::GetLikelyChapterListCandidates() {
  if (const auto* config = blink::Platform::Current()->GetReaderModeConfig()) {
    if (config->content_features.find(
            blink::mojom::ContentFeatureIndex::CATALOG) !=
        config->content_features.end()) {
      return config->content_features.at(
          blink::mojom::ContentFeatureIndex::CATALOG);
    }
  }
  return likely_chapterlist_candidates_;
}

const std::vector<std::string>&
NovelTemplateFeatures::GetLikelyBookMarkCandidates() {
  return likely_bookmark_candidates_;
}

const std::vector<std::string>& NovelTemplateFeatures::GetPreNextRegexes() {
  return pre_next_regexes_;
}

NovelTemplateFeatures::NovelTemplateFeatures() {
  likely_prev_candidates_ = {"第一章", "上一篇", "上一章", "上章",
                             "上一节", "上节",   "上一页", "上页",
                             "前页",   "上ー章", "上一頁", "首页"};

  likely_next_candidates_ = {"末页", "等待更新", "下一篇", "下一章",
                             "下章", "下一节",   "下节",   "下一页",
                             "下页", "后页",     "下ー章", "下一頁"};

  likely_chapterlist_candidates_ = {"目录", "列表", "书页", "查看最新章节"};

  likely_bookmark_candidates_ = {"书签"};

  pre_next_regexes_ = {"第\\d+章"};
}

bool NovelTemplateFeatures::MustHaveCatalog() {
  if (const auto* config = blink::Platform::Current()->GetReaderModeConfig()) {
    return config->must_have_catalog;
  }
  return false;  // false is default
}

int NovelTemplateFeatures::GetMinimumContentLength() {
  if (const auto* config = blink::Platform::Current()->GetReaderModeConfig()) {
    return config->minimum_content_length;
  }
  return 100;  // 100 is default
}

}  // namespace blink
