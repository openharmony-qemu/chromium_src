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

#if BUILDFLAG(ARKWEB_JSPROXY)
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/loader/frame_loader.h"
#endif

namespace blink {

#if BUILDFLAG(ARKWEB_JSPROXY)
void HTMLHeadElement::RunScriptsAtHeadElementAvailable() {
  // When parsing a fragment, its dummy document has a null parser.
  if (!GetDocument().Parser()) {
    return;
  }

  if (GetDocument().GetFrame()) {
    GetDocument().GetFrame()->Loader().RunScriptsAtHeadElementAvailable();
  }
}
#endif

}  // namespace blink
