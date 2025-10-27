/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
 
void DispatchExtensionActionClickedWithCustomArgs(
  content::BrowserContext* context,
  std::string extension_id,
  const NWebExtensionTab* custom_tab);
 
void WebExtensionActionShowPopup(
  content::BrowserContext* context,
  int tab_id,
  std::string extension_id);