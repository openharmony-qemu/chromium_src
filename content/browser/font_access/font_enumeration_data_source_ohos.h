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

#ifndef CONTENT_BROWSER_FONT_ACCESS_FONT_ENUMERATION_DATA_SOURCE_OHOS_H_
#define CONTENT_BROWSER_FONT_ACCESS_FONT_ENUMERATION_DATA_SOURCE_OHOS_H_

#include <string>

#include "base/sequence_checker.h"
#include "content/browser/font_access/font_enumeration_data_source.h"
#include "third_party/blink/public/common/font_access/font_enumeration_table.pb.h"

namespace content {

// OHOS implementation of FontEnumerationDataSource.
class FontEnumerationDataSourceOHOS : public FontEnumerationDataSource {
 public:
  FontEnumerationDataSourceOHOS();

  FontEnumerationDataSourceOHOS(const FontEnumerationDataSourceOHOS&) =
      delete;
  FontEnumerationDataSourceOHOS& operator=(
      const FontEnumerationDataSourceOHOS&) = delete;
  ~FontEnumerationDataSourceOHOS() override;
  blink::FontEnumerationTable GetFonts(const std::string& locale) override;

 private:
  SEQUENCE_CHECKER(sequence_checker_);
};

}  // namespace content

#endif  // CONTENT_BROWSER_FONT_ACCESS_FONT_ENUMERATION_DATA_SOURCE_OHOS_H_
