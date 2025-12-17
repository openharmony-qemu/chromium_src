// Copyright (c) Huawei Technologies Co., Ltd. 2025. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "arkweb/chromium_ext/components/captive_portal/content/captive_portal_tab_helper_ohos.h"

#include "base/functional/bind.h"
#include "base/memory/ptr_util.h"
#include "components/captive_portal/content/captive_portal_login_detector.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/navigation_handle.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents.h"

namespace captive_portal {

namespace {
const int CHECK_CAPTIVE_PORTAL_DUE_NETWORK_CHANGE_DELAY_TIME = 2;
}

std::unique_ptr<OHOS::NWeb::NetEventAdapter>
    CaptivePortalTabHelperOhos::ohos_net_event_adapter_{nullptr};
std::shared_ptr<CaptivePortalTabHelperOhos::NetCommonEventListenerImpl>
    CaptivePortalTabHelperOhos::ohos_net_common_event_listener_{nullptr};
std::atomic<CaptivePortalResult>
    CaptivePortalTabHelperOhos::captive_portal_result_{
        CaptivePortalResult::RESULT_COUNT};
std::atomic<bool>
    CaptivePortalTabHelperOhos::checking_captive_portal_due_network_changed_{
        false};

void CaptivePortalTabHelperOhos::NetCommonEventListenerImpl::
    OnConnectivityChanged(OHOS::NWeb::NetBearType bearType,
                          OHOS::NWeb::NetConnState connState) {
  DCHECK(!content::BrowserThread::CurrentlyOn(content::BrowserThread::UI));

  if (connState != OHOS::NWeb::NetConnState::NET_CONN_STATE_CONNECTED &&
      connState != OHOS::NWeb::NetConnState::NET_CONN_STATE_DISCONNECTED) {
    return;
  }
  LOG(INFO) << __func__
            << " for captive_portal bearType:" << static_cast<int>(bearType)
            << " connState:" << static_cast<int>(connState);
  CaptivePortalTabHelperOhos::captive_portal_result_ =
      CaptivePortalResult::RESULT_COUNT;
  callback_list_.Notify(true);
}

CaptivePortalTabHelperOhos::CaptivePortalTabHelperOhos(
    content::WebContents* web_contents,
    CaptivePortalService* captive_portal_service,
    const CaptivePortalTabReloader::OpenLoginTabCallback&
        open_login_tab_callback)
    : content::WebContentsObserver(web_contents),
      content::WebContentsUserData<CaptivePortalTabHelperOhos>(*web_contents),
      tab_reloader_(new CaptivePortalTabReloader(captive_portal_service,
                                                 web_contents,
                                                 open_login_tab_callback)),
      subscription_(captive_portal_service->RegisterCallback(
          base::BindRepeating(&CaptivePortalTabHelperOhos::Observe,
                              base::Unretained(this)))) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  weak_this_ = weak_ptr_factory_.GetWeakPtr();

  if (!ohos_net_event_adapter_) {
    ohos_net_event_adapter_ =
        OHOS::NWeb::OhosAdapterHelper::GetInstance().CreateNetEventAdapter();
    ohos_net_common_event_listener_ =
        std::make_shared<NetCommonEventListenerImpl>();
    ohos_net_event_adapter_->RegisterCommonEventListener(
        ohos_net_common_event_listener_);
  }
  net_conn_subscription_ =
      ohos_net_common_event_listener_->RegisterCallback(base::BindRepeating(
          &CaptivePortalTabHelperOhos::StartCheckForCaptivePortal, weak_this_));
}

CaptivePortalTabHelperOhos::~CaptivePortalTabHelperOhos() {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
}

void CaptivePortalTabHelperOhos::DidStartNavigation(
    content::NavigationHandle* navigation_handle) {
  if (!navigation_handle->IsInPrimaryMainFrame()) {
    return;
  }
  StartCheckForCaptivePortal(false);
}

void CaptivePortalTabHelperOhos::OnVisibilityChanged(
    content::Visibility visibility) {
  StartCheckForCaptivePortal(false);
}

void CaptivePortalTabHelperOhos::Observe(
    const CaptivePortalService::Results& results) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);
  OnCaptivePortalResults(results.previous_result, results.result);
}

void CaptivePortalTabHelperOhos::OnCaptivePortalResults(
    CaptivePortalResult previous_result,
    CaptivePortalResult result) {
  if (captive_portal_result_ != result) {
    LOG(INFO) << __func__ << " for captive_portal result:"
              << CaptivePortalResultToString(captive_portal_result_) << " -> "
              << CaptivePortalResultToString(result);
    captive_portal_result_ = result;
  }
  tab_reloader_->OnCaptivePortalResults(previous_result, result);
}

void CaptivePortalTabHelperOhos::StartCheckForCaptivePortal(
    bool network_changed) {
  if (!content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
    if (!network_changed || checking_captive_portal_due_network_changed_ ||
        !is_arkweb_https_upgrades_enable_) {
      return;
    }
    content::GetUIThreadTaskRunner({})->PostDelayedTask(
        FROM_HERE,
        base::BindOnce(&CaptivePortalTabHelperOhos::StartCheckForCaptivePortal,
                       weak_this_, network_changed),
        base::Seconds(CHECK_CAPTIVE_PORTAL_DUE_NETWORK_CHANGE_DELAY_TIME));
    checking_captive_portal_due_network_changed_ = true;
    return;
  }

  if (network_changed) {
    checking_captive_portal_due_network_changed_ = false;
  }
  if (!is_arkweb_https_upgrades_enable_ ||
      captive_portal_result_ ==
          CaptivePortalResult::RESULT_INTERNET_CONNECTED) {
    return;
  }

  if (tab_reloader_->state() ==
      CaptivePortalTabReloader::State::STATE_MAYBE_BROKEN_BY_PORTAL) {
    return;
  }
  LOG(INFO) << __func__
            << " for captive_portal network_changed:" << network_changed;
  tab_reloader_->StartCheckForCaptivePortal();
}

void CaptivePortalTabHelperOhos::EnableArkWebHttpsUpgrade(bool enable) {
  if (enable == is_arkweb_https_upgrades_enable_) {
    return;
  }
  is_arkweb_https_upgrades_enable_ = enable;

  StartCheckForCaptivePortal(false);
}

WEB_CONTENTS_USER_DATA_KEY_IMPL(CaptivePortalTabHelperOhos);

}  // namespace captive_portal
