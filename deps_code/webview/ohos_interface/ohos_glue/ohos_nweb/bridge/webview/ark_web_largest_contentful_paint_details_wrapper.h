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

#ifndef ARK_WEB_FIRST_LARGEST_CONTENTFUL_PAINT_DETAILS_WRAPPER_H_
#define ARK_WEB_FIRST_LARGEST_CONTENTFUL_PAINT_DETAILS_WRAPPER_H_
#pragma once

#include "include/nweb_largest_contentful_paint_details.h"
#include "ohos_nweb/include/ark_web_largest_contentful_paint_details.h"

namespace OHOS::ArkWeb {

class ArkWebLargestContentfulPaintDetailsWrapper : public OHOS::NWeb::NWebLargestContentfulPaintDetails {
public:
    explicit ArkWebLargestContentfulPaintDetailsWrapper(
        ArkWebRefPtr<ArkWebLargestContentfulPaintDetails> nweb_largest_contentful_paint_details);
    ~ArkWebLargestContentfulPaintDetailsWrapper() = default;

    /**
     * @brief Get start time of navigation.
     *
     * @return Start time of navigation.
     */
    int64_t GetNavigationStartTime() override;

    /**
     * @brief Get paint time of largest image.
     *
     * @return Paint time of largest image..
     */
    int64_t GetLargestImagePaintTime() override;

    /**
     * @brief Get paint time of largest text.
     *
     * @return Paint time of largest text.
     */
    int64_t GetLargestTextPaintTime() override;

    /**
     * @brief Get load start time of largest image.
     *
     * @return Load start time of largest image.
     */
    int64_t GetLargestImageLoadStartTime() override;

    /**
     * @brief Get load end time of largest image.
     *
     * @return Load end time of largest image.
     */
    int64_t GetLargestImageLoadEndTime() override;

    /**
     * @brief Get bits per pixel of image.
     *
     * @return Bits per pixel of image.
     */
    double_t GetImageBPP() override;

private:
    ArkWebRefPtr<ArkWebLargestContentfulPaintDetails> ark_web_largest_contentful_paint_details_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_FIRST_LARGEST_CONTENTFUL_PAINT_DETAILS_IMPL_H_
