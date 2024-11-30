// Copyright 2022 The Huawei Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/base/clipboard/ohos/clipboard_ohos_read_data.h"

#include <map>

#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/logging.h"
#include "third_party/icu/source/i18n/unicode/regex.h"
#include "ui/base/clipboard/clipboard_constants.h"
#include "url/gurl.h"

using namespace OHOS::NWeb;

namespace ui {
const std::string SPAN_STRING_TAG = "openharmony.styled-string";

ClipboardOhosReadData::ClipboardOhosReadData(PasteRecordVector& record_vector)
    : record_vector_(record_vector) {
  is_in_app_ = OhosAdapterHelper::GetInstance().GetPasteBoard().IsLocalPaste();
  token_id_ = OhosAdapterHelper::GetInstance().GetPasteBoard().GetTokenId();
  std::string htmlString;
  std::string textString;

  for (auto& recordVector : record_vector_) {
    if (!recordVector) {
      continue;
    }
    auto pasteCustomData = recordVector->GetCustomData();
    if (pasteCustomData != nullptr && pasteCustomData->find(SPAN_STRING_TAG) != pasteCustomData->end()) {
      std::vector<uint8_t> customData = (*pasteCustomData)[SPAN_STRING_TAG];
      LOG(DEBUG) << "get paste custom data success, the length is " <<  customData.size();
      if (convert_html_callback_ == nullptr) {
        LOG(ERROR) << "the convert_html_callback is null";
      } else {
        std::string htmlStr =
            convert_html_callback_->SpanstringConvertHtml(customData);
        LOG(DEBUG) << "pasteboard spanstring to html success, the length is "
                   << htmlStr.length();
        htmlString.append(htmlStr);
      }
    }
    ReadCustomDataFromRecord(pasteCustomData);
    if (recordVector->GetHtmlText()) {
      htmlString.append(*(recordVector->GetHtmlText()));
    }
    if (recordVector->GetPlainText()) {
      textString.append(*(recordVector->GetPlainText()));
    }
  }
  html_ = std::make_shared<std::string>(htmlString.c_str());
  text_ = std::make_shared<std::string>(textString.c_str());
}

std::shared_ptr<std::string> ClipboardOhosReadData::ReadHtml() {
  if (!html_) {
    return nullptr;
  }

  if (record_vector_.size() > 1 && !is_in_app_ && !has_been_read_html_) {
    has_been_read_html_ = true;
  }
  return html_;
}

std::shared_ptr<OHOS::NWeb::NWebSpanstringConvertHtmlCallback> ClipboardOhosReadData::convert_html_callback_ = nullptr;
// static
void ClipboardOhosReadData::SetConvertHtmlCallback(
    std::shared_ptr<OHOS::NWeb::NWebSpanstringConvertHtmlCallback> callback) {
  convert_html_callback_ = callback;
}

void ClipboardOhosReadData::ReadCustomDataFromRecord(
    const std::shared_ptr<OHOS::NWeb::PasteCustomData>& custom_data) {
  if (!custom_data) {
    return;
  }
  if (auto it = custom_data->find(kMimeTypeOHOSCustomData);
      it != custom_data->end()) {
    custom_datas_.push_back(it->second);
  }
}

ClipboardOhosReadData::~ClipboardOhosReadData() {}

}  // namespace ui
