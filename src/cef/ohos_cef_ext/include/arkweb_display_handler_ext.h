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

#ifndef ARKWEB_INCLUDE_CEF_DISPLAY_HANDLER_H_
#define ARKWEB_INCLUDE_CEF_DISPLAY_HANDLER_H_
#pragma once

#include "include/cef_base.h"
#include "include/cef_browser.h"
#include "include/cef_display_handler.h"
#include "include/cef_frame.h"

#if BUILDFLAG(ARKWEB_NAVIGATION)
#include "ohos_nweb/src/capi/nweb_icon_size.h"
#endif  // BUILDFLAG(ARKWEB_NAVIGATION)

class ArkWebDisplayHandlerExt : public virtual CefDisplayHandler,
                                public virtual CefBaseRefCounted {
 public:
  ///
  /// Called when the viewport-fit meta is detected for web page.
  ///
  virtual void OnViewportFitChange(CefRefPtr<CefBrowser> browser,
                                   int viewport_fit) {}

  ///
  /// onReceivedTouchIconUrl.
  ///
  virtual void OnReceivedTouchIconUrl(CefRefPtr<CefBrowser> browser,
                                      const CefString& icon_url,
                                      bool precomposed) {}

  ///
  /// onReceivedIcon.
  ///
  virtual void OnReceivedIcon(const void* data,
                              size_t width,
                              size_t height,
                              cef_color_type_t color_type,
                              cef_alpha_type_t alpha_type) {}

  ///
  /// OnReceivedIconUrl.
  ///
  virtual void OnReceivedIconUrl(const CefString& image_url,
                                 const void* data,
                                 size_t width,
                                 size_t height,
                                 cef_color_type_t color_type,
                                 cef_alpha_type_t alpha_type) {}

  ///
  /// Called when the page scale factor has changed.
  ///
  virtual void OnScaleChanged(CefRefPtr<CefBrowser> browser,
                              float old_page_scale_factor,
                              float new_page_scale_factor) {}

  virtual void OnScaleInited(CefRefPtr<CefBrowser> browser,
                             float page_scale_factor) {}

  ///
  /// GetWindowId from nweb.
  ///
  virtual uint32_t GetWindowId() { return 0; }

  ///
  /// Called when the page browser zoom has changed.
  ///
  virtual void OnContentsBrowserZoomChange(double zoom_factor,
                                           bool can_show_bubble) {}

#if BUILDFLAG(ARKWEB_NAVIGATION)
  ///
  /// onTouchIconUrlWithSizesReceived.
  ///
  /*--cef()--*/
  virtual void OnTouchIconUrlWithSizesReceived(
      const CefString& image_url,
      bool precomposed,
      const std::vector<IconSize>& sizes) {}
#endif  // BUILDFLAG(ARKWEB_NAVIGATION)

  ///
  /// Called when the browser is about to display a pip. 
  ///
  virtual bool OnPip(CefRefPtr<CefBrowser> browser,
                     int status,
                     int delegate_id,
                     int child_id,
                     int frame_routing_id,
                     int width,
                     int height) {
    return false;
  }
};
#endif
