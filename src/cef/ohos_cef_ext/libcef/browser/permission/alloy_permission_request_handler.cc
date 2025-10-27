// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libcef/browser/permission/alloy_permission_request_handler.h"

#include "arkweb/build/features/features.h"
#include "content/public/browser/navigation_details.h"
#include "content/public/browser/navigation_entry.h"
#include "content/public/browser/web_contents.h"
#include "include/cef_permission_request.h"

namespace {
int GetLastCommittedEntryID(content::WebContents* web_contents) {
  if (!web_contents) {
    return 0;
  }

  content::NavigationEntry* entry =
      web_contents->GetController().GetLastCommittedEntry();
  return entry ? entry->GetUniqueID() : 0;
}
}  // namespace

AlloyPermissionRequestHandler::AlloyPermissionRequestHandler(
    CefRefPtr<CefPermissionRequest> client,
    content::WebContents* web_contents)
    : content::WebContentsObserver(web_contents),
      client_(client),
      contents_unique_id_(GetLastCommittedEntryID(web_contents)) {}

AlloyPermissionRequestHandler::~AlloyPermissionRequestHandler() {
  CancelAllRequests();
}

void AlloyPermissionRequestHandler::SendRequest(
    CefRefPtr<CefAccessRequest> request) {
  if (Preauthorized(request->Origin(), request->ResourceAcessId())) {
    request->ReportRequestResult(true);
    return;
  }

  requests_.push_back(request);
  client_->OnPermissionRequest(request);
  PruneRequests();
}

void AlloyPermissionRequestHandler::CancelRequest(const CefString& origin,
                                                  int resources) {
  // The request list might have multiple requests with same origin and
  // resources.
  RequestIterator i = FindRequest(origin, resources);
  while (i != requests_.end()) {
    CancelRequestInternal(i);
    requests_.erase(i);
    i = FindRequest(origin, resources);
  }
}

void AlloyPermissionRequestHandler::CancelRequestInternal(RequestIterator i) {
  CefRefPtr<CefAccessRequest> request = i->get();
  if (request) {
    client_->OnPermissionRequestCanceled(request);
  }
}

void AlloyPermissionRequestHandler::CancelAllRequests() {
  for (RequestIterator i = requests_.begin(); i != requests_.end(); ++i) {
    CancelRequestInternal(i);
  }
}

AlloyPermissionRequestHandler::RequestIterator
AlloyPermissionRequestHandler::FindRequest(const CefString& origin,
                                           int resources) {
  RequestIterator i;
  for (i = requests_.begin(); i != requests_.end(); ++i) {
    if (i->get() && i->get()->Origin() == origin &&
        i->get()->ResourceAcessId() == resources) {
      break;
    }
  }
  return i;
}

void AlloyPermissionRequestHandler::PreauthorizePermission(
    const CefString& origin,
    int resources) {
  if (!resources) {
    return;
  }

  if (origin.empty()) {
    LOG(ERROR) << "The origin of preauthorization is empty, ignore it.";
    return;
  }

  preauthorized_permission_[origin] |= resources;
}

void AlloyPermissionRequestHandler::NavigationEntryCommitted(
    const content::LoadCommittedDetails& details) {
  const ui::PageTransition transition = details.entry->GetTransitionType();
  if (details.is_navigation_to_different_page() ||
      ui::PageTransitionCoreTypeIs(transition, ui::PAGE_TRANSITION_RELOAD) ||
      contents_unique_id_ != details.entry->GetUniqueID()) {
    CancelAllRequests();
    contents_unique_id_ = details.entry->GetUniqueID();
  }
}

void AlloyPermissionRequestHandler::PruneRequests() {
  for (RequestIterator i = requests_.begin(); i != requests_.end();) {
    if (!i->get()) {
      i = requests_.erase(i);
    } else {
      ++i;
    }
  }
}

bool AlloyPermissionRequestHandler::Preauthorized(const CefString& origin,
                                                  int resources) {
  std::map<CefString, int>::iterator i = preauthorized_permission_.find(origin);

  return i != preauthorized_permission_.end() && i->second == resources;
}

#if BUILDFLAG(ARKWEB_WEBRTC)
void AlloyPermissionRequestHandler::SendScreenCaptureRequest(
    CefRefPtr<CefScreenCaptureAccessRequest> request) {
  client_->OnScreenCaptureRequest(request);
}
#endif  // BUILDFLAG(ARKWEB_WEBRTC)

#if BUILDFLAG(ARKWEB_SENSOR)
void AlloyPermissionRequestHandler::SendSensorRequest(
    CefRefPtr<CefAccessRequest> request) {
  bool is_exist = false;
  bool allowed = GetSensorPermission(request->Origin(), is_exist);
  if (is_exist) {
    LOG(INFO) << "SendSensorRequest Exist, allowed: " << allowed;
    request->ReportRequestResult(allowed);
    return;
  }

  requests_.push_back(request);
  client_->OnPermissionRequest(request);
  PruneRequests();
}

void AlloyPermissionRequestHandler::SetSensorPermission(const CefString& origin,
                                                        bool allowed) {
  if (origin.empty()) {
    LOG(ERROR) << "The origin of sensor permission is empty, ignore it.";
    return;
  }
  LOG(INFO) << "SetSensorPermission, allowed: " << allowed;
  sensor_permission_[origin] = allowed;
}

bool AlloyPermissionRequestHandler::GetSensorPermission(const CefString& origin,
                                                        bool& is_exist) {
  is_exist = false;
  std::map<CefString, bool>::iterator i = sensor_permission_.find(origin);
  if (i != sensor_permission_.end()) {
    is_exist = true;
    return i->second;
  } else {
    return false;
  }
}
#endif  // BUILDFLAG(ARKWEB_SENSOR)
