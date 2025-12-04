// Copyright (c) 2022 Huawei Device Co., Ltd.
// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos_cef_ext/libcef/browser/arkweb_icon_helper_ext.h"

#include "base/logging.h"
#include "components/favicon_base/select_favicon_frames.h"
#include "ohos_cef_ext/include/arkweb_display_handler_ext.h"
#if BUILDFLAG(ARKWEB_NAVIGATION)
#include "content/public/browser/favicon_status.h"
#include "content/public/browser/navigation_controller.h"
#include "content/public/browser/navigation_entry.h"
#endif  // BUILDFLAG(ARKWEB_NAVIGATION)
#include "content/public/browser/web_contents.h"
#include "third_party/blink/public/mojom/favicon/favicon_url.mojom.h"

#if BUILDFLAG(ARKWEB_WPT)
#include "base/hash/hash.h"
#endif  // BUILDFLAG(ARKWEB_WPT)

#if BUILDFLAG(ARKWEB_FAVICON)
#include "content/public/browser/navigation_handle.h"
#include "arkweb/chromium_ext/url/ohos/log_utils.h"
#endif  // BUILDFLAG(ARKWEB_FAVICON)

#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "base/command_line.h"
#include "base/ohos/sys_info_utils_ext.h"
#include "components/favicon_base/favicon_util.h"
#include "components/favicon_base/select_favicon_frames.h"
#include "content/public/common/content_switches.h"
#include "ui/gfx/favicon_size.h"
#endif

namespace {

constexpr int LARGEST_ICON_SIZE = 192;

cef_color_type_t TransformSkColorType(SkColorType color_type) {
  switch (color_type) {
    case kRGBA_8888_SkColorType:
      return CEF_COLOR_TYPE_RGBA_8888;
    case kBGRA_8888_SkColorType:
      return CEF_COLOR_TYPE_BGRA_8888;
    default:
      return CEF_COLOR_TYPE_UNKNOWN;
  }
}

cef_alpha_type_t TransformSkAlphaType(SkAlphaType alpha_type) {
  switch (alpha_type) {
    case kOpaque_SkAlphaType:
      return CEF_ALPHA_TYPE_OPAQUE;
    case kPremul_SkAlphaType:
      return CEF_ALPHA_TYPE_PREMULTIPLIED;
    case kUnpremul_SkAlphaType:
      return CEF_ALPHA_TYPE_POSTMULTIPLIED;
    default:
      return CEF_ALPHA_TYPE_UNKNOWN;
  }
}

#if BUILDFLAG(ARKWEB_NWEB_EX)
std::vector<int> GetDesiredPixelSizes() {
  std::vector<int> pixel_sizes;
  for (float scale_factor : favicon_base::GetFaviconScales()) {
    pixel_sizes.push_back(
        static_cast<int>(ceil(scale_factor * gfx::kFaviconSize)));
  }
  return pixel_sizes;
}

void sortCallbackData(std::vector<IconHelper::CallbackData>& data_list) {
  std::sort(data_list.begin(), data_list.end(),
    [](const IconHelper::CallbackData& a, const IconHelper::CallbackData& b) {
      return a.score < b.score;
    });
}
#endif
}  // namespace

#if BUILDFLAG(ARKWEB_NAVIGATION)
namespace content {
struct FaviconStatus;
}
#endif  // BUILDFLAG(ARKWEB_NAVIGATION)

void IconHelper::SetDisplayHandler(
    const CefRefPtr<ArkWebDisplayHandlerExt>& handler) {
  handler_ = handler;
}

void IconHelper::SetWebContents(content::WebContents* new_contents) {
  web_contents_ = new_contents;
}

void IconHelper::OnUpdateFaviconURL(
    content::RenderFrameHost* render_frame_host,
    const std::vector<blink::mojom::FaviconURLPtr>& candidates,
    CefRefPtr<CefBrowser> browser) {
  if (!handler_ || !browser) {
    LOG(ERROR) << "Initialized value is invalid";
    return;
  }
  static int request_id = 0;
  request_id++;
  for (const auto& candidate : candidates) {
#if BUILDFLAG(ARKWEB_WPT)
    if (!candidate->icon_url.is_valid() ||
        CheckFailedFaviconUrl(candidate->icon_url)) {
#else
    if (!candidate->icon_url.is_valid()) {
#endif
      continue;
    }
    std::vector<IconSize> sizes;
    for (const auto& size : candidate->icon_sizes) {
      sizes.emplace_back(size.width(), size.height());
    }
    if (sizes.empty()) {
      LOG(WARNING) << "No icon sizes available for URL: ***";
    }
    switch (candidate->icon_type) {
      case blink::mojom::FaviconIconType::kFavicon:
        DownloadFavicon(candidate, request_id, browser);
        break;
      case blink::mojom::FaviconIconType::kTouchIcon:
        handler_->OnReceivedTouchIconUrl(
            browser, CefString(candidate->icon_url.spec()), false);
        handler_->OnTouchIconUrlWithSizesReceived(
            CefString(candidate->icon_url.spec()), false, sizes);
        break;
      case blink::mojom::FaviconIconType::kTouchPrecomposedIcon:
        handler_->OnReceivedTouchIconUrl(
            browser, CefString(candidate->icon_url.spec()), true);
        handler_->OnTouchIconUrlWithSizesReceived(
            CefString(candidate->icon_url.spec()), true, sizes);
        break;
      case blink::mojom::FaviconIconType::kInvalid:
        LOG(INFO) << "An invalid icon type.";
        break;
      default:
        NOTREACHED();
    }
  }
}

void IconHelper::DownloadFavicon(const blink::mojom::FaviconURLPtr& candidate, int request_id,
                                CefRefPtr<CefBrowser> browser) {
  if (!web_contents_) {
    LOG(ERROR) << "WebContents is invalid";
    return;
  }

#if BUILDFLAG(ARKWEB_NWEB_EX)
  std::lock_guard<std::mutex> lock(mutex_);
  pending_downloads_map_[request_id].insert(candidate->icon_url.spec());
#endif
  web_contents_->DownloadImage(
      candidate->icon_url,
      true,               // Is a favicon
      gfx::Size(),        // No preferred size
      LARGEST_ICON_SIZE,  // Max bitmap size 192
      false,              // Normal cache policy
      base::BindOnce(&IconHelper::DownloadFaviconCallback, this, request_id, browser));
}

void IconHelper::DownloadFaviconCallback(
    int request_id,
    CefRefPtr<CefBrowser> browser,
    int id,
    int http_status_code,
    const GURL& image_url,
    const std::vector<SkBitmap>& bitmaps,
    const std::vector<gfx::Size>& original_bitmap_sizes) {
  if (http_status_code == 404) {
#if BUILDFLAG(ARKWEB_WPT)
    InsertFailedFaviconUrl(image_url);
#endif  // BUILDFLAG(ARKWEB_WPT)
    return;
  }

  if (bitmaps.size() == 0) {
    return;
  }

#if BUILDFLAG(ARKWEB_FAVICON)
  if (ShouldAbort()) {
    return;
  }
#endif  // BUILDFLAG(ARKWEB_FAVICON)

  std::vector<size_t> best_indices;
  SelectFaviconFrameIndices(original_bitmap_sizes,
                            std::vector<int>(1U, LARGEST_ICON_SIZE),
                            &best_indices, nullptr);
  const auto& bitmap =
      bitmaps[best_indices.size() == 0 ? 0 : best_indices.front()];
#if BUILDFLAG(ARKWEB_NWEB_EX)
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(::switches::kEnableNwebEx) &&
      base::ohos::IsPcDevice()) {
    float current_score;
    SelectFaviconFrameIndices(original_bitmap_sizes, GetDesiredPixelSizes(), &best_indices, &current_score);
    SkBitmap new_bitmap;
    new_bitmap.allocPixels(bitmap.info());
    bitmap.readPixels(new_bitmap.pixmap(), 0, 0);
    best_results_map_[request_id].push_back({current_score, image_url, new_bitmap});
    std::lock_guard<std::mutex> lock(mutex_);
    std::unordered_set<std::string>& pending_urls = pending_downloads_map_[request_id];
    pending_urls.erase(image_url.spec());
    if (!pending_urls.empty()) {
      return;
    }
    sortCallbackData(best_results_map_[request_id]);
    for (CallbackData& data : best_results_map_[request_id]) {
      DownloadFaviconHandler(data.image_url, data.bitmap, browser);
    }
    pending_downloads_map_.erase(request_id);
    best_results_map_.erase(request_id);
    return;
  }
#endif
  DownloadFaviconHandler(image_url, bitmap, browser);
}

void IconHelper::DownloadFaviconHandler(
    const GURL& image_url,
    const SkBitmap& bitmap,
    CefRefPtr<CefBrowser> browser) {
  if (bitmap.drawsNothing()) {
    return;
  }
  bitmap_ = bitmap;
  auto ret = bitmap_.writePixels(bitmap.pixmap());
  if (!ret) {
    LOG(ERROR) << "Copy pixels failed.";
    return;
  }
  void* pixels = bitmap_.getPixels();
  size_t byte_size = bitmap_.computeByteSize();
  if (!pixels || byte_size == 0) {
    LOG(ERROR) << "Pixels value is invalid.";
    return;
  }
  auto color_type = TransformSkColorType(bitmap_.colorType());
  auto alpha_type = TransformSkAlphaType(bitmap_.alphaType());
  size_t pixel_width = bitmap_.width();
  size_t pixel_height = bitmap_.height();

#if BUILDFLAG(ARKWEB_NAVIGATION)
  if (web_contents_) {
    content::NavigationEntry* entry =
        web_contents_->GetController().GetLastCommittedEntry();
    if (entry) {
      entry->GetFavicon().valid = true;
      entry->GetFavicon().url = image_url;
      entry->GetFavicon().image = gfx::Image::CreateFrom1xBitmap(bitmap);
    }
  }
#endif  // BUILDFLAG(ARKWEB_NAVIGATION)

  OnReceivedIcon(pixels, pixel_width, pixel_height, color_type, alpha_type,
                browser);
  OnReceivedIconUrl(CefString(image_url.spec()), pixels, pixel_width,
                    pixel_height, color_type, alpha_type, browser);
}

void IconHelper::OnReceivedIcon(const void* data,
                                size_t width,
                                size_t height,
                                cef_color_type_t color_type,
                                cef_alpha_type_t alpha_type,
                                CefRefPtr<CefBrowser> browser) {
  if (!handler_ || !browser) {
    LOG(ERROR) << "Initialized value is invalid";
    return;
  }
  handler_->OnReceivedIcon(data, width, height, color_type, alpha_type);
}

void IconHelper::OnReceivedIconUrl(const CefString& image_url,
                                   const void* data,
                                   size_t width,
                                   size_t height,
                                   cef_color_type_t color_type,
                                   cef_alpha_type_t alpha_type,
                                CefRefPtr<CefBrowser> browser) {
  if (!handler_ || !browser) {
    LOG(ERROR) << "Initialized value is invalid";
    return;
  }
  handler_->OnReceivedIconUrl(image_url, data, width, height, color_type,
                              alpha_type);
}

#if BUILDFLAG(ARKWEB_WPT)
void IconHelper::InsertFailedFaviconUrl(const GURL& icon_url) {
  size_t url_hash = base::FastHash(icon_url.spec());
  failed_favicon_urls_set_.insert(url_hash);
}

bool IconHelper::CheckFailedFaviconUrl(const GURL& icon_url) {
  size_t url_hash = base::FastHash(icon_url.spec());
  return failed_favicon_urls_set_.find(url_hash) !=
         failed_favicon_urls_set_.end();
}

void IconHelper::ClearFailedFaviconUrlSets(
    content::NavigationHandle* navigation) {
  if (navigation && navigation->IsInPrimaryMainFrame() &&
      navigation->GetReloadType() == content::ReloadType::BYPASSING_CACHE) {
    failed_favicon_urls_set_.clear();
  }
}
#endif  // BUILDFLAG(ARKWEB_WPT)

#if BUILDFLAG(ARKWEB_FAVICON)
// should abort fetching favicon or not.
bool IconHelper::ShouldAbort() {
  if (!web_contents_) {
    return true;
  }

  GURL page_url = web_contents_->GetURL();
  if (!document_on_load_completed_) {
    LOG(INFO) << "should abort fetching favicon, document load not completed,"
              << " page_url: " << url::LogUtils::ConvertUrlWithMask(page_url.spec());
    return true;
  }

  if (last_page_url_ != web_contents_->GetURL()) {
    LOG(INFO) << "should abort fetching favicon, page urls not equal,"
              << " last_page_url_:" << url::LogUtils::ConvertUrlWithMask(last_page_url_.spec())
              << ", page_url:" << url::LogUtils::ConvertUrlWithMask(page_url.spec());
    return true;
  }

  return false;
}

void IconHelper::SetMainFrameDocumentOnLoadCompleted(bool complete) {
  document_on_load_completed_ = complete;
}

void IconHelper::SetLastPageUrl(const GURL& url) {
  last_page_url_ = url;
}
#endif  // BUILDFLAG(ARKWEB_FAVICON)
