/*
 * Copyright (c) 2023-2026 Huawei Device Co., Ltd.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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

#ifndef OHOS_ADAPTER_PRINT_COMMON_H_
#define OHOS_ADAPTER_PRINT_COMMON_H_

#include <cstdint>
#include <string>
#include <vector>

#include "third_party/aki/include/aki/value/array_buffer.h"

namespace ohos::adapter::print {

enum class PrintQuality {
  QUALITY_DRAFT = 3,
  QUALITY_NORMAL = 4,
  QUALITY_HIGH = 5,
};

struct PrintPageSize {
 public:
  std::string id;
  std::string name;
  int32_t width;
  int32_t height;
};

enum class DocFlavor {
  DocFlavor = 0,
  BYTES = 1,
};

enum class PrintColorMode {
  COLOR_MODE_MONOCHROME = 0,
  COLOR_MODE_COLOR = 1,
};

enum class PrintDuplexMode {
  DUPLEX_MODE_NONE = 0,
  DUPLEX_MODE_LONG_EDGE = 1,
  DUPLEX_MODE_SHORT_EDGE = 2,
};

enum class PrintDocumentFormat {
  DOCUMENT_FORMAT_AUTO = 0,
  DOCUMENT_FORMAT_JPEG = 1,
  DOCUMENT_FORMAT_PDF = 2,
  DOCUMENT_FORMAT_POSTSCRIPT = 3,
  DOCUMENT_FORMAT_TEXT = 4,
  DOCUMENT_FORMAT_RAW = 5,
};

struct PrintJobData {
 public:
  std::string printerId;
  std::string jobName;
  int32_t copyNumber;
  bool isLandscape;
  PrintColorMode colorMode;
  PrintDuplexMode duplexMode;
  PrintPageSize pageSize;
  std::string jobId;
  aki::ArrayBuffer binaryData;
  PrintQuality printQuality;
  std::string mediaType;
  bool isBorderless;
  bool isAutoRotate;
  bool isReverse;
  bool isCollate;
  bool isSequential;
  std::string options;
};

}
#endif