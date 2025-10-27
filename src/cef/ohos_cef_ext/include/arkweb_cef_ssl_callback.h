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

#ifndef CEF_INCLUDE_ARKWEB_CEF_SSL_CALLBACK_H_
#define CEF_INCLUDE_ARKWEB_CEF_SSL_CALLBACK_H_

#include "include/cef_callback.h"

class ArkWebCefSslCallback : public CefCallback {
 public:
  using CefCallback::Cancel;
  
  ///
  /// Handle the result if the user cancelled the url request.
  ///
  /*--cef()--*/
  virtual void Cancel(bool abortLoading) = 0;
};

#endif // CEF_INCLUDE_ARKWEB_CEF_SSL_CALLBACK_H_