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

class ExtensionOmniboxEventRouter {
 public:
  ExtensionOmniboxEventRouter(const ExtensionOmniboxEventRouter&) = delete;
  ExtensionOmniboxEventRouter& operator=(const ExtensionOmniboxEventRouter&) =
      delete;

  // The user has just typed the omnibox keyword. This is sent exactly once in
  // a given input session, before any OnInputChanged events.
  static void OnInputStarted(Profile* profile, const ExtensionId& extension_id);

  // The user has changed what is typed into the omnibox while in an extension
  // keyword session. Returns true if someone is listening to this event, and
  // thus we have some degree of confidence we'll get a response.
  static bool OnInputChanged(Profile* profile,
                             const ExtensionId& extension_id,
                             const std::string& input,
                             int suggest_id);

  // The user has accepted the omnibox input.
  static void OnInputEntered(content::WebContents* web_contents,
                             const ExtensionId& extension_id,
                             const std::string& input,
                             WindowOpenDisposition disposition);

  // The user has cleared the keyword, or closed the omnibox popup. This is
  // sent at most once in a give input session, after any OnInputChanged events.
  static void OnInputCancelled(Profile* profile,
                               const ExtensionId& extension_id);

  // The user has deleted an extension omnibox suggestion result.
  static void OnDeleteSuggestion(Profile* profile,
                                 const ExtensionId& extension_id,
                                 const std::string& suggestion_text);

  // The user has changed what is typed into the omnibox while in an extension
  // keyword session. Returns true if someone is listening to this event, and
  // thus we have some degree of confidence we'll get a response.
  static bool OnInputChanged(const std::string& input,
                             const ExtensionId& extension_id,
                             content::BrowserContext* browser_context);

  // The user has accepted the omnibox input.
  static void OnInputEntered(int32_t tab_id,
                             const std::string& input,
                             const ExtensionId& extension_id,
                             WindowOpenDisposition disposition,
                             content::BrowserContext* browser_context);
};
