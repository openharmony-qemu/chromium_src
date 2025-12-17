// Copyright (c) Huawei Technologies Co., Ltd. 2025. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_CAPTIVE_PORTAL_CONTENT_CAPTIVE_PORTAL_TAB_HELPER_OHOS_H_
#define COMPONENTS_CAPTIVE_PORTAL_CONTENT_CAPTIVE_PORTAL_TAB_HELPER_OHOS_H_

#include <memory>

#include "arkweb/ohos_adapter_ndk/interfaces/ohos_adapter_helper.h"
#include "components/captive_portal/content/captive_portal_service.h"
#include "components/captive_portal/content/captive_portal_tab_reloader.h"
#include "content/public/browser/web_contents_observer.h"
#include "content/public/browser/web_contents_user_data.h"

namespace content {
class NavigationHandle;
class WebContents;
}  // namespace content

namespace captive_portal {

class CaptivePortalTabHelperOhos
    : public content::WebContentsObserver,
      public content::WebContentsUserData<CaptivePortalTabHelperOhos> {
 public:
  class NetCommonEventListenerImpl : public OHOS::NWeb::NetCommonEventListener {
   public:
    NetCommonEventListenerImpl() = default;
    virtual ~NetCommonEventListenerImpl() = default;
    void OnConnectivityChanged(OHOS::NWeb::NetBearType bearType,
                               OHOS::NWeb::NetConnState connState) override;

    base::CallbackListSubscription RegisterCallback(
        const base::RepeatingCallback<void(bool)>& cb) {
      return callback_list_.Add(cb);
    }

   private:
    base::RepeatingCallbackList<void(bool)> callback_list_;
  };

  CaptivePortalTabHelperOhos(const CaptivePortalTabHelperOhos&) = delete;
  CaptivePortalTabHelperOhos& operator=(const CaptivePortalTabHelperOhos&) =
      delete;

  ~CaptivePortalTabHelperOhos() override;

  void DidStartNavigation(
      content::NavigationHandle* navigation_handle) override;
  void OnVisibilityChanged(content::Visibility visibility) override;

  void StartCheckForCaptivePortal(bool network_changed);
  void EnableArkWebHttpsUpgrade(bool enable);

  static bool IsInternetConnected() {
    return captive_portal_result_ ==
           CaptivePortalResult::RESULT_INTERNET_CONNECTED;
  }

 private:
  friend class content::WebContentsUserData<CaptivePortalTabHelperOhos>;
  CaptivePortalTabHelperOhos(
      content::WebContents* web_contents,
      CaptivePortalService* captive_portal_service,
      const CaptivePortalTabReloader::OpenLoginTabCallback&
          open_login_tab_callback);
  void Observe(const CaptivePortalService::Results& results);

  // Called by Observe in response to the corresponding event.
  void OnCaptivePortalResults(CaptivePortalResult previous_result,
                              CaptivePortalResult result);

  // Neither of these will ever be NULL.
  std::unique_ptr<CaptivePortalTabReloader> tab_reloader_;

  base::CallbackListSubscription subscription_;
  base::CallbackListSubscription net_conn_subscription_;
  std::atomic<bool> is_arkweb_https_upgrades_enable_{false};

  static std::shared_ptr<NetCommonEventListenerImpl>
      ohos_net_common_event_listener_;
  static std::unique_ptr<OHOS::NWeb::NetEventAdapter> ohos_net_event_adapter_;
  static std::atomic<CaptivePortalResult> captive_portal_result_;
  static std::atomic<bool> checking_captive_portal_due_network_changed_;

  WEB_CONTENTS_USER_DATA_KEY_DECL();

  base::WeakPtr<CaptivePortalTabHelperOhos> weak_this_;
  base::WeakPtrFactory<CaptivePortalTabHelperOhos> weak_ptr_factory_{this};
};

}  // namespace captive_portal

#endif  // COMPONENTS_CAPTIVE_PORTAL_CONTENT_CAPTIVE_PORTAL_TAB_HELPER_OHOS_H_
