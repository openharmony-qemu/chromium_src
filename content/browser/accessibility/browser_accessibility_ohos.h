/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef CONTENT_BROWSER_ACCESSIBILITY_BROWSER_ACCESSIBILITY_OHOS_H_
#define CONTENT_BROWSER_ACCESSIBILITY_BROWSER_ACCESSIBILITY_OHOS_H_

#include <list>
#include "content/browser/accessibility/browser_accessibility.h"

namespace content {
namespace {
constexpr int kDefaultStepTicksForSliders = 20;
}
class BrowserAccessibilityManager;

enum class ScrollDirection {
    FORWARD = 0,
    BACKWARD,
    UP,
    DOWN,
    LEFT,
    RIGHT
};

// A `BrowserAccessibility` object represents one node in the accessibility tree
// on the browser side. It wraps an `AXNode` and assists in exposing
// web-specific information from the node. It's owned by a
// `BrowserAccessibilityManager`.
//
// There are subclasses of BrowserAccessibility for each platform where we
// implement some of the native accessibility APIs that are only specific to the
// Web.
class CONTENT_EXPORT BrowserAccessibilityOHOS : public BrowserAccessibility {
 public:
  BrowserAccessibilityOHOS(const BrowserAccessibilityOHOS&) = delete;

  BrowserAccessibilityOHOS& operator=(const BrowserAccessibilityOHOS&) = delete;

  ~BrowserAccessibilityOHOS() override;

  int64_t GetAccessibilityId() const;

  bool IsEnabled() const;

  std::string GetHint() const;

  bool IsHint() const;

  bool IsChecked() const;

  bool IsSelected() const;

  bool IsScrollable() const;

  bool IsCheckable() const;

  bool IsMultiLine() const;

  bool CanOpenPopup() const;

  bool IsContentInvalid() const;

  std::string GetContentInvalidErrorMessage() const;

  int32_t OHOSInputType() const;

  int32_t OHOSLiveRegionType() const;

  int32_t GetSelectionStart() const;

  int32_t GetSelectionEnd() const;

  size_t GetItemCount() const;

  int32_t RowCount() const;

  int32_t ColumnCount() const;

  int32_t RowIndex() const;

  int32_t RowSpan() const;

  int32_t ColumnIndex() const;

  int32_t ColumnSpan() const;

  bool IsHierarchical() const;

  bool IsCollection() const;

  bool IsCollectionItem() const;

  bool IsHeading() const;

  bool IsLink() const;

  BrowserAccessibilityOHOS* GetAccessibilityNodeByFocusMove(int32_t direction) const;

  float RangeMin() const;

  float RangeMax() const;

  float RangeCurrentValue() const;

  std::string GetRoleString() const;

  std::string GetTargetUrl() const;

  std::u16string GetTextContentUTF16() const override;

  bool IsClickable() const override;

  bool IsFocusable() const override;

  bool IsRangeControlWithoutAriaValueText() const;

  bool IsTableHeader() const;

  bool HasNonEmptyValue() const;
 
  bool IsScrollSupported() const;

  bool CanScrollForward() const;
  bool CanScrollBackward() const;
  bool CanScrollUp() const;
  bool CanScrollDown() const;
  bool CanScrollLeft() const;
  bool CanScrollRight() const;
  int GetScrollX() const;
  int GetScrollY() const;
  int GetMinScrollX() const;
  int GetMinScrollY() const;
  int GetMaxScrollX() const;
  int GetMaxScrollY() const;

  void Scroll(const ax::mojom::Action& action) const;

  static BrowserAccessibilityOHOS* GetFromAccessibilityId(
      int64_t accessibility_id);

  bool IsAccessibilityGroup() const;

  bool IsIgnoredContainer() const;

  bool IsEmptyContainer() const;

  int64_t GetParentId() const;

  void GetChildrenIds(std::vector<int64_t>& childrenIds) const;

  void SetChildrenIds(const std::vector<int64_t>& childrenIds);

  bool Scroll(ScrollDirection direction, bool is_page_scroll) const;

 protected:
  BrowserAccessibilityOHOS(BrowserAccessibilityManager* manager,
                           ui::AXNode* node);

  friend class BrowserAccessibility;

 private:
  bool ShouldExposeValueAsName() const;

  bool HasOnlyTextChildren() const;

  bool HasOnlyDirectTextChildren() const;

  bool HasOnlyTextAndImageChildren() const;

  bool HasClickableChildren() const;

  bool HasListMarkerChild() const;

  bool IsHeadingLink() const;

  void AddFocusableNode(
      std::list<BrowserAccessibilityOHOS*>& nodeList) const;

  BrowserAccessibilityOHOS* FindNodeInRelativeDirection(
      const std::list<BrowserAccessibilityOHOS*>& nodeList,
      int32_t direction) const;

  BrowserAccessibilityOHOS* FindNodeInAbsoluteDirection(
      const std::list<BrowserAccessibilityOHOS*>& nodeList,
      int32_t direction) const;

  BrowserAccessibilityOHOS* GetNextFocusableNode(
      const std::list<BrowserAccessibilityOHOS*>& nodeList) const;

  BrowserAccessibilityOHOS* GetPreviousFocusableNode(
      const std::list<BrowserAccessibilityOHOS*>& nodeList) const;

  typedef base::RepeatingCallback<bool(const std::u16string& partial)>
      EarlyExitPredicate;
  std::u16string GetSubstringTextContentUTF16(
      absl::optional<EarlyExitPredicate>) const;

  void AppendTextToString(std::u16string extra_text,
                          std::u16string* string) const;

  static bool CheckRectBeam(const gfx::Rect& nodeRect,
                            const gfx::Rect& itemRect,
                            const int32_t direction);

  static bool IsToDirectionOf(const gfx::Rect& nodeRect,
                              const gfx::Rect& itemRect,
                              const int32_t direction);

  static double MajorAxisDistanceToFarEdge(const gfx::Rect& nodeRect,
                                           const gfx::Rect& itemRect,
                                           const int32_t direction);

  static double MajorAxisDistance(const gfx::Rect& nodeRect,
                                  const gfx::Rect& itemRect,
                                  const int32_t direction);

  static double MinorAxisDistance(const gfx::Rect& nodeRect,
                                  const gfx::Rect& itemRect,
                                  const int32_t direction);

  static double GetWeightedDistanceFor(double majorAxisDistance,
                                       double minorAxisDistance);

  static bool IsCandidateRect(const gfx::Rect& nodeRect,
                              const gfx::Rect& itemRect,
                              const int32_t direction);

  // Check whether rect1 is outright better than rect2.
  static bool OutrightBetter(const gfx::Rect& nodeRect,
                             const int32_t direction,
                             const gfx::Rect& Rect1,
                             const gfx::Rect& Rect2);

  static bool CheckBetterRect(const gfx::Rect& nodeRect,
                              const int32_t direction,
                              const gfx::Rect& itemRect,
                              const gfx::Rect& tempBest);

  int64_t accessibility_id_ = -1;

  std::vector<int64_t> childrenIds_;
};
}  // namespace content

#endif  // CONTENT_BROWSER_ACCESSIBILITY_BROWSER_ACCESSIBILITY_OHOS_H_