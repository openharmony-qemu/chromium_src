// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ohos/adapter/font/font_adapter.h"

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/logging.h"
#include "rapidjson/document.h"

namespace ohos::adapter::font {
namespace {

void ConvertFontInfo(const std::string& json_str,
                     OhosFontInfo& ohos_font_info) {
  if (json_str.empty()) {
    LOGE("[FontAdapter] GetFontInfo ets function json_str empty");
    return;
  }
  rapidjson::Document doc;
  doc.Parse(json_str);
  if (doc.HasParseError()) {
    LOGE("[FontAdapter] GetFontInfo ets function doc HasParseError");
    return;
  }
  ohos_font_info.path = doc["path"].GetString();
  ohos_font_info.post_script_name = doc["postScriptName"].GetString();
  ohos_font_info.full_name = doc["fullName"].GetString();
  ohos_font_info.family = doc["family"].GetString();
  ohos_font_info.subfamily = doc["subfamily"].GetString();
  ohos_font_info.weight = doc["weight"].GetInt();
  ohos_font_info.width = doc["width"].GetInt();
  ohos_font_info.italic = doc["italic"].GetBool();
  ohos_font_info.mono_space = doc["monoSpace"].GetBool();
  ohos_font_info.symbolic = doc["symbolic"].GetBool();
}
}  // namespace

FontAdapter& FontAdapter::GetInstance() {
  static FontAdapter instance;
  return instance;
}

std::vector<std::string> FontAdapter::GetSystemFontList() {
  std::vector<std::string> fonts;
  if (auto func =
          ohos::adapter::GetJSFunction("FontAdapter.GetSystemFontList")) {
    fonts = func->Invoke<std::vector<std::string>>();
  }
  return fonts;
}

std::vector<std::string> FontAdapter::GetConfigFontList() {
  std::vector<std::string> fonts;
  if (auto func =
          ohos::adapter::GetJSFunction("FontAdapter.GetConfigFontList")) {
    fonts = func->Invoke<std::vector<std::string>>();
  }
  return fonts;
}

OhosFontInfo FontAdapter::GetFontInfo(const std::string& name) {
  OhosFontInfo ohos_font_info;
  if (auto func = ohos::adapter::GetJSFunction("FontAdapter.GetFontInfo")) {
    ConvertFontInfo(func->Invoke<std::string>(name), ohos_font_info);
  }
  return ohos_font_info;
}
}  // namespace ohos::adapter::font
