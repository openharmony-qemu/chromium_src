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

#ifndef ARKWEB_CHROMIUM_EXT_CHROME_BROWSER_PASSWORD_MANAGER_KEYBOARD_SUPPRESSOR_OHOS_H_
#define ARKWEB_CHROMIUM_EXT_CHROME_BROWSER_PASSWORD_MANAGER_KEYBOARD_SUPPRESSOR_OHOS_H_

#include "base/memory/weak_ptr.h"
#include "base/scoped_observation.h"
#include "base/time/time.h"
#include "base/timer/timer.h"
#include "components/autofill/content/browser/content_autofill_driver_factory.h"

namespace autofill {
class ContentAutofillClient;
}

class KeyboardSuppressorOhos
    : public autofill::ContentAutofillDriverFactory::Observer {
 public:
  KeyboardSuppressorOhos(autofill::ContentAutofillClient* autofill_client,
                         base::TimeDelta timeout);
  KeyboardSuppressorOhos(const KeyboardSuppressorOhos&) = delete;
  KeyboardSuppressorOhos& operator=(const KeyboardSuppressorOhos&) = delete;
  ~KeyboardSuppressorOhos() override;

  // ContentAutofillDriverFactory::Observer:
  void OnContentAutofillDriverFactoryDestroyed(
      autofill::ContentAutofillDriverFactory& factory) override;
  void OnContentAutofillDriverCreated(
      autofill::ContentAutofillDriverFactory& factory,
      autofill::ContentAutofillDriver& driver) override;

  void Suppress(autofill::ContentAutofillDriver* driver);
  void Unsuppress();

 private:
  void InitSuppressor(autofill::ContentAutofillDriver& driver);
  bool isSuppressing() { return suppressed_driver_; }

  base::ScopedObservation<autofill::ContentAutofillDriverFactory,
                          autofill::ContentAutofillDriverFactory::Observer>
      driver_factory_observation_{this};
  bool is_suppressor_inited_ = false;
  bool suppressed_driver_ = false;

  base::OneShotTimer unsuppress_timer_;
  base::TimeDelta timeout_;

  base::WeakPtrFactory<KeyboardSuppressorOhos> weak_ptr_factory_{this};
};

#endif  // ARKWEB_CHROMIUM_EXT_CHROME_BROWSER_PASSWORD_MANAGER_KEYBOARD_SUPPRESSOR_OHOS_H_
