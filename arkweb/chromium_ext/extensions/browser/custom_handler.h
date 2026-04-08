/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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
 
#ifndef ARKWEB_EXTENSIONS_BROWSER_CUSTOM_HANDLER_H_
#define ARKWEB_EXTENSIONS_BROWSER_CUSTOM_HANDLER_H_
 
#include <string>
 
#include "extensions/common/extension.h"
#include "extensions/common/manifest_handler.h"
 
namespace extensions {
 
struct CustomData : public Extension::ManifestData {
  int store_type_;
 
  static int GetStoreType(const Extension* extension);
 
  static void SetStoreType(int store_type, base::Value::Dict* manifest);
 
  static bool CanBeUpdated(const Extension* extension);
};
 
class CustomHandler : public ManifestHandler {
 public:
  CustomHandler();
  ~CustomHandler() override;
 
  bool Parse(Extension* extension, std::u16string* error) override;
 
 private:
  base::span<const char* const> Keys() const override;
};
 
}  // namespace extensions
 
#endif  // ARKWEB_EXTENSIONS_BROWSER_CUSTOM_HANDLER_H_