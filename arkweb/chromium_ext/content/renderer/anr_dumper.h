/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#ifndef CONTENT_RENDERER_ANR_DUMPER_H
#define CONTENT_RENDERER_ANR_DUMPER_H
#include "base/synchronization/lock.h"
#include "content/common/child_process.mojom.h"

namespace content {

class AnrDumper

{
 public:
  static AnrDumper* GetInstance();
  class InterruptData;
  AnrDumper() = default;
  void DumpCurrentJavaScriptStack(
      mojom::ChildProcess::dumpCurrentJavaScriptStackInMainThreadCallback
          callback);
};
}  // namespace content

#endif  // CONTENT_RENDERER_ANR_DUMPER_H
