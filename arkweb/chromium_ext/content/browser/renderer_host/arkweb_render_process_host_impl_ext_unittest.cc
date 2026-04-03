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

#include "arkweb/chromium_ext/content/browser/renderer_host/arkweb_render_process_host_impl_ext.h"
#include "content/test/test_render_view_host.h"
#if BUILDFLAG(ARKWEB_READER_MODE)
#include "arkweb/chromium_ext/third_party/blink/public/mojom/dom_distiller/reader_mode_config.mojom.h"
#include "arkweb/ohos_nweb_ex/overrides/cef/libcef/browser/alloy/alloy_browser_reader_mode_config_utils.h"
#include "arkweb/ohos_nweb_ex/overrides/cef/libcef/browser/alloy/alloy_browser_reader_mode_config_utils_v2.h"
#endif

namespace content { 

class ArkWebRenderProcessHostImplExtTest : public RenderViewHostImplTestHarness {
 public:
  void SetUp() override {
    RenderViewHostImplTestHarness::SetUp();
    BrowserContext* browser_context_test = browser_context();
    StoragePartitionImpl* storage_partition = static_cast<StoragePartitionImpl*>(
        browser_context_test->GetDefaultStoragePartition());
    render_process_host_impl_ext_ = std::make_unique<ArkwebRenderProcessHostImplExt>(
        browser_context_test, storage_partition, true);
  }

  void TearDown() override {
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_MODE)
    render_process_host_impl_ext_->CancelChannelConnectedCheckTask(render_process_host_impl_ext_.get());
#endif
    render_process_host_impl_ext_.reset();
    RenderViewHostImplTestHarness::TearDown();
  }

  void ResetRenderInterface() {
    render_process_host_impl_ext_->renderer_interface_.reset();
  }

  std::unique_ptr<ArkwebRenderProcessHostImplExt> render_process_host_impl_ext_;
};


#if BUILDFLAG(ARKWEB_RENDER_PROCESS_MODE)

class MockArkwebRenderProcessHostImplExt : public ArkwebRenderProcessHostImplExt {
 public:
  explicit MockArkwebRenderProcessHostImplExt(
      BrowserContext* browser_context, StoragePartitionImpl* storage_partition_impl, int flags)
      : ArkwebRenderProcessHostImplExt(browser_context, storage_partition_impl, flags) {}

  bool mock_is_dead_ = false;
  bool mock_is_ready_ = false;

  bool is_dead() const {
    return mock_is_dead_;
  }

  bool IsReady() override {
    return mock_is_ready_;
  }
};

std::unique_ptr<MockArkwebRenderProcessHostImplExt> CreateMockHost(
    ArkWebRenderProcessHostImplExtTest* test) {
  BrowserContext* browser_context_test = test->browser_context();
  StoragePartitionImpl* storage_partition = static_cast<StoragePartitionImpl*>(
      browser_context_test->GetDefaultStoragePartition());
  return std::make_unique<MockArkwebRenderProcessHostImplExt>(
      browser_context_test, storage_partition, true);
}

TEST_F(ArkWebRenderProcessHostImplExtTest, RenderProcessChannelConnectCheck_IsDead) {
  auto mock_host = CreateMockHost(this);
  mock_host->mock_is_dead_ = true;
  mock_host->mock_is_ready_ = true;
  EXPECT_NO_FATAL_FAILURE(mock_host->RenderProcessChannelConnectCheck());
}

TEST_F(ArkWebRenderProcessHostImplExtTest, RenderProcessChannelConnectCheck_NotReady) {
  auto mock_host = CreateMockHost(this);
  mock_host->mock_is_dead_ = false;
  mock_host->mock_is_ready_ = false;
  EXPECT_NO_FATAL_FAILURE(mock_host->RenderProcessChannelConnectCheck());
}

TEST_F(ArkWebRenderProcessHostImplExtTest, StartChannelConnectedCheckTask_CallbackIsNull) {
  render_process_host_impl_ext_->StartChannelConnectedCheckTask(render_process_host_impl_ext_.get());
  EXPECT_FALSE(render_process_host_impl_ext_->channel_connected_check_callback_.callback().is_null());
}

TEST_F(ArkWebRenderProcessHostImplExtTest, CancelChannelConnectedCheckTask_Success) {
  render_process_host_impl_ext_->StartChannelConnectedCheckTask(render_process_host_impl_ext_.get());

  render_process_host_impl_ext_->CancelChannelConnectedCheckTask(render_process_host_impl_ext_.get());
  EXPECT_TRUE(render_process_host_impl_ext_->channel_connected_check_callback_.IsCancelled());
}

TEST_F(ArkWebRenderProcessHostImplExtTest, CancelChannelConnectedCheckTask_CallbackIsNullorCancelled) {
  render_process_host_impl_ext_->CancelChannelConnectedCheckTask(render_process_host_impl_ext_.get());
  EXPECT_TRUE(render_process_host_impl_ext_->channel_connected_check_callback_.callback().is_null());
  EXPECT_TRUE(render_process_host_impl_ext_->channel_connected_check_callback_.IsCancelled());
}

#endif

TEST_F(ArkWebRenderProcessHostImplExtTest, OnThemeFontChange) {
  render_process_host_impl_ext_->OnThemeFontChange();
}

#if BUILDFLAG(ARKWEB_READER_MODE)
TEST_F(ArkWebRenderProcessHostImplExtTest, UpdateCloudControlReaderModeConfigData_NullData) {
  render_process_host_impl_ext_->UpdateCloudControlReaderModeConfigData(nullptr);
}

TEST_F(ArkWebRenderProcessHostImplExtTest, UpdateCloudControlReaderModeConfigData_WithData) {
  nweb_ex::BrowserReaderModeConfigData config_data;
  config_data.is_v2 = false;
  config_data.enabled = true;

  render_process_host_impl_ext_->UpdateCloudControlReaderModeConfigData(&config_data);
}

TEST_F(ArkWebRenderProcessHostImplExtTest, UpdateReaderModeConfig_NullData) {
  testing::internal::CaptureStderr();
  render_process_host_impl_ext_->UpdateReaderModeConfig(nullptr);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("[Distiller] param reader_mode_config_data is null"), std::string::npos);
}

TEST_F(ArkWebRenderProcessHostImplExtTest, UpdateReaderModeConfig_WithData) {
  nweb_ex::BrowserReaderModeConfigData config_data;
  config_data.is_v2 = false;
  config_data.enabled = true;
  render_process_host_impl_ext_->UpdateReaderModeConfig(&config_data);
}

TEST_F(ArkWebRenderProcessHostImplExtTest, UpdateReaderModeConfig_NullRendererInterface) {
  nweb_ex::BrowserReaderModeConfigData config_data;
  config_data.is_v2 = false;
  config_data.enabled = true;
  ResetRenderInterface();
  testing::internal::CaptureStderr();
  render_process_host_impl_ext_->UpdateReaderModeConfig(&config_data);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("[Distiller] interface is null"), std::string::npos);
}

TEST_F(ArkWebRenderProcessHostImplExtTest, UpdateReaderModeConfig_ValidDetailTemplates) {
  nweb_ex::BrowserReaderModeConfigData config_data;
  config_data.is_v2 = false;
  config_data.enabled = true;

  nweb_ex::BrowserReaderModeDetailTemplateConfig template_config;
  int template_id_for_test = 123;
  template_config.templateId = template_id_for_test;
  template_config.templateConfig = {"info", "intro", "chapter"};

  config_data.detail_config.templates.push_back(template_config);
  render_process_host_impl_ext_->UpdateReaderModeConfig(&config_data);
}

TEST_F(ArkWebRenderProcessHostImplExtTest, UpdateReaderModeConfig_InvalidDetailTemplates) {
  nweb_ex::BrowserReaderModeConfigData config_data;
  config_data.is_v2 = false;
  config_data.enabled = true;

  nweb_ex::BrowserReaderModeDetailTemplateConfig template_config;
  int template_id_for_test = 123;
  template_config.templateId = template_id_for_test;
  template_config.templateConfig = {"info", "intro"};

  config_data.detail_config.templates.push_back(template_config);

  testing::internal::CaptureStderr();
  render_process_host_impl_ext_->UpdateReaderModeConfig(&config_data);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("UpdateReaderModeConfig detail template must 3 items[info, intro, chapter]"),
      std::string::npos);
}

TEST_F(ArkWebRenderProcessHostImplExtTest, UpdateReaderModeConfig_ValidContentTemplates) {
  nweb_ex::BrowserReaderModeConfigData config_data;
  config_data.is_v2 = false;
  config_data.enabled = true;

  nweb_ex::BrowserReaderModeContentTemplateConfig template_match_config;
  int template_id_for_test = 123;
  template_match_config.template_id = template_id_for_test;
  template_match_config.template_config = {"body", "chapter"};

  config_data.content_config.template_match.push_back(template_match_config);

  render_process_host_impl_ext_->UpdateReaderModeConfig(&config_data);
}

TEST_F(ArkWebRenderProcessHostImplExtTest, UpdateReaderModeConfig_InvalidContentTemplates) {
  nweb_ex::BrowserReaderModeConfigData config_data;
  config_data.is_v2 = false;
  config_data.enabled = true;

  nweb_ex::BrowserReaderModeContentTemplateConfig template_match_config;
  int template_id_for_test = 123;
  template_match_config.template_id = template_id_for_test;
  template_match_config.template_config = {"body"};

  config_data.content_config.template_match.push_back(template_match_config);

  testing::internal::CaptureStderr();
  render_process_host_impl_ext_->UpdateReaderModeConfig(&config_data);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("UpdateReaderModeConfig content template must 2 items[body, chapter]"),
      std::string::npos);
}

TEST_F(ArkWebRenderProcessHostImplExtTest, UpdateReaderModeConfig_ValidContentFeatureConfig) {
  nweb_ex::BrowserReaderModeConfigData config_data;
  config_data.is_v2 = false;
  config_data.enabled = true;

  config_data.content_config.feature_match = {
    {"prev1", "prev2"},
    {"next1", "next2"},
    {"catalog1", "catalog2"}
  };

  render_process_host_impl_ext_->UpdateReaderModeConfig(&config_data);
}

TEST_F(ArkWebRenderProcessHostImplExtTest, UpdateReaderModeConfig_InvalidContentFeatureConfig) {
  nweb_ex::BrowserReaderModeConfigData config_data;
  config_data.is_v2 = false;
  config_data.enabled = true;

  config_data.content_config.feature_match = {
    {"prev1"}
  };

  testing::internal::CaptureStderr();
  render_process_host_impl_ext_->UpdateReaderModeConfig(&config_data);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("UpdateReaderModeConfig features must 3 items[pre, next, catalog]"),
      std::string::npos);
}

TEST_F(ArkWebRenderProcessHostImplExtTest, SetMojomReaderModeConfigV1_Basic) {
  auto config = blink::mojom::ReaderModeConfig::New();
  nweb_ex::BrowserReaderModeConfigData config_data;

  config_data.enabled = true;
  config_data.content_config.meta_data.must_have_catalog = true;
  config_data.content_config.meta_data.must_have_prev_and_next = false;
  config_data.content_config.meta_data.minimum_content_length = 100;

  render_process_host_impl_ext_->SetMojomReaderModeConfigV1(config.get(), &config_data);

  EXPECT_TRUE(config->reader_mode_enabled);
  EXPECT_TRUE(config->must_have_catalog);
  EXPECT_FALSE(config->must_have_prev_and_next);
  EXPECT_EQ(config->minimum_content_length, 100);
}

TEST_F(ArkWebRenderProcessHostImplExtTest, SetMojomReaderModeConfigV1_NegativeContentLength) {
  auto config = blink::mojom::ReaderModeConfig::New();
  nweb_ex::BrowserReaderModeConfigData config_data;

  config_data.enabled = true;
  config_data.content_config.meta_data.minimum_content_length = -50;

  render_process_host_impl_ext_->SetMojomReaderModeConfigV1(config.get(), &config_data);

  EXPECT_EQ(config->minimum_content_length, 0);
}

TEST_F(ArkWebRenderProcessHostImplExtTest, SetMojomReaderModeConfigV1_WithTemplates) {
  auto config = blink::mojom::ReaderModeConfig::New();
  nweb_ex::BrowserReaderModeConfigData config_data;

  config_data.enabled = true;

  nweb_ex::BrowserReaderModeDetailTemplateConfig detail_template;
  detail_template.templateId = 1;
  detail_template.templateConfig = {"book_info", "book_intro", "book_chapters"};
  config_data.detail_config.templates.push_back(detail_template);

  nweb_ex::BrowserReaderModeContentTemplateConfig content_template;
  content_template.template_id = 2;
  content_template.template_config = {"content", "pager"};
  config_data.content_config.template_match.push_back(content_template);

  render_process_host_impl_ext_->SetMojomReaderModeConfigV1(config.get(), &config_data);

  EXPECT_EQ(config->detail_templates.size(), 1U);
  EXPECT_EQ(config->content_templates.size(), 1U);

  auto detail_it = config->detail_templates.find(1);
  ASSERT_NE(detail_it, config->detail_templates.end());
  EXPECT_EQ(detail_it->second.size(), 3U);

  auto content_it = config->content_templates.find(2);
  ASSERT_NE(content_it, config->content_templates.end());
  EXPECT_EQ(content_it->second.size(), 2U);
}

TEST_F(ArkWebRenderProcessHostImplExtTest, SetMojomReaderModeConfigV1_WithFeatures) {
  auto config = blink::mojom::ReaderModeConfig::New();
  nweb_ex::BrowserReaderModeConfigData config_data;

  config_data.enabled = true;
  config_data.content_config.feature_match = {
    {"prev1", "prev2"},
    {"next1", "next2"},
    {"catalog1", "catalog2"}
  };

  render_process_host_impl_ext_->SetMojomReaderModeConfigV1(config.get(), &config_data);

  EXPECT_EQ(config->content_features.size(), 3U);

  auto prev_it = config->content_features.find(blink::mojom::ContentFeatureIndex::PREV);
  ASSERT_NE(prev_it, config->content_features.end());
  EXPECT_EQ(prev_it->second.size(), 2U);

  auto next_it = config->content_features.find(blink::mojom::ContentFeatureIndex::NEXT);
  ASSERT_NE(next_it, config->content_features.end());
  EXPECT_EQ(next_it->second.size(), 2U);

  auto catalog_it = config->content_features.find(blink::mojom::ContentFeatureIndex::CATALOG);
  ASSERT_NE(catalog_it, config->content_features.end());
  EXPECT_EQ(catalog_it->second.size(), 2U);
}

TEST_F(ArkWebRenderProcessHostImplExtTest, SetMojomReaderModeConfigV2_Basic) {
  auto config = blink::mojom::ReaderModeConfig::New();
  nweb_ex::BrowserReaderModeConfigData config_data;

  config_data.is_v2 = true;
  config_data.globalConfig.metadataConfig.mustHaveCatalog = true;
  config_data.globalConfig.metadataConfig.mustHavePrevAndNext = false;
  config_data.globalConfig.metadataConfig.minimumContentLength = 200;

  render_process_host_impl_ext_->SetMojomReaderModeConfigV2(config.get(), &config_data);

  EXPECT_TRUE(config->must_have_catalog);
  EXPECT_FALSE(config->must_have_prev_and_next);
  EXPECT_EQ(config->minimum_content_length, 200);
}

TEST_F(ArkWebRenderProcessHostImplExtTest, SetMojomReaderModeConfigV2_NegativeContentLength) {
  auto config = blink::mojom::ReaderModeConfig::New();
  nweb_ex::BrowserReaderModeConfigData config_data;

  config_data.is_v2 = true;
  config_data.globalConfig.metadataConfig.minimumContentLength = -100;

  render_process_host_impl_ext_->SetMojomReaderModeConfigV2(config.get(), &config_data);

  EXPECT_EQ(config->minimum_content_length, 0);
}

TEST_F(ArkWebRenderProcessHostImplExtTest, SetMojomReaderModeConfigV2_WithTemplates) {
  auto config = blink::mojom::ReaderModeConfig::New();
  nweb_ex::BrowserReaderModeConfigData config_data;

  config_data.is_v2 = true;

  nweb_ex::BookDetailTemplateConfig book_detail;
  book_detail.templateId = 1;
  book_detail.templateConfig.bookInfo = "book_info";
  book_detail.templateConfig.bookIntro = "book_intro";
  book_detail.templateConfig.bookChapters = "book_chapters";
  config_data.globalConfig.bookDetailMatchConfig.push_back(book_detail);

  nweb_ex::TemplateMatchEntry content_match;
  content_match.templateId = 2;
  content_match.templateConfig.content = "content";
  content_match.templateConfig.pager = "pager";
  config_data.globalConfig.contentMatchConfig.templateMatch.push_back(content_match);

  render_process_host_impl_ext_->SetMojomReaderModeConfigV2(config.get(), &config_data);

  EXPECT_EQ(config->detail_templates.size(), 1U);
  EXPECT_EQ(config->content_templates.size(), 1U);

  auto detail_it = config->detail_templates.find(1);
  ASSERT_NE(detail_it, config->detail_templates.end());
  EXPECT_EQ(detail_it->second.size(), 3U);
  EXPECT_EQ(detail_it->second[0], "book_info");
  EXPECT_EQ(detail_it->second[1], "book_intro");
  EXPECT_EQ(detail_it->second[2], "book_chapters");

  auto content_it = config->content_templates.find(2);
  ASSERT_NE(content_it, config->content_templates.end());
  EXPECT_EQ(content_it->second.size(), 2U);
  EXPECT_EQ(content_it->second[0], "content");
  EXPECT_EQ(content_it->second[1], "pager");
}

TEST_F(ArkWebRenderProcessHostImplExtTest, SetMojomReaderModeConfigV2_WithFeatures) {
  auto config = blink::mojom::ReaderModeConfig::New();
  nweb_ex::BrowserReaderModeConfigData config_data;

  config_data.is_v2 = true;
  config_data.globalConfig.contentMatchConfig.featureMatch.prevPage = {"prev1", "prev2"};
  config_data.globalConfig.contentMatchConfig.featureMatch.nextPage = {"next1", "next2"};
  config_data.globalConfig.contentMatchConfig.featureMatch.catalogPage = {"catalog1", "catalog2"};

  render_process_host_impl_ext_->SetMojomReaderModeConfigV2(config.get(), &config_data);

  EXPECT_EQ(config->content_features.size(), 3U);

  auto prev_it = config->content_features.find(blink::mojom::ContentFeatureIndex::PREV);
  ASSERT_NE(prev_it, config->content_features.end());
  EXPECT_EQ(prev_it->second.size(), 2U);
  EXPECT_EQ(prev_it->second[0], "prev1");
  EXPECT_EQ(prev_it->second[1], "prev2");

  auto next_it = config->content_features.find(blink::mojom::ContentFeatureIndex::NEXT);
  ASSERT_NE(next_it, config->content_features.end());
  EXPECT_EQ(next_it->second.size(), 2U);
  EXPECT_EQ(next_it->second[0], "next1");
  EXPECT_EQ(next_it->second[1], "next2");

  auto catalog_it = config->content_features.find(blink::mojom::ContentFeatureIndex::CATALOG);
  ASSERT_NE(catalog_it, config->content_features.end());
  EXPECT_EQ(catalog_it->second.size(), 2U);
  EXPECT_EQ(catalog_it->second[0], "catalog1");
  EXPECT_EQ(catalog_it->second[1], "catalog2");
}

TEST_F(ArkWebRenderProcessHostImplExtTest, UpdateReaderModeConfig_EnabledV2) {
  nweb_ex::BrowserReaderModeConfigData config_data;

  config_data.enabled = false;
  config_data.is_v2 = true;
  config_data.globalConfig.metadataConfig.mustHaveCatalog = true;

  render_process_host_impl_ext_->UpdateReaderModeConfig(&config_data);
}

TEST_F(ArkWebRenderProcessHostImplExtTest, ReportDistillableResult_ValidInput) {
  std::string event_type = "test_event";
  std::string value = R"({"key":"value"})";

  render_process_host_impl_ext_->ReportDistillableResult(event_type, value);
}

TEST_F(ArkWebRenderProcessHostImplExtTest, ReportDistillableResult_EmptyValue) {
  std::string event_type = "test_event";
  std::string value = "";

  render_process_host_impl_ext_->ReportDistillableResult(event_type, value);
}

TEST_F(ArkWebRenderProcessHostImplExtTest, ReportDistillableResult_EmptyEventType) {
  std::string event_type = "";
  std::string value = R"({"key":"value"})";

  render_process_host_impl_ext_->ReportDistillableResult(event_type, value);
}
#endif

#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION)
TEST_F(ArkWebRenderProcessHostImplExtTest, UpdateVideoLoadOptimizationConfig_WithData) {
  nweb_ex::AlloyVideoLoadOptimizationData config_data;
  config_data.use_video_load_optimization_ = true;
  config_data.preload_video_time_ = 4;
  config_data.min_cache_time_ = 2;
  config_data.max_cache_time_ = 6;
  config_data.moov_size_ = 512;
  config_data.bit_rate_ = 2000;
  std::string exampleDomain = "https://www.baidu.com/";
  config_data.support_domains_.emplace_back(exampleDomain);
  render_process_host_impl_ext_->UpdateVideoLoadOptimizationConfigData(config_data);
}

TEST_F(ArkWebRenderProcessHostImplExtTest, UpdateVideoLoadOptimizationConfig_NullRendererInterface) {
  nweb_ex::AlloyVideoLoadOptimizationData config_data;
  config_data.use_video_load_optimization_ = true;
  ResetRenderInterface();
  testing::internal::CaptureStderr();
  render_process_host_impl_ext_->UpdateVideoLoadOptimizationConfig(config_data);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("UpdateVideoLoadOptimizationConfig interface is null"), std::string::npos);
}

TEST_F(ArkWebRenderProcessHostImplExtTest, UpdateVideoLoadOptimizationConfig_ValidContent) {
  nweb_ex::AlloyVideoLoadOptimizationData config_data;
  config_data.use_video_load_optimization_ = true;
  render_process_host_impl_ext_->UpdateVideoLoadOptimizationConfig(config_data);
}
#endif // ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION

}