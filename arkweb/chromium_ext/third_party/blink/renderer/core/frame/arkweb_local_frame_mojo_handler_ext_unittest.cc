// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "third_party/blink/renderer/core/frame/arkweb_local_frame_mojo_handler_ext.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "base/files/file.h"
#include "base/files/scoped_temp_dir.h"
#include "base/test/scoped_feature_list.h"
#include "mojo/public/cpp/system/handle.h"
#include "mojo/public/cpp/system/platform_handle.h"
#include "third_party/blink/public/common/features.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/frame_test_helpers.h"
#include "third_party/blink/renderer/core/testing/dummy_page_holder.h"
#include "third_party/blink/renderer/core/testing/core_unit_test_helper.h"
#include "third_party/blink/renderer/platform/heap/persistent.h"
#include "third_party/blink/renderer/platform/testing/testing_platform_support.h"


namespace blink {

using ::testing::_;

class MockCallbackHelper {
public:
    MOCK_METHOD(void, JavaScriptCallback, (::base::Value result));
    MOCK_METHOD(void, GenerateCodeCacheCallback, (int32_t status));
    MOCK_METHOD(void, GetImageCallback, 
                (uint32_t id, ::base::ReadOnlySharedMemoryRegion region));
};

class ArkWebLocalFrameMojoHandlerExtTest : public RenderingTest {
protected:
    void SetUp() override {
        EnableCompositing();
        RenderingTest::SetUp();
        web_view_helper_.Initialize();
        dummy_page_holder_ = std::make_unique<DummyPageHolder>();
        local_frame_ = &dummy_page_holder_->GetFrame();
        handler_ = MakeGarbageCollected<ArkWebLocalFrameMojoHandlerExt>(*local_frame_);
        mock_callback_ = std::make_unique<MockCallbackHelper>();
    }

    mojo::ScopedHandle CreateMockHandle(const std::string& script_content) {

        base::ScopedTempDir temp_dir_;
        base::FilePath file_path = temp_dir_.GetPath().AppendASCII("script.js");
        base::File file(file_path, base::File::FLAG_CREATE | base::File::FLAG_WRITE);
        if (!file.IsValid()) return mojo::ScopedHandle();

        file.WriteAtCurrentPos(script_content.data(), script_content.size());
        file.Close();

        base::File read_file(file_path, base::File::FLAG_OPEN | base::File::FLAG_READ);
        if (!read_file.IsValid()) return mojo::ScopedHandle();

        base::ScopedPlatformFile scoped_handle(read_file.TakePlatformFile());

        mojo::ScopedHandle script_handle =
                            mojo::WrapPlatformFile(std::move(scoped_handle));

        return script_handle;
    }

    void JavaScriptExecuteRequestExtTest(
                mojo::ScopedHandle handle_fd,
                const uint64_t scriptLength,
                bool wants_result,
                LocalFrameMojoHandler::JavaScriptExecuteRequestCallback callback)
    {
        handler_->JavaScriptExecuteRequestExt(std::move(handle_fd), scriptLength,
                                        wants_result, std::move(callback));
        return;
    }

    void GenerateCodeCacheTest(
                const WTF::String& url,
                const WTF::String& script,
                mojom::blink::CacheOptionsPtr cache_options,
                LocalFrameMojoHandler::GenerateCodeCacheCallback callback)
    {
        handler_->GenerateCodeCache(url, script,
                            std::move(cache_options), std::move(callback));
        return;
    }

    void GetImageFromCacheTest(
                const WTF::String& url,
                LocalFrameMojoHandler::GetImageFromCacheCallback callback)
    {
        handler_->GetImageFromCache(url, std::move(callback));
        return;
    }

    void DetectBlankScreenTest(
                const WTF::String& url,
                const WTF::Vector<double>& detectionTiming,
                const WTF::Vector<int32_t>& detectionMethods,
                int32_t contentfulNodesCountThreshold)
    {
        handler_->DetectBlankScreen(url, detectionTiming, detectionMethods,
                                    contentfulNodesCountThreshold);
        return;
    }

    void TearDown() override {
        dummy_page_holder_.reset();
    }

    std::unique_ptr<DummyPageHolder> dummy_page_holder_;
    Persistent<LocalFrame> local_frame_;
    Persistent<ArkWebLocalFrameMojoHandlerExt> handler_;
    std::unique_ptr<MockCallbackHelper> mock_callback_;
    frame_test_helpers::WebViewHelper web_view_helper_;
};

TEST_F(ArkWebLocalFrameMojoHandlerExtTest, GetImageFromCache_ValidUrl) {

    auto callback = base::BindOnce(&MockCallbackHelper::GetImageCallback,
                                    base::Unretained(mock_callback_.get()));

    EXPECT_CALL(*mock_callback_, GetImageCallback(
        testing::Eq(0u),
        testing::A<base::ReadOnlySharedMemoryRegion>()
    )).Times(1);

    GetImageFromCacheTest(
        "https://example.com/image.png",
        std::move(callback));
}

TEST_F(ArkWebLocalFrameMojoHandlerExtTest, GetImageFromCache_EmptyUrl) {
    auto callback = base::BindOnce(&MockCallbackHelper::GetImageCallback,
                                    base::Unretained(mock_callback_.get()));

    EXPECT_CALL(*mock_callback_, GetImageCallback(
        testing::Eq(0u),
        testing::A<base::ReadOnlySharedMemoryRegion>()
    )).Times(1);

    GetImageFromCacheTest(
        "",
        std::move(callback));
}

TEST_F(ArkWebLocalFrameMojoHandlerExtTest, GetImageFromCache_InvalidUrl) {
    auto callback = base::BindOnce(&MockCallbackHelper::GetImageCallback,
                                    base::Unretained(mock_callback_.get()));

    EXPECT_CALL(*mock_callback_, GetImageCallback(
        testing::Eq(0u),
        testing::A<base::ReadOnlySharedMemoryRegion>()
    )).Times(1);

    GetImageFromCacheTest(
        "not-a-valid-url",
        std::move(callback));
}

TEST_F(ArkWebLocalFrameMojoHandlerExtTest, GetImageFromCache_ImageFormats) {
    auto callback = base::BindOnce(&MockCallbackHelper::GetImageCallback,
                                    base::Unretained(mock_callback_.get()));

    EXPECT_CALL(*mock_callback_, GetImageCallback(
        testing::_,
        testing::A<base::ReadOnlySharedMemoryRegion>()
    )).Times(1);

    GetImageFromCacheTest(
        "https://example.com/test.jpg",
        std::move(callback));
}

TEST_F(ArkWebLocalFrameMojoHandlerExtTest, GetImageFromCache_PngImage) {
    auto callback = base::BindOnce(&MockCallbackHelper::GetImageCallback,
                                    base::Unretained(mock_callback_.get()));

    EXPECT_CALL(*mock_callback_, GetImageCallback(
        testing::_,
        testing::A<base::ReadOnlySharedMemoryRegion>()
    )).Times(1);

    GetImageFromCacheTest(
        "https://example.com/image.png?cache=true",
        std::move(callback));
}

TEST_F(ArkWebLocalFrameMojoHandlerExtTest, GenerateCodeCache_Success) {
    auto cache_options = mojom::blink::CacheOptions::New();
    EXPECT_CALL(*mock_callback_, GenerateCodeCacheCallback(testing::Eq(0)))
        .Times(1);

    auto callback = base::BindOnce(&MockCallbackHelper::GenerateCodeCacheCallback,
                                    base::Unretained(mock_callback_.get()));

    GenerateCodeCacheTest(
        "https://example.com/script.js",
        "console.log('Hello World');",
        std::move(cache_options),
        std::move(callback));
}

TEST_F(ArkWebLocalFrameMojoHandlerExtTest, GenerateCodeCache_EmptyScript) {
    auto cache_options = mojom::blink::CacheOptions::New();
    EXPECT_CALL(*mock_callback_, GenerateCodeCacheCallback(testing::Eq(0)))
        .Times(1);

    auto callback = base::BindOnce(&MockCallbackHelper::GenerateCodeCacheCallback,
                                    base::Unretained(mock_callback_.get()));

    GenerateCodeCacheTest(
        "https://example.com/script.js",
        "",
        std::move(cache_options),
        std::move(callback));
}

TEST_F(ArkWebLocalFrameMojoHandlerExtTest, GenerateCodeCache_EmptyUrl) {
    auto cache_options = mojom::blink::CacheOptions::New();
    EXPECT_CALL(*mock_callback_, GenerateCodeCacheCallback(testing::Eq(0)))
        .Times(1);

    auto callback = base::BindOnce(&MockCallbackHelper::GenerateCodeCacheCallback,
                                    base::Unretained(mock_callback_.get()));

    GenerateCodeCacheTest(
        "",
        "console.log('Hello World');",
        std::move(cache_options),
        std::move(callback));
}

TEST_F(ArkWebLocalFrameMojoHandlerExtTest, GenerateCodeCache_ModuleScript) {
    auto cache_options = mojom::blink::CacheOptions::New();
    cache_options->is_module = true;
    cache_options->is_top_level = true;

    EXPECT_CALL(*mock_callback_, GenerateCodeCacheCallback(testing::_))
        .Times(1);

    auto callback = base::BindOnce(&MockCallbackHelper::GenerateCodeCacheCallback,
                                    base::Unretained(mock_callback_.get()));

    GenerateCodeCacheTest(
        "https://example.com/module.js",
        "export function test() { return 42; }",
        std::move(cache_options),
        std::move(callback));
}

TEST_F(ArkWebLocalFrameMojoHandlerExtTest, GenerateCodeCache_ComplexScript) {
    auto cache_options = mojom::blink::CacheOptions::New();
    cache_options->is_module = false;
    cache_options->is_top_level = false;

    EXPECT_CALL(*mock_callback_, GenerateCodeCacheCallback(testing::_))
        .Times(1);

    auto callback = base::BindOnce(&MockCallbackHelper::GenerateCodeCacheCallback,
                                    base::Unretained(mock_callback_.get()));

    GenerateCodeCacheTest(
        "https://example.com/complex.js",
        "function outer() { function inner() { return 1 + 1; } return inner(); }",
        std::move(cache_options),
        std::move(callback));
}

TEST_F(ArkWebLocalFrameMojoHandlerExtTest, GenerateCodeCache_WithResponseHeaders) {
    auto cache_options = mojom::blink::CacheOptions::New();
    cache_options->response_headers.insert("Content-Type", "application/javascript");

    EXPECT_CALL(*mock_callback_, GenerateCodeCacheCallback(testing::_))
        .Times(1);

    auto callback = base::BindOnce(&MockCallbackHelper::GenerateCodeCacheCallback,
                                    base::Unretained(mock_callback_.get()));

    GenerateCodeCacheTest(
        "https://example.com/script.js",
        "var x = 10;",
        std::move(cache_options),
        std::move(callback));
}

TEST_F(ArkWebLocalFrameMojoHandlerExtTest, JavaScriptExecuteRequestExt_Success) {
    const std::string kTestScript = "1 + 2;";
    const uint64_t kScriptLength = kTestScript.size();

    mojo::ScopedHandle script_handle = CreateMockHandle(kTestScript);
    auto callback = base::BindOnce(&MockCallbackHelper::JavaScriptCallback,
                                base::Unretained(mock_callback_.get()));

    JavaScriptExecuteRequestExtTest(
        std::move(script_handle), 
        kScriptLength, 
        true,
        std::move(callback));

    base::RunLoop().RunUntilIdle();
}

TEST_F(ArkWebLocalFrameMojoHandlerExtTest, JavaScriptExecuteRequestExt_NoResult) {
    const std::string kTestScript = "console.log('Hello');";
    const uint64_t kScriptLength = kTestScript.size();

    mojo::ScopedHandle script_handle = CreateMockHandle(kTestScript);

    auto callback = base::BindOnce(&MockCallbackHelper::JavaScriptCallback,
                                base::Unretained(mock_callback_.get()));

    JavaScriptExecuteRequestExtTest(
        std::move(script_handle),
        kScriptLength,
        false,
        std::move(callback));

    base::RunLoop().RunUntilIdle();
}

TEST_F(ArkWebLocalFrameMojoHandlerExtTest, JavaScriptExecuteRequestExt_EmptyScript) {
    const std::string kTestScript = "";
    const uint64_t kScriptLength = kTestScript.size();

    mojo::ScopedHandle script_handle = CreateMockHandle(kTestScript);

    auto callback = base::BindOnce(&MockCallbackHelper::JavaScriptCallback,
                                base::Unretained(mock_callback_.get()));

    JavaScriptExecuteRequestExtTest(
        std::move(script_handle),
        kScriptLength,
        true,
        std::move(callback));

    base::RunLoop().RunUntilIdle();
}

TEST_F(ArkWebLocalFrameMojoHandlerExtTest, JavaScriptExecuteRequestExt_LargeScript) {
    const std::string kTestScript = "function test() { var x = 1; return x; } test();";
    const uint64_t kScriptLength = kTestScript.size();

    mojo::ScopedHandle script_handle = CreateMockHandle(kTestScript);

    auto callback = base::BindOnce(&MockCallbackHelper::JavaScriptCallback,
                                base::Unretained(mock_callback_.get()));

    JavaScriptExecuteRequestExtTest(
        std::move(script_handle),
        kScriptLength,
        true,
        std::move(callback));

    base::RunLoop().RunUntilIdle();
}

#if BUILDFLAG(ARKWEB_BLANK_SCREEN_DETECTION)
TEST_F(ArkWebLocalFrameMojoHandlerExtTest, DetectBlankScreen_ValidUrl) {
    WTF::Vector<double> detectionTiming;
    detectionTiming.push_back(1.0);
    detectionTiming.push_back(2.0);

    WTF::Vector<int32_t> detectionMethods;
    detectionMethods.push_back(1);
    detectionMethods.push_back(2);

    DetectBlankScreenTest(
        "https://example.com",
        detectionTiming,
        detectionMethods,
        100);
}

TEST_F(ArkWebLocalFrameMojoHandlerExtTest, DetectBlankScreen_EmptyUrl) {
    WTF::Vector<double> detectionTiming;
    detectionTiming.push_back(1.0);

    WTF::Vector<int32_t> detectionMethods;
    detectionMethods.push_back(1);

    DetectBlankScreenTest(
        "",
        detectionTiming,
        detectionMethods,
        50);
}

TEST_F(ArkWebLocalFrameMojoHandlerExtTest, DetectBlankScreen_EmptyVectors) {
    WTF::Vector<double> detectionTiming;
    WTF::Vector<int32_t> detectionMethods;

    DetectBlankScreenTest(
        "https://example.com",
        detectionTiming,
        detectionMethods,
        0);
}

TEST_F(ArkWebLocalFrameMojoHandlerExtTest, DetectBlankScreen_MultipleTimingPoints) {
    WTF::Vector<double> detectionTiming;
    detectionTiming.push_back(0.5);
    detectionTiming.push_back(1.0);
    detectionTiming.push_back(2.0);
    detectionTiming.push_back(3.0);

    WTF::Vector<int32_t> detectionMethods;
    detectionMethods.push_back(0);
    detectionMethods.push_back(1);
    detectionMethods.push_back(2);

    DetectBlankScreenTest(
        "https://example.com/page",
        detectionTiming,
        detectionMethods,
        200);
}
#endif

}