/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "content/browser/ohos/date_time_chooser_ohos.h"

#include "base/functional/bind.h"
#include "base/logging.h"

namespace content {
DateTimeChooserOHOS::DateTimeChooserOHOS(WebContents* web_contents)
    : WebContentsUserData<DateTimeChooserOHOS>(*web_contents),
      date_time_chooser_receiver_(this) {}

DateTimeChooserOHOS::~DateTimeChooserOHOS() {}

void DateTimeChooserOHOS::OnDateTimeChooserReceiver(
    mojo::PendingReceiver<blink::mojom::DateTimeChooser> receiver) {
  // Disconnect the previous picker first.
  date_time_chooser_receiver_.reset();
  date_time_chooser_receiver_.Bind(std::move(receiver));
  date_time_chooser_receiver_.set_disconnect_handler(base::BindOnce(
      &DateTimeChooserOHOS::OnDateTimeChooserReceiverConnectionError,
      base::Unretained(this)));
}

void DateTimeChooserOHOS::OpenDateTimeDialog(
    blink::mojom::DateTimeDialogValuePtr value,
    OpenDateTimeDialogCallback callback) {
  if (open_date_time_response_callback_) {
    date_time_chooser_receiver_.ReportBadMessage(
        "DateTimeChooserOHOS: Previous picker's binding isn't closed.");
    return;
  }
  open_date_time_response_callback_ = std::move(callback);
  dialog_value_ptr_ = std::move(value);

  GetWebContents().OpenDateTimeChooser();
}

void DateTimeChooserOHOS::NotifyResult(bool success, double dialog_value) {
  std::move(open_date_time_response_callback_).Run(success, dialog_value);
}

void DateTimeChooserOHOS::CloseDateTimeDialog() {
  GetWebContents().CloseDateTimeChooser();
}

void DateTimeChooserOHOS::OnDateTimeChooserReceiverConnectionError() {
  // Close a dialog and reset the Mojo receiver and the callback.
  CloseDateTimeDialog();
  open_date_time_response_callback_.Reset();
  date_time_chooser_receiver_.reset();
}

WEB_CONTENTS_USER_DATA_KEY_IMPL(DateTimeChooserOHOS);
}  // namespace content
