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
#include "arkweb/ohos_nweb_ex/overrides/cef/libcef/browser/alloy/alloy_browser_reader_mode_config_utils.h"
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
  testing::internal::CaptureStderr();
  render_process_host_impl_ext_->StartChannelConnectedCheckTask(render_process_host_impl_ext_.get());
  std::string log = testing::internal::GetCapturedStderr();
  EXPECT_NE(log.find("but channel not connected, wait 10s to check"), std::string::npos);
  EXPECT_FALSE(render_process_host_impl_ext_->channel_connected_check_callback_.callback().is_null());
}

TEST_F(ArkWebRenderProcessHostImplExtTest, CancelChannelConnectedCheckTask_Success) {
  render_process_host_impl_ext_->StartChannelConnectedCheckTask(render_process_host_impl_ext_.get());

  testing::internal::CaptureStderr();
  render_process_host_impl_ext_->CancelChannelConnectedCheckTask(render_process_host_impl_ext_.get());
  std::string log = testing::internal::GetCapturedStderr();
  EXPECT_NE(log.find("cancel channel connected check task"), std::string::npos);
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
  config_data.enabled = true;

  render_process_host_impl_ext_->UpdateCloudControlReaderModeConfigData(&config_data);
}

TEST_F(ArkWebRenderProcessHostImplExtTest, UpdateReaderModeConfig_NullData) {
  testing::internal::CaptureStderr();
  render_process_host_impl_ext_->UpdateReaderModeConfig(nullptr);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("UpdateReaderModeConfig param reader_mode_config_data is null"), std::string::npos);
}

TEST_F(ArkWebRenderProcessHostImplExtTest, UpdateReaderModeConfig_WithData) {
  nweb_ex::BrowserReaderModeConfigData config_data;
  config_data.enabled = true;
  render_process_host_impl_ext_->UpdateReaderModeConfig(&config_data);
}

TEST_F(ArkWebRenderProcessHostImplExtTest, UpdateReaderModeConfig_NullRendererInterface) {
  nweb_ex::BrowserReaderModeConfigData config_data;
  config_data.enabled = true;
  ResetRenderInterface();
  testing::internal::CaptureStderr();
  render_process_host_impl_ext_->UpdateReaderModeConfig(&config_data);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("UpdateReaderModeConfig interface is null"), std::string::npos);
}

TEST_F(ArkWebRenderProcessHostImplExtTest, UpdateReaderModeConfig_ValidDetailTemplates) {
  nweb_ex::BrowserReaderModeConfigData config_data;
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