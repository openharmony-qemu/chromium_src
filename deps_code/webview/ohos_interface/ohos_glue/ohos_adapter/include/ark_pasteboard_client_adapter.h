/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef ARK_PASTEBOARD_CLIENT_ADAPTER_H
#define ARK_PASTEBOARD_CLIENT_ADAPTER_H
#pragma once

#include "ohos_adapter/include/ark_paste_record_vector.h"

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkClipBoardImageDataAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t GetColorType() = 0;

    /*--ark web()--*/
    virtual int32_t GetAlphaType() = 0;

    /*--ark web()--*/
    virtual uint32_t* GetData() = 0;

    /*--ark web()--*/
    virtual size_t GetDataSize() = 0;

    /*--ark web()--*/
    virtual size_t GetRowBytes() = 0;

    /*--ark web()--*/
    virtual int32_t GetWidth() = 0;

    /*--ark web()--*/
    virtual int32_t GetHeight() = 0;

    /*--ark web()--*/
    virtual void SetColorType(int32_t color) = 0;

    /*--ark web()--*/
    virtual void SetAlphaType(int32_t alpha) = 0;

    /*--ark web()--*/
    virtual void SetData(uint32_t* data) = 0;

    /*--ark web()--*/
    virtual void SetDataSize(size_t size) = 0;

    /*--ark web()--*/
    virtual void SetRowBytes(size_t rowBytes) = 0;

    /*--ark web()--*/
    virtual void SetWidth(int32_t width) = 0;

    /*--ark web()--*/
    virtual void SetHeight(int32_t height) = 0;
};

/*--ark web(source=webcore)--*/
class ArkPasteBoardObserverAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void OnPasteBoardChanged() = 0;
};

/*--ark web(source=webview)--*/
class ArkPasteBoardClientAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual bool GetPasteData(ArkPasteRecordVector& data) = 0;

    /*--ark web()--*/
    virtual void SetPasteData(ArkPasteRecordVector& data, int32_t copyOption) = 0;

    /*--ark web()--*/
    virtual bool HasPasteData() = 0;

    /*--ark web()--*/
    virtual void Clear() = 0;

    /*--ark web()--*/
    virtual int32_t OpenRemoteUri(const ArkWebString& path) = 0;

    /*--ark web()--*/
    virtual bool IsLocalPaste() = 0;

    /*--ark web()--*/
    virtual uint32_t GetTokenId() = 0;

    /*--ark web()--*/
    virtual int32_t AddPasteboardChangedObserver(ArkWebRefPtr<ArkPasteBoardObserverAdapter> callback) = 0;

    /*--ark web()--*/
    virtual void RemovePasteboardChangedObserver(int32_t callbackId) = 0;
};

/*--ark web(source=webview)--*/
class ArkPasteDataRecordAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    static ArkWebRefPtr<ArkPasteDataRecordAdapter> NewRecord(const ArkWebString& mimeType);

    /*--ark web()--*/
    static ArkWebRefPtr<ArkPasteDataRecordAdapter> NewRecord(
        const ArkWebString& mimeType, void* htmlText, void* plainText);

    /*--ark web()--*/
    virtual bool SetHtmlText(void* htmlText) = 0;

    /*--ark web()--*/
    virtual bool SetPlainText(void* plainText) = 0;

    /*--ark web()--*/
    virtual bool SetImgData(ArkWebRefPtr<ArkClipBoardImageDataAdapter> imageData) = 0;

    /*--ark web()--*/
    virtual ArkWebString GetMimeType() = 0;

    /*--ark web()--*/
    virtual void GetHtmlText(void* data) = 0;

    /*--ark web()--*/
    virtual void GetPlainText(void* data) = 0;

    /*--ark web()--*/
    virtual bool GetImgData(ArkWebRefPtr<ArkClipBoardImageDataAdapter> imageData) = 0;

    /*--ark web()--*/
    virtual bool SetUri(const ArkWebString& uriString) = 0;

    /*--ark web()--*/
    virtual bool SetCustomData(void* data) = 0;

    /*--ark web()--*/
    virtual void GetUri(void* data) = 0;

    /*--ark web()--*/
    virtual void GetCustomData(void* data) = 0;

    /*--ark web()--*/
    virtual bool SetHtmlTextV2(const ArkWebString& htmlText) { return false; }

    /*--ark web()--*/
    virtual bool SetPlainTextV2(const ArkWebString& plainText) { return false; }

    /*--ark web()--*/
    virtual bool GetHtmlTextV2(ArkWebString& data) { return false; }

    /*--ark web()--*/
    virtual bool GetPlainTextV2(ArkWebString& data) { return false; }

    /*--ark web()--*/
    virtual bool SetCustomDataV2(ArkWebUInt8VectorMap& data) { return false; }

    /*--ark web()--*/
    virtual bool GetUriV2(ArkWebString& data) { return false; }

    /*--ark web()--*/
    virtual bool GetCustomDataV2(ArkWebUInt8VectorMap& data) { return false; }
};

/*--ark web(source=webview)--*/
class ArkPasteDataAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void AddHtmlRecord(const ArkWebString& html) = 0;

    /*--ark web()--*/
    virtual void AddTextRecord(const ArkWebString& text) = 0;

    /*--ark web()--*/
    virtual ArkWebStringVector GetMimeTypes() = 0;

    /*--ark web()--*/
    virtual void GetPrimaryHtml(void* data) = 0;

    /*--ark web()--*/
    virtual void GetPrimaryText(void* data) = 0;

    /*--ark web()--*/
    virtual void GetPrimaryMimeType(void* data) = 0;

    /*--ark web()--*/
    virtual ArkWebRefPtr<ArkPasteDataRecordAdapter> GetRecordAt(size_t index) = 0;

    /*--ark web()--*/
    virtual size_t GetRecordCount() = 0;

    /*--ark web()--*/
    virtual ArkPasteRecordVector AllRecords() = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_PASTEBOARD_CLIENT_ADAPTER_H
