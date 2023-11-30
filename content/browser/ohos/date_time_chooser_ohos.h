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

#ifndef CONTENT_BROWSER_ANDROID_DATE_TIME_CHOOSER_ANDROID_H_
#define CONTENT_BROWSER_ANDROID_DATE_TIME_CHOOSER_ANDROID_H_

#include "content/public/browser/web_contents_user_data.h"
#include "mojo/public/cpp/bindings/receiver.h"
#include "third_party/blink/public/mojom/choosers/date_time_chooser.mojom.h"
#include "ui/base/ime/text_input_type.h"
#include "ui/gfx/native_widget_types.h"

namespace content {
class DateTimeChooserOHOS : public blink::mojom::DateTimeChooser,
                            public WebContentsUserData<DateTimeChooserOHOS> {
 public:
  explicit DateTimeChooserOHOS(WebContents* web_contents);

  DateTimeChooserOHOS(const DateTimeChooserOHOS&) = delete;
  DateTimeChooserOHOS& operator=(const DateTimeChooserOHOS&) = delete;

  ~DateTimeChooserOHOS() override;

  void OnDateTimeChooserReceiver(
      mojo::PendingReceiver<blink::mojom::DateTimeChooser> receiver);

  // blink::mojom::DateTimeChooser implementation:
  // Shows the dialog. |value| is the date/time value converted to a
  // number as defined in HTML. (See blink::InputType::parseToNumber())
  void OpenDateTimeDialog(blink::mojom::DateTimeDialogValuePtr value,
                          OpenDateTimeDialogCallback callback) override;

  void CloseDateTimeDialog() override;

  void NotifyResult(bool success, double dialog_value);
  blink::mojom::DateTimeDialogValuePtr& GetDialogValue() {
    return dialog_value_ptr_;
  }

 private:
  friend class content::WebContentsUserData<DateTimeChooserOHOS>;
  void OnDateTimeChooserReceiverConnectionError();

  OpenDateTimeDialogCallback open_date_time_response_callback_;
  blink::mojom::DateTimeDialogValuePtr dialog_value_ptr_;
  mojo::Receiver<blink::mojom::DateTimeChooser> date_time_chooser_receiver_;
  WEB_CONTENTS_USER_DATA_KEY_DECL();
};
}  // namespace content

#endif
