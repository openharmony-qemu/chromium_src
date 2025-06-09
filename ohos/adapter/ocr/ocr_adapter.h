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

#ifndef OHOS_ADAPTER_OCR_OCR_ADAPTER_H
#define OHOS_ADAPTER_OCR_OCR_ADAPTER_H

#include <cstdint>
#include <memory>
#include <string>

#include "ohos/adapter/aki_hook/aki_hook.h"
#include "ohos/adapter/export.h"
#include "ui/gfx/geometry/vector2d.h"

namespace ohos {
namespace adapter {

struct PixelPoint {
  int x;
  int y;
};

struct TextWord {
  std::string value;
  std::vector<PixelPoint> cornerPoints;
};

struct OcrImage {
  int width = 0;
  int height = 0;
  std::unique_ptr<char[]> buff;
};

struct OcrAdapterImage {
  int width = 0;
  int height = 0;
  aki::ArrayBuffer buff;
};

class ADAPTER_EXPORT_API OcrAdapter {
 public:
  using OnTextRecognizedCallback = std::function<void(std::vector<TextWord>)>;

  static OcrAdapter& GetInstance();
  OcrAdapter(const OcrAdapter&) = delete;
  OcrAdapter(OcrAdapter&&) = delete;
  OcrAdapter& operator=(const OcrAdapter&) = delete;
  virtual ~OcrAdapter() = default;
  std::vector<TextWord> OcrFunction(const OcrImage& image);

 private:
  OcrAdapter();
};

}  // namespace adapter
}  // namespace ohos
#endif
