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
 *
 * Based on android_autofill_manager.cc originally written by
 * Copyright 2017 The Chromium Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "ohos_cef_ext/libcef/browser/autofill/oh_autofill_manager.h"

#include <locale>

#include "base/containers/contains.h"
#include "base/json/json_reader.h"
#include "base/json/json_writer.h"
#include "base/memory/ptr_util.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "components/android_autofill/browser/autofill_provider.h"
#include "components/autofill/content/browser/content_autofill_driver.h"
#include "components/autofill/content/renderer/autofill_agent.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/web_contents.h"
#include "libcef/browser/browser_host_base.h"
#include "ohos_cef_ext/libcef/browser/autofill/oh_autofill_client.h"
#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
#include "chrome/browser/password_manager/chrome_password_manager_client.h"
using OhPasswordManagerClient = ChromePasswordManagerClient;
#endif

namespace {
const std::string EVENT = "event";
const std::string EVENT_SAVE = "save";
const std::string EVENT_FILL = "fill";
const std::string EVENT_UPDATE = "update";
const std::string EVENT_CLOSE = "close";

const std::string KEY_FOUCS = "focus";
const std::string KEY_RECT_X = "x";
const std::string KEY_RECT_Y = "y";
const std::string KEY_RECT_W = "width";
const std::string KEY_RECT_H = "height";
const std::string KEY_PLACEHOLDER = "placeholder";
const std::string KEY_VALUE = "value";
const std::string VALUE_OFF = "off";

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
const std::string KEY_USERNAME = "username";
const std::string KEY_PASSWORD = "password";
const std::string KEY_RETURN_PAGE_URL = "autofill_viewdata_origin_pageurl";
const std::string KEY_RETURN_OTHER_ACCOUNT = "autofill_viewdata_other_account";
#endif
}  // namespace

namespace autofill {

using base::TimeTicks;

OhAutofillManager::OhAutofillManager(AutofillDriver* driver)
    : AutofillManager(driver) {
  StartNewLoggingSession();
  autofill_manager_observation.Observe(this);
}

OhAutofillManager::~OhAutofillManager() = default;

base::WeakPtr<AutofillManager> OhAutofillManager::GetWeakPtr() {
  return weak_ptr_factory_.GetWeakPtr();
}

std::string OhAutofillManager::GetAttributeOrUniqueId(
    const FormFieldData& field) {
  auto attribute = field.autocomplete_attribute();
  if (!attribute.empty() || field.renderer_id().is_null()) {
    return attribute;
  }
  auto renderer_id = base::NumberToString(field.renderer_id().value());
  return renderer_id;
}

bool OhAutofillManager::isFocusField(const FormFieldData& field_data,
                                     const FormFieldData& field) {
  if (field_data.renderer_id().is_null() || field.renderer_id().is_null()) {
    return false;
  }
  return field_data.renderer_id().value() == field.renderer_id().value();
}

absl::optional<std::string> OhAutofillManager::FormDataToJson(
    const FormData& form,
    const FormFieldData& field,
    const std::string& event) {
  auto* rfh = static_cast<ContentAutofillDriver&>(driver()).render_frame_host();
  auto browser = CefBrowserHostBase::GetBrowserForHost(rfh);
  if (!browser) {
    return absl::nullopt;
  }

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
  if (IsUsernamePasswordFormField(form.renderer_id(), field.renderer_id())) {
    return absl::nullopt;
  }
#endif

  float ratio = browser->GetHost()->GetVirtualPixelRatio();
  auto offset =
      content::WebContents::FromRenderFrameHost(rfh)->GetContainerBounds();

  base::Value::List view_data_list;
  view_data_list.Append(base::Value::Dict().Set(EVENT, event));

  for (const FormFieldData& field_data : form.fields()) {
    base::Value::List list;
    auto key = GetAttributeOrUniqueId(field_data);
    list.Append(base::Value::Dict().Set(
        KEY_FOUCS, isFocusField(field_data, field) ? 1 : 0));
    list.Append(base::Value::Dict().Set(
        KEY_RECT_X,
        static_cast<int32_t>((field_data.bounds().x() + offset.x()) * ratio)));
    list.Append(base::Value::Dict().Set(
        KEY_RECT_Y,
        static_cast<int32_t>((field_data.bounds().y() + offset.y()) * ratio)));
    list.Append(base::Value::Dict().Set(
        KEY_RECT_W, static_cast<int32_t>(field_data.bounds().width() * ratio)));
    list.Append(base::Value::Dict().Set(
        KEY_RECT_H,
        static_cast<int32_t>(field_data.bounds().height() * ratio)));
    list.Append(base::Value::Dict().Set(
        KEY_PLACEHOLDER, base::UTF16ToUTF8(field_data.placeholder())));
    list.Append(base::Value::Dict().Set(KEY_VALUE,
                                        base::UTF16ToUTF8(field_data.value())));

    auto dict = base::Value::Dict().Set(key, std::move(list));
    view_data_list.Append(std::move(dict));
  }

  return base::WriteJson(view_data_list);
}

absl::optional<std::string> OhAutofillManager::FormDataToJsonForSave(
    const FormData& form) {
  auto* rfh = static_cast<ContentAutofillDriver&>(driver()).render_frame_host();
  auto browser = CefBrowserHostBase::GetBrowserForHost(rfh);
  if (!browser) {
    return absl::nullopt;
  }

  if (form_ == nullptr) {
    return absl::nullopt;
  }

  // Chromium does not assign values to FormFieldData.bounds in save scenario,
  // but oh-autofill-system requires valid bounds to pass code check,
  // so using the cached FormFieldData.bounds
  if (form.fields().size() != form_->fields().size()) {
    return absl::nullopt;
  }

  float ratio = browser->GetHost()->GetVirtualPixelRatio();
  auto offset =
      content::WebContents::FromRenderFrameHost(rfh)->GetContainerBounds();

  base::Value::List view_data_list;
  view_data_list.Append(base::Value::Dict().Set(EVENT, EVENT_SAVE));

  int32_t index = 0;
  for (const FormFieldData& field_data : form.fields()) {
    base::Value::List list;
    auto key = GetAttributeOrUniqueId(field_data);
    list.Append(base::Value::Dict().Set(KEY_FOUCS, 0));
    list.Append(base::Value::Dict().Set(
        KEY_RECT_X,
        static_cast<int32_t>(
            (form_->fields()[index].bounds().x() + offset.x()) * ratio)));
    list.Append(base::Value::Dict().Set(
        KEY_RECT_Y,
        static_cast<int32_t>(
            (form_->fields()[index].bounds().y() + offset.y()) * ratio)));
    list.Append(base::Value::Dict().Set(
        KEY_RECT_W,
        static_cast<int32_t>(form_->fields()[index].bounds().width() * ratio)));
    list.Append(base::Value::Dict().Set(
        KEY_RECT_H, static_cast<int32_t>(
                        form_->fields()[index].bounds().height() * ratio)));
    list.Append(base::Value::Dict().Set(
        KEY_PLACEHOLDER, base::UTF16ToUTF8(field_data.placeholder())));
    list.Append(base::Value::Dict().Set(KEY_VALUE,
                                        base::UTF16ToUTF8(field_data.value())));

    auto dict = base::Value::Dict().Set(key, std::move(list));
    view_data_list.Append(std::move(dict));
    index++;
  }

  return base::WriteJson(view_data_list);
}

void OhAutofillManager::FillDataWithId(const base::Value::Dict* dict) {
  for (auto it = dict->begin(); it != dict->end(); it++) {
    const base::Value::Dict* sub_dict = dict->FindDict(it->first);
    if (sub_dict) {
      const std::string* str = sub_dict->FindString(KEY_VALUE);
      if (!str) {
        continue;
      }
      absl::optional<int> index = sub_dict->FindInt(KEY_PLACEHOLDER);
      if (!index.has_value() || index.value() <= 0) {
        continue;
      }
      uint32_t i = static_cast<uint32_t>(index.value());
      uint32_t size = static_cast<uint32_t>(form_->fields().size());
      if (i > size) {
        continue;
      }
      const FormFieldData& field_data = form_->fields()[i - 1];
      if (GetAttributeOrUniqueId(field_data) == VALUE_OFF ||
          !field_data.is_visible()) {
        continue;
      }
      driver().ApplyFieldAction(
          mojom::FieldActionType::kReplaceAll, mojom::ActionPersistence::kFill,
          field_data.global_id(), base::UTF8ToUTF16(*str));
    }
  }
}

void OhAutofillManager::FillData(const std::string& json_str) {
  absl::optional<base::Value> root = base::JSONReader::Read(json_str);
  if (!root.has_value()) {
    return;
  }
  const base::Value::Dict* root_dict = root->GetIfDict();
  if (!root_dict) {
    return;
  }

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
  const std::string* username = root_dict->FindString(KEY_USERNAME);
  const std::string* password = root_dict->FindString(KEY_PASSWORD);
  const std::string* page_url = root_dict->FindString(KEY_RETURN_PAGE_URL);
  bool is_other_account =
      root_dict->FindBool(KEY_RETURN_OTHER_ACCOUNT).value_or(false);
  if (username || password) {
    ForwardDataToPasswordManager(page_url ? *page_url : std::string(),
                                 username ? *username : std::string(),
                                 password ? *password : std::string(),
                                 is_other_account);
    return;
  }
#endif

  if (form_ == nullptr) {
    return;
  }
  for (const FormFieldData& field_data : form_->fields()) {
    const std::string* value =
        root_dict->FindString(field_data.autocomplete_attribute());
    if (!value || value->empty() ||
        GetAttributeOrUniqueId(field_data) == VALUE_OFF ||
        !field_data.is_visible()) {
      continue;
    }
    driver().ApplyFieldAction(
        mojom::FieldActionType::kReplaceAll, mojom::ActionPersistence::kFill,
        field_data.global_id(), base::UTF8ToUTF16(*value));
  }
  FillDataWithId(root_dict);
  is_show_ = false;
}

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
void OhAutofillManager::ForwardDataToPasswordManager(
    const std::string& page_url,
    const std::string& username,
    const std::string& password,
    bool is_other_account) {
  LOG(INFO) << "autofill fill data, forward to password_manager";
  auto* rfh = static_cast<ContentAutofillDriver&>(driver()).render_frame_host();
  if (!rfh || !rfh->IsActive()) {
    LOG(ERROR) << "rfh is nullptr or not active";
    return;
  }
  auto* contents = content::WebContents::FromRenderFrameHost(rfh);
  if (!contents) {
    LOG(ERROR) << "webcontents is nullptr";
    return;
  }
  auto* password_manager = OhPasswordManagerClient::FromWebContents(contents);
  if (!password_manager) {
    LOG(ERROR) << "password_manager is nullptr";
    return;
  }
  password_manager->AsChromePasswordManagerClientExt()->FillData(page_url, username, password, is_other_account);
  is_password_popup_show_ = false;
  popup_hide_helper_.reset();
}

bool OhAutofillManager::IsUsernamePasswordFormField(FormRendererId form_id,
                                                    FieldRendererId field_id) {
  auto* rfh = static_cast<ContentAutofillDriver&>(driver()).render_frame_host();
  if (!rfh || !rfh->IsActive()) {
    LOG(ERROR) << "rfh is nullptr or not active";
    return false;
  }
  auto* contents = content::WebContents::FromRenderFrameHost(rfh);
  if (!contents) {
    LOG(ERROR) << "webcontents is nullptr";
    return false;
  }
  auto* password_manager = OhPasswordManagerClient::FromWebContents(contents);
  if (!password_manager) {
    LOG(ERROR) << "password_manager is nullptr";
    return false;
  }

  if (!form_id.is_null()) {
    return password_manager->AsChromePasswordManagerClientExt()->IsUsernamePasswordForm(form_id);
  } else {
    return password_manager->AsChromePasswordManagerClientExt()->IsUsernamePasswordField(field_id);
  }
}

void OhAutofillManager::SetPasswordPopupShow(bool is_show) {
  is_password_popup_show_ = is_show;
  if (is_show) {
    PopupShow();
  }
}

void OhAutofillManager::PopupShow() {
  auto* rfh = static_cast<ContentAutofillDriver&>(driver()).render_frame_host();
  if (!rfh || !rfh->IsActive()) {
    LOG(ERROR) << "rfh is nullptr or not active";
    return;
  }

  auto* contents = content::WebContents::FromRenderFrameHost(rfh);
  if (!contents) {
    LOG(ERROR) << "webcontents is nullptr";
    return;
  }

  AutofillPopupHideHelper::HidingParams hiding_params = {
      .hide_on_web_contents_lost_focus = false};
  AutofillPopupHideHelper::HidingCallback hiding_callback =
      base::BindRepeating(&OhAutofillManager::Hide, base::Unretained(this));
  AutofillPopupHideHelper::PictureInPictureDetectionCallback
      pip_detection_callback = base::BindRepeating([]() { return false; });

  popup_hide_helper_.emplace(
      contents, rfh->GetGlobalId(), std::move(hiding_params),
      std::move(hiding_callback), std::move(pip_detection_callback));
}

void OhAutofillManager::Hide(SuggestionHidingReason reason) {
  if (reason == SuggestionHidingReason::kTabGone ||
      reason == SuggestionHidingReason::kContentAreaMoved) {
    LOG(INFO) << "PopupHide, reason = " << static_cast<int>(reason);
    HidePopup();
  }
}
#endif

bool OhAutofillManager::ShouldClearPreviewedForm() {
  return false;
}

void OhAutofillManager::OnFormSubmittedImpl(const FormData& form,
                                            mojom::SubmissionSource source) {
  LOG(INFO) << "OnFormSubmittedImpl";
  auto* rfh = static_cast<ContentAutofillDriver&>(driver()).render_frame_host();
  if (!rfh || !rfh->IsActive()) {
    LOG(ERROR) << "rfh is nullptr or not active";
    return;
  }

  auto* contents = content::WebContents::FromRenderFrameHost(rfh);
  if (!contents) {
    LOG(ERROR) << "webcontents is nullptr";
    return;
  }

  auto* autofill_client = OhAutofillClient::FromWebContents(contents);
  if (!autofill_client) {
    LOG(ERROR) << "autofill client is nullptr";
    return;
  }

  auto json_str = FormDataToJsonForSave(form);
  if (json_str.has_value()) {
    autofill_client->OnAutofillEvent(json_str.value());
  }
}

void OhAutofillManager::OnTextFieldValueChangedImpl(const FormData& form,
                                                     const FieldGlobalId& field_id,
                                                     const TimeTicks timestamp) {
  LOG(INFO) << "OnTextFieldValueChangedImpl";
  auto* rfh = static_cast<ContentAutofillDriver&>(driver()).render_frame_host();
  if (!rfh || !rfh->IsActive()) {
    LOG(ERROR) << "rfh is nullptr or not active";
    return;
  }

  auto* contents = content::WebContents::FromRenderFrameHost(rfh);
  if (!contents) {
    LOG(ERROR) << "webcontents is nullptr";
    return;
  }

  auto* autofill_client = OhAutofillClient::FromWebContents(contents);
  if (!autofill_client) {
    LOG(ERROR) << "autofill client is nullptr";
    return;
  }

  const FormFieldData* field = form.FindFieldByGlobalId(field_id);

  auto json_str = FormDataToJson(form, *field, EVENT_UPDATE);
  if (json_str.has_value()) {
    autofill_client->OnAutofillEvent(json_str.value());
  }
}

void OhAutofillManager::OnTextFieldDidScrollImpl(
    const FormData& form,
    const FieldGlobalId& field_id) {
  LOG(INFO) << "OnTextFieldDidScrollImpl";
}

void OhAutofillManager::OnAskForValuesToFillImpl(
    const FormData& form,
    const FieldGlobalId& field_id,
    const gfx::Rect& caret_bounds,
    AutofillSuggestionTriggerSource trigger_source,
    std::optional<PasswordSuggestionRequest> password_request) {
  LOG(INFO) << "OnAskForValuesToFillImpl";
#if BUILDFLAG(ARKWEB_DATALIST)
  OnAskForValuesToFillImplForDatalist(form, field_id, caret_bounds);
#endif

  if (is_show_) {
    // Handle this event in OnTextFieldValueChangedImpl
    return;
  } else {
    form_ = std::make_unique<FormData>(form);
  }

  auto* rfh = static_cast<ContentAutofillDriver&>(driver()).render_frame_host();
  if (!rfh || !rfh->IsActive()) {
    LOG(ERROR) << "rfh is nullptr or not active";
    return;
  }

  auto* contents = content::WebContents::FromRenderFrameHost(rfh);
  if (!contents) {
    LOG(ERROR) << "webcontents is nullptr";
    return;
  }

  auto* autofill_client = OhAutofillClient::FromWebContents(contents);
  if (!autofill_client) {
    LOG(ERROR) << "autofill client is nullptr";
    return;
  }

  const FormFieldData* field = form.FindFieldByGlobalId(field_id);
  auto json_str = FormDataToJson(form, *field, EVENT_FILL);
  if (json_str.has_value()) {
    is_show_ = true;
    autofill_client->OnAutofillEvent(json_str.value());
  }
}

void OhAutofillManager::OnFocusOnFormFieldImpl(const FormData& form,
                                               const FieldGlobalId& field_id) {
  LOG(INFO) << "OnFocusOnFormFieldImpl";
}

void OhAutofillManager::OnSelectControlSelectionChangedImpl(
    const FormData& form,
    const FieldGlobalId& field_id) {
  LOG(INFO) << "OnSelectControlSelectionChangedImpl";
}

bool OhAutofillManager::ShouldParseForms() {
  LOG(INFO) << "ShouldParseForms";
  return true;
}

void OhAutofillManager::OnDidFillAutofillFormDataImpl(
    const FormData& form,
    const base::TimeTicks timestamp) {
  LOG(INFO) << "OnDidFillAutofillFormDataImpl";
}

void OhAutofillManager::OnHidePopupImpl() {
  auto* rfh = static_cast<ContentAutofillDriver&>(driver()).render_frame_host();
  if (!rfh || !rfh->IsActive()) {
    LOG(ERROR) << "rfh is nullptr or not active";
    return;
  }

  bool is_focused = false;
  auto browser = CefBrowserHostBase::GetBrowserForHost(rfh);
  if (browser) {
    is_focused = browser->IsFocused();
  }
  LOG(INFO) << "OnHidePopupImpl, is_show=" << is_show_
            << ", is_password_popup_show=" << is_password_popup_show_
            << ", is_focused=" << is_focused;
  if (is_show_ || (is_password_popup_show_ && is_focused)) {
    HidePopup();
  }
}

void OhAutofillManager::HidePopup() {
  auto* rfh = static_cast<ContentAutofillDriver&>(driver()).render_frame_host();
  if (!rfh || !rfh->IsActive()) {
    LOG(ERROR) << "rfh is nullptr or not active";
    return;
  }

  auto* contents = content::WebContents::FromRenderFrameHost(rfh);
  if (!contents) {
    LOG(ERROR) << "webcontents is nullptr";
    return;
  }

  auto* autofill_client = OhAutofillClient::FromWebContents(contents);
  if (!autofill_client) {
    LOG(ERROR) << "autofill client is nullptr";
    return;
  }

  base::Value::List view_data_list;
  view_data_list.Append(base::Value::Dict().Set(EVENT, EVENT_CLOSE));
  absl::optional<std::string> json_str = base::WriteJson(view_data_list);
  if (json_str.has_value()) {
    LOG(INFO) << "send event to hide autofill popup";
    autofill_client->OnAutofillEvent(json_str.value());
  }
  is_show_ = false;
  is_password_popup_show_ = false;
  popup_hide_helper_.reset();
}

void OhAutofillManager::OnFormProcessed(const FormData& form,
                                        const FormStructure& form_structure) {
  LOG(INFO) << "OnFormProcessed";
}

void OhAutofillManager::Reset() {
  LOG(INFO) << "Reset";
  AutofillManager::Reset();
  is_show_ = false;
  is_password_popup_show_ = false;
  if (form_) {
    form_.reset(nullptr);
  }
  popup_hide_helper_.reset();
  has_server_prediction_ = false;
}

#ifndef OHOS_FUZZ_COMPILE_ERROR_FIX
AutofillProvider* OhAutofillManager::GetAutofillProvider() {
  if (auto* rfh =
          static_cast<ContentAutofillDriver&>(driver()).render_frame_host()) {
    if (rfh->IsActive()) {
      if (auto* web_contents = content::WebContents::FromRenderFrameHost(rfh)) {
        return AutofillProvider::FromWebContents(web_contents);
      }
    }
  }
  return nullptr;
}
#endif

void OhAutofillManager::OnFocusOnNonFormFieldImpl() {
  LOG(INFO) << "OnFocusOnNonFormFieldImpl";
}

void OhAutofillManager::FillOrPreviewForm(mojom::ActionPersistence action,
                                          const FormData& form,
                                          FieldTypeGroup field_type_group,
                                          const url::Origin& triggered_origin) {
  LOG(INFO) << "FillOrPreviewForm";
}

void OhAutofillManager::StartNewLoggingSession() {}

#if BUILDFLAG(ARKWEB_DATALIST)
void OhAutofillManager::SuggestionSelected(const FieldGlobalId& field_id,
                                           std::u16string text) {
  driver().RendererShouldAcceptDataListSuggestion(field_id, text);
}

void OhAutofillManager::OnAskForValuesToFillImplForDatalist(
    const FormData& form,
    const FieldGlobalId& field_id,
    const gfx::Rect& caret_bounds) {
  auto* rfh = static_cast<ContentAutofillDriver&>(driver()).render_frame_host();
  if (!rfh || !rfh->IsActive()) {
    LOG(ERROR) << "rfh is nullptr or not active";
    return;
  }
  auto* contents = content::WebContents::FromRenderFrameHost(rfh);
  if (!contents) {
    LOG(ERROR) << "webcontents is nullptr";
    return;
  }
  auto* autofill_client = OhAutofillClient::FromWebContents(contents);
  if (!autofill_client) {
    LOG(ERROR) << "autofill client is nullptr";
    return;
  }

  std::vector<autofill::Suggestion> suggestions;
  const FormFieldData* field = form.FindFieldByGlobalId(field_id);
  for (SelectOption option : field->datalist_options()) {
    suggestions.push_back(autofill::Suggestion(option.value, autofill::SuggestionType::kDatalistEntry));
  }
  if (suggestions.size() == 0) {
    autofill_client->HideAutofillPopup();
  } else {
    autofill::AutofillClient::PopupOpenArgs open_args =
        autofill::AutofillClient::PopupOpenArgs(
            gfx::RectF(caret_bounds.x(), caret_bounds.y(), caret_bounds.width(),
                       caret_bounds.height()),
            base::i18n::TextDirection::LEFT_TO_RIGHT, suggestions,
            autofill::AutofillSuggestionTriggerSource::kOpenTextDataListChooser,
            /*form_control_ax_id=*/0, autofill::PopupAnchorType::kField);
    autofill_client->ShowAutofillPopup(
        open_args, base::BindOnce(&OhAutofillManager::SuggestionSelected,
                                  weak_ptr_factory_.GetWeakPtr(), field_id));
  }
}
#endif  // BUILDFLAG(ARKWEB_DATALIST)

void OhAutofillManager::OnLoadedServerPredictionsImpl(
    base::span<const raw_ptr<FormStructure, VectorExperimental>> forms) {
  LOG(INFO) << "OnLoadedServerPredictionsImpl";
}

}  // namespace autofill
