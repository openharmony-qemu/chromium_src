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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "arkweb/build/features/features.h"
#include "arkweb/ohos_nweb/src/mock_nweb_delegate.h"
#include "base/files/file_path.h"
#include "base/memory/weak_ptr.h"
#include "nweb_content_change_detection.h"
#include "nweb_delegate.h"
#include "ui/base/resource/resource_bundle.h"

using namespace testing;

namespace OHOS::NWeb {

class NWebContentChangeDetectionTest : public ::testing::Test {
 public:
  void SetUp() override {
    // Initialize ResourceBundle with empty pak path
    // GetRawDataResource will return empty string_view
    ui::ResourceBundle::CleanupSharedInstance();
    ui::ResourceBundle::InitSharedInstanceWithPakPath(base::FilePath());

    mock_delegate_ = std::make_shared<MockNWebDelegate>();
    weak_delegate_ = mock_delegate_->WeakFromThis();
  }

  void TearDown() override {
    mock_delegate_.reset();
    ui::ResourceBundle::CleanupSharedInstance();
  }

  std::shared_ptr<MockNWebDelegate> mock_delegate_;
  base::WeakPtr<NWebDelegateInterface> weak_delegate_;
};

// Test: SetContentChangeDetectionEnable with null delegate
TEST_F(NWebContentChangeDetectionTest, SetEnable_NullDelegate) {
  base::WeakPtr<NWebDelegateInterface> null_delegate;
  NWebContentChangeDetection detection(null_delegate);

  // Should not crash when delegate is null
  detection.SetContentChangeDetectionEnable(true);
  detection.SetContentChangeDetectionEnable(false);
}

// Test: SetContentChangeDetectionEnable with valid delegate, enable = true
TEST_F(NWebContentChangeDetectionTest, SetEnable_EnableTrue) {
  NWebContentChangeDetection detection(weak_delegate_);

#if BUILDFLAG(ARKWEB_AI)
  EXPECT_CALL(*mock_delegate_,
              RegisterOnLoadStartedCbForContentChange(_))
      .Times(1);
#endif

  detection.SetContentChangeDetectionEnable(true);
}

// Test: SetContentChangeDetectionEnable with valid delegate, enable = false
TEST_F(NWebContentChangeDetectionTest, SetEnable_EnableFalse) {
  NWebContentChangeDetection detection(weak_delegate_);

#if BUILDFLAG(ARKWEB_AI)
  EXPECT_CALL(*mock_delegate_,
              RegisterOnLoadStartedCbForContentChange(_))
      .Times(1);
#endif

  detection.SetContentChangeDetectionEnable(false);
}

// Test: SetContentChangeDetectionConfig with default values
TEST_F(NWebContentChangeDetectionTest, SetConfig_DefaultValues) {
  NWebContentChangeDetection detection(weak_delegate_);

  detection.SetContentChangeDetectionConfig(100, 0.15f);
}

// Test: SetContentChangeDetectionConfig with custom values
TEST_F(NWebContentChangeDetectionTest, SetConfig_CustomValues) {
  NWebContentChangeDetection detection(weak_delegate_);

  detection.SetContentChangeDetectionConfig(500, 0.5f);
  detection.SetContentChangeDetectionConfig(1000, 0.8f);
  detection.SetContentChangeDetectionConfig(50, 0.05f);
}

// Test: SetContentChangeDetectionConfig with boundary values
TEST_F(NWebContentChangeDetectionTest, SetConfig_BoundaryValues) {
  NWebContentChangeDetection detection(weak_delegate_);

  // Test minimum values
  detection.SetContentChangeDetectionConfig(0, 0.0f);

  // Test maximum values
  detection.SetContentChangeDetectionConfig(INT32_MAX, 1.0f);

  // Test negative values (should be handled)
  detection.SetContentChangeDetectionConfig(-100, -0.5f);
}

// Test: SetContentChangeDetectionConfig with null delegate
TEST_F(NWebContentChangeDetectionTest, SetConfig_NullDelegate) {
  base::WeakPtr<NWebDelegateInterface> null_delegate;
  NWebContentChangeDetection detection(null_delegate);

  // Should not crash when delegate is null
  detection.SetContentChangeDetectionConfig(100, 0.15f);
}

// Test: Multiple calls to SetContentChangeDetectionEnable
TEST_F(NWebContentChangeDetectionTest, SetEnable_MultipleCalls) {
  NWebContentChangeDetection detection(weak_delegate_);

#if BUILDFLAG(ARKWEB_AI)
  EXPECT_CALL(*mock_delegate_,
              RegisterOnLoadStartedCbForContentChange(_))
      .Times(3);
#endif

  detection.SetContentChangeDetectionEnable(true);
  detection.SetContentChangeDetectionEnable(false);
  detection.SetContentChangeDetectionEnable(true);
}

// Test: SetConfig before SetEnable
TEST_F(NWebContentChangeDetectionTest, SetConfig_BeforeSetEnable) {
  NWebContentChangeDetection detection(weak_delegate_);

  detection.SetContentChangeDetectionConfig(200, 0.3f);

#if BUILDFLAG(ARKWEB_AI)
  EXPECT_CALL(*mock_delegate_,
              RegisterOnLoadStartedCbForContentChange(_))
      .Times(1);
#endif

  detection.SetContentChangeDetectionEnable(true);
}

// Test: SetConfig after SetEnable
TEST_F(NWebContentChangeDetectionTest, SetConfig_AfterSetEnable) {
  NWebContentChangeDetection detection(weak_delegate_);

#if BUILDFLAG(ARKWEB_AI)
  EXPECT_CALL(*mock_delegate_,
              RegisterOnLoadStartedCbForContentChange(_))
      .Times(1);
#endif

  detection.SetContentChangeDetectionEnable(true);
  detection.SetContentChangeDetectionConfig(300, 0.4f);
}

// Test: Delegate becomes invalid during operation
TEST_F(NWebContentChangeDetectionTest, DelegateBecomesInvalid) {
  NWebContentChangeDetection detection(weak_delegate_);

#if BUILDFLAG(ARKWEB_AI)
  EXPECT_CALL(*mock_delegate_,
              RegisterOnLoadStartedCbForContentChange(_))
      .Times(1);
#endif

  detection.SetContentChangeDetectionEnable(true);

  // Invalidate the delegate
  mock_delegate_.reset();

  // Should handle invalid delegate gracefully
  detection.SetContentChangeDetectionConfig(100, 0.15f);
  detection.SetContentChangeDetectionEnable(false);
}

// Test: Zero min_report_time
TEST_F(NWebContentChangeDetectionTest, SetConfig_ZeroMinReportTime) {
  NWebContentChangeDetection detection(weak_delegate_);

  detection.SetContentChangeDetectionConfig(0, 0.15f);
}

// Test: Zero text_content_ratio
TEST_F(NWebContentChangeDetectionTest, SetConfig_ZeroTextContentRatio) {
  NWebContentChangeDetection detection(weak_delegate_);

  detection.SetContentChangeDetectionConfig(100, 0.0f);
}

// Test: Both parameters zero
TEST_F(NWebContentChangeDetectionTest, SetConfig_BothZero) {
  NWebContentChangeDetection detection(weak_delegate_);

  detection.SetContentChangeDetectionConfig(0, 0.0f);
}

// Test: Large min_report_time value
TEST_F(NWebContentChangeDetectionTest, SetConfig_LargeMinReportTime) {
  NWebContentChangeDetection detection(weak_delegate_);

  detection.SetContentChangeDetectionConfig(1000000, 0.15f);
}

// Test: Text content ratio at 1.0
TEST_F(NWebContentChangeDetectionTest, SetConfig_TextRatioAtOne) {
  NWebContentChangeDetection detection(weak_delegate_);

  detection.SetContentChangeDetectionConfig(100, 1.0f);
}

// Test: Text content ratio above 1.0
TEST_F(NWebContentChangeDetectionTest, SetConfig_TextRatioAboveOne) {
  NWebContentChangeDetection test(weak_delegate_);

  // Test with ratio > 1.0 (should be handled by the implementation)
  test.SetContentChangeDetectionConfig(100, 1.5f);
}

// Test: Very small text content ratio
TEST_F(NWebContentChangeDetectionTest, SetConfig_VerySmallTextRatio) {
  NWebContentChangeDetection detection(weak_delegate_);

  detection.SetContentChangeDetectionConfig(100, 0.001f);
}

// Test: Alternating enable/disable
TEST_F(NWebContentChangeDetectionTest, SetEnable_Alternating) {
  NWebContentChangeDetection detection(weak_delegate_);

#if BUILDFLAG(ARKWEB_AI)
  EXPECT_CALL(*mock_delegate_,
              RegisterOnLoadStartedCbForContentChange(_))
      .Times(5);
#endif

  detection.SetContentChangeDetectionEnable(true);
  detection.SetContentChangeDetectionEnable(false);
  detection.SetContentChangeDetectionEnable(true);
  detection.SetContentChangeDetectionEnable(false);
  detection.SetContentChangeDetectionEnable(true);
}

// Test: Config change while enabled
TEST_F(NWebContentChangeDetectionTest, SetConfig_WhileEnabled) {
  NWebContentChangeDetection detection(weak_delegate_);

#if BUILDFLAG(ARKWEB_AI)
  EXPECT_CALL(*mock_delegate_,
              RegisterOnLoadStartedCbForContentChange(_))
      .Times(1);
#endif

  detection.SetContentChangeDetectionEnable(true);
  detection.SetContentChangeDetectionConfig(200, 0.25f);
  detection.SetContentChangeDetectionConfig(300, 0.35f);
  detection.SetContentChangeDetectionConfig(400, 0.45f);
}

}  // namespace OHOS::NWeb
