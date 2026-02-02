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

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "base/test/task_environment.h"
#include "content/public/test/test_renderer_host.h"
#include "content/test/test_web_contents.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/receiver.h"
#include "mojo/core/embedder/embedder.h"
#include "arkweb/chromium_ext/third_party/blink/public/mojom/media/video_experience_reporter.mojom.h"
#include "url/gurl.h"
#include "content/public/test/browser_test.h"
#include "content/public/test/browser_test_utils.h"
#include "content/public/test/browser_task_environment.h"
#include "chrome/test/base/platform_browser_test.h"
#include "chrome/test/base/chrome_test_utils.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"

#define private public
#include "ohos_video_experience_reporter.h"
#undef private

namespace content {

class MockVideoExperienceReporter : public blink::mojom::VideoExperienceReporter {
public:
    MockVideoExperienceReporter() = default;
    ~MockVideoExperienceReporter() override = default;
    MOCK_METHOD(void, ReportVideoExperienceToBI, (blink::mojom::VideoExpParamsPtr params), (override));
};

class OHOSVideoExperienceReporterTest : public PlatformBrowserTest {
public:
    OHOSVideoExperienceReporterTest() {
        mojo::core::Init();
    }

    ~OHOSVideoExperienceReporterTest() override = default;

    void SetUpOnMainThread() override {
        PlatformBrowserTest::SetUpOnMainThread();
    }

    void SetUp() override {
        PlatformBrowserTest::SetUp();
    }

    void TearDown() override {
        PlatformBrowserTest::TearDown();
    }

protected:
    void Initialize() {
        content::WebContents* web_contents = chrome_test_utils::GetActiveWebContents(this);
        ASSERT_NE(web_contents, nullptr);
        web_contents_ = static_cast<WebContentsImpl*>(web_contents);
        rfh_ = web_contents_->GetPrimaryMainFrame();
        ASSERT_NE(rfh_, nullptr);
        reporter_ = std::make_unique<OHOSVideoExperienceReporter>(rfh_);
    }

    void Destroy() {
        reporter_.reset();
    }

    WebContentsImpl* web_contents_ = nullptr;
    content::RenderFrameHost* rfh_;
    std::unique_ptr<OHOSVideoExperienceReporter> reporter_;
};

IN_PROC_BROWSER_TEST_F(OHOSVideoExperienceReporterTest, DFX_TestCreateForFrameHost) {
  content::WebContents* web_contents = chrome_test_utils::GetActiveWebContents(this);
  ASSERT_NE(web_contents, nullptr);
  web_contents_ = static_cast<WebContentsImpl*>(web_contents);
  rfh_ = web_contents_->GetPrimaryMainFrame();
  ASSERT_NE(rfh_, nullptr);
  mojo::PendingReceiver<blink::mojom::VideoExperienceReporter> receiver;
  OHOSVideoExperienceReporter::CreateForFrameHost(rfh_, std::move(receiver));
  EXPECT_TRUE(receiver.is_valid());
}

IN_PROC_BROWSER_TEST_F(OHOSVideoExperienceReporterTest, DFX_TestReportVideoExperienceToBI) {
    Initialize();
    mojo::PendingReceiver<blink::mojom::VideoExperienceReporter> pending_receiver;
    reporter_->BindChannel(std::move(pending_receiver));

    auto params = blink::mojom::VideoExpParams::New();
    params->start_used_time = 100;
    params->total_freeze_time = 50;
    params->total_played_time = 200;
    params->video_freeze_cnt = 2;
    params->error_code = 0;
    params->error_msg = "No error";
    params->page_url = "http://example.com";
    params->video_src = GURL("http://example.com/video.mp4");
    params->pipeline_status = "playing";
    params->mime_type = "video/mp4";
    params->net_error_code = 0;
    params->video_duration = 300;
    params->video_width = 1280;
    params->video_height = 720;
    params->video_player = "MediaCodec";

    EXPECT_EQ(params->video_duration, 300);
    reporter_->ReportVideoExperienceToBI(std::move(params));
    Destroy();
}

IN_PROC_BROWSER_TEST_F(OHOSVideoExperienceReporterTest, DFX_TestWebContentsDestroyed) {
    Initialize();
    reporter_->WebContentsDestroyed();
    EXPECT_EQ(reporter_->web_contents_, nullptr);
    Destroy();
}

} // namespace content