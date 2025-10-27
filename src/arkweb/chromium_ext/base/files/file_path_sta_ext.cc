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

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
// static
bool FilePath::IsDataShareUrl(const StringType& url) {
  return StartsWith(url, "datashare://",
                    base::CompareCase::INSENSITIVE_ASCII) ||
         StartsWith(url, "dataability://",
                    base::CompareCase::INSENSITIVE_ASCII) ||
         StartsWith(url, "file://media/",
                    base::CompareCase::INSENSITIVE_ASCII) ||
         StartsWith(url, "file://docs/",
                    base::CompareCase::INSENSITIVE_ASCII);
}
#endif