/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef OHOS_ADAPTER_TEST_MOCK_INPUT_METHOD_OHOS_ADAPTER_H_
#define OHOS_ADAPTER_TEST_MOCK_INPUT_METHOD_OHOS_ADAPTER_H_

#include <cstdint>

#include "mock_utils.h"
#include "ohos/adapter/ime_adapter/input_method_ohos_adapter.h"

namespace ohos::adapter {

class IImeAdapter {
 public:
  using Delegate = InputMethodOHOSAdapter::Delegate;

  virtual void AttachTextInput(IMFAdapterTextConfig textConfig,
                               int32_t requestKeyboardReason) = 0;
  virtual void DetachTextInput() = 0;
  virtual void OffListenIME() = 0;
  virtual void UpdateAttribute(IMFAdapterInputAttribute inputAttribute) = 0;
  virtual void ShowTextInput(int32_t requestKeyboardReason) = 0;
  virtual void ExitFullscreenEvent() = 0;
  virtual void NotifyCursorUpdate(const IMFAdapterCursorInfo cursorInfo) = 0;
  virtual void InsertTextCallback(const std::string& text) = 0;
  virtual void DeleteBackCallback(int32_t length) = 0;
  virtual void DeleteForwardCallback(int32_t length) = 0;
  virtual void SendEnterKeyEventCallback() = 0;
  virtual void MoveCursorCallback(const int direction) = 0;
  virtual void Register(Delegate* delegate) = 0;
};

class MockImeAdapter : public IImeAdapter,
                       public SingletonHolder<MockImeAdapter> {
 public:
  MOCK_METHOD2(AttachTextInput, void(IMFAdapterTextConfig, int32_t));
  MOCK_METHOD0(DetachTextInput, void());
  MOCK_METHOD0(OffListenIME, void());
  MOCK_METHOD1(UpdateAttribute, void(IMFAdapterInputAttribute));
  MOCK_METHOD1(ShowTextInput, void(int32_t));
  MOCK_METHOD0(ExitFullscreenEvent, void());
  MOCK_METHOD1(NotifyCursorUpdate, void(const IMFAdapterCursorInfo));
  MOCK_METHOD1(InsertTextCallback, void(const std::string&));
  MOCK_METHOD1(DeleteBackCallback, void(int32_t));
  MOCK_METHOD1(DeleteForwardCallback, void(int32_t));
  MOCK_METHOD0(SendEnterKeyEventCallback, void());
  MOCK_METHOD1(MoveCursorCallback, void(const int));
  MOCK_METHOD1(Register, void(Delegate*));
};
}  // namespace ohos::adapter

#endif
