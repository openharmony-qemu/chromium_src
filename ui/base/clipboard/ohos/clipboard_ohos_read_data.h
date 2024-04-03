// Copyright 2022 The Huawei Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_BASE_CLIPBOARD_CLIPBOARD_OHOS_READ_DATA_H_
#define UI_BASE_CLIPBOARD_CLIPBOARD_OHOS_READ_DATA_H_

#include "base/files/file_util.h"
#include "ui/base/clipboard/clipboard.h"

#include "ohos_adapter_helper.h"

#include <unordered_map>

namespace ui {
using FileUrlMap = std::unordered_map<std::string, base::File>;
class ClipboardOhosReadData {
 public:
  ClipboardOhosReadData(OHOS::NWeb::PasteRecordVector& record_vector);

  std::unordered_map<std::string, base::File>& GetFileMap() {
    return file_map_;
  }

  OHOS::NWeb::PasteRecordVector& GetPasteRecordVector() { return record_vector_; }
  uint32_t GetTokenId() const { return token_id_; }
  bool IsLocalPaste() const { return is_in_app_; }
  size_t GetRecordVectorSize() const { return record_vector_.size(); }
  std::shared_ptr<std::string> ReadHtml();
  std::shared_ptr<std::string> ReadText() { return text_; }
  ~ClipboardOhosReadData();

 private:
  void ReplaceHTMLImgUriList(std::string& html);
  void SaveImgFile(const std::string& old_uri,
                   std::string& new_uri,
                   uint32_t token_id);

  OHOS::NWeb::PasteRecordVector record_vector_;
  bool has_been_read_html_ = false;
  FileUrlMap file_map_;
  uint32_t token_id_ = 0;
  bool is_in_app_ = false;
  std::shared_ptr<std::string> html_ = nullptr;
  std::shared_ptr<std::string> text_ = nullptr;
};
}  // namespace ui

#endif
