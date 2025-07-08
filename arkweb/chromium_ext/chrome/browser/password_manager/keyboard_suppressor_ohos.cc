/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "chrome/browser/password_manager/keyboard_suppressor_ohos.h"

#include "components/autofill/content/browser/content_autofill_client.h"
#include "components/autofill/content/browser/content_autofill_driver.h"
#include "content/public/browser/render_widget_host.h"

KeyboardSuppressorOhos::KeyboardSuppressorOhos(
    autofill::ContentAutofillClient* autofill_client,
    base::TimeDelta timeout)
    : timeout_(timeout) {
  if (autofill_client) {
    driver_factory_observation_.Observe(
        &autofill_client->GetAutofillDriverFactory());
  }
}

KeyboardSuppressorOhos::~KeyboardSuppressorOhos() {
  Unsuppress();
}

void KeyboardSuppressorOhos::OnContentAutofillDriverFactoryDestroyed(
    autofill::ContentAutofillDriverFactory& factory) {
  Unsuppress();
  driver_factory_observation_.Reset();
}

void KeyboardSuppressorOhos::OnContentAutofillDriverCreated(
    autofill::ContentAutofillDriverFactory& factory,
    autofill::ContentAutofillDriver& driver) {
  InitSuppressor(driver);
}

void KeyboardSuppressorOhos::InitSuppressor(
    autofill::ContentAutofillDriver& driver) {
  content::RenderFrameHost* rfh = driver.render_frame_host();
  if (!rfh) {
    LOG(ERROR) << "rfh is nullptr";
    return;
  }
  content::RenderWidgetHost* rwh = rfh->GetRenderWidgetHost();
  if (!rwh) {
    LOG(ERROR) << "rwh is nullptr";
    return;
  }
  rwh->AddSuppressShowingImeCallback(base::BindRepeating(
      [](base::WeakPtr<KeyboardSuppressorOhos> self) {
        if (self && self->isSuppressing()) {
          LOG(INFO) << "The soft keyboard has been suppressed";
          return true;
        }
        return false;
      },
      weak_ptr_factory_.GetWeakPtr()));
  is_suppressor_inited_ = true;
}

void KeyboardSuppressorOhos::Suppress(autofill::ContentAutofillDriver* driver) {
  if (!driver) {
    LOG(ERROR) << "autofill_driver is nullptr";
    return;
  }
  if (!is_suppressor_inited_) {
    InitSuppressor(*driver);
  }

  Unsuppress();
  LOG(INFO) << "Set the soft keyboard to be suppressd";
  suppressed_driver_ = true;
  unsuppress_timer_.Start(FROM_HERE, timeout_, this,
                          &KeyboardSuppressorOhos::Unsuppress);
}

void KeyboardSuppressorOhos::Unsuppress() {
  if (!suppressed_driver_) {
    return;
  }
  LOG(INFO) << "Restore the soft keyboard to be unsuppressd";
  suppressed_driver_ = false;
  unsuppress_timer_.Stop();
}
