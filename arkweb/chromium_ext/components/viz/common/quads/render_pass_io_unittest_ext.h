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

#ifndef RENDER_PASS_IO_UNITTEST_EXT_H
#define RENDER_PASS_IO_UNITTEST_EXT_H

 #define ARKWEB_UNITTESTS_ASSERT_TRUE() \
   ASSERT_TRUE(base::PathService::Get(Paths::DIR_TEST_DATA, &test_data_dir) \
                  ? true \
                  : base::GetCurrentDirectory(&test_data_dir))

#endif // RENDER_PASS_IO_UNITTEST_EXT_H