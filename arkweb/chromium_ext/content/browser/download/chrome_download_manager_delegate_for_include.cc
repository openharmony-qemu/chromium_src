/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

void ChromeDownloadManagerDelegate::SafeBrowsingCallback(
    const base::FilePath& suggested_path,
    content::SavePackagePathPickedCallback callback) {
  content::SavePackagePathPickedParams params;
  params.file_path = suggested_path;
  std::move(callback).Run(
      std::move(params),
      base::BindOnce(&ChromeDownloadManagerDelegate::DisableSafeBrowsing));
}
