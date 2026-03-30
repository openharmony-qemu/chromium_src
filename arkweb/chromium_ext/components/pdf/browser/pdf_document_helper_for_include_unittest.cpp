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

#include "components/pdf/browser/pdf_document_helper.h"
#include "components/pdf/browser/pdf_document_helper_client.h"
#include "content/public/test/test_renderer_host.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "base/test/scoped_logging_settings.h"

namespace pdf {
namespace {
using namespace testing;

class MockPDFDocumentHelperClient : public PDFDocumentHelperClient {
public:
    MOCK_METHOD(void,
                UpdateContentRestrictions,
                (content::RenderFrameHost* render_frame_host,
                 int content_restrictions),
                (override));

    MOCK_METHOD(void,
                OnPDFHasUnsupportedFeature,
                (content::WebContents* contents),
                (override));

    MOCK_METHOD(void,
                OnSaveURL,
                (content::WebContents* contents),
                (override));

    MOCK_METHOD(void,
                SetPluginCanSave,
                (content::RenderFrameHost* render_frame_host,
                bool can_save),
                (override));

    MOCK_METHOD(void,
                OnDidScroll,
                (const gfx::SelectionBound& start,
                const gfx::SelectionBound& end),
                (override));

    MOCK_METHOD(void,
                OnSearchifyStateChange,
                (bool busy,
                content::WebContents* contents),
                (override));
};

class PDFDocumentHelperTest : public content::RenderViewHostTestHarness {
protected:
    void SetUp() override;
    void TearDown() override;
    std::unique_ptr<logging::ScopedLoggingSettings> logging_settings_;
    static std::string log_buffer_;

    // 日志处理器：把日志写入字符串缓冲区
    static bool LogHandler(
                           logging::LogSeverity severity,
                           const char* file,
                           int line,
                           size_t message_start,
                           const std::string& str) {
                              log_buffer_ += str + "\n";
                              return true;
                           }
};

std::string PDFDocumentHelperTest::log_buffer_ = "";

void PDFDocumentHelperTest::SetUp() {
    content::RenderViewHostTestHarness::SetUp();
    // 保存当前日志设置，测试结束后自动恢复
    logging_settings_ = std::make_unique<logging::ScopedLoggingSettings>();

    // 设置日志处理器：将所有日志输出到内存中
    logging::SetLogMessageHandler(&PDFDocumentHelperTest::LogHandler);

    // 清空旧日志
    log_buffer_.clear();
}

void PDFDocumentHelperTest::TearDown() {
    content::RenderViewHostTestHarness::TearDown();
    // 恢复原始日志设置（由ScopedLoggingSettings析构完成）
    logging_settings_.reset();

    // 恢复默认日志处理器
    logging::SetLogMessageHandler(nullptr);
}

TEST_F(PDFDocumentHelperTest, HideHandleAndQuickMenu) {
    content::BrowserContext* ptr = GetBrowserContext();
    ASSERT_NE(ptr, nullptr);

    content::RenderFrameHostTester* tester =
    content::RenderFrameHostTester::For(main_rfh());
    ASSERT_NE(tester, nullptr);
    tester->InitializeRenderFrameIfNeeded();
    content::RenderFrameHost* subframe1 = tester->AppendChild("subframe1");

    //create pdf_helper
    std::unique_ptr<PDFDocumentHelperClient> mock_client_ = std::make_unique<MockPDFDocumentHelperClient>();
    PDFDocumentHelper::CreateForCurrentDocument(subframe1, std::move(mock_client_));
    auto helper = PDFDocumentHelper::GetForCurrentDocument(subframe1);

    helper->HideHandleAndQuickMenu(true);
    EXPECT_THAT(log_buffer_, testing::HasSubstr("PDF touch_selection_controller_client_manager_ is null"));
}

TEST_F(PDFDocumentHelperTest, ResetResponsePendingInputEvent) {
    EXPECT_TRUE(log_buffer_.empty());
    content::RenderFrameHostTester* tester =
        content::RenderFrameHostTester::For(main_rfh());
    ASSERT_NE(tester, nullptr);
    tester->InitializeRenderFrameIfNeeded();
    content::RenderFrameHost* subframe1 = tester->AppendChild("subframe1");

    //create pdf_helper
    std::unique_ptr<PDFDocumentHelperClient> mock_client_ = std::make_unique<MockPDFDocumentHelperClient>();
    PDFDocumentHelper::CreateForCurrentDocument(subframe1, std::move(mock_client_));
    auto helper = PDFDocumentHelper::GetForCurrentDocument(subframe1);

    helper->ResetResponsePendingInputEvent();
    EXPECT_THAT(log_buffer_, testing::HasSubstr("PDF touch_selection_controller_client_manager_ is null"));
}
}
}