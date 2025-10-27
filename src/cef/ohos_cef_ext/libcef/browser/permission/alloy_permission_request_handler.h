// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_ALLOY_PERMISSION_REQUEST_HANDLER_H_
#define CEF_LIBCEF_BROWSER_ALLOY_PERMISSION_REQUEST_HANDLER_H_

#include <stdint.h>

#include <map>
#include <memory>
#include <vector>

#include "arkweb/build/features/features.h"
#include "content/public/browser/web_contents_observer.h"
#include "include/cef_permission_request.h"

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
#include "include/cef_permission_status_query.h"
#endif // #if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)

// This class is used to send the permission requests, or cancel ongoing
// requests.
// It is owned by WebContents and has 1x1 mapping to WebContents. All methods
// are running on UI thread.
class AlloyPermissionRequestHandler : public content::WebContentsObserver {
 public:
  AlloyPermissionRequestHandler(CefRefPtr<CefPermissionRequest> client,
                                content::WebContents* web_contents);
  ~AlloyPermissionRequestHandler() override;

  AlloyPermissionRequestHandler(const AlloyPermissionRequestHandler&) = delete;
  AlloyPermissionRequestHandler& operator=(
      const AlloyPermissionRequestHandler&) = delete;

  // Send the given |request| to CefPermissionRequest.
  void SendRequest(CefRefPtr<CefAccessRequest> request);

  // Cancel the ongoing request initiated by |origin| for accessing |resources|.
  void CancelRequest(const CefString& origin, int resources);

  // Allow |origin| to access the |resources|.
  void PreauthorizePermission(const CefString& origin, int resources);

#if BUILDFLAG(ARKWEB_SENSOR)
  void SendSensorRequest(CefRefPtr<CefAccessRequest> request);
  void SetSensorPermission(const CefString& origin, bool allowed);
#endif  // BUILDFLAG(ARKWEB_SENSOR)

  // #if BUILDFLAG(ARKWEB_WEBRTC)
  void SendScreenCaptureRequest(
      CefRefPtr<CefScreenCaptureAccessRequest> request);
  // #endif // BUILDFLAG(ARKWEB_WEBRTC)

  // WebContentsObserver
  void NavigationEntryCommitted(
      const content::LoadCommittedDetails& load_details) override;

 private:
  typedef std::vector<CefRefPtr<CefAccessRequest>>::iterator RequestIterator;

  // Return the request initiated by |origin| for accessing |resources|.
  RequestIterator FindRequest(const CefString& origin, int resources);

  // Cancel the given request.
  void CancelRequestInternal(RequestIterator i);

  void CancelAllRequests();

  // Remove the invalid requests from requests_.
  void PruneRequests();

  // Return true if |origin| were preauthorized to access |resources|.
  bool Preauthorized(const CefString& origin, int resources);

#if BUILDFLAG(ARKWEB_SENSOR)
  bool GetSensorPermission(const CefString& origin, bool& is_exist);
#endif  // #BUILDFLAG(ARKWEB_SENSOR)

  CefRefPtr<CefPermissionRequest> client_;

  // A list of ongoing requests.
  std::vector<CefRefPtr<CefAccessRequest>> requests_;

  std::map<CefString, int> preauthorized_permission_;

#if BUILDFLAG(ARKWEB_SENSOR)
  std::map<CefString, bool> sensor_permission_;
#endif  // BUILDFLAG(ARKWEB_SENSOR)

  // The unique id of the active NavigationEntry of the WebContents that we were
  // opened for. Used to help expire on requests.
  int contents_unique_id_;
};

#endif  // CEF_LIBCEF_BROWSER_ALLOY_PERMISSION_REQUEST_HANDLER_H_
