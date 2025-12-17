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

#include <memory>

#include "base/test/scoped_feature_list.h"
#include "build/build_config.h"
#include "content/public/test/browser_task_environment.h"
#include "content/test/test_content_client.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/accessibility/platform/browser_accessibility_manager.h"
#include "ui/accessibility/platform/provide_ax_platform_for_tests.h"
#include "ui/accessibility/platform/test_ax_node_id_delegate.h"
#include "ui/accessibility/platform/test_ax_platform_tree_manager_delegate.h"
#include "ui/strings/grit/auto_image_annotation_strings.h"
#include "ui/strings/grit/ax_strings.h"

#define private public
#include "content/browser/accessibility/browser_accessibility_manager_ohos.h"
#include "content/browser/accessibility/browser_accessibility_ohos.h"
#undef private

namespace content {

class MockContentClient : public content::TestContentClient {
 public:
  std::u16string GetLocalizedString(int message_id) override {
    switch (message_id) {
      case IDS_AX_COMBOBOX_EXPANDED_DIALOG:
        return u"Dialog";
      case IDS_AX_COMBOBOX_EXPANDED_AUTOCOMPLETE_DEFAULT:
        return u"Autocomplete options";
      case IDS_AX_COMBOBOX_EXPANDED_AUTOCOMPLETE_X_OPTIONS_AVAILABLE:
        return u"{x} options available";
      case IDS_AX_COMBOBOX_EXPANDED:
        return u"Expanded";
      case IDS_AX_ROLE_HEADING:
        return u"Heading";
      case IDS_AX_STATE_LINK_VISITED:
        return u"Visited";
      case IDS_AX_ROLE_POP_UP_BUTTON_MENU:
        return u"Popup button menu";
      case IDS_AX_ROLE_POP_UP_BUTTON_DIALOG:
        return u"Popup button dialog";
      case IDS_AX_ROLE_POP_UP_BUTTON:
        return u"Popup button";
      case IDS_AX_ROLL_CANVAS_OHOS:
        return u"Canvas";
      case IDS_AX_ROLE_COMBO_BOX:
        return u"Combo box";
      case IDS_AX_ROLE_DESCRIPTION_LIST:
        return u"Description list";
      case IDS_AX_ROLE_GRAPHIC:
        return u"Graphic";
      case IDS_AX_ROLE_BANNER:
        return u"Banner";
      case IDS_AX_ROLE_TABLE:
        return u"Table";
      case IDS_AX_ROLE_MARQUEE:
        return u"Marquee";
      case IDS_AX_ROLE_CHECK_BOX:
        return u"Checkbox";
      case IDS_AX_ROLE_RADIO:
        return u"Radio";
      case IDS_AX_ROLE_TEXT_FIELD:
        return u"Text field";
      case IDS_AX_MEDIA_VIDEO_ELEMENT:
        return u"Video";
      case IDS_AX_UNLABELED_IMAGE_ROLE_DESCRIPTION:
        return u"Unlabeled image";
      case IDS_AX_IMAGE_ELIGIBLE_FOR_ANNOTATION:
        return u"This image isn't labeled. Double tap on the more options "
               u"button at the top of the browser to get image descriptions.";
      case IDS_AX_IMAGE_ANNOTATION_PENDING:
        return u"Getting description...";
      case IDS_AX_IMAGE_ANNOTATION_ADULT:
        return u"Appears to contain adult content. No description available.";
      case IDS_AX_IMAGE_ANNOTATION_NO_DESCRIPTION:
        return u"No description available.";
      default:
        return std::u16string();
    }
  }
};

class BrowserAccessibilityOHOSTest : public ::testing::Test {
 public:
  void SetUp() override {
    test_browser_accessibility_delegate_ = std::make_unique<ui::TestAXPlatformTreeManagerDelegate>();
    SetContentClient(&client_);
    static ui::ProvideAXPlatformForTests provide_ax_platform_for_tests;
  }
  void TearDown() override {}

  std::unique_ptr<ui::TestAXPlatformTreeManagerDelegate> test_browser_accessibility_delegate_;
  ui::TestAXNodeIdDelegate node_id_delegate_;
  MockContentClient client_;
};

class RecordingAXDelegate : public ui::TestAXPlatformTreeManagerDelegate {
 public:
  void AccessibilityPerformAction(const ui::AXActionData& data) override { performed_actions_.push_back(data); }

  const std::vector<ui::AXActionData>& performed_actions() const { return performed_actions_; }

 private:
  std::vector<ui::AXActionData> performed_actions_;
};

TEST_F(BrowserAccessibilityOHOSTest, DistanceCalculationFunctionality) {
  gfx::Rect node_rect(0, 0, 100, 100);
  gfx::Rect item_rect(50, 50, 100, 100);

  EXPECT_TRUE(
      ui::BrowserAccessibilityOHOS::CheckRectBeam(node_rect, item_rect, static_cast<int32_t>(OHOS::NWeb::LEFT)));
  EXPECT_TRUE(
      ui::BrowserAccessibilityOHOS::CheckRectBeam(node_rect, item_rect, static_cast<int32_t>(OHOS::NWeb::RIGHT)));
  EXPECT_TRUE(ui::BrowserAccessibilityOHOS::CheckRectBeam(node_rect, item_rect, static_cast<int32_t>(OHOS::NWeb::UP)));
  EXPECT_TRUE(
      ui::BrowserAccessibilityOHOS::CheckRectBeam(node_rect, item_rect, static_cast<int32_t>(OHOS::NWeb::DOWN)));

  EXPECT_FALSE(
      ui::BrowserAccessibilityOHOS::IsToDirectionOf(node_rect, item_rect, static_cast<int32_t>(OHOS::NWeb::LEFT)));
  EXPECT_FALSE(
      ui::BrowserAccessibilityOHOS::IsToDirectionOf(node_rect, item_rect, static_cast<int32_t>(OHOS::NWeb::RIGHT)));

  double major_dist =
      ui::BrowserAccessibilityOHOS::MajorAxisDistance(node_rect, item_rect, static_cast<int32_t>(OHOS::NWeb::LEFT));
  EXPECT_EQ(major_dist, 0.0);
  double minor_dist =
      ui::BrowserAccessibilityOHOS::MinorAxisDistance(node_rect, item_rect, static_cast<int32_t>(OHOS::NWeb::LEFT));
  EXPECT_GE(minor_dist, 0.0);

  double weighted_dist = ui::BrowserAccessibilityOHOS::GetWeightedDistanceFor(major_dist, minor_dist);
  EXPECT_GT(weighted_dist, 0.0);
}

TEST_F(BrowserAccessibilityOHOSTest, CandidateRectFunctionality) {
  gfx::Rect node_rect(0, 0, 100, 100);
  gfx::Rect item_rect(150, 50, 100, 100);
  gfx::Rect temp_best(200, 50, 100, 100);
  EXPECT_FALSE(
      ui::BrowserAccessibilityOHOS::IsCandidateRect(node_rect, item_rect, static_cast<int32_t>(OHOS::NWeb::LEFT)));
  EXPECT_TRUE(
      ui::BrowserAccessibilityOHOS::IsCandidateRect(node_rect, item_rect, static_cast<int32_t>(OHOS::NWeb::RIGHT)));
  EXPECT_FALSE(ui::BrowserAccessibilityOHOS::CheckBetterRect(node_rect, static_cast<int32_t>(OHOS::NWeb::LEFT),
                                                             item_rect, temp_best));
  EXPECT_FALSE(ui::BrowserAccessibilityOHOS::OutrightBetter(node_rect, static_cast<int32_t>(OHOS::NWeb::LEFT),
                                                            item_rect, temp_best));
}

TEST_F(BrowserAccessibilityOHOSTest, DistanceCalculationEdgeCases) {
  gfx::Rect node_rect(0, 0, 100, 100);
  gfx::Rect item_rect(200, 200, 100, 100);
  double far_edge_dist =
      ui::BrowserAccessibilityOHOS::MajorAxisDistanceToFarEdge(node_rect, item_rect, static_cast<int32_t>(4));
  EXPECT_GT(far_edge_dist, 0.0);

  for (int32_t direction = 2; direction <= 5; ++direction) {
    double major_dist = ui::BrowserAccessibilityOHOS::MajorAxisDistance(node_rect, item_rect, direction);
    double minor_dist = ui::BrowserAccessibilityOHOS::MinorAxisDistance(node_rect, item_rect, direction);
    EXPECT_GE(major_dist, 0.0);
    EXPECT_GE(minor_dist, 0.0);
  }
}

TEST_F(BrowserAccessibilityOHOSTest, RectBeamEdgeCases) {
  gfx::Rect node_rect(0, 0, 100, 100);
  gfx::Rect overlapping_rect(50, 50, 100, 100);
  EXPECT_TRUE(ui::BrowserAccessibilityOHOS::CheckRectBeam(node_rect, overlapping_rect, static_cast<int32_t>(4)));
  gfx::Rect separated_rect(200, 200, 100, 100);
  EXPECT_FALSE(ui::BrowserAccessibilityOHOS::CheckRectBeam(node_rect, separated_rect, static_cast<int32_t>(4)));
  gfx::Rect touching_rect(100, 0, 100, 100);
  EXPECT_TRUE(ui::BrowserAccessibilityOHOS::CheckRectBeam(node_rect, touching_rect, static_cast<int32_t>(4)));
}

TEST_F(BrowserAccessibilityOHOSTest, DirectionJudgmentCases) {
  gfx::Rect node_rect(100, 100, 100, 100);
  gfx::Rect left_rect(0, 100, 50, 100);
  EXPECT_TRUE(
      ui::BrowserAccessibilityOHOS::IsToDirectionOf(node_rect, left_rect, static_cast<int32_t>(OHOS::NWeb::LEFT)));
  gfx::Rect right_rect(250, 100, 50, 100);
  EXPECT_TRUE(
      ui::BrowserAccessibilityOHOS::IsToDirectionOf(node_rect, right_rect, static_cast<int32_t>(OHOS::NWeb::RIGHT)));
  gfx::Rect up_rect(100, 0, 100, 50);
  EXPECT_TRUE(ui::BrowserAccessibilityOHOS::IsToDirectionOf(node_rect, up_rect, static_cast<int32_t>(OHOS::NWeb::UP)));
  gfx::Rect down_rect(100, 250, 100, 50);
  EXPECT_TRUE(
      ui::BrowserAccessibilityOHOS::IsToDirectionOf(node_rect, down_rect, static_cast<int32_t>(OHOS::NWeb::DOWN)));
}

TEST_F(BrowserAccessibilityOHOSTest, CandidateRectCases) {
  gfx::Rect node_rect(100, 100, 100, 100);
  gfx::Rect valid_candidate(0, 100, 50, 100);
  EXPECT_TRUE(ui::BrowserAccessibilityOHOS::IsCandidateRect(node_rect, valid_candidate, static_cast<int32_t>(4)));
  gfx::Rect invalid_candidate(200, 100, 50, 100);
  EXPECT_FALSE(ui::BrowserAccessibilityOHOS::IsCandidateRect(node_rect, invalid_candidate, static_cast<int32_t>(4)));
  const int32_t directions[] = {static_cast<int32_t>(OHOS::NWeb::UP), static_cast<int32_t>(OHOS::NWeb::DOWN),
                                static_cast<int32_t>(OHOS::NWeb::LEFT), static_cast<int32_t>(OHOS::NWeb::RIGHT)};
  for (int32_t direction : directions) {
    gfx::Rect test_rect;
    switch (direction) {
      case OHOS::NWeb::UP:
        test_rect = gfx::Rect(100, 0, 100, 50);
        break;
      case OHOS::NWeb::DOWN:
        test_rect = gfx::Rect(100, 200, 100, 50);
        break;
      case OHOS::NWeb::LEFT:
        test_rect = gfx::Rect(0, 100, 50, 100);
        break;
      case OHOS::NWeb::RIGHT:
        test_rect = gfx::Rect(200, 100, 50, 100);
        break;
      default:
        break;
    }
    bool is_candidate = ui::BrowserAccessibilityOHOS::IsCandidateRect(node_rect, test_rect, direction);
    EXPECT_TRUE(is_candidate);
  }
}

TEST_F(BrowserAccessibilityOHOSTest, CheckBetterRectCases) {
  gfx::Rect node_rect(100, 100, 100, 100);
  gfx::Rect item_rect(0, 100, 50, 100);
  gfx::Rect temp_best(200, 100, 50, 100);
  bool item_better = ui::BrowserAccessibilityOHOS::CheckBetterRect(node_rect, static_cast<int32_t>(OHOS::NWeb::LEFT),
                                                                   item_rect, temp_best);
  EXPECT_TRUE(item_better);
  bool temp_better = ui::BrowserAccessibilityOHOS::CheckBetterRect(node_rect, static_cast<int32_t>(OHOS::NWeb::RIGHT),
                                                                   temp_best, item_rect);
  EXPECT_TRUE(temp_better);
}

TEST_F(BrowserAccessibilityOHOSTest, WeightedDistanceCalculation) {
  double major_dist = 10.0;
  double minor_dist = 5.0;
  double weighted_dist = ui::BrowserAccessibilityOHOS::GetWeightedDistanceFor(major_dist, minor_dist);
  EXPECT_GT(weighted_dist, 0.0);

  weighted_dist = ui::BrowserAccessibilityOHOS::GetWeightedDistanceFor(0.0, 0.0);
  EXPECT_EQ(weighted_dist, 0.0);

  weighted_dist = ui::BrowserAccessibilityOHOS::GetWeightedDistanceFor(-5.0, -3.0);
  EXPECT_GT(weighted_dist, 0.0);
}

TEST_F(BrowserAccessibilityOHOSTest, MinorAxisDistanceSpecialCases) {
  gfx::Rect node_rect(0, 0, 100, 100);
  gfx::Rect item_rect(200, 0, 100, 100);
  double horizontal_minor =
      ui::BrowserAccessibilityOHOS::MinorAxisDistance(node_rect, item_rect, static_cast<int32_t>(OHOS::NWeb::LEFT));
  EXPECT_EQ(horizontal_minor, 0.0);

  gfx::Rect item_rect2(0, 200, 100, 100);
  double vertical_minor =
      ui::BrowserAccessibilityOHOS::MinorAxisDistance(node_rect, item_rect2, static_cast<int32_t>(OHOS::NWeb::UP));
  EXPECT_EQ(vertical_minor, 0.0);
}

TEST_F(BrowserAccessibilityOHOSTest, MajorAxisDistanceSpecialCases) {
  gfx::Rect node_rect(0, 0, 100, 100);
  gfx::Rect overlapping_rect(50, 50, 100, 100);
  double overlap_dist = ui::BrowserAccessibilityOHOS::MajorAxisDistance(node_rect, overlapping_rect,
                                                                        static_cast<int32_t>(OHOS::NWeb::LEFT));
  EXPECT_EQ(overlap_dist, 0.0);

  gfx::Rect touching_rect(100, 0, 100, 100);
  double touching_dist = ui::BrowserAccessibilityOHOS::MajorAxisDistance(node_rect, touching_rect,
                                                                         static_cast<int32_t>(OHOS::NWeb::LEFT));
  EXPECT_EQ(touching_dist, 0.0);
}

TEST_F(BrowserAccessibilityOHOSTest, MajorAxisDistanceToFarEdgeSpecialCases) {
  gfx::Rect node_rect(0, 0, 100, 100);
  gfx::Rect item_rect(200, 0, 100, 100);
  double far_edge_dist = ui::BrowserAccessibilityOHOS::MajorAxisDistanceToFarEdge(
      node_rect, item_rect, static_cast<int32_t>(OHOS::NWeb::LEFT));
  EXPECT_GT(far_edge_dist, 0.0);

  gfx::Rect overlapping_rect(50, 50, 100, 100);
  double overlap_far_dist = ui::BrowserAccessibilityOHOS::MajorAxisDistanceToFarEdge(
      node_rect, overlapping_rect, static_cast<int32_t>(OHOS::NWeb::LEFT));
  EXPECT_GT(overlap_far_dist, 0.0);
}

TEST_F(BrowserAccessibilityOHOSTest, IsToDirectionOfEdgeCases) {
  gfx::Rect node_rect(100, 100, 100, 100);
  gfx::Rect left_rect(0, 100, 50, 100);
  EXPECT_TRUE(
      ui::BrowserAccessibilityOHOS::IsToDirectionOf(node_rect, left_rect, static_cast<int32_t>(OHOS::NWeb::LEFT)));

  gfx::Rect overlapping_rect(50, 100, 100, 100);
  EXPECT_FALSE(ui::BrowserAccessibilityOHOS::IsToDirectionOf(node_rect, overlapping_rect,
                                                             static_cast<int32_t>(OHOS::NWeb::LEFT)));

  gfx::Rect right_rect(200, 100, 50, 100);
  EXPECT_FALSE(
      ui::BrowserAccessibilityOHOS::IsToDirectionOf(node_rect, right_rect, static_cast<int32_t>(OHOS::NWeb::LEFT)));
}

TEST_F(BrowserAccessibilityOHOSTest, CheckRectBeamEdgeCases) {
  gfx::Rect node_rect(0, 0, 100, 100);
  gfx::Rect same_rect(0, 0, 100, 100);
  EXPECT_TRUE(
      ui::BrowserAccessibilityOHOS::CheckRectBeam(node_rect, same_rect, static_cast<int32_t>(OHOS::NWeb::LEFT)));

  gfx::Rect separated_rect(200, 200, 100, 100);
  EXPECT_FALSE(
      ui::BrowserAccessibilityOHOS::CheckRectBeam(node_rect, separated_rect, static_cast<int32_t>(OHOS::NWeb::LEFT)));

  gfx::Rect touching_rect(100, 0, 100, 100);
  EXPECT_TRUE(
      ui::BrowserAccessibilityOHOS::CheckRectBeam(node_rect, touching_rect, static_cast<int32_t>(OHOS::NWeb::LEFT)));
}

TEST_F(BrowserAccessibilityOHOSTest, InvalidDirectionHandling) {
  gfx::Rect node_rect(0, 0, 100, 100);
  gfx::Rect item_rect(200, 0, 100, 100);
  int32_t invalid_direction = 99;

  bool beam_result = ui::BrowserAccessibilityOHOS::CheckRectBeam(node_rect, item_rect, invalid_direction);
  EXPECT_FALSE(beam_result);

  bool direction_result = ui::BrowserAccessibilityOHOS::IsToDirectionOf(node_rect, item_rect, invalid_direction);
  EXPECT_FALSE(direction_result);

  double major_dist = ui::BrowserAccessibilityOHOS::MajorAxisDistance(node_rect, item_rect, invalid_direction);
  EXPECT_EQ(major_dist, 0.0);

  double minor_dist = ui::BrowserAccessibilityOHOS::MinorAxisDistance(node_rect, item_rect, invalid_direction);
  EXPECT_EQ(minor_dist, 0.0);
}

TEST_F(BrowserAccessibilityOHOSTest, EmptyRectHandling) {
  gfx::Rect empty_rect(0, 0, 0, 0);
  gfx::Rect normal_rect(100, 100, 100, 100);
  bool beam_result =
      ui::BrowserAccessibilityOHOS::CheckRectBeam(empty_rect, normal_rect, static_cast<int32_t>(OHOS::NWeb::LEFT));
  EXPECT_FALSE(beam_result);

  bool direction_result =
      ui::BrowserAccessibilityOHOS::IsToDirectionOf(empty_rect, normal_rect, static_cast<int32_t>(OHOS::NWeb::LEFT));
  EXPECT_FALSE(direction_result);

  double major_dist =
      ui::BrowserAccessibilityOHOS::MajorAxisDistance(empty_rect, normal_rect, static_cast<int32_t>(OHOS::NWeb::LEFT));
  EXPECT_EQ(major_dist, 0.0);

  double minor_dist =
      ui::BrowserAccessibilityOHOS::MinorAxisDistance(empty_rect, normal_rect, static_cast<int32_t>(OHOS::NWeb::LEFT));
  EXPECT_GT(minor_dist, 0.0);
}

TEST_F(BrowserAccessibilityOHOSTest, NegativeCoordinateHandling) {
  gfx::Rect negative_rect(-100, -100, 100, 100);
  gfx::Rect positive_rect(100, 100, 100, 100);

  bool beam_result = ui::BrowserAccessibilityOHOS::CheckRectBeam(negative_rect, positive_rect,
                                                                 static_cast<int32_t>(OHOS::NWeb::LEFT));
  EXPECT_FALSE(beam_result);

  bool direction_result = ui::BrowserAccessibilityOHOS::IsToDirectionOf(negative_rect, positive_rect,
                                                                        static_cast<int32_t>(OHOS::NWeb::RIGHT));
  EXPECT_TRUE(direction_result);

  double major_dist = ui::BrowserAccessibilityOHOS::MajorAxisDistance(negative_rect, positive_rect,
                                                                      static_cast<int32_t>(OHOS::NWeb::RIGHT));
  EXPECT_GT(major_dist, 0.0);

  double minor_dist = ui::BrowserAccessibilityOHOS::MinorAxisDistance(negative_rect, positive_rect,
                                                                      static_cast<int32_t>(OHOS::NWeb::RIGHT));
  EXPECT_GT(minor_dist, 0.0);
}

TEST_F(BrowserAccessibilityOHOSTest, OutrightBetterCases) {
  gfx::Rect node_rect(100, 100, 100, 100);
  gfx::Rect rect1(0, 100, 50, 100);
  gfx::Rect rect2(200, 100, 50, 100);
  bool rect1_better =
      ui::BrowserAccessibilityOHOS::OutrightBetter(node_rect, static_cast<int32_t>(OHOS::NWeb::LEFT), rect1, rect2);
  EXPECT_FALSE(rect1_better);
  bool rect2_better =
      ui::BrowserAccessibilityOHOS::OutrightBetter(node_rect, static_cast<int32_t>(OHOS::NWeb::RIGHT), rect2, rect1);
  EXPECT_FALSE(rect2_better);
}

TEST_F(BrowserAccessibilityOHOSTest, Geometry_OutrightBetterTrue) {
  gfx::Rect node_rect(100, 100, 50, 50);

  gfx::Rect rect1(20, 110, 30, 30);

  gfx::Rect rect2(200, 200, 30, 30);

  bool better =
      ui::BrowserAccessibilityOHOS::OutrightBetter(node_rect, static_cast<int32_t>(OHOS::NWeb::LEFT), rect1, rect2);
  EXPECT_TRUE(better);
}

TEST_F(BrowserAccessibilityOHOSTest, Geometry_MajorAxisDistanceToFarEdge_AllDirs) {
  gfx::Rect a(100, 100, 50, 50);
  gfx::Rect b(10, 10, 20, 20);
  double dL = ui::BrowserAccessibilityOHOS::MajorAxisDistanceToFarEdge(a, b, static_cast<int32_t>(OHOS::NWeb::LEFT));
  double dR = ui::BrowserAccessibilityOHOS::MajorAxisDistanceToFarEdge(a, b, static_cast<int32_t>(OHOS::NWeb::RIGHT));
  double dU = ui::BrowserAccessibilityOHOS::MajorAxisDistanceToFarEdge(a, b, static_cast<int32_t>(OHOS::NWeb::UP));
  double dD = ui::BrowserAccessibilityOHOS::MajorAxisDistanceToFarEdge(a, b, static_cast<int32_t>(OHOS::NWeb::DOWN));
  EXPECT_GT(dL, 0.0);
  EXPECT_GT(dR, 0.0);
  EXPECT_GT(dU, 0.0);
  EXPECT_GT(dD, 0.0);
}

TEST_F(BrowserAccessibilityOHOSTest, Geometry_CheckRectBeam_InvalidDir) {
  gfx::Rect a(0, 0, 10, 10);
  gfx::Rect b(100, 100, 5, 5);
  EXPECT_FALSE(ui::BrowserAccessibilityOHOS::CheckRectBeam(a, b, 99));
}

TEST_F(BrowserAccessibilityOHOSTest, Geometry_IsToDirectionOf_InvalidDir) {
  gfx::Rect a(0, 0, 10, 10);
  gfx::Rect b(100, 100, 5, 5);
  EXPECT_FALSE(ui::BrowserAccessibilityOHOS::IsToDirectionOf(a, b, 99));
}

TEST_F(BrowserAccessibilityOHOSTest, Geometry_MajorMinorDistances_InvalidDir) {
  gfx::Rect a(0, 0, 10, 10);
  gfx::Rect b(100, 100, 5, 5);
  EXPECT_EQ(ui::BrowserAccessibilityOHOS::MajorAxisDistance(a, b, 99), 0.0);
  EXPECT_EQ(ui::BrowserAccessibilityOHOS::MinorAxisDistance(a, b, 99), 0.0);
}

TEST_F(BrowserAccessibilityOHOSTest, Geometry_OutrightBetter_TieBreakers) {
  gfx::Rect node_rect(0, 0, 50, 50);
  int dir = static_cast<int>(OHOS::NWeb::RIGHT);

  gfx::Rect rect1(100, 0, 30, 30);
  gfx::Rect rect2(100, 40, 30, 30);
  EXPECT_FALSE(ui::BrowserAccessibilityOHOS::OutrightBetter(node_rect, dir, rect1, rect2));

  gfx::Rect rect3(100, 0, 30, 30);
  EXPECT_FALSE(ui::BrowserAccessibilityOHOS::OutrightBetter(node_rect, dir, rect1, rect3));
}

TEST_F(BrowserAccessibilityOHOSTest, Geometry_OutrightBetter_LeftRightVariants) {
  gfx::Rect node_rect(0, 0, 50, 50);

  gfx::Rect l1(-100, 10, 20, 20);
  gfx::Rect l2(10, 200, 20, 20);
  EXPECT_TRUE(ui::BrowserAccessibilityOHOS::OutrightBetter(node_rect, static_cast<int>(OHOS::NWeb::LEFT), l1, l2));

  gfx::Rect r1(100, 10, 20, 20);
  gfx::Rect r2(10, -200, 20, 20);
  EXPECT_TRUE(ui::BrowserAccessibilityOHOS::OutrightBetter(node_rect, static_cast<int>(OHOS::NWeb::RIGHT), r1, r2));
}

TEST_F(BrowserAccessibilityOHOSTest, Geometry_OutrightBetter_UpDownDistanceCompare) {
  gfx::Rect node_rect(0, 0, 50, 50);

  gfx::Rect u1(0, -60, 20, 20);
  gfx::Rect u2(0, -10, 20, 5);
  EXPECT_FALSE(ui::BrowserAccessibilityOHOS::OutrightBetter(node_rect, static_cast<int>(OHOS::NWeb::UP), u1, u2));

  gfx::Rect d1(0, 110, 20, 20);
  gfx::Rect d2(0, 60, 20, 5);
  EXPECT_FALSE(ui::BrowserAccessibilityOHOS::OutrightBetter(node_rect, static_cast<int>(OHOS::NWeb::DOWN), d1, d2));

  gfx::Rect d2_far(0, 60, 20, 200);
  EXPECT_FALSE(
      ui::BrowserAccessibilityOHOS::OutrightBetter(node_rect, static_cast<int>(OHOS::NWeb::DOWN), d1, d2_far));
}

TEST_F(BrowserAccessibilityOHOSTest, Geometry_MajorAxisDistanceToFarEdge_MinOne) {
  gfx::Rect node_rect(0, 0, 10, 10);
  gfx::Rect item_rect_right(10, 0, 1, 1);
  double dR = ui::BrowserAccessibilityOHOS::MajorAxisDistanceToFarEdge(node_rect, item_rect_right,
                                                                       static_cast<int32_t>(OHOS::NWeb::RIGHT));
  EXPECT_GE(dR, 1.0);

  gfx::Rect item_rect_left(-1, 0, 1, 1);
  double dL = ui::BrowserAccessibilityOHOS::MajorAxisDistanceToFarEdge(node_rect, item_rect_left,
                                                                       static_cast<int32_t>(OHOS::NWeb::LEFT));
  EXPECT_GE(dL, 1.0);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetAccessibilityNodeByFocusMove_Directions) {
  ui::AXNodeData root;
  root.id = 8700;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids = {8701, 8702, 8703, 8704};

  ui::AXNodeData b1;
  b1.id = 8701;
  b1.role = ax::mojom::Role::kButton;
  b1.relative_bounds.bounds = gfx::RectF(0, 0, 50, 50);

  ui::AXNodeData b2;
  b2.id = 8702;
  b2.role = ax::mojom::Role::kButton;
  b2.relative_bounds.bounds = gfx::RectF(100, 0, 50, 50);

  ui::AXNodeData b3;
  b3.id = 8703;
  b3.role = ax::mojom::Role::kButton;
  b3.relative_bounds.bounds = gfx::RectF(0, 100, 50, 50);

  ui::AXNodeData b4;
  b4.id = 8704;
  b4.role = ax::mojom::Role::kButton;
  b4.relative_bounds.bounds = gfx::RectF(100, 100, 50, 50);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, b1, b2, b3, b4);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n1 = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(8701));
  auto* n2 = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(8702));
  auto* n3 = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(8703));
  auto* n4 = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(8704));
  ASSERT_NE(n1, nullptr);
  ASSERT_NE(n2, nullptr);
  ASSERT_NE(n3, nullptr);
  ASSERT_NE(n4, nullptr);
  EXPECT_EQ(n1->GetAccessibilityNodeByFocusMove(static_cast<int32_t>(OHOS::NWeb::RIGHT)), n2);
  EXPECT_EQ(n1->GetAccessibilityNodeByFocusMove(static_cast<int32_t>(OHOS::NWeb::DOWN)), n3);
  EXPECT_EQ(n4->GetAccessibilityNodeByFocusMove(static_cast<int32_t>(OHOS::NWeb::LEFT)), n3);
  EXPECT_EQ(n4->GetAccessibilityNodeByFocusMove(static_cast<int32_t>(OHOS::NWeb::UP)), n2);
  EXPECT_EQ(n1->GetAccessibilityNodeByFocusMove(99), nullptr);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetAccessibilityNodeByFocusMove_NullManager) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(1));
  ASSERT_NE(n, nullptr);

  EXPECT_EQ(n->GetAccessibilityNodeByFocusMove(static_cast<int32_t>(OHOS::NWeb::FORWARD)), nullptr);
  EXPECT_EQ(n->GetAccessibilityNodeByFocusMove(static_cast<int32_t>(OHOS::NWeb::BACKWARD)), nullptr);
  EXPECT_EQ(n->GetAccessibilityNodeByFocusMove(static_cast<int32_t>(OHOS::NWeb::LEFT)), nullptr);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_SliderScrollAndInputTagGate) {
  ui::AXNodeData root;
  root.id = 100;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(101);

  ui::AXNodeData slider;
  slider.id = 101;
  slider.role = ax::mojom::Role::kSlider;
  slider.AddStringAttribute(ax::mojom::StringAttribute::kHtmlTag, "input");
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kMinValueForRange, 0.0f);
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kMaxValueForRange, 100.0f);
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kValueForRange, 50.0f);
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kStepValueForRange, 5.0f);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, slider);

  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* node = manager->GetFromID(101);
  ASSERT_NE(node, nullptr);
  auto* ohos_node = static_cast<ui::BrowserAccessibilityOHOS*>(node);

  EXPECT_TRUE(ohos_node->IsScrollSupported());
  EXPECT_TRUE(ohos_node->CanScrollForward());
  EXPECT_TRUE(ohos_node->CanScrollBackward());

  ui::AXNodeData root2;
  root2.id = 110;
  root2.role = ax::mojom::Role::kRootWebArea;
  root2.child_ids.push_back(111);

  ui::AXNodeData slider2;
  slider2.id = 111;
  slider2.role = ax::mojom::Role::kSlider;
  slider2.AddStringAttribute(ax::mojom::StringAttribute::kHtmlTag, "div");

  ui::AXTreeUpdate update2 = ui::MakeAXTreeUpdateForTesting(root2, slider2);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager2 = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update2, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* node2 = static_cast<ui::BrowserAccessibilityOHOS*>(manager2->GetFromID(111));
  ASSERT_NE(node2, nullptr);
  EXPECT_FALSE(node2->IsScrollSupported());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_ScrollableContainerCanScroll) {
  ui::AXNodeData root;
  root.id = 400;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(401);

  ui::AXNodeData scrollable;
  scrollable.id = 401;
  scrollable.role = ax::mojom::Role::kGenericContainer;
  scrollable.AddBoolAttribute(ax::mojom::BoolAttribute::kScrollable, true);
  scrollable.AddIntAttribute(ax::mojom::IntAttribute::kScrollX, 10);
  scrollable.AddIntAttribute(ax::mojom::IntAttribute::kScrollY, 20);
  scrollable.AddIntAttribute(ax::mojom::IntAttribute::kScrollXMin, 0);
  scrollable.AddIntAttribute(ax::mojom::IntAttribute::kScrollYMin, 0);
  scrollable.AddIntAttribute(ax::mojom::IntAttribute::kScrollXMax, 100);
  scrollable.AddIntAttribute(ax::mojom::IntAttribute::kScrollYMax, 100);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, scrollable);

  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* node = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(401));
  ASSERT_NE(node, nullptr);

  EXPECT_TRUE(node->IsScrollSupported());
  EXPECT_TRUE(node->CanScrollLeft());
  EXPECT_TRUE(node->CanScrollUp());
  EXPECT_TRUE(node->CanScrollRight());
  EXPECT_TRUE(node->CanScrollDown());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_Scroll_ScrollDirectionsAndLimits) {
  ui::AXNodeData root;
  root.id = 7400;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(7401);

  ui::AXNodeData sc;
  sc.id = 7401;
  sc.role = ax::mojom::Role::kGenericContainer;
  sc.AddBoolAttribute(ax::mojom::BoolAttribute::kScrollable, true);
  sc.AddIntAttribute(ax::mojom::IntAttribute::kScrollX, 0);
  sc.AddIntAttribute(ax::mojom::IntAttribute::kScrollY, 0);
  sc.AddIntAttribute(ax::mojom::IntAttribute::kScrollXMin, 0);
  sc.AddIntAttribute(ax::mojom::IntAttribute::kScrollYMin, 0);
  sc.AddIntAttribute(ax::mojom::IntAttribute::kScrollXMax, 100);
  sc.AddIntAttribute(ax::mojom::IntAttribute::kScrollYMax, 100);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, sc);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(7401));
  ASSERT_NE(n, nullptr);

  EXPECT_FALSE(n->CanScrollUp());
  EXPECT_FALSE(n->CanScrollLeft());
  EXPECT_TRUE(n->CanScrollDown());
  EXPECT_TRUE(n->CanScrollRight());

  EXPECT_TRUE(n->CanScrollDown());
  EXPECT_TRUE(n->CanScrollRight());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_Scroll_AtBoundaries_CanScrollFlags) {
  ui::AXNodeData root;
  root.id = 8460;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(8461);

  ui::AXNodeData cont;
  cont.id = 8461;
  cont.role = ax::mojom::Role::kGenericContainer;
  cont.AddBoolAttribute(ax::mojom::BoolAttribute::kScrollable, true);
  cont.AddIntAttribute(ax::mojom::IntAttribute::kScrollXMin, 0);
  cont.AddIntAttribute(ax::mojom::IntAttribute::kScrollYMin, 0);
  cont.AddIntAttribute(ax::mojom::IntAttribute::kScrollXMax, 50);
  cont.AddIntAttribute(ax::mojom::IntAttribute::kScrollYMax, 50);
  cont.AddIntAttribute(ax::mojom::IntAttribute::kScrollX, 50);
  cont.AddIntAttribute(ax::mojom::IntAttribute::kScrollY, 50);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, cont);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(8461));
  ASSERT_NE(n, nullptr);

  EXPECT_TRUE(n->CanScrollUp());
  EXPECT_TRUE(n->CanScrollLeft());
  EXPECT_FALSE(n->CanScrollDown());
  EXPECT_FALSE(n->CanScrollRight());

  ui::AXNodeData root2;
  root2.id = 8470;
  root2.role = ax::mojom::Role::kRootWebArea;
  root2.child_ids.push_back(8471);

  ui::AXNodeData cont2;
  cont2.id = 8471;
  cont2.role = ax::mojom::Role::kGenericContainer;
  cont2.AddBoolAttribute(ax::mojom::BoolAttribute::kScrollable, true);
  cont2.AddIntAttribute(ax::mojom::IntAttribute::kScrollXMin, 0);
  cont2.AddIntAttribute(ax::mojom::IntAttribute::kScrollYMin, 0);
  cont2.AddIntAttribute(ax::mojom::IntAttribute::kScrollXMax, 50);
  cont2.AddIntAttribute(ax::mojom::IntAttribute::kScrollYMax, 50);
  cont2.AddIntAttribute(ax::mojom::IntAttribute::kScrollX, 0);
  cont2.AddIntAttribute(ax::mojom::IntAttribute::kScrollY, 0);

  ui::AXTreeUpdate update2 = ui::MakeAXTreeUpdateForTesting(root2, cont2);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager2 = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update2, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* m = static_cast<ui::BrowserAccessibilityOHOS*>(manager2->GetFromID(8471));
  ASSERT_NE(m, nullptr);

  EXPECT_FALSE(m->CanScrollUp());
  EXPECT_FALSE(m->CanScrollLeft());
  EXPECT_TRUE(m->CanScrollDown());
  EXPECT_TRUE(m->CanScrollRight());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_TableHeader_ImageUrl_NonScrollable) {
  ui::AXNodeData root;
  root.id = 2500;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(2501);
  root.child_ids.push_back(2502);
  root.child_ids.push_back(2503);

  ui::AXNodeData header;
  header.id = 2501;
  header.role = ax::mojom::Role::kColumnHeader;

  ui::AXNodeData image;
  image.id = 2502;
  image.role = ax::mojom::Role::kImage;
  image.AddStringAttribute(ax::mojom::StringAttribute::kUrl, "https://img.invalid/x.png");

  ui::AXNodeData container;
  container.id = 2503;
  container.role = ax::mojom::Role::kGenericContainer;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, header, image, container);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* h = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2501));
  auto* im = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2502));
  auto* c = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2503));
  ASSERT_NE(h, nullptr);
  ASSERT_NE(im, nullptr);
  ASSERT_NE(c, nullptr);
  EXPECT_TRUE(h->IsTableHeader());
  EXPECT_EQ(im->GetTargetUrl(), "https://img.invalid/x.png");
  EXPECT_FALSE(c->IsScrollSupported());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_SliderEdgeScrollability) {
  ui::AXNodeData root;
  root.id = 2900;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(2901);

  ui::AXNodeData slider;
  slider.id = 2901;
  slider.role = ax::mojom::Role::kSlider;
  slider.AddStringAttribute(ax::mojom::StringAttribute::kHtmlTag, "input");
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kMinValueForRange, 0.0f);
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kMaxValueForRange, 10.0f);
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kValueForRange, 10.0f);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, slider);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* s = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2901));
  ASSERT_NE(s, nullptr);
  EXPECT_FALSE(s->CanScrollForward());

  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kValueForRange, 0.0f);

  ui::AXTreeUpdate update2 = ui::MakeAXTreeUpdateForTesting(root, slider);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager2 = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update2, node_id_delegate_, test_browser_accessibility_delegate_.get());
  s = static_cast<ui::BrowserAccessibilityOHOS*>(manager2->GetFromID(2901));
  EXPECT_FALSE(s->CanScrollBackward());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_ValueExposureAndTargetUrl) {
  ui::AXNodeData root;
  root.id = 200;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(201);
  root.child_ids.push_back(202);

  ui::AXNodeData textfield;
  textfield.id = 201;
  textfield.role = ax::mojom::Role::kTextField;
  textfield.SetValue("Typed Value");

  ui::AXNodeData link;
  link.id = 202;
  link.role = ax::mojom::Role::kLink;
  link.AddStringAttribute(ax::mojom::StringAttribute::kUrl, "https://example.com");

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, textfield, link);

  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* text_node = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(201));
  ASSERT_NE(text_node, nullptr);
  EXPECT_TRUE(text_node->ShouldExposeValueAsName());

  auto* link_node = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(202));
  ASSERT_NE(link_node, nullptr);
  EXPECT_EQ(link_node->GetTargetUrl(), "https://example.com");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetTargetUrl_Branches) {
  ui::AXNodeData root;
  root.id = 5600;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(5601);
  root.child_ids.push_back(5602);
  root.child_ids.push_back(5603);

  ui::AXNodeData image;
  image.id = 5601;
  image.role = ax::mojom::Role::kImage;
  image.AddStringAttribute(ax::mojom::StringAttribute::kUrl, "https://example.com/image.jpg");

  ui::AXNodeData link;
  link.id = 5602;
  link.role = ax::mojom::Role::kLink;
  link.AddStringAttribute(ax::mojom::StringAttribute::kUrl, "https://example.com/page");

  ui::AXNodeData video;
  video.id = 5603;
  video.role = ax::mojom::Role::kVideo;
  video.AddStringAttribute(ax::mojom::StringAttribute::kUrl, "https://example.com/video.mp4");

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, image, link, video);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* img = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(5601));
  auto* lnk = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(5602));
  auto* vid = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(5603));

  ASSERT_NE(img, nullptr);
  ASSERT_NE(lnk, nullptr);
  ASSERT_NE(vid, nullptr);

  EXPECT_EQ(img->GetTargetUrl(), "https://example.com/image.jpg");
  EXPECT_EQ(lnk->GetTargetUrl(), "https://example.com/page");
  EXPECT_EQ(vid->GetTargetUrl(), "https://example.com/video.mp4");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetTargetUrl_NonMediaLink) {
  ui::AXNodeData root;
  root.id = 5700;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(5701);

  ui::AXNodeData button;
  button.id = 5701;
  button.role = ax::mojom::Role::kButton;
  button.AddStringAttribute(ax::mojom::StringAttribute::kUrl, "https://example.com/button");

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, button);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* btn = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(5701));
  ASSERT_NE(btn, nullptr);

  auto url = btn->GetTargetUrl();
  EXPECT_EQ(url, std::string());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetTargetUrl_LinkAndNonLink) {
  // Link with URL
  {
    ui::AXNodeData root;
    root.id = 8600;
    root.role = ax::mojom::Role::kRootWebArea;
    root.child_ids.push_back(8601);

    ui::AXNodeData link;
    link.id = 8601;
    link.role = ax::mojom::Role::kLink;
    link.AddStringAttribute(ax::mojom::StringAttribute::kUrl, "https://example.com/a");

    ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, link);
    std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
        update, node_id_delegate_, test_browser_accessibility_delegate_.get());
    auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(8601));
    ASSERT_NE(n, nullptr);
    EXPECT_EQ(n->GetTargetUrl(), std::string("https://example.com/a"));
  }

  // Non-link returns empty
  {
    ui::AXNodeData root;
    root.id = 8610;
    root.role = ax::mojom::Role::kRootWebArea;
    root.child_ids.push_back(8611);

    ui::AXNodeData btn;
    btn.id = 8611;
    btn.role = ax::mojom::Role::kButton;
    btn.AddStringAttribute(ax::mojom::StringAttribute::kUrl, "https://example.com/img");

    ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, btn);
    std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
        update, node_id_delegate_, test_browser_accessibility_delegate_.get());
    auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(8611));
    ASSERT_NE(n, nullptr);
    EXPECT_EQ(n->GetTargetUrl(), std::string());
  }
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetTargetUrl_VideoAndImage) {
  ui::AXNodeData root;
  root.id = 9850;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(9851);
  root.child_ids.push_back(9852);

  ui::AXNodeData img;
  img.id = 9851;
  img.role = ax::mojom::Role::kImage;
  img.AddStringAttribute(ax::mojom::StringAttribute::kUrl, "https://a/img.png");

  ui::AXNodeData video;
  video.id = 9852;
  video.role = ax::mojom::Role::kVideo;
  video.AddStringAttribute(ax::mojom::StringAttribute::kUrl, "https://a/v.mp4");

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, img, video);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n_img = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(9851));
  auto* n_vid = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(9852));
  ASSERT_NE(n_img, nullptr);
  ASSERT_NE(n_vid, nullptr);
  EXPECT_EQ(n_img->GetTargetUrl(), std::string("https://a/img.png"));
  EXPECT_EQ(n_vid->GetTargetUrl(), std::string("https://a/v.mp4"));
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_ColorWellTextAndRangeSupport) {
  ui::AXNodeData root;
  root.id = 300;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(301);
  root.child_ids.push_back(302);

  ui::AXNodeData colorwell;
  colorwell.id = 301;
  colorwell.role = ax::mojom::Role::kColorWell;
  colorwell.AddIntAttribute(ax::mojom::IntAttribute::kColorValue, 0xFF0000);

  ui::AXNodeData slider;
  slider.id = 302;
  slider.role = ax::mojom::Role::kSlider;
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kValueForRange, 1.0f);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, colorwell, slider);

  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* cw = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(301));
  ASSERT_NE(cw, nullptr);
  auto text16 = cw->GetSubstringTextContentUTF16(absl::nullopt);

  ASSERT_GE(text16.size(), static_cast<size_t>(7));
  EXPECT_EQ(text16[0], u'#');

  auto* sl = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(302));
  ASSERT_NE(sl, nullptr);
  EXPECT_TRUE(sl->GetData().IsRangeValueSupported());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_InputTypeAndLiveRegion) {
  ui::AXNodeData root;
  root.id = 600;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(601);
  root.child_ids.push_back(602);

  ui::AXNodeData input;
  input.id = 601;
  input.role = ax::mojom::Role::kTextField;
  input.AddStringAttribute(ax::mojom::StringAttribute::kHtmlTag, "input");
  input.AddStringAttribute(ax::mojom::StringAttribute::kInputType, "email");

  ui::AXNodeData live;
  live.id = 602;
  live.role = ax::mojom::Role::kGenericContainer;
  live.AddStringAttribute(ax::mojom::StringAttribute::kLiveStatus, "assertive");

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, input, live);

  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* in = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(601));
  ASSERT_NE(in, nullptr);
  EXPECT_EQ(in->OHOSInputType(), static_cast<int32_t>(OHOS::NWeb::AceTextCategory::INPUT_TYPE_EMAIL));

  auto* lr = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(602));
  ASSERT_NE(lr, nullptr);
  EXPECT_EQ(lr->OHOSLiveRegionType(), 2);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_InputTypeAllAndLiveRegionAll) {
  ui::AXNodeData root;
  root.id = 2300;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(2301);
  root.child_ids.push_back(2302);

  ui::AXNodeData input;
  input.id = 2301;
  input.role = ax::mojom::Role::kTextField;
  input.AddStringAttribute(ax::mojom::StringAttribute::kHtmlTag, "input");
  input.AddStringAttribute(ax::mojom::StringAttribute::kInputType, "text");

  ui::AXNodeData live;
  live.id = 2302;
  live.role = ax::mojom::Role::kGenericContainer;
  live.AddStringAttribute(ax::mojom::StringAttribute::kLiveStatus, "polite");

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, input, live);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* in = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2301));
  auto* lr = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2302));
  ASSERT_NE(in, nullptr);
  ASSERT_NE(lr, nullptr);

  struct Case {
    const char* type;
    int32_t expect;
  } cases[] = {
      {"", static_cast<int32_t>(OHOS::NWeb::AceTextCategory::INPUT_TYPE_TEXT)},
      {"text", static_cast<int32_t>(OHOS::NWeb::AceTextCategory::INPUT_TYPE_TEXT)},
      {"search", static_cast<int32_t>(OHOS::NWeb::AceTextCategory::INPUT_TYPE_TEXT)},
      {"date", static_cast<int32_t>(OHOS::NWeb::AceTextCategory::INPUT_TYPE_DATE)},
      {"datetime", static_cast<int32_t>(OHOS::NWeb::AceTextCategory::INPUT_TYPE_TIME)},
      {"datetime-local", static_cast<int32_t>(OHOS::NWeb::AceTextCategory::INPUT_TYPE_TIME)},
      {"email", static_cast<int32_t>(OHOS::NWeb::AceTextCategory::INPUT_TYPE_EMAIL)},
      {"month", static_cast<int32_t>(OHOS::NWeb::AceTextCategory::INPUT_TYPE_DATE)},
      {"number", static_cast<int32_t>(OHOS::NWeb::AceTextCategory::INPUT_TYPE_NUMBER)},
      {"password", static_cast<int32_t>(OHOS::NWeb::AceTextCategory::INPUT_TYPE_PASSWORD)},
      {"tel", static_cast<int32_t>(OHOS::NWeb::AceTextCategory::INPUT_TYPE_PHONENUMBER)},
      {"time", static_cast<int32_t>(OHOS::NWeb::AceTextCategory::INPUT_TYPE_TIME)},
      {"url", static_cast<int32_t>(OHOS::NWeb::AceTextCategory::INPUT_TYPE_DEFAULT)},
      {"week", static_cast<int32_t>(OHOS::NWeb::AceTextCategory::INPUT_TYPE_DATE)},
  };

  for (const auto& c : cases) {
    ui::AXNodeData upd = input;
    upd.AddStringAttribute(ax::mojom::StringAttribute::kInputType, c.type);

    ui::AXTreeUpdate upd_case = ui::MakeAXTreeUpdateForTesting(root, upd, live);
    std::unique_ptr<ui::BrowserAccessibilityManager> m_case = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
        upd_case, node_id_delegate_, test_browser_accessibility_delegate_.get());
    auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(m_case->GetFromID(2301));
    ASSERT_NE(n, nullptr);
    EXPECT_EQ(n->OHOSInputType(), c.expect);
  }

  EXPECT_EQ(lr->OHOSLiveRegionType(), 1);
  ui::AXNodeData upd_live = live;
  upd_live.AddStringAttribute(ax::mojom::StringAttribute::kLiveStatus, "assertive");

  upd_live.AddStringAttribute(ax::mojom::StringAttribute::kLiveStatus, "assertive");
  ui::AXTreeUpdate upd1 = ui::MakeAXTreeUpdateForTesting(root, input, upd_live);
  std::unique_ptr<ui::BrowserAccessibilityManager> m1 = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      upd1, node_id_delegate_, test_browser_accessibility_delegate_.get());
  lr = static_cast<ui::BrowserAccessibilityOHOS*>(m1->GetFromID(2302));
  ASSERT_NE(lr, nullptr);
  EXPECT_EQ(lr->OHOSLiveRegionType(), 2);

  upd_live.AddStringAttribute(ax::mojom::StringAttribute::kLiveStatus, "off");
  ui::AXTreeUpdate upd2 = ui::MakeAXTreeUpdateForTesting(root, input, upd_live);
  std::unique_ptr<ui::BrowserAccessibilityManager> m2 = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      upd2, node_id_delegate_, test_browser_accessibility_delegate_.get());
  lr = static_cast<ui::BrowserAccessibilityOHOS*>(m2->GetFromID(2302));
  ASSERT_NE(lr, nullptr);
  EXPECT_EQ(lr->OHOSLiveRegionType(), 0);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_InputType_NonInputTag) {
  ui::AXNodeData root;
  root.id = 2999;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(3000);

  ui::AXNodeData node;
  node.id = 3000;
  node.role = ax::mojom::Role::kTextField;
  node.AddStringAttribute(ax::mojom::StringAttribute::kHtmlTag, "div");
  node.AddStringAttribute(ax::mojom::StringAttribute::kInputType, "password");

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, node);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(3000));
  ASSERT_NE(n, nullptr);
  EXPECT_EQ(n->OHOSInputType(), static_cast<int32_t>(OHOS::NWeb::AceTextCategory::INPUT_TYPE_DEFAULT));
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_RoleDescriptionVariants) {
  ui::AXNodeData root;
  root.id = 700;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(701);
  root.child_ids.push_back(702);
  root.child_ids.push_back(703);

  ui::AXNodeData heading;
  heading.id = 701;
  heading.role = ax::mojom::Role::kHeading;
  heading.SetName("H");

  ui::AXNodeData link;
  link.id = 702;
  link.role = ax::mojom::Role::kLink;
  link.AddState(ax::mojom::State::kVisited);

  ui::AXNodeData button;
  button.id = 703;
  button.role = ax::mojom::Role::kButton;
  button.AddIntAttribute(ax::mojom::IntAttribute::kHasPopup, static_cast<int>(ax::mojom::HasPopup::kMenu));

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, heading, link, button);

  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* h = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(701));
  auto* l = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(702));
  auto* b = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(703));
  ASSERT_NE(h, nullptr);
  ASSERT_NE(l, nullptr);
  ASSERT_NE(b, nullptr);

  auto h_desc = h->GetRoleDescription();
  EXPECT_EQ(h_desc, u"Heading");
  auto l_desc = l->GetRoleDescription();

  EXPECT_EQ(l_desc, u" Visited");
  auto b_desc = b->GetRoleDescription();
  EXPECT_EQ(b_desc, u"Popup button menu");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_RoleDescription_MoreRoles) {
  ui::AXNodeData root;
  root.id = 3200;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(3201);
  root.child_ids.push_back(3202);
  root.child_ids.push_back(3203);
  root.child_ids.push_back(3204);
  root.child_ids.push_back(3205);

  ui::AXNodeData canvas;
  canvas.id = 3201;
  canvas.role = ax::mojom::Role::kCanvas;

  ui::AXNodeData header;
  header.id = 3202;
  header.role = ax::mojom::Role::kHeader;

  ui::AXNodeData listgrid;
  listgrid.id = 3203;
  listgrid.role = ax::mojom::Role::kListGrid;

  ui::AXNodeData mir;
  mir.id = 3204;
  mir.role = ax::mojom::Role::kMenuItemRadio;

  ui::AXNodeData rb;
  rb.id = 3205;
  rb.role = ax::mojom::Role::kRadioButton;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, canvas, header, listgrid, mir, rb);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  int ids[] = {3201, 3202, 3203, 3204, 3205};
  for (int id : ids) {
    auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(id));
    ASSERT_NE(n, nullptr);
    auto desc = n->GetRoleDescription();
    EXPECT_FALSE(desc.empty());
  }
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_RoleDescription_AriaRoleDescription) {
  ui::AXNodeData root;
  root.id = 3300;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData button;
  button.id = 3301;
  button.role = ax::mojom::Role::kButton;
  button.AddStringAttribute(ax::mojom::StringAttribute::kRoleDescription, "Custom Button");

  root.child_ids = {3301};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, button);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(3301));
  ASSERT_NE(n, nullptr);
  auto desc = n->GetRoleDescription();
  EXPECT_EQ(desc, u"Custom Button");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_RoleDescription_HeadingNoVisited) {
  ui::AXNodeData root;
  root.id = 3400;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(3401);

  ui::AXNodeData heading;
  heading.id = 3401;
  heading.role = ax::mojom::Role::kHeading;
  heading.SetName("Test Heading");

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, heading);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(3401));
  ASSERT_NE(n, nullptr);
  auto desc = n->GetRoleDescription();
  EXPECT_EQ(desc, u"Heading");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_RoleDescription_HeadingWithVisited) {
  ui::AXNodeData root;
  root.id = 3500;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(3501);

  ui::AXNodeData heading;
  heading.id = 3501;
  heading.role = ax::mojom::Role::kHeading;
  heading.SetName("Test Heading");
  heading.AddState(ax::mojom::State::kVisited);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, heading);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(3501));
  ASSERT_NE(n, nullptr);
  auto desc = n->GetRoleDescription();
  EXPECT_EQ(desc, u"Heading Visited");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_RoleDescription_LinkNoVisited) {
  ui::AXNodeData root;
  root.id = 3600;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(3601);

  ui::AXNodeData link;
  link.id = 3601;
  link.role = ax::mojom::Role::kLink;
  link.SetName("Test Link");

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, link);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(3601));
  ASSERT_NE(n, nullptr);
  auto desc = n->GetRoleDescription();
  EXPECT_EQ(desc, std::u16string());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_RoleDescription_ButtonNoPopup) {
  ui::AXNodeData root;
  root.id = 3700;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(3701);

  ui::AXNodeData button;
  button.id = 3701;
  button.role = ax::mojom::Role::kButton;
  button.SetName("Test Button");

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, button);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(3701));
  ASSERT_NE(n, nullptr);
  auto desc = n->GetRoleDescription();
  EXPECT_EQ(desc, std::u16string());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_RoleDescription_ButtonPopupTypes) {
  ui::AXNodeData root;
  root.id = 3800;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(3801);
  root.child_ids.push_back(3802);
  root.child_ids.push_back(3803);
  root.child_ids.push_back(3804);

  ui::AXNodeData button1;
  button1.id = 3801;
  button1.role = ax::mojom::Role::kButton;
  button1.AddIntAttribute(ax::mojom::IntAttribute::kHasPopup, static_cast<int>(ax::mojom::HasPopup::kTrue));

  ui::AXNodeData button2;
  button2.id = 3802;
  button2.role = ax::mojom::Role::kButton;
  button2.AddIntAttribute(ax::mojom::IntAttribute::kHasPopup, static_cast<int>(ax::mojom::HasPopup::kDialog));

  ui::AXNodeData button3;
  button3.id = 3803;
  button3.role = ax::mojom::Role::kButton;
  button3.AddIntAttribute(ax::mojom::IntAttribute::kHasPopup, static_cast<int>(ax::mojom::HasPopup::kListbox));

  ui::AXNodeData button4;
  button4.id = 3804;
  button4.role = ax::mojom::Role::kButton;
  button4.AddIntAttribute(ax::mojom::IntAttribute::kHasPopup, static_cast<int>(ax::mojom::HasPopup::kFalse));

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, button1, button2, button3, button4);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* b1 = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(3801));
  auto* b2 = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(3802));
  auto* b3 = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(3803));
  auto* b4 = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(3804));

  ASSERT_NE(b1, nullptr);
  ASSERT_NE(b2, nullptr);
  ASSERT_NE(b3, nullptr);
  ASSERT_NE(b4, nullptr);

  auto desc1 = b1->GetRoleDescription();
  auto desc2 = b2->GetRoleDescription();
  auto desc3 = b3->GetRoleDescription();
  auto desc4 = b4->GetRoleDescription();

  EXPECT_EQ(desc1, u"Popup button menu");
  EXPECT_EQ(desc2, u"Popup button dialog");
  EXPECT_EQ(desc3, u"Popup button");
  EXPECT_EQ(desc4, std::u16string());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_RoleDescription_VariousRoles) {
  ui::AXNodeData root;
  root.id = 3900;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(3901);
  root.child_ids.push_back(3902);
  root.child_ids.push_back(3903);
  root.child_ids.push_back(3904);
  root.child_ids.push_back(3905);
  root.child_ids.push_back(3906);
  root.child_ids.push_back(3907);
  root.child_ids.push_back(3908);

  ui::AXNodeData alert;
  alert.id = 3901;
  alert.role = ax::mojom::Role::kAlert;

  ui::AXNodeData audio;
  audio.id = 3902;
  audio.role = ax::mojom::Role::kAudio;

  ui::AXNodeData code;
  code.id = 3903;
  code.role = ax::mojom::Role::kCode;

  ui::AXNodeData details;
  details.id = 3904;
  details.role = ax::mojom::Role::kDetails;

  ui::AXNodeData emphasis;
  emphasis.id = 3905;
  emphasis.role = ax::mojom::Role::kEmphasis;

  ui::AXNodeData form;
  form.id = 3906;
  form.role = ax::mojom::Role::kForm;

  ui::AXNodeData rowgroup;
  rowgroup.id = 3907;
  rowgroup.role = ax::mojom::Role::kRowGroup;

  ui::AXNodeData sectionfooter;
  sectionfooter.id = 3908;
  sectionfooter.role = ax::mojom::Role::kSectionFooter;

  ui::AXTreeUpdate update =
      ui::MakeAXTreeUpdateForTesting(root, alert, audio, code, details, emphasis, form, rowgroup, sectionfooter);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  int ids[] = {3901, 3902, 3903, 3904, 3905, 3906, 3907, 3908};
  for (int id : ids) {
    auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(id));
    ASSERT_NE(n, nullptr);
    auto desc = n->GetRoleDescription();

    EXPECT_EQ(desc, std::u16string());
  }
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_RoleDescription_MoreRoleTypes) {
  ui::AXNodeData root;
  root.id = 4000;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(4001);
  root.child_ids.push_back(4002);
  root.child_ids.push_back(4003);
  root.child_ids.push_back(4004);
  root.child_ids.push_back(4005);
  root.child_ids.push_back(4006);
  root.child_ids.push_back(4007);
  root.child_ids.push_back(4008);

  ui::AXNodeData sectionheader;
  sectionheader.id = 4001;
  sectionheader.role = ax::mojom::Role::kSectionHeader;

  ui::AXNodeData sectionwithoutname;
  sectionwithoutname.id = 4002;
  sectionwithoutname.role = ax::mojom::Role::kSectionWithoutName;

  ui::AXNodeData strong;
  strong.id = 4003;
  strong.role = ax::mojom::Role::kStrong;

  ui::AXNodeData subscript;
  subscript.id = 4004;
  subscript.role = ax::mojom::Role::kSubscript;

  ui::AXNodeData superscript;
  superscript.id = 4005;
  superscript.role = ax::mojom::Role::kSuperscript;

  ui::AXNodeData time;
  time.id = 4006;
  time.role = ax::mojom::Role::kTime;

  ui::AXNodeData combobox;
  combobox.id = 4007;
  combobox.role = ax::mojom::Role::kComboBoxSelect;

  ui::AXNodeData figure;
  figure.id = 4008;
  figure.role = ax::mojom::Role::kFigure;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, sectionheader, sectionwithoutname, strong, subscript,
                                                           superscript, time, combobox, figure);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  int no_desc_ids[] = {4001, 4002, 4003, 4004, 4005, 4006};
  for (int id : no_desc_ids) {
    auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(id));
    ASSERT_NE(n, nullptr);
    auto desc = n->GetRoleDescription();
    EXPECT_TRUE(desc.empty());
  }

  auto* cb = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(4007));
  auto* fig = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(4008));
  ASSERT_NE(cb, nullptr);
  ASSERT_NE(fig, nullptr);

  auto cb_desc = cb->GetRoleDescription();
  auto fig_desc = fig->GetRoleDescription();
  EXPECT_EQ(cb_desc, u"Combo box");
  EXPECT_EQ(fig_desc, u"Graphic");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_RoleDescription_DefaultBranch) {
  ui::AXNodeData root;
  root.id = 4100;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(4101);

  ui::AXNodeData generic;
  generic.id = 4101;
  generic.role = ax::mojom::Role::kGenericContainer;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, generic);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(4101));
  ASSERT_NE(n, nullptr);
  auto desc = n->GetRoleDescription();
  EXPECT_EQ(desc, std::u16string());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetRoleDescription_ButtonPopupDetailed) {
  ui::AXNodeData root;
  root.id = 7800;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(7801);
  root.child_ids.push_back(7802);
  root.child_ids.push_back(7803);

  ui::AXNodeData menu_btn;
  menu_btn.id = 7801;
  menu_btn.role = ax::mojom::Role::kButton;
  menu_btn.AddIntAttribute(ax::mojom::IntAttribute::kHasPopup, static_cast<int>(ax::mojom::HasPopup::kMenu));

  ui::AXNodeData dlg_btn;
  dlg_btn.id = 7802;
  dlg_btn.role = ax::mojom::Role::kButton;
  dlg_btn.AddIntAttribute(ax::mojom::IntAttribute::kHasPopup, static_cast<int>(ax::mojom::HasPopup::kDialog));

  ui::AXNodeData list_btn;
  list_btn.id = 7803;
  list_btn.role = ax::mojom::Role::kButton;
  list_btn.AddIntAttribute(ax::mojom::IntAttribute::kHasPopup, static_cast<int>(ax::mojom::HasPopup::kListbox));

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, menu_btn, dlg_btn, list_btn);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* m = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(7801));
  auto* d = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(7802));
  auto* l = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(7803));
  ASSERT_NE(m, nullptr);
  ASSERT_NE(d, nullptr);
  ASSERT_NE(l, nullptr);
  EXPECT_EQ(m->GetRoleDescription(), u"Popup button menu");
  EXPECT_EQ(d->GetRoleDescription(), u"Popup button dialog");
  EXPECT_EQ(l->GetRoleDescription(), u"Popup button");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_SubstringTextContent_Compose) {
  ui::AXNodeData root;
  root.id = 800;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(801);

  ui::AXNodeData tf;
  tf.id = 801;
  tf.role = ax::mojom::Role::kTextField;
  tf.SetValue("only-value");

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, tf);

  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* node = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(801));
  ASSERT_NE(node, nullptr);
  auto text16 = node->GetSubstringTextContentUTF16(absl::nullopt);
  EXPECT_EQ(text16, u"only-value");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_ImageAnnotationStatus_PendingAdultNoDescSucceeded) {
  // Root -> image; cover multiple statuses by re-instantiating trees
  auto make_root = [](int id_root, int id_img) {
    ui::AXNodeData root;
    root.id = id_root;
    root.role = ax::mojom::Role::kRootWebArea;
    root.child_ids.push_back(id_img);
    return root;
  };

  // Pending
  {
    ui::AXNodeData root = make_root(8100, 8101);
    ui::AXNodeData img;
    img.id = 8101;
    img.role = ax::mojom::Role::kImage;
    img.AddIntAttribute(ax::mojom::IntAttribute::kImageAnnotationStatus,
                        static_cast<int>(ax::mojom::ImageAnnotationStatus::kAnnotationPending));
    ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, img);
    std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
        update, node_id_delegate_, test_browser_accessibility_delegate_.get());
    auto* i = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(8101));
    ASSERT_NE(i, nullptr);
    auto text = i->GetSubstringTextContentUTF16(absl::nullopt);
    EXPECT_EQ(text, u"Getting description...");
  }

  // Adult
  {
    ui::AXNodeData root = make_root(8110, 8111);
    ui::AXNodeData img;
    img.id = 8111;
    img.role = ax::mojom::Role::kImage;
    img.AddIntAttribute(ax::mojom::IntAttribute::kImageAnnotationStatus,
                        static_cast<int>(ax::mojom::ImageAnnotationStatus::kAnnotationAdult));
    ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, img);
    std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
        update, node_id_delegate_, test_browser_accessibility_delegate_.get());
    auto* i = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(8111));
    ASSERT_NE(i, nullptr);
    auto text = i->GetSubstringTextContentUTF16(absl::nullopt);
    EXPECT_EQ(text, u"Appears to contain adult content. No description available.");
  }

  // No description (Empty)
  {
    ui::AXNodeData root = make_root(8120, 8121);
    ui::AXNodeData img;
    img.id = 8121;
    img.role = ax::mojom::Role::kImage;
    img.AddIntAttribute(ax::mojom::IntAttribute::kImageAnnotationStatus,
                        static_cast<int>(ax::mojom::ImageAnnotationStatus::kAnnotationEmpty));
    ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, img);
    std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
        update, node_id_delegate_, test_browser_accessibility_delegate_.get());
    auto* i = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(8121));
    ASSERT_NE(i, nullptr);
    auto text = i->GetSubstringTextContentUTF16(absl::nullopt);
    EXPECT_EQ(text, u"No description available.");
  }

  // Succeeded -> uses kImageAnnotation string
  {
    ui::AXNodeData root = make_root(8130, 8131);
    ui::AXNodeData img;
    img.id = 8131;
    img.role = ax::mojom::Role::kImage;
    img.AddIntAttribute(ax::mojom::IntAttribute::kImageAnnotationStatus,
                        static_cast<int>(ax::mojom::ImageAnnotationStatus::kAnnotationSucceeded));
    img.AddStringAttribute(ax::mojom::StringAttribute::kImageAnnotation, "ALT-TEXT");
    ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, img);
    std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
        update, node_id_delegate_, test_browser_accessibility_delegate_.get());
    auto* i = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(8131));
    ASSERT_NE(i, nullptr);
    auto text = i->GetSubstringTextContentUTF16(absl::nullopt);
    EXPECT_EQ(text, u"ALT-TEXT");
  }
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_DocumentSplitterAndUrlFallback) {
  ui::AXNodeData root;
  root.id = 1800;
  root.role = ax::mojom::Role::kRootWebArea;
  root.SetName("DocTitle");
  root.child_ids.push_back(1801);
  root.child_ids.push_back(1802);
  ui::AXNodeData splitter;
  splitter.id = 1801;
  splitter.role = ax::mojom::Role::kSplitter;
  splitter.SetName("Split");
  ui::AXNodeData link;
  link.id = 1802;
  link.role = ax::mojom::Role::kLink;
  link.AddStringAttribute(ax::mojom::StringAttribute::kUrl, "https://example.com/path?q=1");
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, splitter, link);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* r = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(1800));
  auto* s = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(1801));
  auto* l = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(1802));
  ASSERT_NE(r, nullptr);
  ASSERT_NE(s, nullptr);
  ASSERT_NE(l, nullptr);
  auto r_text = r->GetSubstringTextContentUTF16(absl::nullopt);
  EXPECT_EQ(r_text, std::u16string());
  auto s_text = s->GetSubstringTextContentUTF16(absl::nullopt);
  EXPECT_EQ(s_text, u"Split");
  auto l_text = l->GetSubstringTextContentUTF16(absl::nullopt);
  EXPECT_FALSE(l_text.empty());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_SubstringTextContent_RangeCompose) {
  ui::AXNodeData root;
  root.id = 2199;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(2200);
  ui::AXNodeData node;
  node.id = 2200;
  node.role = ax::mojom::Role::kSlider;
  node.SetName("Label");
  node.AddFloatAttribute(ax::mojom::FloatAttribute::kValueForRange, 3.0f);
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, node);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2200));
  ASSERT_NE(n, nullptr);
  auto text = n->GetSubstringTextContentUTF16(absl::nullopt);
  EXPECT_FALSE(text.empty());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSubstringTextContent_RootAndSplitterReturnOwnText) {
  ui::AXNodeData root;
  root.id = 7700;
  root.role = ax::mojom::Role::kRootWebArea;
  root.SetName("RootTitle");
  root.child_ids.push_back(7701);
  ui::AXNodeData splitter;
  splitter.id = 7701;
  splitter.role = ax::mojom::Role::kSplitter;
  splitter.SetName("Divider");
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, splitter);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* r = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(7700));
  auto* s = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(7701));
  ASSERT_NE(r, nullptr);
  ASSERT_NE(s, nullptr);
  EXPECT_EQ(r->GetSubstringTextContentUTF16(absl::nullopt), std::u16string());
  EXPECT_EQ(s->GetSubstringTextContentUTF16(absl::nullopt), u"Divider");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSubstringTextContentUTF16_IframeRole) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;
  ui::AXNodeData iframe;
  iframe.id = 2;
  iframe.role = ax::mojom::Role::kIframe;
  iframe.AddStringAttribute(ax::mojom::StringAttribute::kName, "Test iframe");
  root.child_ids = {2};
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, iframe);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);

  std::u16string result = n->GetSubstringTextContentUTF16(absl::nullopt);
  EXPECT_TRUE(result.empty());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSubstringTextContentUTF16_RootWebAreaRole) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;
  root.AddStringAttribute(ax::mojom::StringAttribute::kName, "Test root");
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(1));
  ASSERT_NE(n, nullptr);

  std::u16string result = n->GetSubstringTextContentUTF16(absl::nullopt);
  EXPECT_TRUE(result.empty());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSubstringTextContentUTF16_ColorWellRole) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;
  ui::AXNodeData color_well;
  color_well.id = 2;
  color_well.role = ax::mojom::Role::kColorWell;
  color_well.AddIntAttribute(ax::mojom::IntAttribute::kColorValue, 0xFF0000);
  root.child_ids = {2};
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, color_well);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  std::u16string result = n->GetSubstringTextContentUTF16(absl::nullopt);
  EXPECT_FALSE(result.empty());
  EXPECT_TRUE(result.find(u"#") != std::u16string::npos);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSubstringTextContentUTF16_RangeValueSupported) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;
  ui::AXNodeData slider;
  slider.id = 2;
  slider.role = ax::mojom::Role::kSlider;
  slider.AddStringAttribute(ax::mojom::StringAttribute::kName, "Volume");
  slider.AddStringAttribute(ax::mojom::StringAttribute::kValue, "50");
  root.child_ids = {2};
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, slider);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  std::u16string result = n->GetSubstringTextContentUTF16(absl::nullopt);
  EXPECT_FALSE(result.empty());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSubstringTextContentUTF16_RangeValueSupportedEmptyName) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;
  ui::AXNodeData slider;
  slider.id = 2;
  slider.role = ax::mojom::Role::kSlider;
  slider.AddStringAttribute(ax::mojom::StringAttribute::kValue, "75");
  root.child_ids = {2};
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, slider);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  std::u16string result = n->GetSubstringTextContentUTF16(absl::nullopt);
  EXPECT_FALSE(result.empty());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSubstringTextContentUTF16_EmptyNameUseValue) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;
  ui::AXNodeData text_field;
  text_field.id = 2;
  text_field.role = ax::mojom::Role::kTextField;
  text_field.AddStringAttribute(ax::mojom::StringAttribute::kValue, "input value");
  root.child_ids = {2};
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, text_field);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);

  std::u16string result = n->GetSubstringTextContentUTF16(absl::nullopt);
  EXPECT_FALSE(result.empty());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSubstringTextContentUTF16_PlatformDocumentRole) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;
  root.AddStringAttribute(ax::mojom::StringAttribute::kName, "Document title");

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(1));
  ASSERT_NE(n, nullptr);

  std::u16string result = n->GetSubstringTextContentUTF16(absl::nullopt);
  EXPECT_TRUE(result.empty());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSubstringTextContentUTF16_SplitterRole) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData splitter;
  splitter.id = 2;
  splitter.role = ax::mojom::Role::kSplitter;
  splitter.AddStringAttribute(ax::mojom::StringAttribute::kName, "Resize handle");

  root.child_ids = {2};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, splitter);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);

  std::u16string result = n->GetSubstringTextContentUTF16(absl::nullopt);
  EXPECT_FALSE(result.empty());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSubstringTextContentUTF16_ImageAnnotationSucceeded) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData image;
  image.id = 2;
  image.role = ax::mojom::Role::kImage;
  image.AddStringAttribute(ax::mojom::StringAttribute::kImageAnnotation, "A beautiful sunset");
  image.AddIntAttribute(ax::mojom::IntAttribute::kImageAnnotationStatus,
                        static_cast<int32_t>(ax::mojom::ImageAnnotationStatus::kAnnotationSucceeded));

  root.child_ids = {2};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, image);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);

  std::u16string result = n->GetSubstringTextContentUTF16(absl::nullopt);
  EXPECT_FALSE(result.empty());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSubstringTextContentUTF16_AlertRoleWithChildren) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData alert;
  alert.id = 2;
  alert.role = ax::mojom::Role::kAlert;

  ui::AXNodeData text;
  text.id = 3;
  text.role = ax::mojom::Role::kStaticText;
  text.AddStringAttribute(ax::mojom::StringAttribute::kName, "Alert message");

  root.child_ids = {2};
  alert.child_ids = {3};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, alert, text);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);

  std::u16string result = n->GetSubstringTextContentUTF16(absl::nullopt);
  EXPECT_FALSE(result.empty());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSubstringTextContentUTF16_WithPredicate) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData div;
  div.id = 2;
  div.role = ax::mojom::Role::kGenericContainer;

  ui::AXNodeData text1;
  text1.id = 3;
  text1.role = ax::mojom::Role::kStaticText;
  text1.AddStringAttribute(ax::mojom::StringAttribute::kName, "First");

  ui::AXNodeData text2;
  text2.id = 4;
  text2.role = ax::mojom::Role::kStaticText;
  text2.AddStringAttribute(ax::mojom::StringAttribute::kName, "Second");

  root.child_ids = {2};
  div.child_ids = {3, 4};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, div, text1, text2);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);

  auto predicate = ui::BrowserAccessibilityOHOS::NonEmptyPredicate();
  std::u16string result = n->GetSubstringTextContentUTF16(predicate);
  EXPECT_FALSE(result.empty());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_NonEmptyPredicate) {
  auto predicate = ui::BrowserAccessibilityOHOS::NonEmptyPredicate();
  EXPECT_FALSE(predicate.Run(u""));
  EXPECT_TRUE(predicate.Run(u"x"));
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSubstringTextContentUTF16_LinkRoleEmptyName) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData link;
  link.id = 2;
  link.role = ax::mojom::Role::kLink;
  link.AddStringAttribute(ax::mojom::StringAttribute::kUrl, "https://example.com");
  root.child_ids = {2};
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, link);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);

  std::u16string result = n->GetSubstringTextContentUTF16(absl::nullopt);
  EXPECT_FALSE(result.empty());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSubstringTextContentUTF16_ImageRoleEmptyName) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData image;
  image.id = 2;
  image.role = ax::mojom::Role::kImage;
  image.AddStringAttribute(ax::mojom::StringAttribute::kUrl, "https://example.com/image.jpg");
  root.child_ids = {2};
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, image);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);

  std::u16string result = n->GetSubstringTextContentUTF16(absl::nullopt);
  EXPECT_FALSE(result.empty());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetFromAccessibilityId_ByHtmlId) {
  ui::AXNodeData root;
  root.id = 7300;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(7301);

  ui::AXNodeData embed;
  embed.id = 7301;
  embed.role = ax::mojom::Role::kEmbeddedObject;
  embed.html_attributes.emplace_back("id", "mapId");

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, embed);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* e = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(7301));
  ASSERT_NE(e, nullptr);
  int64_t mapped = ui::BrowserAccessibilityOHOS::GetAccessibilityIdByHtmlElementId("mapId");
  ASSERT_NE(mapped, -1);
  auto* back = ui::BrowserAccessibilityOHOS::GetFromAccessibilityId(mapped);
  ASSERT_NE(back, nullptr);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_AccessibilityIdMapping_EmbeddedObject) {
  ui::AXNodeData root;
  root.id = 6800;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(6801);

  ui::AXNodeData embed;
  embed.id = 6801;
  embed.role = ax::mojom::Role::kEmbeddedObject;
  embed.html_attributes.emplace_back("id", "embed123");

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, embed);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* e = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(6801));
  ASSERT_NE(e, nullptr);
  int64_t mapped = ui::BrowserAccessibilityOHOS::GetAccessibilityIdByHtmlElementId("embed123");
  EXPECT_NE(mapped, -1);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_SelectionAndItemCount) {
  ui::AXNodeData root;
  root.id = 500;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(501);
  root.child_ids.push_back(502);

  ui::AXNodeData textfield;
  textfield.id = 501;
  textfield.role = ax::mojom::Role::kTextField;
  textfield.AddIntAttribute(ax::mojom::IntAttribute::kTextSelStart, 3);
  textfield.AddIntAttribute(ax::mojom::IntAttribute::kTextSelEnd, 7);

  ui::AXNodeData list;
  list.id = 502;
  list.role = ax::mojom::Role::kList;
  list.AddIntAttribute(ax::mojom::IntAttribute::kSetSize, 4);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, textfield, list);

  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* tf = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(501));
  ASSERT_NE(tf, nullptr);
  EXPECT_EQ(tf->GetSelectionStart(), 3);
  EXPECT_EQ(tf->GetSelectionEnd(), 7);

  auto* ls = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(502));
  ASSERT_NE(ls, nullptr);
  EXPECT_TRUE(ls->IsCollection());
  EXPECT_EQ(ls->GetItemCount(), static_cast<size_t>(4));
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSelectionStart_NonAtomicReturnsZero) {
  ui::AXNodeData root;
  root.id = 9800;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(9801);

  ui::AXNodeData static_text;
  static_text.id = 9801;
  static_text.role = ax::mojom::Role::kStaticText;
  static_text.AddIntAttribute(ax::mojom::IntAttribute::kTextSelStart, 4);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, static_text);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(9801));
  ASSERT_NE(n, nullptr);
  EXPECT_EQ(n->GetSelectionStart(), 0);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSelectionStart_AtomicTextField) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData text_field;
  text_field.id = 2;
  text_field.role = ax::mojom::Role::kTextField;
  text_field.AddIntAttribute(ax::mojom::IntAttribute::kTextSelStart, 5);

  root.child_ids = {2};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, text_field);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  EXPECT_EQ(n->GetSelectionStart(), 5);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSelectionStart_NonAtomicTextFieldNullAnchor) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData text_field;
  text_field.id = 2;
  text_field.role = ax::mojom::Role::kStaticText;
  root.child_ids = {2};
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, text_field);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  EXPECT_EQ(n->GetSelectionStart(), 0);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSelectionEnd_NonAtomicReturnsZero) {
  ui::AXNodeData root;
  root.id = 9300;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(9301);

  ui::AXNodeData static_text;
  static_text.id = 9301;
  static_text.role = ax::mojom::Role::kStaticText;
  static_text.AddIntAttribute(ax::mojom::IntAttribute::kTextSelEnd, 9);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, static_text);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(9301));
  ASSERT_NE(n, nullptr);
  EXPECT_EQ(n->GetSelectionEnd(), 0);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSelectionEnd_AtomicTextField) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData text_field;
  text_field.id = 2;
  text_field.role = ax::mojom::Role::kTextField;
  text_field.AddIntAttribute(ax::mojom::IntAttribute::kTextSelEnd, 10);
  root.child_ids = {2};
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, text_field);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  EXPECT_EQ(n->GetSelectionEnd(), 10);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSelectionEnd_NonAtomicTextField) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData text_field;
  text_field.id = 2;
  text_field.role = ax::mojom::Role::kStaticText;

  root.child_ids = {2};
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, text_field);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  EXPECT_EQ(n->GetSelectionEnd(), 0);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsCollection_DirectoryDeprecatedTrue) {
  ui::AXNodeData root;
  root.id = 9760;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(9761);

  ui::AXNodeData dir;
  dir.id = 9761;
  dir.role = ax::mojom::Role::kDirectoryDeprecated;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, dir);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(9761));
  ASSERT_NE(n, nullptr);
  EXPECT_TRUE(n->IsCollection());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_RangeControlWithoutAriaValueText_Count100) {
  ui::AXNodeData root;
  root.id = 900;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(901);

  ui::AXNodeData slider;
  slider.id = 901;
  slider.role = ax::mojom::Role::kSlider;

  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kValueForRange, 10.0f);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, slider);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* node = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(901));
  ASSERT_NE(node, nullptr);
  EXPECT_TRUE(node->IsRangeControlWithoutAriaValueText());
  EXPECT_EQ(node->GetItemCount(), static_cast<size_t>(100));
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_TableMetrics) {
  ui::AXNodeData root;
  root.id = 1000;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(1001);
  root.child_ids.push_back(1002);

  ui::AXNodeData table;
  table.id = 1001;
  table.role = ax::mojom::Role::kTable;
  table.AddIntAttribute(ax::mojom::IntAttribute::kTableRowCount, 5);

  ui::AXNodeData cell;
  cell.id = 1002;
  cell.role = ax::mojom::Role::kCell;
  cell.AddIntAttribute(ax::mojom::IntAttribute::kTableCellRowIndex, 2);
  cell.AddIntAttribute(ax::mojom::IntAttribute::kTableCellRowSpan, 2);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, table, cell);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* t = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(1001));
  auto* c = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(1002));
  ASSERT_NE(t, nullptr);
  ASSERT_NE(c, nullptr);

  EXPECT_EQ(t->RowCount(), 5);

  EXPECT_EQ(t->ColumnCount(), 0);
  EXPECT_TRUE(c->IsCollectionItem());
  EXPECT_EQ(c->RowIndex(), 0);

  EXPECT_EQ(c->ColumnIndex(), 0);
  EXPECT_EQ(c->RowSpan(), 2);
  EXPECT_EQ(c->ColumnSpan(), 1);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_RowCount_FromSetSizeOnList) {
  ui::AXNodeData root;
  root.id = 7500;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(7501);

  ui::AXNodeData list;
  list.id = 7501;
  list.role = ax::mojom::Role::kList;
  list.AddIntAttribute(ax::mojom::IntAttribute::kSetSize, 7);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, list);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(7501));
  ASSERT_NE(n, nullptr);
  EXPECT_EQ(n->RowCount(), 7);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_RowCount_NonCollectionIsZero) {
  ui::AXNodeData root;
  root.id = 9200;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(9201);

  ui::AXNodeData button;
  button.id = 9201;
  button.role = ax::mojom::Role::kButton;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, button);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(9201));
  ASSERT_NE(n, nullptr);
  EXPECT_EQ(n->RowCount(), 0);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_ColumnCount_FromTableAttribute) {
  ui::AXNodeData root;
  root.id = 7100;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(7101);

  ui::AXNodeData table;
  table.id = 7101;
  table.role = ax::mojom::Role::kTable;
  table.AddIntAttribute(ax::mojom::IntAttribute::kTableColumnCount, 3);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, table);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* t = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(7101));
  ASSERT_NE(t, nullptr);
  EXPECT_EQ(t->ColumnCount(), 3);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsCollectionItem_RowHeaderTrue) {
  ui::AXNodeData root;
  root.id = 9400;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(9401);

  ui::AXNodeData row_header;
  row_header.id = 9401;
  row_header.role = ax::mojom::Role::kRowHeader;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, row_header);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(9401));
  ASSERT_NE(n, nullptr);
  EXPECT_TRUE(n->IsCollectionItem());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsCollectionItem_ColumnHeaderTrue) {
  ui::AXNodeData root;
  root.id = 9800;
  root.role = ax::mojom::Role::kTable;
  root.child_ids.push_back(9801);

  ui::AXNodeData col_header;
  col_header.id = 9801;
  col_header.role = ax::mojom::Role::kColumnHeader;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, col_header);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(9801));
  ASSERT_NE(n, nullptr);
  EXPECT_TRUE(n->IsCollectionItem());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_TableMetrics_DefaultsWhenMissing) {
  ui::AXNodeData root;
  root.id = 8900;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(8901);

  ui::AXNodeData cell;
  cell.id = 8901;
  cell.role = ax::mojom::Role::kCell;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, cell);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* c = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(8901));
  ASSERT_NE(c, nullptr);

  EXPECT_EQ(c->RowIndex(), 0);
  EXPECT_EQ(c->ColumnIndex(), 0);
  EXPECT_EQ(c->RowSpan(), 1);
  EXPECT_EQ(c->ColumnSpan(), 1);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_RowIndex_PosInSetEdgeCases) {
  ui::AXNodeData root;
  root.id = 9100;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(9101);

  ui::AXNodeData cell;
  cell.id = 9101;
  cell.role = ax::mojom::Role::kCell;

  cell.AddIntAttribute(ax::mojom::IntAttribute::kPosInSet, 1);

  ui::AXTreeUpdate update1 = ui::MakeAXTreeUpdateForTesting(root, cell);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager1 = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update1, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* c1 = static_cast<ui::BrowserAccessibilityOHOS*>(manager1->GetFromID(9101));
  ASSERT_NE(c1, nullptr);
  EXPECT_EQ(c1->RowIndex(), 0);

  ui::AXNodeData cell2 = cell;
  cell2.AddIntAttribute(ax::mojom::IntAttribute::kPosInSet, 0);
  cell2.AddIntAttribute(ax::mojom::IntAttribute::kTableCellRowIndex, 5);

  ui::AXTreeUpdate update2 = ui::MakeAXTreeUpdateForTesting(root, cell2);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager2 = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update2, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* c2 = static_cast<ui::BrowserAccessibilityOHOS*>(manager2->GetFromID(9101));
  ASSERT_NE(c2, nullptr);
  EXPECT_EQ(c2->RowIndex(), 0);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_RowIndex_UsesPosInSet) {
  ui::AXNodeData root;
  root.id = 6400;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(6401);

  ui::AXNodeData item;
  item.id = 6401;
  item.role = ax::mojom::Role::kListItem;
  item.AddIntAttribute(ax::mojom::IntAttribute::kPosInSet, 3);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, item);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(6401));
  ASSERT_NE(n, nullptr);
  EXPECT_EQ(n->RowIndex(), 0);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_ColumnSpan_DefaultZero) {
  ui::AXNodeData root;
  root.id = 6500;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(6501);

  ui::AXNodeData cell;
  cell.id = 6501;
  cell.role = ax::mojom::Role::kCell;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, cell);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* c = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(6501));
  ASSERT_NE(c, nullptr);
  EXPECT_EQ(c->ColumnSpan(), 1);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_HeadingRecursion) {
  ui::AXNodeData root;
  root.id = 1099;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(1100);

  ui::AXNodeData heading;
  heading.id = 1100;
  heading.role = ax::mojom::Role::kHeading;
  heading.SetName("H");
  heading.child_ids.push_back(1101);

  ui::AXNodeData child;
  child.id = 1101;
  child.role = ax::mojom::Role::kStaticText;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, heading, child);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(1101));
  ASSERT_NE(n, nullptr);
  EXPECT_TRUE(n->IsHeading());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_OnlyTextChildrenAndListMarker) {
  ui::AXNodeData root;
  root.id = 1199;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(1200);
  root.child_ids.push_back(1203);

  ui::AXNodeData parent1;
  parent1.id = 1200;
  parent1.role = ax::mojom::Role::kGenericContainer;
  parent1.child_ids.push_back(1201);
  parent1.child_ids.push_back(1202);

  ui::AXNodeData text1;
  text1.id = 1201;
  text1.role = ax::mojom::Role::kStaticText;

  ui::AXNodeData text2;
  text2.id = 1202;
  text2.role = ax::mojom::Role::kStaticText;

  ui::AXNodeData parent2;
  parent2.id = 1203;
  parent2.role = ax::mojom::Role::kGenericContainer;
  parent2.child_ids.push_back(1204);

  ui::AXNodeData marker;
  marker.id = 1204;
  marker.role = ax::mojom::Role::kListMarker;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, parent1, text1, text2, parent2, marker);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* p1 = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(1200));
  auto* p2 = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(1203));
  ASSERT_NE(p1, nullptr);
  ASSERT_NE(p2, nullptr);
  EXPECT_TRUE(p1->HasOnlyTextChildren());
  EXPECT_TRUE(p2->HasListMarkerChild());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_HasOnlyTextChildren_NegativeWithImage) {
  ui::AXNodeData root;
  root.id = 9500;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(9501);

  ui::AXNodeData parent;
  parent.id = 9501;
  parent.role = ax::mojom::Role::kGenericContainer;
  parent.child_ids.push_back(9502);
  parent.child_ids.push_back(9503);

  ui::AXNodeData text;
  text.id = 9502;
  text.role = ax::mojom::Role::kStaticText;

  ui::AXNodeData img;
  img.id = 9503;
  img.role = ax::mojom::Role::kImage;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, parent, text, img);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* p = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(9501));
  ASSERT_NE(p, nullptr);
  EXPECT_FALSE(p->HasOnlyTextChildren());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_HasOnlyTextChildren_OnlyTextChildren) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData div;
  div.id = 2;
  div.role = ax::mojom::Role::kGenericContainer;

  ui::AXNodeData text1;
  text1.id = 3;
  text1.role = ax::mojom::Role::kStaticText;

  ui::AXNodeData text2;
  text2.id = 4;
  text2.role = ax::mojom::Role::kInlineTextBox;

  div.child_ids = {3, 4};
  root.child_ids = {2};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, div, text1, text2);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  EXPECT_TRUE(n->HasOnlyTextChildren());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_HasOnlyTextChildren_NonTextChildren) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData div;
  div.id = 2;
  div.role = ax::mojom::Role::kGenericContainer;

  ui::AXNodeData text;
  text.id = 3;
  text.role = ax::mojom::Role::kStaticText;

  ui::AXNodeData button;
  button.id = 4;
  button.role = ax::mojom::Role::kButton;

  div.child_ids = {3, 4};
  root.child_ids = {2};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, div, text, button);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  EXPECT_FALSE(n->HasOnlyTextChildren());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_HasListMarkerChild_WithListMarker) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData list_item;
  list_item.id = 2;
  list_item.role = ax::mojom::Role::kListItem;

  ui::AXNodeData marker;
  marker.id = 3;
  marker.role = ax::mojom::Role::kListMarker;

  ui::AXNodeData text;
  text.id = 4;
  text.role = ax::mojom::Role::kStaticText;

  root.child_ids = {2};
  list_item.child_ids = {3, 4};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, list_item, marker, text);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  EXPECT_TRUE(n->HasListMarkerChild());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_HasListMarkerChild_WithoutListMarker) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData list_item;
  list_item.id = 2;
  list_item.role = ax::mojom::Role::kListItem;

  ui::AXNodeData text;
  text.id = 3;
  text.role = ax::mojom::Role::kStaticText;

  root.child_ids = {2};
  list_item.child_ids = {3};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, list_item, text);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);

  EXPECT_FALSE(n->HasListMarkerChild());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_HeadingLink) {
  ui::AXNodeData root;
  root.id = 1299;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(1300);

  ui::AXNodeData heading;
  heading.id = 1300;
  heading.role = ax::mojom::Role::kHeading;
  heading.child_ids.push_back(1301);

  ui::AXNodeData link;
  link.id = 1301;
  link.role = ax::mojom::Role::kLink;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, heading, link);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* h = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(1300));
  ASSERT_NE(h, nullptr);
  EXPECT_TRUE(h->IsHeadingLink());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsLeafVariants) {
  ui::BrowserAccessibilityOHOS::ResetLeafCache();
  ui::AXNodeData root;
  root.id = 1400;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(1401);
  root.child_ids.push_back(1402);
  root.child_ids.push_back(1403);

  ui::AXNodeData button;
  button.id = 1401;
  button.role = ax::mojom::Role::kButton;

  ui::AXNodeData link;
  link.id = 1402;
  link.role = ax::mojom::Role::kLink;

  ui::AXNodeData container;
  container.id = 1403;
  container.role = ax::mojom::Role::kGenericContainer;
  container.child_ids.push_back(1404);

  ui::AXNodeData static_text;
  static_text.id = 1404;
  static_text.role = ax::mojom::Role::kStaticText;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, button, link, container, static_text);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* b = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(1401));
  auto* l = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(1402));
  auto* c = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(1403));
  ASSERT_NE(b, nullptr);
  ASSERT_NE(l, nullptr);
  ASSERT_NE(c, nullptr);
  EXPECT_TRUE(b->IsLeaf());
  EXPECT_TRUE(l->IsLeaf());
  EXPECT_TRUE(c->IsLeaf());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsLeaf_FocusableChildReturnsFalse) {
  ui::AXNodeData root;
  root.id = 2840;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(2841);

  ui::AXNodeData parent;
  parent.id = 2841;
  parent.role = ax::mojom::Role::kGenericContainer;
  parent.child_ids.push_back(2842);

  ui::AXNodeData child;
  child.id = 2842;
  child.role = ax::mojom::Role::kButton;
  child.AddState(ax::mojom::State::kFocusable);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, parent, child);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* p = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2841));
  ASSERT_NE(p, nullptr);
  EXPECT_FALSE(p->IsLeaf());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_SoleInterestingNode_Null) {
  ui::AXNodeData root;
  root.id = 1499;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(1500);

  ui::AXNodeData parent;
  parent.id = 1500;
  parent.role = ax::mojom::Role::kGenericContainer;
  parent.child_ids.push_back(1501);
  parent.child_ids.push_back(1502);

  ui::AXNodeData btn1;
  btn1.id = 1501;
  btn1.role = ax::mojom::Role::kButton;
  btn1.SetName("A");

  ui::AXNodeData btn2;
  btn2.id = 1502;
  btn2.role = ax::mojom::Role::kButton;
  btn2.SetName("B");

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, parent, btn1, btn2);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* p = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(1500));
  ASSERT_NE(p, nullptr);
  EXPECT_EQ(p->GetSoleInterestingNodeFromSubtree(), nullptr);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_SoleInterestingNode_Positive) {
  ui::AXNodeData root;
  root.id = 3099;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(3100);

  ui::AXNodeData parent;
  parent.id = 3100;
  parent.role = ax::mojom::Role::kGenericContainer;
  parent.child_ids.push_back(3101);

  ui::AXNodeData btn;
  btn.id = 3101;
  btn.role = ax::mojom::Role::kButton;
  btn.SetName("A");

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, parent, btn);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* p = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(3100));
  ASSERT_NE(p, nullptr);
  auto* sole = p->GetSoleInterestingNodeFromSubtree();
  ASSERT_NE(sole, nullptr);
  EXPECT_EQ(sole->GetAccessibilityId(), static_cast<int64_t>(3101 + 1));
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSoleInterestingNodeFromSubtree_InterestingNode) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData button;
  button.id = 2;
  button.role = ax::mojom::Role::kButton;

  root.child_ids = {2};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, button);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  const ui::BrowserAccessibilityOHOS* result = n->GetSoleInterestingNodeFromSubtree();
  EXPECT_EQ(result, n);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSoleInterestingNodeFromSubtree_TwoInterestingChildren) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData div;
  div.id = 2;
  div.role = ax::mojom::Role::kGenericContainer;

  ui::AXNodeData button1;
  button1.id = 3;
  button1.role = ax::mojom::Role::kButton;

  ui::AXNodeData button2;
  button2.id = 4;
  button2.role = ax::mojom::Role::kButton;

  root.child_ids = {2};
  div.child_ids = {3, 4};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, div, button1, button2);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  const ui::BrowserAccessibilityOHOS* result = n->GetSoleInterestingNodeFromSubtree();
  EXPECT_EQ(result, nullptr);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSoleInterestingNodeFromSubtree_OneInterestingChild) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData div;
  div.id = 2;
  div.role = ax::mojom::Role::kGenericContainer;

  ui::AXNodeData button;
  button.id = 3;
  button.role = ax::mojom::Role::kButton;

  ui::AXNodeData text;
  text.id = 4;
  text.role = ax::mojom::Role::kStaticText;

  root.child_ids = {2};
  div.child_ids = {3, 4};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, div, button, text);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  const ui::BrowserAccessibilityOHOS* result = n->GetSoleInterestingNodeFromSubtree();
  EXPECT_NE(result, nullptr);
  EXPECT_EQ(result->GetId(), 3);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetSoleInterestingNodeFromSubtree_NoInterestingChildren) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData div;
  div.id = 2;
  div.role = ax::mojom::Role::kGenericContainer;

  ui::AXNodeData text1;
  text1.id = 3;
  text1.role = ax::mojom::Role::kStaticText;

  ui::AXNodeData text2;
  text2.id = 4;
  text2.role = ax::mojom::Role::kStaticText;

  root.child_ids = {2};
  div.child_ids = {3, 4};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, div, text1, text2);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  const ui::BrowserAccessibilityOHOS* result = n->GetSoleInterestingNodeFromSubtree();
  EXPECT_EQ(result, nullptr);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_CanFireEvents_ChildOfLeaf) {
  ui::AXNodeData root;
  root.id = 1599;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(1600);

  ui::AXNodeData button;
  button.id = 1600;
  button.role = ax::mojom::Role::kButton;
  button.SetName("Btn");
  button.child_ids.push_back(1601);

  ui::AXNodeData child;
  child.id = 1601;
  child.role = ax::mojom::Role::kStaticText;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, button, child);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* c = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(1601));
  ASSERT_NE(c, nullptr);
  EXPECT_FALSE(c->CanFireEvents());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_CanFireEvents_NotChildOfLeaf) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData button;
  button.id = 2;
  button.role = ax::mojom::Role::kButton;

  root.child_ids = {2};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, button);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  EXPECT_TRUE(n->CanFireEvents());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_CanFireEvents_ChildOfLeaf_Second) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData button;
  button.id = 2;
  button.role = ax::mojom::Role::kButton;

  ui::AXNodeData text;
  text.id = 3;
  text.role = ax::mojom::Role::kStaticText;

  root.child_ids = {2};
  button.child_ids = {3};
  root.child_ids = {2};
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, button, text);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(3));
  ASSERT_NE(n, nullptr);
  EXPECT_FALSE(n->CanFireEvents());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_CheckboxGroupStatus) {
  ui::AXNodeData root;
  root.id = 1699;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(1700);

  ui::AXNodeData node;
  node.id = 1700;
  node.role = ax::mojom::Role::kMenuItemCheckBox;
  node.SetCheckedState(ax::mojom::CheckedState::kTrue);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, node);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(1700));
  ASSERT_NE(n, nullptr);
  EXPECT_EQ(n->GetCheckboxGroupSelectedStatus(), 0);

  ui::AXNodeData root_m;
  root_m.id = 1710;
  root_m.role = ax::mojom::Role::kRootWebArea;
  root_m.child_ids.push_back(1711);
  ui::AXNodeData node_m = node;
  node_m.id = 1711;
  node_m.SetCheckedState(ax::mojom::CheckedState::kMixed);
  ui::AXTreeUpdate upd_m = ui::MakeAXTreeUpdateForTesting(root_m, node_m);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager_m = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      upd_m, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n_m = static_cast<ui::BrowserAccessibilityOHOS*>(manager_m->GetFromID(1711));
  ASSERT_NE(n_m, nullptr);
  EXPECT_EQ(n_m->GetCheckboxGroupSelectedStatus(), 1);

  ui::AXNodeData root_f;
  root_f.id = 1720;
  root_f.role = ax::mojom::Role::kRootWebArea;
  root_f.child_ids.push_back(1721);
  ui::AXNodeData node_f = node;
  node_f.id = 1721;
  node_f.SetCheckedState(ax::mojom::CheckedState::kFalse);
  ui::AXTreeUpdate upd_f = ui::MakeAXTreeUpdateForTesting(root_f, node_f);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager_f = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      upd_f, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n_f = static_cast<ui::BrowserAccessibilityOHOS*>(manager_f->GetFromID(1721));
  ASSERT_NE(n_f, nullptr);
  EXPECT_EQ(n_f->GetCheckboxGroupSelectedStatus(), 2);

  ui::AXNodeData root_d;
  root_d.id = 1730;
  root_d.role = ax::mojom::Role::kRootWebArea;
  root_d.child_ids.push_back(1731);
  ui::AXNodeData node_d;
  node_d.id = 1731;
  node_d.role = ax::mojom::Role::kButton;
  ui::AXTreeUpdate upd_d = ui::MakeAXTreeUpdateForTesting(root_d, node_d);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager_d = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      upd_d, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n_d = static_cast<ui::BrowserAccessibilityOHOS*>(manager_d->GetFromID(1731));
  ASSERT_NE(n_d, nullptr);
  EXPECT_EQ(n_d->GetCheckboxGroupSelectedStatus(), -1);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsInterestingVariants) {
  ui::AXNodeData root;
  root.id = 1900;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(1901);
  root.child_ids.push_back(1902);

  ui::AXNodeData button;
  button.id = 1901;
  button.role = ax::mojom::Role::kButton;
  button.SetName("Press");

  ui::AXNodeData link;
  link.id = 1902;
  link.role = ax::mojom::Role::kLink;
  link.child_ids.push_back(1903);

  ui::AXNodeData link_child;
  link_child.id = 1903;
  link_child.role = ax::mojom::Role::kStaticText;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, button, link, link_child);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* r = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(1900));
  auto* b = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(1901));
  auto* lc = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(1903));
  ASSERT_NE(r, nullptr);
  ASSERT_NE(b, nullptr);
  ASSERT_NE(lc, nullptr);
  EXPECT_FALSE(r->IsInterestingOnOHOS());
  EXPECT_TRUE(b->IsInterestingOnOHOS());
  EXPECT_FALSE(lc->IsInterestingOnOHOS());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsInteresting_InvisibleIframeParentFalse) {
  ui::AXNodeData root;
  root.id = 9740;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(9741);

  ui::AXNodeData iframe;
  iframe.id = 9741;
  iframe.role = ax::mojom::Role::kIframe;
  iframe.AddState(ax::mojom::State::kInvisible);
  iframe.child_ids.push_back(9742);

  ui::AXNodeData child;
  child.id = 9742;
  child.role = ax::mojom::Role::kButton;
  child.SetName("btn");

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, iframe, child);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(9742));
  ASSERT_NE(n, nullptr);
  EXPECT_TRUE(n->IsInterestingOnOHOS());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsInterestingOnOHOS_PlatformDocumentEmptyText) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;
  root.AddStringAttribute(ax::mojom::StringAttribute::kName, "");

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(1));
  ASSERT_NE(n, nullptr);
  EXPECT_FALSE(n->IsInterestingOnOHOS());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsInterestingOnOHOS_InvisibleOrIgnored) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData invisible;
  invisible.id = 2;
  invisible.role = ax::mojom::Role::kButton;
  invisible.AddState(ax::mojom::State::kInvisible);

  root.child_ids = {2};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, invisible);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  EXPECT_FALSE(n->IsInterestingOnOHOS());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsInterestingOnOHOS_ControlRole) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData button;
  button.id = 2;
  button.role = ax::mojom::Role::kButton;

  root.child_ids = {2};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, button);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  EXPECT_TRUE(n->IsInterestingOnOHOS());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsInterestingOnOHOS_NonFocusableChildOfControl) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData button;
  button.id = 2;
  button.role = ax::mojom::Role::kButton;

  ui::AXNodeData text;
  text.id = 3;
  text.role = ax::mojom::Role::kStaticText;
  text.child_ids = {};

  button.child_ids = {3};
  root.child_ids = {2};
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, button, text);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(3));
  ASSERT_NE(n, nullptr);
  EXPECT_FALSE(n->IsInterestingOnOHOS());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsInterestingOnOHOS_ChildOfInvisibleIframe) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData iframe;
  iframe.id = 2;
  iframe.role = ax::mojom::Role::kIframe;
  iframe.AddState(ax::mojom::State::kInvisible);

  ui::AXNodeData text;
  text.id = 3;
  text.role = ax::mojom::Role::kStaticText;
  text.child_ids = {};

  iframe.child_ids = {3};
  root.child_ids = {2};
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, iframe, text);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(3));
  ASSERT_NE(n, nullptr);
  EXPECT_FALSE(n->IsInterestingOnOHOS());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsInterestingOnOHOS_ChildOfLink) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData link;
  link.id = 2;
  link.role = ax::mojom::Role::kLink;

  ui::AXNodeData text;
  text.id = 3;
  text.role = ax::mojom::Role::kStaticText;
  text.child_ids = {};

  link.child_ids = {3};
  root.child_ids = {2};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, link, text);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(3));
  ASSERT_NE(n, nullptr);
  EXPECT_FALSE(n->IsInterestingOnOHOS());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsInterestingOnOHOS_FocusableNode) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData button;
  button.id = 2;
  button.role = ax::mojom::Role::kButton;
  button.AddState(ax::mojom::State::kFocusable);

  root.child_ids = {2};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, button);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  EXPECT_TRUE(n->IsInterestingOnOHOS());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsInterestingOnOHOS_ProgressIndicator) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData progress;
  progress.id = 2;
  progress.role = ax::mojom::Role::kProgressIndicator;

  root.child_ids = {2};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, progress);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  EXPECT_TRUE(n->IsInterestingOnOHOS());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsInterestingOnOHOS_DirectDescendantOfLinkNoSiblings) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData link;
  link.id = 2;
  link.role = ax::mojom::Role::kLink;

  ui::AXNodeData text;
  text.id = 3;
  text.role = ax::mojom::Role::kStaticText;
  text.child_ids = {};

  root.child_ids = {2};
  link.child_ids = {3};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, link, text);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(3));
  ASSERT_NE(n, nullptr);
  EXPECT_FALSE(n->IsInterestingOnOHOS());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsInterestingOnOHOS_EmbeddedObject) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData embedded;
  embedded.id = 2;
  embedded.role = ax::mojom::Role::kEmbeddedObject;

  root.child_ids = {2};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, embedded);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  EXPECT_TRUE(n->IsInterestingOnOHOS());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsInterestingOnOHOS_LeafWithNonWhitespaceText) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData text;
  text.id = 2;
  text.role = ax::mojom::Role::kStaticText;
  text.AddStringAttribute(ax::mojom::StringAttribute::kName, "Hello World");

  root.child_ids = {2};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, text);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  EXPECT_TRUE(n->IsInterestingOnOHOS());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsInterestingOnOHOS_LeafWithWhitespaceText) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData text;
  text.id = 2;
  text.role = ax::mojom::Role::kStaticText;
  text.AddStringAttribute(ax::mojom::StringAttribute::kName, "   \t\n   ");

  root.child_ids = {2};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, text);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  EXPECT_FALSE(n->IsInterestingOnOHOS());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_ComboboxExpandedText_Fallback) {
  ui::AXNodeData root;
  root.id = 2099;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(2100);

  ui::AXNodeData combo;
  combo.id = 2100;
  combo.role = ax::mojom::Role::kComboBoxMenuButton;
  combo.child_ids.push_back(2101);

  ui::AXNodeData listbox;
  listbox.id = 2101;
  listbox.role = ax::mojom::Role::kListBox;
  listbox.AddIntAttribute(ax::mojom::IntAttribute::kSetSize, 5);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, combo, listbox);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* c = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2100));
  ASSERT_NE(c, nullptr);
  auto text = c->GetComboboxExpandedTextFallback();
  EXPECT_EQ(text, u"{x} options available");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_ComboboxExpandedTextFallback_SingleCollectionWithSetSize) {
  ui::AXNodeData root;
  root.id = 2760;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(2761);

  ui::AXNodeData combo;
  combo.id = 2761;
  combo.role = ax::mojom::Role::kComboBoxMenuButton;
  combo.child_ids.push_back(2762);

  ui::AXNodeData list;
  list.id = 2762;
  list.role = ax::mojom::Role::kListBox;
  list.AddIntAttribute(ax::mojom::IntAttribute::kSetSize, 3);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, combo, list);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* c = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2761));
  ASSERT_NE(c, nullptr);
  auto text = c->GetComboboxExpandedTextFallback();
  EXPECT_EQ(text, u"{x} options available");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_ComboboxExpandedTextFallback_NoOrMultipleCollections) {
  // No collection sub-nodes: Return default text.
  {
    ui::AXNodeData root;
    root.id = 6700;
    root.role = ax::mojom::Role::kRootWebArea;
    root.child_ids.push_back(6701);

    ui::AXNodeData combo;
    combo.id = 6701;
    combo.role = ax::mojom::Role::kComboBoxMenuButton;

    ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, combo);
    std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
        update, node_id_delegate_, test_browser_accessibility_delegate_.get());
    auto* c = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(6701));
    ASSERT_NE(c, nullptr);
    EXPECT_EQ(c->GetComboboxExpandedTextFallback(), u"Expanded");
  }

  // Multiple collection sub-nodes: Also follow the default text
  {
    ui::AXNodeData root;
    root.id = 6710;
    root.role = ax::mojom::Role::kRootWebArea;
    root.child_ids.push_back(6711);

    ui::AXNodeData combo;
    combo.id = 6711;
    combo.role = ax::mojom::Role::kComboBoxMenuButton;
    combo.child_ids.push_back(6712);
    combo.child_ids.push_back(6713);

    ui::AXNodeData list1;
    list1.id = 6712;
    list1.role = ax::mojom::Role::kListBox;

    ui::AXNodeData list2;
    list2.id = 6713;
    list2.role = ax::mojom::Role::kListBox;

    ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, combo, list1, list2);
    std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
        update, node_id_delegate_, test_browser_accessibility_delegate_.get());
    auto* c = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(6711));
    ASSERT_NE(c, nullptr);
    EXPECT_EQ(c->GetComboboxExpandedTextFallback(), u"Expanded");
  }
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_ComboboxExpandedTextFallback_SingleCollectionNoSetSize) {
  ui::AXNodeData root;
  root.id = 2850;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(2851);

  ui::AXNodeData combo;
  combo.id = 2851;
  combo.role = ax::mojom::Role::kComboBoxMenuButton;
  combo.child_ids.push_back(2852);

  ui::AXNodeData list;
  list.id = 2852;
  list.role = ax::mojom::Role::kListBox;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, combo, list);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* c = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2851));
  ASSERT_NE(c, nullptr);
  auto text = c->GetComboboxExpandedTextFallback();
  EXPECT_EQ(text, u"{x} options available");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetComboboxExpandedTextFallback_NoCollections) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData combobox;
  combobox.id = 2;
  combobox.role = ax::mojom::Role::kComboBoxGrouping;

  ui::AXNodeData text;
  text.id = 3;
  text.role = ax::mojom::Role::kStaticText;

  root.child_ids = {2};
  combobox.child_ids = {3};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, combobox, text);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  std::u16string result = n->GetComboboxExpandedTextFallback();
  EXPECT_EQ(result, u"Expanded");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetComboboxExpandedTextFallback_MultipleCollections) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData combobox;
  combobox.id = 2;
  combobox.role = ax::mojom::Role::kComboBoxGrouping;

  ui::AXNodeData listbox1;
  listbox1.id = 3;
  listbox1.role = ax::mojom::Role::kListBox;

  ui::AXNodeData listbox2;
  listbox2.id = 4;
  listbox2.role = ax::mojom::Role::kListBox;

  root.child_ids = {2};
  combobox.child_ids = {3, 4};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, combobox, listbox1, listbox2);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  std::u16string result = n->GetComboboxExpandedTextFallback();
  EXPECT_EQ(result, u"Expanded");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetComboboxExpandedTextFallback_SingleCollectionNoSetSize) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData combobox;
  combobox.id = 2;
  combobox.role = ax::mojom::Role::kComboBoxGrouping;

  ui::AXNodeData listbox;
  listbox.id = 3;
  listbox.role = ax::mojom::Role::kListBox;

  root.child_ids = {2};
  combobox.child_ids = {3};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, combobox, listbox);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  std::u16string result = n->GetComboboxExpandedTextFallback();
  EXPECT_EQ(result, u"{x} options available");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetComboboxExpandedTextFallback_SingleCollectionWithSetSize) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData combobox;
  combobox.id = 2;
  combobox.role = ax::mojom::Role::kComboBoxGrouping;

  ui::AXNodeData listbox;
  listbox.id = 3;
  listbox.role = ax::mojom::Role::kListBox;
  listbox.AddIntAttribute(ax::mojom::IntAttribute::kSetSize, 3);

  root.child_ids = {2};
  combobox.child_ids = {3};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, combobox, listbox);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  std::u16string result = n->GetComboboxExpandedTextFallback();
  EXPECT_EQ(result, u"{x} options available");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsFocusable_PlatformDocument) {
  ui::AXNodeData root;
  root.id = 2400;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* r = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2400));
  ASSERT_NE(r, nullptr);
  EXPECT_FALSE(r->IsFocusable());

  ui::AXNodeData root_named = root;
  root_named.SetName("Doc");
  ui::AXTreeUpdate update_named = ui::MakeAXTreeUpdateForTesting(root_named);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager_named =
      std::make_unique<ui::BrowserAccessibilityManagerOHOS>(update_named, node_id_delegate_,
                                                            test_browser_accessibility_delegate_.get());
  r = static_cast<ui::BrowserAccessibilityOHOS*>(manager_named->GetFromID(2400));

  EXPECT_FALSE(r->IsFocusable());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsFocusable_IframeNamedFalse) {
  ui::AXNodeData root;
  root.id = 7000;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(7001);

  ui::AXNodeData iframe;
  iframe.id = 7001;
  iframe.role = ax::mojom::Role::kIframe;
  iframe.SetName("I");

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, iframe);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(7001));
  ASSERT_NE(n, nullptr);
  EXPECT_TRUE(n->IsFocusable());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsFocusable_PlatformDocumentWithParentNamedTrue) {
  ui::AXNodeData root;
  root.id = 9750;
  root.role = ax::mojom::Role::kGenericContainer;
  root.child_ids.push_back(9751);

  ui::AXNodeData subdoc;
  subdoc.id = 9751;
  subdoc.role = ax::mojom::Role::kRootWebArea;
  subdoc.SetName("SubDoc");

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, subdoc);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(9751));
  ASSERT_NE(n, nullptr);
  EXPECT_TRUE(n->IsFocusable());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_ComboboxExpandedText_ViaAriaControls) {
  ui::AXNodeData root;
  root.id = 2700;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(2701);

  ui::AXNodeData combo;
  combo.id = 2701;
  combo.role = ax::mojom::Role::kComboBoxMenuButton;
  combo.child_ids.push_back(2702);

  ui::AXNodeData input;
  input.id = 2702;
  input.role = ax::mojom::Role::kTextField;

  input.AddIntListAttribute(ax::mojom::IntListAttribute::kControlsIds, {2703});

  ui::AXNodeData listbox;
  listbox.id = 2703;
  listbox.role = ax::mojom::Role::kListBox;
  listbox.AddIntAttribute(ax::mojom::IntAttribute::kSetSize, 4);

  root.child_ids.push_back(2703);
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, combo, input, listbox);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* c = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2701));
  ASSERT_NE(c, nullptr);
  auto text = c->GetComboboxExpandedText();
  EXPECT_EQ(text, u"{x} options available");

  ui::AXNodeData dialog;
  dialog.id = 2704;
  dialog.role = ax::mojom::Role::kDialog;
  input.AddIntListAttribute(ax::mojom::IntListAttribute::kControlsIds, {2704});

  ui::AXNodeData root2 = root;
  root2.child_ids.clear();
  root2.child_ids.push_back(2701);
  root2.child_ids.push_back(2703);
  root2.child_ids.push_back(2704);
  ui::AXTreeUpdate update2 = ui::MakeAXTreeUpdateForTesting(root2, combo, input, listbox, dialog);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager2 = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update2, node_id_delegate_, test_browser_accessibility_delegate_.get());
  c = static_cast<ui::BrowserAccessibilityOHOS*>(manager2->GetFromID(2701));
  auto text2 = c->GetComboboxExpandedText();
  EXPECT_EQ(text2, u"Dialog");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_ComboboxExpandedText_ControlsDialog) {
  ui::AXNodeData root;
  root.id = 2750;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(2751);

  ui::AXNodeData combo;
  combo.id = 2751;
  combo.role = ax::mojom::Role::kComboBoxMenuButton;
  combo.child_ids.push_back(2752);

  ui::AXNodeData input;
  input.id = 2752;
  input.role = ax::mojom::Role::kTextField;
  input.AddIntListAttribute(ax::mojom::IntListAttribute::kControlsIds, {2753});

  ui::AXNodeData dialog;
  dialog.id = 2753;
  dialog.role = ax::mojom::Role::kDialog;

  root.child_ids.push_back(2753);
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, combo, input, dialog);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* c = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2751));
  ASSERT_NE(c, nullptr);
  auto text = c->GetComboboxExpandedText();
  EXPECT_EQ(text, u"Dialog");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_ComboboxExpandedText_NoInputReturnsFallbackEmpty) {
  ui::AXNodeData root;
  root.id = 9730;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(9731);

  ui::AXNodeData combo;
  combo.id = 9731;
  combo.role = ax::mojom::Role::kComboBoxMenuButton;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, combo);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* c = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(9731));
  ASSERT_NE(c, nullptr);
  auto text = c->GetComboboxExpandedText();
  EXPECT_EQ(text, u"Expanded");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetComboboxExpandedText_NoChildInputNotTextField) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData combobox;
  combobox.id = 2;
  combobox.role = ax::mojom::Role::kComboBoxGrouping;

  root.child_ids = {2};
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, combobox);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);

  std::u16string result = n->GetComboboxExpandedText();
  EXPECT_EQ(result, u"Expanded");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetComboboxExpandedText_NoAriaControls) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData combobox;
  combobox.id = 2;
  combobox.role = ax::mojom::Role::kTextFieldWithComboBox;

  root.child_ids = {2};
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, combobox);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);

  std::u16string result = n->GetComboboxExpandedText();
  EXPECT_EQ(result, u"Expanded");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetComboboxExpandedText_NonComboboxContainer) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData combobox;
  combobox.id = 2;
  combobox.role = ax::mojom::Role::kTextFieldWithComboBox;

  ui::AXNodeData div;
  div.id = 3;
  div.role = ax::mojom::Role::kGenericContainer;

  root.child_ids = {2, 3};
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, combobox, div);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  std::u16string result = n->GetComboboxExpandedText();
  EXPECT_EQ(result, u"Expanded");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetComboboxExpandedText_DialogRole) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData combobox;
  combobox.id = 2;
  combobox.role = ax::mojom::Role::kTextFieldWithComboBox;

  ui::AXNodeData dialog;
  dialog.id = 3;
  dialog.role = ax::mojom::Role::kDialog;

  root.child_ids = {2, 3};
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, combobox, dialog);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  std::u16string result = n->GetComboboxExpandedText();
  EXPECT_EQ(result, u"Expanded");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetComboboxExpandedText_NoSetSize) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData combobox;
  combobox.id = 2;
  combobox.role = ax::mojom::Role::kTextFieldWithComboBox;

  ui::AXNodeData listbox;
  listbox.id = 3;
  listbox.role = ax::mojom::Role::kListBox;

  root.child_ids = {2};
  combobox.child_ids = {3};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, combobox, listbox);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  std::u16string result = n->GetComboboxExpandedText();
  EXPECT_EQ(result, u"Expanded");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetComboboxExpandedText_WithSetSize) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData combobox;
  combobox.id = 2;
  combobox.role = ax::mojom::Role::kTextFieldWithComboBox;

  ui::AXNodeData listbox;
  listbox.id = 3;
  listbox.role = ax::mojom::Role::kListBox;
  listbox.AddIntAttribute(ax::mojom::IntAttribute::kSetSize, 5);

  root.child_ids = {2};
  combobox.child_ids = {3};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, combobox, listbox);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  std::u16string result = n->GetComboboxExpandedText();
  EXPECT_EQ(result, u"Expanded");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsClickable_DefaultVerb_And_NonClickableCases) {
  ui::AXNodeData root;
  root.id = 2800;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(2801);
  root.child_ids.push_back(2802);

  ui::AXNodeData btn;
  btn.id = 2801;
  btn.role = ax::mojom::Role::kButton;
  btn.AddIntAttribute(ax::mojom::IntAttribute::kDefaultActionVerb,
                      static_cast<int>(ax::mojom::DefaultActionVerb::kPress));

  ui::AXNodeData iframe;
  iframe.id = 2802;
  iframe.role = ax::mojom::Role::kIframe;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, btn, iframe);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* b = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2801));
  auto* f = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2802));
  ASSERT_NE(b, nullptr);
  ASSERT_NE(f, nullptr);
  EXPECT_TRUE(b->IsClickable());
  EXPECT_FALSE(f->IsClickable());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsClickable_ClickAncestorFalse) {
  ui::AXNodeData root;
  root.id = 2810;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(2811);
  root.child_ids.push_back(2812);

  ui::AXNodeData btn_click;
  btn_click.id = 2811;
  btn_click.role = ax::mojom::Role::kButton;
  btn_click.AddIntAttribute(ax::mojom::IntAttribute::kDefaultActionVerb,
                            static_cast<int>(ax::mojom::DefaultActionVerb::kClickAncestor));

  ui::AXNodeData btn_press;
  btn_press.id = 2812;
  btn_press.role = ax::mojom::Role::kButton;
  btn_press.AddIntAttribute(ax::mojom::IntAttribute::kDefaultActionVerb,
                            static_cast<int>(ax::mojom::DefaultActionVerb::kPress));

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, btn_click, btn_press);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n_click = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2811));
  auto* n_press = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2812));
  ASSERT_NE(n_click, nullptr);
  ASSERT_NE(n_press, nullptr);
  EXPECT_TRUE(n_click->IsClickable());
  EXPECT_TRUE(n_press->IsClickable());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsClickable_PlatformDocumentFalse) {
  ui::AXNodeData root;
  root.id = 9700;
  root.role = ax::mojom::Role::kRootWebArea;
  root.SetName("Doc");

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(9700));
  ASSERT_NE(n, nullptr);
  EXPECT_FALSE(n->IsClickable());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsClickable_LinkFalse) {
  ui::AXNodeData root;
  root.id = 9710;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(9711);

  ui::AXNodeData link;
  link.id = 9711;
  link.role = ax::mojom::Role::kLink;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, link);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(9711));
  ASSERT_NE(n, nullptr);
  EXPECT_TRUE(n->IsClickable());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsChecked_Branches) {
  ui::AXNodeData root;
  root.id = 5100;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(5101);
  root.child_ids.push_back(5102);

  ui::AXNodeData checked;
  checked.id = 5101;
  checked.role = ax::mojom::Role::kCheckBox;
  checked.SetCheckedState(ax::mojom::CheckedState::kTrue);

  ui::AXNodeData not_checked;
  not_checked.id = 5102;
  not_checked.role = ax::mojom::Role::kCheckBox;
  not_checked.SetCheckedState(ax::mojom::CheckedState::kFalse);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, checked, not_checked);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* check = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(5101));
  auto* not_check = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(5102));

  ASSERT_NE(check, nullptr);
  ASSERT_NE(not_check, nullptr);

  EXPECT_TRUE(check->IsChecked());
  EXPECT_FALSE(not_check->IsChecked());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_Scroll_PageScroll_MidPositions_ReturnTrue) {
  ui::AXNodeData root;
  root.id = 2770;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(2771);

  ui::AXNodeData cont;
  cont.id = 2771;
  cont.role = ax::mojom::Role::kGenericContainer;
  cont.AddBoolAttribute(ax::mojom::BoolAttribute::kScrollable, true);
  cont.AddIntAttribute(ax::mojom::IntAttribute::kScrollXMin, 0);
  cont.AddIntAttribute(ax::mojom::IntAttribute::kScrollYMin, 0);
  cont.AddIntAttribute(ax::mojom::IntAttribute::kScrollXMax, 100);
  cont.AddIntAttribute(ax::mojom::IntAttribute::kScrollYMax, 100);
  cont.AddIntAttribute(ax::mojom::IntAttribute::kScrollX, 50);
  cont.AddIntAttribute(ax::mojom::IntAttribute::kScrollY, 50);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, cont);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2771));
  ASSERT_NE(n, nullptr);
  EXPECT_TRUE(n->Scroll(ui::ScrollDirection::UP, true));
  EXPECT_TRUE(n->Scroll(ui::ScrollDirection::DOWN, true));
  EXPECT_TRUE(n->Scroll(ui::ScrollDirection::LEFT, true));
  EXPECT_TRUE(n->Scroll(ui::ScrollDirection::RIGHT, true));
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_Scroll_PageScroll_AtBoundaries_ReturnFalse) {
  ui::AXNodeData root1;
  root1.id = 2780;
  root1.role = ax::mojom::Role::kRootWebArea;
  root1.child_ids.push_back(2781);
  ui::AXNodeData cont1;
  cont1.id = 2781;
  cont1.role = ax::mojom::Role::kGenericContainer;
  cont1.AddBoolAttribute(ax::mojom::BoolAttribute::kScrollable, true);
  cont1.AddIntAttribute(ax::mojom::IntAttribute::kScrollXMin, 0);
  cont1.AddIntAttribute(ax::mojom::IntAttribute::kScrollYMin, 0);
  cont1.AddIntAttribute(ax::mojom::IntAttribute::kScrollXMax, 100);
  cont1.AddIntAttribute(ax::mojom::IntAttribute::kScrollYMax, 100);
  cont1.AddIntAttribute(ax::mojom::IntAttribute::kScrollX, 0);
  cont1.AddIntAttribute(ax::mojom::IntAttribute::kScrollY, 0);
  ui::AXTreeUpdate upd1 = ui::MakeAXTreeUpdateForTesting(root1, cont1);
  std::unique_ptr<ui::BrowserAccessibilityManager> m1 = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      upd1, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n1 = static_cast<ui::BrowserAccessibilityOHOS*>(m1->GetFromID(2781));
  ASSERT_NE(n1, nullptr);
  EXPECT_FALSE(n1->Scroll(ui::ScrollDirection::UP, true));
  EXPECT_FALSE(n1->Scroll(ui::ScrollDirection::LEFT, true));

  ui::AXNodeData root2;
  root2.id = 2790;
  root2.role = ax::mojom::Role::kRootWebArea;
  root2.child_ids.push_back(2791);
  ui::AXNodeData cont2;
  cont2.id = 2791;
  cont2.role = ax::mojom::Role::kGenericContainer;
  cont2.AddBoolAttribute(ax::mojom::BoolAttribute::kScrollable, true);
  cont2.AddIntAttribute(ax::mojom::IntAttribute::kScrollXMin, 0);
  cont2.AddIntAttribute(ax::mojom::IntAttribute::kScrollYMin, 0);
  cont2.AddIntAttribute(ax::mojom::IntAttribute::kScrollXMax, 100);
  cont2.AddIntAttribute(ax::mojom::IntAttribute::kScrollYMax, 100);
  cont2.AddIntAttribute(ax::mojom::IntAttribute::kScrollX, 100);
  cont2.AddIntAttribute(ax::mojom::IntAttribute::kScrollY, 100);
  ui::AXTreeUpdate upd2 = ui::MakeAXTreeUpdateForTesting(root2, cont2);
  std::unique_ptr<ui::BrowserAccessibilityManager> m2 = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      upd2, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n2 = static_cast<ui::BrowserAccessibilityOHOS*>(m2->GetFromID(2791));
  ASSERT_NE(n2, nullptr);
  EXPECT_FALSE(n2->Scroll(ui::ScrollDirection::DOWN, true));
  EXPECT_FALSE(n2->Scroll(ui::ScrollDirection::RIGHT, true));
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsScrollable_Branches) {
  ui::AXNodeData root;
  root.id = 4900;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(4901);
  root.child_ids.push_back(4902);

  ui::AXNodeData scrollable;
  scrollable.id = 4901;
  scrollable.role = ax::mojom::Role::kGenericContainer;
  scrollable.AddBoolAttribute(ax::mojom::BoolAttribute::kScrollable, true);

  ui::AXNodeData not_scrollable;
  not_scrollable.id = 4902;
  not_scrollable.role = ax::mojom::Role::kGenericContainer;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, scrollable, not_scrollable);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* scroll = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(4901));
  auto* not_scroll = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(4902));

  ASSERT_NE(scroll, nullptr);
  ASSERT_NE(not_scroll, nullptr);

  EXPECT_TRUE(scroll->IsScrollable());
  EXPECT_FALSE(not_scroll->IsScrollable());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_Scroll_OnSliderForwardBackward) {
  ui::AXNodeData root;
  root.id = 7600;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(7601);

  ui::AXNodeData slider;
  slider.id = 7601;
  slider.role = ax::mojom::Role::kSlider;
  slider.AddStringAttribute(ax::mojom::StringAttribute::kHtmlTag, "input");
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kMinValueForRange, 0.0f);
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kMaxValueForRange, 10.0f);
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kValueForRange, 5.0f);
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kStepValueForRange, 1.0f);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, slider);
  auto recording_delegate = std::make_unique<RecordingAXDelegate>();
  ui::AXPlatformTreeManagerDelegate* delegate_ptr = recording_delegate.get();
  std::unique_ptr<ui::BrowserAccessibilityManager> manager =
      std::make_unique<ui::BrowserAccessibilityManagerOHOS>(update, node_id_delegate_, delegate_ptr);
  auto* s = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(7601));
  ASSERT_NE(s, nullptr);
  s->Scroll(ax::mojom::Action::kScrollForward);
  s->Scroll(ax::mojom::Action::kScrollBackward);
  const auto& actions = static_cast<RecordingAXDelegate*>(delegate_ptr)->performed_actions();
  ASSERT_GE(actions.size(), 2u);
  EXPECT_EQ(actions[0].action, ax::mojom::Action::kSetValue);
  EXPECT_EQ(actions[0].target_node_id, s->GetId());
  EXPECT_EQ(actions[1].action, ax::mojom::Action::kSetValue);
  EXPECT_EQ(actions[1].target_node_id, s->GetId());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_Scroll_ContainerAllDirections_NoCrash) {
  ui::AXNodeData root;
  root.id = 8450;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(8451);

  ui::AXNodeData cont;
  cont.id = 8451;
  cont.role = ax::mojom::Role::kGenericContainer;
  cont.AddBoolAttribute(ax::mojom::BoolAttribute::kScrollable, true);
  cont.AddIntAttribute(ax::mojom::IntAttribute::kScrollX, 10);
  cont.AddIntAttribute(ax::mojom::IntAttribute::kScrollY, 10);
  cont.AddIntAttribute(ax::mojom::IntAttribute::kScrollXMin, 0);
  cont.AddIntAttribute(ax::mojom::IntAttribute::kScrollYMin, 0);
  cont.AddIntAttribute(ax::mojom::IntAttribute::kScrollXMax, 100);
  cont.AddIntAttribute(ax::mojom::IntAttribute::kScrollYMax, 100);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, cont);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(8451));
  ASSERT_NE(n, nullptr);

  auto recording_delegate = std::make_unique<RecordingAXDelegate>();
  ui::AXPlatformTreeManagerDelegate* delegate_ptr = recording_delegate.get();
  std::unique_ptr<ui::BrowserAccessibilityManager> manager2 =
      std::make_unique<ui::BrowserAccessibilityManagerOHOS>(update, node_id_delegate_, delegate_ptr);
  auto* n2 = static_cast<ui::BrowserAccessibilityOHOS*>(manager2->GetFromID(8451));
  ASSERT_NE(n2, nullptr);

  n2->Scroll(ax::mojom::Action::kScrollUp);
  n2->Scroll(ax::mojom::Action::kScrollDown);
  n2->Scroll(ax::mojom::Action::kScrollLeft);
  n2->Scroll(ax::mojom::Action::kScrollRight);
  const auto& actions = static_cast<RecordingAXDelegate*>(delegate_ptr)->performed_actions();
  ASSERT_EQ(actions.size(), 4u);
  EXPECT_EQ(actions[0].action, ax::mojom::Action::kScrollUp);
  EXPECT_EQ(actions[1].action, ax::mojom::Action::kScrollDown);
  EXPECT_EQ(actions[2].action, ax::mojom::Action::kScrollLeft);
  EXPECT_EQ(actions[3].action, ax::mojom::Action::kScrollRight);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_Scroll_OnNonScrollable_NoCrash) {
  ui::AXNodeData root;
  root.id = 8620;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(8621);

  ui::AXNodeData button;
  button.id = 8621;
  button.role = ax::mojom::Role::kButton;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, button);
  auto recording_delegate = std::make_unique<RecordingAXDelegate>();
  ui::AXPlatformTreeManagerDelegate* delegate_ptr = recording_delegate.get();
  std::unique_ptr<ui::BrowserAccessibilityManager> manager =
      std::make_unique<ui::BrowserAccessibilityManagerOHOS>(update, node_id_delegate_, delegate_ptr);
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(8621));
  ASSERT_NE(n, nullptr);
  n->Scroll(ax::mojom::Action::kScrollDown);
  n->Scroll(ax::mojom::Action::kScrollRight);
  const auto& actions = static_cast<RecordingAXDelegate*>(delegate_ptr)->performed_actions();
  ASSERT_EQ(actions.size(), 2u);
  EXPECT_EQ(actions[0].action, ax::mojom::Action::kScrollDown);
  EXPECT_EQ(actions[1].action, ax::mojom::Action::kScrollRight);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_Scroll_ForwardBackwardMapping) {
  ui::AXNodeData root;
  root.id = 2820;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(2821);

  ui::AXNodeData cont;
  cont.id = 2821;
  cont.role = ax::mojom::Role::kGenericContainer;
  cont.AddBoolAttribute(ax::mojom::BoolAttribute::kScrollable, true);
  cont.AddIntAttribute(ax::mojom::IntAttribute::kScrollXMin, 0);
  cont.AddIntAttribute(ax::mojom::IntAttribute::kScrollYMin, 0);
  cont.AddIntAttribute(ax::mojom::IntAttribute::kScrollXMax, 0);
  cont.AddIntAttribute(ax::mojom::IntAttribute::kScrollYMax, 100);
  cont.AddIntAttribute(ax::mojom::IntAttribute::kScrollX, 0);
  cont.AddIntAttribute(ax::mojom::IntAttribute::kScrollY, 50);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, cont);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2821));
  ASSERT_NE(n, nullptr);
  EXPECT_TRUE(n->Scroll(ui::ScrollDirection::BACKWARD, false));
  EXPECT_TRUE(n->Scroll(ui::ScrollDirection::FORWARD, false));

  ui::AXNodeData root2;
  root2.id = 2830;
  root2.role = ax::mojom::Role::kRootWebArea;
  root2.child_ids.push_back(2831);

  ui::AXNodeData cont2;
  cont2.id = 2831;
  cont2.role = ax::mojom::Role::kGenericContainer;
  cont2.AddBoolAttribute(ax::mojom::BoolAttribute::kScrollable, true);
  cont2.AddIntAttribute(ax::mojom::IntAttribute::kScrollXMin, 0);
  cont2.AddIntAttribute(ax::mojom::IntAttribute::kScrollYMin, 0);
  cont2.AddIntAttribute(ax::mojom::IntAttribute::kScrollXMax, 100);
  cont2.AddIntAttribute(ax::mojom::IntAttribute::kScrollYMax, 0);
  cont2.AddIntAttribute(ax::mojom::IntAttribute::kScrollX, 50);
  cont2.AddIntAttribute(ax::mojom::IntAttribute::kScrollY, 0);

  ui::AXTreeUpdate upd2 = ui::MakeAXTreeUpdateForTesting(root2, cont2);
  std::unique_ptr<ui::BrowserAccessibilityManager> m2 = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      upd2, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n2 = static_cast<ui::BrowserAccessibilityOHOS*>(m2->GetFromID(2831));
  ASSERT_NE(n2, nullptr);
  EXPECT_TRUE(n2->Scroll(ui::ScrollDirection::BACKWARD, false));
  EXPECT_TRUE(n2->Scroll(ui::ScrollDirection::FORWARD, false));
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_Scroll_NonPage_MidPositions_ReturnTrue) {
  ui::AXNodeData root;
  root.id = 9830;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(9831);

  ui::AXNodeData scroll;
  scroll.id = 9831;
  scroll.role = ax::mojom::Role::kGenericContainer;
  scroll.AddBoolAttribute(ax::mojom::BoolAttribute::kScrollable, true);
  scroll.AddIntAttribute(ax::mojom::IntAttribute::kScrollXMin, 0);
  scroll.AddIntAttribute(ax::mojom::IntAttribute::kScrollXMax, 100);
  scroll.AddIntAttribute(ax::mojom::IntAttribute::kScrollYMin, 0);
  scroll.AddIntAttribute(ax::mojom::IntAttribute::kScrollYMax, 100);
  scroll.AddIntAttribute(ax::mojom::IntAttribute::kScrollX, 50);
  scroll.AddIntAttribute(ax::mojom::IntAttribute::kScrollY, 50);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, scroll);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(9831));
  ASSERT_NE(n, nullptr);
  EXPECT_TRUE(n->Scroll(ui::ScrollDirection::UP, false));
  EXPECT_TRUE(n->Scroll(ui::ScrollDirection::DOWN, false));
  EXPECT_TRUE(n->Scroll(ui::ScrollDirection::LEFT, false));
  EXPECT_TRUE(n->Scroll(ui::ScrollDirection::RIGHT, false));
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_Scroll_NonPage_AtBoundaries_ReturnFalse) {
  ui::AXNodeData root;
  root.id = 9840;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(9841);

  ui::AXNodeData scroll;
  scroll.id = 9841;
  scroll.role = ax::mojom::Role::kGenericContainer;
  scroll.AddBoolAttribute(ax::mojom::BoolAttribute::kScrollable, true);
  scroll.AddIntAttribute(ax::mojom::IntAttribute::kScrollXMin, 0);
  scroll.AddIntAttribute(ax::mojom::IntAttribute::kScrollXMax, 0);
  scroll.AddIntAttribute(ax::mojom::IntAttribute::kScrollYMin, 0);
  scroll.AddIntAttribute(ax::mojom::IntAttribute::kScrollYMax, 0);
  scroll.AddIntAttribute(ax::mojom::IntAttribute::kScrollX, 0);
  scroll.AddIntAttribute(ax::mojom::IntAttribute::kScrollY, 0);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, scroll);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(9841));
  ASSERT_NE(n, nullptr);
  EXPECT_FALSE(n->Scroll(ui::ScrollDirection::UP, false));
  EXPECT_FALSE(n->Scroll(ui::ScrollDirection::LEFT, false));
  EXPECT_FALSE(n->Scroll(ui::ScrollDirection::DOWN, false));
  EXPECT_FALSE(n->Scroll(ui::ScrollDirection::RIGHT, false));
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_Scroll_SliderMaxLessThanOrEqualMin) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData slider;
  slider.id = 2;
  slider.role = ax::mojom::Role::kSlider;
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kValueForRange, 5.0f);
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kMinValueForRange, 0.0f);
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kMaxValueForRange, 0.0f);

  root.child_ids = {2};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, slider);
  auto recording_delegate = std::make_unique<RecordingAXDelegate>();
  ui::AXPlatformTreeManagerDelegate* delegate_ptr = recording_delegate.get();
  std::unique_ptr<ui::BrowserAccessibilityManager> manager =
      std::make_unique<ui::BrowserAccessibilityManagerOHOS>(update, node_id_delegate_, delegate_ptr);
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  n->Scroll(ax::mojom::Action::kScrollForward);
  const auto& actions = static_cast<RecordingAXDelegate*>(delegate_ptr)->performed_actions();
  EXPECT_TRUE(actions.empty());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_Scroll_SliderDisabled) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData slider;
  slider.id = 2;
  slider.role = ax::mojom::Role::kSlider;
  slider.SetRestriction(ax::mojom::Restriction::kDisabled);
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kValueForRange, 5.0f);
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kMinValueForRange, 0.0f);
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kMaxValueForRange, 10.0f);

  root.child_ids = {2};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, slider);
  auto recording_delegate = std::make_unique<RecordingAXDelegate>();
  ui::AXPlatformTreeManagerDelegate* delegate_ptr = recording_delegate.get();
  std::unique_ptr<ui::BrowserAccessibilityManager> manager =
      std::make_unique<ui::BrowserAccessibilityManagerOHOS>(update, node_id_delegate_, delegate_ptr);
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  n->Scroll(ax::mojom::Action::kScrollForward);
  const auto& actions = static_cast<RecordingAXDelegate*>(delegate_ptr)->performed_actions();
  EXPECT_TRUE(actions.empty());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_Scroll_SliderInvalidAction) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData slider;
  slider.id = 2;
  slider.role = ax::mojom::Role::kSlider;
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kValueForRange, 5.0f);
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kMinValueForRange, 0.0f);
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kMaxValueForRange, 10.0f);

  root.child_ids = {2};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, slider);
  auto recording_delegate = std::make_unique<RecordingAXDelegate>();
  ui::AXPlatformTreeManagerDelegate* delegate_ptr = recording_delegate.get();
  std::unique_ptr<ui::BrowserAccessibilityManager> manager =
      std::make_unique<ui::BrowserAccessibilityManagerOHOS>(update, node_id_delegate_, delegate_ptr);
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  n->Scroll(ax::mojom::Action::kDoDefault);
  const auto& actions = static_cast<RecordingAXDelegate*>(delegate_ptr)->performed_actions();
  EXPECT_TRUE(actions.empty());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_Scroll_SliderWithStepValue) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData slider;
  slider.id = 2;
  slider.role = ax::mojom::Role::kSlider;
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kValueForRange, 5.0f);
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kMinValueForRange, 0.0f);
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kMaxValueForRange, 10.0f);
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kStepValueForRange, 0.5f);

  root.child_ids = {2};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, slider);
  auto recording_delegate = std::make_unique<RecordingAXDelegate>();
  ui::AXPlatformTreeManagerDelegate* delegate_ptr = recording_delegate.get();
  std::unique_ptr<ui::BrowserAccessibilityManager> manager =
      std::make_unique<ui::BrowserAccessibilityManagerOHOS>(update, node_id_delegate_, delegate_ptr);
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  n->Scroll(ax::mojom::Action::kScrollForward);
  const auto& actions = static_cast<RecordingAXDelegate*>(delegate_ptr)->performed_actions();
  ASSERT_EQ(actions.size(), 1u);
  EXPECT_EQ(actions[0].action, ax::mojom::Action::kSetValue);
  EXPECT_EQ(actions[0].target_node_id, n->GetId());
  EXPECT_EQ(actions[0].value, std::string("5.5"));
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_Scroll_SliderWithoutStepValue) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData slider;
  slider.id = 2;
  slider.role = ax::mojom::Role::kSlider;
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kValueForRange, 5.0f);
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kMinValueForRange, 0.0f);
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kMaxValueForRange, 10.0f);

  root.child_ids = {2};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, slider);
  auto recording_delegate = std::make_unique<RecordingAXDelegate>();
  ui::AXPlatformTreeManagerDelegate* delegate_ptr = recording_delegate.get();
  std::unique_ptr<ui::BrowserAccessibilityManager> manager =
      std::make_unique<ui::BrowserAccessibilityManagerOHOS>(update, node_id_delegate_, delegate_ptr);
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);

  n->Scroll(ax::mojom::Action::kScrollForward);
  const auto& actions = static_cast<RecordingAXDelegate*>(delegate_ptr)->performed_actions();
  ASSERT_EQ(actions.size(), 1u);
  EXPECT_EQ(actions[0].action, ax::mojom::Action::kSetValue);
  EXPECT_EQ(actions[0].target_node_id, n->GetId());
  EXPECT_EQ(actions[0].value, std::string("5.5"));
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_Scroll_SliderSameValueAfterUpdate) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData slider;
  slider.id = 2;
  slider.role = ax::mojom::Role::kSlider;
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kValueForRange, 10.0f);
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kMinValueForRange, 0.0f);
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kMaxValueForRange, 10.0f);

  root.child_ids = {2};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, slider);
  auto recording_delegate = std::make_unique<RecordingAXDelegate>();
  ui::AXPlatformTreeManagerDelegate* delegate_ptr = recording_delegate.get();
  std::unique_ptr<ui::BrowserAccessibilityManager> manager =
      std::make_unique<ui::BrowserAccessibilityManagerOHOS>(update, node_id_delegate_, delegate_ptr);
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  n->Scroll(ax::mojom::Action::kScrollForward);
  const auto& actions = static_cast<RecordingAXDelegate*>(delegate_ptr)->performed_actions();
  EXPECT_TRUE(actions.empty());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetHint_VariousBranches) {
  ui::AXNodeData root;
  root.id = 4200;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(4201);
  root.child_ids.push_back(4202);
  root.child_ids.push_back(4203);

  ui::AXNodeData textfield;
  textfield.id = 4201;
  textfield.role = ax::mojom::Role::kTextField;
  textfield.SetName("Test Name");
  textfield.SetValue("Test Value");

  ui::AXNodeData input;
  input.id = 4202;
  input.role = ax::mojom::Role::kTextField;
  input.AddStringAttribute(ax::mojom::StringAttribute::kPlaceholder, "Enter text here");
  input.AddStringAttribute(ax::mojom::StringAttribute::kDescription, "Input description");

  ui::AXNodeData button;
  button.id = 4203;
  button.role = ax::mojom::Role::kButton;
  button.AddStringAttribute(ax::mojom::StringAttribute::kDescription, "Button description");

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, textfield, input, button);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* tf = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(4201));
  auto* inp = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(4202));
  auto* btn = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(4203));

  ASSERT_NE(tf, nullptr);
  ASSERT_NE(inp, nullptr);
  ASSERT_NE(btn, nullptr);

  auto tf_hint = tf->GetHint();
  auto inp_hint = inp->GetHint();
  auto btn_hint = btn->GetHint();

  EXPECT_EQ(tf_hint, std::string("Test Name"));
  EXPECT_EQ(inp_hint, std::string("Enter text here Input description"));
  EXPECT_EQ(btn_hint, std::string("Button description"));
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsHint_VariousBranches) {
  ui::AXNodeData root;
  root.id = 4300;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(4301);
  root.child_ids.push_back(4302);

  ui::AXNodeData with_hint;
  with_hint.id = 4301;
  with_hint.role = ax::mojom::Role::kTextField;
  with_hint.SetName("Test Name");
  with_hint.AddStringAttribute(ax::mojom::StringAttribute::kDescription, "Description");

  ui::AXNodeData without_hint;
  without_hint.id = 4302;
  without_hint.role = ax::mojom::Role::kGenericContainer;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, with_hint, without_hint);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* with = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(4301));
  auto* without = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(4302));

  ASSERT_NE(with, nullptr);
  ASSERT_NE(without, nullptr);

  EXPECT_TRUE(with->IsHint());
  EXPECT_FALSE(without->IsHint());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetContentInvalidErrorMessage_Branches) {
  ui::AXNodeData root;
  root.id = 4400;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(4401);
  root.child_ids.push_back(4402);
  root.child_ids.push_back(4403);

  ui::AXNodeData valid;
  valid.id = 4401;
  valid.role = ax::mojom::Role::kTextField;
  valid.AddIntAttribute(ax::mojom::IntAttribute::kInvalidState, static_cast<int>(ax::mojom::InvalidState::kFalse));

  ui::AXNodeData invalid_no_children;
  invalid_no_children.id = 4402;
  invalid_no_children.role = ax::mojom::Role::kTextField;
  invalid_no_children.AddIntAttribute(ax::mojom::IntAttribute::kInvalidState,
                                      static_cast<int>(ax::mojom::InvalidState::kTrue));

  ui::AXNodeData invalid_with_children;
  invalid_with_children.id = 4403;
  invalid_with_children.role = ax::mojom::Role::kTextField;
  invalid_with_children.AddIntAttribute(ax::mojom::IntAttribute::kInvalidState,
                                        static_cast<int>(ax::mojom::InvalidState::kTrue));
  invalid_with_children.child_ids.push_back(4404);

  ui::AXNodeData child;
  child.id = 4404;
  child.role = ax::mojom::Role::kStaticText;
  child.AddIntListAttribute(ax::mojom::IntListAttribute::kMarkerTypes,
                            {static_cast<int32_t>(ax::mojom::MarkerType::kSpelling)});

  ui::AXTreeUpdate update =
      ui::MakeAXTreeUpdateForTesting(root, valid, invalid_no_children, invalid_with_children, child);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* v = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(4401));
  auto* inv1 = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(4402));
  auto* inv2 = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(4403));

  ASSERT_NE(v, nullptr);
  ASSERT_NE(inv1, nullptr);
  ASSERT_NE(inv2, nullptr);

  auto v_msg = v->GetContentInvalidErrorMessage();
  auto inv1_msg = inv1->GetContentInvalidErrorMessage();
  auto inv2_msg = inv2->GetContentInvalidErrorMessage();

  EXPECT_EQ(v_msg, std::string());
  EXPECT_EQ(inv1_msg, std::string());
  EXPECT_EQ(inv2_msg, std::string());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_ContentInvalid_GrammarMessage) {
  ui::AXNodeData root;
  root.id = 6600;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(6601);

  ui::AXNodeData invalid_with_child;
  invalid_with_child.id = 6601;
  invalid_with_child.role = ax::mojom::Role::kTextField;
  invalid_with_child.AddIntAttribute(ax::mojom::IntAttribute::kInvalidState,
                                     static_cast<int>(ax::mojom::InvalidState::kTrue));
  invalid_with_child.child_ids.push_back(6602);

  ui::AXNodeData child;
  child.id = 6602;
  child.role = ax::mojom::Role::kStaticText;
  child.AddIntListAttribute(ax::mojom::IntListAttribute::kMarkerTypes,
                            {static_cast<int32_t>(ax::mojom::MarkerType::kGrammar)});

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, invalid_with_child, child);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(6601));
  ASSERT_NE(n, nullptr);
  EXPECT_EQ(n->GetContentInvalidErrorMessage(), std::string());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetContentInvalidErrorMessage_Spelling) {
  ui::AXNodeData root;
  root.id = 9720;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(9721);

  ui::AXNodeData invalid;
  invalid.id = 9721;
  invalid.role = ax::mojom::Role::kTextField;
  invalid.AddIntAttribute(ax::mojom::IntAttribute::kInvalidState, static_cast<int>(ax::mojom::InvalidState::kTrue));
  invalid.child_ids.push_back(9722);

  ui::AXNodeData child;
  child.id = 9722;
  child.role = ax::mojom::Role::kStaticText;
  child.AddIntListAttribute(ax::mojom::IntListAttribute::kMarkerTypes,
                            {static_cast<int32_t>(ax::mojom::MarkerType::kSpelling)});

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, invalid, child);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(9721));
  ASSERT_NE(n, nullptr);
  EXPECT_EQ(n->GetContentInvalidErrorMessage(), std::string());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetContentInvalidErrorMessage_NullContentClient) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData input;
  input.id = 2;
  input.role = ax::mojom::Role::kTextField;
  input.AddIntAttribute(ax::mojom::IntAttribute::kInvalidState, static_cast<int32_t>(ax::mojom::InvalidState::kTrue));

  root.child_ids = {2};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, input);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  std::string result = n->GetContentInvalidErrorMessage();
  EXPECT_EQ(result, std::string());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetContentInvalidErrorMessage_InvalidTrueNoTextChildren) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData input;
  input.id = 2;
  input.role = ax::mojom::Role::kTextField;
  input.AddIntAttribute(ax::mojom::IntAttribute::kInvalidState, static_cast<int32_t>(ax::mojom::InvalidState::kTrue));

  ui::AXNodeData div;
  div.id = 3;
  div.role = ax::mojom::Role::kGenericContainer;

  root.child_ids = {2};
  input.child_ids = {3};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, input, div);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  std::string result = n->GetContentInvalidErrorMessage();
  EXPECT_EQ(result, std::string());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetContentInvalidErrorMessage_SpellingMarker) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData input;
  input.id = 2;
  input.role = ax::mojom::Role::kTextField;
  input.AddIntAttribute(ax::mojom::IntAttribute::kInvalidState, static_cast<int32_t>(ax::mojom::InvalidState::kTrue));

  ui::AXNodeData text;
  text.id = 3;
  text.role = ax::mojom::Role::kStaticText;
  text.AddIntListAttribute(ax::mojom::IntListAttribute::kMarkerTypes,
                           {static_cast<int32_t>(ax::mojom::MarkerType::kSpelling)});

  root.child_ids = {2};
  input.child_ids = {3};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, input, text);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  std::string result = n->GetContentInvalidErrorMessage();
  EXPECT_TRUE(result.empty());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetContentInvalidErrorMessage_GrammarMarker) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData input;
  input.id = 2;
  input.role = ax::mojom::Role::kTextField;
  input.AddIntAttribute(ax::mojom::IntAttribute::kInvalidState, static_cast<int32_t>(ax::mojom::InvalidState::kTrue));

  ui::AXNodeData text;
  text.id = 3;
  text.role = ax::mojom::Role::kStaticText;
  text.AddIntListAttribute(ax::mojom::IntListAttribute::kMarkerTypes,
                           {static_cast<int32_t>(ax::mojom::MarkerType::kGrammar)});

  root.child_ids = {2};
  input.child_ids = {3};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, input, text);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  std::string result = n->GetContentInvalidErrorMessage();
  EXPECT_TRUE(result.empty());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetContentInvalidErrorMessage_InvalidStateNone) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData input;
  input.id = 2;
  input.role = ax::mojom::Role::kTextField;
  input.AddIntAttribute(ax::mojom::IntAttribute::kInvalidState, static_cast<int32_t>(ax::mojom::InvalidState::kNone));

  root.child_ids = {2};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, input);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  std::string result = n->GetContentInvalidErrorMessage();
  EXPECT_TRUE(result.empty());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetContentInvalidErrorMessage_InvalidStateFalse) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData input;
  input.id = 2;
  input.role = ax::mojom::Role::kTextField;
  input.AddIntAttribute(ax::mojom::IntAttribute::kInvalidState, static_cast<int32_t>(ax::mojom::InvalidState::kFalse));

  root.child_ids = {2};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, input);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  std::string result = n->GetContentInvalidErrorMessage();
  EXPECT_TRUE(result.empty());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsContentInvalid_Branches) {
  ui::AXNodeData root;
  root.id = 4500;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(4501);
  root.child_ids.push_back(4502);

  ui::AXNodeData invalid;
  invalid.id = 4501;
  invalid.role = ax::mojom::Role::kTextField;
  invalid.AddIntAttribute(ax::mojom::IntAttribute::kInvalidState, static_cast<int>(ax::mojom::InvalidState::kTrue));

  ui::AXNodeData valid;
  valid.id = 4502;
  valid.role = ax::mojom::Role::kTextField;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, invalid, valid);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* inv = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(4501));
  auto* val = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(4502));

  ASSERT_NE(inv, nullptr);
  ASSERT_NE(val, nullptr);

  EXPECT_TRUE(inv->IsContentInvalid());
  EXPECT_FALSE(val->IsContentInvalid());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsContentInvalid_WithInvalidStateAttribute) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData input;
  input.id = 2;
  input.role = ax::mojom::Role::kTextField;
  input.AddIntAttribute(ax::mojom::IntAttribute::kInvalidState, static_cast<int32_t>(ax::mojom::InvalidState::kTrue));

  root.child_ids = {2};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, input);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  EXPECT_TRUE(n->IsContentInvalid());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsContentInvalid_InvalidStateFalse) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData input;
  input.id = 2;
  input.role = ax::mojom::Role::kTextField;
  input.AddIntAttribute(ax::mojom::IntAttribute::kInvalidState, static_cast<int32_t>(ax::mojom::InvalidState::kFalse));
  root.child_ids = {2};
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, input);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  EXPECT_FALSE(n->IsContentInvalid());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsContentInvalid_WithoutInvalidStateAttribute) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData input;
  input.id = 2;
  input.role = ax::mojom::Role::kTextField;
  root.child_ids = {2};
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, input);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  EXPECT_FALSE(n->IsContentInvalid());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_CanOpenPopup_Branches) {
  ui::AXNodeData root;
  root.id = 4600;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(4601);
  root.child_ids.push_back(4602);

  ui::AXNodeData with_popup;
  with_popup.id = 4601;
  with_popup.role = ax::mojom::Role::kButton;
  with_popup.AddIntAttribute(ax::mojom::IntAttribute::kHasPopup, static_cast<int>(ax::mojom::HasPopup::kMenu));

  ui::AXNodeData without_popup;
  without_popup.id = 4602;
  without_popup.role = ax::mojom::Role::kButton;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, with_popup, without_popup);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* with = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(4601));
  auto* without = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(4602));

  ASSERT_NE(with, nullptr);
  ASSERT_NE(without, nullptr);

  EXPECT_TRUE(with->CanOpenPopup());
  EXPECT_FALSE(without->CanOpenPopup());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsMultiLine_Branches) {
  ui::AXNodeData root;
  root.id = 4700;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(4701);
  root.child_ids.push_back(4702);

  ui::AXNodeData multiline;
  multiline.id = 4701;
  multiline.role = ax::mojom::Role::kTextField;
  multiline.AddState(ax::mojom::State::kMultiline);

  ui::AXNodeData singleline;
  singleline.id = 4702;
  singleline.role = ax::mojom::Role::kTextField;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, multiline, singleline);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* multi = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(4701));
  auto* single = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(4702));

  ASSERT_NE(multi, nullptr);
  ASSERT_NE(single, nullptr);

  EXPECT_TRUE(multi->IsMultiLine());
  EXPECT_FALSE(single->IsMultiLine());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsCheckable_Branches) {
  ui::AXNodeData root;
  root.id = 4800;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(4801);
  root.child_ids.push_back(4802);

  ui::AXNodeData checkable;
  checkable.id = 4801;
  checkable.role = ax::mojom::Role::kCheckBox;
  checkable.SetCheckedState(ax::mojom::CheckedState::kTrue);

  ui::AXNodeData not_checkable;
  not_checkable.id = 4802;
  not_checkable.role = ax::mojom::Role::kButton;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, checkable, not_checkable);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* check = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(4801));
  auto* not_check = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(4802));

  ASSERT_NE(check, nullptr);
  ASSERT_NE(not_check, nullptr);

  EXPECT_TRUE(check->IsCheckable());
  EXPECT_FALSE(not_check->IsCheckable());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsSelected_Branches) {
  ui::AXNodeData root;
  root.id = 5000;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(5001);
  root.child_ids.push_back(5002);

  ui::AXNodeData selected;
  selected.id = 5001;
  selected.role = ax::mojom::Role::kListBoxOption;
  selected.AddBoolAttribute(ax::mojom::BoolAttribute::kSelected, true);

  ui::AXNodeData not_selected;
  not_selected.id = 5002;
  not_selected.role = ax::mojom::Role::kListBoxOption;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, selected, not_selected);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* sel = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(5001));
  auto* not_sel = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(5002));

  ASSERT_NE(sel, nullptr);
  ASSERT_NE(not_sel, nullptr);

  EXPECT_TRUE(sel->IsSelected());
  EXPECT_FALSE(not_sel->IsSelected());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsEnabled_RestrictionStates) {
  ui::AXNodeData root;
  root.id = 5200;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(5201);
  root.child_ids.push_back(5202);
  root.child_ids.push_back(5203);

  ui::AXNodeData enabled;
  enabled.id = 5201;
  enabled.role = ax::mojom::Role::kButton;
  enabled.AddIntAttribute(ax::mojom::IntAttribute::kRestriction, static_cast<int>(ax::mojom::Restriction::kNone));

  ui::AXNodeData readonly;
  readonly.id = 5202;
  readonly.role = ax::mojom::Role::kTextField;
  readonly.AddIntAttribute(ax::mojom::IntAttribute::kRestriction, static_cast<int>(ax::mojom::Restriction::kReadOnly));

  ui::AXNodeData disabled;
  disabled.id = 5203;
  disabled.role = ax::mojom::Role::kButton;
  disabled.AddIntAttribute(ax::mojom::IntAttribute::kRestriction, static_cast<int>(ax::mojom::Restriction::kDisabled));

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, enabled, readonly, disabled);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* en = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(5201));
  auto* ro = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(5202));
  auto* dis = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(5203));

  ASSERT_NE(en, nullptr);
  ASSERT_NE(ro, nullptr);
  ASSERT_NE(dis, nullptr);

  EXPECT_TRUE(en->IsEnabled());
  EXPECT_FALSE(ro->IsEnabled());
  EXPECT_FALSE(dis->IsEnabled());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsExpanded_Branches) {
  ui::AXNodeData root;
  root.id = 5300;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(5301);
  root.child_ids.push_back(5302);

  ui::AXNodeData expanded;
  expanded.id = 5301;
  expanded.role = ax::mojom::Role::kTreeItem;
  expanded.AddState(ax::mojom::State::kExpanded);

  ui::AXNodeData not_expanded;
  not_expanded.id = 5302;
  not_expanded.role = ax::mojom::Role::kTreeItem;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, expanded, not_expanded);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* exp = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(5301));
  auto* not_exp = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(5302));

  ASSERT_NE(exp, nullptr);
  ASSERT_NE(not_exp, nullptr);

  EXPECT_TRUE(exp->IsExpanded());
  EXPECT_FALSE(not_exp->IsExpanded());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsCollapsed_Branches) {
  ui::AXNodeData root;
  root.id = 5400;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(5401);
  root.child_ids.push_back(5402);

  ui::AXNodeData collapsed;
  collapsed.id = 5401;
  collapsed.role = ax::mojom::Role::kTreeItem;
  collapsed.AddState(ax::mojom::State::kCollapsed);

  ui::AXNodeData not_collapsed;
  not_collapsed.id = 5402;
  not_collapsed.role = ax::mojom::Role::kTreeItem;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, collapsed, not_collapsed);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* coll = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(5401));
  auto* not_coll = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(5402));

  ASSERT_NE(coll, nullptr);
  ASSERT_NE(not_coll, nullptr);

  EXPECT_TRUE(coll->IsCollapsed());
  EXPECT_FALSE(not_coll->IsCollapsed());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetRoleString_Link) {
  ui::AXNodeData root;
  root.id = 9600;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(9601);

  ui::AXNodeData link;
  link.id = 9601;
  link.role = ax::mojom::Role::kLink;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, link);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(9601));
  ASSERT_NE(n, nullptr);
  auto role_string = n->GetRoleString();
  EXPECT_FALSE(role_string.empty());
  EXPECT_EQ(role_string, "link");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetRoleString_Button) {
  ui::AXNodeData root;
  root.id = 9820;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(9821);

  ui::AXNodeData btn;
  btn.id = 9821;
  btn.role = ax::mojom::Role::kButton;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, btn);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(9821));
  ASSERT_NE(n, nullptr);
  EXPECT_EQ(n->GetRoleString(), "button");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_HasNonEmptyValue_Branches) {
  ui::AXNodeData root;
  root.id = 5800;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(5801);
  root.child_ids.push_back(5802);

  ui::AXNodeData with_value;
  with_value.id = 5801;
  with_value.role = ax::mojom::Role::kTextField;
  with_value.SetValue("Test Value");

  ui::AXNodeData without_value;
  without_value.id = 5802;
  without_value.role = ax::mojom::Role::kTextField;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, with_value, without_value);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* with = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(5801));
  auto* without = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(5802));

  ASSERT_NE(with, nullptr);
  ASSERT_NE(without, nullptr);

  EXPECT_TRUE(with->HasNonEmptyValue());
  EXPECT_FALSE(without->HasNonEmptyValue());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsAccessibilityGroup_Branches) {
  ui::AXNodeData root;
  root.id = 5900;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(5901);

  ui::AXNodeData group;
  group.id = 5901;
  group.role = ax::mojom::Role::kGenericContainer;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, group);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* grp = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(5901));
  ASSERT_NE(grp, nullptr);
  EXPECT_FALSE(grp->IsAccessibilityGroup());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsAccessibilityGroup_AlwaysFalse) {
  ui::AXNodeData root;
  root.id = 8000;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(8001);

  ui::AXNodeData group;
  group.id = 8001;
  group.role = ax::mojom::Role::kGroup;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, group);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* g = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(8001));
  ASSERT_NE(g, nullptr);
  EXPECT_FALSE(g->IsAccessibilityGroup());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_RangeValues_Branches) {
  ui::AXNodeData root;
  root.id = 6000;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(6001);

  ui::AXNodeData slider;
  slider.id = 6001;
  slider.role = ax::mojom::Role::kSlider;
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kMinValueForRange, 0.0f);
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kMaxValueForRange, 100.0f);
  slider.AddFloatAttribute(ax::mojom::FloatAttribute::kValueForRange, 50.0f);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, slider);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* sl = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(6001));
  ASSERT_NE(sl, nullptr);

  EXPECT_EQ(sl->RangeMin(), 0.0f);
  EXPECT_EQ(sl->RangeMax(), 100.0f);
  EXPECT_EQ(sl->RangeCurrentValue(), 50.0f);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsHierarchical_Branches) {
  ui::AXNodeData root;
  root.id = 6100;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(6101);
  root.child_ids.push_back(6102);

  ui::AXNodeData tree;
  tree.id = 6101;
  tree.role = ax::mojom::Role::kTree;

  ui::AXNodeData button;
  button.id = 6102;
  button.role = ax::mojom::Role::kButton;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, tree, button);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* tr = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(6101));
  auto* btn = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(6102));

  ASSERT_NE(tr, nullptr);
  ASSERT_NE(btn, nullptr);

  EXPECT_TRUE(tr->IsHierarchical());
  EXPECT_FALSE(btn->IsHierarchical());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsLink_Branches) {
  ui::AXNodeData root;
  root.id = 6200;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(6201);
  root.child_ids.push_back(6202);

  ui::AXNodeData link;
  link.id = 6201;
  link.role = ax::mojom::Role::kLink;

  ui::AXNodeData button;
  button.id = 6202;
  button.role = ax::mojom::Role::kButton;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, link, button);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());

  auto* lnk = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(6201));
  auto* btn = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(6202));

  ASSERT_NE(lnk, nullptr);
  ASSERT_NE(btn, nullptr);

  EXPECT_TRUE(lnk->IsLink());
  EXPECT_FALSE(btn->IsLink());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_HasOnlyTextAndImageChildren_Positive_Alt) {
  ui::AXNodeData root;
  root.id = 7900;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(7901);

  ui::AXNodeData parent;
  parent.id = 7901;
  parent.role = ax::mojom::Role::kGenericContainer;
  parent.child_ids.push_back(7902);
  parent.child_ids.push_back(7903);

  ui::AXNodeData text;
  text.id = 7902;
  text.role = ax::mojom::Role::kStaticText;

  ui::AXNodeData img;
  img.id = 7903;
  img.role = ax::mojom::Role::kImage;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, parent, text, img);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* p = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(7901));
  ASSERT_NE(p, nullptr);
  EXPECT_TRUE(p->HasOnlyTextAndImageChildren());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_HasOnlyTextAndImageChildren_OnlyTextAndImage) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData div;
  div.id = 2;
  div.role = ax::mojom::Role::kGenericContainer;

  ui::AXNodeData text;
  text.id = 3;
  text.role = ax::mojom::Role::kStaticText;

  ui::AXNodeData image;
  image.id = 4;
  image.role = ax::mojom::Role::kImage;

  root.child_ids = {2};
  div.child_ids = {3, 4};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, div, text, image);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  EXPECT_TRUE(n->HasOnlyTextAndImageChildren());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_HasOnlyTextAndImageChildren_NonTextNonImageChildren) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData div;
  div.id = 2;
  div.role = ax::mojom::Role::kGenericContainer;

  ui::AXNodeData text;
  text.id = 3;
  text.role = ax::mojom::Role::kStaticText;

  ui::AXNodeData button;
  button.id = 4;
  button.role = ax::mojom::Role::kButton;

  root.child_ids = {2};
  div.child_ids = {3, 4};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, div, text, button);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  EXPECT_FALSE(n->HasOnlyTextAndImageChildren());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_HasOnlyTextAndImageChildren_Positive) {
  ui::AXNodeData root;
  root.id = 9810;
  root.role = ax::mojom::Role::kGenericContainer;
  root.child_ids.push_back(9811);
  root.child_ids.push_back(9812);

  ui::AXNodeData text;
  text.id = 9811;
  text.role = ax::mojom::Role::kStaticText;
  text.SetName("txt");

  ui::AXNodeData img;
  img.id = 9812;
  img.role = ax::mojom::Role::kImage;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, text, img);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(9810));
  ASSERT_NE(n, nullptr);
  EXPECT_TRUE(n->HasOnlyTextAndImageChildren());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_Scroll_Getters_ReturnAttributes) {
  ui::AXNodeData root;
  root.id = 8800;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(8801);

  ui::AXNodeData cont;
  cont.id = 8801;
  cont.role = ax::mojom::Role::kGenericContainer;
  cont.AddBoolAttribute(ax::mojom::BoolAttribute::kScrollable, true);
  cont.AddIntAttribute(ax::mojom::IntAttribute::kScrollX, 12);
  cont.AddIntAttribute(ax::mojom::IntAttribute::kScrollY, 34);
  cont.AddIntAttribute(ax::mojom::IntAttribute::kScrollXMin, -5);
  cont.AddIntAttribute(ax::mojom::IntAttribute::kScrollYMin, -7);
  cont.AddIntAttribute(ax::mojom::IntAttribute::kScrollXMax, 56);
  cont.AddIntAttribute(ax::mojom::IntAttribute::kScrollYMax, 78);

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, cont);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(8801));
  ASSERT_NE(n, nullptr);

  EXPECT_EQ(n->GetScrollX(), 12);
  EXPECT_EQ(n->GetScrollY(), 34);
  EXPECT_EQ(n->GetMinScrollX(), -5);
  EXPECT_EQ(n->GetMinScrollY(), -7);
  EXPECT_EQ(n->GetMaxScrollX(), 56);
  EXPECT_EQ(n->GetMaxScrollY(), 78);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsChildOfLeaf_True) {
  ui::AXNodeData root;
  root.id = 7200;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(7201);

  ui::AXNodeData button;
  button.id = 7201;
  button.role = ax::mojom::Role::kButton;
  button.child_ids.push_back(7202);

  ui::AXNodeData child;
  child.id = 7202;
  child.role = ax::mojom::Role::kStaticText;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, button, child);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* c = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(7202));
  ASSERT_NE(c, nullptr);
  EXPECT_TRUE(c->IsChildOfLeaf());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsChildOfLeaf_WithLeafAncestor) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData button;
  button.id = 2;
  button.role = ax::mojom::Role::kButton;

  ui::AXNodeData text;
  text.id = 3;
  text.role = ax::mojom::Role::kStaticText;

  button.child_ids = {3};
  root.child_ids = {2};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, button, text);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(3));
  ASSERT_NE(n, nullptr);
  EXPECT_TRUE(n->IsChildOfLeaf());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsChildOfLeaf_WithoutLeafAncestor) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData div;
  div.id = 2;
  div.role = ax::mojom::Role::kGenericContainer;

  ui::AXNodeData text;
  text.id = 3;
  text.role = ax::mojom::Role::kStaticText;

  ui::AXNodeData button2;
  button2.id = 4;
  button2.role = ax::mojom::Role::kButton;

  div.child_ids = {3, 4};
  root.child_ids = {2};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, div, text, button2);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(3));
  ASSERT_NE(n, nullptr);
  EXPECT_FALSE(n->IsChildOfLeaf());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_ShouldExposeValueAsName_Variants) {
  // Date role => true
  {
    ui::AXNodeData root;
    root.id = 6900;
    root.role = ax::mojom::Role::kRootWebArea;
    root.child_ids.push_back(6901);

    ui::AXNodeData date;
    date.id = 6901;
    date.role = ax::mojom::Role::kDate;

    ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, date);
    std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
        update, node_id_delegate_, test_browser_accessibility_delegate_.get());
    auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(6901));
    ASSERT_NE(n, nullptr);
    EXPECT_TRUE(n->ShouldExposeValueAsName());
  }

  // ColorWell => false
  {
    ui::AXNodeData root;
    root.id = 6910;
    root.role = ax::mojom::Role::kRootWebArea;
    root.child_ids.push_back(6911);

    ui::AXNodeData colorwell;
    colorwell.id = 6911;
    colorwell.role = ax::mojom::Role::kColorWell;

    ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, colorwell);
    std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
        update, node_id_delegate_, test_browser_accessibility_delegate_.get());
    auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(6911));
    ASSERT_NE(n, nullptr);
    EXPECT_FALSE(n->ShouldExposeValueAsName());
  }

  // Range value supported (slider) => false
  {
    ui::AXNodeData root;
    root.id = 6920;
    root.role = ax::mojom::Role::kRootWebArea;
    root.child_ids.push_back(6921);

    ui::AXNodeData slider;
    slider.id = 6921;
    slider.role = ax::mojom::Role::kSlider;
    slider.AddFloatAttribute(ax::mojom::FloatAttribute::kValueForRange, 1.0f);

    ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, slider);
    std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
        update, node_id_delegate_, test_browser_accessibility_delegate_.get());
    auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(6921));
    ASSERT_NE(n, nullptr);
    EXPECT_FALSE(n->ShouldExposeValueAsName());
  }
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_ShouldExposeValueAsName_PopUpButton) {
  ui::AXNodeData root;
  root.id = 6300;
  root.role = ax::mojom::Role::kRootWebArea;
  root.child_ids.push_back(6301);

  ui::AXNodeData popup_btn;
  popup_btn.id = 6301;
  popup_btn.role = ax::mojom::Role::kPopUpButton;
  popup_btn.SetValue("selected option");

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, popup_btn);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(6301));
  ASSERT_NE(n, nullptr);
  EXPECT_TRUE(n->ShouldExposeValueAsName());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetNextFocusableNode_EmptyList) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(1));
  ASSERT_NE(n, nullptr);

  std::list<ui::BrowserAccessibilityOHOS*> empty_list;
  EXPECT_EQ(n->GetNextFocusableNode(empty_list), nullptr);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_GetPreviousFocusableNode_EmptyList) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(1));
  ASSERT_NE(n, nullptr);

  std::list<ui::BrowserAccessibilityOHOS*> empty_list;
  EXPECT_EQ(n->GetPreviousFocusableNode(empty_list), nullptr);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_FindNodeInRelativeDirection_InvalidDirection) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(1));
  ASSERT_NE(n, nullptr);

  std::list<ui::BrowserAccessibilityOHOS*> node_list;
  node_list.push_back(n);
  EXPECT_EQ(n->FindNodeInRelativeDirection(node_list, 999), nullptr);
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsLeafConsideringChildren_FocusableChildNotMenuOption) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData heading;
  heading.id = 2;
  heading.role = ax::mojom::Role::kHeading;
  heading.AddStringAttribute(ax::mojom::StringAttribute::kName, "Test");

  ui::AXNodeData button;
  button.id = 3;
  button.role = ax::mojom::Role::kButton;
  button.AddState(ax::mojom::State::kFocusable);

  heading.child_ids = {3};
  root.child_ids = {2};
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, heading, button);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  EXPECT_FALSE(n->IsLeafConsideringChildren());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsLeafConsideringChildren_TableChild) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData heading;
  heading.id = 2;
  heading.role = ax::mojom::Role::kHeading;
  heading.AddStringAttribute(ax::mojom::StringAttribute::kName, "Test");

  ui::AXNodeData table;
  table.id = 3;
  table.role = ax::mojom::Role::kTable;

  heading.child_ids = {3};
  root.child_ids = {2};
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, heading, table);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  EXPECT_FALSE(n->IsLeafConsideringChildren());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsLeafConsideringChildren_CellChild) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData heading;
  heading.id = 2;
  heading.role = ax::mojom::Role::kHeading;
  heading.AddStringAttribute(ax::mojom::StringAttribute::kName, "Test");

  ui::AXNodeData cell;
  cell.id = 3;
  cell.role = ax::mojom::Role::kCell;

  heading.child_ids = {3};
  root.child_ids = {2};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, heading, cell);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  EXPECT_FALSE(n->IsLeafConsideringChildren());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsLeafConsideringChildren_RowChild) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData heading;
  heading.id = 2;
  heading.role = ax::mojom::Role::kHeading;
  heading.AddStringAttribute(ax::mojom::StringAttribute::kName, "Test");

  ui::AXNodeData row;
  row.id = 3;
  row.role = ax::mojom::Role::kRow;

  heading.child_ids = {3};
  root.child_ids = {2};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, heading, row);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  EXPECT_FALSE(n->IsLeafConsideringChildren());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsLeafConsideringChildren_LayoutTableChild) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData heading;
  heading.id = 2;
  heading.role = ax::mojom::Role::kHeading;
  heading.AddStringAttribute(ax::mojom::StringAttribute::kName, "Test");

  ui::AXNodeData layout_table;
  layout_table.id = 3;
  layout_table.role = ax::mojom::Role::kLayoutTable;

  root.child_ids = {2};
  heading.child_ids = {3};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, heading, layout_table);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  EXPECT_FALSE(n->IsLeafConsideringChildren());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsLeafConsideringChildren_LayoutTableCellChild) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData heading;
  heading.id = 2;
  heading.role = ax::mojom::Role::kHeading;
  heading.AddStringAttribute(ax::mojom::StringAttribute::kName, "Test");

  ui::AXNodeData layout_cell;
  layout_cell.id = 3;
  layout_cell.role = ax::mojom::Role::kLayoutTableCell;

  root.child_ids = {2};
  heading.child_ids = {3};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, heading, layout_cell);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  EXPECT_FALSE(n->IsLeafConsideringChildren());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsLeafConsideringChildren_LayoutTableRowChild) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData heading;
  heading.id = 2;
  heading.role = ax::mojom::Role::kHeading;
  heading.AddStringAttribute(ax::mojom::StringAttribute::kName, "Test");

  ui::AXNodeData layout_row;
  layout_row.id = 3;
  layout_row.role = ax::mojom::Role::kLayoutTableRow;

  root.child_ids = {2};
  heading.child_ids = {3};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, heading, layout_row);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  EXPECT_FALSE(n->IsLeafConsideringChildren());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsLeafConsideringChildren_NestedChildReturnsFalse) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData heading;
  heading.id = 2;
  heading.role = ax::mojom::Role::kHeading;
  heading.AddStringAttribute(ax::mojom::StringAttribute::kName, "Test");

  ui::AXNodeData div;
  div.id = 3;
  div.role = ax::mojom::Role::kGenericContainer;

  ui::AXNodeData button;
  button.id = 4;
  button.role = ax::mojom::Role::kButton;
  button.AddState(ax::mojom::State::kFocusable);

  root.child_ids = {2};
  heading.child_ids = {3};
  div.child_ids = {4};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, heading, div, button);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  EXPECT_FALSE(n->IsLeafConsideringChildren());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_IsLeafConsideringChildren_NoProblematicChildren) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData heading;
  heading.id = 2;
  heading.role = ax::mojom::Role::kHeading;
  heading.AddStringAttribute(ax::mojom::StringAttribute::kName, "Test");

  ui::AXNodeData text;
  text.id = 3;
  text.role = ax::mojom::Role::kStaticText;

  root.child_ids = {2};
  heading.child_ids = {3};

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, heading, text);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);
  EXPECT_TRUE(n->IsLeafConsideringChildren());
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_AppendTextToString_EmptyExtraText) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(1));
  ASSERT_NE(n, nullptr);

  std::u16string result = u"existing text";
  n->AppendTextToString(u"", &result);
  EXPECT_EQ(result, u"existing text");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_AppendTextToString_EmptyTargetString) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(1));
  ASSERT_NE(n, nullptr);

  std::u16string result = u"";
  n->AppendTextToString(u"new text", &result);
  EXPECT_EQ(result, u"new text");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_AppendTextToString_BothNonEmpty) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root);
  std::unique_ptr<ui::BrowserAccessibilityManager> manager = std::make_unique<ui::BrowserAccessibilityManagerOHOS>(
      update, node_id_delegate_, test_browser_accessibility_delegate_.get());
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(1));
  ASSERT_NE(n, nullptr);

  std::u16string result = u"existing";
  n->AppendTextToString(u"new", &result);
  EXPECT_EQ(result, u"existing, new");
}

TEST_F(BrowserAccessibilityOHOSTest, Instance_OnLocationChanged) {
  ui::AXNodeData root;
  root.id = 1;
  root.role = ax::mojom::Role::kRootWebArea;

  ui::AXNodeData button;
  button.id = 2;
  button.role = ax::mojom::Role::kButton;

  root.child_ids = {2};
  ui::AXTreeUpdate update = ui::MakeAXTreeUpdateForTesting(root, button);
  auto recording_delegate = std::make_unique<RecordingAXDelegate>();
  ui::AXPlatformTreeManagerDelegate* delegate_ptr = recording_delegate.get();
  std::unique_ptr<ui::BrowserAccessibilityManager> manager =
      std::make_unique<ui::BrowserAccessibilityManagerOHOS>(update, node_id_delegate_, delegate_ptr);
  auto* n = static_cast<ui::BrowserAccessibilityOHOS*>(manager->GetFromID(2));
  ASSERT_NE(n, nullptr);

  // Disable the event dispatcher to prevent crashes caused by asynchronous distribution in the unit test environment.
  static_cast<ui::BrowserAccessibilityManagerOHOS*>(manager.get())->eventDispatcher_ = nullptr;

  n->OnLocationChanged();
  const auto& actions = static_cast<RecordingAXDelegate*>(delegate_ptr)->performed_actions();
  EXPECT_TRUE(actions.empty());
}
}  // namespace content