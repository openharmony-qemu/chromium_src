/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CONTENT_BROWSER_ACCESSIBILITY_BROWSER_ACCESSIBILITY_OHOS_H_
#define CONTENT_BROWSER_ACCESSIBILITY_BROWSER_ACCESSIBILITY_OHOS_H_

#include <list>

#include "content/common/content_export.h"
#include "ui/accessibility/platform/ax_platform_node.h"
#include "ui/accessibility/platform/ax_platform_node_ohos.h"
#include "ui/accessibility/platform/browser_accessibility.h"

namespace ui {

class BrowserAccessibilityManager;

}  // namespace ui

namespace content {

enum FocusMoveDirection : int32_t {
  DIRECTION_INVALID = 0,
  UP = 0x00000001,
  DOWN = 0x00000002,
  LEFT = 0x00000004,
  RIGHT = 0x00000008,
  FORWARD = 0x00000010,
  BACKWARD = 0x00000020,
};

enum class AceTextCategory {
  INPUT_TYPE_DEFAULT = 0,
  INPUT_TYPE_TEXT,
  INPUT_TYPE_EMAIL,
  INPUT_TYPE_DATE,
  INPUT_TYPE_TIME,
  INPUT_TYPE_NUMBER,
  INPUT_TYPE_PASSWORD,
  INPUT_TYPE_PHONENUMBER
};

const struct AceTextCategoryMapping {
  const std::string ace_text;
  const AceTextCategory ace_type;
} kAceTextCategoryMap[] = {
    {"input", AceTextCategory::INPUT_TYPE_DEFAULT},
    {"url", AceTextCategory::INPUT_TYPE_DEFAULT},
    {"text", AceTextCategory::INPUT_TYPE_TEXT},
    {"search", AceTextCategory::INPUT_TYPE_TEXT},
    {"month", AceTextCategory::INPUT_TYPE_DATE},
    {"week", AceTextCategory::INPUT_TYPE_DATE},
    {"date", AceTextCategory::INPUT_TYPE_DATE},
    {"time", AceTextCategory::INPUT_TYPE_TIME},
    {"datetime", AceTextCategory::INPUT_TYPE_TIME},
    {"datetime-local", AceTextCategory::INPUT_TYPE_TIME},
    {"email", AceTextCategory::INPUT_TYPE_EMAIL},
    {"number", AceTextCategory::INPUT_TYPE_NUMBER},
    {"password", AceTextCategory::INPUT_TYPE_PASSWORD},
    {"tel", AceTextCategory::INPUT_TYPE_PHONENUMBER},
};

const int32_t WEIGHTED_VALUE = 13;
 
// A `BrowserAccessibility` object represents one node in the accessibility tree
// on the browser side. It wraps an `AXNode` and assists in exposing
// web-specific information from the node. It's owned by a
// `BrowserAccessibilityManager`.
//
// There are subclasses of BrowserAccessibility for each platform where we
// implement some of the native accessibility APIs that are only specific to the
// Web.
class CONTENT_EXPORT BrowserAccessibilityOHOS : public ui::BrowserAccessibility {
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

  int GetContentTrueMessage() const;

  std::string GetContentInvalidErrorMessage() const;

  AceTextCategory GetInputType(const std::string& name) const;

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

  static BrowserAccessibilityOHOS* GetFromAccessibilityId(
      int64_t accessibility_id);

  ui::AXPlatformNode* GetAXPlatformNode() const override;
  
  gfx::NativeViewAccessible GetNativeViewAccessible() override;

  const BrowserAccessibilityOHOS* GetAccessibilityNodeByFocusMove(
      int32_t direction) const;

  float RangeMin() const;

  float RangeMax() const;

  float RangeCurrentValue() const;

  std::string GetRoleString() const;

  std::string GetTargetUrl() const;

  std::u16string GetTextContentUTF16() const override;

protected:
  BrowserAccessibilityOHOS(ui::BrowserAccessibilityManager* manager,
                           ui::AXNode* node);

  friend class ui::BrowserAccessibility;

private:
  bool ShouldExposeValueAsName() const;

  bool IsRangeControlWithoutAriaValueText() const;

  bool HasOnlyTextChildren() const;

  bool HasOnlyTextAndImageChildren() const;

  bool HasListMarkerChild() const;

  const char* AXRoleToOHOSClassName(ax::mojom::Role role,
                                    bool has_parent) const;

  void AddFocusableNode(
      std::list<const BrowserAccessibilityOHOS*>& node_list) const;

  const BrowserAccessibilityOHOS* FindNodeInRelativeDirection(
      const std::list<const BrowserAccessibilityOHOS*>& node_list,
      int32_t direction) const;

  const BrowserAccessibilityOHOS* FindNodeInAbsoluteDirection(
      const std::list<const BrowserAccessibilityOHOS*>& node_list,
      int32_t direction) const;

  const BrowserAccessibilityOHOS* GetNextFocusableNode(
      const std::list<const BrowserAccessibilityOHOS*>& node_list) const;

  const BrowserAccessibilityOHOS* GetPreviousFocusableNode(
      const std::list<const BrowserAccessibilityOHOS*>& node_list) const;

  typedef base::RepeatingCallback<bool(const std::u16string& partial)>
      EarlyExitPredicate;

  std::u16string CombineTextAndValue(const std::u16string& value,
      const std::u16string& name) const;
  bool ShouldUseChildTextContent() const;
  bool ShouldUseUrlAsText() const;
  std::u16string GetSubstringTextContentUTF16(
      std::optional<EarlyExitPredicate>) const;

  void AppendTextToString(std::u16string extra_text,
                          std::u16string* string) const;

  static bool CheckRectBeam(const gfx::Rect& node_rect,
                            const gfx::Rect& item_rect,
                            const int32_t direction);

  static bool IsToDirectionOf(const gfx::Rect& node_rect,
                              const gfx::Rect& item_rect,
                              const int32_t direction);

  static double MajorAxisDistanceToFarEdge(const gfx::Rect& node_rect,
                                           const gfx::Rect& item_rect,
                                           const int32_t direction);

  static double MajorAxisDistance(const gfx::Rect& node_rect,
                                  const gfx::Rect& item_rect,
                                  const int32_t direction);

  static double MinorAxisDistance(const gfx::Rect& node_rect,
                                  const gfx::Rect& item_rect,
                                  const int32_t direction);

  static double GetWeightedDistanceFor(double major_axis_distance,
                                       double minor_axis_distance);

  static bool IsCandidateRect(const gfx::Rect& node_rect,
                              const gfx::Rect& item_rect,
                              const int32_t direction);

  // Check whether rect1 is outright better than rect2.
  static bool OutrightBetter(const gfx::Rect& node_rect,
                             const int32_t direction,
                             const gfx::Rect& Rect1,
                             const gfx::Rect& Rect2);

  static bool CheckBetterRect(const gfx::Rect& node_rect,
                              const int32_t direction,
                              const gfx::Rect& item_rect,
                              const gfx::Rect& tempBest);

  int64_t accessibility_id_ = -1;
  // Ohos-specific representation of this node.
  raw_ptr<ui::AXPlatformNodeOHOS> platform_node_ = nullptr;
};
}  // namespace content

#endif  // CONTENT_BROWSER_ACCESSIBILITY_BROWSER_ACCESSIBILITY_OHOS_H_
