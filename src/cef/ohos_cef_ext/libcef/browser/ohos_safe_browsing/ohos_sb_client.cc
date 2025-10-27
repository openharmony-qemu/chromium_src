// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libcef/browser/ohos_safe_browsing/ohos_sb_client.h"

#include "base/containers/lru_cache.h"
#include "base/i18n/rtl.h"
#include "base/logging.h"
#include "base/values.h"
#include "components/security_interstitials/content/security_interstitial_tab_helper.h"
#include "content/public/browser/navigation_details.h"
#include "content/public/browser/navigation_entry.h"
#include "content/public/common/page_type.h"
#include "content/public/common/referrer.h"
#include "libcef/browser/alloy/alloy_browser_host_impl.h"
#include "libcef/browser/ohos_safe_browsing/ohos_sb_controller_client.h"
#include "libcef/browser/ohos_safe_browsing/ohos_sb_malicious_allowlist.h"
#include "libcef/browser/ohos_safe_browsing/ohos_sb_prefs.h"

namespace {
constexpr int kMaxCachedProfiles = 100;
base::LRUCache<GURL, ohos_safe_browsing::SbClient::SbBlockingPageInfo>
    blocking_page_info_cache(kMaxCachedProfiles);
}  // namespace

namespace ohos_safe_browsing {

SbClient::SbClient(content::WebContents* web_contents,
                   PrefService* prefs,
                   bool incognito_mode)
    : content::WebContentsObserver(web_contents),
      prefs_(prefs),
      incognito_mode_(incognito_mode) {}

SbClient::~SbClient() = default;

// static
bool SbClient::InMaliciousAllowlist(const PrefService* prefs,
                                    const std::string& url,
                                    bool incognito_mode) {
  if (!prefs) {
    return false;
  }

  const base::Value::List& list = prefs->GetList(
      incognito_mode ? kIncognitoMaliciousAllowList : kMaliciousAllowList);

  for (const base::Value& value : list) {
    if (value.GetString() == url) {
      return true;
    }
  }
  return false;
}

void SbClient::NavigationEntryCommitted(
    const content::LoadCommittedDetails& load_details) {
  if (!web_contents() || web_contents()->IsBeingDestroyed()) {
    return;
  }

  if (!load_details.is_main_frame) {
    return;
  }

  if (load_details.is_same_document) {
    return;
  }

  BlockingPageInfo* blocking_page_info =
      BlockingPageInfo::FromWebContents(web_contents());

  if (!blocking_page_info || blocking_page_info->IsUrlEmpty()) {
    return;
  }
  LOG(INFO) << "SafeBrowsing NavigationEntryCommitted ShowBlockingPage";
  ShowBlockingPage();
  BlockingPageInfo::RemoveBlockingPageInfo(web_contents());
}

void SbClient::HandleChildModePolicy(OHSBThreatType block_type) {
  if (block_type == OHSBThreatType::THREAT_WARNING) {
    NotifySafeBrowsingCheckResult(OHSBThreatType::THREAT_RISK);
    return;
  }
  NotifySafeBrowsingCheckResult(block_type);
}

void SbClient::ShowBlockingPage() {
  if (!web_contents() || web_contents()->IsBeingDestroyed()) {
    return;
  }

  if (web_contents()->IsSafeBrowsingDetectionDisabled()) {
    LOG(INFO) << "SafeBrowsingDetection is disabled";
    return;
  }

  content::NavigationEntry* visible_entry =
      web_contents()->GetController().GetVisibleEntry();

  if (!visible_entry) {
    LOG(WARNING) << "SafeBrowsing no visible entry, skip.";
    return;
  }

  GURL virtual_url = visible_entry->GetVirtualURL();
  std::vector<GURL> redirect_chain = visible_entry->GetRedirectChain();
  redirect_chain.push_back(virtual_url);
  std::reverse(redirect_chain.begin(), redirect_chain.end());

  bool is_page_type_normal =
      (visible_entry->GetPageType() == content::PAGE_TYPE_NORMAL);
  if (!is_page_type_normal) {
    LOG(WARNING) << "SafeBrowsing warning: page type is abnormal";
    return;
  }
  std::string lang = base::i18n::GetConfiguredLocale();

  for (auto& url : redirect_chain) {
    const auto& blocking_cached = blocking_page_info_cache.Peek(url);
    if (blocking_cached != blocking_page_info_cache.end()) {
      SbBlockingPageInfo blocking_page_info = blocking_cached->second;
      bool is_display_block_page = false;
      int policy = blocking_page_info.info_policy;
      OHSBThreatType block_type = blocking_page_info.info_block_type;

      LOG(INFO) << "SafeBrowsing ShowBlockingPage, policy: " << policy
                << ", block_type: " << block_type;

      if (policy == OHSBPolicyType::POLICY_CHILD_MODE_PROHIBIT_ACCESS) {
        is_display_block_page = true;
        HandleChildModePolicy(block_type);
      }

      if (block_type == OHSBThreatType::THREAT_WARNING &&
          (policy == OHSBPolicyType::POLICY_DANGER_LABEL ||
           policy == OHSBPolicyType::POLICY_HALF_POPUP)) {
        NotifySafeBrowsingCheckResult(block_type);
        return;
      }

      if ((policy == OHSBPolicyType::POLICY_POPUP_AND_DANGER ||
           policy == OHSBPolicyType::POLICY_FORBIDDEN_PROHIBIT_ACCESS) &&
          block_type != OHSBThreatType::THREAT_WARNING) {
        is_display_block_page = true;
        NotifySafeBrowsingCheckResult(block_type);
      }

      if (policy != OHSBPolicyType::POLICY_CHILD_MODE_PROHIBIT_ACCESS &&
          MaliciousAllowlist::GetInstance().IsInAllowlist(
              url.has_host() ? url.host() : url.spec(), incognito_mode_)) {
        LOG(WARNING) << "SafeBrowsing in malicious allowlist.";
        break;
      }

      if (is_display_block_page) {
        DisplayBlockingPage(url, policy, block_type, lang);
        return;
      }
    }
  }
}

void SbClient::SetEvilUrlPolicyAndHwCode(const GURL& url,
                                         int policy,
                                         OHSBThreatType block_type,
                                         int hw_code,
                                         const GURL& redirect_url) {
  if (!web_contents() || web_contents()->IsBeingDestroyed()) {
    return;
  }
  BlockingPageInfo::SetBlockingPageInfo(web_contents(), url, policy, block_type,
                                        hw_code, redirect_url);
  content::NavigationEntry* pending_entry =
      web_contents()->GetController().GetPendingEntry();
  if (!pending_entry && !web_contents()->IsWaitingForResponse()) {
    LOG(INFO) << "SafeBrowsing SetEvilUrlPolicyAndHwCode ShowBlockingPage";
    ShowBlockingPage();
    BlockingPageInfo::RemoveBlockingPageInfo(web_contents());
  }
}

SbClient::BlockingPageInfo::BlockingPageInfo(content::WebContents* web_contents)
    : content::WebContentsUserData<SbClient::BlockingPageInfo>(*web_contents) {}

SbClient::SbBlockingPageInfo::SbBlockingPageInfo() {}

SbClient::SbBlockingPageInfo::SbBlockingPageInfo(
    const GURL& url,
    ohos_safe_browsing::OHSBThreatType block_type,
    int hw_code,
    int policy,
    const GURL& redirect_url)
    : info_url(url),
      info_hw_code(hw_code),
      info_block_type(block_type),
      info_policy(policy),
      info_redirect_url(redirect_url) {}

SbClient::SbBlockingPageInfo::~SbBlockingPageInfo() {}

// static
void SbClient::BlockingPageInfo::SetBlockingPageInfo(
    content::WebContents* web_contents,
    const GURL& url,
    int policy,
    OHSBThreatType block_type,
    int hw_code,
    const GURL& redirect_url) {
  BlockingPageInfo::CreateForWebContents(web_contents);
  BlockingPageInfo* blocking_page_info =
      BlockingPageInfo::FromWebContents(web_contents);
  blocking_page_info->url_ = url;
  blocking_page_info->policy_ = policy;
  blocking_page_info->block_type_ = block_type;
  blocking_page_info->hw_code_ = hw_code;
  blocking_page_info->redirect_url_ = redirect_url;
  if (policy == OHSBPolicyType::POLICY_POPUP_AND_DANGER ||
      policy == OHSBPolicyType::POLICY_FORBIDDEN_PROHIBIT_ACCESS ||
      policy == OHSBPolicyType::POLICY_HALF_POPUP ||
      policy == OHSBPolicyType::POLICY_CHILD_MODE_PROHIBIT_ACCESS ||
      policy == OHSBPolicyType::POLICY_DANGER_LABEL) {
    SbBlockingPageInfo info(url, block_type, hw_code, policy, redirect_url);
    blocking_page_info_cache.Put(url, info);
  }
}

void SbClient::BlockingPageInfo::RemoveBlockingPageInfo(
    content::WebContents* web_contents)
{
  if (!web_contents) {
    return;
  }

  web_contents->RemoveUserData(BlockingPageInfo::UserDataKey());

  // The URL policy is changed, but the cache policy remains unchanged.
  // So ShowBlockingPage is complete, and the cache is cleared.
  blocking_page_info_cache.Clear();
}

void SbClient::DisplayBlockingPage(const GURL& url,
                                   int policy,
                                   OHSBThreatType block_type,
                                   const std::string& app_locale) {
  if (!web_contents() || web_contents()->IsBeingDestroyed()) {
    return;
  }
  LOG(INFO) << "SafeBrowsing " << __func__ << ", policy: " << policy
            << ", type: " << block_type;
  auto controller = std::make_unique<SbControllerClient>(
      web_contents(), prefs_, url, app_locale, incognito_mode_);
  std::unique_ptr<SbBlockPage> blocking_page = std::make_unique<SbBlockPage>(
      web_contents(), url, policy, block_type, std::move(controller));
  base::WeakPtr<content::NavigationHandle> error_page_navigation_handle =
      web_contents()->GetController().LoadPostCommitErrorPage(
          web_contents()->GetPrimaryMainFrame(), url,
          blocking_page->GetHTMLContents());
  if (error_page_navigation_handle.get()) {
    security_interstitials::SecurityInterstitialTabHelper::
        AssociateBlockingPage(error_page_navigation_handle.get(),
                              std::move(blocking_page));
  }
}

bool SbClient::IsBlockPageShowing() {
  security_interstitials::SecurityInterstitialTabHelper*
      security_interstitial_tab_helper = security_interstitials::
          SecurityInterstitialTabHelper::FromWebContents(web_contents());
  return security_interstitial_tab_helper &&
         security_interstitial_tab_helper->IsDisplayingInterstitial();
}

void SbClient::NotifySafeBrowsingCheckResult(OHSBThreatType threat_type) {
  if (!web_contents()) {
    LOG(WARNING) << "NotifySafeBrowsingCheckResult: web_contents is null";
    return;
  }
  CefRefPtr<AlloyBrowserHostImpl> browser =
      AlloyBrowserHostImpl::GetBrowserForContents(web_contents());
  if (!browser.get()) {
    LOG(WARNING) << "NotifySafeBrowsingCheckResult: browser is null";
    return;
  }
  CefRefPtr<CefClient> client = browser->GetClient();
  if (!client.get()) {
    LOG(WARNING) << "NotifySafeBrowsingCheckResult: client is null";
    return;
  }
  CefRefPtr<CefLoadHandler> load_handler = client->GetLoadHandler();
  if (!load_handler.get()) {
    LOG(WARNING) << "NotifySafeBrowsingCheckResult: load_handler is null";
    return;
  }
  int type = static_cast<int>(threat_type);
  load_handler->AsArkWebLoadHandlerExt()->OnSafeBrowsingCheckResult(type);
}

WEB_CONTENTS_USER_DATA_KEY_IMPL(SbClient::BlockingPageInfo);

}  // namespace ohos_safe_browsing
