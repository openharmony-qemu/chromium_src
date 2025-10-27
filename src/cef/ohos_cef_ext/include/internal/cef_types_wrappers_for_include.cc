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

#if BUILDFLAG(IS_OHOS)
struct CefSelectPopupItemTraits {
  typedef cef_select_popup_item_t struct_type;

  static inline void init(struct_type* s) {}

  static inline void clear(struct_type* s) {}

  static inline void set(const struct_type* src,
                         struct_type* target,
                         bool copy) {
    *target = *src;
  }
};

///
// Class representing select popup item.
///
typedef CefStructBase<CefSelectPopupItemTraits> CefSelectPopupItem;

struct CefEmbedTouchEventTraits {
  typedef cef_embed_touch_event_t struct_type;

  static inline void init(struct_type* s) {}

  static inline void clear(struct_type* s) {}

  static inline void set(const struct_type* src,
                         struct_type* target,
                         bool copy) {
    *target = *src;
  }
};

using CefEmbedTouchEvent = CefStructBase<CefEmbedTouchEventTraits>;

struct CefNativeEmbedDataTraits {
  typedef cef_native_embed_data_t struct_type;

  static inline void init(struct_type* s) {}

  static inline void clear(struct_type* s) {}

  static inline void set(const struct_type* src,
                         struct_type* target,
                         bool copy) {
    *target = *src;
  }
};

using CefNativeEmbedData = CefStructBase<CefNativeEmbedDataTraits>;

struct CefDateTimeChooserTraits {
  typedef cef_date_time_chooser_t struct_type;

  static inline void init(struct_type* s) {}

  static inline void clear(struct_type* s) {}

  static inline void set(const struct_type* src,
                         struct_type* target,
                         bool copy) {
    *target = *src;
  }
};

struct CefAutofillPopupItemTraits {
  typedef cef_autofill_popup_item_t struct_type;

  static inline void init(struct_type* s) {}

  static inline void clear(struct_type* s) {}

  static inline void set(const struct_type* src,
                         struct_type* target,
                         bool copy) {
    *target = *src;
  }
};

///
// Class representing password autofill popup item.
///
typedef CefStructBase<CefAutofillPopupItemTraits> CefAutofillPopupItem;

///
// Class representing date time chooser.
///
class CefDateTimeChooser : public CefStructBase<CefDateTimeChooserTraits> {
 public:
  typedef CefStructBase<CefDateTimeChooserTraits> parent;

  CefDateTimeChooser() {}
  CefDateTimeChooser(const cef_date_time_chooser_t& r) : parent(r) {}
  CefDateTimeChooser(cef_text_input_type_t dialog_type,
                     double dialog_value,
                     double minumum,
                     double maximum,
                     double step) {
    Set(dialog_type, dialog_value, minumum, maximum, step);
  }

  void Set(cef_text_input_type_t dialog_type_value,
           double dialog_value_value,
           double minumum_value,
           double maximum_value,
           double step_value) {
    dialog_type = dialog_type_value;
    dialog_value = dialog_value_value;
    minimum = minumum_value;
    maximum = maximum_value;
    step = step_value;
  }
};

struct CefDateTimeSuggestionTraits {
  typedef cef_date_time_suggestion_t struct_type;

  static inline void init(struct_type* s) {}

  static inline void clear(struct_type* s) {}

  static inline void set(const struct_type* src,
                         struct_type* target,
                         bool copy) {
    *target = *src;
  }
};

///
// Class representing date time chooser.
///
class CefDateTimeSuggestion
    : public CefStructBase<CefDateTimeSuggestionTraits> {
 public:
  typedef CefStructBase<CefDateTimeSuggestionTraits> parent;

  CefDateTimeSuggestion() {}
  CefDateTimeSuggestion(const cef_date_time_suggestion_t& r) : parent(r) {}
  CefDateTimeSuggestion(double value,
                        cef_string_t localized_value,
                        cef_string_t label) {
    Set(value, localized_value, label);
  }

  void Set(double value_value,
           cef_string_t localized_value_value,
           cef_string_t label_value) {
    value = value_value;
    localized_value = localized_value_value;
    label = label_value;
  }
};
#endif  // BUILDFLAG(IS_OHOS)
