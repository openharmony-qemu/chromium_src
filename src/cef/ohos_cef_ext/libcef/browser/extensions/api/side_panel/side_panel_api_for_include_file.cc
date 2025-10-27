/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

class SidePanelSetOptionsFunction : public SidePanelApiFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("sidePanel.setOptions", SIDEPANEL_SETOPTIONS)
  SidePanelSetOptionsFunction() = default;
  SidePanelSetOptionsFunction(const SidePanelSetOptionsFunction&) = delete;
  SidePanelSetOptionsFunction& operator=(const SidePanelSetOptionsFunction&) =
      delete;
 
 private:
  ~SidePanelSetOptionsFunction() override = default;
  ResponseAction RunFunction() override;
  void RunFunctionForInclude(std::optional<api::side_panel::SetOptions::Params>& params);
};
 
class SidePanelOpenFunction : public SidePanelApiFunction {
 public:
  DECLARE_EXTENSION_FUNCTION("sidePanel.open", SIDEPANEL_OPEN)
  SidePanelOpenFunction() = default;
  SidePanelOpenFunction(const SidePanelOpenFunction&) = delete;
  SidePanelOpenFunction& operator=(const SidePanelOpenFunction&) = delete;
 
 private:
  ~SidePanelOpenFunction() override = default;
  ResponseAction RunFunction() override;
  ResponseAction RunOpenFunctionForInclude(std::optional<api::side_panel::Open::Params>& params);
  static void OnOpen(const base::WeakPtr<SidePanelOpenFunction>& function,
                     const std::optional<std::string>& error);
  bool call_on_open_ = false;
  base::WeakPtrFactory<SidePanelOpenFunction> weak_ptr_factory_{this};
};
