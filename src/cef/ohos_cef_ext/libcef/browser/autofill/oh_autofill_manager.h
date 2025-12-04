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
 * Based on android_autofill_manager.h originally written by
 * Copyright 2017 The Chromium Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef CEF_LIBCEF_BROWSER_AUTOFILL_OH_AUTOFILL_MANAGER_H_
#define CEF_LIBCEF_BROWSER_AUTOFILL_OH_AUTOFILL_MANAGER_H_

#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"
#include "chrome/browser/ui/autofill/autofill_popup_hide_helper.h"
#include "components/autofill/core/browser/foundations/autofill_manager.h"
#include "components/autofill/core/common/dense_set.h"
#include "components/autofill/core/common/password_form_fill_data.h"
#include "ohos_cef_ext/libcef/browser/autofill/oh_autofill_provider.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

namespace autofill {

class AutofillProvider;
class ContentAutofillDriver;
// Creates an OhAutofillManager and attaches it to the `driver`.
//
// This hook is to be passed to CreateForWebContentsAndDelegate().
// It is the glue between ContentAutofillDriver[Factory] and
// OhAutofillManager.
//
// Other embedders (which don't want to use OhAutofillManager) shall use
// other implementations.
// This class forwards AutofillManager calls to AutofillProvider.
class OhAutofillManager : public AutofillManager,
                          public AutofillManager::Observer {
 public:
  explicit OhAutofillManager(AutofillDriver* driver);

  OhAutofillManager(const OhAutofillManager&) = delete;
  OhAutofillManager& operator=(const OhAutofillManager&) = delete;

  ~OhAutofillManager() override;

  base::WeakPtr<OhAutofillManager> GetWeakPtrToLeafClass() {
    return weak_ptr_factory_.GetWeakPtr();
  }
  base::WeakPtr<AutofillManager> GetWeakPtr() override;

  bool ShouldClearPreviewedForm() override;

  void OnDidFillAutofillFormDataImpl(const FormData& form,
                                     const base::TimeTicks timestamp) override;

  void OnDidEndTextFieldEditingImpl() override {}
  void OnHidePopupImpl() override;
  void OnSelectFieldOptionsDidChangeImpl(const FormData& form) override {}

  void Reset() override;

  void ReportAutofillWebOTPMetrics(bool used_web_otp) override {}

  bool has_server_prediction() const { return has_server_prediction_; }

  // Send the |form| to the renderer for the specified |action|.
  //
  // |triggered_origin| is the origin of the field from which the autofill is
  // triggered; this affects the security policy for cross-frame fills. See
  // AutofillDriver::FillOrPreviewForm() for further details.
  void FillOrPreviewForm(mojom::ActionPersistence action,
                         const FormData& form,
                         const FieldTypeGroup field_type_group,
                         const url::Origin& triggered_origin);

  absl::optional<std::string> FormDataToJson(const FormData& form,
                                             const FormFieldData& field,
                                             const std::string& event);
  absl::optional<std::string> FormDataToJsonForSave(const FormData& form);
  void FillData(const std::string& json_str);
  std::string GetAttributeOrUniqueId(const FormFieldData& field);
  void FillDataWithId(const base::Value::Dict* dict);
  bool isFocusField(const FormFieldData& field_data,
                    const FormFieldData& field);

  void OnCaretMovedInFormFieldImpl(const FormData& form,
                                   const FieldGlobalId& field_id,
                                   const gfx::Rect& caret_bounds) override {}

  void OnFocusOnNonFormFieldImpl() override;

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
  void SetPasswordPopupShow(bool is_show);
#endif

 protected:
  // friend void OhDriverInitHook(AutofillClient* client,
  //                              ContentAutofillDriver* driver);

  OhAutofillManager(AutofillDriver* driver, AutofillClient* client);

  void OnFormSubmittedImpl(const FormData& form,
                           mojom::SubmissionSource source) override;

  void OnTextFieldValueChangedImpl(const FormData& form,
                                   const FieldGlobalId& field_id,
                                   const base::TimeTicks timestamp) override;

  void OnTextFieldDidScrollImpl(const FormData& form,
                                const FieldGlobalId& field_id) override;

  void OnAskForValuesToFillImpl(
      const FormData& form,
      const FieldGlobalId& field_id,
      const gfx::Rect& caret_bounds,
      AutofillSuggestionTriggerSource trigger_source,
      std::optional<PasswordSuggestionRequest> password_request) override;

  void OnFocusOnFormFieldImpl(const FormData& form,
                              const FieldGlobalId& field_id) override;

  void OnSelectControlSelectionChangedImpl(const FormData& form,
                                           const FieldGlobalId& field_id) override;

  void OnJavaScriptChangedAutofilledValueImpl(const FormData& form,
                                              const FieldGlobalId& field_id,
                                              const std::u16string& old_value) override {}
  void OnLoadedServerPredictionsImpl(
      base::span<const raw_ptr<FormStructure, VectorExperimental>> forms) override;
  bool ShouldParseForms() override;

  void OnBeforeProcessParsedForms() override {}

  void OnFormProcessed(const FormData& form,
                       const FormStructure& form_structure) override;

 protected:
#ifdef UNIT_TEST
  // For the unit tests where WebContents isn't available.
  void set_autofill_provider_for_testing(AutofillProvider* autofill_provider) {
    autofill_provider_for_testing_ = autofill_provider;
  }
#endif  // UNIT_TEST

 private:
#ifndef OHOS_FUZZ_COMPILE_ERROR_FIX
  AutofillProvider* GetAutofillProvider();
#endif
  void StartNewLoggingSession();

  void HidePopup();

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
  void ForwardDataToPasswordManager(const std::string& page_url,
                                    const std::string& username,
                                    const std::string& password,
                                    bool is_other_account);

  bool IsUsernamePasswordFormField(FormRendererId form_id,
                                   FieldRendererId field_id);
  void PopupShow();

  void Hide(SuggestionHidingReason reason);
#endif

#if BUILDFLAG(ARKWEB_DATALIST)
  void SuggestionSelected(const FieldGlobalId& field_id, std::u16string text);
  void OnAskForValuesToFillImplForDatalist(const FormData& form,
                                           const FieldGlobalId& field_id,
                                           const gfx::Rect& caret_bounds);
#endif

  bool has_server_prediction_ = false;
  bool is_show_ = false;
  bool is_password_popup_show_ = false;
  std::unique_ptr<FormData> form_;
  std::optional<AutofillPopupHideHelper> popup_hide_helper_;
  base::ScopedObservation<AutofillManager, AutofillManager::Observer>
      autofill_manager_observation{this};
  base::WeakPtrFactory<OhAutofillManager> weak_ptr_factory_{this};
};

}  // namespace autofill

#endif
