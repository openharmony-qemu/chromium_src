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

#ifndef EXTENSIONS_BROWSER_EXTENSION_REGISTRY_INFO_MANAGER_TEST_H_
#define EXTENSIONS_BROWSER_EXTENSION_REGISTRY_INFO_MANAGER_TEST_H_

namespace extensions {
std::string GetTypeStr(extensions::MenuItem::Type type);
std::string GetContextStr(extensions::MenuItem::Context context);
std::vector<std::string> ContextListToStrVector(const extensions::MenuItem::ContextList& contextList);
NWebExtensionState UnloadedReasonToState(UnloadedExtensionReason reason);
}

#endif