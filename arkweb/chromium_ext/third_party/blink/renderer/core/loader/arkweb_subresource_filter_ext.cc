// Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.

#include "third_party/blink/renderer/core/loader/subresource_filter.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/core/loader/arkweb_subresource_filter_ext.h"

#include <utility>

#include "base/location.h"
#include "base/task/single_thread_task_runner.h"
#include "third_party/blink/public/mojom/fetch/fetch_api_request.mojom-blink.h"
#include "third_party/blink/public/platform/task_type.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/inspector/console_message.h"
#include "third_party/blink/renderer/core/loader/document_loader.h"
#include "third_party/blink/renderer/platform/heap/garbage_collected.h"
#include "third_party/blink/renderer/platform/weborigin/kurl.h"
#include "third_party/blink/renderer/platform/wtf/functional.h"
#include "third_party/blink/renderer/platform/wtf/text/string_builder.h"

namespace blink {

ArkWebSubresourceFilterExt::ArkWebSubresourceFilterExt(SubresourceFilter *filter) {
  subresourceFilter = filter;
}
ArkWebSubresourceFilterExt::~ArkWebSubresourceFilterExt() = default;

#if BUILDFLAG(ARKWEB_ADBLOCK)
void ArkWebSubresourceFilterExt::RequestSendStatistics(base::TimeDelta delay) {
  if (subresourceFilter->statistics_timer_.IsActive()) {
    return;
  }
  subresourceFilter->statistics_timer_.StartOneShot(delay, FROM_HERE);
}

void SubresourceFilter::SendStatistics(TimerBase*) {
  if (auto* window = DynamicTo<LocalDOMWindow>(execution_context_.Get())) {
    if (window->GetFrame()) {
      window->GetFrame()->DidSubresourceFiltered();
    }
  }
}

std::unique_ptr<std::string> ArkWebSubresourceFilterExt::GetElementHidingSelectors(
    const KURL& document_url,
    bool need_common_selectors) {
  return subresourceFilter->subresource_filter_->GetElementHidingSelectors(document_url,
                                                                           need_common_selectors);
}

WebDocumentSubresourceFilter* ArkWebSubresourceFilterExt::GetWebDocumentSubresourceFilter() {
  return subresourceFilter->subresource_filter_.get();
}

void ArkWebSubresourceFilterExt::DidMatchCssRule(const KURL& document_url,
                                        const std::string& dom_path,
                                        // unsigned rule_line_num,
                                        bool is_for_report) {
  subresourceFilter->subresource_filter_->DidMatchCssRule(document_url, dom_path,
                                                          //  rule_line_num,
                                                          is_for_report);
  RequestSendStatistics(base::Milliseconds(1000));
}

bool ArkWebSubresourceFilterExt::GetDidFinishLoad() {
  return subresourceFilter->subresource_filter_->GetDidFinishLoad();
}

bool ArkWebSubresourceFilterExt::GetAdBlockEnabledByPage() const {
  auto* window = DynamicTo<LocalDOMWindow>(subresourceFilter->execution_context_.Get());
  if (!window) {
    return false;
  }

  LocalFrame* local_frame = window->GetFrame();
  if (!local_frame) {
    return false;
  }
  LocalFrame& local_frame_root = local_frame->LocalFrameRoot();

  if (!local_frame_root.GetGlobalAdblockEnabled()) {
    return false;
  }
  if (local_frame_root.GetAdBlockEnableForSite()) {
    return true;
  }
  return false;
}
#endif

void ArkWebSubresourceFilterExt::Trace(Visitor* visitor) const {
  visitor->Trace(subresourceFilter);
}

}  // namespace blink
