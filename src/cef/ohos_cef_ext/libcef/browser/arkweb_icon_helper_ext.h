// Copyright (c) 2022 Huawei Device Co., Ltd.
// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_ICON_HELPER_H_
#define CEF_LIBCEF_BROWSER_ICON_HELPER_H_
#pragma once

#include <vector>
#include <queue>
 
#include "arkweb/build/features/features.h"
#include "base/memory/raw_ptr.h"
#include "include/cef_base.h"
#include "libcef/browser/thread_util.h"
#include "ohos_nweb/src/capi/nweb_icon_size.h"
#include "third_party/blink/public/mojom/favicon/favicon_url.mojom-forward.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "url/gurl.h"

#if BUILDFLAG(ARKWEB_WPT)
#include <unordered_set>

#include "content/public/browser/navigation_handle.h"
#endif  // BUILDFLAG(ARKWEB_WPT)

namespace gfx {
class Size;
}  // namespace gfx

namespace content {
class RenderFrameHost;
class WebContents;
}  // namespace content

class CefBrowser;
class ArkWebDisplayHandlerExt;
class GURL;

class IconHelper : public virtual CefBaseRefCounted {
 public:
  IconHelper() = default;
  ~IconHelper() { web_contents_ = nullptr; }

  IconHelper(const IconHelper&) = delete;
  IconHelper& operator=(const IconHelper&) = delete;

  void SetDisplayHandler(const CefRefPtr<ArkWebDisplayHandlerExt>& handler);
  void SetWebContents(content::WebContents* new_contents);
  void OnUpdateFaviconURL(
      content::RenderFrameHost* render_frame_host,
      const std::vector<blink::mojom::FaviconURLPtr>& candidates,
      CefRefPtr<CefBrowser> browser);
  void DownloadFavicon(const blink::mojom::FaviconURLPtr& candidate, int request_id,
                      CefRefPtr<CefBrowser> browser);
  void DownloadFaviconCallback(
      int request_id,
      CefRefPtr<CefBrowser> browser,
      int id,
      int http_status_code,
      const GURL& image_url,
      const std::vector<SkBitmap>& bitmaps,
      const std::vector<gfx::Size>& original_bitmap_sizes);
  void DownloadFaviconHandler(
      const GURL& image_url,
      const SkBitmap& bitmap,
      CefRefPtr<CefBrowser> browser);
  void OnReceivedIcon(const void* data,
                      size_t width,
                      size_t height,
                      cef_color_type_t color_type,
                      cef_alpha_type_t alpha_type,
                      CefRefPtr<CefBrowser> browser);

  void OnReceivedIconUrl(const CefString& image_url,
                         const void* data,
                         size_t width,
                         size_t height,
                         cef_color_type_t color_type,
                         cef_alpha_type_t alpha_type,
                         CefRefPtr<CefBrowser> browser);

#if BUILDFLAG(ARKWEB_WPT)
  void ClearFailedFaviconUrlSets(content::NavigationHandle* navigation_handle);
#endif  // BUILDFLAG(ARKWEB_WPT)

#if BUILDFLAG(ARKWEB_FAVICON)
  void SetMainFrameDocumentOnLoadCompleted(bool complete);
  void SetLastPageUrl(const GURL& url);
#endif  // BUILDFLAG(ARKWEB_FAVICON)

#if BUILDFLAG(ARKWEB_NWEB_EX)
  struct CallbackData {
    float score;
    GURL image_url;
    SkBitmap bitmap;
 
    CallbackData() {}
 
    CallbackData(float score_num, const GURL& url,
                 const SkBitmap& bmp)
        : score(score_num), image_url(url),
          bitmap(bmp) {}
  };
#endif

 private:
#if BUILDFLAG(ARKWEB_WPT)
  void InsertFailedFaviconUrl(const GURL& icon_url);
  bool CheckFailedFaviconUrl(const GURL& icon_url);
#endif  // BUILDFLAG(ARKWEB_WPT)

#if BUILDFLAG(ARKWEB_FAVICON)
  bool ShouldAbort();
  bool document_on_load_completed_ = false;
  GURL last_page_url_;
#endif  // BUILDFLAG(ARKWEB_FAVICON)

  raw_ptr<content::WebContents> web_contents_ = nullptr;
  CefRefPtr<ArkWebDisplayHandlerExt> handler_ = nullptr;
  SkBitmap bitmap_;

#if BUILDFLAG(ARKWEB_WPT)
  std::unordered_set<size_t> failed_favicon_urls_set_;
#endif  // BUILDFLAG(ARKWEB_WPT)

#if BUILDFLAG(ARKWEB_NWEB_EX)
  std::map<int, std::unordered_set<std::string>> pending_downloads_map_;
  std::map<int, std::vector<CallbackData>> best_results_map_;
  std::mutex mutex_;
#endif

  IMPLEMENT_REFCOUNTING_DELETE_ON_UIT(IconHelper);
};

#endif  // CEF_LIBCEF_BROWSER_ICON_HELPER_H_
