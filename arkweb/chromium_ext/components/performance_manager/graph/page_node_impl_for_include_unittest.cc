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

#include <memory>
#include "components/performance_manager/public/graph/page_node.h"

#include "gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

#define private public
#include "components/performance_manager/graph/page_node_impl.h"

namespace performance_manager {

#if BUILDFLAG(ARKWEB_PERFORMANCE_PERSISTENT_TASK)

class AudioDecrementObserver : public PageNode::ObserverDefaultImpl {
public:
    MOCK_METHOD(void, OnDecrementAudioNum, (const PageNode*), (override));
};

TEST(PageNodeImplForIncludeTest, SetIsMediaPlaying) {
    PageNodeImpl page_node(nullptr, "", GURL(), PagePropertyFlags(), base::TimeTicks::Now());
	page_node.media_playing_num_ = -2;
	page_node.SetIsMediaPlaying(true);
    EXPECT_EQ(page_node.media_playing_num_, -1);
	page_node.media_playing_num_ = 0;
	page_node.SetIsMediaPlaying(false);
	EXPECT_EQ(page_node.media_playing_num_, -1);
}

TEST(PageNodeImplForIncludeTest, is_media_playing) {
    PageNodeImpl page_node(nullptr, "", GURL(), PagePropertyFlags(), base::TimeTicks::Now());
	page_node.media_playing_num_ = -2;
    EXPECT_FALSE(page_node.is_media_playing());
}

TEST(PageNodeImplForIncludeTest, IsMediaPlaying) {
    PageNodeImpl page_node(nullptr, "", GURL(), PagePropertyFlags(), base::TimeTicks::Now());
	page_node.media_playing_num_ = -2;
    EXPECT_FALSE(page_node.IsMediaPlaying());
}
#endif

} // namespace performance_manager