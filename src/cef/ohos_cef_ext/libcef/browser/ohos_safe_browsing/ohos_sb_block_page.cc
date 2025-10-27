// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libcef/browser/ohos_safe_browsing/ohos_sb_block_page.h"

#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#include "base/i18n/rtl.h"
#include "base/strings/string_number_conversions.h"
#include "base/values.h"
#include "components/grit/components_resources.h"
#include "ohos_resources/components/string/grit/ohos_components_strings.h"
#include "ui/base/l10n/l10n_util.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/base/webui/jstemplate_builder.h"
#include "ui/base/webui/web_ui_util.h"

namespace ohos_safe_browsing {

SbBlockPage::SbBlockPage(
    content::WebContents* web_contents,
    const GURL& request_url,
    int policy,
    OHSBThreatType block_type,
    std::unique_ptr<SecurityInterstitialControllerClient> controller)
    : SecurityInterstitialPage(web_contents,
                               request_url,
                               std::move(controller)),
      block_type_(block_type),
      policy_(policy) {}

SbBlockPage::~SbBlockPage() = default;

std::string SbBlockPage::GetHTMLContents() {
  base::Value::Dict load_time_data;
  PopulateInterstitialStrings(load_time_data);
  webui::SetLoadTimeDataDefaults(controller()->GetApplicationLocale(),
                                 &load_time_data);
  std::string html;
  if (policy_ == OHSBPolicyType::POLICY_CHILD_MODE_PROHIBIT_ACCESS) {
    html = ui::ResourceBundle::GetSharedInstance().LoadDataResourceString(
        IDR_MINOR_CONTROL_HTML);
  } else {
    if (base::ohos::IsPcDevice() || base::ohos::IsTabletDevice()) {
      html = ui::ResourceBundle::GetSharedInstance().LoadDataResourceString(
          IDR_BLOCK_ERROR_OHOS_HTML_LARGE);
    } else {
      html = ui::ResourceBundle::GetSharedInstance().LoadDataResourceString(
          IDR_BLOCK_ERROR_OHOS_HTML_PHONE);
    }
  }

  webui::AppendWebUiCssTextDefaults(&html);
  return webui::GetI18nTemplateHtml(html, load_time_data);
}

void SbBlockPage::CommandReceived(const std::string& command) {
  if (command == "\"pageLoadComplete\"") {
    // content::WaitForRenderFrameReady sends this message when the page
    // load completes. Ignore it.
    return;
  }

  int cmd = 0;
  bool retval = base::StringToInt(command, &cmd);
  DCHECK(retval);
  HandleCommand(
      static_cast<security_interstitials::SecurityInterstitialCommand>(cmd));
}

void SbBlockPage::HandleCommand(
    security_interstitials::SecurityInterstitialCommand command) {
  // CMD_DONT_PROCEED
  if (command == security_interstitials::CMD_DONT_PROCEED) {
    controller()->GoBack();
  }

  // CMD_PROCEED
  if (command == security_interstitials::CMD_PROCEED) {
    controller()->Reload();
  }
}

void SbBlockPage::OnInterstitialClosing() {}

bool SbBlockPage::ShouldDisplayURL() const {
  return false;
}

void SbBlockPage::PopulateInterstitialStrings(
    base::Value::Dict& load_time_data) {
  load_time_data.Set("policy", std::to_string(policy_));
  load_time_data.Set("is_tablet_device",
                     std::to_string(base::ohos::IsTabletDevice()));
  load_time_data.Set("is_pc_device", std::to_string(base::ohos::IsPcDevice()));
  load_time_data.Set("title",
                     l10n_util::GetStringUTF8(IDS_OHOS_BLOCK_PAGE_TITLE));
  if (block_type_ == OHSBThreatType::THREAT_ILLEGAL) {
    load_time_data.Set("block_info_title", l10n_util::GetStringUTF8(
                                               IDS_OHOS_BLOCK_PAGE_INFO_TITLE));
  } else if (block_type_ == OHSBThreatType::THREAT_RISK) {
    load_time_data.Set(
        "block_info_title",
        l10n_util::GetStringUTF8(IDS_OHOS_BLOCK_PAGE_RISK_INFO_TITLE));
  } else if (block_type_ == OHSBThreatType::THREAT_FRAUD &&
             policy_ == OHSBPolicyType::POLICY_POPUP_AND_DANGER) {
    load_time_data.Set(
        "block_info_title",
        l10n_util::GetStringUTF8(IDS_OHOS_BLOCK_PAGE_RISK_INFO_TITLE));
  } else {
    load_time_data.Set(
        "block_info_title",
        l10n_util::GetStringUTF8(IDS_OHOS_BLOCK_PAGE_FRAUD_INFO_TITLE));
  }

  if (policy_ == OHSBPolicyType::POLICY_CHILD_MODE_PROHIBIT_ACCESS) {
    load_time_data.Set(
        "block_info_body",
        l10n_util::GetStringUTF8(IDS_OHOS_MINOR_PROTECTION_NOTE));
  } else if (block_type_ == OHSBThreatType::THREAT_ILLEGAL) {
    load_time_data.Set(
        "block_info_body",
        l10n_util::GetStringUTF8(IDS_OHOS_BLOCK_PAGE_ILLEGAL_INFO_BODY));
  } else if (block_type_ == OHSBThreatType::THREAT_RISK) {
    load_time_data.Set(
        "block_info_body",
        l10n_util::GetStringUTF8(IDS_OHOS_BLOCK_PAGE_RISK_INFO_BODY));
  } else if (block_type_ == OHSBThreatType::THREAT_FRAUD &&
             policy_ == OHSBPolicyType::POLICY_POPUP_AND_DANGER) {
    load_time_data.Set(
        "block_info_body",
        l10n_util::GetStringUTF8(IDS_OHOS_BLOCK_PAGE_RISK_INFO_BODY));
  } else {
    load_time_data.Set(
        "block_info_body",
        l10n_util::GetStringUTF8(IDS_OHOS_BLOCK_PAGE_FRAUD_INFO_BODY));
  }

  if (base::i18n::IsRTL()) {
    load_time_data.Set("page_direction", "rtl");
  } else {
    load_time_data.Set("page_direction", "ltr");
  }

  load_time_data.Set("dontproceed", l10n_util::GetStringUTF8(
                                        IDS_OHOS_BLOCK_PAGE_DONT_PROCEED));
  load_time_data.Set("proceed",
                     l10n_util::GetStringUTF8(IDS_OHOS_BLOCK_PAGE_PROCEED));
  load_time_data.Set("settings",
                     l10n_util::GetStringUTF8(IDS_OHOS_MINOR_PROTECTION_SETING_BUTTON));
}

void SbBlockPage::PopulateUrlTrustListInterstitialStrings(
    base::Value::Dict& load_time_data) {
  load_time_data.Set("title",
                     l10n_util::GetStringUTF8(IDS_OHOS_BLOCK_PAGE_TITLE));
  load_time_data.Set(
      "block_info_title",
      l10n_util::GetStringUTF8(IDS_OHOS_BLOCK_PAGE_URL_TRUST_LIST_TITLE));

  if (base::i18n::IsRTL()) {
    load_time_data.Set("page_direction", "rtl");
  } else {
    load_time_data.Set("page_direction", "ltr");
  }

  load_time_data.Set("dontproceed", l10n_util::GetStringUTF8(
                                        IDS_OHOS_BLOCK_PAGE_DONT_PROCEED));
}

std::string SbBlockPage::GetUrlTrustListErrorHTMLContents() {
  base::Value::Dict load_time_data;
  PopulateUrlTrustListInterstitialStrings(load_time_data);
  webui::SetLoadTimeDataDefaults(controller()->GetApplicationLocale(),
                                 &load_time_data);
  std::string html =
      ui::ResourceBundle::GetSharedInstance().LoadDataResourceString(
          URL_TRUST_LIST_ERROR_OHOS_HTML);
  webui::AppendWebUiCssTextDefaults(&html);
  return webui::GetI18nTemplateHtml(html, load_time_data);
}

}  // namespace ohos_safe_browsing
