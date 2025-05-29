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

#if BUILDFLAG(ARKWEB_WEBSTORAGE)
bool CrackVfsFileNameExt(const std::u16string& vfs_file_name,
                         std::string* origin_identifier,
                         std::u16string* database_name,
                         std::u16string* sqlite_suffix) {
  size_t first_slash_index = vfs_file_name.rfind('/');
  if (first_slash_index == std::u16string::npos) {
    LOG(ERROR) << "DatabaseUtil::CrackVfsFileName not find /";
    return false;
  }
  std::u16string dbnameAndSuffix = vfs_file_name.substr(
      first_slash_index + 1, vfs_file_name.length() - first_slash_index - 1);
  size_t last_pound_index = dbnameAndSuffix.rfind('.');
  if (last_pound_index == std::u16string::npos) {
    LOG(ERROR) << "DatabaseUtil::CrackVfsFileName not find .";
    return false;
  }
  std::u16string suffix = dbnameAndSuffix.substr(
      last_pound_index, dbnameAndSuffix.length() - last_pound_index);
  if (!IsSafeSuffix(suffix)) {
    LOG(ERROR) << "DatabaseUtil::CrackVfsFileName IsSafeSuffix failed";
    return false;
  }
  std::u16string path = vfs_file_name.substr(0, first_slash_index);
  first_slash_index = path.rfind('/');
  if (first_slash_index == std::u16string::npos) {
    LOG(ERROR) << "DatabaseUtil::CrackVfsFileName not find /";
    return false;
  }
  std::u16string name = dbnameAndSuffix.substr(0, last_pound_index);
  std::string origin_id = base::UTF16ToASCII(path.substr(
      first_slash_index + 1, path.length() - first_slash_index - 1));
  if (!IsValidOriginIdentifier(origin_id)) {
    LOG(ERROR)
        << "DatabaseUtil::CrackVfsFileName IsValidOriginIdentifier failed";
    return false;
  }
  if (sqlite_suffix) {
    *sqlite_suffix = suffix;
  }
  if (database_name) {
    *database_name = name;
  }
  if (origin_identifier) {
    *origin_identifier = origin_id;
  }
  return true;
}
#endif  // BUILDFLAG(ARKWEB_WEBSTORAGE)
