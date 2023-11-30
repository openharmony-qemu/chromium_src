// Copyright 2022 The Huawei Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/base/clipboard/ohos/clipboard_ohos_read_data.h"

#include <map>

#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/logging.h"
#include "third_party/icu/source/i18n/unicode/regex.h"
#include "url/gurl.h"

using namespace OHOS::NWeb;

namespace ui {
ClipboardOhosReadData::ClipboardOhosReadData(PasteRecordList& record_list)
    : record_list_(record_list) {
  is_in_app_ = OhosAdapterHelper::GetInstance().GetPasteBoard().IsLocalPaste();
  token_id_ = OhosAdapterHelper::GetInstance().GetPasteBoard().GetTokenId();
  html_ = record_list_[0]->GetHtmlText();
  text_ = record_list_[0]->GetPlainText();
}

std::shared_ptr<std::string> ClipboardOhosReadData::ReadHtml() {
  if (!html_) {
    return nullptr;
  }

  if (record_list_.size() > 1 && !is_in_app_ && !has_been_read_html_) {
    has_been_read_html_ = true;
  }
  return html_;
}

ClipboardOhosReadData::~ClipboardOhosReadData() {}

}  // namespace ui
