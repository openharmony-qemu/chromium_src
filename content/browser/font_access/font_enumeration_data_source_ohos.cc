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
