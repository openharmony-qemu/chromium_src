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

#ifndef CEF_LIBCEF_BROWSER_AUTOFILL_OH_AUTOFILL_PROVIDER_H_
#define CEF_LIBCEF_BROWSER_AUTOFILL_OH_AUTOFILL_PROVIDER_H_

#include "base/memory/raw_ptr.h"
#include "base/time/time.h"
#include "components/autofill/core/common/form_data.h"
#include "components/autofill/core/common/mojom/autofill_types.mojom.h"
#include "components/autofill/core/common/signatures.h"
#include "content/public/browser/web_contents_user_data.h"

namespace content {
class WebContents;
}  // namespace content

namespace gfx {
class RectF;
}  // namespace gfx

namespace autofill {

class OhAutofillManager;

// This class defines the interface for the autofill implementation other than
// default BrowserAutofillManager. Unlike BrowserAutofillManager, this class
// has one instance per WebContents.
class OhAutofillProvider
    : public content::WebContentsUserData<OhAutofillProvider> {
 public:
  ~OhAutofillProvider() override;
  void OnAskForValuesToFill(OhAutofillManager* manager,
                            int32_t id,
                            const FormData& form,
                            const FormFieldData& field,
                            const gfx::RectF& bounding_box,
                            bool autoselect_first_suggestion);

  void OnTextFieldDidChange(OhAutofillManager* manager,
                            const FormData& form,
                            const FormFieldData& field,
                            const gfx::RectF& bounding_box,
                            const base::TimeTicks timestamp);

  void OnTextFieldDidScroll(OhAutofillManager* manager,
                            const FormData& form,
                            const FormFieldData& field,
                            const gfx::RectF& bounding_box);

  void OnSelectControlDidChange(OhAutofillManager* manager,
                                const FormData& form,
                                const FormFieldData& field,
                                const gfx::RectF& bounding_box);

  void OnFormSubmitted(OhAutofillManager* manager,
                       const FormData& form,
                       bool known_success,
                       mojom::SubmissionSource source);

  void OnFocusNoLongerOnForm(OhAutofillManager* manager,
                             bool had_interacted_form);

  void OnFocusOnFormField(OhAutofillManager* manager,
                          const FormData& form,
                          const FormFieldData& field,
                          const gfx::RectF& bounding_box);

  void OnDidFillAutofillFormData(OhAutofillManager* manager,
                                 const FormData& form,
                                 base::TimeTicks timestamp);

  void OnFormsSeen(OhAutofillManager* manager,
                   const std::vector<FormData>& forms);

  void OnHidePopup(OhAutofillManager* manager);

  void OnServerPredictionsAvailable(OhAutofillManager* manager);

  void OnServerQueryRequestError(OhAutofillManager* manager,
                                 FormSignature form_signature);

  void Reset(OhAutofillManager* manager);

  void FillOrPreviewForm(OhAutofillManager* manager,
                         int requestId,
                         const FormData& formData);

  // Notifies the renderer should accept the datalist suggestion given by
  // |value| and fill the input field indified by |field_id|.
  void RendererShouldAcceptDataListSuggestion(OhAutofillManager* manager,
                                              const FieldGlobalId& field_id,
                                              const std::u16string& value);

 protected:
  // WebContents takes the ownership of AutofillProvider.
  explicit OhAutofillProvider(content::WebContents* web_contents);
  friend class content::WebContentsUserData<OhAutofillProvider>;

  content::WebContents* web_contents() { return &GetWebContents(); }

 private:
  WEB_CONTENTS_USER_DATA_KEY_DECL();
};

}  // namespace autofill

#endif
