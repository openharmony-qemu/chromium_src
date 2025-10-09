// Copyright 2013 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_AUTOFILL_CONTENT_RENDERER_TEST_PASSWORD_AUTOFILL_AGENT_H_
#define COMPONENTS_AUTOFILL_CONTENT_RENDERER_TEST_PASSWORD_AUTOFILL_AGENT_H_

#include "components/autofill/content/renderer/password_autofill_agent.h"

namespace autofill {
#if BUILDFLAG(ARKWEB_TEST)
class TestPasswordAutofillAgent : public PasswordAutofillAgentExt {
#else
class TestPasswordAutofillAgent : public PasswordAutofillAgent {
#endif
  public:
  TestPasswordAutofillAgent(content::RenderFrame* render_frame,
                            blink::AssociatedInterfaceRegistry* registry);
  ~TestPasswordAutofillAgent() override;

 private:
  // Always returns true. This allows browser tests with "data: " URL scheme to
  // work with the password manager.
  // PasswordAutofillAgent:
  bool FrameCanAccessPasswordManager() override;
};

}  // namespace autofill

#endif  // COMPONENTS_AUTOFILL_CONTENT_RENDERER_TEST_PASSWORD_AUTOFILL_AGENT_H_
