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

#include "ohos/adapter/ocr/ocr_adapter.h"
#include "ohos/adapter/common/logging.h"

namespace ohos {
namespace adapter {

namespace {
const int kImagePixelMap = 4;
const int kWordCornerNum = 4;
}  // namespace

OcrAdapter::OcrAdapter() {}

OcrAdapter& OcrAdapter::GetInstance() {
  static OcrAdapter adapter;
  return adapter;
}

std::vector<TextWord> OcrAdapter::OcrFunction(const OcrImage& image) {
  if (image.width == 0 || image.height == 0) {
    LOGE("OcrAdapter::OcrFunction image is null");
    return {};
  }
  auto func = ohos::adapter::GetJSFunction("OcrAdapter.OcrFunction");
  if (!func) {
    LOGE("OcrAdapter::OcrFunction GetJSFunction OcrAdapter.OcrFunction fail!");
    return {};
  }

  aki::ArrayBuffer arrayBuffer((uint8_t*)image.buff.get(),
                               image.width * image.height * kImagePixelMap);
  OcrAdapterImage ocr_adapter_image = {image.width, image.height, arrayBuffer};
  std::promise<std::vector<TextWord>> ocr_promise;
  std::function<void(aki::Value, int32_t)> callback =
    [&](aki::Value words, int32_t length) -> void {
    std::vector<TextWord> recognizeWords;
    if (!words.IsArray()) {
      ocr_promise.set_value(recognizeWords);
      return;
    }
    for (int32_t i = 0; i < length; i++) {
      TextWord word;
      word.value = words[i]["value"].As<std::string>();
      aki::Value cornerPoints = words[i]["cornerPoints"];
      for (int j = 0; j < kWordCornerNum; j++) {
        PixelPoint point;
        point.x = cornerPoints[j]["x"].As<int>();
        point.y = cornerPoints[j]["y"].As<int>();
        word.cornerPoints.push_back(point);
      }
      recognizeWords.push_back(word);
    }
    ocr_promise.set_value(recognizeWords);
  };
  func->Invoke<void>(callback, ocr_adapter_image);
  std::vector<TextWord> words = ocr_promise.get_future().get();

  return words;
}

JSBIND_CLASS(PixelPoint) {
  JSBIND_PROPERTY(x);
  JSBIND_PROPERTY(y);
}

JSBIND_CLASS(TextWord) {
  JSBIND_PROPERTY(value);
  JSBIND_PROPERTY(cornerPoints);
}

JSBIND_CLASS(OcrAdapterImage) {
  JSBIND_PROPERTY(width);
  JSBIND_PROPERTY(height);
  JSBIND_PROPERTY(buff);
}

}  // namespace adapter
}  // namespace ohos
