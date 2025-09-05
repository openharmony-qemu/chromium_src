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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_DOM_DOCUMENT_RECOGNISE_CONTENT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_DOM_DOCUMENT_RECOGNISE_CONTENT_H_

#include "arkweb/chromium_ext/third_party/blink/renderer/core/dom/document_novel_template_features.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/core/dom/document_recognise_base.h"

namespace blink {

enum class MatchFeatureType {
  MATCH_NONE_FEATURES = 0,
  MATCH_PART_FEATURES,
  MATCH_FEATURES,
};

enum class MatchFeatureState {
  NONE = 0,
  PRE,
  NEXT,
  CHAPTER,
};

const unsigned int TRAVERSAL_CONTROL_CHILD_DEPTH_MAX = 10;

class DocumentRecogniseContent : public DocumentRecogniseBase {
 public:
  DocumentRecogniseContent() = default;
  ~DocumentRecogniseContent() override;

 protected:
  void ChildDocDistillabilityMatch(
      Document& document,
      const blink::mojom::UrlHostDistillerInfoPtr& distiller_info) override;

 private:
  bool ExistInCandidates(const String& text,
                         const std::vector<std::string>& candidates);
  inline void CheckMatchRegexAndUpdateFeature(
      bool& feature,
      const String& text,
      const std::vector<std::string>& regex);
  inline void CheckMatchAndUpdateFeature(
      bool& feature,
      const String& text,
      const std::vector<std::string>& candidates);
  inline void CheckParentMatchAndUpdateFeature(
      bool& feature,
      const Element& child_node,
      const std::vector<std::string>& candidates);

  void CheckATagAndUpdateMatchFeatures(NovelContentFeatures& features,
                                       Element& element);
  void CollectFeaturesForReaderMode(NovelContentFeatures& features,
                                    Element& root,
                                    unsigned int& traversal_node_depth);

  MatchFeatureType IsMatchFeatures(Element& root);
  bool IsMatchContentTemplates(Document& document,
                               const std::string& content_template);

  MatchFeatureType IsMatchControlTemplates(Document& document,
                                           const std::string& control_template);

  bool IsMetaTypeNovel(Document& document);

  void ConfigMatchInfo(const int32_t template_id, const std::vector<std::string>& page_template);
  bool IsNovelContentMatchFeature(const NovelContentFeatures& features);
  MatchFeatureState match_feature_state_ = MatchFeatureState::NONE;
};
}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_DOM_DOCUMENT_RECOGNISE_CONTENT_H_
