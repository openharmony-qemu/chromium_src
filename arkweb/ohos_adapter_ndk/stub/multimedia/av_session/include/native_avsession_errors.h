/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

/**
 * @addtogroup OHAVSession
 * @{
 *
 * @brief Provide the definition of the C interface for the avsession module.
 * @since 13
 */

/**
 * @file native_avsession_errors.h
 *
 * @brief Declare avsession related error.
 *
 * @library libohavsession.so
 * @syscap SystemCapability.Multimedia.AVSession.Core
 * @kit AVSessionKit
 * @since 13
 */

#ifndef NATIVE_AVSESSION_ERRORS_H
#define NATIVE_AVSESSION_ERRORS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief AVSession error code
 *
 * @since 13
 * @version 1.0
 */
typedef enum {
    /**
     * @error The operation completed successfully.
     */
    AV_SESSION_ERR_SUCCESS = 0,

    /**
     * @error Invalid parameter。
     */
    AV_SESSION_ERR_INVALID_PARAMETER = 401,

    /**
     * @error Service exception.
     */
    AV_SESSION_ERR_SERVICE_EXCEPTION = 6600101,

    /**
     * @error The session does not exist.
     */
    AV_SESSION_ERR_CODE_SESSION_NOT_EXIST = 6600102,

    /**
     * @error Invalid session command.
     */
    AV_SESSION_ERR_CODE_COMMAND_INVALID = 6600105,

    /**
     * @error The session is not activated.
     */
    AV_SESSION_ERR_CODE_SESSION_INACTIVE = 6600106,

    /**
     * @error Too many commands or events.
     */
    AV_SESSION_ERR_CODE_MESSAGE_OVERLOAD = 6600107,

    /**
     * The remote connection is not established.
    */
    AV_SESSION_ERR_CODE_REMOTE_CONNECTION_NOT_EXIST = 6600109,
} AVSession_ErrCode;

/**
 * @brief Defines enumeration of avsession callback result.
 *
 * @since 13
 */
typedef enum {
    /**
     * @brief Result of avsession callabck is success.
     */
    AVSESSION_CALLBACK_RESULT_SUCCESS = 0,
 
    /**
     * @brief Result of avsession callabck failed.
     */
    AVSESSION_CALLBACK_RESULT_FAILURE = -1,
} AVSessionCallback_Result;
 
/**
 * @brief AVMetadata error code
 *
 * @since 13
 * @version 1.0
 */
typedef enum {
    /**
     * @error The call was successful.
     */
    AVMETADATA_SUCCESS = 0,
 
    /**
     * @error This means that the function was executed with an invalid input parameter.
     */
    AVMETADATA_ERROR_INVALID_PARAM = 1,
 
    /**
     * @error This means there is no memory left.
     */
    AVMETADATA_ERROR_NO_MEMORY = 2,
} AVMetadata_Result;
 
/**
 * @brief AVQUEUEITEM error code
 *
 * @since 23
 * @version 1.0
 */
typedef enum {
    /**
     * @error The call was successful.
     */
    AVQUEUEITEM_SUCCESS = 0,
 
    /**
     * @error This means that the function was executed with an invalid input parameter.
     */
    AVQUEUEITEM_ERROR_INVALID_PARAM = 1,
 
    /**
     * @error This means there is no memory left.
     */
    AVQUEUEITEM_ERROR_NO_MEMORY = 2,
} AVQueueItem_Result;
 
/**
 * @brief AVQUEUEITEM error code
 *
 * @since 23
 * @version 1.0
 */
typedef enum {
    /**
     * @error The call was successful.
     */
    AVCASTPICKEROPTION_SUCCESS = 0,
 
    /**
     * @error This means that the function was executed with an invalid input parameter.
     */
    AVCASTPICKEROPTION_ERROR_INVALID_PARAM = 1,
 
    /**
     * @error This means there is no memory left.
     */
    AVCASTPICKEROPTION_ERROR_NO_MEMORY = 2,
} AVCastPickerOptions_Result;

#ifdef __cplusplus
}
#endif

#endif // NATIVE_AVSESSION_ERRORS_H
/** @} */