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

#include "ui/ozone/platform/headless/headless_screen_ohos.h"

#include <string_view>

#include "base/command_line.h"
#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/numerics/safe_conversions.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_split.h"
#include "base/task/single_thread_task_runner.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#include "ui/display/display_layout.h"
#include "ui/display/display_layout_builder.h"
#include "ui/display/util/display_util.h"
#include "ui/ozone/public/ozone_switches.h"

namespace ui {

namespace {
// ohos display defaults.
constexpr int64_t kHeadlessOhosDisplayId = 0;
constexpr float kHeadlessOhosDisplayScale = 1.0f;
constexpr gfx::Size kHeadlessOhosDisplaySize(-1, -1);

// Parse comma-separated screen width and height.
bool ParseScreenSizeOhos(const std::string& screen_size,
                         int* width,
                         int* height) {
  std::vector<std::string_view> width_and_height = base::SplitStringPiece(
      screen_size, ",", base::TRIM_WHITESPACE, base::SPLIT_WANT_NONEMPTY);
  if (width_and_height.size() != 2) {
    return false;
  }

  if (!base::StringToInt(width_and_height[0], width) ||
      !base::StringToInt(width_and_height[1], height)) {
    return false;
  }

  return true;
}

// LCOV_EXCL_START
gfx::Rect GetDisplayBoundsOhos() {
  gfx::Rect bounds(kHeadlessOhosDisplaySize);

  const base::CommandLine& command_line =
      *base::CommandLine::ForCurrentProcess();
  if (command_line.HasSwitch(switches::kOzoneOverrideScreenSize)) {
    int width, height;
    std::string screen_size =
        command_line.GetSwitchValueASCII(switches::kOzoneOverrideScreenSize);
    if (ParseScreenSizeOhos(screen_size, &width, &height)) {
      bounds.set_size(gfx::Size(width, height));
    }
  }

  return bounds;
}
// LCOV_EXCL_STOP

bool DisplayInfosTouch(const display::Display& a, const display::Display& b) {
  const gfx::Rect a_rect(a.native_origin(), a.GetSizeInPixel());
  const gfx::Rect b_rect(b.native_origin(), b.GetSizeInPixel());
  int max_left = std::max(a_rect.x(), b_rect.x());
  int max_top = std::max(a_rect.y(), b_rect.y());
  int min_right = std::min(a_rect.right(), b_rect.right());
  int min_bottom = std::min(a_rect.bottom(), b_rect.bottom());
  return (max_left == min_right && a_rect.y() <= b_rect.bottom() &&
          b_rect.y() <= a_rect.bottom()) ||
         (max_top == min_bottom && a_rect.x() <= b_rect.right() &&
          b_rect.x() <= a_rect.right());
}

std::vector<display::Display> FindAndRemoveTouchingDisplayInfos(
    const display::Display& parent_info,
    std::vector<display::Display>* display_infos) {
  const auto first_touching_it = std::partition(
      display_infos->begin(), display_infos->end(),
      [&](const auto& info) { return !DisplayInfosTouch(parent_info, info); });
  std::vector<display::Display> touching_display_infos(first_touching_it,
                                                       display_infos->end());
  display_infos->erase(first_touching_it, display_infos->end());
  return touching_display_infos;
}

int ScaleOffset(int unscaled_length, float scale_factor, int unscaled_offset) {
  if (unscaled_length == 0 || scale_factor == 0) {
    return 0;
  }
  float scaled_length = static_cast<float>(unscaled_length) / scale_factor;
  float percent =
      static_cast<float>(unscaled_offset) / static_cast<float>(unscaled_length);
  return base::ClampFloor(scaled_length * percent);
}

display::DisplayPlacement::Position CalculateDisplayPosition(
    const display::Display& parent,
    const display::Display& current) {
  const gfx::Rect parent_rect(parent.native_origin(), parent.GetSizeInPixel());
  const gfx::Rect current_rect(current.native_origin(),
                               current.GetSizeInPixel());
  int max_left = std::max(parent_rect.x(), current_rect.x());
  int max_top = std::max(parent_rect.y(), current_rect.y());
  int min_right = std::min(parent_rect.right(), current_rect.right());
  int min_bottom = std::min(parent_rect.bottom(), current_rect.bottom());
  if (max_left == min_right && max_top == min_bottom) {
    // Corner touching.
    if (parent_rect.bottom() == max_top) {
      return display::DisplayPlacement::Position::BOTTOM;
    }
    if (parent_rect.x() == max_left) {
      return display::DisplayPlacement::Position::LEFT;
    }

    return display::DisplayPlacement::Position::TOP;
  }
  if (max_left == min_right && parent_rect.y() <= current_rect.bottom() &&
      current_rect.y() <= parent_rect.bottom()) {
    // Vertical edge touching.
    return parent_rect.x() == max_left
               ? display::DisplayPlacement::Position::LEFT
               : display::DisplayPlacement::Position::RIGHT;
  }
  if (max_top == min_bottom && parent_rect.x() <= current_rect.right() &&
      current_rect.x() <= parent_rect.right()) {
    // Horizontal edge touching.
    return parent_rect.y() == max_top
               ? display::DisplayPlacement::Position::TOP
               : display::DisplayPlacement::Position::BOTTOM;
  }
  NOTREACHED() << "CalculateDisplayPosition relies on touching DisplayInfos.";
}

display::DisplayPlacement CalculateDisplayPlacement(
    const display::Display& parent,
    const display::Display& current) {
  DCHECK(DisplayInfosTouch(parent, current)) << "DisplayInfos must touch.";

  display::DisplayPlacement placement;
  placement.parent_display_id = parent.id();
  placement.display_id = current.id();
  placement.position = CalculateDisplayPosition(parent, current);

  int parent_begin = 0;
  int parent_end = 0;
  int current_begin = 0;
  int current_end = 0;
  const gfx::Rect parent_rect(parent.native_origin(), parent.GetSizeInPixel());
  const gfx::Rect current_rect(current.native_origin(),
                               current.GetSizeInPixel());
  switch (placement.position) {
    case display::DisplayPlacement::Position::TOP:
    case display::DisplayPlacement::Position::BOTTOM:
      parent_begin = parent_rect.x();
      parent_end = parent_rect.right();
      current_begin = current_rect.x();
      current_end = current_rect.right();
      break;
    case display::DisplayPlacement::Position::LEFT:
    case display::DisplayPlacement::Position::RIGHT:
      parent_begin = parent_rect.y();
      parent_end = parent_rect.bottom();
      current_begin = current_rect.y();
      current_end = current_rect.bottom();
      break;
  }

  parent_end -= parent_begin;
  current_begin -= parent_begin;
  current_end -= parent_begin;
  parent_begin = 0;

  if (parent_end == current_end) {
    placement.offset_reference =
        display::DisplayPlacement::OffsetReference::BOTTOM_RIGHT;
    placement.offset = 0;
  } else if (current_begin >= parent_begin && current_begin <= parent_end) {
    placement.offset =
        ScaleOffset(parent_end, parent.device_scale_factor(), current_begin);
  } else if (current_end >= parent_begin && current_end <= parent_end) {
    placement.offset_reference =
        display::DisplayPlacement::OffsetReference::BOTTOM_RIGHT;
    placement.offset = ScaleOffset(parent_end, parent.device_scale_factor(),
                                   parent_end - current_end);
  } else {
    DCHECK((parent_begin >= current_begin && parent_begin <= current_end));
    placement.offset =
        ScaleOffset(current_end - current_begin, current.device_scale_factor(),
                    current_begin);
  }
  return placement;
}
}  // namespace

// LCOV_EXCL_START
HeadlessScreenListener::HeadlessScreenListener(
    base::WeakPtr<HeadlessScreenOhos> headless_screen_ohos)
    : headless_screen_ohos_(headless_screen_ohos) {
  task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
  DCHECK(task_runner_.get());
}

void HeadlessScreenListener::OnCreate(OHOS::NWeb::DisplayId display_id) {
  DCHECK(task_runner_.get());
  if (task_runner_) {
    task_runner_->PostTask(FROM_HERE,
                           base::BindOnce(&HeadlessScreenOhos::OnDisplayCreate,
                                          headless_screen_ohos_, display_id));
  }
}

void HeadlessScreenListener::OnDestroy(OHOS::NWeb::DisplayId display_id) {
  DCHECK(task_runner_.get());
  if (task_runner_) {
    task_runner_->PostTask(FROM_HERE,
                           base::BindOnce(&HeadlessScreenOhos::OnDisplayDestroy,
                                          headless_screen_ohos_, display_id));
  }
}

void HeadlessScreenListener::OnChange(OHOS::NWeb::DisplayId display_id) {
  DCHECK(task_runner_.get());
  if (task_runner_) {
    task_runner_->PostTask(FROM_HERE,
                           base::BindOnce(&HeadlessScreenOhos::OnDisplayChange,
                                          headless_screen_ohos_, display_id));
  }
}

HeadlessScreenOhos::HeadlessScreenOhos() {
  if (!Initialize()) {
    display::Display display(kHeadlessOhosDisplayId);
    display.SetScaleAndBounds(kHeadlessOhosDisplayScale,
                              GetDisplayBoundsOhos());
    display_list_.AddDisplay(display, display::DisplayList::Type::PRIMARY);
  }
}

HeadlessScreenOhos::~HeadlessScreenOhos() {
  if (display_listener_id_ >= 0 && display_listener_ != nullptr &&
      display_manager_adapter_ != nullptr) {
    display_manager_adapter_->UnregisterDisplayListener(display_listener_id_);
  }
}

bool HeadlessScreenOhos::Initialize() {
  bool result = false;
  display_manager_adapter_ =
      OHOS::NWeb::OhosAdapterHelper::GetInstance().CreateDisplayMgrAdapter();
  if (!display_manager_adapter_) {
    LOG(ERROR) << __FUNCTION__ << " failed to create display manager adapter";
    return result;
  }
  result = FetchDisplays(display_list_);
  if (!result) {
    LOG(ERROR) << __FUNCTION__ << " failed to fetch display list";
  }
  display_listener_ =
      std::make_shared<HeadlessScreenListener>(weak_factory_.GetWeakPtr());
  if (display_listener_) {
    display_listener_id_ =
        display_manager_adapter_->RegisterDisplayListener(display_listener_);
    if (display_listener_id_ < 0) {
      LOG(ERROR) << __FUNCTION__ << " failed to register display listener";
    }
  } else {
    LOG(ERROR) << __FUNCTION__ << " failed to create display listener";
  }
  return result;
}
// LCOV_EXCL_STOP

void HeadlessScreenOhos::LayoutDisplays(std::vector<display::Display>& displays) {
  std::vector<display::Display> displays_remaining = displays;
  auto primary_display_iter = base::ranges::find_if(
      displays_remaining, [](const display::Display& display) {
        return display.native_origin().IsOrigin();
      });
  if (primary_display_iter == displays_remaining.end()) {
    return;
  }

  display::DisplayLayoutBuilder builder(primary_display_iter->id());
  std::vector<display::Display> available_parents = {
      *primary_display_iter};
  displays_remaining.erase(primary_display_iter);
  while (!available_parents.empty()) {
    const display::Display parent = available_parents.back();
    available_parents.pop_back();
    for (const auto& child :
         FindAndRemoveTouchingDisplayInfos(parent, &displays_remaining)) {
      builder.AddDisplayPlacement(CalculateDisplayPlacement(parent, child));
      available_parents.push_back(child);
    }
  }
  builder.Build()->ApplyToDisplayList(&displays, nullptr, 0);
}

bool HeadlessScreenOhos::FetchDisplays(display::DisplayList& displays) {
  if (!display_manager_adapter_) {
    return false;
  }
  OHOS::NWeb::DisplayId default_id =
      display_manager_adapter_->GetDefaultDisplayId();
  OHOS::NWeb::DisplayId primary_id = default_id;
  auto primary_display = display_manager_adapter_->GetPrimaryDisplay();
  if (primary_display) {
    primary_id = primary_display->GetId();
  }
  auto ohos_displays = display_manager_adapter_->GetAllDisplays();
  std::vector<display::Display> display_list;
  for (auto& ohos_display : ohos_displays) {
    display::Display dst_display;
    if (!ConvertDisplay(ohos_display, dst_display)) {
      continue;
    }
    display_list.push_back(dst_display);
  }
  if (display_list.empty()) {
    return false;
  }
  LayoutDisplays(display_list);
  for (auto& display : display_list) {
    if (static_cast<int64_t>(primary_id) == display.id()) {
      displays.AddOrUpdateDisplay(display,
                                  display::DisplayList::Type::PRIMARY);
    } else {
      displays.AddOrUpdateDisplay(display,
                                  display::DisplayList::Type::NOT_PRIMARY);
    }
  }
  return true;
}

bool HeadlessScreenOhos::ConvertDisplay(
    std::shared_ptr<OHOS::NWeb::DisplayAdapter> src_display,
    display::Display& dst_display) {
  if (!src_display) {
    return false;
  }
  auto display_id = static_cast<int64_t>(src_display->GetId());
  if (display_id < 0) {
    return false;
  }
  dst_display.set_id(display_id);
  float scale = src_display->GetVirtualPixelRatio();
  if (scale != 0) {
    int32_t x = src_display->GetX();
    int32_t y = src_display->GetY();
    int32_t width = src_display->GetWidth();
    int32_t height = src_display->GetHeight();
    gfx::Rect bounds(x, y, width, height);
    gfx::Rect configRect = GetDisplayBoundsOhos();
    if (configRect.size() != kHeadlessOhosDisplaySize) {
      bounds = configRect;
    }
    dst_display.SetScaleAndBounds(scale, bounds);

    int32_t available_width = src_display->GetAvailableWidth();
    int32_t available_height = src_display->GetAvailableHeight();
    gfx::Rect available(x, y, available_width, available_height);
    gfx::Rect work_area = gfx::ScaleToEnclosingRect(available, 1.0f / scale);
    work_area.Intersect(dst_display.bounds());
    dst_display.set_work_area(work_area);
  }
  OHOS::NWeb::RotationType rotation_type = src_display->GetRotation();
  switch (rotation_type) {
    case OHOS::NWeb::RotationType::ROTATION_0:
      dst_display.set_rotation(display::Display::ROTATE_0);
      break;
    case OHOS::NWeb::RotationType::ROTATION_90:
      dst_display.set_rotation(display::Display::ROTATE_90);
      break;
    case OHOS::NWeb::RotationType::ROTATION_180:
      dst_display.set_rotation(display::Display::ROTATE_180);
      break;
    case OHOS::NWeb::RotationType::ROTATION_270:
      dst_display.set_rotation(display::Display::ROTATE_270);
      break;
    default:
      break;
  }
  return true;
}

void HeadlessScreenOhos::OnDisplayEvent(const std::string& event,
                                        const int64_t display_id) {
  if (event == "create" || event == "change") {
    display::DisplayList current_displays;
    if (!FetchDisplays(current_displays)) {
      LOG(ERROR) << __FUNCTION__ << " failed to fetch display list on event "
                 << event;
      return;
    }
    auto iter = current_displays.FindDisplayById(display_id);
    if (iter != current_displays.displays().end()) {
      if (display_id == current_displays.GetPrimaryDisplayIterator()->id()) {
        display_list_.AddOrUpdateDisplay(*iter,
                                         display::DisplayList::Type::PRIMARY);
      } else {
        display_list_.AddOrUpdateDisplay(
            *iter, display::DisplayList::Type::NOT_PRIMARY);
      }
    }
  } else if (event == "destroy") {
    auto it = display_list_.FindDisplayById(display_id);
    if (it != display_list_.displays().end()) {
      display_list_.RemoveDisplay(display_id);
    }
  }
}

// LCOV_EXCL_START
void HeadlessScreenOhos::OnDisplayCreate(
    const OHOS::NWeb::DisplayId display_id) {
  if (!weak_factory_.HasWeakPtrs()) {
    LOG(ERROR) << "HeadlessScreenOhos has been destroyed.";
    return;
  }
  auto id = static_cast<int64_t>(display_id);
  OnDisplayEvent("create", id);
}

void HeadlessScreenOhos::OnDisplayDestroy(
    const OHOS::NWeb::DisplayId display_id) {
  if (!weak_factory_.HasWeakPtrs()) {
    LOG(ERROR) << "HeadlessScreenOhos has been destroyed.";
    return;
  }
  auto id = static_cast<int64_t>(display_id);
  OnDisplayEvent("destroy", id);
}

void HeadlessScreenOhos::OnDisplayChange(
    const OHOS::NWeb::DisplayId display_id) {
  if (!weak_factory_.HasWeakPtrs()) {
    LOG(ERROR) << "HeadlessScreenOhos has been destroyed.";
    return;
  }
  auto id = static_cast<int64_t>(display_id);
  OnDisplayEvent("change", id);
}

const std::vector<display::Display>& HeadlessScreenOhos::GetAllDisplays()
    const {
  return display_list_.displays();
}

display::Display HeadlessScreenOhos::GetPrimaryDisplay() const {
  auto iter = display_list_.GetPrimaryDisplayIterator();
  DCHECK(iter != display_list_.displays().end());
  return *iter;
}

void HeadlessScreenOhos::AddObserver(display::DisplayObserver* observer) {
  display_list_.AddObserver(observer);
}

void HeadlessScreenOhos::RemoveObserver(display::DisplayObserver* observer) {
  display_list_.RemoveObserver(observer);
}
// LCOV_EXCL_STOP
}  // namespace ui
