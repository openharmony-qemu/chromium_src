// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef COMPONENTS_SUBRESOURCE_FILTER_CONTENT_RENDERER_ARKWEB_SUBRESOURCE_FILTER_AGENT_EXT_H_
#define COMPONENTS_SUBRESOURCE_FILTER_CONTENT_RENDERER_ARKWEB_SUBRESOURCE_FILTER_AGENT_EXT_H_

#include <memory>

#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"
#include "components/subresource_filter/content/mojom/subresource_filter.mojom.h"
#include "components/subresource_filter/core/mojom/subresource_filter.mojom.h"
#include "content/public/renderer/render_frame_observer.h"
#include "content/public/renderer/render_frame_observer_tracker.h"
#include "mojo/public/cpp/bindings/associated_receiver.h"
#include "mojo/public/cpp/bindings/associated_remote.h"
#include "mojo/public/cpp/bindings/pending_associated_receiver.h"
#include "url/gurl.h"

namespace blink {
class WebDocumentSubresourceFilter;
}  // namespace blink

namespace subresource_filter {

class UnverifiedRulesetDealer;
class WebDocumentSubresourceFilterImpl;

// The renderer-side agent of ContentSubresourceFilterThrottleManager. There is
// one instance per RenderFrame, responsible for setting up the subresource
// filter for the ongoing provisional document load in the frame when instructed
// to do so by the manager.
class ArkWebSubresourceFilterAgentExt : public SubresourceFilterAgent {
 public:
  friend class SubresourceFilterAgent;
  // The |ruleset_dealer| must not be null and must outlive this instance. The
  // |render_frame| may be null in unittests.
  explicit ArkWebSubresourceFilterAgentExt(content::RenderFrame* render_frame,
                                  UnverifiedRulesetDealer* ruleset_dealer);

  ArkWebSubresourceFilterAgentExt(const ArkWebSubresourceFilterAgentExt&) = delete;
  ArkWebSubresourceFilterAgentExt& operator=(const ArkWebSubresourceFilterAgentExt&) = delete;

  ~ArkWebSubresourceFilterAgentExt() override;

  ArkWebSubresourceFilterAgentExt *AsArkWebSubresourceFilterAgentExt() override { return this; }

 protected:
#if BUILDFLAG(ARKWEB_ADBLOCK)
  virtual void SendStatisticsAfterDocumentLoad(
      const mojom::DocumentLoadStatistics& statistics);
#endif

 private:
#if BUILDFLAG(ARKWEB_ADBLOCK)
  void CalcElementHidingTypeOption(content::RenderFrame* render_frame);
  void DidSubresourceFiltered() override;
#endif
};

}  // namespace subresource_filter

#endif  // COMPONENTS_SUBRESOURCE_FILTER_CONTENT_RENDERER_ARKWEB_SUBRESOURCE_FILTER_AGENT_EXT_H_
