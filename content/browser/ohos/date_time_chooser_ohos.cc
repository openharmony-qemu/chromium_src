// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "content/browser/ohos/date_time_chooser_ohos.h"

#include "base/bind.h"
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