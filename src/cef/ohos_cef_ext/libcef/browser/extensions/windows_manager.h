// /*
//  * Copyright (c) 2024 Huawei Device Co., Ltd.
//  * Licensed under the Apache License, Version 2.0 (the "License");
//  * you may not use this file except in compliance with the License.
//  * You may obtain a copy of the License at
//  *
//  *     http://www.apache.org/licenses/LICENSE-2.0
//  *
//  * Unless required by applicable law or agreed to in writing, software
//  * distributed under the License is distributed on an "AS IS" BASIS,
//  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  * See the License for the specific language governing permissions and
//  * limitations under the License.
//  */

// #ifndef CEF_LIBCEF_BROWSER_WINDOWS_MANAGER_H_
// #define CEF_LIBCEF_BROWSER_WINDOWS_MANAGER_H_
// #pragma once

// #include <string>
// #include <mutex>
// #include <memory>

// #include "include/cef_extension_window_handler.h"
// #include "base/memory/raw_ptr.h"

// class CefWindowsManager {
//   public:
//     static CefWindowsManager* GetInstance();

//     void SetWindowHandler(CefExtensionWindowHandler* handler);

//     std::vector<WebExtensionWindow> GetAllWindows(
//       const WebExtensionWindowQueryOptions& queryOptions);
 
//   private:
//     static std::unique_ptr<CefWindowsManager> instance;
//     static std::mutex mtx;

//     raw_ptr<CefExtensionWindowHandler> cef_window_handler_;
// };

// #endif  // CEF_LIBCEF_BROWSER_WINDOWS_MANAGER_H_
