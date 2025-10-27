// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libcef/browser/permission/alloy_permission_manager.h"

#include <memory>
#include <string>

#include "base/base_switches.h"
#include "base/command_line.h"
#include "base/functional/callback.h"
#include "base/logging.h"
#include "base/memory/raw_ptr.h"
#include "components/permissions/permission_util.h"
#include "content/public/browser/permission_controller.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/web_contents.h"
#include "content/public/common/content_switches.h"
#include "third_party/blink/public/common/permissions/permission_utils.h"

#if BUILDFLAG(ARKWEB_WEBRTC)
#include "libcef/browser/permission/alloy_access_request.h"
#endif // BUILDFLAG(ARKWEB_WEBRTC)
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
#include "services/network/public/cpp/is_potentially_trustworthy.h"
#endif // #if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)

using blink::PermissionType;
using blink::mojom::PermissionStatus;

using RequestPermissionsCallback =
    base::OnceCallback<void(const std::vector<PermissionStatus>&)>;

namespace {
constexpr int32_t APPLICATION_API_10 = 40000010;

int32_t GetApplicationApiVersion() {
  if (!base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kOhosAppApiVersion)) {
    LOG(ERROR) << "kOhosAppApiVersion not exist";
    return -1;
  }
  std::string apiVersion =
      base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
          switches::kOhosAppApiVersion);
  if (apiVersion.empty()) {
    return -1;
  }
  return std::stoi(apiVersion);
}

void PermissionRequestResponseCallbackWrapper(
    base::OnceCallback<void(PermissionStatus)> callback,
    const std::vector<PermissionStatus>& vector) {
  DCHECK_EQ(vector.size(), 1ul);
  std::move(callback).Run(vector.at(0));
}

}  // namespace

class AlloyPermissionManager::UnhandledRequest {
 public:
  UnhandledRequest(const std::vector<PermissionType> permissions,
                   content::RenderFrameHost* render_frame_host,
                   GURL requesting_origin,
                   RequestPermissionsCallback callback)
      : permissions_(permissions),
        render_frame_host_(render_frame_host),
        requesting_origin_(requesting_origin),
        callback_(std::move(callback)),
        results_(permissions.size(), PermissionStatus::DENIED),
        cancelled_(false) {
    for (size_t i = 0; i < permissions.size(); ++i) {
      permission_index_map_.insert(std::make_pair(permissions[i], i));
    }
  }

  ~UnhandledRequest() = default;

  bool HasPermissionType(PermissionType type) {
    return permission_index_map_.find(type) != permission_index_map_.end();
  }

  bool IsCompleted(PermissionType type) const {
    return resolved_permissions_.count(type) != 0;
  }

  bool IsCompleted() const {
    return results_.size() == resolved_permissions_.size();
  }

  void SetPermissionStatus(PermissionType type, PermissionStatus status) {
    auto result = permission_index_map_.find(type);
    if (result == permission_index_map_.end()) {
      NOTREACHED();
    }
    DCHECK(!IsCompleted());
    results_[result->second] = status;
    resolved_permissions_.insert(type);
  }

  PermissionStatus GetPermissionStatus(PermissionType type) {
    auto result = permission_index_map_.find(type);
    if (result == permission_index_map_.end()) {
      NOTREACHED();
    }
    return results_[result->second];
  }

  void Cancel() { cancelled_ = true; }

  bool IsCancelled() const { return cancelled_; }

  std::vector<PermissionType> permissions_;
  raw_ptr<content::RenderFrameHost> render_frame_host_;
  GURL requesting_origin_;
  RequestPermissionsCallback callback_;
  std::vector<PermissionStatus> results_;

 private:
  std::map<PermissionType, size_t> permission_index_map_;
  std::set<PermissionType> resolved_permissions_;
  bool cancelled_;
};

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
using PermissionStatusCallback =
    base::OnceCallback<void(blink::mojom::PermissionStatus)>;
 
class AlloyPermissionManager::UnhandledQuery {
 public:
  UnhandledQuery(PermissionType permission,
                 GURL requesting_origin,
                 PermissionStatusCallback callback)
      : permission_(permission),
        requesting_origin_(requesting_origin),
        callback_(std::move(callback)) {}
 
  PermissionType GetPermissionType() { return permission_; }
  GURL GetOrigin() { return requesting_origin_; }
 
 private:
  friend class AlloyPermissionManager;
  PermissionType permission_;
  GURL requesting_origin_;
  PermissionStatusCallback callback_;
};
#endif // #if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)

AlloyPermissionManager::AlloyPermissionManager() {}

AlloyPermissionManager::~AlloyPermissionManager() {
  AbortPermissionRequests();
}

void AlloyPermissionManager::RequestPermissions(
    content::RenderFrameHost* render_frame_host,
    const content::PermissionRequestDescription& request_description,
    base::OnceCallback<void(const std::vector<blink::mojom::PermissionStatus>&)>
        callback) {
  const std::vector<blink::PermissionType>& permissions =
      request_description.permissions;
  const GURL& requesting_origin = request_description.requesting_origin;
  if (permissions.empty()) {
    std::move(callback).Run(std::vector<PermissionStatus>());
    return;
  }

  auto pending_request = std::make_unique<UnhandledRequest>(
      permissions, render_frame_host, requesting_origin, std::move(callback));
  std::vector<bool> should_delegate_requests =
      std::vector<bool>(permissions.size(), true);
  for (size_t i = 0; i < permissions.size(); ++i) {
    for (UnhandledRequestsMap::Iterator<UnhandledRequest> it(
             &unhandled_requests_);
         !it.IsAtEnd(); it.Advance()) {
      if (!it.GetCurrentValue()->HasPermissionType(permissions[i]) ||
          it.GetCurrentValue()->requesting_origin_ != requesting_origin) {
        continue;
      }
      if (it.GetCurrentValue()->IsCompleted(permissions[i])) {
        pending_request->SetPermissionStatus(
            permissions[i],
            it.GetCurrentValue()->GetPermissionStatus(permissions[i]));
      }
      should_delegate_requests[i] = false;
      break;
    }
  }

  // Keep copy of pointer for performing further operations after ownership is
  // transferred to unhandled_requests_
  UnhandledRequest* pending_request_raw = pending_request.get();
  const int request_id = unhandled_requests_.Add(std::move(pending_request));

  CefRefPtr<CefBrowserHostBase> browser =
      CefBrowserHostBase::GetBrowserForHost(render_frame_host);

  for (size_t i = 0; i < permissions.size(); ++i) {
    if (should_delegate_requests[i]) {
      RequestPermissionByType(permissions[i], browser, pending_request_raw,
                              request_id);
    }
  }

  // If delegate resolve the permission synchronously, all requests could be
  // already resolved here.
  if (!unhandled_requests_.Lookup(request_id)) {
    return;
  }

  // If requests are resolved without calling delegate functions, e.g.
  // PermissionType::MIDI is permitted within the previous for-loop, all
  // requests could be already resolved, but still in the |unhandled_requests_|
  // without invoking the callback.
  if (pending_request_raw->IsCompleted()) {
    std::vector<PermissionStatus> results = pending_request_raw->results_;
    RequestPermissionsCallback completed_callback =
        std::move(pending_request_raw->callback_);
    unhandled_requests_.Remove(request_id);
    std::move(completed_callback).Run(results);
    return;
  }

  return;
}

void AlloyPermissionManager::RequestPermissionByType(
    const blink::PermissionType permission_type,
    CefRefPtr<CefBrowserHostBase> browser,
    UnhandledRequest* pending_request_raw,
    const int request_id) {
  switch (permission_type) {
    case PermissionType::GEOLOCATION:
      if (browser) {
        browser->AskGeolocationPermission(
            pending_request_raw->requesting_origin_.spec(),
            base::BindRepeating(
                &AlloyPermissionManager::OnRequestResponseCallBack,
                weak_ptr_factory_.GetWeakPtr(), request_id, permission_type));
      }
      break;
    case PermissionType::PROTECTED_MEDIA_IDENTIFIER:
      if (browser) {
        browser->AskProtectedMediaIdentifierPermission(
            pending_request_raw->requesting_origin_.spec(),
            base::BindRepeating(
                &AlloyPermissionManager::OnRequestResponseCallBack,
                weak_ptr_factory_.GetWeakPtr(), request_id, permission_type));
      }        
      break;
    case PermissionType::MIDI_SYSEX:
      if (browser) {
        browser->AskMIDISysexPermission(
            pending_request_raw->requesting_origin_.spec(),
            base::BindRepeating(
                &AlloyPermissionManager::OnRequestResponseCallBack,
                weak_ptr_factory_.GetWeakPtr(), request_id, permission_type));
      }        
      break;
    case PermissionType::CLIPBOARD_READ_WRITE:
      if (GetApplicationApiVersion() <= APPLICATION_API_10) {
        LOG(INFO) << "application version <= 10";
        pending_request_raw->SetPermissionStatus(permission_type,
                                                 PermissionStatus::GRANTED);
        break;
      }
      LOG(INFO)
          << "application version > 10, ask clipboard readwrite permission.";
      browser->AskClipboardReadWritePermission(
          pending_request_raw->requesting_origin_.spec(),
          base::BindRepeating(
              &AlloyPermissionManager::OnRequestResponseCallBack,
              weak_ptr_factory_.GetWeakPtr(), request_id, permission_type));
      break;
    case PermissionType::SENSORS:
#if BUILDFLAG(ARKWEB_SENSOR)
      if (browser) {
        browser->AskSensorsPermission(
            pending_request_raw->requesting_origin_.spec(),
            base::BindRepeating(
                &AlloyPermissionManager::OnRequestResponseCallBack,
                weak_ptr_factory_.GetWeakPtr(), request_id, permission_type));
      }
      break;
#endif  // BUILDFLAG(ARKWEB_SENSOR)
    case PermissionType::AUDIO_CAPTURE:
      if (browser) {
        browser->AskAudioCapturePermission(
            pending_request_raw->requesting_origin_.spec(),
            base::BindRepeating(
                &AlloyPermissionManager::OnRequestResponseCallBack,
                weak_ptr_factory_.GetWeakPtr(), request_id, permission_type));
      }        
      break;
    case PermissionType::VIDEO_CAPTURE:
      if (browser) {
        browser->AskVideoCapturePermission(
            pending_request_raw->requesting_origin_.spec(),
            base::BindRepeating(
                &AlloyPermissionManager::OnRequestResponseCallBack,
                weak_ptr_factory_.GetWeakPtr(), request_id, permission_type));
      }        
      break;
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
    case PermissionType::NOTIFICATIONS:
      if (browser) {
        if (!network::IsUrlPotentiallyTrustworthy(pending_request_raw->requesting_origin_)) {
          break;
        }
        browser->AskNotificationPermission(
          pending_request_raw->requesting_origin_.spec(),
            base::BindRepeating(
              &AlloyPermissionManager::OnRequestResponseCallBack,
              weak_ptr_factory_.GetWeakPtr(), request_id, permission_type));
      }
      break;
#else
    case PermissionType::NOTIFICATIONS:
#endif
    case PermissionType::DURABLE_STORAGE:
    case PermissionType::BACKGROUND_SYNC:
    case PermissionType::PAYMENT_HANDLER:
    case PermissionType::BACKGROUND_FETCH:
    case PermissionType::IDLE_DETECTION:
    case PermissionType::PERIODIC_BACKGROUND_SYNC:
    case PermissionType::NFC:
    case PermissionType::VR:
    case PermissionType::AR:
    case PermissionType::STORAGE_ACCESS_GRANT:
    case PermissionType::CAMERA_PAN_TILT_ZOOM:
    case PermissionType::WINDOW_MANAGEMENT:
    case PermissionType::LOCAL_FONTS:
    case PermissionType::DISPLAY_CAPTURE:
    case PermissionType::MIDI:
    case PermissionType::WAKE_LOCK_SYSTEM:
    default:
      LOG(ERROR) << "Invalid PermissionType.";
      pending_request_raw->SetPermissionStatus(permission_type,
                                               PermissionStatus::DENIED);
      break;
#ifdef OHOS_EX_PERMISSION
    case PermissionType::CLIPBOARD_SANITIZED_WRITE:
      if ((*base::CommandLine::ForCurrentProcess())
              .HasSwitch(switches::kEnableNwebExPermission)) {
        browser->AskClipboardSanitizedWritePermission(
            pending_request_raw->requesting_origin_.spec(),
            base::BindRepeating(
                &AlloyPermissionManager::OnRequestResponseCallBack,
                weak_ptr_factory_.GetWeakPtr(), request_id, permission_type));
      } else {
        pending_request_raw->SetPermissionStatus(permission_type,
                                                 PermissionStatus::GRANTED);
      }
      break;
#else
    case PermissionType::CLIPBOARD_SANITIZED_WRITE:
#endif
    case PermissionType::WAKE_LOCK_SCREEN:
      pending_request_raw->SetPermissionStatus(permission_type,
                                               PermissionStatus::GRANTED);
      break;
  }
}

void AlloyPermissionManager::OnRequestResponseCallBack(
    const base::WeakPtr<AlloyPermissionManager>& manager,
    int request_id,
    PermissionType permission,
    bool allowed) {
  LOG(INFO) << "OnRequestResponseCallBack permission:" << (int)permission
            << ", allowed:" << allowed;
  // All delegate functions should be cancelled when the manager runs
  // destructor. Therefore |manager| should be always valid here.
  DCHECK(manager);
  PermissionStatus status =
      allowed ? PermissionStatus::GRANTED : PermissionStatus::DENIED;
  UnhandledRequest* pending_request =
      manager->unhandled_requests_.Lookup(request_id);
  std::vector<int> complete_request_ids;
  std::vector<
      std::pair<RequestPermissionsCallback, std::vector<PermissionStatus>>>
      complete_request_pairs;
  for (UnhandledRequestsMap::Iterator<UnhandledRequest> it(
           &manager->unhandled_requests_);
       !it.IsAtEnd(); it.Advance()) {
    if (!it.GetCurrentValue()->HasPermissionType(permission) ||
        it.GetCurrentValue()->requesting_origin_ !=
            pending_request->requesting_origin_) {
      continue;
    }
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
    if (permission == PermissionType::NOTIFICATIONS) {
      manager->notification_permission_[pending_request->requesting_origin_] = (int32_t)status;
    }
    if (permission == PermissionType::GEOLOCATION) {
      manager->geolocation_permission_[pending_request->requesting_origin_] = (int32_t)status;
    }
#endif // #if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
    it.GetCurrentValue()->SetPermissionStatus(permission, status);
    if (it.GetCurrentValue()->IsCompleted()) {
      complete_request_ids.push_back(it.GetCurrentKey());
      if (!it.GetCurrentValue()->IsCancelled()) {
        complete_request_pairs.emplace_back(
            std::move(it.GetCurrentValue()->callback_),
            std::move(it.GetCurrentValue()->results_));
      }
    }
  }
  for (auto id : complete_request_ids) {
    manager->unhandled_requests_.Remove(id);
  }
  for (auto& pair : complete_request_pairs) {
    std::move(pair.first).Run(pair.second);
  }
}

PermissionStatus AlloyPermissionManager::GetPermissionStatus(
    PermissionType permission,
    const GURL& requesting_origin,
    const GURL& embedding_origin) {
  if (permission == PermissionType::CLIPBOARD_READ_WRITE ||
      permission == PermissionType::CLIPBOARD_SANITIZED_WRITE) {
    return PermissionStatus::GRANTED;
#if BUILDFLAG(ARKWEB_SENSOR)
  } else if (permission == PermissionType::SENSORS) {
    return PermissionStatus::ASK;
#endif  //BUILDFLAG(ARKWEB_SENSOR)
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  } else if (permission == PermissionType::NOTIFICATIONS) {
    if ((*base::CommandLine::ForCurrentProcess()).HasSwitch(
        switches::kEnableNwebExPermission)) {
      if (notification_permission_.count(requesting_origin)) {
        return (PermissionStatus)notification_permission_[requesting_origin];
      } else {
        return PermissionStatus::DENIED;
      }
    }
  } else if (permission == PermissionType::GEOLOCATION) {
    if ((*base::CommandLine::ForCurrentProcess()).HasSwitch(
        switches::kEnableNwebExPermission)) {
      if (geolocation_permission_.count(requesting_origin)) {
        return (PermissionStatus)geolocation_permission_[requesting_origin];
      } else {
        return PermissionStatus::DENIED;
      }
    }
#endif // #if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
#if BUILDFLAG(ARKWEB_WEBRTC)
  } else if (permission == PermissionType::AUDIO_CAPTURE) {
    if ((*base::CommandLine::ForCurrentProcess()).HasSwitch(
        switches::kEnableNwebExPermission)) {
      if (AlloyMediaAccessRequest::microphone_permission_.count(requesting_origin)) {
        return (PermissionStatus)AlloyMediaAccessRequest::microphone_permission_[requesting_origin];
      } else {
        LOG(WARNING) << __FUNCTION__ << ", requesting_origin is not in microphone permission map";
        return PermissionStatus::ASK;
      }
    }
  } else if (permission == PermissionType::VIDEO_CAPTURE) {
    if ((*base::CommandLine::ForCurrentProcess()).HasSwitch(
        switches::kEnableNwebExPermission)) {
      if (AlloyMediaAccessRequest::camera_permission_.count(requesting_origin)) {
        return (PermissionStatus)AlloyMediaAccessRequest::camera_permission_[requesting_origin];
      } else {
        LOG(WARNING) << __FUNCTION__ << ", requesting_origin is not in camera permission map";
        return PermissionStatus::ASK;
      }
    }
#endif // BUILDFLAG(ARKWEB_WEBRTC)
  }
  return PermissionStatus::DENIED;
}
PermissionStatus AlloyPermissionManager::GetPermissionStatusForWorker(
    PermissionType permission,
    content::RenderProcessHost* render_process_host,
    const GURL& worker_origin) {
  return GetPermissionStatus(permission, worker_origin, worker_origin);
}
PermissionStatus
AlloyPermissionManager::GetPermissionStatusForEmbeddedRequester(
    blink::PermissionType permission,
    content::RenderFrameHost* render_frame_host,
    const url::Origin& requesting_origin) {
  return GetPermissionStatus(
      permission, requesting_origin.GetURL(),
      permissions::PermissionUtil::GetLastCommittedOriginAsURL(
          render_frame_host->GetMainFrame()));
}

content::PermissionResult
AlloyPermissionManager::GetPermissionResultForOriginWithoutContext(
    blink::PermissionType permission,
    const url::Origin& requesting_origin,
    const url::Origin& embedding_origin) {
  blink::mojom::PermissionStatus status = GetPermissionStatus(
      permission, requesting_origin.GetURL(), requesting_origin.GetURL());

  return content::PermissionResult(
      status, content::PermissionStatusSource::UNSPECIFIED);
}

PermissionStatus AlloyPermissionManager::GetPermissionStatusForCurrentDocument(
    PermissionType permission,
    content::RenderFrameHost* render_frame_host,
    bool should_include_device_status) {
  return GetPermissionStatus(
      permission,
      permissions::PermissionUtil::GetLastCommittedOriginAsURL(
          render_frame_host),
      permissions::PermissionUtil::GetLastCommittedOriginAsURL(
          render_frame_host->GetMainFrame()));
}

void AlloyPermissionManager::ResetPermission(PermissionType permission,
                                             const GURL& requesting_origin,
                                             const GURL& embedding_origin) {}

void AlloyPermissionManager::AbortPermissionRequests() {
  std::vector<int> request_ids;
  for (UnhandledRequestsMap::Iterator<UnhandledRequest> it(
           &unhandled_requests_);
       !it.IsAtEnd(); it.Advance()) {
    request_ids.push_back(it.GetCurrentKey());
  }
  for (auto request_id : request_ids) {
    AbortPermissionRequest(request_id);
  }
  DCHECK(unhandled_requests_.IsEmpty());
}

void AlloyPermissionManager::AbortPermissionRequest(int request_id) {
  UnhandledRequest* pending_request = unhandled_requests_.Lookup(request_id);
  if (pending_request == nullptr || pending_request->IsCancelled()) {
    return;
  }
  pending_request->Cancel();

  const GURL& requesting_origin = pending_request->requesting_origin_;

  CefRefPtr<CefBrowserHostBase> browser = CefBrowserHostBase::GetBrowserForHost(
      pending_request->render_frame_host_);

  for (auto permission : pending_request->permissions_) {
    // If the permission was already resolved, we do not need to cancel it.
    if (pending_request->IsCompleted(permission)) {
      continue;
    }

    // If another pending_request waits for the same permission being resolved,
    // we should not cancel the delegate's request.
    bool should_not_cancel_ = false;
    for (UnhandledRequestsMap::Iterator<UnhandledRequest> it(
             &unhandled_requests_);
         !it.IsAtEnd(); it.Advance()) {
      if (it.GetCurrentValue() != pending_request &&
          it.GetCurrentValue()->HasPermissionType(permission) &&
          it.GetCurrentValue()->requesting_origin_ == requesting_origin &&
          !it.GetCurrentValue()->IsCompleted(permission)) {
        should_not_cancel_ = true;
        break;
      }
    }
    if (should_not_cancel_) {
      continue;
    }
    AbortPermissionRequestByType(permission, browser, requesting_origin);
    pending_request->SetPermissionStatus(permission, PermissionStatus::DENIED);
  }

  // If there are still active requests, we should not remove request_id here,
  // but just do not invoke a relevant callback when the request is resolved in
  // OnRequestResponseCallBack().
  if (pending_request->IsCompleted()) {
    unhandled_requests_.Remove(request_id);
  }
}

void AlloyPermissionManager::RequestPermissionsFromCurrentDocument(
    content::RenderFrameHost* render_frame_host,
    const content::PermissionRequestDescription& request_description,
    base::OnceCallback<void(const std::vector<blink::mojom::PermissionStatus>&)>
        callback) {
  RequestPermissions(render_frame_host, request_description,
                     std::move(callback));
}

GURL AlloyPermissionManager::LastCommittedOrigin(
    content::RenderFrameHost* render_frame_host) {
  return permissions::PermissionUtil::GetLastCommittedOriginAsURL(
      render_frame_host);
}

void AlloyPermissionManager::AbortPermissionRequestByType(
    const blink::PermissionType permission_type,
    CefRefPtr<CefBrowserHostBase> browser,
    const GURL& requesting_origin) {
  switch (permission_type) {
    case PermissionType::GEOLOCATION:
      if (browser) {
        browser->AbortAskGeolocationPermission(requesting_origin.spec());
      }
      break;
    case PermissionType::PROTECTED_MEDIA_IDENTIFIER:
      if (browser) {
        browser->AbortAskProtectedMediaIdentifierPermission(
            requesting_origin.spec());
      }
      break;
    case PermissionType::MIDI_SYSEX:
      if (browser) {
        browser->AbortAskMIDISysexPermission(requesting_origin.spec());
      }
      break;
    case PermissionType::CLIPBOARD_READ_WRITE:
      if (browser) {
        browser->AbortAskClipboardReadWritePermission(requesting_origin.spec());
      }
      break;
    case PermissionType::CLIPBOARD_SANITIZED_WRITE:
      if ((*base::CommandLine::ForCurrentProcess())
              .HasSwitch(switches::kEnableNwebEx) &&
          browser) {
        browser->AbortAskClipboardSanitizedWritePermission(
            requesting_origin.spec());
      } else {
        NOTREACHED() << "Invalid PermissionType.";
      }
      break;
    case PermissionType::AUDIO_CAPTURE:
      if (browser) {
        browser->AbortAskAudioCapturePermission(requesting_origin.spec());
      }
      break;
    case PermissionType::VIDEO_CAPTURE:
      if (browser) {
        browser->AbortAskVideoCapturePermission(requesting_origin.spec());
      }
      break;
    case PermissionType::SENSORS:
#if BUILDFLAG(ARKWEB_SENSOR)
      if (browser) {
        browser->AbortAskSensorsPermission(requesting_origin.spec());
      }
      break;
#endif  // BUILDFLAG(ARKWEB_SENSOR)
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
    case PermissionType::NOTIFICATIONS:
      if ((*base::CommandLine::ForCurrentProcess()).HasSwitch(
          switches::kEnableNwebEx) && browser) {
        browser->AbortAskNotificationPermission(
            requesting_origin.spec());
      } else {
        NOTREACHED() << "Invalid PermissionType.";
      }
      break;
#else
    case PermissionType::NOTIFICATIONS:
#endif // #if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
    case PermissionType::DURABLE_STORAGE:
    case PermissionType::BACKGROUND_SYNC:
    case PermissionType::PAYMENT_HANDLER:
    case PermissionType::BACKGROUND_FETCH:
    case PermissionType::IDLE_DETECTION:
    case PermissionType::PERIODIC_BACKGROUND_SYNC:
    case PermissionType::NFC:
    case PermissionType::VR:
    case PermissionType::AR:
    case PermissionType::STORAGE_ACCESS_GRANT:
    case PermissionType::CAMERA_PAN_TILT_ZOOM:
    case PermissionType::WINDOW_MANAGEMENT:
    case PermissionType::LOCAL_FONTS:
    case PermissionType::DISPLAY_CAPTURE:
    case PermissionType::MIDI:
    case PermissionType::WAKE_LOCK_SCREEN:
    case PermissionType::WAKE_LOCK_SYSTEM:
    default:
      NOTREACHED() << "Invalid PermissionType.";
  }
}

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
void AlloyPermissionManager::OnQueryResponseCallBack(
    const base::WeakPtr<AlloyPermissionManager>& manager,
    int query_id,
    blink::PermissionType permission,
    int32_t status) {
  if (!manager) {
    return;
  }
 
  UnhandledQuery* pending_query =
      manager->unhandled_querys_.Lookup(query_id);
  if (!pending_query) {
    LOG(ERROR) << "cannot find UnhandledQuery for query response";
    return;
  }
  if (!pending_query->callback_) {
    LOG(ERROR) << "query callback had been destroyed";
    return;
  }
 
  if (status < (int32_t)blink::mojom::PermissionStatus::kMinValue
      || status > (int32_t)blink::mojom::PermissionStatus::kMaxValue) {
    return;
  }
 
  manager->notification_permission_[pending_query->GetOrigin()] = status;
  std::move(pending_query->callback_).Run((blink::mojom::PermissionStatus)status);
  manager->unhandled_querys_.Remove(query_id);
}
 
void AlloyPermissionManager::GetPermissionStatusAsync(
    blink::PermissionType permission,
    const GURL& requesting_origin,
    base::OnceCallback<void(blink::mojom::PermissionStatus)> callback) {
  if (permission != PermissionType::NOTIFICATIONS) {
    std::move(callback).Run(PermissionStatus::DENIED);
    return;
  }
 
  auto pending_query = std::make_unique<UnhandledQuery>(
        permission, requesting_origin, std::move(callback));
  UnhandledQuery* pending_query_raw = pending_query.get();
  const int query_id =
      unhandled_querys_.Add(std::move(pending_query));
 
 CefBrowserHostBase::GetPermissionStatusAsync(
     pending_query_raw->requesting_origin_.spec(),
     base::BindRepeating(
         &OnQueryResponseCallBack, weak_ptr_factory_.GetWeakPtr(),
         query_id, permission));
}
#endif // #if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
