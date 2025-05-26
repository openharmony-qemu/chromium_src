// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/browser/font_access/font_enumeration_data_source_ohos.h"

#include "base/notreached.h"
#include "base/sequence_checker.h"
#include "base/threading/scoped_blocking_call.h"
#include "ohos/adapter/font/font_adapter.h"
#include "third_party/blink/public/common/font_access/font_enumeration_table.pb.h"

namespace content {
namespace {

std::vector<std::string> GetSystemFonts() {
  base::ScopedBlockingCall scoped_blocking_call(FROM_HERE,
                                                base::BlockingType::MAY_BLOCK);
  return ohos::adapter::font::FontAdapter::GetInstance().GetSystemFontList();
}

ohos::adapter::font::OhosFontInfo GetFontInformation(const std::string& name) {
  base::ScopedBlockingCall scoped_blocking_call(FROM_HERE,
                                                base::BlockingType::MAY_BLOCK);
  return ohos::adapter::font::FontAdapter::GetInstance().GetFontInfo(name);
}
}  // namespace

FontEnumerationDataSourceOHOS::FontEnumerationDataSourceOHOS() {
  DETACH_FROM_SEQUENCE(sequence_checker_);
}

FontEnumerationDataSourceOHOS::~FontEnumerationDataSourceOHOS() {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);
}

blink::FontEnumerationTable FontEnumerationDataSourceOHOS::GetFonts(
    const std::string& locale) {
  DCHECK_CALLED_ON_VALID_SEQUENCE(sequence_checker_);

  blink::FontEnumerationTable font_enumeration_table;

  std::vector<std::string> fonts = GetSystemFonts();
  for (const auto& font : fonts) {
    ohos::adapter::font::OhosFontInfo font_info = GetFontInformation(font);
    blink::FontEnumerationTable_FontData* data =
        font_enumeration_table.add_fonts();
    if (!data) {
      continue;
    }
    data->set_postscript_name(font_info.post_script_name);
    data->set_full_name(font);
    data->set_family(font_info.family);
    data->set_style(font_info.subfamily);
  }

  return font_enumeration_table;
}

}  // namespace content
