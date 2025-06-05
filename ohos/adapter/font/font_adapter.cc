/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ohos/adapter/font/font_adapter.h"

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/common/logging.h"
#include "rapidjson/document.h"

namespace ohos::adapter::font {
namespace {

void ConvertFontInfo(const std::string& json_str, OhosFontInfo& ohos_font_info) {
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
  if (auto func = ohos::adapter::GetJSFunction("FontAdapter.GetSystemFontList")) {
    fonts = func->Invoke<std::vector<std::string>>();
  }
  return fonts;
}

std::vector<std::string> FontAdapter::GetConfigFontList() {
  std::vector<std::string> fonts;
  if (auto func = ohos::adapter::GetJSFunction("FontAdapter.GetConfigFontList")) {
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
