// Copyright 2016 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/subresource_filter/core/common/document_subresource_filter.h"

#include <memory>
#include <string_view>
#include <utility>

#include "base/check.h"
#include "base/check_op.h"
#include "base/metrics/histogram_functions.h"
#include "base/not_fatal_until.h"
#include "base/strings/strcat.h"
#include "base/time/time.h"
#include "base/trace_event/trace_event.h"
#include "components/subresource_filter/core/common/first_party_origin.h"
#include "components/subresource_filter/core/common/memory_mapped_ruleset.h"
#include "components/subresource_filter/core/common/scoped_timers.h"
#include "components/subresource_filter/core/common/time_measurements.h"
#include "url/gurl.h"
#include "url/origin.h"

#if BUILDFLAG(ARKWEB_ADBLOCK)
#include "base/logging.h"
#include "base/strings/string_number_conversions.h"
#endif

namespace subresource_filter {

DocumentSubresourceFilter::DocumentSubresourceFilter(
    url::Origin document_origin,
    mojom::ActivationState activation_state,
    scoped_refptr<const MemoryMappedRuleset> ruleset,
    std::string_view uma_tag)
    : activation_state_(activation_state),
      ruleset_(std::move(ruleset)),
      ruleset_matcher_(ruleset_->data()),
      uma_tag_(uma_tag) {
  CHECK_NE(activation_state_.activation_level,
           mojom::ActivationLevel::kDisabled, base::NotFatalUntil::M129);
  if (!activation_state_.filtering_disabled_for_document) {
    document_origin_ =
        std::make_unique<FirstPartyOrigin>(std::move(document_origin));
  }
}

DocumentSubresourceFilter::~DocumentSubresourceFilter() = default;

LoadPolicy DocumentSubresourceFilter::GetLoadPolicy(
    const GURL& subresource_url,
    url_pattern_index::proto::ElementType subresource_type) {
  TRACE_EVENT1(TRACE_DISABLED_BY_DEFAULT("loading"),
               "DocumentSubresourceFilter::GetLoadPolicy", "url",
               subresource_url.spec());

  ++statistics_.num_loads_total;

  if (activation_state_.filtering_disabled_for_document)
    return LoadPolicy::ALLOW;
  if (subresource_url.SchemeIs(url::kDataScheme))
    return LoadPolicy::ALLOW;

  // If ThreadTicks is not supported, then no CPU time measurements have been
  // collected. Don't report both CPU and wall duration to be consistent.
  auto wall_duration_timer = ScopedTimers::StartIf(
      activation_state_.measure_performance &&
          ScopedThreadTimers::IsSupported(),
      [this](base::TimeDelta delta) {
        statistics_.evaluation_total_wall_duration += delta;
        // Here we use custom histograms to maintain subresource filter
        // metrics, which used UMA_HISTOGRAM_MICRO_TIMES prior to introducing
        // support for fingerprinting protection filter counterpart.
        UmaHistogramCustomMicrosecondsTimes(
            base::StrCat(
                {uma_tag_, ".SubresourceLoad.Evaluation.WallDuration"}),
            delta, base::Microseconds(1), base::Seconds(1), 50);
      });
  auto cpu_duration_timer = ScopedThreadTimers::StartIf(
      activation_state_.measure_performance, [this](base::TimeDelta delta) {
        statistics_.evaluation_total_cpu_duration += delta;
        UmaHistogramCustomMicrosecondsTimes(
            base::StrCat({uma_tag_, ".SubresourceLoad.Evaluation.CPUDuration"}),
            delta, base::Microseconds(1), base::Seconds(1), 50);
      });

  ++statistics_.num_loads_evaluated;
  CHECK(document_origin_, base::NotFatalUntil::M129);
  LoadPolicy result = ruleset_matcher_.GetLoadPolicyForResourceLoad(
      subresource_url, *document_origin_, subresource_type,
      activation_state_.generic_blocking_rules_disabled);
  CHECK_NE(LoadPolicy::WOULD_DISALLOW, result, base::NotFatalUntil::M129);
  if (result == LoadPolicy::DISALLOW) {
    ++statistics_.num_loads_matching_rules;
    if (activation_state_.activation_level ==
        mojom::ActivationLevel::kEnabled) {
#if BUILDFLAG(ARKWEB_ADBLOCK)
      statistics_.loads_disallowed_url_map.insert(
          std::make_pair(subresource_url.spec(), subresource_type));
#endif
      ++statistics_.num_loads_disallowed;
      return LoadPolicy::DISALLOW;
    } else if (activation_state_.activation_level ==
               mojom::ActivationLevel::kDryRun) {
      return LoadPolicy::WOULD_DISALLOW;
    }
  }
  return result;
}

const url_pattern_index::flat::UrlRule*
DocumentSubresourceFilter::FindMatchingUrlRule(
    const GURL& subresource_url,
    url_pattern_index::proto::ElementType subresource_type) {
  if (activation_state_.filtering_disabled_for_document)
    return nullptr;
  if (subresource_url.SchemeIs(url::kDataScheme))
    return nullptr;

  return ruleset_matcher_.MatchedUrlRule(
      subresource_url, *document_origin_, subresource_type,
      activation_state_.generic_blocking_rules_disabled);
}

#if BUILDFLAG(ARKWEB_ADBLOCK)
std::unique_ptr<std::string> DocumentSubresourceFilter::GetSelectors(
    const GURL& document_url,
    bool disable_generic_rules) const {
  auto rules = FindMatchingCssRule(document_url, disable_generic_rules);
  if (!rules) {
    return nullptr;
  }

  std::unique_ptr<std::string> selectors = std::make_unique<std::string>();
  // Matched 21823 css rule(21636 generic and 187 special) in
  // https://www.baidu.com, selectors.size=992794
  selectors->reserve(1310720);  //(1024+256)*1024=1310720

  for (auto* rule : *rules) {
    if (!rule) {
      continue;
    }

    if (rule->css_selector()) {
      *selectors +=
          rule->css_selector()->str() + " {display: none !important;}\n";
    }
  }

  return selectors;
}

std::unique_ptr<const std::vector<const url_pattern_index::flat::CssRule*>>
DocumentSubresourceFilter::FindMatchingCssRule(
    const GURL& document_url,
    bool disable_generic_rules) const {
  if (activation_state_.filtering_disabled_for_document) {
    return nullptr;
  }

  if (document_url.SchemeIs(url::kDataScheme)) {
    return nullptr;
  }

  return ruleset_matcher_.MatchedCssRule(document_url, disable_generic_rules);
}

bool DocumentSubresourceFilter::HasGenericHideTypeOption(
    const GURL& document_url,
    const url::Origin& parent_document_origin) const {
  return ruleset_matcher_.HasGenericHideOption(document_url,
                                               parent_document_origin);
}

bool DocumentSubresourceFilter::HasElemHideTypeOption(
    const GURL& document_url,
    const url::Origin& parent_document_origin) const {
  return ruleset_matcher_.HasElemHideOption(document_url,
                                            parent_document_origin);
}

bool DocumentSubresourceFilter::HasDocumentTypeOption(
    const GURL& document_url,
    const url::Origin& parent_document_origin) const {
  return ruleset_matcher_.HasDocumentOption(document_url,
                                            parent_document_origin);
}

void DocumentSubresourceFilter::DidMatchCssRule(const GURL& document_url,
                                                const std::string& dom_path,
                                                // unsigned rule_line_num,
                                                bool is_for_report) {
  std::string tmp = document_url.spec() + "##" + dom_path;

  if (!GetDidFinishLoad() && is_for_report) {
    return;
  }

  for (auto& pair : statistics_.loads_disallowed_url_map) {
    if (tmp.find(pair.first) != tmp.npos) {
      return;
    }

    if (pair.first.find(tmp) != pair.first.npos) {
      statistics_.loads_disallowed_url_map.erase(pair.first);
      break;
    }
  }

  statistics_.loads_disallowed_url_map.insert(std::make_pair(
      tmp, url_pattern_index::proto::ElementType::ELEMENT_TYPE_OTHER));
}

void DocumentSubresourceFilter::ClearStatistics() {
  statistics_.num_loads_total = 0;
  statistics_.num_loads_evaluated = 0;
  statistics_.num_loads_matching_rules = 0;
  statistics_.num_loads_disallowed = 0;
  statistics_.loads_disallowed_url_map.clear();
}

std::unique_ptr<std::vector<std::string>>
DocumentSubresourceFilter::GetUserDomPathSelectors(
    const GURL& document_url,
    bool disable_generic_rules) const {
  auto rules = FindMatchingCssRule(document_url, disable_generic_rules);
  if (!rules) {
    return nullptr;
  }
  std::unique_ptr<std::vector<std::string>> selectors =
      std::make_unique<std::vector<std::string>>();
  for (auto* rule : *rules) {
    if (!rule) {
      continue;
    }
    if (rule->css_selector()) {
      selectors->push_back(rule->css_selector()->str());
    }
  }
  return selectors;
}
#endif

}  // namespace subresource_filter
