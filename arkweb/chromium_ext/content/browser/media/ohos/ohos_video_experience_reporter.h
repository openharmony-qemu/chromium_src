/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef CONTENT_BROWSER_MEDIA_OHOS_VIDEO_EXPERIENCE_REPORTER_H_
#define CONTENT_BROWSER_MEDIA_OHOS_VIDEO_EXPERIENCE_REPORTER_H_

#include "content/public/browser/web_contents_observer.h"
#include "arkweb/chromium_ext/third_party/blink/public/mojom/media/video_experience_reporter.mojom.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/associated_receiver.h"

namespace content {
class RenderFrameHost;

class OHOSVideoExperienceReporter
    : public blink::mojom::VideoExperienceReporter
    , public content::WebContentsObserver {
public:
  static void CreateForFrameHost(content::RenderFrameHost* rfh,
    mojo::PendingReceiver<blink::mojom::VideoExperienceReporter> receiver);
  OHOSVideoExperienceReporter(const OHOSVideoExperienceReporter&) = delete;
  OHOSVideoExperienceReporter& operator=(const OHOSVideoExperienceReporter&) = delete;

  ~OHOSVideoExperienceReporter() override;

  void ReportVideoExperienceToBI(
      blink::mojom::VideoExpParamsPtr params) override;

  void BindChannel(mojo::PendingReceiver<blink::mojom::VideoExperienceReporter>
                       pendingReceive);
private:
  friend class OHOSVideoExperienceReporterTest;

  OHOSVideoExperienceReporter(content::RenderFrameHost* rfh);
 
  // content::WebContentsObserver implements
  void WebContentsDestroyed() override;
  
  mojo::Receiver<blink::mojom::VideoExperienceReporter> receiver_{this};

  raw_ptr<content::WebContents> web_contents_;

};

} // namespace content

#endif // CONTENT_BROWSER_MEDIA_OHOS_VIDEO_EXPERIENCE_REPORTER_H_