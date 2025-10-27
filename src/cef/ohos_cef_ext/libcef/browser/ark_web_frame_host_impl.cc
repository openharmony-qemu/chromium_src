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

#include "cef/ohos_cef_ext/libcef/browser/ark_web_frame_host_impl.h"

#include "cef/libcef/browser/frame_host_impl.h"

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
#include "base/strings/escape.h"
#include "cef/ohos_cef_ext/libcef/common/net/url_util_ex.h"
#include "cef/libcef/browser/thread_util.h"
#include "content/browser/renderer_host/frame_tree.h"
#include "content/browser/renderer_host/render_frame_host_impl.h"
#endif
 
namespace {
 
constexpr char kMethodPost[] = "POST";
 
}  // namespace

int64_t CefFrameHostImpl::GetFrameId() const {
  base::AutoLock lock_scope(state_lock_);
  return is_main_frame_ ? kMainFrameId : frame_id_;
}

GURL ArkWebFixupFileUrl(const std::string& url) {
  GURL gurl = url_util::FixupGURL(url);
  if (!base::StartsWith(url, "file:/") && gurl.SchemeIsFile()) {
    std::string unscaped_url_str = base::UnescapeURLComponent(
        gurl.spec(),
        base::UnescapeRule::URL_SPECIAL_CHARS_EXCEPT_PATH_SEPARATORS);
    gurl = GURL(unscaped_url_str);
  }
  return gurl;
}
 
void ArkWebDealWithPostData(const std::string& method,
                            const std::vector<char>& post_data,
                            content::OpenURLParams* params) {
  if (method == kMethodPost) {
    if (post_data.size() <= 0) {
      params->post_data = new network::ResourceRequestBody();
    } else {
      params->post_data = network::ResourceRequestBody::CreateFromBytes(
          reinterpret_cast<const char*>(&post_data[0]), post_data.size());
    }
  }
}
 
void ArkWebDealWithPostUploadData(const std::string& method,
                                  const std::vector<char>& post_data,
                                  cef::mojom::RequestParamsPtr& params) {
  if (method == kMethodPost) {
    params->method = method;
    if (post_data.size() <= 0) {
      params->upload_data = new network::ResourceRequestBody();
    } else {
      params->upload_data = network::ResourceRequestBody::CreateFromBytes(
          reinterpret_cast<const char*>(&post_data[0]), post_data.size());
    }
  }
}
 
#if BUILDFLAG(ARKWEB_SCREEN_ROTATION)
void CefFrameHostImpl::UpdatePixelRatio(float ratio) {
  LOG(INFO) << "UpdatePixelRatio in browser CefFrameHostImpl start, ratio:"
            << ratio;
  SendToRenderFrame(__FUNCTION__,
                    base::BindOnce(
                        [](float ratio, const RenderFrameType& render_frame) {
                          render_frame->UpdatePixelRatio(ratio);
                        },
                        ratio));
}
#endif
 
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void CefFrameHostImpl::SendHitEvent(float x,
                                    float y,
                                    float width,
                                    float height) {
  cef::mojom::HitEventParamsPtr hit_event = cef::mojom::HitEventParams::New();
  hit_event->x = x;
  hit_event->y = y;
  hit_event->width = width;
  hit_event->height = height;
  SendToRenderFrame(__FUNCTION__,
                    base::BindOnce(
                        [](cef::mojom::HitEventParamsPtr hit_event,
                           const RenderFrameType& render_frame) {
                          render_frame->SendHitEvent(std::move(hit_event));
                        },
                        std::move(hit_event)));
}
 
void CefFrameHostImpl::SetScrollable(bool enable) {
  SendToRenderFrame(__FUNCTION__,
                    base::BindOnce(
                        [](bool enable, const RenderFrameType& render_frame) {
                          render_frame->SetScrollable(enable);
                        },
                        enable));
}
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
 
#if BUILDFLAG(ARKWEB_OPTIMIZE_PARSER_BUDGET)
void CefFrameHostImpl::SetOptimizeParserBudgetEnabled(bool enable) {
  SendToRenderFrame(__FUNCTION__,
                    base::BindOnce(
                        [](bool enable, const RenderFrameType& render_frame) {
                          render_frame->SetOptimizeParserBudgetEnabled(enable);
                        },
                        enable));
}
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
content::RenderFrameHost* CefFrameHostImpl::GetRenderFrameHostFromGlobalId()
    const {
  CEF_REQUIRE_UIT();
  if (rfh_global_id_) {
    return content::RenderFrameHost::FromID(rfh_global_id_);
  }
  return nullptr;
}

bool CefFrameHostImpl::IsPrerendering() {
  content::RenderFrameHostImpl* rfh = static_cast<content::RenderFrameHostImpl*>(
    GetRenderFrameHostFromGlobalId());
  if (!rfh) {
    return initial_is_prerendering_;
  }
  if (!rfh->frame_tree()) {
    return initial_is_prerendering_;
  }

  return rfh->frame_tree()->is_prerendering();
}
#endif
