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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/frame/local_frame_for_include.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/core/page/page_utils.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/renderer/core/frame/frame_test_helpers.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/local_frame_view.h"
#include "third_party/blink/renderer/core/testing/core_unit_test_helper.h"
#include "third_party/blink/renderer/core/testing/dummy_page_holder.h"
#include "third_party/blink/renderer/core/page/chrome_client.h"
#include "third_party/blink/renderer/platform/testing/task_environment.h"
#include "arkweb/ohos_adapter_ndk/mock_ndk_api/include/mock_sys_info_util_ext.h"

namespace blink {

class LocalFrameUtilTest : public RenderingTest {
 protected:
  void SetUp() override {
    EnableCompositing();
    RenderingTest::SetUp();
    web_view_helper_.Initialize();
    dummy_page_holder_ = std::make_unique<DummyPageHolder>(gfx::Size(800, 600));
    local_frame_ = &dummy_page_holder_->GetFrame();
    page_ = &dummy_page_holder_->GetPage();
  }

  Document& GetDocument() {
    return *static_cast<Document*>(
        web_view_helper_.LocalMainFrame()->GetDocument());
  }

  void SetTextZoomFactor(float text_zoom_factor) {
    local_frame_->text_zoom_factor_ = text_zoom_factor;
  }

  void SetLayoutZoomFactor(float layout_zoom_factor) {
    local_frame_->layout_zoom_factor_ = layout_zoom_factor;
  }

  std::unique_ptr<DummyPageHolder> dummy_page_holder_;
  raw_ptr<LocalFrame> local_frame_;
  raw_ptr<Page> page_;
  frame_test_helpers::WebViewHelper web_view_helper_;
};

TEST_F(LocalFrameUtilTest, SetZoomFactorsExt_NonTablet) {
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = true;
  auto& system_properties_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(system_properties_mock, IsTabletDevice())
      .WillOnce(testing::Return(false))
      .WillRepeatedly(testing::Return(false));

  float layout_zoom = 1.5f;
  float text_zoom = 1.2f;
  bool layout_zoom_changed = true;

  local_frame_->scale_limits_min_changed_ = false;
  local_frame_->scale_limits_max_changed_ = false;

  LocalFrameUtil::SetLayoutAndTextZoomFactorsExt(
      local_frame_, layout_zoom, text_zoom, layout_zoom_changed, page_);

  EXPECT_FALSE(local_frame_->scale_limits_min_changed_);
  EXPECT_FALSE(local_frame_->scale_limits_max_changed_);
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = false;
}

TEST_F(LocalFrameUtilTest, SetZoomFactorsExt_Tablet_ZoomLessOrEqualOne) {
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = true;
  auto& system_properties_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(system_properties_mock, IsTabletDevice())
      .WillOnce(testing::Return(true))
      .WillRepeatedly(testing::Return(true));

  float layout_zoom = 0.8f;
  float text_zoom = 1.0f;
  bool layout_zoom_changed = true;

  local_frame_->scale_limits_min_changed_ = false;
  local_frame_->scale_limits_max_changed_ = false;

  LocalFrameUtil::SetLayoutAndTextZoomFactorsExt(
      local_frame_, layout_zoom, text_zoom, layout_zoom_changed, page_);

  EXPECT_FALSE(local_frame_->scale_limits_min_changed_);
  EXPECT_TRUE(local_frame_->scale_limits_max_changed_);
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = false;
}

TEST_F(LocalFrameUtilTest, SetZoomFactorsExt_Tablet_NoZoomChange) {
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = true;
  auto& system_properties_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(system_properties_mock, IsTabletDevice())
      .WillOnce(testing::Return(true))
      .WillRepeatedly(testing::Return(true));

  float layout_zoom = 1.5f;
  float text_zoom = 1.2f;
  bool layout_zoom_changed = false;

  local_frame_->scale_limits_min_changed_ = false;
  local_frame_->scale_limits_max_changed_ = false;

  LocalFrameUtil::SetLayoutAndTextZoomFactorsExt(
      local_frame_, layout_zoom, text_zoom, layout_zoom_changed, page_);

  EXPECT_FALSE(local_frame_->scale_limits_min_changed_);
  EXPECT_FALSE(local_frame_->scale_limits_max_changed_);
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = false;
}

TEST_F(LocalFrameUtilTest, SetZoomFactorsPage_NullPage) {
  float layout_zoom = 1.0f;
  float text_zoom = 1.0f;

  bool result = LocalFrameUtil::SetLayoutAndTextZoomFactorsPage(
      local_frame_, layout_zoom, text_zoom, nullptr);

  EXPECT_FALSE(result);
}

TEST_F(LocalFrameUtilTest, SetZoomFactorsPage_NoChange) {
  SetLayoutZoomFactor(1.5f);
  SetTextZoomFactor(1.2f);
  page_->GetSettings().SetTextZoomFactor(1.2f);

  float layout_zoom = 1.5f;
  float text_zoom = 0.0f;

  bool result = LocalFrameUtil::SetLayoutAndTextZoomFactorsPage(
      local_frame_, layout_zoom, text_zoom, page_);

  EXPECT_FALSE(result);
  EXPECT_EQ(text_zoom, 1.2f);
}

TEST_F(LocalFrameUtilTest, SetZoomFactorsPage_LayoutChanged) {
  SetLayoutZoomFactor(1.0f);
  SetTextZoomFactor(1.0f);
  page_->GetSettings().SetTextZoomFactor(1.0f);

  float layout_zoom = 1.5f;
  float text_zoom = 0.0f;

  bool result = LocalFrameUtil::SetLayoutAndTextZoomFactorsPage(
      local_frame_, layout_zoom, text_zoom, page_);

  EXPECT_TRUE(result);
  EXPECT_EQ(text_zoom, 1.0f);
}

TEST_F(LocalFrameUtilTest, SetZoomFactorsPage_TextChanged) {
  SetLayoutZoomFactor(1.0f);
  SetTextZoomFactor(1.0f);
  page_->GetSettings().SetTextZoomFactor(1.5f);

  float layout_zoom = 1.0f;
  float text_zoom = 0.0f;

  bool result = LocalFrameUtil::SetLayoutAndTextZoomFactorsPage(
      local_frame_, layout_zoom, text_zoom, page_);

  EXPECT_TRUE(result);
  EXPECT_EQ(text_zoom, 1.5f);
}

TEST_F(LocalFrameUtilTest, SetTextZoomFactorsExt) {
  LocalFrameUtil::SetTextZoomFactorsExt(local_frame_);
}

TEST_F(LocalFrameUtilTest, SetZoomFactorsExt_Tablet_ZoomGreaterThanOne_AlreadyChanged) {
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = true;
  auto& system_properties_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(system_properties_mock, IsTabletDevice())
      .WillOnce(testing::Return(true))
      .WillRepeatedly(testing::Return(true));

  float layout_zoom = 1.5f;
  float text_zoom = 1.2f;
  bool layout_zoom_changed = true;

  local_frame_->scale_limits_min_changed_ = true;
  local_frame_->scale_limits_max_changed_ = false;

  LocalFrameUtil::SetLayoutAndTextZoomFactorsExt(
      local_frame_, layout_zoom, text_zoom, layout_zoom_changed, page_);

  EXPECT_TRUE(local_frame_->scale_limits_min_changed_);
  EXPECT_FALSE(local_frame_->scale_limits_max_changed_);
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = false;
}

TEST_F(LocalFrameUtilTest, SetAdBlockEnableForSite) {
  local_frame_->SetAdBlockEnableForSite(true);
  EXPECT_TRUE(local_frame_->GetAdBlockEnableForSite());
  local_frame_->SetAdBlockEnableForSite(false);
  EXPECT_FALSE(local_frame_->GetAdBlockEnableForSite());
}

TEST_F(LocalFrameUtilTest, SetZoomFactorsExt_NotTabletDevice) {
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = true;
  auto& system_properties_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(system_properties_mock, IsTabletDevice())
      .WillOnce(testing::Return(false))
      .WillRepeatedly(testing::Return(false));

  float layout_zoom = 1.5f;
  float text_zoom = 1.2f;
  bool layout_zoom_changed = true;

  local_frame_->scale_limits_min_changed_ = false;
  local_frame_->scale_limits_max_changed_ = false;

  LocalFrameUtil::SetLayoutAndTextZoomFactorsExt(
      local_frame_, layout_zoom, text_zoom, layout_zoom_changed, page_);

  EXPECT_FALSE(local_frame_->scale_limits_min_changed_);
  EXPECT_FALSE(local_frame_->scale_limits_max_changed_);
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = false;
}

TEST_F(LocalFrameUtilTest, SetZoomFactorsExt_Tablet_ZoomMoreAndNoZoomChange) {
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = true;
  auto& system_properties_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(system_properties_mock, IsTabletDevice())
      .WillOnce(testing::Return(true))
      .WillRepeatedly(testing::Return(true));

  float layout_zoom = 1.5f;
  float text_zoom = 1.2f;
  bool layout_zoom_changed = false;

  local_frame_->scale_limits_min_changed_ = false;
  local_frame_->scale_limits_max_changed_ = false;

  LocalFrameUtil::SetLayoutAndTextZoomFactorsExt(
      local_frame_, layout_zoom, text_zoom, layout_zoom_changed, page_);

  EXPECT_FALSE(local_frame_->scale_limits_min_changed_);
  EXPECT_FALSE(local_frame_->scale_limits_max_changed_);
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = false;
}

TEST_F(LocalFrameUtilTest, SetZoomFactorsExt_TabletDevice_ZoomLessAndZoomChanged) {
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = true;
  auto& system_properties_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(system_properties_mock, IsTabletDevice())
      .WillOnce(testing::Return(true))
      .WillRepeatedly(testing::Return(true));

  float layout_zoom = 0.8f;
  float text_zoom = 1.0f;
  bool layout_zoom_changed = true;

  local_frame_->scale_limits_min_changed_ = false;
  local_frame_->scale_limits_max_changed_ = false;

  LocalFrameUtil::SetLayoutAndTextZoomFactorsExt(
      local_frame_, layout_zoom, text_zoom, layout_zoom_changed, page_);

  EXPECT_FALSE(local_frame_->scale_limits_min_changed_);
  EXPECT_TRUE(local_frame_->scale_limits_max_changed_);
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = false;
}

TEST_F(LocalFrameUtilTest, SetZoomFactorsExt_Tablet_ZoomMoreAndZoomChanged) {
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = true;
  auto& system_properties_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(system_properties_mock, IsTabletDevice())
      .WillOnce(testing::Return(true))
      .WillRepeatedly(testing::Return(true));

  float layout_zoom = 1.5f;
  float text_zoom = 1.2f;
  bool layout_zoom_changed = true;

  local_frame_->scale_limits_min_changed_ = true;
  local_frame_->scale_limits_max_changed_ = false;

  LocalFrameUtil::SetLayoutAndTextZoomFactorsExt(
      local_frame_, layout_zoom, text_zoom, layout_zoom_changed, page_);

  EXPECT_TRUE(local_frame_->scale_limits_min_changed_);
  EXPECT_FALSE(local_frame_->scale_limits_max_changed_);
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = false;
}

TEST_F(LocalFrameUtilTest, SetZoomFactorsExt_Tablet_ZoomMoreAndAlreadyChanged) {
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = true;
  auto& system_properties_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(system_properties_mock, IsTabletDevice())
      .WillOnce(testing::Return(true))
      .WillRepeatedly(testing::Return(true));
  
  float layout_zoom = 1.5f;
  float text_zoom = 1.2f;
  bool layout_zoom_changed = true;

  local_frame_->scale_limits_min_changed_ = false;
  local_frame_->scale_limits_max_changed_ = true;

  LocalFrameUtil::SetLayoutAndTextZoomFactorsExt(
      local_frame_, layout_zoom, text_zoom, layout_zoom_changed, page_);

  EXPECT_TRUE(local_frame_->scale_limits_min_changed_);
  EXPECT_FALSE(local_frame_->scale_limits_max_changed_);
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = false;
}

TEST_F(LocalFrameUtilTest, SetZoomFactorsExt_TabletDevice_ZoomLessAndMaxZoomChanged) {
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = true;
  auto& system_properties_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(system_properties_mock, IsTabletDevice())
      .WillOnce(testing::Return(true))
      .WillRepeatedly(testing::Return(true));

  float layout_zoom = 0.8f;
  float text_zoom = 1.0f;
  bool layout_zoom_changed = true;

  local_frame_->scale_limits_min_changed_ = false;
  local_frame_->scale_limits_max_changed_ = true;

  LocalFrameUtil::SetLayoutAndTextZoomFactorsExt(
      local_frame_, layout_zoom, text_zoom, layout_zoom_changed, page_);

  EXPECT_FALSE(local_frame_->scale_limits_min_changed_);
  EXPECT_TRUE(local_frame_->scale_limits_max_changed_);
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = false;
}

TEST_F(LocalFrameUtilTest, SetZoomFactorsExt_TabletDevice_ZoomLessAndAlreadyChange) {
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = true;
  auto& system_properties_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(system_properties_mock, IsTabletDevice())
      .WillOnce(testing::Return(true))
      .WillRepeatedly(testing::Return(true));

  float layout_zoom = 0.8f;
  float text_zoom = 1.0f;
  bool layout_zoom_changed = true;

  local_frame_->scale_limits_min_changed_ = true;
  local_frame_->scale_limits_max_changed_ = false;

  LocalFrameUtil::SetLayoutAndTextZoomFactorsExt(
      local_frame_, layout_zoom, text_zoom, layout_zoom_changed, page_);

  EXPECT_FALSE(local_frame_->scale_limits_min_changed_);
  EXPECT_TRUE(local_frame_->scale_limits_max_changed_);
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = false;
}

#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION)
TEST_F(LocalFrameUtilTest, VideoLoadOpt_IsVideoPrioritySupportedTest) {
  EXPECT_FALSE(local_frame_->IsVideoPrioritySupported());
}

TEST_F(LocalFrameUtilTest, VideoLoadOpt_SetNewsFeedPageFittedTest) {
  EXPECT_FALSE(local_frame_->SetNewsFeedPageFitted());
}

TEST_F(LocalFrameUtilTest, VideoLoadOpt_SetVideoIsPlayingTest) {
  std::string videoId = "v_662102_html5_api";
  local_frame_->SetVideoIsPlaying(videoId, false);
}

TEST_F(LocalFrameUtilTest, VideoLoadOpt_SetVideoPriorityTest) {
  HeapVector<Member<VideoPriority>> videoVec;
  auto* videoPri = MakeGarbageCollected<VideoPriority>();
  videoPri->setId("v_662102_html5_api");
  videoPri->setPriority(1);

  auto* videoPriOther = MakeGarbageCollected<VideoPriority>();
  videoPriOther->setId("v_7a47ff_html5_api");
  videoPriOther->setPriority(2);

  videoVec.push_back(videoPri);
  videoVec.push_back(videoPriOther);
  local_frame_->SetVideoPriority(videoVec);
}
#endif // ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION

TEST_F(LocalFrameUtilTest, LocalFrameUtilTest_001) {
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = true;
  auto& system_properties_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(system_properties_mock, IsTabletDevice())
      .WillOnce(testing::Return(false))
      .WillRepeatedly(testing::Return(false));

  float layout_zoom = 1.5f;
  float text_zoom = 1.2f;
  bool layout_zoom_changed = true;

  local_frame_->scale_limits_min_changed_ = false;
  local_frame_->scale_limits_max_changed_ = false;

  LocalFrameUtil::SetLayoutAndTextZoomFactorsExt(
      local_frame_, layout_zoom, text_zoom, layout_zoom_changed, page_);

  EXPECT_FALSE(local_frame_->scale_limits_min_changed_);
  EXPECT_FALSE(local_frame_->scale_limits_max_changed_);
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = false;
}

TEST_F(LocalFrameUtilTest, LocalFrameUtilTest_002) {
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = true;
  auto& system_properties_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(system_properties_mock, IsTabletDevice())
      .WillOnce(testing::Return(true))
      .WillRepeatedly(testing::Return(true));

  float layout_zoom = 0.8f;
  float text_zoom = 1.0f;
  bool layout_zoom_changed = true;

  local_frame_->scale_limits_min_changed_ = false;
  local_frame_->scale_limits_max_changed_ = false;

  LocalFrameUtil::SetLayoutAndTextZoomFactorsExt(
      local_frame_, layout_zoom, text_zoom, layout_zoom_changed, page_);

  EXPECT_FALSE(local_frame_->scale_limits_min_changed_);
  EXPECT_TRUE(local_frame_->scale_limits_max_changed_);
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = false;
}

TEST_F(LocalFrameUtilTest, LocalFrameUtilTest_003) {
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = true;
  auto& system_properties_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(system_properties_mock, IsTabletDevice())
      .WillOnce(testing::Return(true))
      .WillRepeatedly(testing::Return(true));

  float layout_zoom = 1.5f;
  float text_zoom = 1.2f;
  bool layout_zoom_changed = false;

  local_frame_->scale_limits_min_changed_ = false;
  local_frame_->scale_limits_max_changed_ = false;

  LocalFrameUtil::SetLayoutAndTextZoomFactorsExt(
      local_frame_, layout_zoom, text_zoom, layout_zoom_changed, page_);

  EXPECT_FALSE(local_frame_->scale_limits_min_changed_);
  EXPECT_FALSE(local_frame_->scale_limits_max_changed_);
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = false;
}

TEST_F(LocalFrameUtilTest, LocalFrameUtilTest_004) {
  float layout_zoom = 1.0f;
  float text_zoom = 1.0f;

  bool result = LocalFrameUtil::SetLayoutAndTextZoomFactorsPage(
      local_frame_, layout_zoom, text_zoom, nullptr);

  EXPECT_FALSE(result);
}

TEST_F(LocalFrameUtilTest, LocalFrameUtilTest_005) {
  SetLayoutZoomFactor(1.5f);
  SetTextZoomFactor(1.2f);
  page_->GetSettings().SetTextZoomFactor(1.2f);

  float layout_zoom = 1.5f;
  float text_zoom = 0.0f;

  bool result = LocalFrameUtil::SetLayoutAndTextZoomFactorsPage(
      local_frame_, layout_zoom, text_zoom, page_);

  EXPECT_FALSE(result);
  EXPECT_EQ(text_zoom, 1.2f);
}

TEST_F(LocalFrameUtilTest, LocalFrameUtilTest_006) {
  SetLayoutZoomFactor(1.0f);
  SetTextZoomFactor(1.0f);
  page_->GetSettings().SetTextZoomFactor(1.0f);

  float layout_zoom = 1.5f;
  float text_zoom = 0.0f;

  bool result = LocalFrameUtil::SetLayoutAndTextZoomFactorsPage(
      local_frame_, layout_zoom, text_zoom, page_);

  EXPECT_TRUE(result);
  EXPECT_EQ(text_zoom, 1.0f);
}

TEST_F(LocalFrameUtilTest, LocalFrameUtilTest_007) {
  SetLayoutZoomFactor(1.0f);
  SetTextZoomFactor(1.0f);
  page_->GetSettings().SetTextZoomFactor(1.5f);

  float layout_zoom = 1.0f;
  float text_zoom = 0.0f;

  bool result = LocalFrameUtil::SetLayoutAndTextZoomFactorsPage(
      local_frame_, layout_zoom, text_zoom, page_);

  EXPECT_TRUE(result);
  EXPECT_EQ(text_zoom, 1.5f);
}

TEST_F(LocalFrameUtilTest, LocalFrameUtilTest_008) {
  LocalFrameUtil::SetTextZoomFactorsExt(local_frame_);
}

TEST_F(LocalFrameUtilTest, LocalFrameUtilTest_009) {
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = true;
  auto& system_properties_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(system_properties_mock, IsTabletDevice())
      .WillOnce(testing::Return(true))
      .WillRepeatedly(testing::Return(true));

  float layout_zoom = 1.5f;
  float text_zoom = 1.2f;
  bool layout_zoom_changed = true;

  local_frame_->scale_limits_min_changed_ = true;
  local_frame_->scale_limits_max_changed_ = false;

  LocalFrameUtil::SetLayoutAndTextZoomFactorsExt(
      local_frame_, layout_zoom, text_zoom, layout_zoom_changed, page_);

  EXPECT_TRUE(local_frame_->scale_limits_min_changed_);
  EXPECT_FALSE(local_frame_->scale_limits_max_changed_);
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = false;
}

TEST_F(LocalFrameUtilTest, LocalFrameUtilTest_010) {
  local_frame_->SetAdBlockEnableForSite(true);
  EXPECT_TRUE(local_frame_->GetAdBlockEnableForSite());
  local_frame_->SetAdBlockEnableForSite(false);
  EXPECT_FALSE(local_frame_->GetAdBlockEnableForSite());
}

TEST_F(LocalFrameUtilTest, LocalFrameUtilTest_011) {
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = true;
  auto& system_properties_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(system_properties_mock, IsTabletDevice())
      .WillOnce(testing::Return(false))
      .WillRepeatedly(testing::Return(false));

  float layout_zoom = 1.5f;
  float text_zoom = 1.2f;
  bool layout_zoom_changed = true;

  local_frame_->scale_limits_min_changed_ = false;
  local_frame_->scale_limits_max_changed_ = false;

  LocalFrameUtil::SetLayoutAndTextZoomFactorsExt(
      local_frame_, layout_zoom, text_zoom, layout_zoom_changed, page_);

  EXPECT_FALSE(local_frame_->scale_limits_min_changed_);
  EXPECT_FALSE(local_frame_->scale_limits_max_changed_);
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = false;
}

TEST_F(LocalFrameUtilTest, LocalFrameUtilTest_012) {
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = true;
  auto& system_properties_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(system_properties_mock, IsTabletDevice())
      .WillOnce(testing::Return(true))
      .WillRepeatedly(testing::Return(true));

  float layout_zoom = 1.5f;
  float text_zoom = 1.2f;
  bool layout_zoom_changed = false;

  local_frame_->scale_limits_min_changed_ = false;
  local_frame_->scale_limits_max_changed_ = false;

  LocalFrameUtil::SetLayoutAndTextZoomFactorsExt(
      local_frame_, layout_zoom, text_zoom, layout_zoom_changed, page_);

  EXPECT_FALSE(local_frame_->scale_limits_min_changed_);
  EXPECT_FALSE(local_frame_->scale_limits_max_changed_);
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = false;
}

TEST_F(LocalFrameUtilTest, LocalFrameUtilTest_013) {
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = true;
  auto& system_properties_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(system_properties_mock, IsTabletDevice())
      .WillOnce(testing::Return(true))
      .WillRepeatedly(testing::Return(true));

  float layout_zoom = 0.8f;
  float text_zoom = 1.0f;
  bool layout_zoom_changed = true;

  local_frame_->scale_limits_min_changed_ = false;
  local_frame_->scale_limits_max_changed_ = false;

  LocalFrameUtil::SetLayoutAndTextZoomFactorsExt(
      local_frame_, layout_zoom, text_zoom, layout_zoom_changed, page_);

  EXPECT_FALSE(local_frame_->scale_limits_min_changed_);
  EXPECT_TRUE(local_frame_->scale_limits_max_changed_);
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = false;
}

TEST_F(LocalFrameUtilTest, LocalFrameUtilTest_014) {
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = true;
  auto& system_properties_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(system_properties_mock, IsTabletDevice())
      .WillOnce(testing::Return(true))
      .WillRepeatedly(testing::Return(true));

  float layout_zoom = 1.5f;
  float text_zoom = 1.2f;
  bool layout_zoom_changed = true;

  local_frame_->scale_limits_min_changed_ = true;
  local_frame_->scale_limits_max_changed_ = false;

  LocalFrameUtil::SetLayoutAndTextZoomFactorsExt(
      local_frame_, layout_zoom, text_zoom, layout_zoom_changed, page_);

  EXPECT_TRUE(local_frame_->scale_limits_min_changed_);
  EXPECT_FALSE(local_frame_->scale_limits_max_changed_);
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = false;
}

TEST_F(LocalFrameUtilTest, LocalFrameUtilTest_015) {
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = true;
  auto& system_properties_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(system_properties_mock, IsTabletDevice())
      .WillOnce(testing::Return(true))
      .WillRepeatedly(testing::Return(true));
  
  float layout_zoom = 1.5f;
  float text_zoom = 1.2f;
  bool layout_zoom_changed = true;

  local_frame_->scale_limits_min_changed_ = false;
  local_frame_->scale_limits_max_changed_ = true;

  LocalFrameUtil::SetLayoutAndTextZoomFactorsExt(
      local_frame_, layout_zoom, text_zoom, layout_zoom_changed, page_);

  EXPECT_TRUE(local_frame_->scale_limits_min_changed_);
  EXPECT_FALSE(local_frame_->scale_limits_max_changed_);
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = false;
}

TEST_F(LocalFrameUtilTest, LocalFrameUtilTest_016) {
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = true;
  auto& system_properties_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(system_properties_mock, IsTabletDevice())
      .WillOnce(testing::Return(true))
      .WillRepeatedly(testing::Return(true));

  float layout_zoom = 0.8f;
  float text_zoom = 1.0f;
  bool layout_zoom_changed = true;

  local_frame_->scale_limits_min_changed_ = false;
  local_frame_->scale_limits_max_changed_ = true;

  LocalFrameUtil::SetLayoutAndTextZoomFactorsExt(
      local_frame_, layout_zoom, text_zoom, layout_zoom_changed, page_);

  EXPECT_FALSE(local_frame_->scale_limits_min_changed_);
  EXPECT_TRUE(local_frame_->scale_limits_max_changed_);
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = false;
}

TEST_F(LocalFrameUtilTest, LocalFrameUtilTest_017) {
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = true;
  auto& system_properties_mock = base::ohos::SysInfoUtilsMock::GetInstance();
  EXPECT_CALL(system_properties_mock, IsTabletDevice())
      .WillOnce(testing::Return(true))
      .WillRepeatedly(testing::Return(true));

  float layout_zoom = 0.8f;
  float text_zoom = 1.0f;
  bool layout_zoom_changed = true;

  local_frame_->scale_limits_min_changed_ = true;
  local_frame_->scale_limits_max_changed_ = false;

  LocalFrameUtil::SetLayoutAndTextZoomFactorsExt(
      local_frame_, layout_zoom, text_zoom, layout_zoom_changed, page_);

  EXPECT_FALSE(local_frame_->scale_limits_min_changed_);
  EXPECT_TRUE(local_frame_->scale_limits_max_changed_);
  base::ohos::SysInfoUtilsMock::mockIsTabletDevice = false;
}

#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION)
TEST_F(LocalFrameUtilTest, LocalFrameUtilTest_018) {
  EXPECT_FALSE(local_frame_->IsVideoPrioritySupported());
}

TEST_F(LocalFrameUtilTest, LocalFrameUtilTest_019) {
  EXPECT_FALSE(local_frame_->SetNewsFeedPageFitted());
}

TEST_F(LocalFrameUtilTest, LocalFrameUtilTest_020) {
  std::string videoId = "v_662102_html5_api";
  local_frame_->SetVideoIsPlaying(videoId, false);
}

TEST_F(LocalFrameUtilTest, LocalFrameUtilTest_021) {
  HeapVector<Member<VideoPriority>> videoVec;
  auto* videoPri = MakeGarbageCollected<VideoPriority>();
  videoPri->setId("v_662102_html5_api");
  videoPri->setPriority(1);

  auto* videoPriOther = MakeGarbageCollected<VideoPriority>();
  videoPriOther->setId("v_7a47ff_html5_api");
  videoPriOther->setPriority(2);

  videoVec.push_back(videoPri);
  videoVec.push_back(videoPriOther);
  local_frame_->SetVideoPriority(videoVec);
}
#endif // ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION

#if BUILDFLAG(ARKWEB_AI)
TEST_F(LocalFrameUtilTest, StartHighlightFadeTimer_001) {
  local_frame_->GetSettings()->SetArkwebAgentEnabled(false);
  local_frame_->StartHighlightFadeTimer(base::Milliseconds(1000));
  local_frame_->GetSettings()->SetArkwebAgentEnabled(true);
  local_frame_->StartHighlightFadeTimer(base::Milliseconds(1000));
  local_frame_->StartHighlightFadeTimer(base::Milliseconds(0));
}
#endif
}  // namespace blink