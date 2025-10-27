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

#ifndef OHOS_NWEB_TEST_OHOS_NWEB_TEST_SUITE_H_
#define OHOS_NWEB_TEST_OHOS_NWEB_TEST_SUITE_H_

#include <memory>

#include "base/test/test_discardable_memory_allocator.h"
#include "base/test/test_suite.h"

namespace base {
namespace test {
class TaskEnvironment;
}
}  // namespace base

namespace OHOS::NWeb {

class OhosNwebTestSuite : public base::TestSuite {
 public:
  OhosNwebTestSuite(int argc, char** argv);

  OhosNwebTestSuite(const OhosNwebTestSuite&) = delete;
  OhosNwebTestSuite& operator=(const OhosNwebTestSuite&) = delete;

  ~OhosNwebTestSuite() override;

  static void RunUntilIdle();

 protected:
  void Initialize() override;
  void Shutdown() override;

 private:
  static std::unique_ptr<base::test::TaskEnvironment> task_environment_;

  base::TestDiscardableMemoryAllocator discardable_memory_allocator_;
};

}  // namespace OHOS::NWeb

#endif  // OHOS_NWEB_TEST_OHOS_NWEB_TEST_SUITE_H_
