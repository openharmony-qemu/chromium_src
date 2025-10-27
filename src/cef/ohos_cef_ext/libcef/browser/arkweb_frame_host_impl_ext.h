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

#ifndef CEF_LIBCEF_BROWSER_FRAME_HOST_IMPL_EXT_H_
#define CEF_LIBCEF_BROWSER_FRAME_HOST_IMPL_EXT_H_
#pragma once

#include <memory>
#include <optional>
#include <queue>
#include <string>

#include "arkweb/build/features/features.h"
#include "base/memory/raw_ptr.h"
#include "base/synchronization/lock.h"
#include "cef/include/cef_frame.h"
#include "cef/libcef/browser/frame_host_impl.h"
#include "cef/libcef/browser/image_impl.h"
#include "cef/libcef/common/mojom/cef.mojom.h"
#include "cef/libcef/renderer/blink_glue.h"
#include "cef/ohos_cef_ext/include/arkweb_frame_ext.h"
#include "content/public/browser/global_routing_id.h"
#include "mojo/public/cpp/bindings/receiver_set.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "ui/base/page_transition_types.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

class ArkwebFrameHostExtImpl : public ArkwebFrameExt, public CefFrameHostImpl {
 public:
  CefRefPtr<ArkwebFrameExt> AsArkWebFrame() override { return this; }
  CefRefPtr<CefFrameHostImpl> AsCefFrameHostImpl() override { return this; }
  ArkwebFrameHostExtImpl(
      scoped_refptr<CefBrowserInfo> browser_info,
      std::optional<content::GlobalRenderFrameHostToken> parent_frame_token)
      : CefFrameHostImpl(browser_info, parent_frame_token) {}
  ArkwebFrameHostExtImpl(scoped_refptr<CefBrowserInfo> browser_info,
                         content::RenderFrameHost* render_frame_host)
      : CefFrameHostImpl(browser_info, render_frame_host) {}

  void PostURL(const CefString& url,
               const std::vector<char>& post_data) override;
  void LoadHeaderUrl(const CefString& url,
                     const CefString& additionalHttpHeaders) override;
  void GetImages(CefRefPtr<CefGetImagesCallback> callback) override;

#if BUILDFLAG(ARKWEB_SYNC_RENDER)
  void UpdateDrawRect() override;
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  void LoadURLWithUserGesture(const CefString& url,
                              bool user_gesture = false) override;
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void ScrollToWithAnime(float x, float y, int32_t duration);
  void ScrollByWithAnime(float delta_x, float delta_y, int32_t duration);
  void SetInitialScale(float scale);
#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
  void GetOverScrollOffset(float* offset_x, float* offset_y);
#endif
#endif
#if BUILDFLAG(ARKWEB_COMPOSITE_RENDER)
  void OnGetImageFromCache(std::string url,
                           int command_id,
                           uint32_t buffer_size,
                           base::ReadOnlySharedMemoryRegion region);
  void PutZoomingForTextFactorEx(float factor) override {
    PutZoomingForTextFactor(factor);
  }
  void PutZoomingForTextFactor(float factor);
#endif

#if BUILDFLAG(ARKWEB_TERMINATE_RENDER)
  void TerminateRenderProcess(bool& result);
#endif
#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
void SetIsFling(bool is_fling);
#endif
#if BUILDFLAG(ARKWEB_CLIPBOARD)
  void GetImageForContextNode(int command_id);
  void OnGetImageForContextNode(
      cef::mojom::GetImageForContextNodeParamsPtr params,
      int command_id) override;
  void OnGetImageForContextNodeNull(int command_id) override;
#if BUILDFLAG(ARKWEB_NWEB_EX)
  void OnGetImageFromCacheEx(std::string url,
                             int command_id,
                             uint32_t buffer_size,
                             base::ReadOnlySharedMemoryRegion region);
#endif
#endif  // BUILDFLAG(ARKWEB_CLIPBOARD)

#if BUILDFLAG(ARKWEB_NETWORK_CONNINFO)
  void SetJsOnlineProperty(bool network_up) override;
#endif  // BUILDFLAG(ARKWEB_NETWORK_CONNINFO)

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  void ScrollTo(float x, float y);
  void ScrollBy(float delta_x, float delta_y);
  void SlideScroll(float vx, float vy);
  void ZoomBy(float delta, float width, float height);
  void GetHitData(int& type, CefString& extra_data);
  void GetLastHitData(int& type, CefString& extra_data);
  void UpdateHitTestData(int32_t type, const std::string& extra_data) override;
  void SetOverscrollMode(int mode);
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_MEDIA)
  void GetImagesCallback(CefRefPtr<CefFrameHostImpl> frame,
                         CefRefPtr<CefGetImagesCallback> callback,
                         bool response);
  void GetImagesWithResponse(
      cef::mojom::RenderFrame::GetImagesWithResponseCallback response_callback);
#endif  // BUILDFLAG(ARKWEB_MEDIA)

#if BUILDFLAG(ARKWEB_PAGE_UP_DOWN)
  void ScrollPageUpDown(bool is_up, bool is_half, float view_height);
#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
  void GetScrollOffset(float* offset_x, float* offset_y);
#endif
#endif  // #ARKWEB_PAGE_UP_DOWN

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  void ShouldOverrideUrlLoading(
      const std::string& url,
      const std::string& request_method,
      bool user_gesture,
      bool is_redirect,
      bool is_outermost_main_frame,
      cef::mojom::BrowserFrame::ShouldOverrideUrlLoadingCallback callback)
      override;
#endif

#if BUILDFLAG(ARKWEB_ERROR_PAGE)
  void OverrideErrorPage(
    const std::string& url,
    const std::string& request_method,
    bool user_gesture,
    bool is_redirect,
    bool is_outermost_main_frame,
    int error_code,
    const std::string& error_text,
    cef::mojom::BrowserFrame::OverrideErrorPageCallback callback)
    override;
#endif

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
  void RemoveCache(bool include_disk_files);
#endif
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  bool SetFocusByPosition(float x, float y);
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  virtual void SendBlanklessKeyToRenderFrame(uint32_t nweb_id,
                                             uint64_t blankless_key,
                                             uint64_t frame_sink_id,
                                             int64_t pref_hash) override;
#endif
#if BUILDFLAG(IS_ARKWEB)
 private:
  using RenderFrameType = mojo::Remote<cef::mojom::RenderFrame>;

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  struct CefHitData {
    int type;
    CefString extra_data;
    CefHitData() : type(0), extra_data("") {}
  };
  CefHitData hit_data_;
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
  base::WeakPtrFactory<ArkwebFrameHostExtImpl> weak_ptr_factory_{this};
#endif
};
#endif  // CEF_LIBCEF_BROWSER_FRAME_HOST_IMPL_EXT_H_
