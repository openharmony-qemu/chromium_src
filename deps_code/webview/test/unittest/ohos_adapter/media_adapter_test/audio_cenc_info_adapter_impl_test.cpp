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

#include "gtest/gtest.h"
#define private public
#include "audio_cenc_info_adapter_impl.h"
#undef private

using namespace testing;
using namespace testing::ext;
using namespace OHOS::NWeb;

class AudioCencInfoAdapterImplTest : public ::testing::Test {
    protected:
        void SetUp() override {}
        void TearDown() override {}
};

/**
 * @tc.name: GetKeyID_ShouldReturnKeyId_WhenCalled
 * @tc.number: AudioCencInfoAdapterImplTest_001
 * @tc.desc: Test GetKeyId method of AudioCencInfoAdapterImpl class
 */
HWTEST_F(AudioCencInfoAdapterImplTest, GetKeyID_ShouldReturnKeyId_WhenCalled, TestSize.Level0)
{
    AudioCencInfoAdapterImpl audioCencInfoAdapterImpl;
    uint8_t* keyId = (uint8_t*)"keyId";
    audioCencInfoAdapterImpl.SetKeyId(keyId);
    EXPECT_EQ(audioCencInfoAdapterImpl.GetKeyId(), keyId);
}

/**
 * @tc.name: GetKeyIDLen_ShouldReturnKeyIdLen_WhenCalled
 * @tc.number: AudioCencInfoAdapterImplTest_002
 * @tc.desc: Test GetKeyIdLen method of AudioCencInfoAdapterImpl class
 */
HWTEST_F(AudioCencInfoAdapterImplTest, GetKeyIDLen_ShouldReturnKeyIdLen_WhenCalled, TestSize.Level0)
{
    AudioCencInfoAdapterImpl audioCencInfoAdapterImpl;
    uint32_t keyIdLen = 16;
    audioCencInfoAdapterImpl.SetKeyIdLen(keyIdLen);
    EXPECT_EQ(audioCencInfoAdapterImpl.GetKeyIdLen(), keyIdLen);
}

/**
 * @tc.name: GetIv_ShouldReturnIv_WhenCalled
 * @tc.number: AudioCencInfoAdapterImplTest_003
 * @tc.desc: Test GetIv method of AudioCencInfoAdapterImpl class
 */
HWTEST_F(AudioCencInfoAdapterImplTest, GetIv_ShouldReturnIv_WhenCalled, TestSize.Level0)
{
    AudioCencInfoAdapterImpl audioCencInfoAdapterImpl;
    uint8_t* iv = (uint8_t*)"iv";
    audioCencInfoAdapterImpl.SetIv(iv);
    EXPECT_EQ(audioCencInfoAdapterImpl.GetIv(), iv);
}

/**
 * @tc.name: GetIvLen_ShouldReturnIvLen_WhenCalled
 * @tc.number: AudioCencInfoAdapterImplTest_004
 * @tc.desc: Test GetIvLen method of AudioCencInfoAdapterImpl class
 */
HWTEST_F(AudioCencInfoAdapterImplTest, GetIvLen_ShouldReturnIvLen_WhenCalled, TestSize.Level0)
{
    AudioCencInfoAdapterImpl audioCencInfoAdapterImpl;
    uint32_t ivLen = 16;
    audioCencInfoAdapterImpl.SetIvLen(ivLen);
    EXPECT_EQ(audioCencInfoAdapterImpl.GetIvLen(), ivLen);
}

/**
 * @tc.name: GetAlgo_ShouldReturnAlgo_WhenCalled
 * @tc.number: AudioCencInfoAdapterImplTest_005
 * @tc.desc: Test GetAlgo method of AudioCencInfoAdapterImpl class
 */
HWTEST_F(AudioCencInfoAdapterImplTest, GetAlgo_ShouldReturnAlgo_WhenCalled, TestSize.Level0)
{
    AudioCencInfoAdapterImpl audioCencInfoAdapterImpl;
    uint32_t algo = 1;
    audioCencInfoAdapterImpl.SetAlgo(algo);
    EXPECT_EQ(audioCencInfoAdapterImpl.GetAlgo(), algo);
}

/**
 * @tc.name: GetEncryptedBlockCount_ShouldReturnEncryptedBlockCount_WhenCalled
 * @tc.number: AudioCencInfoAdapterImplTest_006
 * @tc.desc: Test GetEncryptedBlockCount method of AudioCencInfoAdapterImpl class
 */
HWTEST_F(AudioCencInfoAdapterImplTest,
         GetEncryptedBlockCount_ShouldReturnEncryptedBlockCount_WhenCalled, TestSize.Level0)
{
    AudioCencInfoAdapterImpl audioCencInfoAdapterImpl;
    uint32_t encryptedBlockCount = 10;
    audioCencInfoAdapterImpl.SetEncryptedBlockCount(encryptedBlockCount);
    EXPECT_EQ(audioCencInfoAdapterImpl.GetEncryptedBlockCount(), encryptedBlockCount);
}

/**
 * @tc.name: GetSkippedBlockCount_ShouldReturnSkippedBlockCount_WhenCalled
 * @tc.number: AudioCencInfoAdapterImplTest_007
 * @tc.desc: Test GetSkippedBlockCount method of AudioCencInfoAdapterImpl class
 */
HWTEST_F(AudioCencInfoAdapterImplTest,
         GetSkippedBlockCount_ShouldReturnSkippedBlockCount_WhenCalled, TestSize.Level0)
{
    AudioCencInfoAdapterImpl audioCencInfoAdapterImpl;
    uint32_t skippedBlockCount = 5;
    audioCencInfoAdapterImpl.SetSkippedBlockCount(skippedBlockCount);
    EXPECT_EQ(audioCencInfoAdapterImpl.GetSkippedBlockCount(), skippedBlockCount);
}

/**
 * @tc.name: GetFirstEncryptedOffset_ShouldReturnFirstEncryptedOffset_WhenCalled
 * @tc.number: AudioCencInfoAdapterImplTest_008
 * @tc.desc: Test GetFirstEncryptedOffset method of AudioCencInfoAdapterImpl class
 */
HWTEST_F(AudioCencInfoAdapterImplTest,
         GetFirstEncryptedOffset_ShouldReturnFirstEncryptedOffset_WhenCalled, TestSize.Level0)
{
    AudioCencInfoAdapterImpl audioCencInfoAdapterImpl;
    uint32_t firstEncryptedOffset = 5;
    audioCencInfoAdapterImpl.SetFirstEncryptedOffset(firstEncryptedOffset);
    EXPECT_EQ(audioCencInfoAdapterImpl.GetFirstEncryptedOffset(), firstEncryptedOffset);
}

/**
 * @tc.name: GetClearHeaderLens_ShouldReturnClearHeaderLens_WhenCalled
 * @tc.number: AudioCencInfoAdapterImplTest_009
 * @tc.desc: Test GetClearHeaderLens method of AudioCencInfoAdapterImpl class
 */
HWTEST_F(AudioCencInfoAdapterImplTest,
         GetClearHeaderLens_ShouldReturnClearHeaderLens_WhenCalled, TestSize.Level0)
{
    AudioCencInfoAdapterImpl audioCencInfoAdapterImpl;
    std::vector<uint32_t> clearHeaderLens = {1, 2, 3};
    audioCencInfoAdapterImpl.SetClearHeaderLens(clearHeaderLens);
    EXPECT_EQ(audioCencInfoAdapterImpl.GetClearHeaderLens(), clearHeaderLens);
}

/**
 * @tc.name: GetPayLoadLens_ShouldReturnPayLoadLens_WhenCalled
 * @tc.number: AudioCencInfoAdapterImplTest_010
 * @tc.desc: Test GetPayLoadLens method of AudioCencInfoAdapterImpl class
 */
HWTEST_F(AudioCencInfoAdapterImplTest,
         GetPayLoadLens_ShouldReturnPayLoadLens_WhenCalled, TestSize.Level0)
{
    AudioCencInfoAdapterImpl audioCencInfoAdapterImpl;
    std::vector<uint32_t> payLoadLens = {4, 5, 6};
    audioCencInfoAdapterImpl.SetPayLoadLens(payLoadLens);
    EXPECT_EQ(audioCencInfoAdapterImpl.GetPayLoadLens(), payLoadLens);
}

/**
 * @tc.name: GetMode_ShouldReturnMode_WhenCalled
 * @tc.number: AudioCencInfoAdapterImplTest_011
 * @tc.desc: Test GetMode method of AudioCencInfoAdapterImpl class
 */
HWTEST_F(AudioCencInfoAdapterImplTest,
         GetMode_ShouldReturnMode_WhenCalled, TestSize.Level0)
{
    AudioCencInfoAdapterImpl audioCencInfoAdapterImpl;
    uint32_t mode = 1;
    audioCencInfoAdapterImpl.SetMode(mode);
    EXPECT_EQ(audioCencInfoAdapterImpl.GetMode(), mode);
}