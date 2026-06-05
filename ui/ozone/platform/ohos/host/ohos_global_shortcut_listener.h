/*
 * Copyright (c) 2023-2026 Huawei Device Co., Ltd.
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

#ifndef OHOS_GLOBAL_SHORTCUT_LISTENER_H_
#define OHOS_GLOBAL_SHORTCUT_LISTENER_H_

#include "base/ohos/task_scheduler/task_runner_ohos.h"
#include "ui/ozone/public/platform_global_shortcut_listener.h"

namespace ui {

class OhosGlobalShortcutListener : public PlatformGlobalShortcutListener {
 public:
  explicit OhosGlobalShortcutListener(
      PlatformGlobalShortcutListenerDelegate* delegate);
  OhosGlobalShortcutListener(const OhosGlobalShortcutListener&) = delete;
  OhosGlobalShortcutListener& operator=(const OhosGlobalShortcutListener&) =
      delete;
  ~OhosGlobalShortcutListener() override;

  // Hotkey Callback
  void OnKeyPressed(KeyboardCode key_code,
                    bool is_alt_down,
                    bool is_ctrl_down,
                    bool is_shift_down);

  scoped_refptr<base::SingleThreadTaskRunner> ui_task_runner_;

 private:
  // GlobalShortcutListener implementation.
  void StartListening() override;
  void StopListening() override;
  bool RegisterAccelerator(KeyboardCode key_code,
                           bool is_alt_down,
                           bool is_ctrl_down,
                           bool is_shift_down) override;
  void UnregisterAccelerator(KeyboardCode key_code,
                             bool is_alt_down,
                             bool is_ctrl_down,
                             bool is_shift_down) override;
};

}  // namespace ui

#endif  // OHOS_GLOBAL_SHORTCUT_LISTENER_H_
