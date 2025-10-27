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

#ifndef CEF_LIBCEF_RENDERER_FRAME_IMPL_EXT_H_
#define CEF_LIBCEF_RENDERER_FRAME_IMPL_EXT_H_
#pragma once

#include <queue>
#include <string>

#include "arkweb/build/features/features.h"
#include "base/memory/weak_ptr.h"
#include "base/timer/timer.h"
#include "cef/include/cef_frame.h"
#include "cef/include/cef_v8.h"
#include "cef/libcef/common/mojom/cef.mojom.h"
#include "cef/libcef/renderer/blink_glue.h"
#include "cef/ohos_cef_ext/include/arkweb_frame_ext.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/receiver.h"
#include "mojo/public/cpp/bindings/remote.h"

class ArkwebFrameExtImpl : public ArkwebFrameExt, public CefFrameImpl {
 public:
  ArkwebFrameExtImpl(CefBrowserImpl* browser, blink::WebLocalFrame* frame)
      : CefFrameImpl(browser, frame) {}
  CefRefPtr<ArkwebFrameExt> AsArkWebFrame() override { return this; }
// TODO:
#if BUILDFLAG(IS_OHOS)
  void GetImages(CefRefPtr<CefGetImagesCallback> callback) override;
  void LoadHeaderUrl(const CefString& url,
                     const CefString& additionalHttpHeaders) override;
#endif  // BUILDFLAG(IS_OHOS)
#if BUILDFLAG(ARKWEB_POST_URL)
  void PostURL(const CefString& url,
               const std::vector<char>& post_data) override;
#endif  // BUILDFLAG(ARKWEB_POST_URL)

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  void LoadURLWithUserGesture(const CefString& url, bool user_gesture = false) override;
#endif
#if BUILDFLAG(ARKWEB_SYNC_RENDER)
  void UpdateDrawRect() override;
#endif

#if BUILDFLAG(ARKWEB_MEDIA)
  void GetImagesWithResponse(
      cef::mojom::RenderFrame::GetImagesWithResponseCallback response_callback)
      override;
#endif  // BUILDFLAG(ARKWEB_MEDIA)

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void ScrollToWithAnime(float x, float y, int32_t duration) override;
  void ScrollByWithAnime(float delta_x,
                         float delta_y,
                         int32_t duration) override;
  void SetInitialScale(float initialScale) override;
#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
  void GetScrollOffset(
      cef::mojom::RenderFrame::GetScrollOffsetCallback callback) override;
  void GetOverScrollOffset(
      cef::mojom::RenderFrame::GetOverScrollOffsetCallback callback) override;
#endif
#endif
#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
  void PutZoomingForTextFactor(float factor) override;
#endif

#if BUILDFLAG(ARKWEB_TERMINATE_RENDER)
  void TerminateRenderProcess() override;
#endif

#if BUILDFLAG(ARKWEB_CLIPBOARD)
  void GetImageForContextNode(int command_id) override;
  int total_mem_ = -1;
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  bool ShouldOverrideUrlLoading(const CefString& url,
                                const CefString& request_method,
                                bool user_gesture,
                                bool is_redirect,
                                bool is_outermost_main_frame) override;
#endif

#if BUILDFLAG(ARKWEB_ERROR_PAGE)
std::string OverrideErrorPage(const CefString& url,
                              const CefString& request_method,
                              bool user_gesture,
                              bool is_redirect,
                              bool is_outermost_main_frame,
                              int error_code,
                              const CefString& error_text) override;
#endif

#if BUILDFLAG(ARKWEB_SCROLLBAR)
  void UpdatePixelRatio(float ratio) override;
#endif  // BUILDFLAG(IS_OHOS)

#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
  void SetIsFling(bool is_fling) override;
#endif

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
  void RemoveCache() override;
#endif

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  virtual void SendBlanklessKeyToRenderFrame(uint32_t nweb_id,
                                             uint64_t blankless_key,
                                             uint64_t frame_sink_id,
                                             int64_t pref_hash) override;
#endif

 private:
#if BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
  void SetJsOnlineProperty(bool network_up) override;
#endif
#if BUILDFLAG(ARKWEB_OPTIMIZE_PARSER_BUDGET)
  void SetOptimizeParserBudgetEnabled(bool enable) override;
#endif
#if BUILDFLAG(ARKWEB_PAGE_UP_DOWN)
  void ScrollPageUpDown(bool is_up, bool is_half, float view_height) override;
#endif  // #if BUILDFLAG(ARKWEB_PAGE_UP_DOWN)
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  struct CefHitData {
    int type;
    CefString extra_data;
    CefHitData() : type(0), extra_data("") {}
  };
  void OnFocusedNodeChanged(const blink::WebElement& element);
  void GetHitData(
      cef::mojom::RenderFrame::GetHitDataCallback callback) override;
  void ScrollTo(float x, float y) override;
  void ScrollBy(float delta_x, float delta_y) override;
  void SlideScroll(float vx, float vy) override;
  void ZoomBy(float delta, float width, float height) override;
  void SetOverscrollMode(int mode) override;
  void SetFocusByPosition(float x, float y, cef::mojom::RenderFrame::SetFocusByPositionCallback callback) override;
  void SendHitEvent(cef::mojom::HitEventParamsPtr params) override;
  GURL GetChildImageUrlFromElement(const blink::WebElement& element);
  blink::WebElement GetImgChild(const blink::WebNode& node);
  GURL GetAbsoluteSrcUrl(const blink::WebElement& element);
  GURL GetAbsoluteUrl(const blink::WebNode& node,
                      const std::u16string& url_fragment);
  void PopulateHitTestData(const GURL& absolute_link_url,
                           const GURL& absolute_image_url,
                           bool is_editable,
                           cef::mojom::HitDataParamsPtr& data);
  bool RemovePrefixAndAssignIfMatches(const std::string_view prefix,
                                      const GURL& url,
                                      std::string* dest);
  void DistinguishAndAssignSrcLinkType(const GURL& url,
                                       cef::mojom::HitDataParamsPtr& data);
  void SetScrollable(bool enable) override;
  CefHitData cef_hit_data_;
  bool is_update_ = false;
  bool scroll_enabled_ = true;
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
};
#endif  // CEF_LIBCEF_RENDERER_FRAME_IMPL_EXT_H_
