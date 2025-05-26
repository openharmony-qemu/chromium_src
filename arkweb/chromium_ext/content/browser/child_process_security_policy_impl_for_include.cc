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

// Determine if the certain permissions have been granted to a datashare URI.
bool HasPermissionsForDatashareUri(const base::FilePath& file,
                                   int permissions) {
  DCHECK(!file.empty());
  DCHECK(file.IsDataShareUri());
  if (!permissions) {
    return false;
  }
  base::FilePath file_path = file.StripTrailingSeparators();
  FileMap::const_iterator it = file_permissions_.find(file_path);
  if (it != file_permissions_.end()) {
    return (it->second & permissions) == permissions;
  }
  return false;
}
