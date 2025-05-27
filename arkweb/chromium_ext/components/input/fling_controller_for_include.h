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
 
#ifndef COMPONENTS_INPUT_FLING_CONTROLLER_FOR_INCLUDE_H_
#define COMPONENTS_INPUT_FLING_CONTROLLER_FOR_INCLUDE_H_

#if BUILDFLAG(ARKWEB_REPORT_LOSS_FRAME)
  void DynamicFrameLossEvent(const std::string& sceneId, bool isStart);
#endif
#if BUILDFLAG(ARKWEB_D_VSYNC)
  void SetIsFlingFalse(const bool flag);
#endif
#if BUILDFLAG(IS_ARKWEB)
  void StartWebPageFling();
#endif
#if BUILDFLAG(ARKWEB_SLIDE_LTPO)
  void FlingUpdate(base::TimeTicks current_time);
#endif
#if BUILDFLAG(IS_ARKWEB)
  void StopWebPageFling();
#endif

#endif