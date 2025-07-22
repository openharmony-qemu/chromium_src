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

#include "ohos_nweb/src/cef_delegate/nweb_touch_handle_state_impl.h"

#include <fuzzer/FuzzedDataProvider.h>

#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "ohos_nweb/include/nweb.h"
#include "ohos_nweb/src/nweb_impl.h"

using namespace OHOS::NWeb;

void NWebTouchHandleStateImplFuzzTest(FuzzedDataProvider* fdp) {
  CefTouchHandleState state;
  state.touch_handle_id = fdp->ConsumeIntegralInRange<int>(0, 100);
  state.origin.x = fdp->ConsumeIntegralInRange<int>(0, 100);
  state.origin.y = fdp->ConsumeIntegralInRange<int>(0, 100);
  state.view_port.x = fdp->ConsumeIntegralInRange<int>(0, 100);
  state.view_port.y = fdp->ConsumeIntegralInRange<int>(0, 100);
  state.edge_height = fdp->ConsumeIntegralInRange<int>(0, 100);
  state.enabled = fdp->ConsumeBool();
  state.alpha = fdp->ConsumeIntegralInRange<int>(0, 100);
  cef_horizontal_alignment_t array[] = {CEF_HORIZONTAL_ALIGNMENT_LEFT,
                                        CEF_HORIZONTAL_ALIGNMENT_CENTER,
                                        CEF_HORIZONTAL_ALIGNMENT_RIGHT};
  state.orientation = fdp->PickValueInArray<cef_horizontal_alignment_t>(array);
  NWebTouchHandleStateImpl impl(state);

  impl.GetTouchHandleId();

  impl.GetX();

  impl.GetY();

  impl.GetViewPortX();

  impl.GetViewPortY();

  impl.GetTouchHandleType();

  impl.IsEnable();

  impl.GetAlpha();

  impl.GetEdgeHeight();
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (data == nullptr || size == 0) {
    return 0;
  }

  FuzzedDataProvider fdp(data, size);

  NWebTouchHandleStateImplFuzzTest(&fdp);

  return 0;
}
