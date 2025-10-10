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
 
#ifndef OHOS_NWEB_SRC_WEB_EXTENSION_LOAD_URL_PARAMS_H_
#define OHOS_NWEB_SRC_WEB_EXTENSION_LOAD_URL_PARAMS_H_
 
#include <string>
#include <vector>
 
typedef enum {
	LOAD_TYPE_DEFAULT = 0,
	LOAD_TYPE_HTTP_POST = 1,
} LoadUrlType;
 
 
struct LoadUrlParams {
  std::string url;
  LoadUrlType load_type;
  std::string refer;
  std::string headers;
  std::string post_data;
  bool allow_https_upgrade;
  int32_t transition_type = -1;
};
 
 
#endif //OHOS_NWEB_SRC_WEB_EXTENSION_LOAD_URL_PARAMS_H_