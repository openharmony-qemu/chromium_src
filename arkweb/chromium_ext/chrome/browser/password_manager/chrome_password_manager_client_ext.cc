/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "arkweb/chromium_ext/chrome/browser/password_manager/chrome_password_manager_client_ext.h"
#include "arkweb/build/features/features.h"

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#include "base/json/json_reader.h"
#include "base/json/json_writer.h"
#include "base/strings/utf_string_conversions.h"
#include "components/password_manager/content/browser/content_password_manager_driver.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/render_widget_host_view.h"
#include "crypto/sha2.h"
#include "libcef/browser/alloy/alloy_browser_host_impl.h"
#include "libcef/browser/autofill/oh_autofill_client.h"
#include "libcef/browser/autofill/oh_autofill_manager.h"
#include "libcef/browser/browser_host_base.h"
#include "ohos_cef_ext/include/arkweb_render_handler_ext.h"
#endif

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
const std::string SOURCE = "source";
const std::string SOURCE_LOGIN = "login";
const std::string EVENT = "event";
const std::string EVENT_SAVE = "save";
const std::string EVENT_FILL = "fill";
const std::string EVENT_UPDATE = "update";

const std::string KEY_FOCUS = "focus";
const std::string KEY_RECT_X = "x";
const std::string KEY_RECT_Y = "y";
const std::string KEY_RECT_W = "width";
const std::string KEY_RECT_H = "height";
const std::string KEY_PLACEHOLDER = "placeholder";
const std::string KEY_VALUE = "value";
const std::string KEY_SELECTABLE_USER_NAMES = "selectableUsernames";

const std::string KEY_PAGE_URL = "pageUrl";
const std::string KEY_IS_USER_SELECTED = "isUserSelected";
const std::string KEY_IS_OTHER_ACCOUNT = "isOtherAccount";

const std::string KEY_USERNAME = "username";
const std::string KEY_PASSWORD = "password";

const std::string HASH_SALT = "OHOS@PASSWORD@AUTOFILL";

using autofill::mojom::OhosPasswordFormAutofillState;
using password_manager::ContentPasswordManagerDriverFactory;
using password_manager::PasswordManagerDriver;

ChromePasswordManagerClientExt::ChromePasswordManagerClientExt(
    content::WebContents* web_contents)
    : ChromePasswordManagerClient(web_contents) {
  last_request_fill_username_.type = autofill::mojom::OhosInputElementType::kUsernameType;
  last_request_fill_username_.is_focused = false;
  last_request_fill_password_.type = autofill::mojom::OhosInputElementType::kPasswordType;
  last_request_fill_password_.is_focused = false;
}

ChromePasswordManagerClientExt::~ChromePasswordManagerClientExt() = default;

std::optional<std::string> ChromePasswordManagerClientExt::PasswordFormToJsonForRequest(
    const std::string& event,
    const GURL& page_url,
    const autofill::InputFillRequestData& username_data,
    const autofill::InputFillRequestData& password_data,
    AutofillIMFInfo* imf_info) {
  if (!web_contents()) {
    LOG(ERROR) << "web_contents is nullptr";
    return std::nullopt;
  }
  content::RenderFrameHost* rfh = web_contents()->GetFocusedFrame();
  if (!rfh || !rfh->IsActive()) {
    LOG(ERROR) << "rfh is nullptr or not active";
    return std::nullopt;
  }
  auto browser = CefBrowserHostBase::GetBrowserForHost(rfh);
  if (!browser || !browser->GetHost()) {
    LOG(ERROR) << "browser is nullptr";
    return std::nullopt;
  }

  float ratio = browser->GetHost()->GetVirtualPixelRatio();
  int32_t viewport_height = browser->GetHost()->GetShrinkViewportHeight();
  gfx::Rect offset =
      content::WebContents::FromRenderFrameHost(rfh)->GetContainerBounds();

  base::Value::List view_data_list;
  view_data_list.Append(base::Value::Dict().Set(EVENT, event));
  view_data_list.Append(base::Value::Dict().Set(SOURCE, SOURCE_LOGIN));
  view_data_list.Append(base::Value::Dict().Set(KEY_PAGE_URL, page_url.spec()));
  if (imf_info) {
    view_data_list.Append(
        base::Value::Dict().Set(KEY_IS_USER_SELECTED, imf_info->is_username));
    view_data_list.Append(base::Value::Dict().Set(KEY_IS_OTHER_ACCOUNT,
                                                  imf_info->is_other_account));
  }

  password_manager::ContentPasswordManagerDriver* content_driver =
      password_manager::ContentPasswordManagerDriver::GetForRenderFrameHost(
          web_contents()->GetFocusedFrame());

  std::unordered_map<std::string, autofill::InputFillRequestData> fillItem = {
      {KEY_USERNAME, username_data}, {KEY_PASSWORD, password_data}};
  for (auto item : fillItem) {
    base::Value::List list;
    list.Append(
        base::Value::Dict().Set(KEY_FOCUS, item.second.is_focused ? 1 : 0));
    if (content_driver != nullptr) {
      item.second.bounds = TransformToRootCoordinates(
          content_driver->render_frame_host(), item.second.bounds);
    }
    list.Append(base::Value::Dict().Set(
        KEY_RECT_X,
        static_cast<int32_t>((item.second.bounds.x() + offset.x()) * ratio)));
    list.Append(base::Value::Dict().Set(
        KEY_RECT_Y,
        static_cast<int32_t>(
            (item.second.bounds.y() + offset.y() + viewport_height) * ratio)));
    list.Append(base::Value::Dict().Set(
        KEY_RECT_W, static_cast<int32_t>(item.second.bounds.width() * ratio)));
    list.Append(base::Value::Dict().Set(
        KEY_RECT_H, static_cast<int32_t>(item.second.bounds.height() * ratio)));
    if (base::ohos::IsPcDevice() && item.second.is_focused) {
      LOG(INFO) << "autofill request toJson, rect:"
                << base::WriteJson(list).value_or("null");
    }
    list.Append(base::Value::Dict().Set(KEY_VALUE,
                                        base::UTF16ToUTF8(item.second.value)));

    auto dict = base::Value::Dict().Set(item.first, std::move(list));
    view_data_list.Append(std::move(dict));
  }

  return base::WriteJson(view_data_list);
}

std::optional<std::string> ChromePasswordManagerClientExt::PasswordFormToJsonForSave(
    const password_manager::PasswordForm& form) {
  base::Value::List view_data_list;
  view_data_list.Append(base::Value::Dict().Set(EVENT, EVENT_SAVE));
  view_data_list.Append(base::Value::Dict().Set(SOURCE, SOURCE_LOGIN));
  view_data_list.Append(base::Value::Dict().Set(
      KEY_PAGE_URL, url::Origin::Create(form.url).GetURL().spec()));

  base::Value::List list_username;
  list_username.Append(
    base::Value::Dict().Set(KEY_VALUE, base::UTF16ToUTF8(form.username_value)));

  base::Value::List list_metadata;
  for (const auto& alt_username : form.all_alternative_usernames) {
    list_metadata.Append(base::UTF16ToUTF8(alt_username.value));
  }
  list_username.Append(
      base::Value::Dict().Set(KEY_SELECTABLE_USER_NAMES, std::move(list_metadata)));

  view_data_list.Append(
      base::Value::Dict().Set(KEY_USERNAME, std::move(list_username)));

  view_data_list.Append(
      base::Value::Dict().Set(KEY_PASSWORD, 
          base::Value::List().Append(
              base::Value::Dict().Set(KEY_VALUE, base::UTF16ToUTF8(form.password_value))
          )
      )
  );

  return base::WriteJson(view_data_list);
}

void ChromePasswordManagerClientExt::ProcessAutofillCancel(
    const std::string& fillContent) {
  // If the soft keyboard does not need to be restored, the fill cancel event
  // will not be processed.
  if (!is_need_restore_keyboard_) {
    LOG(INFO) << "don't need to handle the fill cancle event";
    return;
  }
  is_need_restore_keyboard_ = false;

  LOG(INFO) << "autofill handle fill cancel event";
  if (suppressor_) {
    suppressor_->Unsuppress();
  }

  if (!web_contents()) {
    LOG(ERROR) << "web_contents is nullptr";
    return;
  }
  content::RenderFrameHost* rfh = web_contents()->GetFocusedFrame();
  if (!rfh || !rfh->IsActive()) {
    LOG(ERROR) << "rfh is nullptr or not active";
    return;
  }
  auto browser = CefBrowserHostBase::GetBrowserForHost(rfh);
  if (browser && browser->GetClient() &&
      browser->GetClient()->GetRenderHandler()) {
    browser->GetClient()->GetRenderHandler()->SetFillContent(fillContent);
  }
  auto* pwd_manager_driver =
      password_manager::ContentPasswordManagerDriver::GetForRenderFrameHost(
          rfh);
  if (!pwd_manager_driver) {
    LOG(ERROR) << "pwd_manager_driver is nullptr";
    return;
  }

  // Will retry to pull up inputmethod soft keyboard.
  pwd_manager_driver->AutofillSurfaceClosed(true);
}

void ChromePasswordManagerClientExt::AutoFillWithIMFEvent(
    bool is_username,
    bool is_other_account,
    bool is_new_password,
    const std::string& content) {
  LOG(INFO) << "autofill with IMF event, flag=" << is_username
            << is_other_account << is_new_password;
  auto* autofill_client =
      autofill::OhAutofillClient::FromWebContents(web_contents());
  if (!autofill_client) {
    LOG(ERROR) << " autofill_client is nullptr";
    return;
  }

  auto username = last_request_fill_username_;
  auto password = last_request_fill_password_;
  if (is_username) {
    username.value = base::UTF8ToUTF16(content);
  }
  AutofillIMFInfo imf_info = {
      is_username,
      is_other_account,
      is_new_password,
  };
  auto json_str = PasswordFormToJsonForRequest(EVENT_FILL, form_to_request_url_,
                                               username, password, &imf_info);
  if (json_str.has_value()) {
    LOG(INFO) << "call autofill for request from IMF";
    bool result = autofill_client->OnAutofillEvent(json_str.value());
    if (!result) {
      LOG(ERROR) << "failed to call autofill for request";
      return;
    }
    is_need_restore_keyboard_ = true;
  }
}

void ChromePasswordManagerClientExt::FillData(const std::string& page_url,
                                           const std::string& username,
                                           const std::string& password,
                                           bool is_other_account) {
  if (suppressor_) {
    suppressor_->Unsuppress();
  }
  is_need_restore_keyboard_ = false;
  auto username_id = last_request_fill_username_.field_renderer_id;
  auto password_id = last_request_fill_password_.field_renderer_id;
  auto digest = is_other_account
                    ? std::string()
                    : crypto::SHA256HashString(username + HASH_SALT + password);
  if (username_id) {
    auto_filled_forms_username_[*username_id] = digest;
  }
  if (password_id) {
    auto_filled_forms_password_[*password_id] = digest;
  }

  FillAccountSuggestion(GURL(page_url), base::UTF8ToUTF16(username),
                        base::UTF8ToUTF16(password));
}

void ChromePasswordManagerClientExt::SuppressKeyboard() {
  if (!web_contents()) {
    LOG(ERROR) << "web_contents is nullptr";
    return;
  }
  content::RenderFrameHost* rfh = web_contents()->GetFocusedFrame();
  if (!rfh || !rfh->IsActive()) {
    LOG(ERROR) << "rfh is nullptr or not active";
    return;
  }
  autofill::ContentAutofillDriver* autofill_driver =
      autofill::ContentAutofillDriver::GetForRenderFrameHost(rfh);
  if (!autofill_driver) {
    LOG(ERROR) << "autofill_driver is nullptr";
    return;
  }

  if (!suppressor_) {
    auto* autofill_client =
        autofill::ContentAutofillClient::FromWebContents(web_contents());
    if (!autofill_client) {
      LOG(ERROR) << "autofill_client is nullptr";
      return;
    }
    suppressor_ = std::make_unique<KeyboardSuppressorOhos>(autofill_client,
                                                           base::Seconds(1));
  }
  suppressor_->Suppress(autofill_driver);
  is_need_restore_keyboard_ = true;
}

bool ChromePasswordManagerClientExt::IsLoginInfoConsistentWithFilled(
    const password_manager::PasswordForm& info) {
  auto username_id = info.username_element_renderer_id;
  auto password_id = info.password_element_renderer_id;
  AutofilledMap::iterator it;
  AutofilledMap* auto_filled_forms = nullptr;
  LOG(INFO) << "login autosave, username renderer_id:" << username_id
            << ", password renderer_id:" << password_id;
  if (password_id) {
    auto_filled_forms = &auto_filled_forms_password_;
    it = auto_filled_forms->find(*password_id);
  } else if (username_id) {
    auto_filled_forms = &auto_filled_forms_username_;
    it = auto_filled_forms->find(*username_id);
  }

  if (auto_filled_forms && it != auto_filled_forms->end() &&
      !it->second.empty()) {
    std::string login_digest = crypto::SHA256HashString(
        base::UTF16ToUTF8(info.username_value) + HASH_SALT +
        base::UTF16ToUTF8(info.password_value));
    if (it->second == login_digest) {
      auto_filled_forms->erase(it);
      return true;
    }
  }
  return false;
}

void ChromePasswordManagerClientExt::UpdateLastRequestFilledItems(
    const autofill::InputFillRequestData& username_data,
    const autofill::InputFillRequestData& password_data) {
  last_request_fill_username_ = username_data;
  last_request_fill_password_ = password_data;
}

void ChromePasswordManagerClientExt::NotifyAutofillPopupShow(bool is_show) {
  if (!web_contents()) {
    LOG(ERROR) << "web_contents is nullptr";
    return;
  }
  content::RenderFrameHost* rfh = web_contents()->GetFocusedFrame();
  if (!rfh || !rfh->IsActive()) {
    LOG(ERROR) << "rfh is nullptr or not active";
    return;
  }
  auto* driver = autofill::ContentAutofillDriver::GetForRenderFrameHost(rfh);
  if (!driver) {
    LOG(ERROR) << "autofill_driver is nullptr";
    return;
  }
  auto* autofill_manager =
      static_cast<autofill::OhAutofillManager*>(&driver->GetAutofillManager());
  if (!autofill_manager) {
    LOG(ERROR) << "autofill_manager is nullptr";
    return;
  }
  autofill_manager->SetPasswordPopupShow(is_show);
}

void ChromePasswordManagerClientExt::FillAccountSuggestion(
    const GURL& page_url,
    const std::u16string& username,
    const std::u16string& password) {
  if (!web_contents()) {
    LOG(ERROR) << "[Autofill] web_contents is nullptr";
    return;
  }
  content::RenderFrameHost* rfh = web_contents()->GetFocusedFrame();
  if (!rfh || !rfh->IsActive()) {
    LOG(ERROR) << "[Autofill] rfh is nullptr or not active";
    return;
  }
  password_manager::ContentPasswordManagerDriver* driver =
      password_manager::ContentPasswordManagerDriver::GetForRenderFrameHost(
          rfh);
  if (!driver) {
    return;
  }

  LOG(INFO) << "[Autofill] Try to fill account suggestion.";
  password_manager::ContentPasswordManagerDriverExt* contentPasswordManagerDriverExt =
      driver->AsContentPasswordManagerDriverExt();
  if (contentPasswordManagerDriverExt == nullptr) {
    LOG(ERROR) << "[Autofill] contentPasswordManagerDriverExt is nullptr";
    return;
  }
  contentPasswordManagerDriverExt->FillAccountSuggestion(page_url, username, password);
}

void ChromePasswordManagerClientExt::OnRequestAutofill(
    PasswordManagerDriver* driver,
    const GURL& page_url,
    autofill::FormRendererId form_id,
    const autofill::mojom::OhosPasswordFormAutofillState state,
    const autofill::InputFillRequestData& username_data,
    const autofill::InputFillRequestData& password_data) {
  LOG(INFO)
      << "[Autofill] On request autofill"
      << ", state=" << static_cast<int>(state)
      << ", username.field_renderer_id=" << username_data.field_renderer_id
      << ", username.bounds=" << username_data.bounds.ToString()
      << ", username.is_focus=" << username_data.is_focused
      << ", username.type=" << username_data.type
      << ", username.autocomplete_attr=" << username_data.autocomplete_attr
      << ", password.field_renderer_id=" << password_data.field_renderer_id
      << ", password.bounds=" << password_data.bounds.ToString()
      << ", password.is_focus=" << password_data.is_focused
      << ", password.type=" << password_data.type
      << ", password.autocomplete_attr=" << password_data.autocomplete_attr;

  auto* autofill_client =
      autofill::OhAutofillClient::FromWebContents(web_contents());
  if (!autofill_client) {
    LOG(ERROR) << "autofill_client is nullptr";
    return;
  }
  form_to_request_url_ = page_url;
  last_fill_form_id_ = form_id;
  last_fill_focus_renderer_id_ = username_data.is_focused
                                     ? username_data.field_renderer_id
                                     : password_data.field_renderer_id;

  if (!base::ohos::IsPcDevice()) {
    if (state == OhosPasswordFormAutofillState::kNotRequested) {
      auto json_str = PasswordFormToJsonForRequest(
          EVENT_FILL, page_url, username_data, password_data);
      if (json_str.has_value()) {
        LOG(INFO) << "call autofill for request from system, form_id="
                  << form_id;
        bool result = autofill_client->OnAutofillEvent(json_str.value());
        if (!result) {
          LOG(ERROR) << "failed to call autofill for request";
          return;
        }
        SuppressKeyboard();
        UpdateLastRequestFilledItems(username_data, password_data);
      }
    }
  } else {
    auto event = (state == OhosPasswordFormAutofillState::kTextChanged)
                     ? EVENT_UPDATE
                     : EVENT_FILL;
    auto json_str = PasswordFormToJsonForRequest(event, page_url, username_data,
                                                 password_data);
    if (json_str.has_value()) {
      LOG(INFO) << "call autofill for request from system, state="
                << static_cast<int32_t>(state);
      bool result = autofill_client->OnAutofillEvent(json_str.value());
      if (!result) {
        LOG(ERROR) << "failed to call autofill for request";
        return;
      }
      NotifyAutofillPopupShow(true);
      UpdateLastRequestFilledItems(username_data, password_data);
    }
  }
}

bool ChromePasswordManagerClientExt::ArkPromptUserToSaveOrUpdatePassword(
    std::unique_ptr<password_manager::PasswordFormManagerForUI> form_to_save) {
  
  if (!web_contents()) {
    LOG(ERROR) << "web contents is nullptr, can't get autofill enable state.";
  } else {
    auto web_preference = web_contents()->GetOrCreateWebPreferences();
    if (!web_preference.is_autofill_enabled) {
      LOG(INFO) << "[PassWord Autofill] autofill interception successful.";
      return false;
    }
  }
 
  if (!form_to_save) {
    LOG(ERROR) << "form_to_save is nullptr";
    return false;
  }

  if (form_to_save->IsBlocklisted()) {
    LOG(INFO) << "autosave password is block listed.";
    return false;
  }

  // If the information used when the user logs in is current site filled in and
  // not modified, then do not prompt the user to save.
  if (IsLoginInfoConsistentWithFilled(form_to_save->GetPendingCredentials())) {
    LOG(INFO) << "auto filled password, not save on login";
    return false;
  }

  auto* autofill_client =
      autofill::OhAutofillClient::FromWebContents(web_contents());
  if (!autofill_client) {
    LOG(ERROR) << "autofill_client is nullptr";
    return false;
  }
  auto json_str =
      PasswordFormToJsonForSave(form_to_save->GetPendingCredentials());
  if (json_str.has_value()) {
    LOG(INFO) << "call autofill for save from system.";
    bool result = autofill_client->OnAutofillEvent(json_str.value());
    if (!result) {
      LOG(ERROR) << "failed to call autofill for save";
      return false;
    }
  }
  return true;
}
#endif
