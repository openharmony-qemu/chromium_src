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

namespace extensions {

void CreateAliasName(v8::Local<v8::Context> context,
                       v8::Local<v8::Object> object,
                       std::string alias_name) {
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  v8::Local<v8::String> alias_string =
      gin::StringToSymbol(context->GetIsolate(), alias_name);
  v8::Local<v8::Value> alias_value;

  if (!context->Global()->Get(context, alias_string).ToLocal(&alias_value)) {
    LOG(ERROR) << "extension alias name " << alias_name << " get failed";
    return;
  }
  if (alias_value->IsUndefined()) {
    v8::Maybe<bool> success = context->Global()->CreateDataProperty(
        context, alias_string, object);
    if (!success.IsJust() || !success.FromJust()) {
        LOG(ERROR) << "extension alias name '" << alias_name
                   << "' create failed";
        return;
    }
    LOG(INFO) << "extension alias name " << alias_name << " create succeeded";
  }
#endif
}
    
} // namespace extensions 