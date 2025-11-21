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

#include <bundle/native_interface_bundle.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "base/logging.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "arkweb/ohos_adapter_ndk/mock_ndk_api/include/mock_ndk_api.h"
#define private public
#include "media_avsession_adapter_impl.h"
#undef private

using namespace testing;
using namespace MockNdkApi;
using namespace OHOS::NWeb;

class MediaAVSessionMetadataAdapterMock : public MediaAVSessionMetadataAdapter {
 public:
  MediaAVSessionMetadataAdapterMock() = default;
  ~MediaAVSessionMetadataAdapterMock() = default;

  MOCK_METHOD1(SetTitle, void(const std::string& title));
  MOCK_METHOD0(GetTitle, std::string());
  MOCK_METHOD1(SetArtist, void(const std::string& artist));
  MOCK_METHOD0(GetArtist, std::string());
  MOCK_METHOD1(SetAlbum, void(const std::string& album));
  MOCK_METHOD0(GetAlbum, std::string());
};

class MediaAVSessionPositionAdapterMock : public MediaAVSessionPositionAdapter {
 public:
  MOCK_METHOD0(GetDuration, int64_t());
  MOCK_METHOD1(SetDuration, void(int64_t duration));
  MOCK_METHOD0(GetElapsedTime, int64_t());
  MOCK_METHOD1(SetElapsedTime, void(int64_t elapsedTime));
  MOCK_METHOD0(GetUpdateTime, int64_t());
  MOCK_METHOD1(SetUpdateTime, void(int64_t updateTime));
};

class MediaAVSessionCallbackAdapterMock : public MediaAVSessionCallbackAdapter {
 public:
  MediaAVSessionCallbackAdapterMock() = default;
  MOCK_METHOD(void, Play, (), (override));
  MOCK_METHOD(void, Pause, (), (override));
  MOCK_METHOD(void, Stop, (), (override));
  MOCK_METHOD(void, SeekTo, (int64_t), (override));
};

class MediaAVSessionAdapterImplTest : public testing::Test {
 public:
  MediaAVSessionAdapterImplTest() = default;
  ~MediaAVSessionAdapterImplTest() = default;
  void TearDown() override {
    SetAllMockType(false);
  }
  void SetAllMockType(bool type) {
    OhosInterfaceMock::bNativeBundleGetMainElementName = type;
    OhosInterfaceMock::bAVMetadataBuilderCreate = type;
    OhosInterfaceMock::bAVMetadataBuilderSetAssetId = type;
    OhosInterfaceMock::bAVMetadataBuilderGenerateAVMetadata = type;
    OhosInterfaceMock::bAVMetadataDestroy = type;
    OhosInterfaceMock::bAVMetadataBuilderDestroy = type;
    OhosInterfaceMock::bAVSessionCreate = type;
    OhosInterfaceMock::bAVSessionDestroy = type;
    OhosInterfaceMock::bAVSessionActivate = type;
    OhosInterfaceMock::bAVSessionDeactivate = type;
    OhosInterfaceMock::bAVSessionRegisterCommandCallback = type;
    OhosInterfaceMock::bAVSessionRegisterSeekCallback = type;
    OhosInterfaceMock::bAVMetadataBuilderSetTitle = type;
    OhosInterfaceMock::bAVMetadataBuilderSetArtist = type;
    OhosInterfaceMock::bAVMetadataBuilderSetAlbum = type;
    OhosInterfaceMock::bAVMetadataBuilderSetDuration = type;
    OhosInterfaceMock::bAVMetadataBuilderSetMediaImageUri = type;
    OhosInterfaceMock::bAVSessionSetPlaybackState = type;
    OhosInterfaceMock::bAVSessionSetAVMetadata = type;
    OhosInterfaceMock::bAVSessionSetPlaybackPosition = type;
  }
};

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionKeyInitTest) {
  SetAllMockType(true);
  MediaAVSessionKey key;
  static OH_NativeBundle_ElementName info;
  info.bundleName = strdup("com.xxx.hmxx");
  info.moduleName = strdup("entry");
  info.abilityName = strdup("ndkxx");
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_NativeBundle_GetMainElementName())
      .WillRepeatedly(testing::Return(info));
  key.Init();
  EXPECT_EQ(key.GetPID(), getpid());
  EXPECT_EQ(key.GetType(), MediaAVSessionType::MEDIA_TYPE_INVALID);
  EXPECT_EQ(key.GetBundleName(), std::string(info.bundleName));
  EXPECT_EQ(key.GetAbilityName(), std::string(info.abilityName));
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionKeyInitTest1) {
  SetAllMockType(true);
  MediaAVSessionKey key;
  static OH_NativeBundle_ElementName info;
  info.moduleName = strdup("entry");
  info.abilityName = strdup("ndkxx");
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_NativeBundle_GetMainElementName())
      .WillRepeatedly(testing::Return(info));
  key.Init();
  EXPECT_EQ(key.GetPID(), getpid());
  EXPECT_EQ(key.GetType(), MediaAVSessionType::MEDIA_TYPE_INVALID);
  EXPECT_EQ(key.GetBundleName(), "");
  EXPECT_EQ(key.GetAbilityName(), std::string(info.abilityName));
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionKeyInitTest2) {
  SetAllMockType(true);
  MediaAVSessionKey key;
  static OH_NativeBundle_ElementName info;
  info.bundleName = strdup("com.xxx.hmxx");
  info.moduleName = strdup("entry");
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_NativeBundle_GetMainElementName())
      .WillRepeatedly(testing::Return(info));
  key.Init();
  EXPECT_EQ(key.GetPID(), getpid());
  EXPECT_EQ(key.GetType(), MediaAVSessionType::MEDIA_TYPE_INVALID);
  EXPECT_EQ(key.GetBundleName(), std::string(info.bundleName));
  EXPECT_EQ(key.GetAbilityName(), "");
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionKeyInitTest3) {
  SetAllMockType(true);
  MediaAVSessionKey key;
  static OH_NativeBundle_ElementName info;
  info.moduleName = strdup("entry");
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_NativeBundle_GetMainElementName())
      .WillRepeatedly(testing::Return(info));
  key.Init();
  EXPECT_EQ(key.GetPID(), getpid());
  EXPECT_EQ(key.GetType(), MediaAVSessionType::MEDIA_TYPE_INVALID);
  EXPECT_EQ(key.GetBundleName(), "");
  EXPECT_EQ(key.GetAbilityName(), "");
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplInitTest0) {
  SetAllMockType(true);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_Create(testing::_))
      .WillOnce(testing::Return(AVMETADATA_SUCCESS));

  MediaAVSessionAdapterImpl adapter;
  EXPECT_FALSE(adapter.IsActivated());
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplInitTest1) {
  SetAllMockType(true);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_Create(testing::_))
      .WillOnce(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetAssetId(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_ERROR_INVALID_PARAM));

  MediaAVSessionAdapterImpl adapter;
  EXPECT_FALSE(adapter.IsActivated());
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplInitTest2) {
  SetAllMockType(true);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_Create(testing::_))
      .WillOnce(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetAssetId(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_ERROR_INVALID_PARAM));

  MediaAVSessionAdapterImpl adapter;
  EXPECT_FALSE(adapter.IsActivated());
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplInitTest3) {
  SetAllMockType(true);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_Create(testing::_))
      .WillOnce(testing::Return(AVMETADATA_SUCCESS))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetAssetId(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_SUCCESS))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_SUCCESS))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));

  MediaAVSessionAdapterImpl adapter;
  EXPECT_FALSE(adapter.IsActivated());
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplDestoryTest0) {
  SetAllMockType(true);
  MediaAVSessionAdapterImpl adapter;
  adapter.SetMetadata(nullptr);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadata_Destroy(testing::_)).Times(0);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplCreateAVSessionTest0) {
  SetAllMockType(true);
  auto g_adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  auto type = MediaAVSessionType::MEDIA_TYPE_INVALID;
  g_adapter->avSessionKey_->SetType(MediaAVSessionType::MEDIA_TYPE_INVALID);
  bool ret = g_adapter->CreateAVSession(type);
  EXPECT_EQ(ret, false);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplCreateAVSessionTest1) {
  SetAllMockType(true);
  static OH_NativeBundle_ElementName info;
  info.bundleName = strdup("com.xxx.hmxx");
  info.moduleName = strdup("entry");
  info.abilityName = strdup("ndkxx");
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_NativeBundle_GetMainElementName())
      .WillRepeatedly(testing::Return(info));

  auto g_adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  g_adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);

  auto type = MediaAVSessionType::MEDIA_TYPE_VIDEO;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(),
              OH_AVSession_Create(testing::_, testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS))
      .WillRepeatedly(testing::Return(AV_SESSION_ERR_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Destroy(testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS))
      .WillRepeatedly(testing::Return(AV_SESSION_ERR_SUCCESS));
  g_adapter->avSessionKey_->SetType(type);
  bool ret = g_adapter->CreateAVSession(MediaAVSessionType::MEDIA_TYPE_AUDIO);

  EXPECT_EQ(ret, true);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplCreateAVSessionTest2) {
  SetAllMockType(true);
  static OH_NativeBundle_ElementName info;
  info.bundleName = strdup("com.xxx.hmxx");
  info.moduleName = strdup("entry");
  info.abilityName = strdup("ndkxx");
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_NativeBundle_GetMainElementName())
      .WillRepeatedly(testing::Return(info));

  auto g_adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  g_adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);

  auto type = MediaAVSessionType::MEDIA_TYPE_VIDEO;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(),
              OH_AVSession_Create(testing::_, testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS))
      .WillRepeatedly(testing::Return(AV_SESSION_ERR_SUCCESS));

  g_adapter->avSessionKey_->SetType(type);
  bool ret = g_adapter->CreateAVSession(type);

  EXPECT_EQ(ret, true);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplCreateAVSessionTest3) {
  SetAllMockType(true);
  static OH_NativeBundle_ElementName info;
  info.bundleName = strdup("com.xxx.hmxx");
  info.moduleName = strdup("entry");
  info.abilityName = strdup("ndkxx");
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_NativeBundle_GetMainElementName())
      .WillRepeatedly(testing::Return(info));

  auto g_adapter = std::make_shared<MediaAVSessionAdapterImpl>();

  auto type = MediaAVSessionType::MEDIA_TYPE_VIDEO;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(),
              OH_AVSession_Create(testing::_, testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS))
      .WillRepeatedly(testing::Return(AV_SESSION_ERR_SUCCESS));

  g_adapter->avSessionKey_->SetType(type);
  g_adapter->CreateNewSession(type);
  bool ret = g_adapter->CreateAVSession(type);

  EXPECT_EQ(ret, true);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplCreateAVSessionTest4) {
  SetAllMockType(true);
  static OH_NativeBundle_ElementName info;
  info.bundleName = strdup("com.xxx.hmxx");
  info.moduleName = strdup("entry");
  info.abilityName = strdup("ndkxx");
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_NativeBundle_GetMainElementName())
      .WillRepeatedly(testing::Return(info));

  auto g_adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  g_adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);
  auto type = MediaAVSessionType::MEDIA_TYPE_AUDIO;

  type = MediaAVSessionType::MEDIA_TYPE_VIDEO;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(),
              OH_AVSession_Create(testing::_, testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS))
      .WillRepeatedly(testing::Return(AV_SESSION_ERR_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Destroy(testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS))
      .WillRepeatedly(testing::Return(AV_SESSION_ERR_SUCCESS));
  g_adapter->avSessionKey_->SetType(type);
  bool ret = g_adapter->CreateAVSession(type);

  EXPECT_EQ(ret, true);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplCreateAVSessionTest5) {
  SetAllMockType(true);
  static OH_NativeBundle_ElementName info;
  info.bundleName = strdup("com.xxx.hmxx");
  info.moduleName = strdup("entry");
  info.abilityName = strdup("ndkxx");
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_NativeBundle_GetMainElementName())
      .WillRepeatedly(testing::Return(info));

  auto g_adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  auto g_adapter2 = std::make_shared<MediaAVSessionAdapterImpl>();
  g_adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);
  g_adapter2->avSession_ = reinterpret_cast<OH_AVSession*>(0x123456);
  auto type = MediaAVSessionType::MEDIA_TYPE_AUDIO;

  type = MediaAVSessionType::MEDIA_TYPE_VIDEO;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(),
              OH_AVSession_Create(testing::_, testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS))
      .WillRepeatedly(testing::Return(AV_SESSION_ERR_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Destroy(testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS))
      .WillRepeatedly(testing::Return(AV_SESSION_ERR_SUCCESS));
  g_adapter->avSessionKey_->SetType(type);
  g_adapter2->avSessionKey_->SetType(type);
  g_adapter->CreateAVSession(type);
  bool ret = g_adapter2->CreateAVSession(type);

  EXPECT_EQ(ret, true);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplCreateAVSessionTest6) {
  SetAllMockType(true);
  static OH_NativeBundle_ElementName info;
  info.bundleName = strdup("com.xxx.hmxx");
  info.moduleName = strdup("entry");
  info.abilityName = strdup("ndkxx");
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_NativeBundle_GetMainElementName())
      .WillRepeatedly(testing::Return(info));

  auto g_adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  g_adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);
  auto type = MediaAVSessionType::MEDIA_TYPE_AUDIO;

  auto avSessionKey = std::make_shared<MediaAVSessionKey>();
  g_adapter->avSessionKey_ = avSessionKey;
  g_adapter->avSessionKey_->Init();

  type = MediaAVSessionType::MEDIA_TYPE_VIDEO;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(),
              OH_AVSession_Create(testing::_, testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS))
      .WillRepeatedly(testing::Return(AV_SESSION_ERR_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Destroy(testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS))
      .WillRepeatedly(testing::Return(AV_SESSION_ERR_SUCCESS));
  g_adapter->CreateNewSession(type);
  g_adapter->avSessionKey_->SetType(type);
  bool ret = g_adapter->CreateAVSession(type);

  EXPECT_EQ(ret, false);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplDestroyAVSessionTest1) {
  SetAllMockType(true);
  static OH_NativeBundle_ElementName info;
  info.bundleName = strdup("com.xxx.hmxx");
  info.moduleName = strdup("entry");
  info.abilityName = strdup("ndkxx");
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_NativeBundle_GetMainElementName())
      .WillRepeatedly(testing::Return(info));

  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);
  auto type = MediaAVSessionType::MEDIA_TYPE_VIDEO;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(),
              OH_AVSession_Create(testing::_, testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));

  adapter->CreateNewSession(type);
  ASSERT_NE(adapter->avSession_, nullptr);

  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Destroy(testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));

  ASSERT_NO_FATAL_FAILURE(adapter->DestroyAVSession());
  EXPECT_EQ(adapter->avSession_, nullptr);
  EXPECT_EQ(adapter->avSessionMap.find(adapter->avSessionKey_->ToString()), adapter->avSessionMap.end());
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplDestroyAVSessionTest2) {
  SetAllMockType(true);
  static OH_NativeBundle_ElementName info;
  info.bundleName = strdup("com.xxx.hmxx");
  info.moduleName = strdup("entry");
  info.abilityName = strdup("ndkxx");
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_NativeBundle_GetMainElementName())
      .WillRepeatedly(testing::Return(info));

  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);
  auto type = MediaAVSessionType::MEDIA_TYPE_VIDEO;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(),
              OH_AVSession_Create(testing::_, testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));

  adapter->CreateNewSession(type);
  ASSERT_NE(adapter->avSession_, nullptr);
  adapter->avSession_ = nullptr;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Destroy(testing::_)).Times(0);

  ASSERT_NO_FATAL_FAILURE(adapter->DestroyAVSession());
  EXPECT_EQ(adapter->avSessionMap.find(adapter->avSessionKey_->ToString()), adapter->avSessionMap.end());
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplDestroyAVSessionTest3) {
  SetAllMockType(true);
  static OH_NativeBundle_ElementName info;
  info.bundleName = strdup("com.xxx.hmxx");
  info.moduleName = strdup("entry");
  info.abilityName = strdup("ndkxx");
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_NativeBundle_GetMainElementName())
      .WillRepeatedly(testing::Return(info));

  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);
  auto type = MediaAVSessionType::MEDIA_TYPE_VIDEO;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(),
              OH_AVSession_Create(testing::_, testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));

  adapter->CreateNewSession(type);
  ASSERT_NE(adapter->avSession_, nullptr);

  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Destroy(testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));

  ASSERT_NO_FATAL_FAILURE(adapter->DestroyAVSession());
  EXPECT_EQ(adapter->avSession_, nullptr);
  EXPECT_EQ(adapter->avSessionMap.find(adapter->avSessionKey_->ToString()), adapter->avSessionMap.end());
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplDestroyAVSessionTest4) {
  SetAllMockType(true);
  static OH_NativeBundle_ElementName info;
  info.bundleName = strdup("com.xxx.hmxx");
  info.moduleName = strdup("entry");
  info.abilityName = strdup("ndkxx");
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_NativeBundle_GetMainElementName())
      .WillRepeatedly(testing::Return(info));

  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);
  auto type = MediaAVSessionType::MEDIA_TYPE_VIDEO;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(),
              OH_AVSession_Create(testing::_, testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));

  adapter->CreateNewSession(type);
  ASSERT_NE(adapter->avSession_, nullptr);

  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Destroy(testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_INVALID_PARAMETER));

  ASSERT_NO_FATAL_FAILURE(adapter->DestroyAVSession());
  EXPECT_EQ(adapter->avSession_, nullptr);
  EXPECT_EQ(adapter->avSessionMap.find(adapter->avSessionKey_->ToString()), adapter->avSessionMap.end());
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplDestroyAVSessionTest5) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = nullptr;
  adapter->avSessionKey_ = nullptr;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Destroy(testing::_)).Times(0);
  EXPECT_EQ(adapter->avSession_, nullptr);
  EXPECT_EQ(adapter->avSessionKey_, nullptr);
  ASSERT_NO_FATAL_FAILURE(adapter->DestroyAVSession());
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplDestroyAVSessionTest6) {
  SetAllMockType(true);
  static OH_NativeBundle_ElementName info;
  info.bundleName = strdup("com.xxx.hmxx");
  info.moduleName = strdup("entry");
  info.abilityName = strdup("ndkxx");
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_NativeBundle_GetMainElementName())
      .WillRepeatedly(testing::Return(info));

  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);
  auto type = MediaAVSessionType::MEDIA_TYPE_VIDEO;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(),
              OH_AVSession_Create(testing::_, testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));

  adapter->CreateNewSession(type);
  ASSERT_NE(adapter->avSession_, nullptr);
  ASSERT_NE(adapter->avSessionKey_, nullptr);

  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Destroy(adapter->avSession_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));

  ASSERT_NO_FATAL_FAILURE(adapter->DestroyAVSession());
  EXPECT_EQ(adapter->avSession_, nullptr);
  EXPECT_EQ(adapter->avSessionMap.find(adapter->avSessionKey_->ToString()), adapter->avSessionMap.end());
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplDestroyAVSessionTest7) {
  SetAllMockType(true);
  static OH_NativeBundle_ElementName info;
  info.bundleName = strdup("com.xxx.hmxx");
  info.moduleName = strdup("entry");
  info.abilityName = strdup("ndkxx");
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_NativeBundle_GetMainElementName())
      .WillRepeatedly(testing::Return(info));

  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);
  auto type = MediaAVSessionType::MEDIA_TYPE_VIDEO;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(),
              OH_AVSession_Create(testing::_, testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));

  adapter->CreateNewSession(type);
  ASSERT_NE(adapter->avSession_, nullptr);
  ASSERT_NE(adapter->avSessionKey_, nullptr);

  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Destroy(adapter->avSession_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  std::string tmpKey = adapter->avSessionKey_->ToString();
  adapter->avSessionKey_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(adapter->DestroyAVSession());
  EXPECT_EQ(adapter->avSession_, nullptr);
  EXPECT_NE(adapter->avSessionMap.find(tmpKey), adapter->avSessionMap.end());
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplDestroyAVSessionTest8) {
  SetAllMockType(true);
  static OH_NativeBundle_ElementName info;
  info.bundleName = strdup("com.xxx.hmxx");
  info.moduleName = strdup("entry");
  info.abilityName = strdup("ndkxx");
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_NativeBundle_GetMainElementName())
      .WillRepeatedly(testing::Return(info));

  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);
  auto type = MediaAVSessionType::MEDIA_TYPE_VIDEO;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(),
              OH_AVSession_Create(testing::_, testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));

  adapter->CreateNewSession(type);
  ASSERT_NE(adapter->avSession_, nullptr);
  ASSERT_NE(adapter->avSessionKey_, nullptr);

  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Destroy(adapter->avSession_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS))
      .WillRepeatedly(testing::Return(AV_SESSION_ERR_SUCCESS));

  adapter->avSessionMap.erase(adapter->avSessionKey_->ToString());
  ASSERT_NO_FATAL_FAILURE(adapter->DestroyAVSession());
  EXPECT_EQ(adapter->avSession_, nullptr);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplAVSessionOnCommandCallbackTest1) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);
  auto ret = adapter->AVSessionOnCommandCallback(adapter->avSession_, CONTROL_CMD_PLAY, nullptr);
  EXPECT_EQ(ret, AVSESSION_CALLBACK_RESULT_FAILURE);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplAVSessionOnCommandCallbackTest2) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);

  size_t callback_index = 9999;
  AVSessionCallback_Result result = MediaAVSessionAdapterImpl::AVSessionOnCommandCallback(
      adapter->avSession_, CONTROL_CMD_PLAY, reinterpret_cast<void*>(callback_index));
  EXPECT_EQ(result, AVSESSION_CALLBACK_RESULT_FAILURE);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplAVSessionOnCommandCallbackTest3) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);
  auto callback_adapter_mock = std::make_shared<MediaAVSessionCallbackAdapterMock>();
  size_t callback_index = MediaAVSessionAdapterImpl::callback_wrapper_.AddCallback(callback_adapter_mock);
  EXPECT_CALL(*callback_adapter_mock, Play()).Times(1);
  AVSessionCallback_Result result = MediaAVSessionAdapterImpl::AVSessionOnCommandCallback(
      nullptr, CONTROL_CMD_PLAY, reinterpret_cast<void*>(callback_index));
  EXPECT_EQ(result, AVSESSION_CALLBACK_RESULT_SUCCESS);
  MediaAVSessionAdapterImpl::callback_wrapper_.Clear(callback_index);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplAVSessionOnCommandCallbackTest4) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);
  auto callback_adapter_mock = std::make_shared<MediaAVSessionCallbackAdapterMock>();
  size_t callback_index = MediaAVSessionAdapterImpl::callback_wrapper_.AddCallback(callback_adapter_mock);
  EXPECT_CALL(*callback_adapter_mock, Pause()).Times(1);
  AVSessionCallback_Result result = MediaAVSessionAdapterImpl::AVSessionOnCommandCallback(
      nullptr, CONTROL_CMD_PAUSE, reinterpret_cast<void*>(callback_index));
  EXPECT_EQ(result, AVSESSION_CALLBACK_RESULT_SUCCESS);
  MediaAVSessionAdapterImpl::callback_wrapper_.Clear(callback_index);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplAVSessionOnCommandCallbackTest5) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);
  auto callback_adapter_mock = std::make_shared<MediaAVSessionCallbackAdapterMock>();
  size_t callback_index = MediaAVSessionAdapterImpl::callback_wrapper_.AddCallback(callback_adapter_mock);
  EXPECT_CALL(*callback_adapter_mock, Stop()).Times(1);
  AVSessionCallback_Result result = MediaAVSessionAdapterImpl::AVSessionOnCommandCallback(
      nullptr, CONTROL_CMD_STOP, reinterpret_cast<void*>(callback_index));
  EXPECT_EQ(result, AVSESSION_CALLBACK_RESULT_SUCCESS);
  MediaAVSessionAdapterImpl::callback_wrapper_.Clear(callback_index);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplAVSessionOnCommandCallbackTest6) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);
  auto callback_adapter_mock = std::make_shared<MediaAVSessionCallbackAdapterMock>();
  size_t callback_index = MediaAVSessionAdapterImpl::callback_wrapper_.AddCallback(callback_adapter_mock);
  EXPECT_CALL(*callback_adapter_mock, Play()).Times(0);
  EXPECT_CALL(*callback_adapter_mock, Pause()).Times(0);
  EXPECT_CALL(*callback_adapter_mock, Stop()).Times(0);
  AVSessionCallback_Result result = MediaAVSessionAdapterImpl::AVSessionOnCommandCallback(
      nullptr, CONTROL_CMD_PLAY_NEXT, reinterpret_cast<void*>(callback_index));
  EXPECT_EQ(result, AVSESSION_CALLBACK_RESULT_FAILURE);
  MediaAVSessionAdapterImpl::callback_wrapper_.Clear(callback_index);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplAVSessionOnSeekCallbackTest1) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);
  auto ret = adapter->AVSessionOnSeekCallback(adapter->avSession_, 1000, nullptr);
  EXPECT_EQ(ret, AVSESSION_CALLBACK_RESULT_FAILURE);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplAVSessionOnSeekCallbackTest2) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);
  size_t callback_index = 9999;
  AVSessionCallback_Result result = MediaAVSessionAdapterImpl::AVSessionOnSeekCallback(
      adapter->avSession_, 1000, reinterpret_cast<void*>(callback_index));
  EXPECT_EQ(result, AVSESSION_CALLBACK_RESULT_FAILURE);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplAVSessionOnSeekCallbackTest3) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);
  auto callback_adapter_mock = std::make_shared<MediaAVSessionCallbackAdapterMock>();
  size_t callback_index = MediaAVSessionAdapterImpl::callback_wrapper_.AddCallback(callback_adapter_mock);
  EXPECT_CALL(*callback_adapter_mock, SeekTo(1000)).Times(1);
  AVSessionCallback_Result result = MediaAVSessionAdapterImpl::AVSessionOnSeekCallback(
      adapter->avSession_, 1000, reinterpret_cast<void*>(callback_index));
  EXPECT_EQ(result, AVSESSION_CALLBACK_RESULT_SUCCESS);
  MediaAVSessionAdapterImpl::callback_wrapper_.Clear(callback_index);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplRegistCallbackTest1) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  auto mediaAVSessionCallbackAdapterMock = std::make_shared<MediaAVSessionCallbackAdapterMock>();
  bool ret = adapter->RegistCallback(mediaAVSessionCallbackAdapterMock);
  EXPECT_EQ(ret, false);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplRegistCallbackTest2) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);
  auto mediaAVSessionCallbackAdapterMock = std::make_shared<MediaAVSessionCallbackAdapterMock>();
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Activate(testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_INVALID_PARAMETER));
  bool ret = adapter->RegistCallback(mediaAVSessionCallbackAdapterMock);
  EXPECT_EQ(ret, false);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplRegistCallbackTest3) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);
  auto mediaAVSessionCallbackAdapterMock = std::make_shared<MediaAVSessionCallbackAdapterMock>();
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Activate(testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  mediaAVSessionCallbackAdapterMock = nullptr;
  bool ret = adapter->RegistCallback(mediaAVSessionCallbackAdapterMock);
  EXPECT_EQ(ret, false);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplRegistCallbackTest4) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);
  auto mediaAVSessionCallbackAdapterMock = std::make_shared<MediaAVSessionCallbackAdapterMock>();
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Activate(testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(),
              OH_AVSession_RegisterCommandCallback(testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_INVALID_PARAMETER));
  bool ret = adapter->RegistCallback(mediaAVSessionCallbackAdapterMock);
  EXPECT_EQ(ret, false);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplRegistCallbackTest6) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);
  auto mediaAVSessionCallbackAdapterMock = std::make_shared<MediaAVSessionCallbackAdapterMock>();
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Activate(testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(),
              OH_AVSession_RegisterCommandCallback(testing::_, testing::_, testing::_, testing::_))
      .Times(3)
      .WillRepeatedly(testing::Return(AV_SESSION_ERR_SUCCESS));
  bool ret = adapter->RegistCallback(mediaAVSessionCallbackAdapterMock);
  EXPECT_EQ(ret, true);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplRegistCallbackTest5) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);
  auto mediaAVSessionCallbackAdapterMock = std::make_shared<MediaAVSessionCallbackAdapterMock>();
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Activate(testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(),
              OH_AVSession_RegisterCommandCallback(testing::_, testing::_, testing::_, testing::_))
      .Times(3)
      .WillRepeatedly(testing::Return(AV_SESSION_ERR_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_RegisterSeekCallback(testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_INVALID_PARAMETER));
  bool ret = adapter->RegistCallback(mediaAVSessionCallbackAdapterMock);
  EXPECT_EQ(ret, false);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplIsActiveTest1) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);
  adapter->isActived_ = true;
  EXPECT_EQ(adapter->IsActivated(), adapter->isActived_);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplIsActiveTest2) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  EXPECT_EQ(adapter->IsActivated(), false);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplActiveTest1) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  EXPECT_EQ(adapter->Activate(), false);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplActiveTest2) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Activate(testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_INVALID_PARAMETER));
  EXPECT_EQ(adapter->Activate(), false);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplActiveTest3) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Activate(testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  EXPECT_EQ(adapter->Activate(), true);
  EXPECT_EQ(adapter->isActived_, true);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplDeActivateTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Deactivate(testing::_)).Times(0);
  ASSERT_NO_FATAL_FAILURE(adapter->DeActivate());
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplDeActivateTest1) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Activate(testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  adapter->Activate();
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Deactivate(testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  ASSERT_NO_FATAL_FAILURE(adapter->DeActivate());
  EXPECT_EQ(adapter->isActived_, false);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplDeActivateTest2) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Activate(testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  adapter->Activate();
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Deactivate(testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  ASSERT_NO_FATAL_FAILURE(adapter->DeActivate());
  EXPECT_EQ(adapter->isActived_, false);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplDeActivateTest3) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Activate(testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  adapter->Activate();
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Deactivate(testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_INVALID_PARAMETER));
  ASSERT_NO_FATAL_FAILURE(adapter->DeActivate());
  EXPECT_EQ(adapter->isActived_, true);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplUpdateAVMetadataTest1) {
  SetAllMockType(true);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::DoAll(testing::SetArgPointee<1>(reinterpret_cast<OH_AVMetadata*>(0x87654321)),
                                     testing::Return(AVMETADATA_SUCCESS)));
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->builder_ = reinterpret_cast<OH_AVMetadataBuilder*>(0x12345678);
  adapter->avMetadata_ = nullptr;
  AVMetadata_Result ret = adapter->UpdateAVMetadata();
  EXPECT_EQ(ret, AVMETADATA_SUCCESS);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplUpdateAVMetadataTest2) {
  SetAllMockType(true);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillOnce(testing::DoAll(testing::SetArgPointee<1>(reinterpret_cast<OH_AVMetadata*>(0x87654321)),
                               testing::Return(AVMETADATA_SUCCESS)))
      .WillOnce(testing::Return(AVMETADATA_ERROR_INVALID_PARAM));
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->builder_ = reinterpret_cast<OH_AVMetadataBuilder*>(0x12345678);
  adapter->avMetadata_ = nullptr;

  AVMetadata_Result ret = adapter->UpdateAVMetadata();
  EXPECT_EQ(ret, AVMETADATA_ERROR_INVALID_PARAM);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplUpdateAVMetadataTest3) {
  SetAllMockType(true);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadata_Destroy(testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::DoAll(testing::SetArgPointee<1>(reinterpret_cast<OH_AVMetadata*>(0x87654321)),
                                     testing::Return(AVMETADATA_SUCCESS)));
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avMetadata_ = reinterpret_cast<OH_AVMetadata*>(0x87654321);

  AVMetadata_Result ret = adapter->UpdateAVMetadata();
  EXPECT_EQ(ret, AVMETADATA_SUCCESS);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplUpdateAVMetadataTest4) {
  SetAllMockType(true);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadata_Destroy(testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_ERROR_INVALID_PARAM));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::DoAll(testing::SetArgPointee<1>(reinterpret_cast<OH_AVMetadata*>(0x87654321)),
                                     testing::Return(AVMETADATA_SUCCESS)));
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avMetadata_ = reinterpret_cast<OH_AVMetadata*>(0x87654321);

  AVMetadata_Result ret = adapter->UpdateAVMetadata();
  EXPECT_EQ(ret, AVMETADATA_SUCCESS);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplUpdateMetaDataCacheTest1) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  std::shared_ptr<MediaAVSessionMetadataAdapterMock> metadata = nullptr;
  auto ret = adapter->UpdateMetaData(metadata);
  EXPECT_EQ(ret, false);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplUpdateMetaDataCacheTest2) {
  SetAllMockType(true);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::DoAll(testing::SetArgPointee<1>(reinterpret_cast<OH_AVMetadata*>(0x87654321)),
                                     testing::Return(AVMETADATA_SUCCESS)));
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->builder_ = reinterpret_cast<OH_AVMetadataBuilder*>(0x12345678);
  adapter->avMetadata_ = nullptr;
  auto metadata = std::make_shared<MediaAVSessionMetadataAdapterMock>();

  EXPECT_CALL(*metadata, GetTitle()).WillRepeatedly(testing::Return("title"));
  EXPECT_CALL(*metadata, GetArtist()).WillRepeatedly(testing::Return("artist"));
  EXPECT_CALL(*metadata, GetAlbum()).WillRepeatedly(testing::Return("album"));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetTitle(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetArtist(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetAlbum(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_SUCCESS));

  bool result = adapter->UpdateMetaData(metadata);
  EXPECT_EQ(result, true);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplUpdateMetaDataCacheTest3) {
  SetAllMockType(true);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::DoAll(testing::SetArgPointee<1>(reinterpret_cast<OH_AVMetadata*>(0x87654321)),
                                     testing::Return(AVMETADATA_SUCCESS)));
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->builder_ = reinterpret_cast<OH_AVMetadataBuilder*>(0x12345678);
  adapter->avMetadata_ = nullptr;
  auto metadata = std::make_shared<MediaAVSessionMetadataAdapterMock>();

  EXPECT_CALL(*metadata, GetTitle()).WillRepeatedly(testing::Return("title"));
  EXPECT_CALL(*metadata, GetArtist()).WillRepeatedly(testing::Return("artist"));
  EXPECT_CALL(*metadata, GetAlbum()).WillRepeatedly(testing::Return("album"));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetTitle(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_ERROR_INVALID_PARAM));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetArtist(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetAlbum(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_SUCCESS));

  bool result = adapter->UpdateMetaData(metadata);
  EXPECT_EQ(result, true);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplUpdateMetaDataCacheTest4) {
  SetAllMockType(true);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::DoAll(testing::SetArgPointee<1>(reinterpret_cast<OH_AVMetadata*>(0x87654321)),
                                     testing::Return(AVMETADATA_SUCCESS)));
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->builder_ = reinterpret_cast<OH_AVMetadataBuilder*>(0x12345678);
  adapter->avMetadata_ = nullptr;
  auto metadata = std::make_shared<MediaAVSessionMetadataAdapterMock>();

  EXPECT_CALL(*metadata, GetTitle()).WillRepeatedly(testing::Return("title"));
  EXPECT_CALL(*metadata, GetArtist()).WillRepeatedly(testing::Return("artist"));
  EXPECT_CALL(*metadata, GetAlbum()).WillRepeatedly(testing::Return("album"));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetTitle(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetArtist(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_ERROR_INVALID_PARAM));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetAlbum(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_SUCCESS));

  bool result = adapter->UpdateMetaData(metadata);
  EXPECT_EQ(result, true);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplUpdateMetaDataCacheTest5) {
  SetAllMockType(true);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::DoAll(testing::SetArgPointee<1>(reinterpret_cast<OH_AVMetadata*>(0x87654321)),
                                     testing::Return(AVMETADATA_SUCCESS)));
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->builder_ = reinterpret_cast<OH_AVMetadataBuilder*>(0x12345678);
  adapter->avMetadata_ = nullptr;
  auto metadata = std::make_shared<MediaAVSessionMetadataAdapterMock>();

  EXPECT_CALL(*metadata, GetTitle()).WillRepeatedly(testing::Return("title"));
  EXPECT_CALL(*metadata, GetArtist()).WillRepeatedly(testing::Return("artist"));
  EXPECT_CALL(*metadata, GetAlbum()).WillRepeatedly(testing::Return("album"));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetTitle(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetArtist(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetAlbum(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_ERROR_INVALID_PARAM));

  bool result = adapter->UpdateMetaData(metadata);
  EXPECT_EQ(result, true);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplUpdateMetaDataCacheTest6) {
  SetAllMockType(true);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::DoAll(testing::SetArgPointee<1>(reinterpret_cast<OH_AVMetadata*>(0x87654321)),
                                     testing::Return(AVMETADATA_SUCCESS)));
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->builder_ = reinterpret_cast<OH_AVMetadataBuilder*>(0x12345678);
  adapter->avMetadata_ = nullptr;
  auto metadata = std::make_shared<MediaAVSessionMetadataAdapterMock>();

  EXPECT_CALL(*metadata, GetTitle()).WillRepeatedly(testing::Return("title"));
  EXPECT_CALL(*metadata, GetArtist()).WillRepeatedly(testing::Return("artist"));
  EXPECT_CALL(*metadata, GetAlbum()).WillRepeatedly(testing::Return("album"));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetTitle(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_ERROR_INVALID_PARAM));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetArtist(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_ERROR_INVALID_PARAM));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetAlbum(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_ERROR_INVALID_PARAM));

  bool result = adapter->UpdateMetaData(metadata);
  EXPECT_EQ(result, false);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplUpdateMetaDataCacheTest7) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->builder_ = reinterpret_cast<OH_AVMetadataBuilder*>(0x12345678);
  adapter->avMetadata_ = nullptr;
  auto metadata = std::make_shared<MediaAVSessionMetadataAdapterMock>();

  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetTitle(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetArtist(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetAlbum(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_ERROR_INVALID_PARAM))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));

  bool result = adapter->UpdateMetaData(metadata);
  EXPECT_EQ(result, false);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplUpdateMetaDataNoChangeTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->builder_ = reinterpret_cast<OH_AVMetadataBuilder*>(0x12345678);
  adapter->avMetadata_ = nullptr;
  auto metadata = std::make_shared<MediaAVSessionMetadataAdapterMock>();

  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetTitle(testing::_, testing::_)).Times(AnyNumber());
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetArtist(testing::_, testing::_)).Times(AnyNumber());
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetAlbum(testing::_, testing::_)).Times(AnyNumber());
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .Times(AnyNumber());

  bool result = adapter->UpdateMetaData(metadata);
  EXPECT_EQ(result, false);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplUpdateMetaDataCachePositionTest1) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  std::shared_ptr<MediaAVSessionPositionAdapterMock> position = nullptr;
  auto ret = adapter->UpdateDuration(position);
  EXPECT_EQ(ret, false);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplUpdateMetaDataCachePositionTest2) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->builder_ = reinterpret_cast<OH_AVMetadataBuilder*>(0x12345678);

  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetDuration(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_ERROR_INVALID_PARAM));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));

  auto position = std::make_shared<MediaAVSessionPositionAdapterMock>();
  EXPECT_CALL(*position, GetDuration()).WillRepeatedly(testing::Return(1000));
  auto ret = adapter->UpdateDuration(position);
  EXPECT_EQ(ret, false);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplUpdateMetaDataCachePositionTest3) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->builder_ = reinterpret_cast<OH_AVMetadataBuilder*>(0x12345678);

  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetDuration(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_ERROR_INVALID_PARAM));

  auto position = std::make_shared<MediaAVSessionPositionAdapterMock>();
  EXPECT_CALL(*position, GetDuration()).WillRepeatedly(testing::Return(1000));
  auto ret = adapter->UpdateDuration(position);
  EXPECT_EQ(ret, false);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplUpdateMetaDataCachePositionTest4) {
  SetAllMockType(true);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->builder_ = reinterpret_cast<OH_AVMetadataBuilder*>(0x12345678);

  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetDuration(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_SUCCESS));

  auto position = std::make_shared<MediaAVSessionPositionAdapterMock>();
  EXPECT_CALL(*position, GetDuration()).WillRepeatedly(testing::Return(1000));
  auto ret = adapter->UpdateDuration(position);
  EXPECT_EQ(ret, true);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplUpdateMetaDataCachePositionSetUriFailTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->builder_ = reinterpret_cast<OH_AVMetadataBuilder*>(0x12345678);

  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetDuration(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetMediaImageUri(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_ERROR_INVALID_PARAM));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .Times(0);

  auto position = std::make_shared<MediaAVSessionPositionAdapterMock>();
  EXPECT_CALL(*position, GetDuration()).WillRepeatedly(testing::Return(1000));
  auto ret = adapter->UpdateDuration(position);
  EXPECT_EQ(ret, false);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplUpdatePlaybackStateCacheTest) {
  SetAllMockType(true);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  MediaAVSessionPlayState playState = MediaAVSessionPlayState::STATE_PLAY;
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = nullptr;
  auto ret = adapter->UpdatePlaybackState(playState);
  auto playBack = adapter->avPlaybackState_;
  EXPECT_EQ(playBack, PLAYBACK_STATE_PLAYING);
  EXPECT_EQ(ret, false);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplUpdatePlaybackStateCacheTest2) {
  SetAllMockType(true);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  MediaAVSessionPlayState playState = MediaAVSessionPlayState::STATE_PAUSE;
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetPlaybackState(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AV_SESSION_ERR_INVALID_PARAMETER));
  auto ret = adapter->UpdatePlaybackState(playState);
  auto playBack = adapter->avPlaybackState_;
  EXPECT_EQ(playBack, PLAYBACK_STATE_PAUSED);
  EXPECT_EQ(ret, false);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplUpdatePlaybackStateCacheTest3) {
  SetAllMockType(true);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  MediaAVSessionPlayState playState = static_cast<MediaAVSessionPlayState>(4);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetPlaybackState(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AV_SESSION_ERR_SUCCESS));
  auto ret = adapter->UpdatePlaybackState(playState);
  auto playBack = adapter->avPlaybackState_;
  EXPECT_EQ(playBack, PLAYBACK_STATE_PAUSED);
  EXPECT_EQ(ret, true);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplUpdatePlaybackStateCacheTest4) {
  SetAllMockType(true);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  MediaAVSessionPlayState playState = MediaAVSessionPlayState::STATE_INITIAL;
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetPlaybackState(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AV_SESSION_ERR_SUCCESS));
  auto ret = adapter->UpdatePlaybackState(playState);
  auto playBack = adapter->avPlaybackState_;
  EXPECT_EQ(playBack, PLAYBACK_STATE_PAUSED);
  EXPECT_EQ(ret, true);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplSetMetadataTest) {
  SetAllMockType(true);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadata_Destroy(testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));

  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->builder_ = reinterpret_cast<OH_AVMetadataBuilder*>(0x12345678);
  adapter->avMetadata_ = nullptr;
  adapter->avSession_ = nullptr;
  adapter->isActived_ = false;
  auto metadata = std::make_shared<MediaAVSessionMetadataAdapterMock>();

  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetTitle(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetArtist(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetAlbum(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Activate(testing::_)).Times(0);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetAVMetadata(testing::_, testing::_)).Times(0);

  ASSERT_NO_FATAL_FAILURE(adapter->SetMetadata(metadata));
  EXPECT_EQ(adapter->isActived_, false);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplSetMetadataTest2) {
  SetAllMockType(true);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadata_Destroy(testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));

  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->builder_ = reinterpret_cast<OH_AVMetadataBuilder*>(0x12345678);
  adapter->avMetadata_ = nullptr;
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x1234567);
  auto metadata = std::make_shared<MediaAVSessionMetadataAdapterMock>();

  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetTitle(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetArtist(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetAlbum(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Activate(testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetAVMetadata(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_INVALID_PARAMETER));

  ASSERT_NO_FATAL_FAILURE(adapter->SetMetadata(metadata));
  EXPECT_EQ(adapter->isActived_, true);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplSetMetadataTest3) {
  SetAllMockType(true);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadata_Destroy(testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));

  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->builder_ = reinterpret_cast<OH_AVMetadataBuilder*>(0x12345678);
  adapter->avMetadata_ = nullptr;
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x1234567);
  auto metadata = std::make_shared<MediaAVSessionMetadataAdapterMock>();

  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetTitle(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetArtist(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetAlbum(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Activate(testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetAVMetadata(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));

  ASSERT_NO_FATAL_FAILURE(adapter->SetMetadata(metadata));
  EXPECT_EQ(adapter->isActived_, true);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplSetPlaybackStateTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->builder_ = reinterpret_cast<OH_AVMetadataBuilder*>(0x12345678);
  adapter->avSession_ = nullptr;
  adapter->isActived_ = false;
  MediaAVSessionPlayState playState = MediaAVSessionPlayState::STATE_PLAY;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetPlaybackState(testing::_, testing::_)).Times(0);
  ASSERT_NO_FATAL_FAILURE(adapter->SetPlaybackState(playState));
  EXPECT_EQ(adapter->isActived_, false);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplSetPlaybackStateTest2) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->builder_ = reinterpret_cast<OH_AVMetadataBuilder*>(0x12345678);
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x1234567);
  adapter->isActived_ = false;
  MediaAVSessionPlayState playState = MediaAVSessionPlayState::STATE_PLAY;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetPlaybackState(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_INVALID_PARAMETER));
  ASSERT_NO_FATAL_FAILURE(adapter->SetPlaybackState(playState));
  EXPECT_EQ(adapter->isActived_, false);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplSetPlaybackStateTest3) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->builder_ = reinterpret_cast<OH_AVMetadataBuilder*>(0x12345678);
  adapter->avSession_ = nullptr;
  adapter->isActived_ = false;
  MediaAVSessionPlayState playState = MediaAVSessionPlayState::STATE_PLAY;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetPlaybackState(testing::_, testing::_)).Times(0);
  ASSERT_NO_FATAL_FAILURE(adapter->SetPlaybackState(playState));
  EXPECT_EQ(adapter->isActived_, false);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplSetPlaybackStateTest4) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->builder_ = reinterpret_cast<OH_AVMetadataBuilder*>(0x12345678);
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x1234567);
  adapter->avPlaybackState_ = PLAYBACK_STATE_PLAYING;
  adapter->isActived_ = false;
  MediaAVSessionPlayState playState = MediaAVSessionPlayState::STATE_PLAY;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetPlaybackState(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AV_SESSION_ERR_SUCCESS));
  EXPECT_EQ(adapter->UpdatePlaybackState(playState), true);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Activate(testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));

  ASSERT_NO_FATAL_FAILURE(adapter->SetPlaybackState(playState));
  EXPECT_EQ(adapter->isActived_, true);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplSetPlaybackStateTest5) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->builder_ = reinterpret_cast<OH_AVMetadataBuilder*>(0x12345678);
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x1234567);
  adapter->avPlaybackState_ = PLAYBACK_STATE_PLAYING;
  adapter->isActived_ = false;
  MediaAVSessionPlayState playState = MediaAVSessionPlayState::STATE_PLAY;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetPlaybackState(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS))
      .WillOnce(testing::Return(AV_SESSION_ERR_INVALID_PARAMETER));
  EXPECT_EQ(adapter->UpdatePlaybackState(playState), true);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Activate(testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));

  ASSERT_NO_FATAL_FAILURE(adapter->SetPlaybackState(playState));
  EXPECT_EQ(adapter->isActived_, true);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplUpdatePlaybackStateCachePositionTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  std::shared_ptr<MediaAVSessionPositionAdapterMock> position = nullptr;
  auto ret = adapter->UpdatePlaybackPosition(position);
  EXPECT_EQ(ret, false);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplUpdatePlaybackStateCachePositionTest2) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = nullptr;

  auto position = std::make_shared<MediaAVSessionPositionAdapterMock>();
  EXPECT_CALL(*position, GetElapsedTime()).WillOnce(testing::Return(500));
  EXPECT_CALL(*position, GetUpdateTime()).WillOnce(testing::Return(100));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetPlaybackPosition(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_INVALID_PARAMETER));

  auto ret = adapter->UpdatePlaybackPosition(position);
  EXPECT_EQ(ret, false);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplUpdatePlaybackStateCachePositionTest3) {
  SetAllMockType(true);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = nullptr;

  auto position = std::make_shared<MediaAVSessionPositionAdapterMock>();
  EXPECT_CALL(*position, GetElapsedTime()).WillOnce(testing::Return(500));
  EXPECT_CALL(*position, GetUpdateTime()).WillOnce(testing::Return(100));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetPlaybackPosition(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));

  auto ret = adapter->UpdatePlaybackPosition(position);
  EXPECT_EQ(ret, true);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplUpdatePlaybackStateCachePositionTest4) {
  SetAllMockType(true);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = nullptr;

  auto position = std::make_shared<MediaAVSessionPositionAdapterMock>();
  EXPECT_CALL(*position, GetElapsedTime()).WillOnce(testing::Return(500));
  EXPECT_CALL(*position, GetUpdateTime()).WillOnce(testing::Return(100));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetPlaybackPosition(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));

  auto ret = adapter->UpdatePlaybackPosition(position);
  EXPECT_EQ(ret, true);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplUpdatePlaybackStateCachePositionTest5) {
  SetAllMockType(true);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = nullptr;

  auto position = std::make_shared<MediaAVSessionPositionAdapterMock>();
  EXPECT_CALL(*position, GetElapsedTime()).WillOnce(testing::Return(500));
  EXPECT_CALL(*position, GetUpdateTime()).WillOnce(testing::Return(100));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetPlaybackPosition(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_INVALID_PARAMETER));

  auto ret = adapter->UpdatePlaybackPosition(position);
  EXPECT_EQ(ret, false);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplUpdatePlaybackStateCachePositionTest6) {
  SetAllMockType(true);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = nullptr;

  auto position = std::make_shared<MediaAVSessionPositionAdapterMock>();
  EXPECT_CALL(*position, GetElapsedTime()).WillOnce(testing::Return(500));
  EXPECT_CALL(*position, GetUpdateTime()).WillOnce(testing::Return(100));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetPlaybackPosition(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));

  auto ret = adapter->UpdatePlaybackPosition(position);
  EXPECT_EQ(ret, true);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplUpdatePlaybackStateCachePositionTest7) {
  SetAllMockType(true);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = nullptr;

  auto position = std::make_shared<MediaAVSessionPositionAdapterMock>();
  EXPECT_CALL(*position, GetElapsedTime()).WillOnce(testing::Return(500));
  EXPECT_CALL(*position, GetUpdateTime()).WillOnce(testing::Return(100));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetPlaybackPosition(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_INVALID_PARAMETER));

  auto ret = adapter->UpdatePlaybackPosition(position);
  EXPECT_EQ(ret, false);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplSetPlaybackPositionTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  std::shared_ptr<MediaAVSessionPositionAdapterMock> position = nullptr;
  ASSERT_NO_FATAL_FAILURE(adapter->SetPlaybackPosition(position));
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplSetPlaybackPositionTest2) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = nullptr;
  auto position = std::make_shared<MediaAVSessionPositionAdapterMock>();
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetDuration(testing::_, testing::_)).Times(0);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetPlaybackPosition(testing::_, testing::_)).Times(1);
  ASSERT_NO_FATAL_FAILURE(adapter->SetPlaybackPosition(position));
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplSetPlaybackPositionTest3) {
  SetAllMockType(true);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadata_Destroy(testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->builder_ = reinterpret_cast<OH_AVMetadataBuilder*>(0x12345678);
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x1234567);

  auto position = std::make_shared<MediaAVSessionPositionAdapterMock>();
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetDuration(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Activate(testing::_))
      .WillRepeatedly(testing::Return(AV_SESSION_ERR_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetAVMetadata(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_INVALID_PARAMETER));
  EXPECT_CALL(*position, GetDuration()).WillRepeatedly(testing::Return(1000));
  EXPECT_CALL(*position, GetElapsedTime()).WillOnce(testing::Return(500));
  EXPECT_CALL(*position, GetUpdateTime()).WillOnce(testing::Return(100));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetPlaybackPosition(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetPlaybackState(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_INVALID_PARAMETER));

  ASSERT_NO_FATAL_FAILURE(adapter->SetPlaybackPosition(position));
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplSetPlaybackPositionTest4) {
  SetAllMockType(true);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadata_Destroy(testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->builder_ = reinterpret_cast<OH_AVMetadataBuilder*>(0x12345678);
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x1234567);

  auto position = std::make_shared<MediaAVSessionPositionAdapterMock>();
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetDuration(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Activate(testing::_))
      .WillRepeatedly(testing::Return(AV_SESSION_ERR_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetAVMetadata(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  EXPECT_CALL(*position, GetDuration()).WillRepeatedly(testing::Return(1000));
  EXPECT_CALL(*position, GetElapsedTime()).WillOnce(testing::Return(500));
  EXPECT_CALL(*position, GetUpdateTime()).WillOnce(testing::Return(100));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetPlaybackPosition(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetPlaybackState(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_INVALID_PARAMETER));

  ASSERT_NO_FATAL_FAILURE(adapter->SetPlaybackPosition(position));
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplSetPlaybackPositionTest5) {
  SetAllMockType(true);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadata_Destroy(testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->builder_ = reinterpret_cast<OH_AVMetadataBuilder*>(0x12345678);
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x1234567);

  auto position = std::make_shared<MediaAVSessionPositionAdapterMock>();
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetDuration(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Activate(testing::_))
      .WillRepeatedly(testing::Return(AV_SESSION_ERR_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetAVMetadata(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_INVALID_PARAMETER));
  EXPECT_CALL(*position, GetDuration()).WillRepeatedly(testing::Return(1000));
  EXPECT_CALL(*position, GetElapsedTime()).WillOnce(testing::Return(500));
  EXPECT_CALL(*position, GetUpdateTime()).WillOnce(testing::Return(100));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetPlaybackPosition(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetPlaybackState(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));

  ASSERT_NO_FATAL_FAILURE(adapter->SetPlaybackPosition(position));
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplSetPlaybackPositionTest6) {
  SetAllMockType(true);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadata_Destroy(testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->builder_ = reinterpret_cast<OH_AVMetadataBuilder*>(0x12345678);
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x1234567);

  auto position = std::make_shared<MediaAVSessionPositionAdapterMock>();
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetDuration(testing::_, testing::_))
      .WillOnce(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Activate(testing::_))
      .WillRepeatedly(testing::Return(AV_SESSION_ERR_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetAVMetadata(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  EXPECT_CALL(*position, GetDuration()).WillRepeatedly(testing::Return(1000));
  EXPECT_CALL(*position, GetElapsedTime()).WillOnce(testing::Return(500));
  EXPECT_CALL(*position, GetUpdateTime()).WillOnce(testing::Return(100));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetPlaybackPosition(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetPlaybackState(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));

  ASSERT_NO_FATAL_FAILURE(adapter->SetPlaybackPosition(position));
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplDestroyAndEraseSessionTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSessionKey_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(adapter->DestroyAndEraseSession());
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplDestroyAndEraseSessionTest2) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  ASSERT_NE(adapter->avSessionKey_, nullptr);
  EXPECT_EQ(adapter->avSessionMap.find(adapter->avSessionKey_->ToString()), adapter->avSessionMap.end());
  ASSERT_NO_FATAL_FAILURE(adapter->DestroyAndEraseSession());
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplDestroyAndEraseSessionTest3) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  ASSERT_NE(adapter->avSessionKey_, nullptr);
  adapter->avSessionMap.insert(
      std::pair<std::string, MediaAVSessionAdapterImpl*>(adapter->avSessionKey_->ToString(), adapter.get()));
  adapter->avSession_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(adapter->DestroyAndEraseSession());
  EXPECT_EQ(adapter->avSessionMap.find(adapter->avSessionKey_->ToString()), adapter->avSessionMap.end());
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplDestroyAndEraseSessionTest4) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  ASSERT_NE(adapter->avSessionKey_, nullptr);
  adapter->avSessionMap.insert(
      std::pair<std::string, MediaAVSessionAdapterImpl*>(adapter->avSessionKey_->ToString(), nullptr));
  ASSERT_NO_FATAL_FAILURE(adapter->DestroyAndEraseSession());
  EXPECT_EQ(adapter->avSessionMap.find(adapter->avSessionKey_->ToString()), adapter->avSessionMap.end());
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplDestroyAndEraseSessionTest5) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  ASSERT_NE(adapter->avSessionKey_, nullptr);
  adapter->avSessionMap.insert(
      std::pair<std::string, MediaAVSessionAdapterImpl*>(adapter->avSessionKey_->ToString(), nullptr));
  adapter->avSession_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(adapter->DestroyAndEraseSession());
  EXPECT_EQ(adapter->avSessionMap.find(adapter->avSessionKey_->ToString()), adapter->avSessionMap.end());
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplDestroyAndEraseSessionTest6) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  ASSERT_NE(adapter->avSessionKey_, nullptr);
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x1234567);
  adapter->avSessionMap.insert(
      std::pair<std::string, MediaAVSessionAdapterImpl*>(adapter->avSessionKey_->ToString(), adapter.get()));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Destroy(testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_INVALID_PARAMETER));
  ASSERT_NO_FATAL_FAILURE(adapter->DestroyAndEraseSession());
  EXPECT_EQ(adapter->avSessionMap.find(adapter->avSessionKey_->ToString()), adapter->avSessionMap.end());
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplDestroyAndEraseSessionTest7) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  ASSERT_NE(adapter->avSessionKey_, nullptr);
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x1234567);
  adapter->avSessionMap.insert(
      std::pair<std::string, MediaAVSessionAdapterImpl*>(adapter->avSessionKey_->ToString(), adapter.get()));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Destroy(testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  ASSERT_NO_FATAL_FAILURE(adapter->DestroyAndEraseSession());
  EXPECT_EQ(adapter->avSessionMap.find(adapter->avSessionKey_->ToString()), adapter->avSessionMap.end());
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplCreateNewSessionTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  auto type = MediaAVSessionType::MEDIA_TYPE_INVALID;
  bool ret = adapter->CreateNewSession(type);
  EXPECT_FALSE(ret);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplCreateNewSessionTest2) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  auto type = MediaAVSessionType::MEDIA_TYPE_AUDIO;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(),
              OH_AVSession_Create(testing::_, testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  bool ret = adapter->CreateNewSession(type);
  EXPECT_TRUE(ret);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplCreateNewSessionTest3) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  auto type = MediaAVSessionType::MEDIA_TYPE_VIDEO;
  ASSERT_NE(adapter->avSessionKey_, nullptr);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(),
              OH_AVSession_Create(testing::_, testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_INVALID_PARAMETER));
  bool ret = adapter->CreateNewSession(type);
  EXPECT_FALSE(ret);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplCreateNewSessionTest4) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  ASSERT_NE(adapter->avSessionKey_, nullptr);

  EXPECT_CALL(OhosInterfaceMock::GetInstance(),
              OH_AVSession_Create(testing::_, testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_INVALID_PARAMETER));

  auto type = MediaAVSessionType::MEDIA_TYPE_AUDIO;
  bool ret = adapter->CreateNewSession(type);
  EXPECT_FALSE(ret);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplCreateNewSessionTest5) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  ASSERT_NE(adapter->avSessionKey_, nullptr);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(),
              OH_AVSession_Create(testing::_, testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  auto type = MediaAVSessionType::MEDIA_TYPE_AUDIO;
  bool ret = adapter->CreateNewSession(type);
  EXPECT_TRUE(ret);
  EXPECT_EQ(adapter->avSessionKey_->GetType(), type);
  EXPECT_NE(adapter->avSessionMap.find(adapter->avSessionKey_->ToString()), adapter->avSessionMap.end());
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplCreateNewSessionTest6) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  ASSERT_NE(adapter->avSessionKey_, nullptr);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(),
              OH_AVSession_Create(testing::_, testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  auto type = MediaAVSessionType::MEDIA_TYPE_VIDEO;
  bool ret = adapter->CreateNewSession(type);
  EXPECT_TRUE(ret);
  EXPECT_EQ(adapter->avSessionKey_->GetType(), type);
  EXPECT_NE(adapter->avSessionMap.find(adapter->avSessionKey_->ToString()), adapter->avSessionMap.end());
}