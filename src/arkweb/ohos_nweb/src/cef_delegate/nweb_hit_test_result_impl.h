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

#ifndef WEB_HIT_TEST_RESULT_IMPL_H
#define WEB_HIT_TEST_RESULT_IMPL_H

#include "nweb_hit_testresult.h"

namespace OHOS::NWeb {

class HitTestResultImpl : public HitTestResult {
 public:
  HitTestResultImpl() = default;
  ~HitTestResultImpl() = default;

  void SetType(int type) override;

  int GetType() override;

  void SetExtra(std::string extra);

  std::string GetExtra() override;

 private:
  int type_ = 0;
  std::string extra_;
};

}  // namespace OHOS::NWeb

#endif  // WEB_HIT_TEST_RESULT_IMPL_H
