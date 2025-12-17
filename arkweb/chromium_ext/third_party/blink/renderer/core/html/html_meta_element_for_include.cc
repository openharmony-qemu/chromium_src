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

#if BUILDFLAG(ARKWEB_FLING)
void HTMLMetaElement::ParseFlingData(Document* document,
                                     bool report_warnings,
                                     const String& key_string,
                                     const String& value_string,
                                     ViewportDescription& description) {
  if (key_string == "max-fling-speed-x") {
    bool is_transforms_x = false;
    float reference_num_x = std::numeric_limits<float>::max();

    reference_num_x = ParsePositiveNumber(document, report_warnings, key_string, value_string,
                                        &is_transforms_x);
    if (!is_transforms_x) {
      reference_num_x = std::numeric_limits<float>::max();
      return;
    }

    if (description.max_fling_velocity_x != reference_num_x) {
      LOG(INFO) << "ParseFlingVelocity key= " << key_string << ", value "
                << value_string << ", reference_num_x= " << reference_num_x;
      description.max_fling_velocity_x = reference_num_x;
    }
    
    if (report_warnings) {
      ReportViewportWarning(document, kViewportLimitSpeedUnsupported, key_string,
                            value_string);
    }
  } else if (key_string == "max-fling-speed-y") {
    bool is_transforms_y = false;
    float reference_num_y = std::numeric_limits<float>::max();

    reference_num_y = ParsePositiveNumber(document, report_warnings, key_string, value_string,
                                         &is_transforms_y);
    if (!is_transforms_y) {
      reference_num_y = std::numeric_limits<float>::max();
      return;
    }

    if (description.max_fling_velocity_y != reference_num_y) {
      LOG(INFO) << "ParseFlingVelocity key= " << key_string << ", value "
                << value_string << ", reference_num_y= " << reference_num_y;
      description.max_fling_velocity_y = reference_num_y;
    }
    
    if (report_warnings) {
      ReportViewportWarning(document, kViewportLimitSpeedUnsupported, key_string,
                            value_string);
    }
  }
}
#endif