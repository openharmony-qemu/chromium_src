// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "arkweb/build/features/features.h"
#include "content/browser/renderer_host/media/media_stream_manager.h"

#if BUILDFLAG(ARKWEB_MEDIA_AVSESSION)
#include "content/browser/media/session/media_session_impl.h"
#endif  // ARKWEB_MEDIA_AVSESSION

#include "content/browser/media/session/media_session_impl.h"

#if BUILDFLAG(IS_ARKWEB)
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#endif

#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
#include "cc/input/browser_controls_state.h"
#endif

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
#include "content/public/common/referrer.h"
#endif

#if BUILDFLAG(ARKWEB_EXT_NAVIGATION)
#include "content/public/browser/navigation_controller.h"
#endif

#if BUILDFLAG(ARKWEB_USERAGENT)
#include "cef/libcef/browser/alloy/alloy_browser_host_impl.h"
#endif

#if BUILDFLAG(ARKWEB_JAVASCRIPT_BRIDGE)
#include "cef/ohos_cef_ext/libcef/browser/javascript/oh_javascript_injector.h"
#endif

#if BUILDFLAG(ARKWEB_PERMISSION)
#include "ohos_cef_ext/libcef/browser/permission/alloy_access_request.h"
#include "ohos_cef_ext/libcef/browser/permission/alloy_geolocation_access.h"
#include "ohos_cef_ext/libcef/browser/permission/alloy_access_query.h"
#endif

static void StartDownloadExt(GURL& gurl,
                             content::WebContents* web_contents,
                             std::unique_ptr<download::DownloadUrlParameters>& params,
                             CefBrowserHostBase* obj)
{
  content::Referrer referrer = content::Referrer::SanitizeForRequest(
      gurl, content::Referrer(web_contents->GetLastCommittedURL(),
                              network::mojom::ReferrerPolicy::kDefault));
  params->set_referrer(referrer.url);
  CefString custom_user_agent;
  if (obj->AsAlloyBrowserHostImpl()) {
    custom_user_agent = obj->AsAlloyBrowserHostImpl()->GetCustomUserAgent();
  }
  if (!custom_user_agent.empty()) {
    params->add_request_header(net::HttpRequestHeaders::kUserAgent,
                               custom_user_agent);
  }
}

#if BUILDFLAG(ARKWEB_EX_SCREEN_CAPTURE)
void CefBrowserHostBase::StopScreenCapture(int32_t nweb_id,
                                           const CefString& session_id) {
  auto web_contents = GetWebContents();
  if (!web_contents) {
    LOG(ERROR) << "GetWebContents null";
    return;
  }
  std::string session_id_str = session_id;
  web_contents->StopScreenCapture(nweb_id, session_id_str);
}

void CefBrowserHostBase::SetScreenCapturePickerShow() {
  auto web_contents = GetWebContents();
  if (!web_contents) {
    LOG(ERROR) << "GetWebContents null";
    return;
  }
  web_contents->SetScreenCapturePickerShow();
}

void CefBrowserHostBase::DisableSessionReuse() {
  auto web_contents = GetWebContents();
  if (!web_contents) {
    LOG(ERROR) << "GetWebContents null";
    return;
  }
  web_contents->DisableSessionReuse();
}

void CefBrowserHostBase::RegisterScreenCaptureDelegateListener(
    CefRefPtr<CefScreenCaptureCallback> listener) {
  LOG(INFO) << "BrowserHostBase RegisterScreenCaptureDelegateListener";
  if (!listener) {
    LOG(ERROR) << "CefBrowserHostBase::RegisterScreenCaptureDelegateListener "
                  "listener is nullptr";
    return;
  }

  auto web_contents = GetWebContents();
  if (!web_contents) {
    LOG(ERROR) << "GetWebContents null";
    return;
  }

  content::MediaStreamManagerExt::SetScreenCaptureDelegateCallback(
      base::BindRepeating(
          [](CefRefPtr<CefScreenCaptureCallback> callback, int32_t nweb_id,
             const char* session_id, int32_t code) {
            callback->OnStateChange(nweb_id, CefString(session_id), code);
          },
          listener));
}
#endif  // defined(ARKWEB_EX_SCREEN_CAPTURE)

void CefBrowserHostBase::EnableVideoAssistant(bool enable) {
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (!GetWebContents()) {
    LOG(ERROR) << "failed to get content when enable video assistant";
    return;
  }

  GetWebContents()->EnableVideoAssistant(enable);
#endif  // ARKWEB_VIDEO_ASSISTANT
}

void CefBrowserHostBase::ExecuteVideoAssistantFunction(const CefString& cmdId) {
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (!GetWebContents()) {
    LOG(ERROR) << "failed to get content when execute video assistant function";
    return;
  }

  GetWebContents()->ExecuteVideoAssistantFunction(cmdId.ToString());
#endif  // ARKWEB_VIDEO_ASSISTANT
}

void CefBrowserHostBase::CustomWebMediaPlayer(bool enable) {
#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
  if (!GetWebContents()) {
    LOG(ERROR) << "failed to get content when enable custom web media player";
    return;
  }

  GetWebContents()->CustomWebMediaPlayer(enable);
#endif  // ARKWEB_VIDEO_ASSISTANT
}

void CefBrowserHostBase::SetMediaResumeFromBFCachePage(bool resume) {
  LOG(INFO) << "CefBrowserHostBase SetMediaResumeFromBFCachePage resume:" << resume;
#if BUILDFLAG(ARKWEB_BFCACHE)
  if (!GetWebContents()) {
    LOG(ERROR) << "failed to get content when set media resume playback";
    return;
  }
  GetWebContents()->SetMediaResumeFromBFCachePage(resume);
#endif  // BUILDFLAG(ARKWEB_BFCACHE)
}

#if BUILDFLAG(ARKWEB_PERMISSION)

CefRefPtr<CefBrowserPermissionRequestDelegate>
CefBrowserHostBase::GetPermissionRequestDelegate() {
  return this;
}

CefRefPtr<CefGeolocationAcess> CefBrowserHostBase::GetGeolocationPermissions() {
  if (geolocation_permissions_ == nullptr) {
    geolocation_permissions_ = new AlloyGeolocationAccess();
  }
  return geolocation_permissions_;
}

bool CefBrowserHostBase::UseLegacyGeolocationPermissionAPI() {
  return true;
}

void CefBrowserHostBase::AskGeolocationPermission(
    const CefString& origin,
    cef_permission_callback_t callback) {
  if (UseLegacyGeolocationPermissionAPI()) {
    PopupGeolocationPrompt(origin, std::move(callback));
    return;
  }
  permission_request_handler_->SendRequest(new AlloyAccessRequest(
      origin, AlloyAccessRequest::Resources::GEOLOCATION, std::move(callback)));
}

void CefBrowserHostBase::AbortAskGeolocationPermission(
    const CefString& origin) {
  RemoveGeolocationPrompt(origin);
  return;
}

void CefBrowserHostBase::PopupGeolocationPrompt(
    std::string origin,
    cef_permission_callback_t callback) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  bool show_prompt = unhandled_geolocation_prompts_.empty();
  unhandled_geolocation_prompts_.emplace_back(origin, std::move(callback));
  if (show_prompt) {
    OnGeolocationShow(origin);
  }
}

void CefBrowserHostBase::OnGeolocationShow(std::string origin) {
  // Reject if geoloaction is disabled, or the origin has a retained deny
  if (!settings_.geolocation_enabled) {
    NotifyGeolocationPermission(false, origin);
    return;
  }

  auto permissions = GetGeolocationPermissions();
  if (permissions->ContainOrigin(origin)) {
    NotifyGeolocationPermission(permissions->IsOriginAccessEnabled(origin),
                                origin);
    return;
  }

  GetClient()->AsArkWebClient()->GetPermissionRequest()->OnGeolocationShow(
      origin);
}

void CefBrowserHostBase::NotifyGeolocationPermission(bool value,
                                                     const CefString& origin) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  if (unhandled_geolocation_prompts_.empty()) {
    return;
  }
  if (origin == unhandled_geolocation_prompts_.front().first) {
    std::move(unhandled_geolocation_prompts_.front().second).Run(value);
    unhandled_geolocation_prompts_.pop_front();
    if (!unhandled_geolocation_prompts_.empty()) {
      OnGeolocationShow(unhandled_geolocation_prompts_.front().first);
    }
  }
}

void CefBrowserHostBase::RemoveGeolocationPrompt(std::string origin) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  bool removed_current_outstanding_callback = false;
  std::list<OriginCallback>::iterator it =
      unhandled_geolocation_prompts_.begin();
  while (it != unhandled_geolocation_prompts_.end()) {
    if ((*it).first == origin) {
      if (it == unhandled_geolocation_prompts_.begin()) {
        removed_current_outstanding_callback = true;
      }
      it = unhandled_geolocation_prompts_.erase(it);
    } else {
      ++it;
    }
  }

  if (removed_current_outstanding_callback) {
    GetClient()->AsArkWebClient()->GetPermissionRequest()->OnGeolocationHide();
    if (!unhandled_geolocation_prompts_.empty()) {
      OnGeolocationShow(unhandled_geolocation_prompts_.front().first);
    }
  }
}
#if BUILDFLAG(ARKWEB_SENSOR)
void CefBrowserHostBase::AskSensorsPermission(
    const CefString& origin,
    cef_permission_callback_t callback) {
  if (permission_request_handler_) {
    permission_request_handler_->SendSensorRequest(
        new AlloySensorAccessRequest(this, origin, std::move(callback)));
  } else {
    LOG(ERROR) << "AskSensorsPermission, handler is null.";
  }
}

void CefBrowserHostBase::AbortAskSensorsPermission(const CefString& origin) {
  if (permission_request_handler_) {
    permission_request_handler_->CancelRequest(
        origin, AlloyAccessRequest::Resources::SENSORS);
  } else {
    LOG(ERROR) << "AbortAskSensorsPermission, handler is null.";
  }
}
#endif  // #if BUILDFLAG(ARKWEB_SENSOR)

#if BUILDFLAG(ARKWEB_NOTIFICATION)
void CefBrowserHostBase::AskNotificationPermission(
    const CefString& origin,
    cef_permission_callback_t callback) {
  if (permission_request_handler_) {
    permission_request_handler_->SendRequest(new AlloyAccessRequest(
        origin, AlloyAccessRequest::Resources::NOTIFICATION,
        std::move(callback)));
  } else {
    LOG(ERROR) << "AskNotificationPermission, handler is null.";
  }
}
 
void CefBrowserHostBase::AbortAskNotificationPermission(
    const CefString& origin) {
  if (permission_request_handler_) {
    permission_request_handler_->CancelRequest(
        origin, AlloyAccessRequest::Resources::NOTIFICATION);
  } else {
    LOG(ERROR) << "AbortAskNotificationPermission, handler is null.";
  }
}

void CefBrowserHostBase::GetPermissionStatusAsync(
    const CefString& origin,
    cef_permission_status_query_callback_t callback) {
  CefPermissionQuery::GetPermissionStatusAsync(
      new AlloyAccessQuery(origin, AlloyAccessRequest::Resources::NOTIFICATION,
                           std::move(callback)));
}

#endif // #if BUILDFLAG(ARKWEB_NOTIFICATION)

void CefBrowserHostBase::AskMIDISysexPermission(
    const CefString& origin,
    cef_permission_callback_t callback) {
  permission_request_handler_->SendRequest(new AlloyAccessRequest(
      origin, AlloyAccessRequest::Resources::MIDI_SYSEX, std::move(callback)));
}

void CefBrowserHostBase::AbortAskMIDISysexPermission(const CefString& origin) {
  permission_request_handler_->CancelRequest(
      origin, AlloyAccessRequest::Resources::MIDI_SYSEX);
}

void CefBrowserHostBase::AskClipboardReadWritePermission(
    const CefString& origin,
    cef_permission_callback_t callback) {
  permission_request_handler_->SendRequest(new AlloyAccessRequest(
      origin, AlloyAccessRequest::Resources::CLIPBOARD_READ_WRITE,
      std::move(callback)));
}

void CefBrowserHostBase::AbortAskClipboardReadWritePermission(
    const CefString& origin) {
  permission_request_handler_->CancelRequest(
      origin, AlloyAccessRequest::Resources::CLIPBOARD_READ_WRITE);
}

void CefBrowserHostBase::AskClipboardSanitizedWritePermission(
    const CefString& origin,
    cef_permission_callback_t callback) {
  permission_request_handler_->SendRequest(new AlloyAccessRequest(
      origin, AlloyAccessRequest::Resources::CLIPBOARD_SANITIZED_WRITE,
      std::move(callback)));
}

void CefBrowserHostBase::AbortAskClipboardSanitizedWritePermission(
    const CefString& origin) {
  permission_request_handler_->CancelRequest(
      origin, AlloyAccessRequest::Resources::CLIPBOARD_SANITIZED_WRITE);
}

void CefBrowserHostBase::AskAudioCapturePermission(
    const CefString& origin,
    cef_permission_callback_t callback) {
  permission_request_handler_->SendRequest(new AlloyAccessRequest(
      origin, AlloyAccessRequest::Resources::AUDIO_CAPTURE,
      std::move(callback)));
}

void CefBrowserHostBase::AbortAskAudioCapturePermission(
    const CefString& origin) {
  permission_request_handler_->CancelRequest(
      origin, AlloyAccessRequest::Resources::AUDIO_CAPTURE);
}

void CefBrowserHostBase::AskVideoCapturePermission(
    const CefString& origin,
    cef_permission_callback_t callback) {
  permission_request_handler_->SendRequest(new AlloyAccessRequest(
      origin, AlloyAccessRequest::Resources::VIDEO_CAPTURE,
      std::move(callback)));
}

void CefBrowserHostBase::AbortAskVideoCapturePermission(
    const CefString& origin) {
  permission_request_handler_->CancelRequest(
      origin, AlloyAccessRequest::Resources::VIDEO_CAPTURE);
}
#endif  // BUILDFLAG(ARKWEB_PERMISSION)

#if BUILDFLAG(ARKWEB_BLANK_SCREEN_DETECTION)
void CefBrowserHostBase::SetBlankScreenDetectionConfig(
    bool enable,
    const std::vector<double> &detectionTiming,
    const std::vector<int32_t> &detectionMethods,
    int32_t contentfulNodesCountThreshold) {
  auto web_contents = GetWebContents();
  if (!web_contents) {
    LOG(ERROR) << "SetBlankScreenDetectionConfig GetWebContents null";
    return;
  }
  web_contents->SetBlankScreenDetectionConfig(
      enable, detectionTiming, detectionMethods, contentfulNodesCountThreshold);
}
#endif

#if BUILDFLAG(ARKWEB_DISATCH_BEFORE_UNLOAD)
bool CefBrowserHostBase::NeedToFireBeforeUnloadOrUnloadEvents() {
  if (!GetWebContents()) {
    return false;
  }
  return GetWebContents()->NeedToFireBeforeUnloadOrUnloadEvents();
}

void CefBrowserHostBase::DispatchBeforeUnload() {
  if (!GetWebContents()) {
    return;
  }
  GetWebContents()->DispatchBeforeUnload(false);
}
#endif  // ARKWEB_DISATCH_BEFORE_UNLOAD