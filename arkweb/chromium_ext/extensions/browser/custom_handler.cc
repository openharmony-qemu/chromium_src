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
 
#include "custom_handler.h"
 
#include "base/logging.h"
#include "extensions/common/extension_urls.h"
#include "extensions/common/manifest_constants.h"
 
namespace extensions {
 
namespace {
 
inline constexpr char kWebStoreType[] = "webstore_type";
 
}  // namespace
 
// static
int CustomData::GetStoreType(const Extension* extension) {
  if (!extension) {
    return kWebStoreTypeDefault;
  }
 
  CustomData* data = static_cast<CustomData*>(
      extension->GetManifestData(manifest_keys::kCustomData));
  return data ? data->store_type : kWebStoreTypeDefault;
}
 
// static
void CustomData::SetStoreType(int store_type, base::Value::Dict* manifest) {
  if (!manifest) {
    return;
  }
 
  LOG(INFO) << "store type is " << store_type;
  manifest->Set(manifest_keys::kCustomData,
                base::Value::Dict().Set(kWebStoreType, store_type));
}
 
// static
bool CustomData::CanBeUpdated(const Extension* extension) {
  return GetStoreType(extension) != kWebStoreTypeDefault;
}
 
CustomHandler::CustomHandler() = default;
 
CustomHandler::~CustomHandler() = default;
 
bool CustomHandler::Parse(Extension* extension, std::u16string* error) {
  auto data = std::make_unique<CustomData>();
  const base::Value::Dict* value =
      extension->manifest()->FindDictPath(kWebStoreType);
  if (!value) {
    data->store_type = kWebStoreTypeDefault;
  } else {
    auto store_type = value->FindInt(kWebStoreType);
    data->store_type = store_type.value_or(kWebStoreTypeDefault);
  }
 
  extension->SetManifestData(manifest_keys::kCustomData, std::move(data));
  return true;
}
 
base::span<const char* const> CustomHandler::Keys() const {
  static constexpr const char* kKeys[] = {manifest_keys::kCustomData};
  return kKeys;
}
 
}  // namespace extensions