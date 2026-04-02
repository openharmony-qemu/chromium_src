/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "nweb_render_handler.h"

#include <sys/time.h>

#include <cerrno>
#include <cstring>

#include "arkweb/build/features/features.h"
#include "arkweb/chromium_ext/url/ohos/log_utils.h"
#include "base/command_line.h"
#include "base/logging.h"
#include "base/numerics/safe_math.h"
#include "content/public/common/content_switches.h"
#include "nweb_delegate_interface.h"
#include "nweb_gesture_event_result_impl.h"
#include "nweb_mouse_event_result_impl.h"
#include "nweb_touch_handle_hot_zone_impl.h"
#include "nweb_touch_handle_state_impl.h"
#include "arkweb/ohos_adapter_ndk/interfaces/ohos_adapter_helper.h"
#include "res_sched_client_adapter.h"
#include "third_party/bounds_checking_function/include/securec.h"
#if BUILDFLAG(ARKWEB_DRAG_DROP)
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/ohos/sys_info_utils_ext.h"
#include "cef/libcef/common/drag_data_impl.h"
#include "content/public/common/drop_data.h"
#if BUILDFLAG(ARKWEB_REPORT_SYS_EVENT)
#include "event_reporter.h"
#endif
#include "nweb_drag_data.h"
#include "nweb_drag_data_impl.h"
#endif  // BUILDFLAG(ARKWEB_DRAG_DROP)
#if BUILDFLAG(ARKWEB_AI)
#include "cef/libcef/browser/image_impl.h"
#include "nweb_agent_handler.h"
#include "ui/gfx/image/image_skia.h"
#endif  // #if BUILDFLAG(ARKWEB_AI)

namespace {
#if BUILDFLAG(ARKWEB_AI)
constexpr size_t kWordSelectionOffsetSize = 2;
#endif  // ARKWEB_AI

#if BUILDFLAG(ARKWEB_SCREEN_SIZE)
constexpr int32_t APPLICATION_API_20 = 20;
#endif  // #if BUILDFLAG(ARKWEB_SCREEN_SIZE)

cef_screen_orientation_type_t ConvertOrientationType(
    OHOS::NWeb::DisplayOrientation type) {
  switch (type) {
    case OHOS::NWeb::DisplayOrientation::PORTRAIT:
      return cef_screen_orientation_type_t::PORTRAIT_PRIMARY;
    case OHOS::NWeb::DisplayOrientation::LANDSCAPE:
      return cef_screen_orientation_type_t::LANDSCAPE_PRIMARY;
    case OHOS::NWeb::DisplayOrientation::PORTRAIT_INVERTED:
      return cef_screen_orientation_type_t::PORTRAIT_SECONDARY;
    case OHOS::NWeb::DisplayOrientation::LANDSCAPE_INVERTED:
      return cef_screen_orientation_type_t::LANDSCAPE_SECONDARY;
    default:
      return cef_screen_orientation_type_t::UNDEFINED;
  }
}

cef_screen_orientation_type_t SwapPortraitAndLandscape(
    cef_screen_orientation_type_t type) {
  switch (type) {
    case cef_screen_orientation_type_t::PORTRAIT_PRIMARY:
      return cef_screen_orientation_type_t::LANDSCAPE_PRIMARY;
    case cef_screen_orientation_type_t::PORTRAIT_SECONDARY:
      return cef_screen_orientation_type_t::LANDSCAPE_SECONDARY;
    case cef_screen_orientation_type_t::LANDSCAPE_PRIMARY:
      return cef_screen_orientation_type_t::PORTRAIT_PRIMARY;
    case cef_screen_orientation_type_t::LANDSCAPE_SECONDARY:
      return cef_screen_orientation_type_t::PORTRAIT_SECONDARY;
    default:
      return cef_screen_orientation_type_t::UNDEFINED;
  }
}

cef_screen_orientation_type_t SwapPrimaryAndSecondary(
    cef_screen_orientation_type_t type) {
  switch (type) {
    case cef_screen_orientation_type_t::PORTRAIT_PRIMARY:
      return cef_screen_orientation_type_t::PORTRAIT_SECONDARY;
    case cef_screen_orientation_type_t::PORTRAIT_SECONDARY:
      return cef_screen_orientation_type_t::PORTRAIT_PRIMARY;
    case cef_screen_orientation_type_t::LANDSCAPE_PRIMARY:
      return cef_screen_orientation_type_t::LANDSCAPE_SECONDARY;
    case cef_screen_orientation_type_t::LANDSCAPE_SECONDARY:
      return cef_screen_orientation_type_t::LANDSCAPE_PRIMARY;
    default:
      return cef_screen_orientation_type_t::UNDEFINED;
  }
}

enum RotationAngels {
  ROTATION_0 = 0,
  ROTATION_90 = 90,
  ROTATION_180 = 180,
  ROTATION_270 = 270,
};

uint16_t ConvertRotationAngel(OHOS::NWeb::RotationType type) {
  // Notice: 90 and 270 is reverse.
  // The DMS system returns angles in a clockwise direction,
  // while the W3C standard uses a counterclockwise direction.
  bool use_counter_clockwise = true;
  switch (type) {
    case OHOS::NWeb::RotationType::ROTATION_0:
      return RotationAngels::ROTATION_0;
    case OHOS::NWeb::RotationType::ROTATION_90:
      return use_counter_clockwise ? RotationAngels::ROTATION_270
                                   : RotationAngels::ROTATION_90;
    case OHOS::NWeb::RotationType::ROTATION_180:
      return RotationAngels::ROTATION_180;
    case OHOS::NWeb::RotationType::ROTATION_270:
      return use_counter_clockwise ? RotationAngels::ROTATION_90
                                   : RotationAngels::ROTATION_270;
    default:
      return RotationAngels::ROTATION_0;
  }
}
}  // namespace

namespace OHOS::NWeb {
typedef std::map<std::string, std::string> AttributesMap;

class NWebNativeEmbedInfoImpl : public NWebNativeEmbedInfo {
 public:
  NWebNativeEmbedInfoImpl() = default;

  explicit NWebNativeEmbedInfoImpl(
      int32_t width,
      int32_t height,
      const std::string& id,
      const std::string& src,
      const std::string& url,
      const std::string& type,
      const std::string& tag,
      const std::map<std::string, std::string>& params,
      int32_t x,
      int32_t y);
  ~NWebNativeEmbedInfoImpl() = default;

  int32_t GetWidth() override { return width_; }

  int32_t GetHeight() override { return height_; }

  std::string GetId() override { return id_; }

  std::string GetSrc() override { return src_; }

  std::string GetUrl() override { return url_; }

  std::string GetType() override { return type_; }

  std::string GetTag() override { return tag_; }

  std::map<std::string, std::string> GetParams() override { return params_; }

  int32_t GetX() override { return x_; }

  int32_t GetY() override { return y_; }

 private:
  int32_t width_ = 0;
  int32_t height_ = 0;
  std::string id_;
  std::string src_;
  std::string url_;
  std::string type_;
  std::string tag_;
  std::map<std::string, std::string> params_;
  int32_t x_ = 0;
  int32_t y_ = 0;
};

NWebNativeEmbedInfoImpl::NWebNativeEmbedInfoImpl(
    int32_t width,
    int32_t height,
    const std::string& id,
    const std::string& src,
    const std::string& url,
    const std::string& type,
    const std::string& tag,
    const std::map<std::string, std::string>& params,
    int32_t x,
    int32_t y)
    : width_(width),
      height_(height),
      id_(id),
      src_(src),
      url_(url),
      type_(type),
      tag_(tag),
      params_(params),
      x_(x),
      y_(y) {}

class NWebNativeEmbedDataInfoImpl : public NWebNativeEmbedDataInfo {
 public:
  NWebNativeEmbedDataInfoImpl() = default;
  ~NWebNativeEmbedDataInfoImpl() = default;

  NativeEmbedStatus GetStatus() override { return status_; }

  void SetStatus(NativeEmbedStatus status) { status_ = status; }

  std::string GetEmbedId() override { return embedId_; }

  void SetEmbedId(const std::string& embedId) { embedId_ = embedId; }

  std::string GetSurfaceId() override { return surfaceId_; }

  void SetSurfaceId(const std::string& surfaceId) { surfaceId_ = surfaceId; }

  std::shared_ptr<NWebNativeEmbedInfo> GetNativeEmbedInfo() override {
    return info_;
  }

  void SetNativeEmbedInfo(std::shared_ptr<NWebNativeEmbedInfo> info) {
    info_ = info;
  }

 private:
  NativeEmbedStatus status_ = NativeEmbedStatus::CREATE;
  std::string surfaceId_;
  std::string embedId_;
  std::shared_ptr<NWebNativeEmbedInfo> info_ = nullptr;
};

class NWebNativeEmbedTouchEventImpl : public NWebNativeEmbedTouchEvent {
 public:
  NWebNativeEmbedTouchEventImpl() = default;
  ~NWebNativeEmbedTouchEventImpl() = default;

  float GetX() override { return x_; }

  void SetX(float x) { x_ = x; }

  float GetY() override { return y_; }

  void SetY(float y) { y_ = y; }

  int32_t GetId() override { return id_; }

  void SetId(int32_t id) { id_ = id; }

  TouchType GetType() override { return type_; }

  void SetType(TouchType type) { type_ = type; }

  float GetOffsetX() override { return offsetX_; }

  void SetOffsetX(float offsetX) { offsetX_ = offsetX; }

  float GetOffsetY() override { return offsetY_; }

  void SetOffsetY(float offsetY) { offsetY_ = offsetY; }

  float GetScreenX() override { return screenX_; }

  void SetScreenX(float screenX) { screenX_ = screenX; }

  float GetScreenY() override { return screenY_; }

  void SetScreenY(float screenY) { screenY_ = screenY; }

  std::string GetEmbedId() override { return embedId_; }

  void SetEmbedId(const std::string& embedId) { embedId_ = embedId; }

  std::shared_ptr<NWebGestureEventResult> GetResult() override {
    return result_;
  }
  void SetResult(const std::shared_ptr<NWebGestureEventResult> result) {
    result_ = result;
  }

 private:
  std::string embedId_;
  int32_t id_ = 0;
  float x_ = 0;
  float y_ = 0;
  float offsetX_ = 0;
  float offsetY_ = 0;
  float screenX_ = 0;
  float screenY_ = 0;
  TouchType type_ = TouchType::DOWN;
  std::shared_ptr<NWebGestureEventResult> result_;
};

class NWebNativeEmbedMouseEventImpl : public NWebNativeEmbedMouseEvent {
 public:
  NWebNativeEmbedMouseEventImpl() = default;
  ~NWebNativeEmbedMouseEventImpl() = default;

  float GetX() override { return x_; }

  void SetX(float x) { x_ = x; }

  float GetY() override { return y_; }

  void SetY(float y) { y_ = y; }

  bool IsHitNativeArea() override {
    return isHitNativeArea_;
  }

  void SetIsHitNativeArea(bool isHitNativeArea) {
    isHitNativeArea_ = isHitNativeArea;
  }

  MouseType GetType() override { return type_; }

  void SetType(MouseType type) { type_ = type; }

  MouseButton GetButton() override { return button_; }

  void SetButton(MouseButton button) { button_ = button; }

  float GetOffsetX() override { return offsetX_; }

  void SetOffsetX(float offsetX) { offsetX_ = offsetX; }

  float GetOffsetY() override { return offsetY_; }

  void SetOffsetY(float offsetY) { offsetY_ = offsetY; }

  float GetScreenX() override { return screenX_; }

  void SetScreenX(float screenX) { screenX_ = screenX; }

  float GetScreenY() override { return screenY_; }

  void SetScreenY(float screenY) { screenY_ = screenY; }

  std::string GetEmbedId() override { return embedId_; }

  void SetEmbedId(const std::string& embedId) { embedId_ = embedId; }

  std::shared_ptr<NWebMouseEventResult> GetResult() override {
    return result_;
  }
  void SetResult(const std::shared_ptr<NWebMouseEventResult> result) {
    result_ = result;
  }

 private:
  std::string embedId_;
  bool isHitNativeArea_ = false;
  float x_ = 0.0;
  float y_ = 0.0;
  float offsetX_ = 0.0;
  float offsetY_ = 0.0;
  float screenX_ = 0.0;
  float screenY_ = 0.0;
  MouseType type_ = MouseType::PRESS;
  MouseButton button_ = MouseButton::NONE_BUTTON;
  std::shared_ptr<NWebMouseEventResult> result_;
};

class NWebNativeEmbedParamItemImpl : public NWebNativeEmbedParamItem {
 public:
  NWebNativeEmbedParamItemImpl() = default;
  ~NWebNativeEmbedParamItemImpl() = default;

  NativeEmbedParamStatus GetStatus() override { return status_; }

  void SetStatus(NativeEmbedParamStatus status) { status_ = status; }

  std::string GetId() override { return id_; }

  void SetId(const std::string& id) { id_ = id; }

  std::string GetName() override { return name_; }

  void SetName(const std::string& name) { name_ = name; }

  std::string GetValue() override { return value_; }

  void SetValue(const std::string& value) { value_ = value; }

 private:
  NativeEmbedParamStatus status_ = NativeEmbedParamStatus::ADD;
  std::string id_;
  std::string name_;
  std::string value_;
};

class NWebNativeEmbedParamDataInfoImpl : public NWebNativeEmbedParamDataInfo {
 public:
  NWebNativeEmbedParamDataInfoImpl() = default;
  ~NWebNativeEmbedParamDataInfoImpl() = default;

  std::string GetEmbedId() override { return embedId_; }

  void SetEmbedId(const std::string& embedId) { embedId_ = embedId; }

  std::string GetObjectAttributeId() override { return objectAttributeId_; }

  void SetObjectAttributeId(const std::string& objectAttributeId) {
    objectAttributeId_ = objectAttributeId;
  }

  std::vector<std::shared_ptr<NWebNativeEmbedParamItem>> GetParamItems() override { return paramItems_; }

  void SetParamItems(const std::vector<std::shared_ptr<NWebNativeEmbedParamItem>>& paramItems) {
    paramItems_ = paramItems;
  }

 private:
  std::string embedId_;
  std::string objectAttributeId_;
  std::vector<std::shared_ptr<NWebNativeEmbedParamItem>> paramItems_;
};

// static
CefRefPtr<NWebRenderHandler> NWebRenderHandler::Create() {
  CefRefPtr<NWebRenderHandler> renderHandler(new NWebRenderHandler());
  if (renderHandler == nullptr) {
    LOG(ERROR) << "fail to create NWebRenderHandler instance";
    return nullptr;
  }
  return renderHandler;
}

void NWebRenderHandler::RegisterNativeScrollCallback(
    std::function<void(double, double)>&& callback) {
  on_scroll_cb_ = std::move(callback);
}

void NWebRenderHandler::RegisterRenderCb(
    std::function<void(const char*)> render_update_cb) {
  render_update_cb_ = render_update_cb;
}

void NWebRenderHandler::RegisterNWebHandler(
    std::shared_ptr<NWebHandler> handler) {
  handler_ = std::weak_ptr<NWebHandler>(handler);
}

#if BUILDFLAG(ARKWEB_AI)
void NWebRenderHandler::RegisterNWebAgentHandler(
    std::shared_ptr<NWebAgentHandler> handler) {
  nweb_agent_handler_ = handler;
}
#endif  // #if BUILDFLAG(ARKWEB_AI)

void NWebRenderHandler::SetInputMethodClient(
    CefRefPtr<NWebInputMethodClient> client) {
  inputmethod_client_ = client;
}
#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
void NWebRenderHandler::SendDynamicFrameLossEvent(CefRefPtr<CefBrowser> browser,
                                                  const CefString& sceneId,
                                                  bool isStart) {
  if (auto handler = handler_.lock()) {
    handler->ReportDynamicFrameLossEvent(sceneId, isStart);
  }
}
#endif
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void NWebRenderHandler::SetNWebDelegateInterface(
    std::shared_ptr<NWebDelegateInterface> client) {
  delegate_interface_ = std::weak_ptr<NWebDelegateInterface>(client);
}

void NWebRenderHandler::OnOverscroll(CefRefPtr<CefBrowser> browser,
                                     const float x,
                                     const float y) {
  if (auto handler = handler_.lock()) {
    handler->OnOverScroll(x, y);
  }
#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
  has_over_scroll_ = true;
#endif
}

void NWebRenderHandler::OnSelectionChanged(CefRefPtr<CefBrowser> browser,
                                           const CefString& text,
                                           const CefRange& selected_range) {
  if (inputmethod_client_) {
    inputmethod_client_->OnSelectionChanged(browser, text, selected_range);
  }
}

void NWebRenderHandler::OnCursorUpdate(CefRefPtr<CefBrowser> browser,
                                       const CefRect& rect) {
  if (!inputmethod_client_) {
    LOG(ERROR)
        << "NWebRenderHandler::OnCursorUpdate inputmethod_client_ is nullptr";
    return;
  }
  inputmethod_client_->OnCursorUpdate(rect);
  if (auto handler = handler_.lock()) {
    handler->OnCursorUpdate(rect.x * screen_info_.display_ratio,
                            rect.y * screen_info_.display_ratio,
                            rect.width * screen_info_.display_ratio,
                            rect.height * screen_info_.display_ratio);
  }
}

void NWebRenderHandler::SetFocusStatus(bool focus_status) {
  if (inputmethod_client_) {
    inputmethod_client_->SetFocusStatus(focus_status);
  }
}

void NWebRenderHandler::OnUpdateTextInputStateCalled(
    CefRefPtr<CefBrowser> browser,
    const CefString& text,
    const CefRange& selected_range,
    const CefRange& compositon_range) {
  if (inputmethod_client_) {
    inputmethod_client_->OnUpdateTextInputStateCalled(
        browser, text, selected_range, compositon_range);
  }
}

#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
void NWebRenderHandler::GetScrollOffset(float& x, float& y) {
  x = scroll_offset_x_;
  y = scroll_offset_y_;
}

bool NWebRenderHandler::HasOverscroll() {
  return has_over_scroll_;
}
#endif

#if BUILDFLAG(IS_OHOS)
void NWebRenderHandler::OnEditableChanged(CefRefPtr<CefBrowser> browser,
                                          bool is_editable_node) {
  if (!inputmethod_client_) {
    LOG(ERROR) << "inputmethod_client_ is nullptr.";
    return;
  }
  inputmethod_client_->OnEditableChanged(browser, is_editable_node);
}
#endif  // #if BUILDFLAG(IS_OHOS)

#if BUILDFLAG(ARKWEB_SCREEN_OFFSET)
void NWebRenderHandler::SetScreenOffset(double x, double y)
{
  screen_x_ = x;
  screen_y_ = y;
}
void NWebRenderHandler::GetScreenOffset(CefRefPtr<CefBrowser> browser, double& x, double& y)
{
  x = screen_x_;
  y = screen_y_;
}
#endif  // BUILDFLAG(ARKWEB_SCREEN_OFFSET)

void NWebRenderHandler::Resize(uint32_t width, uint32_t height) {
  width_ = width;
  height_ = height;
}

#if BUILDFLAG(IS_OHOS)
gfx::Size NWebRenderHandler::GetSize() {
  return gfx::Size(width_, height_);
}

void NWebRenderHandler::OnPopupSize(CefRefPtr<CefBrowser> browser,
                                    const CefRect& rect) {
  if (auto handler = handler_.lock()) {
    float ratio = GetCefDeviceRatio();
    handler->OnPopupSize(rect.x * ratio, rect.y * ratio, rect.width * ratio,
                         rect.height * ratio);
  }
}

void NWebRenderHandler::OnPopupShow(CefRefPtr<CefBrowser> browser, bool show) {
  if (auto handler = handler_.lock()) {
    handler->OnPopupShow(show);
  }
}

#endif

#if BUILDFLAG(ARKWEB_DSS)
void NWebRenderHandler::GetDevicePixelSize(CefRefPtr<CefBrowser> browser,
                                           CefSize& size) {
  size.width = width_;
  size.height = height_;
}
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void NWebRenderHandler::ResizeVisibleViewport(uint32_t width, uint32_t height) {
  visible_width_ = width;
  visible_height_ = height;
}

void NWebRenderHandler::StartVibraFeedback(const std::string& vibratorType) {
  if (auto handler = handler_.lock()) {
    handler->StartVibraFeedback(vibratorType);
  }
}
#endif

void NWebRenderHandler::GetViewRect(CefRefPtr<CefBrowser> browser,
                                    CefRect& rect) {
  rect.x = 0;
  rect.y = 0;
  if (screen_info_.display_ratio <= 0) {
    rect.width = width_;
    rect.height = height_;
  } else {
    // Surface greater than Web compoment in case show black line.
    rect.width = std::ceil(width_ / screen_info_.display_ratio);
    rect.height = std::ceil(height_ / screen_info_.display_ratio);
  }

  if (rect.width <= 0) {
    rect.width = 1;
  }
  if (rect.height <= 0) {
    rect.height = 1;
  }
}

#if BUILDFLAG(ARKWEB_INPUT_EVENTS) || BUILDFLAG(ARKWEB_VIEWPORT_AVOID)
void NWebRenderHandler::SetNeedFocusViewport(bool need) {
  LOG(INFO) << "NWebRenderHandler::SetNeedFocusViewport needFocusViewport:"
            << need;
  needFocusViewport_ = need;
}

void NWebRenderHandler::OnResizeScrollableViewport(
    CefRefPtr<CefBrowser> browser) {
  LOG(INFO)
      << "NWebRenderHandler::OnResizeScrollableViewport needFocusViewport:"
      << needFocusViewport_;
  if (!browser || !browser->GetHost()) {
    return;
  }
#if BUILDFLAG(ARKWEB_VIEWPORT_AVOID)
  if (viewportAvoidScrollOffset_ != 0) {
    LOG(INFO) << "AvoidVisibleViewportBottom set: " << viewportAvoidHeight_
              << " viewportAvoidScrollOffset_" << viewportAvoidScrollOffset_;
    browser->GetHost()->ScrollBy(0, viewportAvoidScrollOffset_);
  } else if (inputmethod_client_ && inputmethod_client_->IsAttached()) {
#else
  if (inputmethod_client_ && inputmethod_client_->IsAttached()) {
#endif
    LOG(INFO) << "system keyboard is attached, scroll focused node into view";
    browser->GetHost()->ScrollFocusedEditableNodeIntoView();
  } else if (custom_keyboard_handler_ &&
             custom_keyboard_handler_->IsAttached()) {
    LOG(INFO) << "custom keyboard is attached, scroll focused node into view";
    browser->GetHost()->ScrollFocusedEditableNodeIntoView();
  }
}

void NWebRenderHandler::GetVisibleViewportRect(CefRefPtr<CefBrowser> browser,
                                               CefRect& rect) {
  if (visible_width_ == 0 && visible_height_ == 0) {
    GetViewRect(browser, rect);
    return;
  }
  rect.x = 0;
  rect.y = 0;
  if (screen_info_.display_ratio <= 0) {
    rect.width = visible_width_;
    rect.height = visible_height_;
  } else {
    // Surface greater than Web compoment in case show black line.
    rect.width = std::ceil(visible_width_ / screen_info_.display_ratio);
    rect.height = std::ceil(visible_height_ / screen_info_.display_ratio);
  }

  if (rect.width <= 0) {
    rect.width = 1;
  }
  if (rect.height <= 0) {
    rect.height = 1;
  }
}

void NWebRenderHandler::UpdateSecurityLayer(bool isNeedSecurityLayer) {
  auto handler = handler_.lock();
  if (handler) {
    handler->EnableSecurityLayer(isNeedSecurityLayer);
  }
}

void NWebRenderHandler::UpdateTextFieldStatus(bool isShowKeyboard, bool isAttachIME) {
  auto handler = handler_.lock();
  if (handler) {
    handler->UpdateTextFieldStatus(isShowKeyboard, isAttachIME);
  }
}
#endif

#if BUILDFLAG(ARKWEB_VIEWPORT_AVOID)
void NWebRenderHandler::SetViewportAvoidHeight(int32_t viewportAvoidHeight) {
  if (viewportAvoidHeight > viewportAvoidHeight_) {
    viewportAvoidScrollOffset_ = viewportAvoidHeight - viewportAvoidHeight_;
  } else {
    viewportAvoidScrollOffset_ = 0;
  }
  viewportAvoidHeight_ = viewportAvoidHeight;
}
#endif

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
void NWebRenderHandler::SetFillContent(const CefString& content) {
  if (inputmethod_client_) {
    inputmethod_client_->SetFillContent(content, node_id_);
  }
}
#endif

// #if BUILDFLAG(ARKWEB_SCREEN_ROTATION)
void NWebRenderHandler::SetScreenInfo(const NWebScreenInfo& screen_info) {
  screen_info_ = screen_info;
  auto delegete = delegate_interface_.lock();
  if (delegete) {
    delegete->SetVirtualPixelRatio(screen_info_.display_ratio);
  }
}
// #endif  // #if BUILDFLAG(ARKWEB_SCREEN_ROTATION)

bool NWebRenderHandler::GetScreenInfo(CefRefPtr<CefBrowser> browser,
                                      CefScreenInfo& screen_info) {
  screen_info.orientation = ConvertOrientationType(screen_info_.orientation);
  bool is_portrait = screen_info_.orientation == OHOS::NWeb::DisplayOrientation::PORTRAIT
                     || screen_info_.orientation == OHOS::NWeb::DisplayOrientation::PORTRAIT_INVERTED;
  bool need_swap_portrait_and_landscape = (is_portrait && screen_info_.width > screen_info_.height)
                                          || (!is_portrait && screen_info_.width <= screen_info_.height);
  if (need_swap_portrait_and_landscape) {
    screen_info.orientation = SwapPortraitAndLandscape(screen_info.orientation);
  }
  if (screen_info_.rotation == OHOS::NWeb::RotationType::ROTATION_90
        || screen_info_.rotation == OHOS::NWeb::RotationType::ROTATION_270) {
    screen_info.orientation = SwapPrimaryAndSecondary(screen_info.orientation);
  }
  screen_info.angle = ConvertRotationAngel(screen_info_.rotation);
  screen_info.rect.width = screen_info_.width;
  screen_info.rect.height = screen_info_.height;
  screen_info.device_scale_factor = screen_info_.display_ratio;

  // TODO: currently display dont have interface to get. We use fix value
  // instead.
  screen_info.depth = 24;
  screen_info.depth_per_component = 8;

#if BUILDFLAG(ARKWEB_SCREEN_SIZE)
  if (base::ohos::ApplicationApiVersion() >= APPLICATION_API_20) {
    screen_info.available_rect.width = screen_info_.width;
    screen_info.available_rect.height = screen_info_.height;
  }
#endif  // #if BUILDFLAG(ARKWEB_SCREEN_SIZE)

  cef_device_ratio_ = screen_info.device_scale_factor;
  return true;
}

void NWebRenderHandler::OnPaint(CefRefPtr<CefBrowser> browser,
                                PaintElementType type,
                                const RectList& dirty_rects,
                                const void* buffer,
                                int width,
                                int height) {
  // run render callback to take buffer.
  // note: buffer must be consumed asap with no lock protected
  if (render_update_cb_ == nullptr) {
    return;
  }
  if ((uint32_t)width != width_ || (uint32_t)height != height_) {
    LOG(INFO) << "frame size(" << width << "*" << height
              << ") is not identical to request size (" << width_ << "*"
              << height_ << "), drop this frame";
    constexpr uint8_t kBitsPerPixel = 4;
    uint32_t white_frame_size;
    if (!(base::CheckMul(width_, height_) * kBitsPerPixel).AssignIfValid(&white_frame_size)) {
      LOG(ERROR) << "white_frame_size overflow";
      return;
    }
    char* white_frame = new char[white_frame_size];
    const char pixel_in_white = 0xFF;
    (void)memset_s(white_frame, white_frame_size, pixel_in_white,
                   white_frame_size);
    render_update_cb_(white_frame);
    delete[] white_frame;
  } else {
    render_update_cb_(reinterpret_cast<const char*>(buffer));
  }
}

void NWebRenderHandler::OnRootLayerChanged(CefRefPtr<CefBrowser> browser,
                                           int height,
                                           int width) {
  content_height_ = height;
  content_width_ = width;
  if (auto handler = handler_.lock()) {
    handler->OnRootLayerChanged(width, height);
  }
}

void NWebRenderHandler::ReleaseResizeHold(CefRefPtr<CefBrowser> browser) {
  if (auto handler = handler_.lock()) {
    handler->ReleaseResizeHold();
  }
}

void NWebRenderHandler::OnScrollOffsetChanged(CefRefPtr<CefBrowser> browser,
                                              double x,
                                              double y) {
  if (auto handler = handler_.lock()) {
    handler->OnScroll(x, y);
  }

#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
  scroll_offset_x_ = static_cast<float>(x);
  scroll_offset_y_ = static_cast<float>(y);
#endif
  if (on_scroll_cb_) {
    on_scroll_cb_(x, y);
  }

  ResSchedClientAdapter::ReportScene(ResSchedStatusAdapter::WEB_SCENE_ENTER,
                                     ResSchedSceneAdapter::SLIDE);
}

int NWebRenderHandler::ContentHeight() {
  return content_height_;
}

void NWebRenderHandler::OnImeCompositionRangeChanged(
    CefRefPtr<CefBrowser> browser,
    const CefRange& selected_range,
    const RectList& character_bounds) {
  if (inputmethod_client_) {
    inputmethod_client_->OnImeCompositionRangeChanged(browser, selected_range);
  }
}

void NWebRenderHandler::OnTextSelectionChanged(CefRefPtr<CefBrowser> browser,
                                               const CefString& selected_text,
                                               const CefRange& selected_range) {
  if (inputmethod_client_) {
    inputmethod_client_->OnTextSelectionChanged(browser, selected_text,
                                                selected_range);
  }
  if (auto handler = handler_.lock()) {
    handler->OnTextSelectionChange(selected_text);
  }
}

void NWebRenderHandler::OnVirtualKeyboardRequestedEx(
    CefRefPtr<CefBrowser> browser,
    TextInputInfo text_input_info,
    bool is_need_reset_listener,
    const AttributesMap& attributes) {
  LOG(INFO) << "NWebRenderHandler::OnVirtualKeyboardRequested"
            << ", node_id = " << text_input_info.node_id
            << ", input_mode = " << text_input_info.input_mode
            << ", input_type = " << text_input_info.input_type
            << ", input_action = " << text_input_info.input_action
            << ", input_flags = " << text_input_info.input_flags
            << ", show_keyboard = " << text_input_info.show_keyboard
            << ", always_hide_ime = " << text_input_info.always_hide_ime;

  std::map<std::string, std::string> attributesMap;
  for (const auto& item : attributes) {
    LOG(DEBUG)
        << "WebCustomKeyboard OnVirtualKeyboardRequested attributes, key = "
        << item.first.ToString() << ", value = " << item.second.ToString();
    attributesMap.insert({item.first.ToString(), item.second.ToString()});
  }

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  if (!inputmethod_client_) {
    LOG(ERROR) << "inputmethod_client_ is nullptr.";
    return;
  }
  if (text_input_info.node_id > 0) {
    node_id_ = text_input_info.node_id;
  }
  // When click Web input field.
  // If input_mode is CEF_TEXT_INPUT_MODE_NONE,
  // it means that the web does not require a keyboard.
  if (text_input_info.input_mode == CEF_TEXT_INPUT_MODE_NONE) {
    noNeedKeyboardByInput_ = true;
  } else {
    noNeedKeyboardByInput_ = false;
  }

  bool is_hide = (text_input_info.node_id == 0) || text_input_info.always_hide_ime;
  if (!is_hide) {
    auto delegate = delegate_interface_.lock();
    if (delegate) {
      if (virtualKeyboardRequestFocus_) delegate->OnFocus();
      // If the inputmode is none and keyboard exists, the keyboard should be hide.
      if (text_input_info.input_mode == CEF_TEXT_INPUT_MODE_NONE &&
          inputmethod_client_->IsKeyboardShow()) {
        HandleKeyboardDetach();
      }
      HandleKeyboardAttach(browser, text_input_info, is_need_reset_listener,
                           attributesMap);
    }
  } else {
    HandleKeyboardDetach();
  }
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
}

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void NWebRenderHandler::HandleKeyboardAttach(
    CefRefPtr<CefBrowser> browser,
    const TextInputInfo& text_input_info,
    bool is_need_reset_listener,
    const std::map<std::string, std::string>& attributesMap) {
  bool useSystemKeyboard = true;
  int32_t enterKeyType = -1;
  auto handler = handler_.lock();
  if (handler && !custom_keyboard_handler_) {
    custom_keyboard_handler_ =
        std::make_shared<NWebCustomKeyboardHandlerImpl>(handler);
  }
  UpdateSecurityLayer(text_input_info.input_type == CEF_TEXT_INPUT_TYPE_PASSWORD);
  if (handler && text_input_info.show_keyboard) {
    handler->OnInterceptKeyboardAttach(custom_keyboard_handler_, attributesMap,
                                       useSystemKeyboard, enterKeyType);
    LOG(INFO) << "WebCustomKeyboard OnInterceptKeyboardAttach return, "
              << "useSystemKeyboard = " << useSystemKeyboard
              << ", enterKeyType = " << enterKeyType;
  }

  if (useSystemKeyboard) {
    if (!isSystemKeyboard_ && custom_keyboard_handler_) {
      LOG(INFO) << "WebCustomKeyboard before use system keyboard, need to close custom keyboard";
      custom_keyboard_handler_->Close();
    }
    int32_t requestKeyboardReason = 0;
    auto iter = attributesMap.find("requestKeyboardReason");
    if (iter != attributesMap.end()) {
      requestKeyboardReason = std::stoi(iter->second);
    }
    LOG(INFO) << "WebCustomKeyboard attach system keyboard requestKeyboardReason = " << requestKeyboardReason;
    inputmethod_client_->Attach(browser, text_input_info,
                                is_need_reset_listener, enterKeyType, requestKeyboardReason);
  } else {
    if (isSystemKeyboard_) {
      LOG(INFO) << "WebCustomKeyboard before use custom keyboard, need to "
                   "close system keyboard";
      inputmethod_client_->HideTextInput(0, NWebInputMethodClient::HideTextinputType::FROM_KERNEL,
                                         noNeedKeyboardByInput_);
    }
    // need to colse system forcely from native input change to custom keyboard
    // input
    inputmethod_client_->HideTextInputForce();
    if (custom_keyboard_handler_) {
      LOG(INFO) << "WebCustomKeyboard attach custom keyboard";
      custom_keyboard_handler_->Attach(
          browser, text_input_info.show_keyboard,
          static_cast<int32_t>(text_input_info.input_flags));
    }
  }

  isSystemKeyboard_ = useSystemKeyboard;
}

void NWebRenderHandler::HandleKeyboardDetach() {
  if (isSystemKeyboard_) {
    LOG(INFO) << "WebCustomKeyboard close system keyboard";
    inputmethod_client_->HideTextInput();
  } else {
    if (custom_keyboard_handler_) {
      LOG(INFO) << "WebCustomKeyboard close custom keyboard";
      custom_keyboard_handler_->Close();
    }
  }
  UpdateSecurityLayer(false);
}

void NWebRenderHandler::EnableVirtualKeyboardRequestFocus(bool isNeedRequestFocus) {
	virtualKeyboardRequestFocus_ = isNeedRequestFocus;
}
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

void NWebRenderHandler::GetTouchHandleSize(
    CefRefPtr<CefBrowser> browser,
    cef_horizontal_alignment_t orientation,
    CefSize& size) {
  // TODO: need to refactor in 3.2.8.1 use arkui refactor.
  size.width = 10;
  size.height = 10;
  if (screen_info_.display_ratio <= 0.0) {
    LOG(INFO) << "invalid display_ratio_, display_ratio_ = "
              << screen_info_.display_ratio;
    return;
  }
  if (auto handler = handler_.lock()) {
    std::shared_ptr<NWebTouchHandleHotZoneImpl> hot_zone =
        std::make_shared<NWebTouchHandleHotZoneImpl>();
    handler->OnGetTouchHandleHotZone(hot_zone);
    if (hot_zone->GetWidth() > 0 && hot_zone->GetHeight() > 0) {
      size.width = static_cast<int>(hot_zone->GetWidth()) + 1;
      size.height = static_cast<int>(hot_zone->GetHeight()) + 1;
#if BUILDFLAG(ARKWEB_MENU)
    } else if (hot_zone->GetWidth() > 0 && hot_zone->GetHeight() == 0) {
      size.width = static_cast<int>(hot_zone->GetWidth()) + 1;
      size.height = 0;
#endif
    }
  }
  LOG(INFO) << "GetTouchHandleSize " << size.width << " " << size.height;
}

void NWebRenderHandler::OpenEyeDropper(CefRefPtr<CefBrowser> browser) {
  if (!browser || !browser->GetHost()) {
    return;
  }
  std::pair<double, double> position;
  auto delegate = delegate_interface_.lock();
  if (delegate) {
    position = delegate->GetLastTouchMousePosition();
  }
  auto view_port_height = browser->GetHost()->GetShrinkViewportHeight();
  view_port_height +=
      view_port_height > 0 ? browser->GetHost()->GetTopControlsOffset() : 0;

  OhosAdapterHelper::GetInstance()
      .GetColorPickerAdapter()
      .StartColorPickerWithColorValue(
          position.first + screen_x_ * screen_info_.display_ratio,
          position.second +
              (view_port_height + screen_y_) * screen_info_.display_ratio,
          [browser](bool success, uint32_t color) {
            if (browser && browser->GetHost()) {
              LOG(INFO) << "OnEyeDropperResult, success == " << success
                        << ", color == " << color;
              browser->GetHost()->OnEyeDropperResult(success, color);
            }
          });
}

std::shared_ptr<NWebTouchHandleState> NWebRenderHandler::GetTouchHandleState(
    NWebTouchHandleState::TouchHandleType type) {
  switch (type) {
    case NWebTouchHandleState::TouchHandleType::INSERT_HANDLE:
      return insert_handle_.enabled
                 ? std::make_shared<NWebTouchHandleStateImpl>(insert_handle_)
                 : nullptr;
    case NWebTouchHandleState::TouchHandleType::SELECTION_BEGIN_HANDLE:
      return start_selection_handle_.enabled
                 ? std::make_shared<NWebTouchHandleStateImpl>(
                       start_selection_handle_)
                 : nullptr;
    case NWebTouchHandleState::TouchHandleType::SELECTION_END_HANDLE:
      return end_selection_handle_.enabled
                 ? std::make_shared<NWebTouchHandleStateImpl>(
                       end_selection_handle_)
                 : nullptr;
    default:
      return nullptr;
  }
}
#if BUILDFLAG(ARKWEB_MENU)
std::shared_ptr<NWebTouchHandleState>
NWebRenderHandler::GetDefalutTouchHandleState(
    NWebTouchHandleState::TouchHandleType type) {
  CefTouchHandleState state;
  state.touch_handle_id = -1;
  state.origin.x = 0;
  state.origin.y = 0;
  state.view_port.x = 0;
  state.view_port.y = 0;
  state.edge_height = 0;
  state.enabled = true;
  state.alpha = 0;
  if (type == NWebTouchHandleState::TouchHandleType::SELECTION_BEGIN_HANDLE) {
    state.orientation = CEF_HORIZONTAL_ALIGNMENT_LEFT;
  } else if (type ==
             NWebTouchHandleState::TouchHandleType::SELECTION_END_HANDLE) {
    state.orientation = CEF_HORIZONTAL_ALIGNMENT_RIGHT;
  } else {
    state.orientation = CEF_HORIZONTAL_ALIGNMENT_CENTER;
  }

  return std::make_shared<NWebTouchHandleStateImpl>(state);
}

CefRect NWebRenderHandler::ConvertSelectAreaDisplayRatio(const CefRect& rect)
{
  CefRect result_select_area = rect;
  if (screen_info_.display_ratio <= 0) {
    LOG(WARNING) << "virtual display ratio is invalid";
    return result_select_area;
  }
  result_select_area.x *= screen_info_.display_ratio;
  result_select_area.y *= screen_info_.display_ratio;
  result_select_area.width *= screen_info_.display_ratio;
  result_select_area.height *= screen_info_.display_ratio;
  return result_select_area;
}

void NWebRenderHandler::OnSelectAreaChanged(CefRect& select_area) {
  select_area = ConvertSelectAreaDisplayRatio(select_area);
}

void NWebRenderHandler::OnClippedSelectionBoundsChanged(const CefRect& rect, bool need_report) {
    clipped_selection_bounds_ = rect;
    if (!need_report) {
        return;
    }
    if (auto handler = handler_.lock()) {
        handler->OnClippedSelectionBoundsChanged(rect.x, rect.y, rect.width, rect.height);
    }
}
#endif
CefTouchHandleState NWebRenderHandler::ConvertTouchHandleDisplayRatio(
    const CefTouchHandleState& touch_handle) {
  CefTouchHandleState result_touch_handle = touch_handle;
  if (screen_info_.display_ratio <= 0) {
    LOG(WARNING) << "virtual display ratio is invalid";
    return result_touch_handle;
  }
  result_touch_handle.edge_height *= screen_info_.display_ratio;
  result_touch_handle.origin.x *= screen_info_.display_ratio;
  result_touch_handle.origin.y *= screen_info_.display_ratio;
  result_touch_handle.view_port.x *= screen_info_.display_ratio;
  result_touch_handle.view_port.y *= screen_info_.display_ratio;
  return result_touch_handle;
}

void NWebRenderHandler::OnTouchSelectionChanged(
    const CefTouchHandleState& insert_handle,
    const CefTouchHandleState& start_selection_handle,
    const CefTouchHandleState& end_selection_handle,
    bool need_report) {
  insert_handle_ = ConvertTouchHandleDisplayRatio(insert_handle);
  start_selection_handle_ =
      ConvertTouchHandleDisplayRatio(start_selection_handle);
  end_selection_handle_ = ConvertTouchHandleDisplayRatio(end_selection_handle);
  if (!need_report) {
    return;
  }
  if (auto handler = handler_.lock()) {
    handler->OnTouchSelectionChanged(
        std::make_shared<NWebTouchHandleStateImpl>(insert_handle_),
        std::make_shared<NWebTouchHandleStateImpl>(start_selection_handle_),
        std::make_shared<NWebTouchHandleStateImpl>(end_selection_handle_));
  }
}

#if BUILDFLAG(ARKWEB_DRAG_DROP)
void NWebRenderHandler::NotifySelectAllClicked(bool select_all) {
  select_all_ = select_all;
}

void NWebRenderHandler::ImageDragForFileUri(CefRefPtr<CefDragData> drag_data) {
  // default temp dir in sandbox
  CefString tempPath("/data/storage/el2/base/haps/entry/temp/dragdrop/");
  auto delegete = delegate_interface_.lock();
  if (delegete && !delegete->GetAppTempDir().empty()) {
    tempPath = delegete->GetAppTempDir() + "/dragdrop/";
  }

  if (base::DirectoryExists(base::FilePath(tempPath))) {
    if (!base::IsDirectoryEmpty(base::FilePath(tempPath))) {
      base::DeletePathRecursively(base::FilePath(tempPath));
    }
  } else {
    LOG(INFO) << "DragDrop temp dir not exist, create it";
  }
  base::CreateDirectory(base::FilePath(tempPath));

  CefString fileName = drag_data->GetFileName();
  if (!fileName.ToString().empty()) {
    CefString fullName(tempPath.ToString() + fileName.ToString());
    if (base::PathExists(base::FilePath(fullName))) {
      LOG(INFO) << "DragDrop image file already exist, delete it first";
      base::DeleteFile(base::FilePath(fullName));
    }

    const int image_file_size_max = 10 * 1024 * 1024;  // 10M
    if (drag_data->GetImageFileSize() > image_file_size_max) {
      LOG(WARNING) << "DragDrop The image size exceeds 10MB";
      return;
    }

    CefRefPtr<CefStreamWriter> stream(CefStreamWriter::CreateForFile(fullName));
    size_t size = drag_data->GetFileContents(stream);
    if (size == drag_data->GetImageFileSize()) {
      LOG(INFO) << "DragDrop image file write success, size:" << size;
    } else {
      LOG(ERROR) << "DragDrop image file write failed";
    }
  } else {
    LOG(INFO) << "DragDrop this is not a image drag, pass";
  }
}

void NWebRenderHandler::GetVisibleRectToWeb(int& visibleX,
                                            int& visibleY,
                                            int& visibleWidth,
                                            int& visibleHeight) {
  auto handler = handler_.lock();
  if (handler == nullptr) {
    LOG(ERROR) << "can't get strong ptr with handler";
    return;
  }
  handler->GetVisibleRectToWeb(visibleX, visibleY, visibleWidth, visibleHeight);
}

// chromium内核上报的拖拽数据
bool NWebRenderHandler::StartDragging(CefRefPtr<CefBrowser> browser,
                                      CefRefPtr<CefDragData> drag_data,
                                      DragOperationsMask allowed_ops,
                                      int x,
                                      int y) {
  LOG(INFO) << "DragDrop StartDragging received dragData from chromium start "
               "dragging callback, operation = "
            << allowed_ops << ", x = " << x << ", y = " << y;
  if (!drag_data || !drag_data->HasImage()) {
    LOG(ERROR) << "drag data invalid";
    return false;
  }

  auto fragment = drag_data->GetFragmentText();
  LOG(INFO) << "DragDrop drag data GetFragmentText:" << fragment.length();
  auto link_url = drag_data->GetLinkURL();
  LOG(INFO) << "DragDrop drag data GetLinkURL:" << link_url.length();
  auto link_html = drag_data->GetFragmentHtml();
  LOG(INFO) << "DragDrop drag data GetFragmentHtml:" << link_html.length();

  ImageDragForFileUri(drag_data);
  CefPoint drag_touch_point(x, y);
  if (browser && browser->GetHost()) {
    int shrink_viewport_height = browser->GetHost()->GetShrinkViewportHeight();
    drag_touch_point.Set(x, y + shrink_viewport_height);
    LOG(INFO) << "DragDrop add offset to drag point y : " << shrink_viewport_height;
  }

  std::vector<CefPoint> start_edge{
      CefPoint(start_selection_handle_.origin.x,
               start_selection_handle_.origin.y -
                   start_selection_handle_.edge_height),
      CefPoint(start_selection_handle_.origin.x,
               start_selection_handle_.origin.y)};
  std::vector<CefPoint> end_edge{
      CefPoint(
          end_selection_handle_.origin.x,
          end_selection_handle_.origin.y - end_selection_handle_.edge_height),
      CefPoint(end_selection_handle_.origin.x, end_selection_handle_.origin.y)};

  bool usefull_selection = false;
  if (!link_url.empty() && !drag_data->IsImageFileContents()) {
    usefull_selection = false;
  } else if (select_all_) {
    usefull_selection = false;
  } else {
    usefull_selection = is_irregular_drag_background_;
  }

  // default value false
  bool dark_mode_enable = false;
  auto delegete = delegate_interface_.lock();
  if (delegete) {
    dark_mode_enable = delegete->DarkModeEnabled();
  }

  LOG(DEBUG) << "DragDrop StartDragging darkModeEnable:" << dark_mode_enable;

  bool is_drag_new_style = true;
  if (base::ohos::IsPcDevice()) {
    is_drag_new_style = false;
  }
  nweb_drag_data_ = std::make_shared<NWebDragDataImpl>(
      drag_data, drag_touch_point, start_edge, end_edge,
      screen_info_.display_ratio, usefull_selection, dark_mode_enable,
      is_drag_new_style);
  nweb_drag_data_->SetAllowedDragOperation(static_cast<NWebDragData::DragOperationsMask>(allowed_ops));
  auto handler = handler_.lock();
  if (handler == nullptr) {
    LOG(ERROR) << "can't get strong ptr with handler";
    return false;
  }
#if BUILDFLAG(ARKWEB_REPORT_SYS_EVENT)
  if (browser) {
    ReportDragDropStatus("DRAG_START", browser->AsArkWebBrowser()->GetNWebId());
  }
#endif
  return handler->OnDragAndDropDataUdmf(nweb_drag_data_);
}

void NWebRenderHandler::UpdateDragCursor(CefRefPtr<CefBrowser> browser,
                                         DragOperation operation) {
  auto handler = handler_.lock();
  if (handler == nullptr) {
    LOG(ERROR) << "DragDrop can't get strong ptr with handler";
    return;
  }
  handler->UpdateDragCursor(
      static_cast<NWebDragData::DragOperation>(operation));
}

#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
void NWebRenderHandler::OnCompleteSwapWithNewSize() {
  if (auto handler = handler_.lock()) {
    handler->OnCompleteSwapWithNewSize();
  }
}

void NWebRenderHandler::OnResizeNotWork() {
  if (auto handler = handler_.lock()) {
    handler->OnResizeNotWork();
  }
}
#endif  // BUILDFLAG(ARKWEB_COMPOSITE_RENDER)

CefRefPtr<CefDragData> NWebRenderHandler::GetDragData() {
  if (!nweb_drag_data_) {
    content::DropData drop_data;
    CefRefPtr<CefDragDataImpl> drag_data(new CefDragDataImpl(drop_data));
    nweb_drag_data_ = std::make_shared<NWebDragDataImpl>(drag_data.get());
  }
  return std::static_pointer_cast<NWebDragDataImpl>(nweb_drag_data_)
      ->GetDragData();
}

void NWebRenderHandler::FreePixlMapData() {
  if (nweb_drag_data_) {
    std::static_pointer_cast<NWebDragDataImpl>(nweb_drag_data_)
        ->FreePixlMapData();
  }
}

void NWebRenderHandler::SetIrregularDragBackground(
    bool is_irregular_background) {
  is_irregular_drag_background_ = is_irregular_background;
}

#endif  // BUILDFLAG(ARKWEB_DRAG_DROP)

#if BUILDFLAG(IS_OHOS)
void NWebRenderHandler::OnOverScrollFlingVelocity(CefRefPtr<CefBrowser> browser,
                                                  const float x,
                                                  const float y,
                                                  bool is_fling) {
  if (auto handler = handler_.lock()) {
    // Value multiplied by virtual pixel ratio.
    is_fling ? handler->OnOverScrollFlingVelocity(
                   x * screen_info_.display_ratio,
                   y * screen_info_.display_ratio, is_fling)
             : handler->OnOverScrollFlingVelocity(x, y, is_fling);
  }
}

void NWebRenderHandler::OnOverScrollFlingEnd(CefRefPtr<CefBrowser> browser) {
  if (auto handler = handler_.lock()) {
    handler->OnOverScrollFlingEnd();
  }
}

void NWebRenderHandler::OnScrollState(CefRefPtr<CefBrowser> browser,
                                      bool scroll_state) {
  if (auto handler = handler_.lock()) {
    handler->OnScrollState(scroll_state);
  }
}

void NWebRenderHandler::OnScrollStart(CefRefPtr<CefBrowser> browser,
                                      const float x,
                                      const float y) {
  if (auto handler = handler_.lock()) {
    handler->OnScrollStart(x, y);
  }
}

bool NWebRenderHandler::FilterScrollEvent(CefRefPtr<CefBrowser> browser,
                                          const float x,
                                          const float y,
                                          const float fling_x,
                                          const float fling_y) {
  if (auto handler = handler_.lock()) {
    // Value multiplied by virtual pixel ratio.
    return handler->FilterScrollEvent(x * screen_info_.display_ratio,
                                      y * screen_info_.display_ratio,
                                      fling_x * screen_info_.display_ratio,
                                      fling_y * screen_info_.display_ratio);
  }
  return false;
}

bool NWebRenderHandler::OnNestedScroll(CefRefPtr<CefBrowser> browser,
                                       float& x,
                                       float& y,
                                       float& fling_x,
                                       float& fling_y,
                                       bool& isAvailable)
{
  if (auto handler = handler_.lock()) {
    // Value multiplied by virtual pixel ratio.
    x = x * screen_info_.display_ratio;
    y = y * screen_info_.display_ratio;
    fling_x = fling_x * screen_info_.display_ratio;
    fling_y = fling_y * screen_info_.display_ratio;
    bool consumed = handler->OnNestedScroll(x, y, fling_x, fling_y, isAvailable);
    x = x / screen_info_.display_ratio;
    y = y / screen_info_.display_ratio;
    fling_x = fling_x / screen_info_.display_ratio;
    fling_y = fling_y / screen_info_.display_ratio;
    return consumed;
  }
  return false;
}

void NWebRenderHandler::OnNativeEmbedGestureEvent(
    CefRefPtr<CefBrowser> browser,
    const CefEmbedTouchEvent& touchEvent,
    CefRefPtr<CefGestureEventCallback> callback) {
  if (auto handler = handler_.lock()) {
    std::shared_ptr<NWebNativeEmbedTouchEventImpl> info =
        std::make_shared<NWebNativeEmbedTouchEventImpl>();
    info->SetX(touchEvent.x);
    info->SetY(touchEvent.y);
    info->SetId(touchEvent.id);
    info->SetEmbedId(touchEvent.embedId);
    info->SetOffsetX(touchEvent.offsetX);
    info->SetOffsetY(touchEvent.offsetY);
    info->SetScreenX(touchEvent.screenX);
    info->SetScreenY(touchEvent.screenY);
    info->SetType(static_cast<OHOS::NWeb::TouchType>(touchEvent.type));
    std::shared_ptr<NWebGestureEventResult> result =
        std::make_shared<NWebGestureEventResultImpl>(callback);

    info->SetResult(result);
    handler->OnNativeEmbedGestureEvent(info);
  }
}

void NWebRenderHandler::OnNativeEmbedMouseEvent(
    CefRefPtr<CefBrowser> browser,
    const CefEmbedMouseEvent& mouseEvent,
    CefRefPtr<CefMouseEventCallback> callback) {
  if (auto handler = handler_.lock()) {
    std::shared_ptr<NWebNativeEmbedMouseEventImpl> info =
        std::make_shared<NWebNativeEmbedMouseEventImpl>();
    info->SetX(mouseEvent.x);
    info->SetY(mouseEvent.y);
    info->SetEmbedId(mouseEvent.embedId);
    info->SetIsHitNativeArea(mouseEvent.isHitNativeArea);
    info->SetOffsetX(mouseEvent.offsetX);
    info->SetOffsetY(mouseEvent.offsetY);
    info->SetScreenX(mouseEvent.screenX);
    info->SetScreenY(mouseEvent.screenY);
    info->SetType(static_cast<OHOS::NWeb::MouseType>(mouseEvent.type));
    info->SetButton(static_cast<OHOS::NWeb::MouseButton>(mouseEvent.button));
    std::shared_ptr<NWebMouseEventResult> result =
        std::make_shared<NWebMouseEventResultImpl>(callback);

    info->SetResult(result);
    handler->OnNativeEmbedMouseEvent(info);
  }
}

std::shared_ptr<NWebNativeEmbedDataInfo> NWebRenderHandler::CefEmbedDataToWeb(
    const ArkWebRenderHandlerExt::CefNativeEmbedData& embedData) {
  auto info = embedData.info;
  std::string url;
  if (auto delegate = delegate_interface_.lock()) {
    url = delegate->GetUrl();
  } else {
    url = info.url;
  }
  LOG(DEBUG) << "GetUrl CefEmbedDataToWeb url is "
             << url::LogUtils::ConvertUrlWithMask(url);
  std::shared_ptr<NWebNativeEmbedInfoImpl> embedinfo =
      std::make_shared<NWebNativeEmbedInfoImpl>(
          info.width, info.height, info.id, info.src, url, info.type, info.tag,
          info.params, info.x, info.y);

  std::shared_ptr<NWebNativeEmbedDataInfoImpl> datainfo =
      std::make_shared<NWebNativeEmbedDataInfoImpl>();
  datainfo->SetNativeEmbedInfo(embedinfo);
  datainfo->SetEmbedId(embedData.embedId);
  datainfo->SetSurfaceId(embedData.surfaceId);
  datainfo->SetStatus(
      static_cast<OHOS::NWeb::NativeEmbedStatus>(embedData.status));
  return datainfo;
}

void NWebRenderHandler::OnNativeEmbedLifecycleChange(
    CefRefPtr<CefBrowser> browser,
    const CefNativeEmbedData& info) {
  auto nativeEmbedDataInfo = CefEmbedDataToWeb(info);
  if (auto delegate = delegate_interface_.lock()) {
    delegate->UpdateNativeEmbedInfo(nativeEmbedDataInfo);
  }
  if (auto handler = handler_.lock()) {
    handler->OnNativeEmbedLifecycleChange(nativeEmbedDataInfo);
  }
}

void NWebRenderHandler::OnNativeEmbedVisibilityChange(const CefString& embed_id,
                                                      bool visibility) {
  if (auto handler = handler_.lock()) {
    handler->OnNativeEmbedVisibilityChange(embed_id, visibility);
  }
}

std::shared_ptr<NWebNativeEmbedParamDataInfo> NWebRenderHandler::CefEmbedParamDataToWeb(
    const ArkWebRenderHandlerExt::CefNativeParamData& paramData) {
  std::shared_ptr<NWebNativeEmbedParamDataInfoImpl> paramDataInfo =
      std::make_shared<NWebNativeEmbedParamDataInfoImpl>();
  std::vector<std::shared_ptr<NWebNativeEmbedParamItem>> paramItems;
  for (const auto& param_item : paramData.paramItems) {
    std::shared_ptr<NWebNativeEmbedParamItemImpl> paramItem =
      std::make_shared<NWebNativeEmbedParamItemImpl>();
    paramItem->SetId(param_item.id);
    paramItem->SetName(param_item.name);
    paramItem->SetValue(param_item.value);
    paramItem->SetStatus(static_cast<OHOS::NWeb::NativeEmbedParamStatus>(param_item.status));
    paramItems.push_back(paramItem);
  }
  paramDataInfo->SetEmbedId(paramData.embedId);
  paramDataInfo->SetObjectAttributeId(paramData.objectAttributeId);
  paramDataInfo->SetParamItems(paramItems);
  return paramDataInfo;
}

void NWebRenderHandler::OnNativeEmbedObjectParamChange(
    CefRefPtr<CefBrowser> browser,
    const CefNativeParamData& paramData) {
  auto nativeEmbedParamDataInfo = CefEmbedParamDataToWeb(paramData);
  if (auto handler = handler_.lock()) {
    handler->OnNativeEmbedObjectParamChange(nativeEmbedParamDataInfo);
  }
}

void NWebRenderHandler::SetContentSize(int width, int height) {
  content_height_ = height;
  content_width_ = width;
}

void NWebRenderHandler::SetGestureEventResult(bool result) {
  gesture_event_result_ = result;
}

bool NWebRenderHandler::GetGestureEventResult() {
  return gesture_event_result_;
}
#endif

#if BUILDFLAG(ARKWEB_AI)
void NWebRenderHandler::GetWordSelection(CefRefPtr<CefBrowser> browser,
                                         const CefString& text,
                                         int8_t offset,
                                         CefPoint& select) {
  if (auto handler = handler_.lock()) {
    LOG(DEBUG) << "NWebRenderHandler::GetWordSelection, text: "
               << text.ToString() << ", offset: " << static_cast<int>(offset);
    std::vector<int8_t> vec =
        handler->GetWordSelection(text.ToString(), offset);
    if (vec.size() == kWordSelectionOffsetSize) {
      select.x = vec[0];
      select.y = vec[1];
    }
  }
}

void NWebRenderHandler::CreateOverlay(CefRefPtr<CefBrowser> browser,
                                      CefRefPtr<CefImage> cef_image,
                                      const CefRect& cef_image_rect,
                                      const CefPoint& cef_touch_point) {
  if (auto handler = handler_.lock()) {
    gfx::ImageSkia image_skia =
        static_cast<CefImageImpl*>(cef_image.get())->AsImageSkia();
    int width = cef_image->GetWidth();
    int height = cef_image->GetHeight();
    auto bitmap = cef_image->GetAsBitmap(1, CEF_COLOR_TYPE_BGRA_8888,
                                         CEF_ALPHA_TYPE_OPAQUE, width, height);
    if (!bitmap) {
      LOG(ERROR) << "NWebRenderHandler::CreateOverlay, bitmap invalid";
      return;
    }
    size_t data_size = bitmap->GetSize();
    void* buffer = calloc(1, data_size);
    if (!buffer) {
      LOG(ERROR) << "NWebRenderHandler::CreateOverlay, calloc failed";
      return;
    }
    size_t read_size = bitmap->GetData(buffer, data_size, 0);
    if (read_size != data_size) {
      free(buffer);
      LOG(ERROR)
          << "NWebRenderHandler::CreateOverlay, get data from bitmap failed";
      return;
    }
    LOG(INFO) << "NWebRenderHandler::CreateOverlay, data_size = " << data_size;

    if (!browser || !browser->GetHost()) {
      free(buffer);
      LOG(ERROR)
          << "NWebRenderHandler::CreateOverlay, browser or GetHost is nullptr";
      return;
    }
    float scale = browser->GetHost()->GetPageScaleFactor();
    auto view_port_height = browser->GetHost()->GetShrinkViewportHeight();
    view_port_height +=
        view_port_height > 0 ? browser->GetHost()->GetTopControlsOffset() : 0;
    handler->CreateOverlay(
        buffer, read_size, width, height, cef_image_rect.x,
        cef_image_rect.y + view_port_height * screen_info_.display_ratio,
        cef_image_rect.width, cef_image_rect.height, cef_touch_point.x * scale,
        cef_touch_point.y * scale);
    free(buffer);
  }
}

void NWebRenderHandler::OnOverlayStateChanged(CefRefPtr<CefBrowser> browser,
                                              const CefRect& cef_image_rect) {
  if (!browser || !browser->GetHost()) {
    return;
  }
  if (auto handler = handler_.lock()) {
    auto view_port_height = browser->GetHost()->GetShrinkViewportHeight();
    view_port_height +=
        view_port_height > 0 ? browser->GetHost()->GetTopControlsOffset() : 0;
    handler->OnOverlayStateChanged(
        cef_image_rect.x,
        cef_image_rect.y + view_port_height * screen_info_.display_ratio,
        cef_image_rect.width, cef_image_rect.height);
  }
}

bool NWebRenderHandler::GetDataDetectorEnable() {
  auto delegate = delegate_interface_.lock();
  if (delegate) {
    return delegate->GetDataDetectorEnable();
  }
  return false;
}
#endif

#if BUILDFLAG(ARKWEB_MAXIMIZE_RESIZE)
void NWebRenderHandler::RestoreRenderFit() {
  auto handler = handler_.lock();
  if (handler == nullptr) {
    LOG(ERROR) << "can't get strong ptr with handler";
    return;
  }
  handler->RestoreRenderFit();
}
#endif  // ARKWEB_MAXIMIZE_RESIZE

#if BUILDFLAG(ARKWEB_BLANK_SCREEN_DETECTION)
void NWebRenderHandler::OnDetectedBlankScreen(
    const std::string& url,
    int32_t blankScreenReason,
    int32_t detectedContentfulNodesCount) {
  LOG(INFO) << "NWebRenderHandler::OnDetectedBlankScreen";
  if (auto handler = handler_.lock()) {
    handler->OnDetectedBlankScreen(url, blankScreenReason,
                                   detectedContentfulNodesCount);
  }
}
#endif

#if BUILDFLAG(ARKWEB_FIRST_SCREEN_PAINT)
void NWebRenderHandler::OnFirstScreenPaint(const std::string& url,
                                           int64_t navigationStartTime,
                                           int64_t firstScreenPaintTime) {
  LOG(INFO) << "NWebRenderHandler::OnFirstScreenPaint"
      << " duration: " << (firstScreenPaintTime - navigationStartTime)
      << " navigationStartTime: " << navigationStartTime
      << " firstScreenPaintTime: " << firstScreenPaintTime;
  
  if (auto handler = handler_.lock()) {
    handler->OnFirstScreenPaint(url, navigationStartTime, firstScreenPaintTime);
  }
}
#endif

#if BUILDFLAG(ARKWEB_ACCESSIBILITY)
void NWebRenderHandler::OnAccessibilityEvent(int64_t accessibilityId,
                                             int32_t eventType,
                                             const CefString& argument) {
  if (auto handler = handler_.lock()) {
    handler->OnAccessibilityEventV2(accessibilityId, eventType,
                                    argument.ToString());
  }
}
#endif
}  // namespace OHOS::NWeb
