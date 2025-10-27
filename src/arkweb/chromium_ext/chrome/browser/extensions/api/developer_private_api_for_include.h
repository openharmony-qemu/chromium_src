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

#ifndef CHROME_BROWSER_EXTENSIONS_API_DEVELOPER_PRIVATE_DEVELOPER_PRIVATE_API_FOR_INCLUDE_H_
#define CHROME_BROWSER_EXTENSIONS_API_DEVELOPER_PRIVATE_DEVELOPER_PRIVATE_API_FOR_INCLUDE_H_

namespace extensions {

namespace api {

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
class DeveloperPrivateOpenUrlFunction : public DeveloperPrivateAPIFunction {
  DECLARE_EXTENSION_FUNCTION("developerPrivate.openUrl",
                             DEVELOPERPRIVATE_OPENURL)
  DeveloperPrivateOpenUrlFunction();

  DeveloperPrivateOpenUrlFunction(const DeveloperPrivateOpenUrlFunction&) =
      delete;
  DeveloperPrivateOpenUrlFunction& operator=(
      const DeveloperPrivateOpenUrlFunction&) = delete;

 private:
  ~DeveloperPrivateOpenUrlFunction() override;

  ResponseAction Run() override;
};
#endif  // ARKWEB_ARKWEB_EXTENSIONS
}  // namespace api

}  // namespace extensions

#endif  // CHROME_BROWSER_EXTENSIONS_API_DEVELOPER_PRIVATE_DEVELOPER_PRIVATE_API_FOR_INCLUDE_H_
