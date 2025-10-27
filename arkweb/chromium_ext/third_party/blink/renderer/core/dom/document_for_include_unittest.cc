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

namespace blink {

#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION)
TEST_F(DocumentTest, VideoLoadOpt_IsVideoPrioritySupportedTest) {
    bool supported = GetDocument().isVideoPrioritySupported();
    EXPECT_EQ(supported, false);
}

TEST_F(DocumentTest, VideoLoadOpt_SetVideoPriorityTest) {
    HeapVector<Member<VideoPriority>> videoVec;
    auto* videoPri = MakeGarbageCollected<VideoPriority>();
    videoPri->setId("v_662102_html5_api");
    videoPri->setPriority(1);

    auto* videoPriOther = MakeGarbageCollected<VideoPriority>();
    videoPriOther->setId("v_7a47ff_html5_api");
    videoPriOther->setPriority(2);

    videoVec.push_back(videoPri);
    videoVec.push_back(videoPriOther);

    GetDocument().setVideoPriority(videoVec);
    EXPECT_EQ(videoVec.size(), 2u);
}

TEST_F(DocumentTest, VideoLoadOpt_SetVideoIsPlayingTest) {
    std::string videoId = "v_662102_html5_api";
    bool is_playing = false;
    GetDocument().SetVideoIsPlaying(videoId, is_playing);
}

TEST_F(DocumentTest, VideoLoadOpt_IsUseVideoLoadOptimizationTest) {
    bool supported =  GetDocument().isVideoPrioritySupported();
    bool isVideoOpt = GetDocument().IsUseVideoLoadOptimization();
    EXPECT_EQ(supported, false);
    EXPECT_EQ(isVideoOpt, false);
}
#endif // ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION

}