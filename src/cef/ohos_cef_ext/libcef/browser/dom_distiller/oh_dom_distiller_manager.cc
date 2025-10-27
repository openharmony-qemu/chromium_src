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

#include "libcef/browser/dom_distiller/oh_dom_distiller_manager.h"

#include <utility>

#include "base/strings/utf_string_conversions.h"
#include "base/uuid.h"
#include "components/back_forward_cache/back_forward_cache_disable.h"
#include "components/dom_distiller/content/browser/distiller_page_web_contents.h"
#include "components/dom_distiller/core/distiller_page.h"
#include "components/dom_distiller/core/url_constants.h"
#include "components/dom_distiller/core/url_utils.h"
#include "content/public/browser/back_forward_cache.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/navigation_controller.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_observer.h"
#include "libcef/browser/dom_distiller/oh_self_deleting_request_delegate.h"
#include "libcef/browser/dom_distiller/oh_dom_distiller_service_factory.h"
#include "base/logging.h"

namespace oh_dom_distiller {

namespace {

// Start loading the viewer URL of the current page in |web_contents|.
void StartNavigationToDistillerViewer(content::WebContents* web_contents,
                                      const GURL& url) {
  GURL viewer_url = dom_distiller::url_utils::GetDistillerViewUrlFromUrl(
      dom_distiller::kDomDistillerScheme, url,
      base::UTF16ToUTF8(web_contents->GetTitle()),
      (base::TimeTicks::Now() - base::TimeTicks()).InMilliseconds());
  content::NavigationController::LoadURLParams params(viewer_url);
  params.transition_type = ui::PAGE_TRANSITION_AUTO_BOOKMARK;
  web_contents->GetController().LoadURLWithParams(params);
}

}  // namespace

// static
OhDomDistillerManager* OhDomDistillerManager::GetInstance() {
  static base::NoDestructor<OhDomDistillerManager> instance;
  return instance.get();
}

OhDomDistillerManager::OhDomDistillerManager() {}
OhDomDistillerManager::~OhDomDistillerManager() = default;

dom_distiller::DomDistillerService*
OhDomDistillerManager::GetDomDistillerService(
    content::WebContents* web_contents) {
  CHECK(web_contents);
  if (!dom_distiller_service_) {
    // Start distillation using |source_page_handle|, and ensure ViewerHandle
    // stays around until the viewer requests distillation.
    dom_distiller_service_ =
        OhDomDistillerServiceFactory::GetForBrowserContext(
            web_contents->GetBrowserContext());
  }
  return dom_distiller_service_;
}

void OhDomDistillerManager::DistillCurrentPageAndView(
    content::WebContents* old_web_contents) {
  DCHECK(old_web_contents);
}

void OhDomDistillerManager::DistillCurrentPage(
    content::WebContents* source_web_contents,
    const DistillOptions& distill_options,
    DistillResultCallback callback) {
  DCHECK(source_web_contents);
  std::unique_ptr<dom_distiller::SourcePageHandleWebContents>
      source_page_handle(new dom_distiller::SourcePageHandleWebContents(
          source_web_contents, false));

  const GURL& distill_url = GURL(distill_options.distill_url);
  if (!dom_distiller::url_utils::IsUrlDistillable(distill_url)) {
    LOG(INFO) << __func__ << " [Distiller] DistillResult, url cannot distillable";
    std::move(callback).Run(
        "{\"resultCode\": -50, \"resultMessage\": \"url is invalid or not "
        "http[s].\"}");
    return;
  }

  std::string guid = base::Uuid::GenerateRandomV4().AsLowercaseString();
  LOG(INFO) << __func__ << " [Distiller] option:" << static_cast<int32_t>(distill_options.distill_type) << " "
            << static_cast<int32_t>(distill_options.fetch_action) << " "
            << distill_options.max_distill_pages;

  // Disable back-forward cache when the distillation is in progress as it would
  // be cancelled and would not be restarted when the page is restored from the
  // cache.
  content::BackForwardCache::DisableForRenderFrameHost(
      source_page_handle->web_contents()->GetPrimaryMainFrame(),
      back_forward_cache::DisabledReason(
          back_forward_cache::DisabledReasonId::
              kDomDistiller_SelfDeletingRequestDelegate));

  // Start distillation using |source_page_handle|, and ensure ViewerHandle
  // stays around until the viewer requests distillation.
  auto* dom_distiller_service = GetDomDistillerService(source_web_contents);
  if (dom_distiller_service == nullptr) {
    LOG(ERROR) << "[Distiller] dom_distiller_service is nullptr";
    return;
  }
  std::unique_ptr<dom_distiller::DistillerPage> distiller_page =
      dom_distiller_service->CreateDefaultDistillerPageWithHandle(
          std::move(source_page_handle));
  if (distiller_page == nullptr) {
    LOG(ERROR) << "[Distiller] distiller_page is nullptr";
    return;
  }
  distiller_page->SetDistillOptions(distill_options);

  auto* view_request_delegate =
      new OhSelfDeletingRequestDelegate(std::move(callback), distill_url, guid);

  std::unique_ptr<dom_distiller::ViewerHandle> viewer_handle =
      dom_distiller_service->ViewUrl(view_request_delegate,
                                     std::move(distiller_page), distill_url);
  view_request_delegate->TakeViewerHandle(std::move(viewer_handle));
}

void OhDomDistillerManager::DistillAndView(
    content::WebContents* source_web_contents,
    const DistillOptions& distill_options,
    content::WebContents* destination_web_contents) {
  DCHECK(destination_web_contents);

  DistillCurrentPage(source_web_contents, distill_options, base::DoNothing());

  StartNavigationToDistillerViewer(destination_web_contents,
                                   source_web_contents->GetLastCommittedURL());
}

void OhDomDistillerManager::AbortDistill(
    content::WebContents* source_web_contents) {
  DCHECK(source_web_contents);
  if (auto* dom_distiller_service = GetDomDistillerService(source_web_contents)) {
    if (!dom_distiller_service->utils()) {
      LOG(ERROR) << "[Distiller] dom_distiller_service_utils is nullptr";
      return;
    }
    dom_distiller_service->utils()->AbortDistill();
  }
}

} // namespace oh_dom_distiller