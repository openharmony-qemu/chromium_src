/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <fuzzer/FuzzedDataProvider.h>
#include <stddef.h>
#include <stdint.h>

#include <string>
#include <vector>

#include "base/strings/utf_string_conversions.h"
#include "content/browser/renderer_host/clipboard_host_impl.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace OHOS::NWeb;

namespace OHOS {
void ClipboardHostFuzzTest(const uint8_t* data, size_t size) {
  std::u16string read_text;
  ClipboardHostImpl::ReadText(
      ui::ClipboardBuffer::kCopyPaste,
      base::BindLambdaForTesting(
          [&read_text](const std::u16string& value) { read_text = value; }));
  const std::u16string kText = u"text";
  ClipboardHostImpl::WriteText(kText);
}
}  // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  OHOS::ClipboardHostFuzzTest(data, size);
  return 0;
}
