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
  MOCK_METHOD(int32_t, GetMediaCastCurrentTime, (), (override));
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
    OhosInterfaceMock::bAVCastControllerDestroy = type;
    OhosInterfaceMock::bAVSessionRegisterOutputDeviceChangeCallback = type;
    OhosInterfaceMock::bAVSessionUnregisterOutputDeviceChangeCallback = type;
    OhosInterfaceMock::bDeviceInfoGetDeviceName = type;
    OhosInterfaceMock::bAVSessionSetRemoteCastEnabled = type;
    OhosInterfaceMock::bAVSessionStopCasting = type;
    OhosInterfaceMock::bAVSessionCreateAVCastController = type;
    OhosInterfaceMock::bAVSessionAVMediaDescriptionBuilderCreate = type;
    OhosInterfaceMock::bAVSessionAVMediaDescriptionBuilderDestroy = type;
    OhosInterfaceMock::bAVSessionAVMediaDescriptionDestroy = type;
    OhosInterfaceMock::bAVSessionAVMediaDescriptionBuilderSetDuration = type;
    OhosInterfaceMock::bAVSessionAVMediaDescriptionBuilderSetMediaUri = type;
    OhosInterfaceMock::bAVSessionAVMediaDescriptionBuilderSetStartPosition = type;
    OhosInterfaceMock::bAVSessionAVMediaDescriptionBuilderSetMediaType = type;
    OhosInterfaceMock::bAVSessionAVMediaDescriptionBuilderSetTitle = type;
    OhosInterfaceMock::bAVSessionAVMediaDescriptionBuilderSetAssetId = type;
    OhosInterfaceMock::bAVSessionAVMediaDescriptionBuilderSetAlbumCoverUri = type;
    OhosInterfaceMock::bAVSessionAVMediaDescriptionBuilderGenerateAVMediaDescription = type;
    OhosInterfaceMock::bAVSessionAVMediaDescriptionGetMediaUri = type;
    OhosInterfaceMock::bAVSessionAVMediaDescriptionGetStartPosition = type;
    OhosInterfaceMock::bAVSessionAVMediaDescriptionGetDuration = type;
    OhosInterfaceMock::bAVSessionAVMediaDescriptionGetTitle = type;
    OhosInterfaceMock::bAVSessionAVMediaDescriptionGetAssetId = type;
    OhosInterfaceMock::bAVCastControllerPrepare = type;
    OhosInterfaceMock::bAVCastControllerStart = type;
    OhosInterfaceMock::bAVCastControllerRegisterPlaybackStateChangedCallback = type;
    OhosInterfaceMock::bAVCastControllerRegisterMediaItemChangedCallback = type;
    OhosInterfaceMock::bAVCastControllerRegisterSeekDoneCallback = type;
    OhosInterfaceMock::bAVCastControllerRegisterEndOfStreamCallback = type;
    OhosInterfaceMock::bAVCastControllerRegisterErrorCallback = type;
    OhosInterfaceMock::bAVCastControllerUnregisterPlaybackStateChangedCallback = type;
    OhosInterfaceMock::bAVCastControllerUnregisterMediaItemChangedCallback = type;
    OhosInterfaceMock::bAVCastControllerUnregisterSeekDoneCallback = type;
    OhosInterfaceMock::bAVCastControllerUnregisterEndOfStreamCallback = type;
    OhosInterfaceMock::bAVCastControllerUnregisterErrorCallback = type;
    OhosInterfaceMock::bAVSessionGetPlaybackState = type;
    OhosInterfaceMock::bAVSessionGetPlaybackPosition = type;
    OhosInterfaceMock::bAVCastControllerSendCommonCommand = type;
    OhosInterfaceMock::bAVCastControllerSendSeekCommand = type;
    OhosInterfaceMock::bAVCastControllerGetPlaybackState = type;
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
  g_adapter->avSession_ = nullptr;

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
  g_adapter->avSession_ = nullptr;
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
  g_adapter->avSession_ = nullptr;
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
  g_adapter->avSession_ = nullptr;
  g_adapter2->avSession_ = nullptr;
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
  g_adapter->avSession_ = nullptr;
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
  type = MediaAVSessionType::MEDIA_TYPE_INVALID;
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
  adapter->avSession_ = nullptr;
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
  adapter->avSession_ = nullptr;
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
  adapter->avSession_ = nullptr;
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
  adapter->avSession_ = nullptr;
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
  adapter->avSession_ = nullptr;
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
  adapter->avSession_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(adapter->DestroyAVSession());
  EXPECT_EQ(adapter->avSession_, nullptr);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplAVSessionOnCommandCallbackTest1) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);
  auto ret = adapter->AVSessionOnCommandCallback(adapter->avSession_, CONTROL_CMD_PLAY, nullptr);
  EXPECT_EQ(ret, AVSESSION_CALLBACK_RESULT_FAILURE);
  adapter->avSession_ = nullptr;
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplAVSessionOnCommandCallbackTest2) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);

  size_t callback_index = 9999;
  AVSessionCallback_Result result = MediaAVSessionAdapterImpl::AVSessionOnCommandCallback(
      adapter->avSession_, CONTROL_CMD_PLAY, reinterpret_cast<void*>(callback_index));
  EXPECT_EQ(result, AVSESSION_CALLBACK_RESULT_FAILURE);
  adapter->avSession_ = nullptr;
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
  adapter->avSession_ = nullptr;
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
  adapter->avSession_ = nullptr;
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
  adapter->avSession_ = nullptr;
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
  adapter->avSession_ = nullptr;
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplAVSessionOnSeekCallbackTest1) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);
  auto ret = adapter->AVSessionOnSeekCallback(adapter->avSession_, 1000, nullptr);
  EXPECT_EQ(ret, AVSESSION_CALLBACK_RESULT_FAILURE);
  adapter->avSession_ = nullptr;
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplAVSessionOnSeekCallbackTest2) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);
  size_t callback_index = 9999;
  AVSessionCallback_Result result = MediaAVSessionAdapterImpl::AVSessionOnSeekCallback(
      adapter->avSession_, 1000, reinterpret_cast<void*>(callback_index));
  EXPECT_EQ(result, AVSESSION_CALLBACK_RESULT_FAILURE);
  adapter->avSession_ = nullptr;
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
  adapter->avSession_ = nullptr;
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
  adapter->avSession_ = nullptr;
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
  adapter->avSession_ = nullptr;
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
  adapter->avSession_ = nullptr;
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
  adapter->avSession_ = nullptr;
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
  adapter->avSession_ = nullptr;
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplIsActiveTest1) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);
  adapter->isActived_ = true;
  EXPECT_EQ(adapter->IsActivated(), adapter->isActived_);
  adapter->avSession_ = nullptr;
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
  adapter->avSession_ = nullptr;
}

TEST_F(MediaAVSessionAdapterImplTest, MediaAVSessionAdapterImplActiveTest3) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x12345678);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_Activate(testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  EXPECT_EQ(adapter->Activate(), true);
  EXPECT_EQ(adapter->isActived_, true);
  adapter->avSession_ = nullptr;
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
  adapter->avSession_ = nullptr;
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
  adapter->avSession_ = nullptr;
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
  adapter->avSession_ = nullptr;
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
  adapter->avSession_ = nullptr;
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
  adapter->avSession_ = nullptr;
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
  adapter->avSession_ = nullptr;
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
  adapter->avSession_ = nullptr;
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
  adapter->avSession_ = nullptr;
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
  adapter->avSession_ = nullptr;
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
  adapter->avSession_ = nullptr;
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
  adapter->avSession_ = nullptr;
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
  auto position = std::make_shared<MediaAVSessionPositionAdapterMock>();
  EXPECT_CALL(*position, GetDuration()).WillRepeatedly(testing::Return(1000));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetPlaybackPosition(testing::_, testing::_)).Times(1);
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
  EXPECT_CALL(*position, GetElapsedTime()).WillRepeatedly(testing::Return(500));
  EXPECT_CALL(*position, GetUpdateTime()).WillOnce(testing::Return(100));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetPlaybackPosition(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetPlaybackState(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_INVALID_PARAMETER));

  ASSERT_NO_FATAL_FAILURE(adapter->SetPlaybackPosition(position));
  adapter->avSession_ = nullptr;
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
  EXPECT_CALL(*position, GetElapsedTime()).WillRepeatedly(testing::Return(500));
  EXPECT_CALL(*position, GetUpdateTime()).WillOnce(testing::Return(100));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetPlaybackPosition(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetPlaybackState(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_INVALID_PARAMETER));

  ASSERT_NO_FATAL_FAILURE(adapter->SetPlaybackPosition(position));
  adapter->avSession_ = nullptr;
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
  EXPECT_CALL(*position, GetElapsedTime()).WillRepeatedly(testing::Return(500));
  EXPECT_CALL(*position, GetUpdateTime()).WillOnce(testing::Return(100));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetPlaybackPosition(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetPlaybackState(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));

  ASSERT_NO_FATAL_FAILURE(adapter->SetPlaybackPosition(position));
  adapter->avSession_ = nullptr;
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
  EXPECT_CALL(*position, GetElapsedTime()).WillRepeatedly(testing::Return(500));
  EXPECT_CALL(*position, GetUpdateTime()).WillOnce(testing::Return(100));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetPlaybackPosition(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetPlaybackState(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));

  ASSERT_NO_FATAL_FAILURE(adapter->SetPlaybackPosition(position));
  adapter->avSession_ = nullptr;
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
  adapter->avSession_ = nullptr;
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
  adapter->avSession_ = nullptr;
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

TEST_F(MediaAVSessionAdapterImplTest, IsUrlInQueueTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  std::string testUrl = "https://example.com/poster.jpg";
  EXPECT_FALSE(adapter->IsUrlInQueue(testUrl));
}

TEST_F(MediaAVSessionAdapterImplTest, IsUrlInQueueTest2) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  std::string testUrl = "https://example.com/poster.jpg";
  adapter->url_queue_.push_back(testUrl);
  EXPECT_TRUE(adapter->IsUrlInQueue(testUrl));
}

TEST_F(MediaAVSessionAdapterImplTest, AddUrlTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  std::string testUrl = "https://example.com/poster.jpg";
  ASSERT_NO_FATAL_FAILURE(adapter->AddUrl(testUrl));
  EXPECT_TRUE(adapter->IsUrlInQueue(testUrl));
}

TEST_F(MediaAVSessionAdapterImplTest, AddUrlTest2) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  std::string testUrl = "https://example.com/poster.jpg";
  adapter->url_queue_.push_back(testUrl);
  ASSERT_NO_FATAL_FAILURE(adapter->AddUrl(testUrl));
  EXPECT_EQ(adapter->url_queue_.size(), 1);
}

TEST_F(MediaAVSessionAdapterImplTest, AddUrlTest3) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->url_queue_.push_back("url1");
  adapter->url_queue_.push_back("url2");
  std::string testUrl = "https://example.com/poster.jpg";
  ASSERT_NO_FATAL_FAILURE(adapter->AddUrl(testUrl));
  EXPECT_EQ(adapter->url_queue_.size(), 2);
  EXPECT_FALSE(adapter->IsUrlInQueue("url1"));
}

TEST_F(MediaAVSessionAdapterImplTest, StartAsyncPosterUpdateTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  bool ret = adapter->StartAsyncPosterUpdate();
  EXPECT_TRUE(ret);
}

TEST_F(MediaAVSessionAdapterImplTest, StartAsyncPosterUpdateTest2) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->url_queue_.push_back("url1");
  bool ret = adapter->StartAsyncPosterUpdate();
  EXPECT_TRUE(ret);
}

TEST_F(MediaAVSessionAdapterImplTest, ProcessPosterQueueTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->builder_ = reinterpret_cast<OH_AVMetadataBuilder*>(0x12345678);
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x1234567);
  adapter->url_queue_.push_back("https://example.com/poster.jpg");
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetMediaImageUri(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadata_Destroy(testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AV_SESSION_ERR_SUCCESS));
  ASSERT_NO_FATAL_FAILURE(adapter->ProcessPosterQueue());
  adapter->avSession_ = nullptr;
}

TEST_F(MediaAVSessionAdapterImplTest, ProcessPosterQueueTest2) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->builder_ = reinterpret_cast<OH_AVMetadataBuilder*>(0x12345678);
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x1234567);
  adapter->url_queue_.push_back("https://example.com/poster.jpg");
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_SetMediaImageUri(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_ERROR_INVALID_PARAM));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadata_Destroy(testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AV_SESSION_ERR_SUCCESS));
  ASSERT_NO_FATAL_FAILURE(adapter->ProcessPosterQueue());
  adapter->avSession_ = nullptr;
}

TEST_F(MediaAVSessionAdapterImplTest, UpdateUiPlayStateByClientTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  AVSession_PlaybackState state = AVSession_PlaybackState::PLAYBACK_STATE_PLAYING;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  ASSERT_NO_FATAL_FAILURE(adapter->UpdateUiPlayStateByClient(state));
}

TEST_F(MediaAVSessionAdapterImplTest, UpdateUiPlayStateByClientTest2) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->playbackState_ = AVSession_PlaybackState::PLAYBACK_STATE_PAUSED;
  AVSession_PlaybackState state = AVSession_PlaybackState::PLAYBACK_STATE_PLAYING;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  ASSERT_NO_FATAL_FAILURE(adapter->UpdateUiPlayStateByClient(state));
}

TEST_F(MediaAVSessionAdapterImplTest, SetUiPlayStateByClientTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  AVSession_PlaybackState state = AVSession_PlaybackState::PLAYBACK_STATE_PLAYING;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  ASSERT_NO_FATAL_FAILURE(adapter->SetUiPlayStateByClient(state));
  EXPECT_EQ(adapter->playbackState_, state);
}

TEST_F(MediaAVSessionAdapterImplTest, SetUiPlayPositionByClientTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  AVSession_PlaybackPosition position = {1000, 2000};
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  ASSERT_NO_FATAL_FAILURE(adapter->SetUiPlayPositionByClient(position));
  EXPECT_EQ(adapter->playbackPosition_.elapsedTime, 1000);
}

TEST_F(MediaAVSessionAdapterImplTest, SetUilastUiTimeByClientTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  int64_t position = 5000;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  ASSERT_NO_FATAL_FAILURE(adapter->SetUilastUiTimeByClient(position));
  EXPECT_EQ(adapter->lastUiTime_, position);
}

TEST_F(MediaAVSessionAdapterImplTest, SetUiSeekingByClientTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  ASSERT_NO_FATAL_FAILURE(adapter->SetUiSeekingByClient(true));
  EXPECT_TRUE(adapter->is_seeking_);
}

TEST_F(MediaAVSessionAdapterImplTest, GetUiPlayStateByClientTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->playbackState_ = AVSession_PlaybackState::PLAYBACK_STATE_PLAYING;
  AVSession_PlaybackState state = adapter->GetUiPlayStateByClient();
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  EXPECT_EQ(state, AVSession_PlaybackState::PLAYBACK_STATE_PLAYING);
}

TEST_F(MediaAVSessionAdapterImplTest, GetUilastUiTimeByClientTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->lastUiTime_ = 10000;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVMetadataBuilder_GenerateAVMetadata(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AVMETADATA_SUCCESS));
  int64_t time = adapter->GetUilastUiTimeByClient();
  EXPECT_EQ(time, 10000);
}

TEST_F(MediaAVSessionAdapterImplTest, GetUiSeekingByClientTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->is_seeking_ = true;
  bool seeking = adapter->GetUiSeekingByClient();
  EXPECT_TRUE(seeking);
}

TEST_F(MediaAVSessionAdapterImplTest, SetAvCastTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  ASSERT_NO_FATAL_FAILURE(adapter->SetAvCast(true));
  EXPECT_TRUE(adapter->is_avcast_);
}

TEST_F(MediaAVSessionAdapterImplTest, UpdateUiPlayStateTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  ASSERT_NO_FATAL_FAILURE(adapter->UpdateUiPlayState(true));
}

TEST_F(MediaAVSessionAdapterImplTest, UpdateUiPlayPositionTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  int64_t position = 3000;
  ASSERT_NO_FATAL_FAILURE(adapter->UpdateUiPlayPosition(position));
}

TEST_F(MediaAVSessionAdapterImplTest, UpdateRemotePlayStateTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avCastStarted_ = true;
  adapter->avCastController_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(adapter->UpdateRemotePlayState(true));
}

TEST_F(MediaAVSessionAdapterImplTest, UpdateRemotePlayPositionTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avCastStarted_ = true;
  adapter->avCastController_ = nullptr;
  int64_t position = 4000;
  ASSERT_NO_FATAL_FAILURE(adapter->UpdateRemotePlayPosition(position));
}

TEST_F(MediaAVSessionAdapterImplTest, SeekNativeTest) {
  SetAllMockType(true);
  auto callback = std::make_shared<MediaAVSessionCallbackAdapterMock>();
  EXPECT_CALL(*callback, SeekTo(testing::_)).Times(1);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->callback_index_ = adapter->callback_wrapper_.AddCallback(callback);
  int64_t millis = 5000;
  ASSERT_NO_FATAL_FAILURE(adapter->SeekNative(millis));
}

TEST_F(MediaAVSessionAdapterImplTest, PlayNativeTest) {
  SetAllMockType(true);
  auto callback = std::make_shared<MediaAVSessionCallbackAdapterMock>();
  EXPECT_CALL(*callback, Play()).Times(1);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->callback_index_ = adapter->callback_wrapper_.AddCallback(callback);
  ASSERT_NO_FATAL_FAILURE(adapter->PlayNative());
}

TEST_F(MediaAVSessionAdapterImplTest, PauseNativeTest) {
  SetAllMockType(true);
  auto callback = std::make_shared<MediaAVSessionCallbackAdapterMock>();
  EXPECT_CALL(*callback, Pause()).Times(1);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->callback_index_ = adapter->callback_wrapper_.AddCallback(callback);
  ASSERT_NO_FATAL_FAILURE(adapter->PauseNative());
}

TEST_F(MediaAVSessionAdapterImplTest, GetAVCastPlaybackStateTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->playbackState_ = AVSession_PlaybackState::PLAYBACK_STATE_PLAYING;
  AVSession_PlaybackState state = adapter->GetAVCastPlaybackState();
  EXPECT_EQ(state, AVSession_PlaybackState::PLAYBACK_STATE_PLAYING);
}

TEST_F(MediaAVSessionAdapterImplTest, IsAvCastPlayingTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->playbackState_ = AVSession_PlaybackState::PLAYBACK_STATE_PLAYING;
  EXPECT_TRUE(adapter->IsAvCastPlaying());
}

TEST_F(MediaAVSessionAdapterImplTest, IsAvCastPlayingTest2) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->playbackState_ = AVSession_PlaybackState::PLAYBACK_STATE_PAUSED;
  EXPECT_FALSE(adapter->IsAvCastPlaying());
}

TEST_F(MediaAVSessionAdapterImplTest, GetPlaybackPositionTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->playbackPosition_.elapsedTime = 6000;
  int64_t position = adapter->GetPlaybackPosition();
  EXPECT_EQ(position, 6000);
}

TEST_F(MediaAVSessionAdapterImplTest, SetAVCastUiPlayStateTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  AVSession_PlaybackState state = AVSession_PlaybackState::PLAYBACK_STATE_PLAYING;
  ASSERT_NO_FATAL_FAILURE(adapter->SetAVCastUiPlayState(state));
  EXPECT_EQ(adapter->playbackState_, state);
}

TEST_F(MediaAVSessionAdapterImplTest, SetAVCastUiPlayPositionTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  AVSession_PlaybackPosition position = {7000, 8000};
  ASSERT_NO_FATAL_FAILURE(adapter->SetAVCastUiPlayPosition(position));
  EXPECT_EQ(adapter->playbackPosition_.elapsedTime, 7000);
}

TEST_F(MediaAVSessionAdapterImplTest, SetAVCastUilastUiTimeTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  int64_t position = 9000;
  ASSERT_NO_FATAL_FAILURE(adapter->SetAVCastUilastUiTime(position));
  EXPECT_EQ(adapter->lastUiTime_, position);
}

TEST_F(MediaAVSessionAdapterImplTest, SetAVCastUiSeekingTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  ASSERT_NO_FATAL_FAILURE(adapter->SetAVCastUiSeeking(true));
  EXPECT_TRUE(adapter->is_seeking_);
}

TEST_F(MediaAVSessionAdapterImplTest, GetAVCastUiPlayStateTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->playbackState_ = AVSession_PlaybackState::PLAYBACK_STATE_PLAYING;
  AVSession_PlaybackState state = adapter->GetAVCastUiPlayState();
  EXPECT_EQ(state, AVSession_PlaybackState::PLAYBACK_STATE_PLAYING);
}

TEST_F(MediaAVSessionAdapterImplTest, GetAVCastUiPlayPositionTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->playbackPosition_.elapsedTime = 10000;
  AVSession_PlaybackPosition position = adapter->GetAVCastUiPlayPosition();
  EXPECT_EQ(position.elapsedTime, 10000);
}

TEST_F(MediaAVSessionAdapterImplTest, GetAVCastUilastUiTimeTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->lastUiTime_ = 11000;
  int64_t time = adapter->GetAVCastUilastUiTime();
  EXPECT_EQ(time, 11000);
}

TEST_F(MediaAVSessionAdapterImplTest, GetAVCastUiSeekingTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->is_seeking_ = true;
  bool seeking = adapter->GetAVCastUiSeeking();
  EXPECT_TRUE(seeking);
}

TEST_F(MediaAVSessionAdapterImplTest, SetAVCastDeviceTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  const char* deviceName = "TestDevice";
  ASSERT_NO_FATAL_FAILURE(adapter->SetAVCastDevice(deviceName));
  EXPECT_EQ(adapter->deviceName_, "TestDevice");
}

TEST_F(MediaAVSessionAdapterImplTest, SetAVCastDeviceTest2) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  ASSERT_NO_FATAL_FAILURE(adapter->SetAVCastDevice(nullptr));
  EXPECT_EQ(adapter->deviceName_, "");
}

TEST_F(MediaAVSessionAdapterImplTest, GetAVCastDeviceTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->deviceName_ = "MyDevice";
  std::string device = adapter->GetAVCastDevice();
  EXPECT_EQ(device, "MyDevice");
}

TEST_F(MediaAVSessionAdapterImplTest, PrepareMediaCastDescriptionTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = nullptr;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_RegisterOutputDeviceChangeCallback(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  ASSERT_NO_FATAL_FAILURE(adapter->PrepareMediaCastDescription());
}

TEST_F(MediaAVSessionAdapterImplTest, GetMediaCastCurrentTimeTest) {
  SetAllMockType(true);
  auto callback = std::make_shared<MediaAVSessionCallbackAdapterMock>();
  EXPECT_CALL(*callback, GetMediaCastCurrentTime()).WillOnce(testing::Return(5000));
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->callback_index_ = adapter->callback_wrapper_.AddCallback(callback);
  int32_t time = adapter->GetMediaCastCurrentTime();
  EXPECT_EQ(time, 5000);
}

TEST_F(MediaAVSessionAdapterImplTest, GetMediaCastCurrentTimeTest2) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  int32_t time = adapter->GetMediaCastCurrentTime();
  EXPECT_EQ(time, 0);
}

TEST_F(MediaAVSessionAdapterImplTest, SetRemoteCastEnabledTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = nullptr;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_SetRemoteCastEnabled(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  ASSERT_NO_FATAL_FAILURE(adapter->SetRemoteCastEnabled(true));
}

TEST_F(MediaAVSessionAdapterImplTest, SetMediaCastUriTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSessionKey_ = std::make_shared<MediaAVSessionKey>();
  adapter->avSessionKey_->pid_ = 12345;
  adapter->title_ = "TestTitle";
  adapter->album_url_ = "https://example.com/album.jpg";
  std::string mediaUri = "https://example.com/video.mp4";
  ASSERT_NO_FATAL_FAILURE(adapter->SetMediaCastUri(mediaUri));
  EXPECT_EQ(adapter->media_uri_storage_, mediaUri);
}

TEST_F(MediaAVSessionAdapterImplTest, HandleStopMediaCastTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = nullptr;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_StopCasting(testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  ASSERT_NO_FATAL_FAILURE(adapter->HandleStopMediaCast());
}

TEST_F(MediaAVSessionAdapterImplTest, RegistAVSessionCallbackOutputDeviceChangeTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = nullptr;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_RegisterOutputDeviceChangeCallback(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  ASSERT_NO_FATAL_FAILURE(adapter->RegistAVSessionCallbackOutputDeviceChange());
}

TEST_F(MediaAVSessionAdapterImplTest, UpdateAVCastDeviceTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  AVSession_OutputDeviceInfo outputDeviceInfo = {};
  AVSession_DeviceInfo* deviceInfoPtr = reinterpret_cast<AVSession_DeviceInfo*>(0x12345678);
  outputDeviceInfo.deviceInfos = &deviceInfoPtr;
  char deviceName[] = "TestDevice";
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_DeviceInfo_GetDeviceName(testing::_, testing::_))
      .WillOnce(testing::DoAll(testing::SetArgPointee<1>(deviceName), testing::Return(AV_SESSION_ERR_SUCCESS)));
  ASSERT_NO_FATAL_FAILURE(adapter->UpdateAVCastDevice(&outputDeviceInfo));
  EXPECT_EQ(adapter->deviceName_, "TestDevice");
}

TEST_F(MediaAVSessionAdapterImplTest, UpdateAVCastDeviceTest2) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  ASSERT_NO_FATAL_FAILURE(adapter->UpdateAVCastDevice(nullptr));
}

TEST_F(MediaAVSessionAdapterImplTest, UpdateAVCastDeviceTest3) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  AVSession_OutputDeviceInfo outputDeviceInfo = {};
  outputDeviceInfo.deviceInfos = nullptr;
  ASSERT_NO_FATAL_FAILURE(adapter->UpdateAVCastDevice(&outputDeviceInfo));
}

TEST_F(MediaAVSessionAdapterImplTest, PullUpCastBackGroundTest) {
  SetAllMockType(true);
  auto callback = std::make_shared<MediaAVSessionCallbackAdapterMock>();
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->deviceName_ = "TestDevice";
  adapter->callback_index_ = adapter->callback_wrapper_.AddCallback(callback);
  ASSERT_NO_FATAL_FAILURE(adapter->PullUpCastBackGround());
}

TEST_F(MediaAVSessionAdapterImplTest, MediaCastStoppedTest) {
  SetAllMockType(true);
  auto callback = std::make_shared<MediaAVSessionCallbackAdapterMock>();
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->callback_index_ = adapter->callback_wrapper_.AddCallback(callback);
  ASSERT_NO_FATAL_FAILURE(adapter->MediaCastStopped());
}

TEST_F(MediaAVSessionAdapterImplTest, UnregisterMediaCastOutputDeviceCallbackTest) {
  SetAllMockType(true);
  auto callback = std::make_shared<MediaAVSessionCallbackAdapterMock>();
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->callback_index_ = adapter->callback_wrapper_.AddCallback(callback);
  adapter->avSession_ = nullptr;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_UnregisterOutputDeviceChangeCallback(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  ASSERT_NO_FATAL_FAILURE(adapter->UnregisterMediaCastOutputDeviceCallback());
}

TEST_F(MediaAVSessionAdapterImplTest, GetAVCastControllerTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x1234567);
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_CreateAVCastController(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  bool ret = adapter->GetAVCastController();
  EXPECT_TRUE(ret);
  adapter->avSession_ = nullptr;
}

TEST_F(MediaAVSessionAdapterImplTest, GetAVCastControllerTest2) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = nullptr;
  bool ret = adapter->GetAVCastController();
  EXPECT_FALSE(ret);
}

TEST_F(MediaAVSessionAdapterImplTest, GetAVCastControllerTest3) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = nullptr;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_CreateAVCastController(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_INVALID_PARAMETER));
  bool ret = adapter->GetAVCastController();
  EXPECT_FALSE(ret);
}

TEST_F(MediaAVSessionAdapterImplTest, PrepareTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  MediaCastDescription description;
  description.duration = 10000;
  description.startPosition = 0;
  description.mediaUri = "https://example.com/video.mp4";
  description.mediaType = "VIDEO";
  description.title = "Test Video";
  description.assetId = "12345";
  description.albumUrl = "https://example.com/album.jpg";
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_AVMediaDescriptionBuilder_Create(testing::_))
      .WillOnce(testing::Return(AVQUEUEITEM_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_AVMediaDescriptionBuilder_SetDuration(testing::_, testing::_))
      .WillOnce(testing::Return(AVQUEUEITEM_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_AVMediaDescriptionBuilder_SetMediaUri(testing::_, testing::_))
      .WillOnce(testing::Return(AVQUEUEITEM_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_AVMediaDescriptionBuilder_SetStartPosition(testing::_, testing::_))
      .WillOnce(testing::Return(AVQUEUEITEM_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_AVMediaDescriptionBuilder_SetMediaType(testing::_, testing::_))
      .WillOnce(testing::Return(AVQUEUEITEM_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_AVMediaDescriptionBuilder_SetTitle(testing::_, testing::_))
      .WillOnce(testing::Return(AVQUEUEITEM_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_AVMediaDescriptionBuilder_SetAssetId(testing::_, testing::_))
      .WillOnce(testing::Return(AVQUEUEITEM_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_AVMediaDescriptionBuilder_SetAlbumCoverUri(testing::_, testing::_))
      .WillOnce(testing::Return(AVQUEUEITEM_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_AVMediaDescriptionBuilder_GenerateAVMediaDescription(testing::_, testing::_))
      .WillOnce(testing::Return(AVQUEUEITEM_SUCCESS));
  bool ret = adapter->Prepare(description);
  EXPECT_TRUE(ret);
}

TEST_F(MediaAVSessionAdapterImplTest, PrepareTest2) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  MediaCastDescription description;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_AVMediaDescriptionBuilder_Create(testing::_))
      .WillOnce(testing::Return(AVQUEUEITEM_ERROR_INVALID_PARAM));
  bool ret = adapter->Prepare(description);
  EXPECT_FALSE(ret);
}

TEST_F(MediaAVSessionAdapterImplTest, PlayRemoteTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avCastStarted_ = true;
  adapter->avCastController_ = nullptr;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVCastController_SendCommonCommand(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  ASSERT_NO_FATAL_FAILURE(adapter->PlayRemote());
}

TEST_F(MediaAVSessionAdapterImplTest, PlayRemoteTest2) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avCastStarted_ = false;
  ASSERT_NO_FATAL_FAILURE(adapter->PlayRemote());
}

TEST_F(MediaAVSessionAdapterImplTest, PlayRemoteTest3) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avCastStarted_ = true;
  adapter->avCastController_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(adapter->PlayRemote());
}

TEST_F(MediaAVSessionAdapterImplTest, PauseRemoteTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avCastStarted_ = true;
  adapter->avCastController_ = nullptr;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVCastController_SendCommonCommand(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  ASSERT_NO_FATAL_FAILURE(adapter->PauseRemote());
}

TEST_F(MediaAVSessionAdapterImplTest, PauseRemoteTest2) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avCastStarted_ = false;
  ASSERT_NO_FATAL_FAILURE(adapter->PauseRemote());
}

TEST_F(MediaAVSessionAdapterImplTest, SetPlaybackPositionRemoteTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avCastStarted_ = true;
  adapter->avCastController_ = nullptr;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVCastController_SendSeekCommand(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  ASSERT_NO_FATAL_FAILURE(adapter->SetPlaybackPositionRemote(5000));
}

TEST_F(MediaAVSessionAdapterImplTest, SetPlaybackPositionRemoteTest2) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avCastStarted_ = false;
  ASSERT_NO_FATAL_FAILURE(adapter->SetPlaybackPositionRemote(5000));
}

TEST_F(MediaAVSessionAdapterImplTest, UpdateRemotePlayStateCastTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avCastStarted_ = true;
  adapter->avCastController_ = nullptr;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVCastController_SendCommonCommand(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AV_SESSION_ERR_SUCCESS));
  ASSERT_NO_FATAL_FAILURE(adapter->UpdateRemotePlayStateCast(true));
}

TEST_F(MediaAVSessionAdapterImplTest, UpdateRemotePlayStateCastTest2) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avCastStarted_ = true;
  adapter->avCastController_ = nullptr;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVCastController_SendCommonCommand(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AV_SESSION_ERR_SUCCESS));
  ASSERT_NO_FATAL_FAILURE(adapter->UpdateRemotePlayStateCast(false));
}

TEST_F(MediaAVSessionAdapterImplTest, UpdateRemotePlayPositionCastTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avCastStarted_ = true;
  adapter->avCastController_ = nullptr;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVCastController_SendSeekCommand(testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  ASSERT_NO_FATAL_FAILURE(adapter->UpdateRemotePlayPositionCast(6000));
  EXPECT_TRUE(adapter->is_seeking_);
}

TEST_F(MediaAVSessionAdapterImplTest, RegisterCallbackTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avCastController_ = nullptr;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVCastController_RegisterPlaybackStateChangedCallback(testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVCastController_RegisterMediaItemChangedCallback(testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVCastController_RegisterSeekDoneCallback(testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVCastController_RegisterEndOfStreamCallback(testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVCastController_RegisterErrorCallback(testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_SUCCESS));
  bool ret = adapter->RegisterCallback();
  EXPECT_TRUE(ret);
}

TEST_F(MediaAVSessionAdapterImplTest, RegisterCallbackTest2) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avCastController_ = nullptr;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVCastController_RegisterPlaybackStateChangedCallback(testing::_, testing::_, testing::_, testing::_))
      .WillOnce(testing::Return(AV_SESSION_ERR_INVALID_PARAMETER));
  bool ret = adapter->RegisterCallback();
  EXPECT_FALSE(ret);
}

TEST_F(MediaAVSessionAdapterImplTest, UnregisterCallbackTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avCastController_ = nullptr;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVCastController_UnregisterPlaybackStateChangedCallback(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AV_SESSION_ERR_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVCastController_UnregisterMediaItemChangedCallback(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AV_SESSION_ERR_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVCastController_UnregisterSeekDoneCallback(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AV_SESSION_ERR_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVCastController_UnregisterEndOfStreamCallback(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AV_SESSION_ERR_SUCCESS));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVCastController_UnregisterErrorCallback(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AV_SESSION_ERR_SUCCESS));
  bool ret = adapter->UnregisterCallback();
  EXPECT_TRUE(ret);
}

TEST_F(MediaAVSessionAdapterImplTest, UnregisterCallbackTest2) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avCastController_ = nullptr;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVCastController_UnregisterPlaybackStateChangedCallback(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AV_SESSION_ERR_INVALID_PARAMETER));
  bool ret = adapter->UnregisterCallback();
  EXPECT_FALSE(ret);
}

TEST_F(MediaAVSessionAdapterImplTest, OutputDeviceChangeCallbackTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = nullptr;
  adapter->avSessionMapOther_.clear();
  adapter->avSessionMapOther_[adapter->avSession_] = adapter.get();
  AVSession_OutputDeviceInfo outputDeviceInfo = {};
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVCastController_UnregisterPlaybackStateChangedCallback(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AV_SESSION_ERR_SUCCESS));
  AVSessionCallback_Result result = adapter->OutputDeviceChangeCallback(adapter->avSession_, AVSession_ConnectionState::STATE_CONNECTED, &outputDeviceInfo);
  EXPECT_EQ(result, AVSESSION_CALLBACK_RESULT_SUCCESS);
}

TEST_F(MediaAVSessionAdapterImplTest, OutputDeviceChangeCallbackTest2) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = nullptr;
  adapter->avSessionMapOther_.clear();
  adapter->avSessionMapOther_[adapter->avSession_] = adapter.get();
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVCastController_UnregisterPlaybackStateChangedCallback(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AV_SESSION_ERR_SUCCESS));
  AVSessionCallback_Result result = adapter->OutputDeviceChangeCallback(adapter->avSession_, AVSession_ConnectionState::STATE_DISCONNECTED, nullptr);
  EXPECT_EQ(result, AVSESSION_CALLBACK_RESULT_SUCCESS);
}

TEST_F(MediaAVSessionAdapterImplTest, AVCastStateConnectTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = nullptr;
  adapter->avSessionMapOther_.clear();
  adapter->avSessionMapOther_[adapter->avSession_] = adapter.get();
  adapter->deviceName_ = "RemoteDevice";
  AVSession_OutputDeviceInfo outputDeviceInfo = {};
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVCastController_UnregisterPlaybackStateChangedCallback(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AV_SESSION_ERR_SUCCESS));
  ASSERT_NO_FATAL_FAILURE(adapter->AVCastStateConnect(adapter->avSession_, &outputDeviceInfo));
}

TEST_F(MediaAVSessionAdapterImplTest, AVCastStateConnectTest2) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = nullptr;
  adapter->avSessionMapOther_.clear();
  adapter->avSessionMapOther_[adapter->avSession_] = adapter.get();
  adapter->deviceName_ = "LocalDevice";
  AVSession_OutputDeviceInfo outputDeviceInfo = {};
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVCastController_UnregisterPlaybackStateChangedCallback(testing::_, testing::_))
      .WillRepeatedly(testing::Return(AV_SESSION_ERR_SUCCESS));
  ASSERT_NO_FATAL_FAILURE(adapter->AVCastStateConnect(adapter->avSession_, &outputDeviceInfo));
}

TEST_F(MediaAVSessionAdapterImplTest, AVCastStateDisconnectTest) {
  SetAllMockType(true);
  auto callback = std::make_shared<MediaAVSessionCallbackAdapterMock>();
  EXPECT_CALL(*callback, SeekTo(testing::_)).Times(1);
  EXPECT_CALL(*callback, Play()).Times(1);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->callback_index_ = adapter->callback_wrapper_.AddCallback(callback);
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x1234567);
  adapter->playbackState_ = AVSession_PlaybackState::PLAYBACK_STATE_PLAYING;
  adapter->playbackPosition_.elapsedTime = 5000;
  adapter->avSessionMapOther_.clear();
  adapter->avSessionMapOther_[adapter->avSession_] = adapter.get();
  ASSERT_NO_FATAL_FAILURE(adapter->AVCastStateDisconnect(adapter->avSession_));
  EXPECT_FALSE(adapter->is_avcast_);
  adapter->avSession_ = nullptr;
}

TEST_F(MediaAVSessionAdapterImplTest, AVCastStateDisconnectDefaultTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->avSession_ = reinterpret_cast<OH_AVSession*>(0x1234567);
  adapter->avSessionMapOther_.clear();
  adapter->avSessionMapOther_[adapter->avSession_] = adapter.get();
  ASSERT_NO_FATAL_FAILURE(adapter->AVCastStateDisconnectDefault(adapter->avSession_));
  EXPECT_TRUE(adapter->is_error_);
  adapter->avSession_ = nullptr;
}

TEST_F(MediaAVSessionAdapterImplTest, UpdateUiPlayPositionStaticTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  int64_t position = 5000;
  bool is_seek = true;
  ASSERT_NO_FATAL_FAILURE(MediaAVSessionAdapterImpl::UpdateUiPlayPosition(adapter, position, is_seek));
}

TEST_F(MediaAVSessionAdapterImplTest, UpdateUiPlayPositionStaticTest2) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  ASSERT_NO_FATAL_FAILURE(MediaAVSessionAdapterImpl::UpdateUiPlayPosition(nullptr, 5000, false));
}

TEST_F(MediaAVSessionAdapterImplTest, UpdateUiPlayPositionStaticTest3) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->lastUiTime_ = 4800;
  ASSERT_NO_FATAL_FAILURE(MediaAVSessionAdapterImpl::UpdateUiPlayPosition(adapter, 5000, false));
}

TEST_F(MediaAVSessionAdapterImplTest, UpdateUiPlayPositionStaticTest4) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  adapter->is_seeking_ = true;
  ASSERT_NO_FATAL_FAILURE(MediaAVSessionAdapterImpl::UpdateUiPlayPosition(adapter, 5000, false));
}

TEST_F(MediaAVSessionAdapterImplTest, PlaybackStateChangedCallbackTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  size_t callback_index = adapter->avsession_callback_wrapper_.AddCallback(adapter);
  OH_AVSession_AVPlaybackState* playbackState = nullptr;
  AVSession_PlaybackState state = AVSession_PlaybackState::PLAYBACK_STATE_PLAYING;
  AVSession_PlaybackPosition position;
  position.elapsedTime = 5000;
  position.updateTime = 6000;
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_GetPlaybackState(testing::_, testing::_))
      .WillOnce(testing::DoAll(testing::SetArgPointee<1>(state), testing::Return(AV_SESSION_ERR_SUCCESS)));
  EXPECT_CALL(OhosInterfaceMock::GetInstance(), OH_AVSession_GetPlaybackPosition(testing::_, testing::_))
      .WillOnce(testing::DoAll(testing::SetArgPointee<1>(position), testing::Return(AV_SESSION_ERR_SUCCESS)));
  AVSessionCallback_Result result = adapter->PlaybackStateChangedCallback(nullptr, playbackState, reinterpret_cast<void*>(callback_index));
  EXPECT_EQ(result, AVSESSION_CALLBACK_RESULT_SUCCESS);
}

TEST_F(MediaAVSessionAdapterImplTest, PlaybackStateChangedCallbackTest2) {
  SetAllMockType(true);
  AVSessionCallback_Result result = MediaAVSessionAdapterImpl::PlaybackStateChangedCallback(nullptr, nullptr, nullptr);
  EXPECT_EQ(result, AVSESSION_CALLBACK_RESULT_FAILURE);
}

TEST_F(MediaAVSessionAdapterImplTest, MediaItemChangeCallbackTest) {
  SetAllMockType(true);
  AVSessionCallback_Result result = MediaAVSessionAdapterImpl::MediaItemChangeCallback(nullptr, nullptr, nullptr);
  EXPECT_EQ(result, AVSESSION_CALLBACK_RESULT_SUCCESS);
}

TEST_F(MediaAVSessionAdapterImplTest, SeekDoneCallbackTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  size_t callback_index = adapter->avsession_callback_wrapper_.AddCallback(adapter);
  AVSessionCallback_Result result = adapter->SeekDoneCallback(nullptr, 5000, reinterpret_cast<void*>(callback_index));
  EXPECT_EQ(result, AVSESSION_CALLBACK_RESULT_SUCCESS);
}

TEST_F(MediaAVSessionAdapterImplTest, SeekDoneCallbackTest2) {
  SetAllMockType(true);
  AVSessionCallback_Result result = MediaAVSessionAdapterImpl::SeekDoneCallback(nullptr, 5000, nullptr);
  EXPECT_EQ(result, AVSESSION_CALLBACK_RESULT_FAILURE);
}

TEST_F(MediaAVSessionAdapterImplTest, EndOfStreamCallbackTest) {
  SetAllMockType(true);
  AVSessionCallback_Result result = MediaAVSessionAdapterImpl::EndOfStreamCallback(nullptr, nullptr);
  EXPECT_EQ(result, AVSESSION_CALLBACK_RESULT_SUCCESS);
}

TEST_F(MediaAVSessionAdapterImplTest, ErrorCallbackTest) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  size_t callback_index = adapter->avsession_callback_wrapper_.AddCallback(adapter);
  AVSessionCallback_Result result = adapter->ErrorCallback(nullptr, reinterpret_cast<void*>(callback_index), AV_SESSION_ERR_SUCCESS);
  EXPECT_EQ(result, AVSESSION_CALLBACK_RESULT_SUCCESS);
  EXPECT_FALSE(adapter->is_error_);
}

TEST_F(MediaAVSessionAdapterImplTest, ErrorCallbackTest2) {
  SetAllMockType(true);
  auto adapter = std::make_shared<MediaAVSessionAdapterImpl>();
  size_t callback_index = adapter->avsession_callback_wrapper_.AddCallback(adapter);
  AVSessionCallback_Result result = adapter->ErrorCallback(nullptr, reinterpret_cast<void*>(callback_index), AV_SESSION_ERR_INVALID_PARAMETER);
  EXPECT_EQ(result, AVSESSION_CALLBACK_RESULT_SUCCESS);
  EXPECT_TRUE(adapter->is_error_);
}

TEST_F(MediaAVSessionAdapterImplTest, ErrorCallbackTest3) {
  SetAllMockType(true);
  AVSessionCallback_Result result = MediaAVSessionAdapterImpl::ErrorCallback(nullptr, nullptr, AV_SESSION_ERR_SUCCESS);
  EXPECT_EQ(result, AVSESSION_CALLBACK_RESULT_FAILURE);
}