/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "libcef/browser/autofill/oh_autofill_client.h"

#include <utility>

#include "base/check_op.h"
#include "base/notimplemented.h"
#include "base/notreached.h"
#include "components/autofill/core/browser/payments/legal_message_line.h"
#include "components/autofill/core/browser/suggestions/suggestion.h"
#include "components/autofill/core/browser/webdata/autofill_webdata_service.h"
#include "chrome/browser/autofill/personal_data_manager_factory.h"
#include "components/prefs/pref_registry_simple.h"
#include "components/prefs/pref_service.h"
#include "components/prefs/pref_service_factory.h"
#include "components/user_prefs/user_prefs.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/navigation_controller.h"
#include "content/public/browser/navigation_entry.h"
#include "content/public/browser/ssl_status.h"
#include "content/public/browser/storage_partition.h"
#include "content/public/browser/web_contents.h"
#include "ui/gfx/geometry/rect_f.h"
#include "arkweb/build/features/features.h"

#include "chrome/browser/browser_process.h"
#include "libcef/browser/autofill/oh_autofill_manager.h"

using content::WebContents;

namespace autofill {

void OhAutofillClient::CreateForWebContents(content::WebContents* contents) {
  DCHECK(contents);
  if (!ContentAutofillClient::FromWebContents(contents)) {
    contents->SetUserData(UserDataKey(), base::WrapUnique(new OhAutofillClient(
                                             contents)));
  }
}

OhAutofillClient::OhAutofillClient(content::WebContents* web_contents)
    : autofill::ContentAutofillClient(web_contents) {}

OhAutofillClient::~OhAutofillClient() {
  HideAutofillSuggestions(autofill::SuggestionHidingReason::kTabGone);
}

base::WeakPtr<autofill::AutofillClient> OhAutofillClient::GetWeakPtr() {
  return weak_ptr_factory_.GetWeakPtr();
}

void OhAutofillClient::FillData(CefRefPtr<CefValue> data) {
#if BUILDFLAG(ARKWEB_AUTOFILL)
  if (!data) {
    LOG(ERROR) << "data is null";
    return;
  }
  std::string json_str = data->GetString();
  content::RenderFrameHost* rfh = GetWebContents().GetPrimaryMainFrame();
  if (!rfh) {
    LOG(ERROR) << "rfh is nullptr";
    return;
  }
  autofill::ContentAutofillDriver* driver =
      autofill::ContentAutofillDriver::GetForRenderFrameHost(rfh);
  if (!driver) {
    LOG(ERROR) << "driver is nullptr";
    return;
  }
  auto mgr = static_cast<OhAutofillManager*>(&driver->GetAutofillManager());
  if (mgr) {
    mgr->FillData(json_str);
  }
#endif
}

bool OhAutofillClient::OnAutofillEvent(const std::string& json_str) {
  if (callback_) {
    CefRefPtr<CefValue> data = CefValue::Create();
    data->SetStdString(json_str);
    return callback_->OnMessageWithBoolResult(data);
  }
  return false;
}

void OhAutofillClient::SetSaveFormData(bool enabled) {
  save_form_data_ = enabled;
}

bool OhAutofillClient::GetSaveFormData() const {
  return save_form_data_;
}

bool OhAutofillClient::IsOffTheRecord() const {
  auto* mutable_this = const_cast<OhAutofillClient*>(this);
  return mutable_this->GetWebContents().GetBrowserContext()->IsOffTheRecord();
}

scoped_refptr<network::SharedURLLoaderFactory>
OhAutofillClient::GetURLLoaderFactory() {
  return GetWebContents()
      .GetBrowserContext()
      ->GetDefaultStoragePartition()
      ->GetURLLoaderFactoryForBrowserProcess();
}

PersonalDataManager& OhAutofillClient::GetPersonalDataManager() {
  NOTREACHED();
}

AutocompleteHistoryManager*
OhAutofillClient::GetAutocompleteHistoryManager() {
  if (!autocomplete_history_manager_) {
    autocomplete_history_manager_ =
        std::make_unique<AutocompleteHistoryManager>();
  }
  return autocomplete_history_manager_.get();
}

PrefService* OhAutofillClient::GetPrefs() {
  return const_cast<PrefService*>(std::as_const(*this).GetPrefs());
}

const PrefService* OhAutofillClient::GetPrefs() const {
  if (g_browser_process) {
    return g_browser_process->local_state();
  }

  return nullptr;
}

syncer::SyncService* OhAutofillClient::GetSyncService() {
  return nullptr;
}

signin::IdentityManager* OhAutofillClient::GetIdentityManager() {
  return nullptr;
}

const signin::IdentityManager* OhAutofillClient::GetIdentityManager()
    const {
  return nullptr;
}

autofill::FormDataImporter* OhAutofillClient::GetFormDataImporter() {
  return nullptr;
}

autofill::StrikeDatabase* OhAutofillClient::GetStrikeDatabase() {
  return nullptr;
}

ukm::UkmRecorder* OhAutofillClient::GetUkmRecorder() {
  return nullptr;
}


autofill::AddressNormalizer* OhAutofillClient::GetAddressNormalizer() {
  return nullptr;
}

const GURL& OhAutofillClient::GetLastCommittedPrimaryMainFrameURL() const {
  return GetWebContents().GetPrimaryMainFrame()->GetLastCommittedURL();
}

url::Origin OhAutofillClient::GetLastCommittedPrimaryMainFrameOrigin() const {
  return GetWebContents().GetPrimaryMainFrame()->GetLastCommittedOrigin();
}

security_state::SecurityLevel
OhAutofillClient::GetSecurityLevelForUmaHistograms() {
  // The metrics are not recorded for Android webview, so return the count value
  // which will not be recorded.
  return security_state::SecurityLevel::SECURITY_LEVEL_COUNT;
}

const translate::LanguageState* OhAutofillClient::GetLanguageState() {
  return nullptr;
}

translate::TranslateDriver* OhAutofillClient::GetTranslateDriver() {
  return nullptr;
}

void OhAutofillClient::ShowAutofillSettings(autofill::SuggestionType suggestion_type) {
  NOTIMPLEMENTED();
}

void OhAutofillClient::ConfirmSaveAddressProfile(
    const autofill::AutofillProfile& profile,
    const autofill::AutofillProfile* original_profile,
    bool is_migration_to_account,
    AddressProfileSavePromptCallback callback) {
  NOTIMPLEMENTED();
}


autofill::AutofillClient::SuggestionUiSessionId OhAutofillClient::ShowAutofillSuggestions(
    const autofill::AutofillClient::PopupOpenArgs& open_args,
    base::WeakPtr<autofill::AutofillSuggestionDelegate> delegate) {
  NOTIMPLEMENTED();
  return SuggestionUiSessionId();
}

#if BUILDFLAG(ARKWEB_DATALIST)
void OhAutofillClient::ShowAutofillPopup(
    const autofill::AutofillClient::PopupOpenArgs& open_args,
    SelectedCallback callback) {
  suggestions_ = open_args.suggestions;

  // Convert element_bounds to be in screen space.
  gfx::Rect client_area = GetWebContents().GetContainerBounds();
  gfx::RectF element_bounds_in_screen_space =
      open_args.element_bounds + client_area.OffsetFromOrigin();

  LOG(INFO) << "showAutofillPopup client_area" << client_area.ToString()
            << ", element_bounds_in_screen_space"
            << element_bounds_in_screen_space.ToString();
  selected_callback_ = std::move(callback);
  GetWebContents().ShowAutofillPopup(
      element_bounds_in_screen_space,
      open_args.text_direction == base::i18n::RIGHT_TO_LEFT,
      open_args.suggestions, false);
}

void OhAutofillClient::HideAutofillPopup() {
  GetWebContents().HideAutofillPopup();
}

void OhAutofillClient::SuggestionSelected(int position) {
  if (position < 0 || position > static_cast<int>(suggestions_.size()) - 1) {
    LOG(WARNING) << "parameter is invalid, position: " << position;
    return;
  }
  std::move(selected_callback_).Run(suggestions_[position].main_text.value);
}
#endif

void OhAutofillClient::UpdateAutofillDataListValues(
    base::span<const autofill::SelectOption> datalist) {
  // Leaving as an empty method since updating autofill popup window
  // dynamically does not seem to be a useful feature when delegating to Android
  // APIs.
}


void OhAutofillClient::HideAutofillSuggestions(autofill::SuggestionHidingReason reason) {
  delegate_.reset();
}

bool OhAutofillClient::IsAutocompleteEnabled() const {
  return GetSaveFormData();
}

bool OhAutofillClient::IsPasswordManagerEnabled() const {
  return false;
}

void OhAutofillClient::DidFillForm(
    autofill::AutofillTriggerSource trigger_source,
    bool is_refill) {}

bool OhAutofillClient::IsContextSecure() const {
  content::SSLStatus ssl_status;
  content::NavigationEntry* navigation_entry =
      GetWebContents().GetController().GetLastCommittedEntry();
  if (!navigation_entry) {
    return false;
  }

  ssl_status = navigation_entry->GetSSL();
  // Note: As of crbug.com/701018, Chrome relies on SecurityStateTabHelper to
  // determine whether the page is secure, but WebView has no equivalent class.

  return navigation_entry->GetURL().SchemeIsCryptographic() &&
         ssl_status.certificate &&
         !net::IsCertStatusError(ssl_status.cert_status) &&
         !(ssl_status.content_status &
           content::SSLStatus::RAN_INSECURE_CONTENT);
}

autofill::FormInteractionsFlowId
OhAutofillClient::GetCurrentFormInteractionsFlowId() {
  // Currently not in use here. See `ChromeAutofillClient` for a proper
  // implementation.
  return {};
}

std::unique_ptr<autofill::AutofillManager> OhAutofillClient::CreateManager(
    base::PassKey<autofill::ContentAutofillDriver> pass_key,
    autofill::ContentAutofillDriver& driver) {
  return base::WrapUnique(new autofill::OhAutofillManager(&driver));
}

content::WebContents& OhAutofillClient::GetWebContents() const {
  // While a const_cast is not ideal. The Autofill API uses const in various
  // spots and the content public API doesn't have const accessors. So the const
  // cast is the lesser of two evils.
  return const_cast<content::WebContents&>(
      ContentAutofillClient::GetWebContents());
}

const std::string& OhAutofillClient::GetAppLocale() const {
  const static std::string locale = "en-US";
  return locale;
}

autofill::VotesUploader& OhAutofillClient::GetVotesUploader() {
  NOTIMPLEMENTED();
  LOG(FATAL) << "GetVotesUploader not implemented - cannot construct without proper parameters";
  // VotesUploader requires complex initialization, so this method needs proper implementation
  __builtin_unreachable();
}

autofill::AutofillCrowdsourcingManager& OhAutofillClient::GetCrowdsourcingManager() {
  NOTIMPLEMENTED();
  LOG(FATAL) << "GetCrowdsourcingManager not implemented - requires AutofillClient and Channel parameters";
  // AutofillCrowdsourcingManager requires AutofillClient* and version_info::Channel parameters
  __builtin_unreachable();
}

autofill::ValuablesDataManager* OhAutofillClient::GetValuablesDataManager() {
  NOTIMPLEMENTED();
  return nullptr;
}

autofill::EntityDataManager* OhAutofillClient::GetEntityDataManager() {
  NOTIMPLEMENTED();
  return nullptr;
}

autofill::SingleFieldFillRouter& OhAutofillClient::GetSingleFieldFillRouter() {
  NOTIMPLEMENTED();
  LOG(FATAL) << "GetSingleFieldFillRouter not implemented - requires multiple manager parameters";
  // SingleFieldFillRouter requires AutocompleteHistoryManager*, IbanManager*, and MerchantPromoCodeManager*
  __builtin_unreachable();
}

credential_management::ContentCredentialManager* OhAutofillClient::GetContentCredentialManager() {
  NOTIMPLEMENTED();
  LOG(WARNING) << "GetContentCredentialManager not implemented";
  return nullptr;
}

bool OhAutofillClient::IsAutofillEnabled() const {
  return true;
}

bool OhAutofillClient::IsAutofillProfileEnabled() const {
  return true;
}

bool OhAutofillClient::IsAutofillPaymentMethodsEnabled() const {
  return false;
}

autofill_metrics::FormInteractionsUkmLogger& OhAutofillClient::GetFormInteractionsUkmLogger() {
  NOTIMPLEMENTED();
  LOG(FATAL) << "GetFormInteractionsUkmLogger not implemented - requires proper initialization";
  // FormInteractionsUkmLogger requires complex initialization with proper UKM service
  __builtin_unreachable();
}

}  // namespace autofill
