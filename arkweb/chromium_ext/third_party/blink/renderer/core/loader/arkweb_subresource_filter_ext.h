// Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_LOADER_ARKWEB_SUBRESOURCE_FILTER_EXT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_LOADER_ARKWEB_SUBRESOURCE_FILTER_EXT_H_

#include <memory>
#include <utility>

#include "services/network/public/mojom/fetch_api.mojom.h"
#include "third_party/blink/public/mojom/fetch/fetch_api_request.mojom-blink-forward.h"
#include "third_party/blink/public/platform/web_document_subresource_filter.h"
#include "third_party/blink/public/platform/web_url_request.h"
#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/platform/heap/garbage_collected.h"
#include "third_party/blink/renderer/platform/heap/member.h"
#include "third_party/blink/renderer/platform/weborigin/kurl.h"
#include "third_party/blink/renderer/platform/weborigin/reporting_disposition.h"

#if BUILDFLAG(ARKWEB_ADBLOCK)
#include "third_party/blink/renderer/core/page/page.h"
#include "third_party/blink/renderer/platform/timer.h"
#endif

namespace blink {

class ExecutionContext;
class KURL;
class SubresourceFilter;

class CORE_EXPORT ArkWebSubresourceFilterExt : public GarbageCollected<ArkWebSubresourceFilterExt> {
 public:
  Member<SubresourceFilter> subresourceFilter;
  ArkWebSubresourceFilterExt(SubresourceFilter *filter);
  ~ArkWebSubresourceFilterExt();

  void Trace(Visitor*) const;

#if BUILDFLAG(ARKWEB_ADBLOCK)
  std::unique_ptr<std::string> GetElementHidingSelectors(
      const KURL& document_url,
      bool need_common_selectors);

  WebDocumentSubresourceFilter* GetWebDocumentSubresourceFilter();

  void DidMatchCssRule(const KURL& document_url,
                       const std::string& dom_path,
                       bool is_for_report = false);

  bool GetDidFinishLoad();
#endif

 private:
  friend class SubresourceFilter;
#if BUILDFLAG(ARKWEB_ADBLOCK)
  void RequestSendStatistics(base::TimeDelta delay);
  bool GetAdBlockEnabledByPage() const;
#endif
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_LOADER_ARKWEB_SUBRESOURCE_FILTER_EXT_H_
