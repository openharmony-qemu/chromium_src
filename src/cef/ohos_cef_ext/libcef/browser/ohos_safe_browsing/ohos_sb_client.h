// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_BROWSER_SAFE_BROWSING_SB_CLIENT_H_
#define CEF_LIBCEF_BROWSER_SAFE_BROWSING_SB_CLIENT_H_

#include <memory>

#include "components/prefs/pref_service.h"
#include "content/browser/renderer_host/navigation_entry_impl.h"
#include "content/public/browser/navigation_handle.h"
#include "content/public/browser/web_contents_observer.h"
#include "content/public/browser/web_contents_user_data.h"
#include "libcef/browser/ohos_safe_browsing/ohos_sb_block_page.h"
#include "libcef/browser/ohos_safe_browsing/ohos_sb_threat_type.h"
#include "base/memory/raw_ptr.h"

namespace ohos_safe_browsing {

class SbClient : public content::WebContentsObserver {
 public:
  explicit SbClient(content::WebContents* web_contents,
                    PrefService* prefs,
                    bool incognito_mode);
  ~SbClient() override;

  // DISALLOW_COPY_AND_ASSIGN
  SbClient(const SbClient&) = delete;
  SbClient& operator=(const SbClient&) = delete;

  struct SbBlockingPageInfo {
    SbBlockingPageInfo();

    SbBlockingPageInfo(const GURL& url,
                       ohos_safe_browsing::OHSBThreatType block_type,
                       int hw_code,
                       int policy,
                       const GURL& redirect_url);

    ~SbBlockingPageInfo();

    GURL info_url = GURL("");
    int info_hw_code{0};
    OHSBThreatType info_block_type = OHSBThreatType::THREAT_DEFAULT;
    int info_policy{0};
    GURL info_redirect_url = GURL("");
  };

  class BlockingPageInfo
      : public content::WebContentsUserData<BlockingPageInfo> {
   public:
    ~BlockingPageInfo() override = default;

    BlockingPageInfo(const BlockingPageInfo&) = delete;
    BlockingPageInfo& operator=(const BlockingPageInfo&) = delete;

    static void SetBlockingPageInfo(content::WebContents* web_contents,
                                    const GURL& url,
                                    int policy,
                                    OHSBThreatType block_type,
                                    int hw_code,
                                    const GURL& redirect_url);

    static void RemoveBlockingPageInfo(content::WebContents* web_contents);
 
    bool IsUrlEmpty() const { return url_.is_empty(); }

   private:
    explicit BlockingPageInfo(content::WebContents* web_contents);
    friend class content::WebContentsUserData<BlockingPageInfo>;

    GURL url_ = GURL("");
    int policy_{0};
    OHSBThreatType block_type_ = OHSBThreatType::THREAT_DEFAULT;
    int hw_code_{0};
    GURL redirect_url_ = GURL("");

    WEB_CONTENTS_USER_DATA_KEY_DECL();
  };

  void HandleChildModePolicy(OHSBThreatType block_type);

  void ShowBlockingPage();

  bool IsBlockPageShowing();

  void SetEvilUrlPolicyAndHwCode(const GURL& url,
                                 int policy,
                                 OHSBThreatType block_type,
                                 int hw_code,
                                 const GURL& redirect_url);

  static bool InMaliciousAllowlist(const PrefService* prefs,
                                   const std::string& url,
                                   bool incognito_mode);

  bool InMaliciousAllowlist(const std::string& url) {
    return SbClient::InMaliciousAllowlist(prefs_, url, incognito_mode_);
  }

 private:
  // WebContentsObserver implementation
  void NavigationEntryCommitted(
      const content::LoadCommittedDetails& load_details) override;

  void DisplayBlockingPage(const GURL& url,
                           int policy,
                           OHSBThreatType block_type,
                           const std::string& app_locale);

  void NotifySafeBrowsingCheckResult(OHSBThreatType threat_type);

  raw_ptr<PrefService> prefs_;
  bool incognito_mode_{false};
};

}  // namespace ohos_safe_browsing

#endif  // CEF_LIBCEF_BROWSER_SAFE_BROWSING_SB_CLIENT_H_
