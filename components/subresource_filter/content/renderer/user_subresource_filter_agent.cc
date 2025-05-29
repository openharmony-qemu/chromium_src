// Copyright 2016 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/subresource_filter/content/renderer/user_subresource_filter_agent.h"

#include <utility>

#include "base/check.h"
#include "base/feature_list.h"
#include "base/functional/bind.h"
#include "base/memory/ref_counted.h"
#include "base/metrics/histogram_macros.h"
#include "base/time/time.h"
#include "components/subresource_filter/content/renderer/user_unverified_ruleset_dealer.h"
#include "components/subresource_filter/content/renderer/web_document_subresource_filter_impl.h"
#include "components/subresource_filter/content/shared/common/utils.h"
#include "components/subresource_filter/core/common/document_subresource_filter.h"
#include "components/subresource_filter/core/common/memory_mapped_ruleset.h"
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

namespace {

bool IsFencedFrameRoot(content::RenderFrame* frame) {
  // Unit tests may have a nullptr render_frame.
  if (!frame) {
    return false;
  }
  return frame->IsInFencedFrameTree() && frame->IsMainFrame();
}

}  // namespace

namespace subresource_filter {

UserSubresourceFilterAgent::UserSubresourceFilterAgent(
    content::RenderFrame* render_frame,
    UserUnverifiedRulesetDealer* ruleset_dealer)
    : content::RenderFrameObserver(render_frame),
      content::RenderFrameObserverTracker<UserSubresourceFilterAgent>(
          render_frame),
      ruleset_dealer_(ruleset_dealer) {
  DCHECK(ruleset_dealer);
}

void UserSubresourceFilterAgent::Initialize() {
  const GURL& url = GetDocumentURL();
  // The initial empty document will always inherit activation.
  DCHECK(ShouldInheritActivation(url));

  // We must check for provisional here because in that case 2 RenderFrames will
  // be created for the same FrameTreeNode in the browser. The browser service
  // only expects us to call SendFrameWasCreatedByAdScript() and
  // SendFrameIsAd() a single time each for a newly created RenderFrame,
  // so we must choose one. A provisional frame is created when a navigation is
  // performed cross-site and the navigation is done there to isolate it from
  // the previous frame tree. We choose to send this message from the initial
  // (non-provisional) "about:blank" frame that is created before the navigation
  // to match previous behaviour, and because this frame will always exist.
  // Whereas the provisional frame would only be created to perform the
  // navigation conditionally, so we ignore sending the IPC there.
  if (IsSubresourceFilterChild() && !IsFencedFrameRoot(render_frame()) &&
      !IsProvisional()) {
    // Note: We intentionally exclude fenced-frame roots here since they do not
    // create a RenderFrame in the creating renderer. By the time the fenced
    // frame initializes its RenderFrame in a new process,
    // IsFrameCreatedByAdScript will not see the creating call stack. Fenced
    // frames compute and send this information to the browser from
    // DidCreateFencedFrame which is called by the creating RenderFrame.
    // Additionally, there's no need to set evidence for the initial empty
    // subframe since the fenced frame is isolated from its embedder.
    if (IsFrameCreatedByAdScript()) {
      SendFrameWasCreatedByAdScript();
    }

    // As this is the initial empty document, we won't have received any message
    // from the browser and so we must populate the ad evidence here.
    SetAdEvidenceForInitialEmptySubframe();
  }

  // `render_frame()` can be null in unit tests.
  if (render_frame()) {
    render_frame()
        ->GetAssociatedInterfaceRegistry()
        ->AddInterface<mojom::UserSubresourceFilterAgent>(base::BindRepeating(
            &UserSubresourceFilterAgent::OnSubresourceFilterAgentRequest,
            base::Unretained(this)));

    if (!IsSubresourceFilterChild()) {
      // If a root frame has an activated opener, we will activate the
      // subresource filter for the initial empty document, which was created
      // before the constructor for `this`. This ensures that a popup's final
      // document is appropriately activated, even when the the initial
      // navigation is aborted and there are no further documents created.
      // TODO(dcheng): Navigation is an asynchronous operation, and the opener
      // frame may have been destroyed between the time the window is opened
      // and the RenderFrame in the window is constructed leading us to here.
      // To avoid that race condition the activation state would need to be
      // determined without the use of the opener frame.
      if (GetInheritedActivationState(render_frame()).activation_level !=
          mojom::ActivationLevel::kDisabled) {
        ConstructFilter(GetInheritedActivationStateForNewDocument(), url);
      }
    } else {
      // Child frames always have a parent, so the empty initial document can
      // always inherit activation.
      ConstructFilter(GetInheritedActivationStateForNewDocument(), url);
    }
  }
}

UserSubresourceFilterAgent::~UserSubresourceFilterAgent() = default;

GURL UserSubresourceFilterAgent::GetDocumentURL() {
  return render_frame()->GetWebFrame()->GetDocument().Url();
}

bool UserSubresourceFilterAgent::IsSubresourceFilterChild() {
  return !render_frame()->IsMainFrame() ||
         render_frame()->IsInFencedFrameTree();
}

bool UserSubresourceFilterAgent::IsParentAdFrame() {
  // A fenced frame root should never ask this since it can't see the outer
  // frame tree. Its AdEvidence is always computed by the browser.
  DCHECK(!IsFencedFrameRoot(render_frame()));
  return render_frame()->GetWebFrame()->Parent()->IsAdFrame();
}

bool UserSubresourceFilterAgent::IsProvisional() {
  return render_frame()->GetWebFrame()->IsProvisional();
}

bool UserSubresourceFilterAgent::IsFrameCreatedByAdScript() {
  DCHECK(!IsFencedFrameRoot(render_frame()));
  return render_frame()->GetWebFrame()->IsFrameCreatedByAdScript();
}

void UserSubresourceFilterAgent::SetUserSubresourceFilterForCurrentDocument(
    std::unique_ptr<blink::WebDocumentSubresourceFilter> filter) {
  blink::WebLocalFrame* web_frame = render_frame()->GetWebFrame();
  DCHECK(web_frame->GetDocumentLoader());
  web_frame->GetDocumentLoader()->SetWebUserSubresourceFilter(filter.release());
}

void UserSubresourceFilterAgent::
    SignalFirstSubresourceDisallowedForCurrentDocument() {
  GetSubresourceFilterHost()->DidDisallowFirstUserSubresource();
}

void UserSubresourceFilterAgent::SendDocumentLoadStatistics(
    const mojom::DocumentLoadStatistics& statistics) {
  GetSubresourceFilterHost()->UserSetDocumentLoadStatistics(statistics.Clone());
}

void UserSubresourceFilterAgent::SendFrameIsAd() {
  GetSubresourceFilterHost()->FrameIsUserAd();
}

void UserSubresourceFilterAgent::SendFrameWasCreatedByAdScript() {
  DCHECK(!IsFencedFrameRoot(render_frame()));
  GetSubresourceFilterHost()->FrameWasCreatedByUserAdScript();
}

bool UserSubresourceFilterAgent::IsAdFrame() {
  return render_frame()->GetWebFrame()->IsAdFrame();
}

void UserSubresourceFilterAgent::SetAdEvidence(
    const blink::FrameAdEvidence& ad_evidence) {
  render_frame()->GetWebFrame()->SetAdEvidence(ad_evidence);
}

const std::optional<blink::FrameAdEvidence>&
UserSubresourceFilterAgent::AdEvidence() {
  return render_frame()->GetWebFrame()->AdEvidence();
}

// static
mojom::ActivationState UserSubresourceFilterAgent::GetInheritedActivationState(
    content::RenderFrame* render_frame) {
  if (!render_frame) {
    return mojom::ActivationState();
  }

  // A fenced frame is isolated from its outer embedder so we cannot inspect
  // the parent's activation state. However, that's ok because the embedder
  // cannot script the fenced frame so we can wait until a navigation to set
  // activation state.
  if (IsFencedFrameRoot(render_frame)) {
    return mojom::ActivationState();
  }

  blink::WebFrame* frame_to_inherit_from =
      render_frame->IsMainFrame() ? render_frame->GetWebFrame()->Opener()
                                  : render_frame->GetWebFrame()->Parent();

  if (!frame_to_inherit_from || !frame_to_inherit_from->IsWebLocalFrame()) {
    return mojom::ActivationState();
  }

  blink::WebSecurityOrigin render_frame_origin =
      render_frame->GetWebFrame()->GetSecurityOrigin();
  blink::WebSecurityOrigin inherited_origin =
      frame_to_inherit_from->GetSecurityOrigin();

  // Only inherit from same-origin frames.
  if (render_frame_origin.IsSameOriginWith(inherited_origin)) {
    auto* agent =
        UserSubresourceFilterAgent::Get(content::RenderFrame::FromWebFrame(
            frame_to_inherit_from->ToWebLocalFrame()));
    if (agent && agent->filter_for_last_created_document_) {
      return agent->filter_for_last_created_document_->activation_state();
    }
  }

  return mojom::ActivationState();
}

void UserSubresourceFilterAgent::RecordHistogramsOnFilterCreation(
    const mojom::ActivationState& activation_state) {
  // Note: mojom::ActivationLevel used to be called mojom::ActivationState, the
  // legacy name is kept for the histogram.
  mojom::ActivationLevel activation_level = activation_state.activation_level;
  UMA_HISTOGRAM_ENUMERATION("SubresourceFilter.DocumentLoad.ActivationState",
                            activation_level);

  if (!IsSubresourceFilterChild()) {
    UMA_HISTOGRAM_BOOLEAN(
        "SubresourceFilter.MainFrameLoad.RulesetIsAvailableAnyActivationLevel",
        ruleset_dealer_->IsRulesetFileAvailable());
  }
  if (activation_level != mojom::ActivationLevel::kDisabled) {
    UMA_HISTOGRAM_BOOLEAN("SubresourceFilter.DocumentLoad.RulesetIsAvailable",
                          ruleset_dealer_->IsRulesetFileAvailable());
  }
}

void UserSubresourceFilterAgent::ResetInfoForNextDocument() {
  activation_state_for_next_document_ = mojom::ActivationState();
}

mojom::UserSubresourceFilterHost*
UserSubresourceFilterAgent::GetSubresourceFilterHost() {
  if (!subresource_filter_host_) {
    render_frame()->GetRemoteAssociatedInterfaces()->GetInterface(
        &subresource_filter_host_);
  }
  return subresource_filter_host_.get();
}

void UserSubresourceFilterAgent::OnSubresourceFilterAgentRequest(
    mojo::PendingAssociatedReceiver<mojom::UserSubresourceFilterAgent>
        receiver) {
  receiver_.reset();
  receiver_.Bind(std::move(receiver));
}

void UserSubresourceFilterAgent::ActivateForNextCommittedLoad(
    mojom::ActivationStatePtr activation_state,
    const std::optional<blink::FrameAdEvidence>& ad_evidence) {
  activation_state_for_next_document_ = *activation_state;

  if (IsSubresourceFilterChild()) {
    DCHECK(ad_evidence.has_value());
    SetAdEvidence(ad_evidence.value());
  } else {
    DCHECK(!ad_evidence.has_value());
  }
}

void UserSubresourceFilterAgent::OnDestruct() {
  delete this;
}

void UserSubresourceFilterAgent::SetAdEvidenceForInitialEmptySubframe() {
  DCHECK(!IsAdFrame());
  DCHECK(!AdEvidence().has_value());
  DCHECK(!IsFencedFrameRoot(render_frame()));

  blink::FrameAdEvidence ad_evidence(IsParentAdFrame());
  ad_evidence.set_created_by_ad_script(
      IsFrameCreatedByAdScript()
          ? blink::mojom::FrameCreationStackEvidence::kCreatedByAdScript
          : blink::mojom::FrameCreationStackEvidence::kNotCreatedByAdScript);
  ad_evidence.set_is_complete();
  SetAdEvidence(ad_evidence);

  if (ad_evidence.IndicatesAdFrame()) {
    SendFrameIsAd();
  }
}

void UserSubresourceFilterAgent::DidCreateNewDocument() {
#if BUILDFLAG(ARKWEB_ADBLOCK)
  did_load_finished_ = false;
#endif
  // TODO(csharrison): Use WebURL and WebSecurityOrigin for efficiency here,
  // which requires changes to the unit tests.
  const GURL& url = GetDocumentURL();

  // A new browser-side host is created for each new page (i.e. new document in
  // a subresource filter root frame) so we have to reset the remote so we
  // re-bind on the next message.
  if (!IsSubresourceFilterChild()) {
    subresource_filter_host_.reset();
  }

  const mojom::ActivationState activation_state =
      ShouldInheritActivation(url) ? GetInheritedActivationStateForNewDocument()
                                   : activation_state_for_next_document_;

  ResetInfoForNextDocument();

  // Do not pollute the histograms with uninteresting root frame documents.
  const bool should_record_histograms = IsSubresourceFilterChild() ||
                                        url.SchemeIsHTTPOrHTTPS() ||
                                        url.SchemeIsFile();
  if (should_record_histograms) {
    RecordHistogramsOnFilterCreation(activation_state);
  }

  ConstructFilter(activation_state, url);
}

const mojom::ActivationState
UserSubresourceFilterAgent::GetInheritedActivationStateForNewDocument() {
  DCHECK(ShouldInheritActivation(GetDocumentURL()));
  return GetInheritedActivationState(render_frame());
}

void UserSubresourceFilterAgent::ConstructFilter(
    const mojom::ActivationState activation_state,
    const GURL& url) {
  filter_for_last_created_document_.reset();

  if (activation_state.activation_level == mojom::ActivationLevel::kDisabled ||
      !ruleset_dealer_->IsRulesetFileAvailable()) {
    return;
  }

  scoped_refptr<const MemoryMappedRuleset> ruleset =
      ruleset_dealer_->GetRuleset();

  if (!ruleset) {
    LOG(ERROR) << "adblock ruleset nullptr";
    return;
  }

  base::OnceClosure first_disallowed_load_callback(
      base::BindOnce(&UserSubresourceFilterAgent::
                         SignalFirstSubresourceDisallowedForCurrentDocument,
                     weak_ptr_factory_.GetWeakPtr()));
  auto filter = std::make_unique<WebDocumentSubresourceFilterImpl>(
      url::Origin::Create(url), activation_state, std::move(ruleset),
      std::move(first_disallowed_load_callback));
  filter_for_last_created_document_ = filter->AsWeakPtr();
  SetUserSubresourceFilterForCurrentDocument(std::move(filter));

#if BUILDFLAG(ARKWEB_ADBLOCK)
  // This calc will only allowed after SetUserSubresourceFilterForCommittedLoad
  // which already finish
  // web_frame->GetDocumentLoader()->SetUserSubresourceFilter
  CalcElementHidingTypeOption(render_frame());
#endif
}

void UserSubresourceFilterAgent::DidFailProvisionalLoad() {
  // TODO(engedy): Add a test with `frame-ancestor` violation to exercise this.
  ResetInfoForNextDocument();
}

void UserSubresourceFilterAgent::DidFinishLoad() {
  if (!filter_for_last_created_document_) {
    return;
  }
  const auto& statistics =
      filter_for_last_created_document_->filter().statistics();

  SendDocumentLoadStatistics(statistics);

#if BUILDFLAG(ARKWEB_ADBLOCK)
  filter_for_last_created_document_->SetDidFinishLoad(true);
  filter_for_last_created_document_->ClearStatistics();
  did_load_finished_ = true;
#endif
}

void UserSubresourceFilterAgent::WillCreateWorkerFetchContext(
    blink::WebWorkerFetchContext* worker_fetch_context) {
  if (!filter_for_last_created_document_) {
    return;
  }
  if (!ruleset_dealer_->IsRulesetFileAvailable()) {
    return;
  }
  base::File ruleset_file = ruleset_dealer_->DuplicateRulesetFile();
  if (!ruleset_file.IsValid()) {
    return;
  }

  worker_fetch_context->SetUserSubresourceFilterBuilder(
      std::make_unique<WebDocumentSubresourceFilterImpl::BuilderImpl>(
          url::Origin::Create(GetDocumentURL()),
          filter_for_last_created_document_->filter().activation_state(),
          std::move(ruleset_file),
          base::BindOnce(&UserSubresourceFilterAgent::
                             SignalFirstSubresourceDisallowedForCurrentDocument,
                         weak_ptr_factory_.GetWeakPtr())));
}

void UserSubresourceFilterAgent::OnOverlayPopupAdDetected() {
  GetSubresourceFilterHost()->OnAdsViolationTriggered(
      subresource_filter::mojom::AdsViolation::kOverlayPopupAd);
}

void UserSubresourceFilterAgent::OnLargeStickyAdDetected() {
  GetSubresourceFilterHost()->OnAdsViolationTriggered(
      subresource_filter::mojom::AdsViolation::kLargeStickyAd);
}

void UserSubresourceFilterAgent::DidCreateFencedFrame(
    const blink::RemoteFrameToken& placeholder_token) {
  if (render_frame()->GetWebFrame()->IsAdScriptInStack()) {
    GetSubresourceFilterHost()->UserAdScriptDidCreateFencedFrame(
        placeholder_token);
  }
}

#if BUILDFLAG(ARKWEB_ADBLOCK)
void UserSubresourceFilterAgent::SendStatisticsAfterDocumentLoad(
    const mojom::DocumentLoadStatistics& statistics) {
  GetSubresourceFilterHost()->UserSetStatisticsAfterDocumentLoad(
      statistics.Clone());
}

void UserSubresourceFilterAgent::DidSubresourceFiltered() {
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

void UserSubresourceFilterAgent::CalcElementHidingTypeOption(
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
      web_local_frame->GetDocumentLoader()->GetWebUserSubresourceFilter();
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
