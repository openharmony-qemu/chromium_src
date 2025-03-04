/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

namespace safe_browsing {
struct ArchiveAnalyzerResults {
  ArchiveAnalyzerResults();
  ArchiveAnalyzerResults(const ArchiveAnalyzerResults& other);
  ~ArchiveAnalyzerResults();
};

ArchiveAnalyzerResults::ArchiveAnalyzerResults() = default;
ArchiveAnalyzerResults::ArchiveAnalyzerResults(
    const ArchiveAnalyzerResults& other) = default;
ArchiveAnalyzerResults::~ArchiveAnalyzerResults() {}
}  // namespace safe_browsing

#include "base/component_export.h"
#include "base/memory/scoped_refptr.h"
#include "base/pickle.h"
#include "ipc/ipc_message_utils.h"
#include "ipc/ipc_mojo_param_traits.h"
#include "ipc/ipc_platform_file.h"

namespace IPC {
template <>
struct ParamTraits<safe_browsing::ArchiveAnalyzerResults> {
  typedef safe_browsing::ArchiveAnalyzerResults param_type;
  static void Write(base::Pickle* m, const param_type& p);
  static bool Read(const base::Pickle* m,
                   base::PickleIterator* iter,
                   param_type* r);
  static void Log(const param_type& p, std::string* l);
};

void ParamTraits<safe_browsing::ArchiveAnalyzerResults>::Write(
    base::Pickle* m,
    const param_type& p) {}

bool ParamTraits<safe_browsing::ArchiveAnalyzerResults>::Read(
    const base::Pickle* m,
    base::PickleIterator* iter,
    param_type* r) {
  return true;
}

void ParamTraits<safe_browsing::ArchiveAnalyzerResults>::Log(
    const param_type& p,
    std::string* l) {}
}  // namespace IPC
                  