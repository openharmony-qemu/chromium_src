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

#include "arkweb/ohos_adapter_ndk/pasteboard_adapter/include/pasteboard_client_adapter_impl.h"

#include <mutex>
#include <set>
#include <database/udmf/udmf_err_code.h>
#include <database/udmf/udmf_meta.h>
#include <database/pasteboard/oh_pasteboard_err_code.h>
#include <multimedia/image_framework/image/pixelmap_native.h>
#include <multimedia/image_framework/image/image_common.h>

#include "AbilityKit/ability_runtime/application_context.h"
#include "arkweb/ohos_nweb/src/nweb_hilog.h"
#include "ohos_adapter_helper.h"
#include "third_party/bounds_checking_function/include/securec.h"

#define FIRST_ELEMENT 0
#define PLAINTEXT "text/plain"

namespace OHOS::NWeb {
PasteDataRecordAdapterImpl::PasteDataRecordAdapterImpl(
    OH_UdmfRecord* record)
    : record_(record) {}

PasteDataRecordAdapterImpl::PasteDataRecordAdapterImpl(
    const std::string& mimeType)
{
    record_ = OH_UdmfRecord_Create();
    mimeType_ = mimeType;
}

PasteDataRecordAdapterImpl::PasteDataRecordAdapterImpl(
    const std::string& mimeType,
    std::shared_ptr<std::string> htmlText,
    std::shared_ptr<std::string> plainText)
{
    record_ = OH_UdmfRecord_Create();
    mimeType_ = mimeType;
}

PasteDataRecordAdapterImpl::~PasteDataRecordAdapterImpl()
{
    OH_UdmfRecord_Destroy(record_);
}

std::shared_ptr<PasteDataRecordAdapter> PasteDataRecordAdapter::NewRecord(
    const std::string& mimeType)
{
    return std::make_shared<PasteDataRecordAdapterImpl>(mimeType);
}

std::shared_ptr<PasteDataRecordAdapter> PasteDataRecordAdapter::NewRecord(
    const std::string& mimeType,
    std::shared_ptr<std::string> htmlText,
    std::shared_ptr<std::string> plainText)
{
    return std::make_shared<PasteDataRecordAdapterImpl>(mimeType,
                                                        htmlText,
                                                        plainText);
}

bool PasteDataRecordAdapterImpl::SetHtmlText(std::shared_ptr<std::string> htmlText)
{
    const char* text = htmlText->c_str();
    OH_UdsHtml* udsHtml = OH_UdsHtml_Create();

    if (udsHtml == nullptr) {
        WVLOG_E("Create UdsHtml failed when set htmltext");
        return false;
    }

    (void) OH_UdmfRecord_GetHtml(record_, udsHtml);
    int setContent_res = OH_UdsHtml_SetContent(udsHtml, text);
    if (setContent_res != UDMF_E_OK) {
        WVLOG_E("AddUdsHtml failed. error code is : %{public}d", setContent_res);
        OH_UdsHtml_Destroy(udsHtml);
        return false;
    }

    int setRecord_res = OH_UdmfRecord_AddHtml(record_, udsHtml);
    if (setRecord_res != UDMF_E_OK) {
        WVLOG_E("AddHtml failed. error code is : %{public}d", setRecord_res);
        OH_UdsHtml_Destroy(udsHtml);
        return false;
    }
    OH_UdsHtml_Destroy(udsHtml);
    return true;
}

bool PasteDataRecordAdapterImpl::SetPlainText(std::shared_ptr<std::string> plainText)
{
    OH_UdsPlainText *udsPlain = OH_UdsPlainText_Create();
    if (udsPlain == nullptr) {
        WVLOG_E("Create Udsplaintext failed when set plaintext");
        return false;
    }

    auto err = OH_UdsPlainText_SetContent(udsPlain, plainText->c_str());
    if (err != UDMF_E_OK) {
        WVLOG_E("set plaintext failed, err_code=%{public}d", err);
        OH_UdsPlainText_Destroy(udsPlain);
        return false;
    }
    err = OH_UdmfRecord_AddPlainText(record_, udsPlain);
    if (err != UDMF_E_OK) {
        WVLOG_E("add plaintext to recorf failed, err_code=%{public}d", err);
        OH_UdsPlainText_Destroy(udsPlain);
        return false;
    }
    OH_UdsPlainText_Destroy(udsPlain);
    return true;
}

bool PasteDataRecordAdapterImpl::SetUri(const std::string& uriString)
{
    if (uriString.empty()) {
        WVLOG_E("uriString is empty");
        return false;
    }

    OH_UdsFileUri* udsFileUri = OH_UdsFileUri_Create();
    const char* uri = uriString.c_str();
    int setFileUri_res = OH_UdsFileUri_SetFileUri(udsFileUri, uri);
    if (setFileUri_res != UDMF_E_OK) {
        WVLOG_E("AddUdsFileUri failed. error code is : %{public}d", setFileUri_res);
        OH_UdsFileUri_Destroy(udsFileUri);
        return false;
    }

    int addFileUri_res = OH_UdmfRecord_AddFileUri(record_, udsFileUri);
    if (addFileUri_res != UDMF_E_OK) {
        WVLOG_E("AddFileUri failed. error code is : %{public}d", addFileUri_res);
        OH_UdsFileUri_Destroy(udsFileUri);
        return false;
    }
    OH_UdsFileUri_Destroy(udsFileUri);
    return true;
}

bool PasteDataRecordAdapterImpl::SetCustomData(PasteCustomData& data)
{
    if (data.empty()) {
        WVLOG_E("custom data is empty");
        return false;
    }

    for (auto& pData : data) {
        const char* typeId = pData.first.c_str();
        std::vector<uint8_t> value = pData.second;
        unsigned int count = value.size();
        unsigned char* entry = reinterpret_cast<unsigned char*>(value.data());
        int addGeneralEntry_res = OH_UdmfRecord_AddGeneralEntry(record_, typeId, entry, count);
        if (addGeneralEntry_res != UDMF_E_OK) {
            WVLOG_E("AddGeneralEntry failed. error code is : %{public}d", addGeneralEntry_res);
            return false;
        }
    }
    return true;
}

void PasteDataRecordAdapterImpl::DestoryOptions(int createResult, OH_Pixelmap_InitializationOptions *options, std::string type)
{
    WVLOG_E("Pixelmap %{public}s. error code is : %{public}d", type.c_str(), createResult);
    int optionsDestroy_res = OH_PixelmapInitializationOptions_Release(options);
    if (optionsDestroy_res != IMAGE_SUCCESS) {
        WVLOG_E("Options Destroy failed. error code is : %{public}d", optionsDestroy_res);
    }
}

void PasteDataRecordAdapterImpl::DestoryPixelmapNative(OH_PixelmapNative *pixelmapNative)
{
    int pixelmapNativeDestroy_res = OH_PixelmapNative_Release(pixelmapNative);
    if (pixelmapNativeDestroy_res != IMAGE_SUCCESS) {
        WVLOG_E("PixelmapNative Destroy failed. error code is : %{public}d", pixelmapNativeDestroy_res);
    }
}

PIXEL_FORMAT PasteDataRecordAdapterImpl::ClipBoardToPixelColorType(ClipBoardImageColorType colorType)
{
    switch (colorType) {
        case ClipBoardImageColorType::COLOR_TYPE_RGBA_8888 :
            return PIXEL_FORMAT::PIXEL_FORMAT_RGBA_8888;
        case ClipBoardImageColorType::COLOR_TYPE_BGRA_8888 :
            return PIXEL_FORMAT::PIXEL_FORMAT_BGRA_8888;
        default :
            return PIXEL_FORMAT::PIXEL_FORMAT_UNKNOWN;
    }
}

ClipBoardImageColorType PasteDataRecordAdapterImpl::PixelToClipBoardColorType(PIXEL_FORMAT colorType)
{
    switch (colorType) {
        case PIXEL_FORMAT::PIXEL_FORMAT_RGBA_8888 :
            return ClipBoardImageColorType::COLOR_TYPE_RGBA_8888;
        case PIXEL_FORMAT::PIXEL_FORMAT_BGRA_8888 :
            return ClipBoardImageColorType::COLOR_TYPE_BGRA_8888;
        default :
            return ClipBoardImageColorType::COLOR_TYPE_UNKNOWN;
    }
}

PIXELMAP_ALPHA_TYPE PasteDataRecordAdapterImpl::ClipboardToPixelAlphaType(ClipBoardImageAlphaType alphaType)
{
    switch (alphaType) {
        case ClipBoardImageAlphaType::ALPHA_TYPE_UNKNOWN :
            return PIXELMAP_ALPHA_TYPE::PIXELMAP_ALPHA_TYPE_UNKNOWN;
        case ClipBoardImageAlphaType::ALPHA_TYPE_OPAQUE :
            return PIXELMAP_ALPHA_TYPE::PIXELMAP_ALPHA_TYPE_OPAQUE;
        case ClipBoardImageAlphaType::ALPHA_TYPE_PREMULTIPLIED :
            return PIXELMAP_ALPHA_TYPE::PIXELMAP_ALPHA_TYPE_PREMULTIPLIED;
        case ClipBoardImageAlphaType::ALPHA_TYPE_POSTMULTIPLIED :
            return PIXELMAP_ALPHA_TYPE::PIXELMAP_ALPHA_TYPE_UNPREMULTIPLIED;
        default :
            return PIXELMAP_ALPHA_TYPE::PIXELMAP_ALPHA_TYPE_UNKNOWN;
    }
}

ClipBoardImageAlphaType PasteDataRecordAdapterImpl::PixelToClipboardAlphaType(PIXELMAP_ALPHA_TYPE alphaType)
{
    switch (alphaType) {
        case PIXELMAP_ALPHA_TYPE::PIXELMAP_ALPHA_TYPE_UNKNOWN :
            return ClipBoardImageAlphaType::ALPHA_TYPE_UNKNOWN;
        case PIXELMAP_ALPHA_TYPE::PIXELMAP_ALPHA_TYPE_OPAQUE :
            return ClipBoardImageAlphaType::ALPHA_TYPE_OPAQUE;
        case PIXELMAP_ALPHA_TYPE::PIXELMAP_ALPHA_TYPE_PREMULTIPLIED :
            return ClipBoardImageAlphaType::ALPHA_TYPE_PREMULTIPLIED;
        case PIXELMAP_ALPHA_TYPE::PIXELMAP_ALPHA_TYPE_UNPREMULTIPLIED :
            return ClipBoardImageAlphaType::ALPHA_TYPE_POSTMULTIPLIED;
        default :
            return ClipBoardImageAlphaType::ALPHA_TYPE_UNKNOWN;
    }
}

bool PasteDataRecordAdapterImpl::SetImgData(std::shared_ptr<ClipBoardImageDataAdapter> imageData)
{
    if (imageData == nullptr) {
        WVLOG_E("imageData is null");
        return false;
    }

    OH_Pixelmap_InitializationOptions *options = nullptr;
    int createOptions_res = OH_PixelmapInitializationOptions_Create(&options);
    if (createOptions_res != IMAGE_SUCCESS) {
        WVLOG_E("create OH_Pixelmap_InitializationOptions failed. error code is : %{public}d", createOptions_res);
        return false;
    }

    uint32_t width = static_cast<uint32_t>(imageData->GetWidth());
    int setWidth_res = OH_PixelmapInitializationOptions_SetWidth(options, width);
    if (setWidth_res != IMAGE_SUCCESS) {
        DestoryOptions(setWidth_res, options, "SetWidth failed");
        return false;
    }
    
    uint32_t height = static_cast<uint32_t>(imageData->GetHeight());
    int setHeight_res = OH_PixelmapInitializationOptions_SetHeight(options, height);
    if (setHeight_res != IMAGE_SUCCESS) {
        DestoryOptions(setHeight_res, options, "SetHeight failed");
        return false;
    }
    
    int32_t srcpixelFormat = ClipBoardToPixelColorType(imageData->GetColorType());
    int setSrcPixelFormat_res = OH_PixelmapInitializationOptions_SetSrcPixelFormat(options, srcpixelFormat);
    if (setSrcPixelFormat_res != IMAGE_SUCCESS) {
        DestoryOptions(setSrcPixelFormat_res, options, "SetSrcPixelFormat failed");
        return false;
    }
    
    int32_t alphaType = ClipboardToPixelAlphaType(imageData->GetAlphaType());
    int setAlphaType_res = OH_PixelmapInitializationOptions_SetAlphaType(options, alphaType);
    if (setAlphaType_res != IMAGE_SUCCESS) {
        DestoryOptions(setAlphaType_res, options, "SetAlphaType failed");
        return false;
    }
    
    OH_PixelmapNative *pixelmapNative = nullptr;
    uint8_t* data = reinterpret_cast<uint8_t*>(imageData->GetData());
    uint64_t stride = static_cast<uint64_t>(imageData->GetWidth()) << 2;
    size_t bufferSize = static_cast<size_t>(stride * static_cast<uint64_t>(imageData->GetHeight()));
    int createPixelmap_res = OH_PixelmapNative_CreatePixelmap(data, bufferSize, options, &pixelmapNative);
    if (createPixelmap_res != IMAGE_SUCCESS) {
        DestoryOptions(createPixelmap_res, options, "CreatePixelmap failed");
        return false;
    }

    OH_UdsPixelMap* udsPixelMap = OH_UdsPixelMap_Create();
    if (udsPixelMap == nullptr) {
        DestoryOptions(0, options, "Create UdsPixelMap failed");
        DestoryPixelmapNative(pixelmapNative);
        return false;
    }

    int setPixelMap_res = OH_UdsPixelMap_SetPixelMap(udsPixelMap, pixelmapNative);
    if (setPixelMap_res != IMAGE_SUCCESS) {
        DestoryOptions(setPixelMap_res, options, "SetPixelMapNative failed");
        DestoryPixelmapNative(pixelmapNative);
        OH_UdsPixelMap_Destroy(udsPixelMap);
        return false;
    }

    int addPixelMap_res = OH_UdmfRecord_AddPixelMap(record_, udsPixelMap);
    if (addPixelMap_res != IMAGE_SUCCESS) {
        DestoryOptions(addPixelMap_res, options, "AddPixelMap failed");
        DestoryPixelmapNative(pixelmapNative);
        OH_UdsPixelMap_Destroy(udsPixelMap);
        return false;
    }

    DestoryOptions(0, options, "SetImgData ended");
    DestoryPixelmapNative(pixelmapNative);
    OH_UdsPixelMap_Destroy(udsPixelMap);
    return true;
}

std::string PasteDataRecordAdapterImpl::GetMimeType()
{
    if (record_ == nullptr) {
        WVLOG_E("record_ is nullptr");
        return "";
    }
    unsigned int count;
    char** types = OH_UdmfRecord_GetTypes(record_, &count);
    if (types == nullptr || count == 0) {
        return "";
    }
    std::string type = types[FIRST_ELEMENT];
    return type;
}

std::vector<std::string> PasteDataRecordAdapterImpl::GetMimeTypes()
{
    if (record_ == nullptr) {
        WVLOG_E("record_ is nullptr");
        return std::vector<std::string>();
    }
    unsigned int count;
    char** types = OH_UdmfRecord_GetTypes(record_, &count);
    std::vector<std::string> stringVector;
    for (unsigned int i = 0; i < count; i++) {
        stringVector.push_back(std::string(types[i]));
    }
    return stringVector;
}

std::shared_ptr<std::string> PasteDataRecordAdapterImpl::GetHtmlText()
{
    if (record_ == nullptr) {
        WVLOG_E("record_ is nullptr");
        return nullptr;
    }

    OH_UdsHtml* udsHtml = OH_UdsHtml_Create();
    int getHtml_res = OH_UdmfRecord_GetHtml(record_, udsHtml);
    if (getHtml_res != UDMF_E_OK) {
        WVLOG_E("GetHtml failed. error code is : %{public}d", getHtml_res);
        OH_UdsHtml_Destroy(udsHtml);
        return nullptr;
    }

    const char* html = OH_UdsHtml_GetContent(udsHtml);
    if (html == nullptr) {
        WVLOG_E("GetContent is nullptr");
        return nullptr;
    }
    std::shared_ptr<std::string> htmlText = std::make_shared<std::string>(html);
    OH_UdsHtml_Destroy(udsHtml);
    return htmlText;
}

std::string PasteDataRecordAdapterImpl::HtmlToPlainText(const std::string& html)
{
    std::string text;
    size_t start = 0;
    size_t end = 0;
    size_t tagStart = 0;
    bool firstBlockProcessed = false;
    std::set<std::string> blockTags = {"p", "div", "h1", "h2", "h3", "h4", "h5", "h6", "li", "ul", "ol"};
    while (start < html.size()) {
        tagStart = html.find('<', end);
        if (tagStart == std::string::npos) {
            break;
        }
        start = html.find('>', tagStart);
        if (start == std::string::npos) {
            break;
        }
        start += 1;
        end = html.find('<', start);
        size_t spacePos = html.find(' ', tagStart + 1);
        size_t tagEnd = (spacePos != std::string::npos && spacePos < start) ? spacePos : start - 1;
        std::string tagName = html.substr(tagStart + 1, tagEnd - tagStart - 1);
        if (blockTags.find(tagName) != blockTags.end()) {
            if (firstBlockProcessed) {
                text += "\r\n";
            } else {
                firstBlockProcessed = true;
            }
        }
        if (end == std::string::npos) {
            text += html.substr(start);
            break;
        }
        if (end > start) {
            text += html.substr(start, end - start);
        }
    }
    return text;
}

std::shared_ptr<std::string> PasteDataRecordAdapterImpl::GetPlainText()
{
    if (record_ == nullptr) {
        WVLOG_E("record_ is nullptr");
        return nullptr;
    }

    std::vector<std::string> types = GetMimeTypes();
    bool hasPlainText = std::find(types.cbegin(), types.cend(), UDMF_META_PLAIN_TEXT) != types.cend();
    WVLOG_D("GetPlainText, hasPlainText = %{public}d", hasPlainText);
    if (hasPlainText) {
        OH_UdsPlainText* udsPlainText = OH_UdsPlainText_Create();
        int getPlainText_res = OH_UdmfRecord_GetPlainText(record_, udsPlainText);
        if (getPlainText_res != UDMF_E_OK) {
            WVLOG_E("GetPlainText failed. error code is : %{public}d", getPlainText_res);
            OH_UdsPlainText_Destroy(udsPlainText);
            return nullptr;
        }
        const char* text = OH_UdsPlainText_GetContent(udsPlainText);
        std::shared_ptr<std::string> plainText = std::make_shared<std::string>(text);
        OH_UdsPlainText_Destroy(udsPlainText);
        return plainText;
    }

    bool hasHtml = std::find(types.cbegin(), types.cend(), UDMF_META_HTML) != types.cend();
    WVLOG_D("GetPlainText, hasPlainText = %{public}d", hasHtml);
    if (hasHtml) {
        std::shared_ptr<std::string> html = GetHtmlText();
        std::shared_ptr<std::string> htmlPlainText = std::make_shared<std::string>(HtmlToPlainText(*html));
        return htmlPlainText;
    }

    return nullptr;
}

bool PasteDataRecordAdapterImpl::GetImgData(std::shared_ptr<ClipBoardImageDataAdapter> imageData)
{
    if (record_ == nullptr) {
        WVLOG_E("record_ is null");
        return false;
    }

    if (imageData == nullptr) {
        WVLOG_E("imageData is null");
        return false;
    }

    OH_UdsPixelMap* udsPixelMap = OH_UdsPixelMap_Create();
    if (udsPixelMap == nullptr) {
        WVLOG_E("Create UdsPixelMap fail.");
    }

    int getPixelMap_res = OH_UdmfRecord_GetPixelMap(record_, udsPixelMap);
    if (getPixelMap_res != UDMF_E_OK) {
        WVLOG_E("GetPixelmap failed. error code is : %{public}d", getPixelMap_res);
        OH_UdsPixelMap_Destroy(udsPixelMap);
        return false;
    }

    OH_Pixelmap_InitializationOptions *options = nullptr;
    int createOptions_res = OH_PixelmapInitializationOptions_Create(&options);
    if (createOptions_res != IMAGE_SUCCESS) {
        WVLOG_E("create OH_Pixelmap_InitializationOptions failed. error code is : %{public}d", createOptions_res);
        return false;
    }

    OH_PixelmapInitializationOptions_SetWidth(options, 1);
    OH_PixelmapInitializationOptions_SetHeight(options, 1);
    OH_PixelmapInitializationOptions_SetPixelFormat(options, PIXEL_FORMAT::PIXEL_FORMAT_BGRA_8888);
    OH_PixelmapInitializationOptions_SetAlphaType(options, PIXELMAP_ALPHA_TYPE::PIXELMAP_ALPHA_TYPE_PREMULTIPLIED);

    OH_PixelmapNative *pixelmapNative = nullptr;
    int createPixelMap_res = OH_PixelmapNative_CreateEmptyPixelmap(options, &pixelmapNative);
    if (createPixelMap_res != IMAGE_SUCCESS) {
        WVLOG_E("create OH_PixelmapNative failed. error code is : %{public}d", createPixelMap_res);
        DestoryOptions(createPixelMap_res, options, "create OH_PixelmapNative failed");
        return false;
    }

    OH_UdsPixelMap_GetPixelMap(udsPixelMap, pixelmapNative);
    if (pixelmapNative == nullptr) {
        WVLOG_E("GetPixelMapNative is null");
        OH_UdsPixelMap_Destroy(udsPixelMap);
        return false;
    }

    OH_Pixelmap_ImageInfo* imageInfo = nullptr;
    int createImageInfo_res = OH_PixelmapImageInfo_Create(&imageInfo);
    if (createImageInfo_res != IMAGE_SUCCESS) {
        WVLOG_E("CreateImageInfo failed. error code is : %{public}d", createImageInfo_res);
        OH_UdsPixelMap_Destroy(udsPixelMap);
        return false;
    }

    int GetImageInfo_res = OH_PixelmapNative_GetImageInfo(pixelmapNative, imageInfo);
    if (GetImageInfo_res != IMAGE_SUCCESS) {
        WVLOG_E("GetImageInfo failed. error code is : %{public}d", createImageInfo_res);
        OH_UdsPixelMap_Destroy(udsPixelMap);
        return false;
    }

    uint32_t width = 0;
    int getWidth_res = OH_PixelmapImageInfo_GetWidth(imageInfo, &width);
    if (getWidth_res != IMAGE_SUCCESS) {
        WVLOG_E("GetWidth failed. error code is : %{public}d", getWidth_res);
    }

    uint32_t height = 0;
    int getHeight_res = OH_PixelmapImageInfo_GetHeight(imageInfo, &height);
    if (getHeight_res != IMAGE_SUCCESS) {
        WVLOG_E("GetHeight failed. error code is : %{public}d", getHeight_res);
    }

    uint32_t rowStride = 0;
    int getRowStride_res = OH_PixelmapImageInfo_GetRowStride(imageInfo, &rowStride);
    if (getRowStride_res != IMAGE_SUCCESS) {
        WVLOG_E("GetRowStride failed. error code is : %{public}d", getRowStride_res);
    }

    int32_t alphaType = 0;
    int getAlphaType_res = OH_PixelmapImageInfo_GetAlphaType(imageInfo, &alphaType);
    if (getAlphaType_res != IMAGE_SUCCESS) {
        WVLOG_E("GetAlphaType failed. error code is : %{public}d", getAlphaType_res);
    }
    if (!alphaType) {
        alphaType = PIXELMAP_ALPHA_TYPE::PIXELMAP_ALPHA_TYPE_PREMULTIPLIED;
    }

    int32_t pixelFormat = 0;
    int getPixelFormat_res = OH_PixelmapImageInfo_GetPixelFormat(imageInfo, &pixelFormat);
    if (getPixelFormat_res != IMAGE_SUCCESS) {
        WVLOG_E("GetPixelFormat failed. error code is : %{public}d", getPixelFormat_res);
    }

    /* ndk16 OH_PixelmapNative_GetByteCount */
    size_t dataSize = static_cast<size_t>(width * height * 4);
    uint8_t* dataBuffer = static_cast<uint8_t *>(calloc(dataSize, sizeof(uint8_t)));
    if (dataBuffer == nullptr) {
        WVLOG_E("calloc dataBuffer failed");
        return false;
    }

    uint32_t* data = static_cast<uint32_t *>(calloc(dataSize, sizeof(uint32_t)));
    int readPixels_res = OH_PixelmapNative_ReadPixels(pixelmapNative, dataBuffer, &dataSize);
    if (readPixels_res != IMAGE_SUCCESS) {
        WVLOG_E("ReadPixels failed. error code is : %{public}d", readPixels_res);
    } else {
        if (memcpy_s(data, dataSize, dataBuffer, dataSize)) {
            WVLOG_E("memcpy_s failed");
            return false;
        }
        free(dataBuffer);
        dataBuffer = nullptr;
    }

    imageData->SetData(data);
    imageData->SetDataSize(dataSize);
    imageData->SetWidth(width);
    imageData->SetHeight(height);
    imageData->SetRowBytes(static_cast<size_t>(rowStride));
    imageData->SetAlphaType(PixelToClipboardAlphaType(static_cast<PIXELMAP_ALPHA_TYPE>(alphaType)));
    imageData->SetColorType(PixelToClipBoardColorType(static_cast<PIXEL_FORMAT>(pixelFormat)));

    DestoryOptions(0, options, "GetImgData ended");
    DestoryPixelmapNative(pixelmapNative);
    OH_UdsPixelMap_Destroy(udsPixelMap);
    int pixelmapImageInfoDestroy_res = OH_PixelmapImageInfo_Release(imageInfo);
    if (pixelmapImageInfoDestroy_res != UDMF_E_OK) {
        WVLOG_E("imageInfo destroy failed. error code is : %{public}d", pixelmapImageInfoDestroy_res);
    }
    return true;
}

std::shared_ptr<std::string> PasteDataRecordAdapterImpl::GetUri()
{
    if (record_ == nullptr) {
        WVLOG_E("record_ is nullptr");
        return nullptr;
    }

    OH_UdsFileUri* udsFileUri = OH_UdsFileUri_Create();
    int getFileUri_res = OH_UdmfRecord_GetFileUri(record_, udsFileUri);
    if (getFileUri_res != UDMF_E_OK) {
        WVLOG_E("GetUdsFileUri failed.  error code is : %{public}d", getFileUri_res);
        OH_UdsFileUri_Destroy(udsFileUri);
        return nullptr;
    }

    const char* fileUri = OH_UdsFileUri_GetFileUri(udsFileUri);
    if (fileUri == nullptr) {
        WVLOG_E("Get FileUri failed.");
        OH_UdsFileUri_Destroy(udsFileUri);
        return nullptr;
    }

    return std::make_shared<std::string>(fileUri);
}

std::shared_ptr<PasteCustomData> PasteDataRecordAdapterImpl::GetCustomData()
{
    if (record_ == nullptr) {
        WVLOG_E("record_ is nullptr");
        return nullptr;
    }

    auto customData = std::make_shared<PasteCustomData>();
    unsigned char* entrys;
    char typeId[] = "ohos/custom-data";
    unsigned int count = 0;
    int getGeneralEntry_res = OH_UdmfRecord_GetGeneralEntry(record_, typeId, &entrys, &count);
    if (getGeneralEntry_res != UDMF_E_OK) {
        WVLOG_E("GetGeneralEntry failed. error code is : %{public}d", getGeneralEntry_res);
        return nullptr;
    }
    std::string key(typeId);
    std::vector<uint8_t> value(entrys, entrys + count);
    customData->emplace(key, value);

    return (customData->empty() ? nullptr : customData);
}

void PasteDataRecordAdapterImpl::ClearImgBuffer()
{
    if (imgBuffer_) {
        free(imgBuffer_);
        imgBuffer_ = nullptr;
        bufferSize_ = 0;
    }
}

void PasteDataRecordAdapterImpl::Clear()
{
    ClearImgBuffer();
}

OH_UdmfRecord* PasteDataRecordAdapterImpl::GetRecord()
{
    return record_;
}

PasteDataAdapterImpl::PasteDataAdapterImpl()
{
    data_ = OH_UdmfData_Create();
}

PasteDataAdapterImpl::PasteDataAdapterImpl(
    OH_UdmfData* data) : data_(data) {}

PasteDataAdapterImpl::~PasteDataAdapterImpl()
{
    OH_UdmfData_Destroy(data_);
}

void PasteDataAdapterImpl::AddHtmlRecord(const std::string& html)
{
    if (data_ == nullptr) {
        return;
    }

    OH_UdsHtml* udsHtml = OH_UdsHtml_Create();
    if (udsHtml == nullptr) {
        WVLOG_E("Create UdsHtml failed.");
        return;
    }

    const char* content = html.c_str();
    int setContent_res = OH_UdsHtml_SetContent(udsHtml, content);
    if (setContent_res != UDMF_E_OK) {
        WVLOG_E("UdsHtml SetContent failed. error code is : %{public}d", setContent_res);
        OH_UdsHtml_Destroy(udsHtml);
        return;
    }

    OH_UdmfRecord* record = OH_UdmfRecord_Create();
    if (record == nullptr) {
        WVLOG_E("Create UdmfRecord failed.");
        OH_UdsHtml_Destroy(udsHtml);
        return;
    }

    int addHtml_res = OH_UdmfRecord_AddHtml(record, udsHtml);
    if (addHtml_res != UDMF_E_OK) {
        WVLOG_E("AddHtml failed. error code is : %{public}d", addHtml_res);
        OH_UdsHtml_Destroy(udsHtml);
        OH_UdmfRecord_Destroy(record);
        return;
    }

    int addRecord_res = OH_UdmfData_AddRecord(data_, record);
    if (addRecord_res != UDMF_E_OK) {
        WVLOG_E("AddRecord failed. error code is : %{public}d", addRecord_res);
    }
    OH_UdsHtml_Destroy(udsHtml);
    OH_UdmfRecord_Destroy(record);
}

void PasteDataAdapterImpl::AddTextRecord(const std::string& text)
{
    if (data_ == nullptr) {
        return;
    }

    OH_UdsPlainText* udsPlainText = OH_UdsPlainText_Create();
    if (udsPlainText == nullptr) {
        WVLOG_E("Create UdsPlainText failed.");
        return;
    }

    const char* content = text.c_str();
    int setContent_res = OH_UdsPlainText_SetContent(udsPlainText, content);
    if (setContent_res != UDMF_E_OK) {
        WVLOG_E("AddUdsPlainText failed. error code is : %{public}d", setContent_res);
        OH_UdsPlainText_Destroy(udsPlainText);
        return;
    }

    OH_UdmfRecord* record = OH_UdmfRecord_Create();
    if (record == nullptr) {
        WVLOG_E("Create UdmfRecord failed.");
        return;
    }

    int addText_res = OH_UdmfRecord_AddPlainText(record, udsPlainText);
    if (addText_res != UDMF_E_OK) {
        WVLOG_E("AddPlainText failed. error code is : %{public}d", addText_res);
        OH_UdsPlainText_Destroy(udsPlainText);
        OH_UdmfRecord_Destroy(record);
        return;
    }

    int addRecord_res = OH_UdmfData_AddRecord(data_, record);
    if (addRecord_res != UDMF_E_OK) {
        WVLOG_E("AddRecord failed. error code is : %{public}d", addRecord_res);
    }
    OH_UdsPlainText_Destroy(udsPlainText);
    OH_UdmfRecord_Destroy(record);
}

std::vector<std::string> PasteDataAdapterImpl::GetMimeTypes()
{
    if (data_ == nullptr) {
        return std::vector<std::string>();
    }

    unsigned int count;
    char** types = OH_UdmfData_GetTypes(data_, &count);
    std::vector<std::string> stringVector;
    for (unsigned int i = 0; i < count; i++) {
        stringVector.push_back(std::string(types[i]));
    }
    return stringVector;
}

std::shared_ptr<std::string> PasteDataAdapterImpl::GetPrimaryHtml()
{
    if (data_ == nullptr) {
        return nullptr;
    }

    OH_UdsHtml* udsHtml = OH_UdsHtml_Create();
    if (udsHtml == nullptr) {
        WVLOG_E("Create UdsHtml failed.");
        return nullptr;
    }

    int getPrimaryHtml_res = OH_UdmfData_GetPrimaryHtml(data_, udsHtml);
    if (getPrimaryHtml_res != UDMF_E_OK) {
        WVLOG_E("GetPrimaryHtml failed. error code is : %{public}d", getPrimaryHtml_res);
        OH_UdsHtml_Destroy(udsHtml);
        return nullptr;
    }

    const char* html = OH_UdsHtml_GetContent(udsHtml);
    std::shared_ptr<std::string> primaryHtml = std::make_shared<std::string>(html);
    OH_UdsHtml_Destroy(udsHtml);
    return primaryHtml;
}

std::shared_ptr<std::string> PasteDataAdapterImpl::GetPrimaryText()
{
    if (data_ == nullptr) {
        return nullptr;
    }

    OH_UdsPlainText* udsPlainText = OH_UdsPlainText_Create();
    if (udsPlainText == nullptr) {
        WVLOG_E("Create UdsPlainText failed.");
        return nullptr;
    }

    int getPrimaryPlainText_res = OH_UdmfData_GetPrimaryPlainText(data_, udsPlainText);
    if (getPrimaryPlainText_res != UDMF_E_OK) {
        WVLOG_E("GetPrimaryPlainText failed. error code is : %{public}d", getPrimaryPlainText_res);
        OH_UdsPlainText_Destroy(udsPlainText);
        return nullptr;
    }

    const char* plainText = OH_UdsPlainText_GetContent(udsPlainText);
    std::shared_ptr<std::string> primaryText = std::make_shared<std::string>(plainText);
    OH_UdsPlainText_Destroy(udsPlainText);
    return primaryText;
}

std::shared_ptr<std::string> PasteDataAdapterImpl::GetPrimaryMimeType()
{
    if (data_ == nullptr) {
        return nullptr;
    }

    std::shared_ptr<PasteDataRecordAdapter> pasteDataRecordAdapter = GetRecordAt(FIRST_ELEMENT);
    if (pasteDataRecordAdapter == nullptr) {
        WVLOG_E("Get Record failed.");
        return nullptr;
    }

    std::string mimeType = pasteDataRecordAdapter->GetMimeType();
    return std::make_shared<std::string>(mimeType);
}

std::shared_ptr<PasteDataRecordAdapter> PasteDataAdapterImpl::GetRecordAt(
    std::size_t index)
{
    if (data_ == nullptr || GetRecordCount() <= index) {
        WVLOG_E("data is nullptr or out of index.");
        return nullptr;
    }

    OH_UdmfRecord* record = OH_UdmfData_GetRecord(data_, index);
    if (record == nullptr) {
        WVLOG_E("GetRecord failed.");
        return nullptr;
    }
    return std::make_shared<PasteDataRecordAdapterImpl>(record);
}

std::size_t PasteDataAdapterImpl::GetRecordCount()
{
    return (data_ != nullptr) ? OH_UdmfData_GetRecordCount(data_) : 0;
}

PasteRecordVector PasteDataAdapterImpl::AllRecords()
{
    if (data_ == nullptr) {
        return PasteRecordVector();
    }

    PasteRecordVector result;
    unsigned int count;
    OH_UdmfRecord** records = OH_UdmfData_GetRecords(data_, &count);
    for (unsigned int i = 0; i < count; i++) {
        result.push_back(std::make_shared<PasteDataRecordAdapterImpl>(records[i]));
    }
    return result;
}

PasteBoardClientAdapterImpl& PasteBoardClientAdapterImpl::GetInstance()
{
    static PasteBoardClientAdapterImpl instance;
    return instance;
}

PasteBoardClientAdapterImpl::PasteBoardClientAdapterImpl()
{
    pasteboard_ = OH_Pasteboard_Create();
}

PasteBoardClientAdapterImpl :: ~PasteBoardClientAdapterImpl()
{
    OH_Pasteboard_Destroy(pasteboard_);
}

Udmf_ShareOption PasteBoardClientAdapterImpl::TransitionCopyOption(CopyOptionMode copyOption)
{
    auto shareOption = Udmf_ShareOption::SHARE_OPTIONS_INVALID;
    switch (copyOption) {
        case CopyOptionMode::IN_APP:
            shareOption = Udmf_ShareOption::SHARE_OPTIONS_IN_APP;
            break;
        case CopyOptionMode::LOCAL_DEVICE:
            shareOption = Udmf_ShareOption::SHARE_OPTIONS_CROSS_APP;
            break;
        default:
            break;
    }
    return shareOption;
}

bool PasteBoardClientAdapterImpl::GetPasteData(PasteRecordVector& data)
{
    if (pasteboard_ == nullptr) {
        WVLOG_E("pasteboard_ is nullptr");
    }
    Pasteboard_GetDataParams *params = OH_Pasteboard_GetDataParams_Create();

    AbilityRuntime_ErrorCode code = ABILITY_RUNTIME_ERROR_CODE_PARAM_INVALID;
    constexpr int32_t NATIVE_BUFFER_SIZE = 1024;
    char cacheDir[NATIVE_BUFFER_SIZE];
    int32_t cacheDirLength = 0;
    code = OH_AbilityRuntime_ApplicationContextGetCacheDir(cacheDir, NATIVE_BUFFER_SIZE, &cacheDirLength);
    if (code != ABILITY_RUNTIME_ERROR_CODE_NO_ERROR) {
        WVLOG_E("GetDistributedFilesDirOfApplicationContext failed:err=%{public}d", code);
        return false;
    }

    OH_Pasteboard_GetDataParams_SetProgressIndicator(params, PASTEBOARD_DEFAULT);
    OH_Pasteboard_GetDataParams_SetDestUri(params, cacheDir, cacheDirLength);
    OH_Pasteboard_GetDataParams_SetFileConflictOptions(params, PASTEBOARD_OVERWRITE);

    int status = -1;
    OH_UdmfData* getData = OH_Pasteboard_GetDataWithProgress(pasteboard_, params, &status);
    if (!OH_Pasteboard_HasData(pasteboard_) || status != ERR_OK) {
        WVLOG_E("get paste data failed. error code is : %{public}d", status);
        isLocalPaste_ = false;
        tokenId_ = 0;
        if (params != nullptr) {
            OH_Pasteboard_GetDataParams_Destroy(params);
        }
        return false;
    }

    unsigned int count;
    OH_UdmfRecord** records = OH_UdmfData_GetRecords(getData, &count);
    if (records == nullptr) {
        WVLOG_E("GetRecord failed.");
        if (params != nullptr) {
            OH_Pasteboard_GetDataParams_Destroy(params);
        }
        return false;
    }
    for (unsigned int i = 0; i < count; i++) {
        data.push_back(std::make_shared<PasteDataRecordAdapterImpl>(records[i]));
    }
    isLocalPaste_ = OH_UdmfData_IsLocal(getData);
    if (params != nullptr) {
        OH_Pasteboard_GetDataParams_Destroy(params);
    }
    return true;
}

void PasteBoardClientAdapterImpl::SetPasteData(const PasteRecordVector& data, CopyOptionMode copyOption)
{
    if (copyOption == CopyOptionMode::NONE) {
        WVLOG_E("SetPasteData failed, copy option mode is 'NONE'");
        return;
    }

    OH_UdmfData* uData = OH_UdmfData_Create();
    for (auto& record: data) {
        PasteDataRecordAdapterImpl* rawRecord =
            reinterpret_cast<PasteDataRecordAdapterImpl*>(record.get());
        if (rawRecord == nullptr) {
            continue;
        }
        int addRecord_res = OH_UdmfData_AddRecord(uData, rawRecord->GetRecord());
        if (addRecord_res != ERR_OK) {
            WVLOG_E("add record failed. error code is : %{public}d", addRecord_res);
            continue;
        }
    }
    
    auto ret = OH_Pasteboard_SetData(pasteboard_, uData);
    if (ret != ERR_OK) {
        WVLOG_E("set paste data failed. error code is : %{public}d", ret);
    }
    OH_UdmfData_Destroy(uData);
}

bool PasteBoardClientAdapterImpl::HasPasteData()
{
    return OH_Pasteboard_HasData(pasteboard_);
}

void PasteBoardClientAdapterImpl::Clear()
{
    PasteRecordVector recordVector;
    if (!GetPasteData(recordVector)) {
        WVLOG_E("get paste data failed while clear");
        int ret = OH_Pasteboard_ClearData(pasteboard_);
        if (ret != ERR_OK) {
            WVLOG_E("not get paste data and paste board clear data failed. error code is : %{public}d", ret);
        }
        return;
    }
    for (auto& record: recordVector) {
        PasteDataRecordAdapterImpl* rawRecord =
            reinterpret_cast<PasteDataRecordAdapterImpl*>(record.get());
        if (rawRecord == nullptr) {
            continue;
        }
        rawRecord->Clear();
    }
    int ret = OH_Pasteboard_ClearData(pasteboard_);
    if (ret != ERR_OK) {
        WVLOG_E("paste board clear data failed. error code is : %{public}d", ret);
    }
}

int32_t PasteBoardClientAdapterImpl::OpenRemoteUri(const std::string& path)
{
    return -1;
}

bool PasteBoardClientAdapterImpl::IsLocalPaste()
{
    return isLocalPaste_;
}

uint32_t PasteBoardClientAdapterImpl::GetTokenId()
{
    return tokenId_;
}

void PasteBoardNotify(void* context, Pasteboard_NotifyType type)
{
    if (context == nullptr) {
        WVLOG_E("PasteBoardNotify failed, context is NULL");
        return;
    }
    std::shared_ptr<PasteBoardCallback> pasteBoardCallback =
                *(static_cast<std::shared_ptr<PasteBoardCallback>*>(context));
    pasteBoardCallback->callback->OnPasteboardChanged();
}

void PasteBoardFinalize(void* context)
{
    WVLOG_I("PasteBoardFinalize start");
}

int32_t PasteBoardClientAdapterImpl::AddPasteboardChangedObserver(
    std::shared_ptr<PasteboardObserverAdapter> callback)
{
    static int32_t count = 0;
    int32_t id = -1;
    if (callback) {
        pasteCallback_ = std::make_shared<PasteBoardCallback>();
        pasteCallback_->callback = callback;
        OH_PasteboardObserver* observer = nullptr;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            observer = OH_PasteboardObserver_Create();
            if (!observer) {
                return -1;
            }

            auto ret = OH_PasteboardObserver_SetData(observer, static_cast<void*>(&pasteCallback_),
                                                     PasteBoardNotify, PasteBoardFinalize);
            if (ret != ERR_OK) {
                WVLOG_E("PasteboardObserver SetData failed. error code is : %{public}d", ret);
                int des_ret = OH_PasteboardObserver_Destroy(observer);
                if (des_ret != ERR_OK) {
                    WVLOG_E("PasteboardObserver destroy failed. error code is : %{public}d", des_ret);
                }
                return -1;
            }

            id = count++;
            if (count < 0) {
                count = 0;
            }
            reg_.emplace(std::make_pair(id, observer));
        }
        auto ret = OH_Pasteboard_Subscribe(pasteboard_, NOTIFY_LOCAL_DATA_CHANGE, observer);
        //if subscribe failed, should remove and destroy observer
        if (ret != ERR_OK) {
            WVLOG_E("Subscribe pasteboard failed. error code is : %{public}d", ret);
            ObserverMap::iterator iter = reg_.find(id);
            if (iter != reg_.end()) {
                (void)OH_PasteboardObserver_Destroy(iter->second);
                reg_.erase(iter);
            }
        }
    }
    return id;
}

void PasteBoardClientAdapterImpl::RemovePasteboardChangedObserver(
    int32_t callbackId)
{
    OH_PasteboardObserver* observer = nullptr;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        ObserverMap::iterator iter = reg_.find(callbackId);
        if (iter == reg_.end()) {
            return;
        }
        observer = iter->second;
        reg_.erase(iter);
    }
    auto ret = OH_Pasteboard_Unsubscribe(pasteboard_, NOTIFY_LOCAL_DATA_CHANGE, observer);
    if (ret != ERR_OK) {
        WVLOG_E("unsubscribe observer failed. error code is : %{public}d", ret);
        return;
    }
    ret = OH_PasteboardObserver_Destroy(observer);
    if (ret != ERR_OK) {
        WVLOG_E("destroy observer failed. error code is : %{public}d", ret);
    }
}
} // namespace OHOS::NWeb
