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

#ifndef BEGIN_FRAME_SOURCE_UNITTEST_EXT_H
#define BEGIN_FRAME_SOURCE_UNITTEST_EXT_H

 #define ARKWEB_UNITTESTS_CREATE_BEGIN_FRAME() \
  int64_t now = (base::TimeTicks::Now()).ToInternalValue(); \
  BeginFrameArgs args = CreateBeginFrameArgsForTesting( \
    BEGINFRAME_FROM_HERE, 0, 2, now + 10000, now + 10100, 100)


#define ARKWEB_UNITTESTS_CREATE_BEGIN_FRAME_ONLY() \
  args = CreateBeginFrameArgsForTesting(BEGINFRAME_FROM_HERE, 0, 2, now + 9999, \
                                        now + 10100, 101)

#endif // BEGIN_FRAME_SOURCE_UNITTEST_EXT_H