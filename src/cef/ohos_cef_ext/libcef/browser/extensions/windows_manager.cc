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
 
// #include "windows_manager.h"
 
// #include "base/logging.h"
  
// std::unique_ptr<CefWindowsManager> CefWindowsManager::instance = nullptr;
// std::mutex CefWindowsManager::mtx;
 
// // static
// CefWindowsManager* CefWindowsManager::GetInstance() {
//   std::lock_guard<std::mutex> lock(mtx);
//   if (!instance) {
//     instance = std::make_unique<CefWindowsManager>();
//   }
//   return instance.get();
// }

// void CefWindowsManager::SetWindowHandler(CefExtensionWindowHandler* handler) {
//   cef_window_handler_ = handler;
// }

// std::vector<WebExtensionWindow> CefWindowsManager::GetAllWindows(
//       const WebExtensionWindowQueryOptions& queryOptions) {
//   if (!cef_window_handler_) {
//     LOG(ERROR) << "cef window handler is null";
//     return {};
//   }
//   return cef_window_handler_->OnGetAllWindows(queryOptions);
// }