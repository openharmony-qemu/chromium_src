// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_ADAPTER_PASTEBOARD_PASTEBOARD_ADAPTER_H_
#define OHOS_ADAPTER_PASTEBOARD_PASTEBOARD_ADAPTER_H_

#include <stdlib.h>

#include <string>
#include <vector>

#include "ohos/adapter/export.h"

namespace ohos {
namespace adapter {

enum class ClipBoardImageColorType {
  COLOR_TYPE_UNKNOWN = 0,
  COLOR_TYPE_RGB_565,
  COLOR_TYPE_RGB_888,
  COLOR_TYPE_RGBA_8888,
  COLOR_TYPE_BGRA_8888
};

enum class ClipBoardImageAlphaType {
  ALPHA_TYPE_UNKNOWN = 0,
  ALPHA_TYPE_OPAQUE,
  ALPHA_TYPE_PREMULTIPLIED,
  ALPHA_TYPE_UNPREMUL
};

struct ClipBoardImageData {
  void ReadImage();

  ClipBoardImageColorType color_type =
      ClipBoardImageColorType::COLOR_TYPE_UNKNOWN;
  ClipBoardImageAlphaType alpha_type =
      ClipBoardImageAlphaType::ALPHA_TYPE_UNKNOWN;
  std::unique_ptr<uint8_t> data;
  size_t data_size = 0;
  size_t row_bytes = 0;
  int32_t width = 0;
  int32_t height = 0;
};

struct PasteDataRecord {
  std::string html_text;
  std::string mime_type = "text/html";
  std::string plain_text;
  std::string bookmark_data = "";
};

class ADAPTER_EXPORT_API PasteBoardAdapter {
 public:
  static PasteBoardAdapter& GetInstance();
  std::string ReadText();
  std::string ReadHTML();
  std::vector<std::string> ReadFilenames();
  void Clear();
  bool IsExistMimeType(const std::string& type);
  ClipBoardImageData ReadImage();
  void WriteImage(uint8_t* data,
                  int32_t height,
                  int32_t width,
                  ClipBoardImageColorType pixel_format,
                  ClipBoardImageAlphaType alpha_type);
  void WriteData(const std::string& format,
                 const char* data_data,
                 size_t data_len);
  std::string ReadData(const std::string& type);

  void SetPasteData(PasteDataRecord record);

 private:
  int GetColorChannelNum(ClipBoardImageColorType pixel_format);

  PasteBoardAdapter() = default;
  virtual ~PasteBoardAdapter() = default;
  PasteBoardAdapter(const PasteBoardAdapter&) = delete;
  PasteBoardAdapter& operator=(const PasteBoardAdapter&) = delete;
};
}  // namespace adapter
}  // namespace ohos
#endif  // OHOS_ADAPTER_PASTEBOARD_PASTEBOARD_ADAPTER_H_
