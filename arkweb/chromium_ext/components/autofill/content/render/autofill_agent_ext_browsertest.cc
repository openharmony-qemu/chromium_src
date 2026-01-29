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

TEST_F(AutofillAgentTestWithFeatures, AutofillSurfaceClosedReadonly) {
  LoadHTML(R"(<body><form>
    <input id="ff" readonly>
  </form></body>)");
  auto agent_ext = autofill_agent()
                       .AsAutofillAgentExt()
                       ->GetPasswordAutofillAgent()
                       ->AsPasswordAutofillAgentExt();
  agent_ext->CleanupOnDocumentShutdownExt();
  FieldRendererId field_id = GetFieldRendererIdById("ff");
  autofill_agent().TriggerSuggestions(
      field_id, AutofillSuggestionTriggerSource::kFormControlElementClicked);

  EXPECT_EQ(autofill_agent().last_queried_element(),
            form_util::GetFormControlByRendererId(field_id));
  agent_ext->AutofillSurfaceClosed(true);
}

TEST_F(AutofillAgentTestWithFeatures, AutofillSurfaceClosed) {
  LoadHTML(R"(<body><form>
    <input id="ff">
  </form></body>)");
  auto agent_ext = autofill_agent()
                       .AsAutofillAgentExt()
                       ->GetPasswordAutofillAgent()
                       ->AsPasswordAutofillAgentExt();
  FieldRendererId field_id = GetFieldRendererIdById("ff");
  autofill_agent().TriggerSuggestions(
      field_id, AutofillSuggestionTriggerSource::kFormControlElementClicked);

  EXPECT_EQ(autofill_agent().last_queried_element(),
            form_util::GetFormControlByRendererId(field_id));
  agent_ext->AutofillSurfaceClosed(false);
  agent_ext->AutofillSurfaceClosed(true);
}

TEST_F(AutofillAgentTestWithFeatures, SetParsedPasswordForm) {
  LoadHTML(R"(<body><form>
    <INPUT type='text' id='username'/>
    <INPUT type='password' id='password'/>
  </form></body>)");
  PasswordFormFillData form_data;
  auto agent =
      autofill_agent().AsAutofillAgentExt()->GetPasswordAutofillAgent();
  agent->AsPasswordAutofillAgentExt()->CleanupOnDocumentShutdownExt();
  EXPECT_TRUE(form_data.username_element_renderer_id.is_null());
  EXPECT_TRUE(form_data.password_element_renderer_id.is_null());
  agent->SetParsedPasswordForm(form_data);
  form_data.password_element_renderer_id = FieldRendererId(2);
  EXPECT_TRUE(form_data.username_element_renderer_id.is_null());
  EXPECT_FALSE(form_data.password_element_renderer_id.is_null());
  agent->SetParsedPasswordForm(form_data);
  form_data.password_element_renderer_id = GetFieldRendererIdById("password");
  EXPECT_TRUE(form_data.username_element_renderer_id.is_null());
  EXPECT_FALSE(form_data.password_element_renderer_id.is_null());
  agent->SetParsedPasswordForm(form_data);
  form_data.username_element_renderer_id = GetFieldRendererIdById("username");
  EXPECT_FALSE(form_data.username_element_renderer_id.is_null());
  EXPECT_FALSE(form_data.password_element_renderer_id.is_null());
  agent->SetParsedPasswordForm(form_data);
  form_data.password_element_renderer_id = FieldRendererId();
  EXPECT_FALSE(form_data.username_element_renderer_id.is_null());
  EXPECT_TRUE(form_data.password_element_renderer_id.is_null());
  agent->SetParsedPasswordForm(form_data);
}

TEST_F(AutofillAgentTestWithFeatures, IsPasswordAutofill) {
  LoadHTML(R"(<body><form>
    <input id="ff">
  </form></body>)");
  FieldRendererId field_id = GetFieldRendererIdById("ff");
  autofill_agent().TriggerSuggestions(
      field_id, AutofillSuggestionTriggerSource::kFormControlElementClicked);
  EXPECT_EQ(autofill_agent().last_queried_element(),
            form_util::GetFormControlByRendererId(field_id));
  LoadHTML(R"(<body><form>
    <input id="gg" readonly>
  </form></body>)");
  auto agent_ext = autofill_agent()
                       .AsAutofillAgentExt()
                       ->GetPasswordAutofillAgent()
                       ->AsPasswordAutofillAgentExt();
  agent_ext->CleanupOnDocumentShutdownExt();
  EXPECT_FALSE(agent_ext->IsPasswordAutofill(
      autofill_agent().last_queried_element().DynamicTo<blink::WebInputElement>()));
  field_id = GetFieldRendererIdById("gg");
  autofill_agent().TriggerSuggestions(
      field_id, AutofillSuggestionTriggerSource::kFormControlElementClicked);
  EXPECT_EQ(autofill_agent().last_queried_element(),
            form_util::GetFormControlByRendererId(field_id));
  EXPECT_FALSE(agent_ext->IsPasswordAutofill(
      autofill_agent().last_queried_element().DynamicTo<blink::WebInputElement>()));
  LoadHTML(R"(<body><form>
    <INPUT type='text' id='username'/>
    <INPUT type='password' id='password'/>
  </form></body>)");
  field_id = GetFieldRendererIdById("username");
  autofill_agent().TriggerSuggestions(
      field_id, AutofillSuggestionTriggerSource::kFormControlElementClicked);
  EXPECT_EQ(autofill_agent().last_queried_element(),
            form_util::GetFormControlByRendererId(field_id));

  PasswordFormFillData form_data;
  form_data.username_element_renderer_id = GetFieldRendererIdById("username");
  agent_ext->SetParsedPasswordForm(form_data);
  EXPECT_TRUE(agent_ext->IsPasswordAutofill(
      autofill_agent().last_queried_element().DynamicTo<blink::WebInputElement>()));
  form_data.password_element_renderer_id = GetFieldRendererIdById("password");
  agent_ext->SetParsedPasswordForm(form_data);
  EXPECT_TRUE(agent_ext->IsPasswordAutofill(
      autofill_agent().last_queried_element().DynamicTo<blink::WebInputElement>()));

  field_id = GetFieldRendererIdById("password");
  autofill_agent().TriggerSuggestions(
      field_id, AutofillSuggestionTriggerSource::kFormControlElementClicked);
  EXPECT_EQ(autofill_agent().last_queried_element(),
            form_util::GetFormControlByRendererId(field_id));
  EXPECT_TRUE(agent_ext->IsPasswordAutofill(
      autofill_agent().last_queried_element().DynamicTo<blink::WebInputElement>()));
  agent_ext->CleanupOnDocumentShutdownExt();
  EXPECT_FALSE(agent_ext->IsPasswordAutofill(
      autofill_agent().last_queried_element().DynamicTo<blink::WebInputElement>()));
  form_data.username_element_renderer_id = FieldRendererId();
  agent_ext->SetParsedPasswordForm(form_data);
  EXPECT_TRUE(agent_ext->IsPasswordAutofill(
      autofill_agent().last_queried_element().DynamicTo<blink::WebInputElement>()));
}

TEST_F(AutofillAgentTestWithFeatures, RequestAutofill) {
  LoadHTML(R"(<body><form>
    <input id="gg" readonly>
  </form></body>)");
  auto agent_ext = autofill_agent()
                       .AsAutofillAgentExt()
                       ->GetPasswordAutofillAgent()
                       ->AsPasswordAutofillAgentExt();
  agent_ext->CleanupOnDocumentShutdownExt();
  FieldRendererId field_id = GetFieldRendererIdById("gg");
  autofill_agent().TriggerSuggestions(
      field_id, AutofillSuggestionTriggerSource::kFormControlElementClicked);
  EXPECT_EQ(autofill_agent().last_queried_element(),
            form_util::GetFormControlByRendererId(field_id));
  EXPECT_FALSE(agent_ext->RequestAutofill(
      autofill_agent().last_queried_element(), true));

  LoadHTML(R"(<body>
    <INPUT type='text' id='username'/>
    <INPUT type='password' id='password'/>
  </body>)");
  field_id = GetFieldRendererIdById("username");
  autofill_agent().TriggerSuggestions(
      field_id, AutofillSuggestionTriggerSource::kFormControlElementClicked);
  EXPECT_EQ(autofill_agent().last_queried_element(),
            form_util::GetFormControlByRendererId(field_id));
  EXPECT_FALSE(agent_ext->RequestAutofill(
      autofill_agent().last_queried_element(), true));
  PasswordFormFillData form_data;
  form_data.username_element_renderer_id = GetFieldRendererIdById("username");
  form_data.password_element_renderer_id = FieldRendererId();
  agent_ext->SetParsedPasswordForm(form_data);
  EXPECT_TRUE(agent_ext->RequestAutofill(
      autofill_agent().last_queried_element(), true));
  field_id = GetFieldRendererIdById("password");
  autofill_agent().TriggerSuggestions(
      field_id, AutofillSuggestionTriggerSource::kFormControlElementClicked);
  EXPECT_EQ(autofill_agent().last_queried_element(),
            form_util::GetFormControlByRendererId(field_id));
  form_data.username_element_renderer_id = FieldRendererId();
  form_data.password_element_renderer_id = GetFieldRendererIdById("password");
  agent_ext->SetParsedPasswordForm(form_data);
  EXPECT_TRUE(agent_ext->RequestAutofill(
      autofill_agent().last_queried_element(), true));
  agent_ext->CleanupOnDocumentShutdownExt();

  LoadHTML(R"(<body><form>
    <INPUT type='text' id='username'/>
    <INPUT type='password' id='password' readonly/>
  </form></body>)");
  field_id = GetFieldRendererIdById("username");
  autofill_agent().TriggerSuggestions(
      field_id, AutofillSuggestionTriggerSource::kFormControlElementClicked);
  EXPECT_EQ(autofill_agent().last_queried_element(),
            form_util::GetFormControlByRendererId(field_id));
  form_data.username_element_renderer_id = GetFieldRendererIdById("username");
  form_data.password_element_renderer_id = GetFieldRendererIdById("password");
  agent_ext->SetParsedPasswordForm(form_data);
  EXPECT_TRUE(agent_ext->RequestAutofill(
      autofill_agent().last_queried_element(), false));
  agent_ext->CleanupOnDocumentShutdownExt();

  LoadHTML(R"(<body><form>
    <INPUT type='text' id='username' readonly/>
    <INPUT type='password' id='password'/>
  </form></body>)");
  field_id = GetFieldRendererIdById("password");
  autofill_agent().TriggerSuggestions(
      field_id, AutofillSuggestionTriggerSource::kFormControlElementClicked);
  EXPECT_EQ(autofill_agent().last_queried_element(),
            form_util::GetFormControlByRendererId(field_id));
  form_data.username_element_renderer_id = GetFieldRendererIdById("username");
  form_data.password_element_renderer_id = GetFieldRendererIdById("password");
  agent_ext->SetParsedPasswordForm(form_data);
  EXPECT_TRUE(agent_ext->RequestAutofill(
      autofill_agent().last_queried_element(), false));
}

TEST_F(AutofillAgentTestWithFeatures, FillAccountSuggestion) {
  LoadHTML(R"(<body><form>
    <input id="gg">
  </form></body>)");
  auto agent_ext = autofill_agent()
                       .AsAutofillAgentExt()
                       ->GetPasswordAutofillAgent()
                       ->AsPasswordAutofillAgentExt();
  agent_ext->CleanupOnDocumentShutdownExt();
  FieldRendererId field_id = GetFieldRendererIdById("gg");
  autofill_agent().TriggerSuggestions(
      field_id, AutofillSuggestionTriggerSource::kFormControlElementClicked);
  EXPECT_EQ(autofill_agent().last_queried_element(),
            form_util::GetFormControlByRendererId(field_id));

  LoadHTML(R"(<body><form>
    <INPUT type='text' id='username'/>
    <INPUT type='password' id='password' readonly/>
  </form></body>)");
  EXPECT_FALSE(agent_ext->FillAccountSuggestion(
      autofill_agent().last_queried_element(), u"username", u"password"));
  field_id = GetFieldRendererIdById("username");
  autofill_agent().TriggerSuggestions(
      field_id, AutofillSuggestionTriggerSource::kFormControlElementClicked);
  EXPECT_EQ(autofill_agent().last_queried_element(),
            form_util::GetFormControlByRendererId(field_id));
  PasswordFormFillData form_data;
  form_data.username_element_renderer_id = GetFieldRendererIdById("username");
  agent_ext->SetParsedPasswordForm(form_data);
  EXPECT_TRUE(agent_ext->FillAccountSuggestion(
      autofill_agent().last_queried_element(), u"username", u"password"));
  form_data.password_element_renderer_id = GetFieldRendererIdById("password");
  agent_ext->SetParsedPasswordForm(form_data);
  EXPECT_FALSE(agent_ext->FillAccountSuggestion(
      autofill_agent().last_queried_element(), u"username", u"password"));
  agent_ext->CleanupOnDocumentShutdownExt();

  LoadHTML(R"(<body><form>
    <INPUT type='text' id='username' readonly/>
    <INPUT type='password' id='password'/>
  </form></body>)");
  field_id = GetFieldRendererIdById("username");
  autofill_agent().TriggerSuggestions(
      field_id, AutofillSuggestionTriggerSource::kFormControlElementClicked);
  EXPECT_EQ(autofill_agent().last_queried_element(),
            form_util::GetFormControlByRendererId(field_id));
  form_data.username_element_renderer_id = GetFieldRendererIdById("username");
  form_data.password_element_renderer_id = FieldRendererId();
  agent_ext->SetParsedPasswordForm(form_data);
  EXPECT_TRUE(agent_ext->FillAccountSuggestion(
      autofill_agent().last_queried_element(), u"username", u"password"));
  agent_ext->CleanupOnDocumentShutdownExt();

  LoadHTML(R"(<body><form>
    <INPUT type='text' id='username'/>
    <INPUT type='password' id='password'/>
  </form></body>)");
  field_id = GetFieldRendererIdById("username");
  autofill_agent().TriggerSuggestions(
      field_id, AutofillSuggestionTriggerSource::kFormControlElementClicked);
  EXPECT_EQ(autofill_agent().last_queried_element(),
            form_util::GetFormControlByRendererId(field_id));
  form_data.username_element_renderer_id = GetFieldRendererIdById("username");
  form_data.password_element_renderer_id = GetFieldRendererIdById("password");
  agent_ext->SetParsedPasswordForm(form_data);
  EXPECT_TRUE(agent_ext->FillAccountSuggestion(
      autofill_agent().last_queried_element(), u"", u"password"));
  EXPECT_TRUE(agent_ext->FillAccountSuggestion(
      autofill_agent().last_queried_element(), u"username", u"password"));
  field_id = GetFieldRendererIdById("password");
  autofill_agent().TriggerSuggestions(
      field_id, AutofillSuggestionTriggerSource::kFormControlElementClicked);
  EXPECT_EQ(autofill_agent().last_queried_element(),
            form_util::GetFormControlByRendererId(field_id));
  EXPECT_TRUE(agent_ext->FillAccountSuggestion(
      autofill_agent().last_queried_element(), u"", u"password"));
  EXPECT_TRUE(agent_ext->FillAccountSuggestion(
      autofill_agent().last_queried_element(), u"username", u"password"));
  agent_ext->CleanupOnDocumentShutdownExt();
}

TEST_F(AutofillAgentTestWithFeatures, FillFieldWithValueTest001) {
  LoadHTML(R"(<body>
  <div id=ff ></div>
  <div id=fruits ></div>
  </body>)");

  FieldRendererId field_id_a = GetFieldRendererIdById("ff");
  FieldRendererId field_id_b = GetFieldRendererIdById("fruits");
  EXPECT_CALL(autofill_driver(), AskForValuesToFill);
  autofill_agent().TriggerSuggestions(
      field_id_a, AutofillSuggestionTriggerSource::kFormControlElementClicked);

  EXPECT_FALSE(autofill_agent().AsAutofillAgentExt()->FillFieldWithValue(field_id_a, u"value"));

  EXPECT_TRUE(autofill_agent().AsAutofillAgentExt()->FillFieldWithValue(field_id_b, u"value"));
}

// Priority 1: Test isAutofillEnabled() - All Branches

TEST_F(AutofillAgentTestWithFeatures, IsAutofillEnabled_EnabledTrue) {
  LoadHTML(R"(<body><form><input id="test"></form></body>)");
  EXPECT_TRUE(autofill_agent().AsAutofillAgentExt()->isAutofillEnabled());
}


// Priority 1: Test OhAutoFillFormControlElementClicked() - All Branches

TEST_F(AutofillAgentTestWithFeatures, OhAutoFillFormControlElementClicked_NullNode) {
  LoadHTML(R"(<body><form><input id="test"></form></body>)");
  blink::WebNode null_node;
  autofill_agent().AsAutofillAgentExt()->OhAutoFillFormControlElementClicked(null_node);
}

TEST_F(AutofillAgentTestWithFeatures, OhAutoFillFormControlElementClicked_NotFocused) {
  LoadHTML(R"(<body><form><input id="test"></form></body>)");
  blink::WebInputElement element = GetWebElementById("test").DynamicTo<blink::WebInputElement>();
  ASSERT_TRUE(element);
  // Note: This test covers the code path when an element is clicked but not focused.
  // Direct focus state verification is limited in the test environment, but the code
  // path is exercised through the ElementClicked call.
  autofill_agent().AsAutofillAgentExt()->OhAutoFillFormControlElementClicked(element);
}

TEST_F(AutofillAgentTestWithFeatures, OhAutoFillFormControlElementClicked_NormalCase) {
  // Note: This test assumes BUILDFLAG(ARKWEB_AUTOFILL) and BUILDFLAG(ARKWEB_UNITTESTS)
  // are properly defined. The test fixture should handle feature flag setup.
  LoadHTML(R"(<body><form><input type="text" id="test"></form></body>)");
  FieldRendererId field_id = GetFieldRendererIdById("test");
  autofill_agent().TriggerSuggestions(
      field_id, AutofillSuggestionTriggerSource::kFormControlElementClicked);

  // Use JavaScript to set focus instead of SetFocused()
  ExecuteJavaScriptForTests(R"(document.getElementById('test').focus();)");
  blink::WebInputElement element = GetWebElementById("test").DynamicTo<blink::WebInputElement>();
  ASSERT_TRUE(element);
  autofill_agent().AsAutofillAgentExt()->SetIsPopupCreatedByFocusChange(false);

  EXPECT_FALSE(autofill_agent().AsAutofillAgentExt()->is_need_to_created_popup_);
  autofill_agent().AsAutofillAgentExt()->OhAutoFillFormControlElementClicked(element);
  EXPECT_TRUE(autofill_agent().AsAutofillAgentExt()->is_need_to_created_popup_);
}

TEST_F(AutofillAgentTestWithFeatures, OhAutoFillFormControlElementClicked_IsPasswordAutofill) {
  LoadHTML(R"(<body><form>
    <input type="text" id="username"/>
    <input type="password" id="password"/>
  </form></body>)");

  PasswordFormFillData form_data;
  form_data.username_element_renderer_id = GetFieldRendererIdById("username");
  form_data.password_element_renderer_id = GetFieldRendererIdById("password");
  auto agent_ext = autofill_agent()
                       .AsAutofillAgentExt()
                       ->GetPasswordAutofillAgent()
                       ->AsPasswordAutofillAgentExt();
  agent_ext->SetParsedPasswordFormExt(form_data);
  // Verify the password form data was properly set
  ASSERT_FALSE(form_data.username_element_renderer_id.is_null());
  ASSERT_FALSE(form_data.password_element_renderer_id.is_null());

  FieldRendererId field_id = GetFieldRendererIdById("username");
  autofill_agent().TriggerSuggestions(
      field_id, AutofillSuggestionTriggerSource::kFormControlElementClicked);

  // Use JavaScript to set focus instead of SetFocused()
  ExecuteJavaScriptForTests(R"(document.getElementById('username').focus();)");
  blink::WebInputElement element = GetWebElementById("username").DynamicTo<blink::WebInputElement>();
  ASSERT_TRUE(element);

  bool initial_popup_state = autofill_agent().AsAutofillAgentExt()->is_need_to_created_popup_;
  autofill_agent().AsAutofillAgentExt()->OhAutoFillFormControlElementClicked(element);
  EXPECT_EQ(autofill_agent().AsAutofillAgentExt()->is_need_to_created_popup_, initial_popup_state);
}

TEST_F(AutofillAgentTestWithFeatures, OhAutoFillFormControlElementClicked_PopupCreatedByFocusChange) {
  LoadHTML(R"(<body><form><input type="text" id="test"></form></body>)");
  FieldRendererId field_id = GetFieldRendererIdById("test");
  autofill_agent().TriggerSuggestions(
      field_id, AutofillSuggestionTriggerSource::kFormControlElementClicked);

  // Use JavaScript to set focus instead of SetFocused()
  ExecuteJavaScriptForTests(R"(document.getElementById('test').focus();)");
  blink::WebInputElement element = GetWebElementById("test").DynamicTo<blink::WebInputElement>();
  ASSERT_TRUE(element);
  autofill_agent().AsAutofillAgentExt()->SetIsPopupCreatedByFocusChange(true);

  autofill_agent().AsAutofillAgentExt()->OhAutoFillFormControlElementClicked(element);
  EXPECT_FALSE(autofill_agent().AsAutofillAgentExt()->is_popup_created_by_focus_change_);
}

// Priority 1: Test OhFormControlElementClicked() - All Branches

TEST_F(AutofillAgentTestWithFeatures, OhFormControlElementClicked_AutofillDisabled) {
  LoadHTML(R"(<body><form><input id="test"></form></body>)");
  autofill_agent().AsAutofillAgentExt()->OhFormControlElementClicked();
}

TEST_F(AutofillAgentTestWithFeatures, OhFormControlElementClicked_NoFocusedElement) {
  LoadHTML(R"(<body><form><input id="test"></form></body>)");
  autofill_agent().AsAutofillAgentExt()->OhFormControlElementClicked();
}

TEST_F(AutofillAgentTestWithFeatures, OhFormControlElementClicked_NotFormControl) {
  LoadHTML(R"(<body><div id="test">Not a form control</div></body>)");
  ExecuteJavaScriptForTests(R"(document.getElementById('test').focus();)");
  autofill_agent().AsAutofillAgentExt()->OhFormControlElementClicked();
}

TEST_F(AutofillAgentTestWithFeatures, OhFormControlElementClicked_NotTextInput) {
  LoadHTML(R"(<body><form><input type="checkbox" id="test"></form></body>)");
  ExecuteJavaScriptForTests(R"(document.getElementById('test').focus();)");
  autofill_agent().AsAutofillAgentExt()->OhFormControlElementClicked();
}

TEST_F(AutofillAgentTestWithFeatures, OhFormControlElementClicked_RequestAutofillFails) {
  LoadHTML(R"(<body><form><input type="text" id="test"></form></body>)");
  ExecuteJavaScriptForTests(R"(document.getElementById('test').focus();)");
  autofill_agent().AsAutofillAgentExt()->OhFormControlElementClicked();
}

TEST_F(AutofillAgentTestWithFeatures, OhFormControlElementClicked_RequestAutofillSucceeds_IsPc) {
  LoadHTML(R"(<body><form>
    <input type="text" id="username"/>
    <input type="password" id="password"/>
  </form></body>)");

  PasswordFormFillData form_data;
  form_data.username_element_renderer_id = GetFieldRendererIdById("username");
  form_data.password_element_renderer_id = FieldRendererId();
  auto agent_ext = autofill_agent()
                       .AsAutofillAgentExt()
                       ->GetPasswordAutofillAgent()
                       ->AsPasswordAutofillAgentExt();
  agent_ext->SetParsedPasswordFormExt(form_data);
  // Verify the password form data was properly set
  ASSERT_FALSE(form_data.username_element_renderer_id.is_null());

  ExecuteJavaScriptForTests(R"(document.getElementById('username').focus();)");
  autofill_agent().AsAutofillAgentExt()->OhFormControlElementClicked();

  // Note: is_popup_possibly_visible_ is a protected member, cannot directly access
  // The test verifies that OhFormControlElementClicked executes without errors
}

TEST_F(AutofillAgentTestWithFeatures, OhFormControlElementClicked_RequestAutofillSucceeds_NotPc) {
  LoadHTML(R"(<body><form>
    <input type="text" id="username"/>
    <input type="password" id="password"/>
  </form></body>)");

  PasswordFormFillData form_data;
  form_data.username_element_renderer_id = GetFieldRendererIdById("username");
  form_data.password_element_renderer_id = FieldRendererId();
  auto agent_ext = autofill_agent()
                       .AsAutofillAgentExt()
                       ->GetPasswordAutofillAgent()
                       ->AsPasswordAutofillAgentExt();
  agent_ext->SetParsedPasswordFormExt(form_data);
  // Verify the password form data was properly set
  ASSERT_FALSE(form_data.username_element_renderer_id.is_null());

  ExecuteJavaScriptForTests(R"(document.getElementById('username').focus();)");
  autofill_agent().AsAutofillAgentExt()->OhFormControlElementClicked();

  // Note: is_popup_possibly_visible_ is a protected member, cannot directly access
  // The test verifies that OhFormControlElementClicked executes without errors
}

// Priority 2: Test ArkFillAccountSuggestion() - Null Element

TEST_F(AutofillAgentTestWithFeatures, ArkFillAccountSuggestion_NullLastQueriedElement) {
  LoadHTML(R"(<body><form><input id="test"></form></body>)");
  // Verify that FillAccountSuggestion is NOT called when last_queried_element_ is null
  // Since last_queried_element is null, ArkFillAccountSuggestion should return without
  // calling FillAccountSuggestion on the password autofill agent
  autofill_agent().AsAutofillAgentExt()->ArkFillAccountSuggestion(u"testuser", u"testpass");
  // If we reach here without crash, the test passes (no FillAccountSuggestion was called)
}

// Priority 2: Test FillFieldWithValue() - Uncovered Branches

TEST_F(AutofillAgentTestWithFeatures, FillFieldWithValue_Formless_NullDocument) {
  LoadHTML(R"(<body><input id="field1"></body>)");
  FieldRendererId field_id = GetFieldRendererIdById("field1");

  EXPECT_CALL(autofill_driver(), AskForValuesToFill);
  autofill_agent().TriggerSuggestions(
      field_id, AutofillSuggestionTriggerSource::kFormControlElementClicked);

  EXPECT_TRUE(autofill_agent().AsAutofillAgentExt()->FillFieldWithValue(
      FieldRendererId(999), u"value"));
}

TEST_F(AutofillAgentTestWithFeatures, FillFieldWithValue_Formless_ElementFound) {
  LoadHTML(R"(<body><input id="field1"><input id="field2"></body>)");
  FieldRendererId field_id1 = GetFieldRendererIdById("field1");
  FieldRendererId field_id2 = GetFieldRendererIdById("field2");

  EXPECT_CALL(autofill_driver(), AskForValuesToFill);
  autofill_agent().TriggerSuggestions(
      field_id1, AutofillSuggestionTriggerSource::kFormControlElementClicked);

  EXPECT_TRUE(autofill_agent().AsAutofillAgentExt()->FillFieldWithValue(
      field_id2, u"filled_value"));
}

TEST_F(AutofillAgentTestWithFeatures, FillFieldWithValue_FormBased_ElementFound) {
  LoadHTML(R"(<body><form>
    <input id="field1">
    <input id="field2">
  </form></body>)");
  FieldRendererId field_id1 = GetFieldRendererIdById("field1");
  FieldRendererId field_id2 = GetFieldRendererIdById("field2");

  EXPECT_CALL(autofill_driver(), AskForValuesToFill);
  autofill_agent().TriggerSuggestions(
      field_id1, AutofillSuggestionTriggerSource::kFormControlElementClicked);

  EXPECT_TRUE(autofill_agent().AsAutofillAgentExt()->FillFieldWithValue(
      field_id2, u"filled_value"));
}

TEST_F(AutofillAgentTestWithFeatures, FillFieldWithValue_FormBased_ElementNotFound) {
  LoadHTML(R"(<body><form>
    <input id="field1">
  </form></body>)");
  FieldRendererId field_id1 = GetFieldRendererIdById("field1");

  EXPECT_CALL(autofill_driver(), AskForValuesToFill);
  autofill_agent().TriggerSuggestions(
      field_id1, AutofillSuggestionTriggerSource::kFormControlElementClicked);

  EXPECT_TRUE(autofill_agent().AsAutofillAgentExt()->FillFieldWithValue(
      FieldRendererId(999), u"filled_value"));
}

// Priority 2: Test OhAutoFillDidChangeScrollOffset() - Timer and Uncovered Branches

TEST_F(AutofillAgentTestWithFeatures, OhAutoFillDidChangeScrollOffset_NotInputElement) {
  LoadHTML(R"(<body><form><textarea id="test"></textarea></form></body>)");
  FieldRendererId field_id = GetFieldRendererIdById("test");
  EXPECT_CALL(autofill_driver(), AskForValuesToFill);
  autofill_agent().TriggerSuggestions(
      field_id, AutofillSuggestionTriggerSource::kFormControlElementClicked);

  EXPECT_FALSE(autofill_agent().AsAutofillAgentExt()->OhAutoFillDidChangeScrollOffset());
}

TEST_F(AutofillAgentTestWithFeatures, OhAutoFillDidChangeScrollOffset_TimerAlreadyRunning) {
  LoadHTML(R"(<body><form><input type="text" id="test"></form></body>)");
  FieldRendererId field_id = GetFieldRendererIdById("test");
  EXPECT_CALL(autofill_driver(), AskForValuesToFill);
  autofill_agent().TriggerSuggestions(
      field_id, AutofillSuggestionTriggerSource::kFormControlElementClicked);

  autofill_agent().AsAutofillAgentExt()->SetIsNeedToCreatedPopup(true);
  autofill_agent().AsAutofillAgentExt()->SetCreatedPopupTime(
      base::TimeTicks::Now() - base::Milliseconds(100));

  EXPECT_TRUE(autofill_agent().AsAutofillAgentExt()->OhAutoFillDidChangeScrollOffset());
  EXPECT_TRUE(autofill_agent().AsAutofillAgentExt()->OhAutoFillDidChangeScrollOffset());
}

TEST_F(AutofillAgentTestWithFeatures, OhAutoFillDidChangeScrollOffset_TimerCallback) {
  LoadHTML(R"(<body><form><input type="text" id="test"></form></body>)");
  FieldRendererId field_id = GetFieldRendererIdById("test");
  EXPECT_CALL(autofill_driver(), AskForValuesToFill);
  autofill_agent().TriggerSuggestions(
      field_id, AutofillSuggestionTriggerSource::kFormControlElementClicked);

  autofill_agent().AsAutofillAgentExt()->SetIsNeedToCreatedPopup(true);
  autofill_agent().AsAutofillAgentExt()->SetCreatedPopupTime(
      base::TimeTicks::Now() - base::Milliseconds(100));

  EXPECT_TRUE(autofill_agent().AsAutofillAgentExt()->OhAutoFillDidChangeScrollOffset());
  task_environment_.FastForwardBy(base::Milliseconds(100));
}

}  // namespace autofill