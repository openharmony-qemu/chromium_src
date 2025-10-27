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

#ifndef ARK_MEDIA_AVSESSION_ADAPTER_H
#define ARK_MEDIA_AVSESSION_ADAPTER_H
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkMediaAVSessionMetadataAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void SetTitle(const ArkWebString& title) = 0;

    /*--ark web()--*/
    virtual ArkWebString GetTitle() = 0;

    /*--ark web()--*/
    virtual void SetArtist(const ArkWebString& artist) = 0;

    /*--ark web()--*/
    virtual ArkWebString GetArtist() = 0;

    /*--ark web()--*/
    virtual void SetAlbum(const ArkWebString& album) = 0;

    /*--ark web()--*/
    virtual ArkWebString GetAlbum() = 0;

    /*--ark web()--*/
    virtual void SetImageUrl(const ArkWebString& imageUrl) = 0;

    /*--ark web()--*/
    virtual ArkWebString GetImageUrl() = 0;
};

/*--ark web(source=webcore)--*/
class ArkMediaAVSessionPositionAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void SetDuration(int64_t duration) = 0;

    /*--ark web()--*/
    virtual int64_t GetDuration() = 0;

    /*--ark web()--*/
    virtual void SetElapsedTime(int64_t elapsedTime) = 0;

    /*--ark web()--*/
    virtual int64_t GetElapsedTime() = 0;

    /*--ark web()--*/
    virtual void SetUpdateTime(int64_t updateTime) = 0;

    /*--ark web()--*/
    virtual int64_t GetUpdateTime() = 0;
};

/*--ark web(source=webcore)--*/
class ArkMediaAVSessionCallbackAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void Play() = 0;

    /*--ark web()--*/
    virtual void Pause() = 0;

    /*--ark web()--*/
    virtual void Stop() = 0;

    /*--ark web()--*/
    virtual void SeekTo(int64_t millisTime) = 0;
};

/*--ark web(source=webview)--*/
class ArkMediaAVSessionAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual bool CreateAVSession(int32_t type) = 0;

    /*--ark web()--*/
    virtual void DestroyAVSession() = 0;

    /*--ark web()--*/
    virtual bool RegistCallback(ArkWebRefPtr<ArkMediaAVSessionCallbackAdapter> callbackAdapter) = 0;

    /*--ark web()--*/
    virtual bool IsActivated() = 0;

    /*--ark web()--*/
    virtual bool Activate() = 0;

    /*--ark web()--*/
    virtual void DeActivate() = 0;

    /*--ark web()--*/
    virtual void SetMetadata(const ArkWebRefPtr<ArkMediaAVSessionMetadataAdapter> metadata) = 0;

    /*--ark web()--*/
    virtual void SetPlaybackState(int32_t state) = 0;

    /*--ark web()--*/
    virtual void SetPlaybackPosition(const ArkWebRefPtr<ArkMediaAVSessionPositionAdapter> position) = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_MEDIA_AVSESSION_ADAPTER_H
