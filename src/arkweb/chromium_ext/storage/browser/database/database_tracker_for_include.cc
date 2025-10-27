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

#if BUILDFLAG(IS_ARKWEB)
const std::u16string kBaseDatabaseDir =
    base::UTF8ToUTF16(std::string_view{"/data/storage/el2/base/"});
const std::u16string divisionStr = base::UTF8ToUTF16(std::string_view{"/"});
const std::u16string kSuffixStr = base::UTF8ToUTF16(std::string_view{".db"});
#endif

#if BUILDFLAG(ARKWEB_WEBSTORAGE)
base::FilePath DatabaseTracker::GetFullDBFilePath(
    const std::string& origin_identifier,
    const std::u16string& database_name,
    bool suffix) {
  DCHECK(task_runner_->RunsTasksInCurrentSequence());
  DCHECK(!origin_identifier.empty());
  if (!LazyInit()) {
    return base::FilePath();
  }

  int64_t id =
      databases_table_->GetDatabaseID(origin_identifier, database_name);
  if (id < 0) {
    return base::FilePath();
  }

  return GetOriginDirectory(origin_identifier)
      .AppendASCII((suffix ? base::UTF16ToASCII(database_name + kSuffixStr)
                           : base::UTF16ToASCII(database_name)));
}
#endif  // BUILDFLAG(ARKWEB_WEBSTORAGE)
