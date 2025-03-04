// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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

  FontEnumerationDataSourceOHOS(const FontEnumerationDataSourceOHOS&) = delete;
  FontEnumerationDataSourceOHOS& operator=(
      const FontEnumerationDataSourceOHOS&) = delete;
  ~FontEnumerationDataSourceOHOS() override;
  blink::FontEnumerationTable GetFonts(const std::string& locale) override;

 private:
  SEQUENCE_CHECKER(sequence_checker_);
};

}  // namespace content

#endif  // CONTENT_BROWSER_FONT_ACCESS_FONT_ENUMERATION_DATA_SOURCE_OHOS_H_
