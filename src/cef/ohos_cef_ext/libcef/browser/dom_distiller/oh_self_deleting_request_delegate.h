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

#ifndef CEF_OHOS_CEF_EXT_LIBCEF_BROWSER_DOM_DISTILLER_OH_SELF_DELETING_REQUEST_DELEGATE_H_
#define CEF_OHOS_CEF_EXT_LIBCEF_BROWSER_DOM_DISTILLER_OH_SELF_DELETING_REQUEST_DELEGATE_H_

#include "components/dom_distiller/core/task_tracker.h"
#include "content/public/browser/web_contents_observer.h"
#include "url/gurl.h"

namespace content {
class WebContents;
}  // namespace content

namespace oh_dom_distiller {

using dom_distiller::ArticleDistillationUpdate;
using dom_distiller::DistilledArticleProto;
using dom_distiller::ViewerHandle;
using dom_distiller::ViewRequestDelegate;

using DistillResultCallback =
    base::OnceCallback<void(const std::string& content)>;

// An no-op ViewRequestDelegate which holds a ViewerHandle and deletes itself
// after the WebContents navigates or goes away. This class is a band-aid to
// keep a TaskTracker around until the distillation starts from the viewer.
class OhSelfDeletingRequestDelegate : public ViewRequestDelegate {
 public:
  explicit OhSelfDeletingRequestDelegate(DistillResultCallback callback,
                                         GURL url,
                                         std::string guid);
  ~OhSelfDeletingRequestDelegate() override;

  // ViewRequestDelegate implementation.
  void OnArticleReady(const DistilledArticleProto* article_proto) override;
  void OnArticleUpdated(ArticleDistillationUpdate article_update) override;
  void OnArticleAborted() override;

  // Takes ownership of the ViewerHandle to keep distillation alive until |this|
  // is deleted.
  void TakeViewerHandle(std::unique_ptr<ViewerHandle> viewer_handle);

  const GURL& GetRequestURL() { return request_url_; }

 private:
  // The handle to the view request towards the DomDistillerService. It
  // needs to be kept around to ensure the distillation request finishes.
  std::unique_ptr<ViewerHandle> viewer_handle_;
  DistillResultCallback distill_result_callback_;
  GURL request_url_;
  std::string guid_;
};

} // namespace oh_dom_distiller

#endif // CEF_OHOS_CEF_EXT_LIBCEF_BROWSER_DOM_DISTILLER_OH_SELF_DELETING_REQUEST_DELEGATE_H_