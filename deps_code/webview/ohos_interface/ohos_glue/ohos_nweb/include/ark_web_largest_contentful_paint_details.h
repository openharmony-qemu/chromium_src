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

#ifndef ARK_WEB_LARGEST_CONTENTFUL_PAINT_DETAILS_H
#define ARK_WEB_LARGEST_CONTENTFUL_PAINT_DETAILS_H
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkWebLargestContentfulPaintDetails : public virtual ArkWebBaseRefCounted {
public:
    /**
     * @brief Get start time of navigation.
     *
     * @return Start time of navigation.
     */
    /*--ark web()--*/
    virtual int64_t GetNavigationStartTime() = 0;

    /**
     * @brief Get paint time of largest image.
     *
     * @return Paint time of largest image..
     */
    /*--ark web()--*/
    virtual int64_t GetLargestImagePaintTime() = 0;

    /**
     * @brief Get paint time of largest text.
     *
     * @return Paint time of largest text.
     */
    /*--ark web()--*/
    virtual int64_t GetLargestTextPaintTime() = 0;

    /**
     * @brief Get load start time of largest image.
     *
     * @return Load start time of largest image.
     */
    /*--ark web()--*/
    virtual int64_t GetLargestImageLoadStartTime() = 0;

    /**
     * @brief Get load end time of largest image.
     *
     * @return Load end time of largest image.
     */
    /*--ark web()--*/
    virtual int64_t GetLargestImageLoadEndTime() = 0;

    /**
     * @brief Get bits per pixel of image.
     *
     * @return Bits per pixel of image.
     */
    /*--ark web()--*/
    virtual double_t GetImageBPP() = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_LARGEST_CONTENTFUL_PAINT_DETAILS_H