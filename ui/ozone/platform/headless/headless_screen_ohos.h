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

#ifndef UI_OZONE_PLATFORM_HEADLESS_HEADLESS_SCREEN_OHOS_H_
#define UI_OZONE_PLATFORM_HEADLESS_HEADLESS_SCREEN_OHOS_H_

#include <vector>

#include "base/memory/weak_ptr.h"
#include "base/task/single_thread_task_runner.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/display_manager_adapter.h"
#include "ui/display/display_list.h"
#include "ui/ozone/platform/headless/headless_screen.h"

namespace ui {

class HeadlessScreenOhos;

class HeadlessScreenListener : public OHOS::NWeb::DisplayListenerAdapter {
 public:
  explicit HeadlessScreenListener(
      base::WeakPtr<HeadlessScreenOhos> headless_screen_ohos);
  virtual ~HeadlessScreenListener() = default;
  HeadlessScreenListener(const HeadlessScreenListener&) = delete;
  HeadlessScreenListener& operator=(const HeadlessScreenListener&) = delete;

  // Overridden implementation from OHOS::NWeb::DisplayListenerAdapter
  void OnCreate(OHOS::NWeb::DisplayId display_id) override;
  void OnDestroy(OHOS::NWeb::DisplayId display_id) override;
  void OnChange(OHOS::NWeb::DisplayId display_id) override;

  base::WeakPtr<HeadlessScreenOhos> headless_screen_ohos_;
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
};

class HeadlessScreenOhos : public HeadlessScreen {
 public:
  HeadlessScreenOhos();
  ~HeadlessScreenOhos() override;
  HeadlessScreenOhos(const HeadlessScreenOhos&) = delete;
  HeadlessScreenOhos& operator=(const HeadlessScreenOhos&) = delete;

  void OnDisplayCreate(const OHOS::NWeb::DisplayId display_id);
  void OnDisplayDestroy(const OHOS::NWeb::DisplayId display_id);
  void OnDisplayChange(const OHOS::NWeb::DisplayId display_id);

  // Overridden implementation from HeadlessScreen
  const std::vector<display::Display>& GetAllDisplays() const override;
  display::Display GetPrimaryDisplay() const override;
  void AddObserver(display::DisplayObserver* observer) override;
  void RemoveObserver(display::DisplayObserver* observer) override;

 private:
  bool Initialize();
  bool FetchDisplays(display::DisplayList& displays);
  bool ConvertDisplay(std::shared_ptr<OHOS::NWeb::DisplayAdapter> src_display,
                      display::Display& dst_display);
  void OnDisplayEvent(const std::string& event, const int64_t display_id);

 private:
  display::DisplayList display_list_;
  OHOS::NWeb::ListenerId display_listener_id_;
  std::shared_ptr<HeadlessScreenListener> display_listener_;
  std::unique_ptr<OHOS::NWeb::DisplayManagerAdapter> display_manager_adapter_;

  base::WeakPtrFactory<HeadlessScreenOhos> weak_factory_{this};
};
}  // namespace ui

#endif  // UI_OZONE_PLATFORM_HEADLESS_HEADLESS_SCREEN_OHOS_H_
