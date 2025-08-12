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
#include "arkweb/chromium_ext/components/autofill/content/render/autofill_agent_ext.h"

namespace autofill {

TEST_F(AutofillAgentTestWithFeatures, ArkFillAccountSuggestion) {
  LoadHTML(R"(<body><form>
    <input id="ff" list="fruits">
    <datalist id="fruits">
      <option value="Strawberry">
      <option value="Apple">
    </datalist>
  </form></body>)");
  autofill_agent().AsAutofillAgentExt()->ArkFillAccountSuggestion(u"username",
                                                                  u"password");
  FieldRendererId field_id = GetFieldRendererIdById("ff");
  EXPECT_CALL(autofill_driver(), AskForValuesToFill);
  autofill_agent().TriggerSuggestions(
      field_id, AutofillSuggestionTriggerSource::kFormControlElementClicked);
  autofill_agent().AsAutofillAgentExt()->ArkFillAccountSuggestion(u"username",
                                                                  u"password");
  EXPECT_EQ(autofill_agent().last_queried_element(),
            form_util::GetFormControlByRendererId(field_id));
}

TEST_F(AutofillAgentTestWithFeatures, OhAutoFillDidChangeScrollOffset) {
  LoadHTML(R"(<body><form>
    <input id="ff" list="fruits">
    <datalist id="fruits">
      <option value="Strawberry">
      <option value="Apple">
    </datalist>
  </form></body>)");
  EXPECT_FALSE(
      autofill_agent().AsAutofillAgentExt()->OhAutoFillDidChangeScrollOffset());
  FieldRendererId field_id = GetFieldRendererIdById("ff");
  EXPECT_CALL(autofill_driver(), AskForValuesToFill);
  autofill_agent().TriggerSuggestions(
      field_id, AutofillSuggestionTriggerSource::kFormControlElementClicked);

  autofill_agent().AsAutofillAgentExt()->SetIsNeedToCreatedPopup(false);
  EXPECT_FALSE(
      autofill_agent().AsAutofillAgentExt()->OhAutoFillDidChangeScrollOffset());

  autofill_agent().AsAutofillAgentExt()->SetIsNeedToCreatedPopup(true);
  autofill_agent().AsAutofillAgentExt()->SetCreatedPopupTime(
      base::TimeTicks::Now() + base::Milliseconds(1000));
  EXPECT_FALSE(
      autofill_agent().AsAutofillAgentExt()->OhAutoFillDidChangeScrollOffset());

  autofill_agent().AsAutofillAgentExt()->SetCreatedPopupTime(
      base::TimeTicks::Now() - base::Milliseconds(1000));
  EXPECT_FALSE(
      autofill_agent().AsAutofillAgentExt()->OhAutoFillDidChangeScrollOffset());

  autofill_agent().AsAutofillAgentExt()->SetCreatedPopupTime(
      base::TimeTicks::Now() - base::Milliseconds(100));
  EXPECT_TRUE(
      autofill_agent().AsAutofillAgentExt()->OhAutoFillDidChangeScrollOffset());

  EXPECT_TRUE(
      autofill_agent().AsAutofillAgentExt()->OhAutoFillDidChangeScrollOffset());
}

TEST_F(AutofillAgentTestWithFeatures, FillFieldWithValue) {
  LoadHTML(R"(<body><form>
    <input id="ff" list="fruits">
    <datalist id="fruits">
      <option value="Strawberry">
      <option value="Apple">
    </datalist>
  </form></body>)");
  EXPECT_TRUE(autofill_agent().AsAutofillAgentExt()->FillFieldWithValue(
      FieldRendererId(2), u"value"));
  FieldRendererId field_id = GetFieldRendererIdById("ff");
  FieldRendererId datalist_id = GetFieldRendererIdById("fruits");
  EXPECT_CALL(autofill_driver(), AskForValuesToFill);
  autofill_agent().TriggerSuggestions(
      field_id, AutofillSuggestionTriggerSource::kFormControlElementClicked);

  EXPECT_FALSE(autofill_agent().AsAutofillAgentExt()->FillFieldWithValue(
      field_id, u"value"));

  EXPECT_TRUE(autofill_agent().AsAutofillAgentExt()->FillFieldWithValue(
      datalist_id, u"value"));

  EXPECT_TRUE(autofill_agent().AsAutofillAgentExt()->FillFieldWithValue(
      FieldRendererId(0), u"value"));

  LoadHTML("<body><div id=ce contenteditable></div></body>");
  EXPECT_TRUE(autofill_agent().AsAutofillAgentExt()->FillFieldWithValue(
      datalist_id, u"value"));
  EXPECT_TRUE(autofill_agent().AsAutofillAgentExt()->FillFieldWithValue(
      GetFieldRendererIdById("ce"), u"value"));
}

}  // namespace autofill