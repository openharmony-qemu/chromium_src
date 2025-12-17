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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/dom/document_recognise_content.h"

#include "base/strings/string_util.h"
#include "third_party/blink/renderer/core/html/html_anchor_element.h"
#include "third_party/blink/renderer/core/html/html_head_element.h"
#include "third_party/blink/renderer/core/html/html_meta_element.h"
#include "third_party/blink/renderer/core/html_names.h"
#include "third_party/blink/renderer/platform/wtf/casting.h"
#include "third_party/blink/renderer/platform/wtf/text/atomic_string.h"

#include <regex>

namespace blink {

DocumentRecogniseContent::~DocumentRecogniseContent() = default;

bool DocumentRecogniseContent::ExistInCandidates(
    const String& text,
    const std::vector<std::string>& candidates) {
  if (text.empty() || candidates.empty()) {
    return false;
  }
  String strip_text = text.RemoveCharacters(IsASCIISpace);
  for (auto& candidate_str : candidates) {
    if (strip_text.Contains(String::FromUTF8(candidate_str.c_str()))) {
      switch (match_feature_state_) {
        case MatchFeatureState::PRE:
          match_info_.prev_feature = candidate_str;
          break;
        case MatchFeatureState::NEXT:
          match_info_.next_feature = candidate_str;
          break;
        case MatchFeatureState::CHAPTER:
          match_info_.catalog_feature = candidate_str;
          break;
        default:
          break;
      }
      return true;
    }
  }
  return false;
}

void DocumentRecogniseContent::CheckMatchRegexAndUpdateFeature(
    bool& feature,
    const String& text,
    const std::vector<std::string>& regexes) {
  if (feature || text.empty() || regexes.empty()) {
    return;
  }
  String strip_text = text.RemoveCharacters(IsASCIISpace);
  std::string strip_text_utf8 = strip_text.Utf8();
  for (auto& regex_utf8 : regexes) {
    std::regex re(regex_utf8);
    if (std::regex_match(strip_text_utf8, re)) {
      feature = true;
      return;
    }
  }
}

void DocumentRecogniseContent::CheckMatchAndUpdateFeature(
    bool& feature,
    const String& text,
    const std::vector<std::string>& candidates) {
  if (!feature) {
    feature = ExistInCandidates(text, candidates);
  }
}

void DocumentRecogniseContent::CheckParentMatchAndUpdateFeature(
    bool& feature,
    const Element& child_node,
    const std::vector<std::string>& candidates) {
  if (feature) {
    return;
  }

  Element* parent_node = child_node.parentElement();
  if (parent_node == nullptr) {
    return;
  }
  String text = parent_node->innerText();
  feature = ExistInCandidates(text, candidates);
}

void DocumentRecogniseContent::CheckATagAndUpdateMatchFeatures(
    NovelContentFeatures& features,
    Element& element) {
  NovelTemplateFeatures& instance = NovelTemplateFeatures::GetInstance();
  String text = element.innerText();

  match_feature_state_ = MatchFeatureState::PRE;
  CheckMatchAndUpdateFeature(features.has_prev_btn, text,
                             instance.GetLikelyPrevCandidates());
  CheckParentMatchAndUpdateFeature(features.has_prev_btn, element,
                                   instance.GetLikelyPrevCandidates());

  match_feature_state_ = MatchFeatureState::NEXT;
  CheckMatchAndUpdateFeature(features.has_next_btn, text,
                             instance.GetLikelyNextCandidates());
  CheckParentMatchAndUpdateFeature(features.has_next_btn, element,
                                   instance.GetLikelyNextCandidates());
  CheckMatchRegexAndUpdateFeature(features.has_next_btn, text,
                                  instance.GetPreNextRegexes());

  match_feature_state_ = MatchFeatureState::CHAPTER;
  CheckMatchAndUpdateFeature(features.has_chapterlist_btn, text,
                             instance.GetLikelyChapterListCandidates());
  CheckParentMatchAndUpdateFeature(features.has_chapterlist_btn, element,
                                   instance.GetLikelyChapterListCandidates());

  match_feature_state_ = MatchFeatureState::NONE;

  CheckMatchAndUpdateFeature(features.has_bookmark_btn, text,
                             instance.GetLikelyBookMarkCandidates());
}

void DocumentRecogniseContent::CollectFeaturesForReaderMode(
    NovelContentFeatures& features,
    Element& root,
    unsigned int& traversal_node_depth) {
  if (traversal_node_depth >= TRAVERSAL_CONTROL_CHILD_DEPTH_MAX) {
    return;
  }
  unsigned int child_depth = traversal_node_depth + 1;

  for (Element& element : ElementTraversal::ChildrenOf(root)) {
    if (element.HasTagName(html_names::kATag)) {
      CheckATagAndUpdateMatchFeatures(features, element);
    }
    CollectFeaturesForReaderMode(features, element, child_depth);
  }
}

MatchFeatureType DocumentRecogniseContent::IsMatchFeatures(Element& root) {
  NovelContentFeatures features;
  features.has_prev_btn = false;
  features.has_next_btn = false;
  features.has_chapterlist_btn = false;
  features.has_bookmark_btn = false;
  unsigned int traversal_node_depth = 0;

  CollectFeaturesForReaderMode(features, root, traversal_node_depth);

  bool match_feature = IsNovelContentMatchFeature(features);
  if (match_feature) {
    return MatchFeatureType::MATCH_FEATURES;
  }

  if (features.has_prev_btn || features.has_next_btn) {
    return MatchFeatureType::MATCH_PART_FEATURES;
  }

  return MatchFeatureType::MATCH_NONE_FEATURES;
}

MatchFeatureType DocumentRecogniseContent::IsMatchControlTemplates(
    Document& document,
    const std::string& control_template) {
  if (control_template.empty()) {
    return MatchFeatureType::MATCH_NONE_FEATURES;
  }

  StaticElementList* content_control_elements = document.QuerySelectorAll(
      WTF::AtomicString::FromUTF8(control_template.c_str()));
  if (!content_control_elements || content_control_elements->length() == 0) {
    return MatchFeatureType::MATCH_NONE_FEATURES;
  }

  bool match_part_feature = false;
  for (unsigned int j = 0; j < content_control_elements->length(); j++) {
    MatchFeatureType match_features =
        IsMatchFeatures(*(content_control_elements->item(j)));
    if (match_features == MatchFeatureType::MATCH_FEATURES) {
      return match_features;
    }
    if (match_features == MatchFeatureType::MATCH_PART_FEATURES) {
      match_part_feature = true;
    }
  }

  return match_part_feature ? MatchFeatureType::MATCH_PART_FEATURES
                            : MatchFeatureType::MATCH_NONE_FEATURES;
}

bool DocumentRecogniseContent::IsMatchContentTemplates(
    Document& document,
    const std::string& content_template) {
  if (content_template.empty()) {
    return false;
  }
  Element* content_element = document.QuerySelector(
      WTF::AtomicString::FromUTF8(content_template.c_str()));
  if (!content_element) {
    return false;
  }

  unsigned int minimum_content_length = static_cast<unsigned int>(
      NovelTemplateFeatures::GetInstance().GetMinimumContentLength());
  if (content_element->innerText().length() < minimum_content_length) {
    LOG(INFO) << __func__ << " [Distiller] content too less for "
              << content_template
              << " content length:" << content_element->innerText().length();

    auto txt_attribute = WTF::AtomicString::FromUTF8("data-txt");
    if (!content_element->hasAttribute(txt_attribute)) {
      return false;
    }

    if (content_element->getAttribute(txt_attribute).length() <
        minimum_content_length) {
      LOG(INFO) << __func__ << " [Distiller] content attribute too less for "
                << content_template << " attribute length:"
                << content_element->innerText().length();
      return false;
    }
  }

  if (IsElementDisplayNone(content_element)) {
    return false;
  }

  return true;
}

bool DocumentRecogniseContent::IsMetaTypeNovel(Document& document) {
  HTMLElement* head = document.head();
  if (!head) {
    return false;
  }

  auto property_attribute = WTF::AtomicString::FromUTF8("property");
  auto property_value = WTF::AtomicString::FromUTF8("og:type");
  auto content_attribute = WTF::AtomicString::FromUTF8("content");
  auto novel_value = WTF::AtomicString::FromUTF8("novel");
  auto novel_con_value = WTF::AtomicString::FromUTF8("novelcontent");

  for (Node* child = head->firstChild(); child; child = child->nextSibling()) {
    if (!child->HasTagName(html_names::kMetaTag)) {
      continue;
    }

    auto* meta = To<HTMLMetaElement>(child);

    bool need_check_meta_attr =
        meta->hasAttribute(property_attribute) &&
        meta->getAttribute(property_attribute) == property_value;
    if (need_check_meta_attr) {
      bool isNovel = meta->hasAttribute(content_attribute) &&
                     (meta->getAttribute(content_attribute) == novel_value ||
                      meta->getAttribute(content_attribute) == novel_con_value);
      if (isNovel) {
        LOG(INFO) << __func__ << " [Distiller] content_attribute "
                  << meta->getAttribute(content_attribute);
        return true;
      }
    }
  }

  return false;
}

void DocumentRecogniseContent::ChildDocDistillabilityMatch(
    Document& document,
    const blink::mojom::UrlHostDistillerInfoPtr& distiller_info) {
  bool distill_by_regex = !distiller_info->content_regexes.empty();
  if (distill_by_regex) {
    match_info_.distill_by_regex = true;
    if (IsDocUrlMatchRegexes(document, distiller_info->content_regexes)) {
      match_info_.result = WebDistillabilityMatchResult::MATCH_TEMPLATE;
    }
    return;
  }

  const auto* novel_content_page_templates =
      NovelTemplateFeatures::GetInstance().GetContentPageTemplates();
  if (!novel_content_page_templates) {
    return;
  }

  bool match_content_part_feature = false;
  std::vector<std::string> templates;
  int32_t template_id = 0;
  for (const auto& page_template : *novel_content_page_templates) {
    if (page_template.second.size() != PageTemplates::PAGE_MAX_IDX) {
      continue;
    }

    if (!IsMatchContentTemplates(
            document, page_template.second[PageTemplates::PAGE_CONTENT])) {
      continue;
    }

    MatchFeatureType match_control = IsMatchControlTemplates(
        document, page_template.second[PageTemplates::PAGE_CONTROL]);
    if (match_control == MatchFeatureType::MATCH_FEATURES) {
      LOG(INFO) << __func__ << " [Distiller] MATCH_TEMPLATE template:"
                << page_template.second[PageTemplates::PAGE_CONTENT];
      ConfigMatchInfo(page_template.first, page_template.second);
      return;
    }

    LOG(INFO) << __func__
              << " [Distiller] control element not match features for "
              << page_template.second[PageTemplates::PAGE_CONTROL]
              << ", page content is " << page_template.second[PageTemplates::PAGE_CONTENT];

    if (!match_content_part_feature &&
        match_control == MatchFeatureType::MATCH_PART_FEATURES) {
      LOG(INFO) << __func__ << " [Distiller] only match content for "
                << page_template.second[PageTemplates::PAGE_CONTROL];
      match_content_part_feature = true;
      templates = page_template.second;
      template_id = page_template.first;
    }
  }

  if (match_content_part_feature && IsMetaTypeNovel(document)) {
    LOG(INFO) << __func__ << " [Distiller] MATCH_TEMPLATE match meta type.";
    ConfigMatchInfo(template_id, templates);
  }
}

bool DocumentRecogniseContent::IsNovelContentMatchFeature(
    const NovelContentFeatures& features) {
  bool match_feature =
      (features.has_prev_btn && features.has_chapterlist_btn) ||
      (features.has_next_btn && features.has_chapterlist_btn) ||
      (features.has_chapterlist_btn && features.has_bookmark_btn);
  if (!NovelTemplateFeatures::GetInstance().MustHaveCatalog()) {
    match_feature =
        match_feature || (features.has_prev_btn && features.has_next_btn);
  }
  return match_feature;
}

void DocumentRecogniseContent::ConfigMatchInfo(
    const int32_t template_id,
    const std::vector<std::string>& page_template) {
  match_info_.result = WebDistillabilityMatchResult::MATCH_TEMPLATE;
  match_info_.template_id = template_id;
  match_info_.templates.clear();
  match_info_.templates = base::JoinString(page_template, ",");
}

}  // namespace blink
