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

class ContentSettingsContentSettingGetFunction : public ExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("contentSettings.get", CONTENTSETTINGS_GET)
  ~ContentSettingsContentSettingGetFunction() override {}
  ResponseAction Run() override;

 private:
  static void GetCallback(const base::WeakPtr<ContentSettingsContentSettingGetFunction>& function,
                          const NWebExtensionContentSettingsDetail* detailParam, const char* error);
  bool call_get_content_settings_ = false;
  base::WeakPtrFactory<ContentSettingsContentSettingGetFunction> weak_ptr_factory_{this};
};

class ContentSettingsContentSettingSetFunction : public ExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("contentSettings.set", CONTENTSETTINGS_SET)
  ~ContentSettingsContentSettingSetFunction() override {}
  ResponseAction Run() override;

 private:
  static void SetCallback(const base::WeakPtr<ContentSettingsContentSettingSetFunction>& function,
                          const char* error);
  bool call_set_content_settings_ = false;
  base::WeakPtrFactory<ContentSettingsContentSettingSetFunction> weak_ptr_factory_{this};
};

class ContentSettingsContentSettingClearFunction : public ExtensionFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("contentSettings.clear", CONTENTSETTINGS_CLEAR)
  ~ContentSettingsContentSettingClearFunction() override {}
  ResponseAction Run() override;

 private:
  static void ClearCallback(const base::WeakPtr<ContentSettingsContentSettingClearFunction>& function,
                          const char* error);
  bool call_clear_content_settings_ = false;
  base::WeakPtrFactory<ContentSettingsContentSettingClearFunction> weak_ptr_factory_{this};
};