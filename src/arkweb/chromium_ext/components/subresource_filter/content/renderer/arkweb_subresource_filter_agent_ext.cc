// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "components/subresource_filter/content/renderer/subresource_filter_agent.h"

#include <utility>

#include "base/check.h"
#include "base/feature_list.h"
#include "base/functional/bind.h"
#include "base/memory/ref_counted.h"
#include "base/metrics/histogram_macros.h"
#include "base/not_fatal_until.h"
#include "base/time/time.h"
#include "components/subresource_filter/content/renderer/unverified_ruleset_dealer.h"
#include "components/subresource_filter/content/shared/common/utils.h"
#include "components/subresource_filter/core/common/document_subresource_filter.h"
#include "components/subresource_filter/core/common/memory_mapped_ruleset.h"
#include "components/subresource_filter/content/renderer/web_document_subresource_filter_impl.h"
#include "components/subresource_filter/core/common/scoped_timers.h"
#include "components/subresource_filter/core/common/time_measurements.h"
#include "content/public/common/content_features.h"
#include "content/public/common/url_constants.h"
#include "content/public/renderer/render_frame.h"
#include "ipc/ipc_message.h"
#include "third_party/blink/public/common/associated_interfaces/associated_interface_provider.h"
#include "third_party/blink/public/common/associated_interfaces/associated_interface_registry.h"
#include "third_party/blink/public/common/features.h"
#include "third_party/blink/public/common/frame/frame_ad_evidence.h"
#include "third_party/blink/public/platform/web_worker_fetch_context.h"
#include "third_party/blink/public/web/web_document.h"
#include "third_party/blink/public/web/web_document_loader.h"
#include "third_party/blink/public/web/web_local_frame.h"
#include "url/url_constants.h"

namespace subresource_filter {

ArkWebSubresourceFilterAgentExt::ArkWebSubresourceFilterAgentExt(
    content::RenderFrame* render_frame,
    UnverifiedRulesetDealer* ruleset_dealer)
    : SubresourceFilterAgent(render_frame, ruleset_dealer) {}

ArkWebSubresourceFilterAgentExt::~ArkWebSubresourceFilterAgentExt() = default;

#if BUILDFLAG(ARKWEB_ADBLOCK)
void ArkWebSubresourceFilterAgentExt::SendStatisticsAfterDocumentLoad(
    const mojom::DocumentLoadStatistics& statistics) {
  GetSubresourceFilterHost()->SetStatisticsAfterDocumentLoad(
      statistics.Clone());
}

void ArkWebSubresourceFilterAgentExt::DidSubresourceFiltered() {
  if (!filter_for_last_created_document_) {
    return;
  }
  if (did_load_finished_ == false) {
    return;
  }

  const auto& statistics =
      filter_for_last_created_document_->filter().statistics();
  SendStatisticsAfterDocumentLoad(statistics);
  filter_for_last_created_document_->ClearStatistics();
}

void ArkWebSubresourceFilterAgentExt::CalcElementHidingTypeOption(
    content::RenderFrame* render_frame) {
  // this render frame will never be null
  if (!render_frame) {
    LOG(ERROR) << "[AdBlock] render frame is null, will not calculate element "
                  "hiding type option";
    return;
  }

  blink::WebLocalFrame* web_local_frame = render_frame->GetWebFrame();
  if (!web_local_frame) {
    return;
  }

  blink::WebDocument document = web_local_frame->GetDocument();
  if (!document.Url().ProtocolIs("https") &&
      !document.Url().ProtocolIs("http")) {
    return;
  }

  blink::WebDocumentSubresourceFilter* filter =
      web_local_frame->GetDocumentLoader()->GetWebSubresourceFilter();
  if (!filter) {
    LOG(ERROR) << "[AdBlock] subresource filter is null, will not calculate"
                  "element hiding type option";
    return;
  }

  base::TimeTicks start = base::TimeTicks::Now();

  blink::WebDocument parent_document = document;
  blink::WebLocalFrame* parent_web_local_frame = nullptr;
  blink::WebFrame* parent_web_frame = web_local_frame->Parent();
  if (parent_web_frame && parent_web_frame->IsWebLocalFrame()) {
    parent_web_local_frame = parent_web_frame->ToWebLocalFrame();
    if (parent_web_local_frame) {
      parent_document = parent_web_local_frame->GetDocument();
    }
  }

  bool has_document_type_option = false;
  bool has_elemhide_type_option = false;
  bool has_generichide_type_option = false;

  bool parent_has_document_type_option = false;
  bool parent_has_elemhide_type_option = false;
  bool parent_has_generichide_type_option = false;

  // Assuming that parent frame will be created first
  if (parent_web_local_frame) {
    parent_has_document_type_option =
        parent_web_local_frame->GetHasDocumentTypeOption();
    parent_has_elemhide_type_option =
        parent_web_local_frame->GetHasElemHideTypeOption();
    parent_has_generichide_type_option =
        parent_web_local_frame->GetHasGenericHideTypeOption();
  }

  if (parent_has_document_type_option == true) {
    has_document_type_option = parent_has_document_type_option;
  } else {
    has_document_type_option = filter->HasDocumentTypeOption(
        document.Url(), url::Origin::Create(parent_document.Url()));
  }

  web_local_frame->SetHasDocumentTypeOption(has_document_type_option);
  if (parent_has_elemhide_type_option == true) {
    has_elemhide_type_option = parent_has_elemhide_type_option;
  } else {
    has_elemhide_type_option = filter->HasElemHideTypeOption(
        document.Url(), url::Origin::Create(parent_document.Url()));
  }

  web_local_frame->SetHasElemHideTypeOption(has_elemhide_type_option);

  if (parent_has_generichide_type_option == true) {
    has_generichide_type_option = parent_has_generichide_type_option;
  } else {
    has_generichide_type_option = filter->HasGenericHideTypeOption(
        document.Url(), url::Origin::Create(parent_document.Url()));
  }

  web_local_frame->SetHasGenericHideTypeOption(has_generichide_type_option);
  base::TimeDelta duration = base::TimeTicks::Now() - start;
  VLOG(2) << "[AdBlock] Calculate $document =" << has_document_type_option
          << ", $elemhide =" << has_elemhide_type_option
          << ", $generichide = " << has_generichide_type_option << " assumming "
          << duration.InMicroseconds() << " microseconds";
}
#endif
}  // namespace subresource_filter
