// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef INPUT_METHOD_OHOS_ADAPTER_H_
#define INPUT_METHOD_OHOS_ADAPTER_H_

#include <cstdint>
#include <functional>
#include <mutex>
#include <string>

#include "ohos/adapter/export.h"

namespace ohos {
namespace adapter {
enum IMFAdapterTextInputType {
  NONE_INPUT = -1,
  TEXT = 0,
  MULTILINE,
  NUMBER,
  PHONE,
  DATETIME,
  EMAIL_ADDRESS,
  URL,
  VISIBLE_PASSWORD,
};

struct IMFAdapterCursorInfo {
  double left = 0.0;
  double top = 0.0;
  double width = 0.0;
  double height = 0.0;
};

struct IMFAdapterInputAttribute {
  int32_t inputPattern = 0;
  int32_t enterKeyType = 0;
};

struct IMFAdapterSelectionRange {
  int32_t start = -1;
  int32_t end = -1;
};

struct IMFAdapterTextConfig {
  IMFAdapterInputAttribute inputAttribute = {};
  IMFAdapterCursorInfo cursorInfo = {};
  IMFAdapterSelectionRange range = {};
  uint32_t windowId = -1;
};

enum IMFAdapterEnterKeyType {
  UNSPECIFIED = 0,
  NONE_KEY,
  GO,
  SEARCH,
  SEND,
  NEXT,
  DONE,
  PREVIOUS
};

enum IMFAdapterDirection {
  NONE = 0,
  UP = 1,
  DOWN,
  LEFT,
  RIGHT,
};

class ADAPTER_EXPORT_API InputMethodOHOSAdapter {
 public:
  using InsertTextCallback = std::function<void(const std::string&)>;
  using DeleteBackCallback = std::function<void(int32_t)>;
  using DeleteForwardCallback = std::function<void(int32_t)>;
  using SendEnterKeyEventCallback = std::function<void()>;
  using MoveCursorCallback = std::function<void(int)>;
  static InputMethodOHOSAdapter& GetInstance();
  virtual ~InputMethodOHOSAdapter() = default;
  void AttachTextInput(IMFAdapterTextConfig textConfig);
  void DetachTextInput();
  void UpdateAttribute(IMFAdapterInputAttribute inputAttribute);
  void ShowTextInput();
  InsertTextCallback GetInsertTextCallbcak() { return insertTextCallback_; }
  DeleteBackCallback GetDeleteBackCallbcak() { return deleteBackCallback_; }
  DeleteForwardCallback GetDeleteForwardCallbcak() {
    return deleteForwardCallback_;
  }
  SendEnterKeyEventCallback GetSendEnterKeyEventCallbcak() {
    return sendEnterKeyEventCallback_;
  }
  MoveCursorCallback GetMoveCursorCallbcak() { return moveCursorCallback_; }
  void RegisterSendEnterKeyEventCallback(SendEnterKeyEventCallback callback);
  void RegisterInsertTextCallback(InsertTextCallback callback);
  void RegisterDeleteForwardCallback(DeleteForwardCallback callback);
  void RegisterDeleteBackwardCallback(DeleteBackCallback callback);
  void RegisterMoveCursorCallback(MoveCursorCallback callback);
  void NotifyCursorUpdate(const IMFAdapterCursorInfo cursorInfo);

 private:
  std::mutex mutex_;
  InsertTextCallback insertTextCallback_;
  DeleteBackCallback deleteBackCallback_;
  DeleteForwardCallback deleteForwardCallback_;
  SendEnterKeyEventCallback sendEnterKeyEventCallback_;
  MoveCursorCallback moveCursorCallback_;
};
}  // namespace adapter
}  // namespace ohos
#endif  // INPUT_METHOD_OHOS_ADAPTER_H_
