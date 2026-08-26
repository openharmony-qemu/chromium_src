// Copyright 2013 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/autofill/core/browser/single_field_fillers/autocomplete/autocomplete_history_manager.h"

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "base/check_deref.h"
#include "base/containers/to_vector.h"
#include "base/functional/bind.h"
#include "base/memory/weak_ptr.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "base/version_info/version_info.h"
#include "components/autofill/core/browser/data_quality/validation.h"
#include "components/autofill/core/browser/field_types.h"
#include "components/autofill/core/browser/form_structure.h"
#include "components/autofill/core/browser/metrics/autofill_metrics.h"
#include "components/autofill/core/browser/studies/autofill_experiments.h"
#include "components/autofill/core/browser/suggestions/suggestion.h"
#include "components/autofill/core/browser/suggestions/suggestion_type.h"
#include "components/autofill/core/browser/webdata/autocomplete/autocomplete_entry.h"
#include "components/autofill/core/common/autofill_clock.h"
#include "components/autofill/core/common/autofill_features.h"
#include "components/autofill/core/common/autofill_prefs.h"
#include "components/autofill/core/common/autofill_regexes.h"
#include "components/autofill/core/common/credit_card_number_validation.h"
#include "components/autofill/core/common/form_data.h"
#include "components/prefs/pref_service.h"
#include "components/version_info/version_info.h"

namespace autofill {

namespace {
// Returns true if the field type is eligible to be saved in the autocomplete
// history. Some types (promo codes, IBANs, CCs, CVCs) are excluded. Loyalty
// card IDs are also excluded if they were autofilled.
bool IsFieldTypeSaveable(const FormStructure* form, FieldGlobalId field_id) {
  const AutofillField* field = form ? form->GetFieldById(field_id) : nullptr;
  if (!field) {
    return true;
  }
  for (const FieldType field_type : field->Type().GetTypes()) {
    if (field_type == MERCHANT_PROMO_CODE || field_type == IBAN_VALUE ||
        field_type == CREDIT_CARD_VERIFICATION_CODE ||
        field_type == CREDIT_CARD_STANDALONE_VERIFICATION_CODE ||
        field_type == CREDIT_CARD_NUMBER) {
      return false;
    }
    if (field_type == LOYALTY_MEMBERSHIP_ID && field->is_autofilled()) {
      return false;
    }
  }
  return true;
}
}  // namespace

AutocompleteHistoryManager::AutocompleteHistoryManager() = default;

AutocompleteHistoryManager::~AutocompleteHistoryManager() = default;

void AutocompleteHistoryManager::OnGetSingleFieldSuggestions(
    const FormData& form,
    const FormStructure* form_structure,
    const FormFieldData& trigger_field,
    const AutofillField* trigger_autofill_field,
    const AutofillClient& client,
    SingleFieldFillRouter::OnSuggestionsReturnedCallback
        on_suggestions_returned) {
  // Cancel the pending query if there is one.
  suggestion_generator_ = nullptr;
  if (!profile_database_) {
    std::move(on_suggestions_returned).Run(trigger_field.global_id(), {});
    return;
  }
  suggestion_generator_ =
      std::make_unique<AutocompleteSuggestionGenerator>(profile_database_);

  auto on_suggestions_generated = base::BindOnce(
      [](SingleFieldFillRouter::OnSuggestionsReturnedCallback callback,
         FieldGlobalId field_id,
         SuggestionGenerator::ReturnedSuggestions returned_suggestions) {
        std::move(callback).Run(field_id,
                                std::move(returned_suggestions.second));
      },
      std::move(on_suggestions_returned), trigger_field.global_id());

  auto on_suggestion_data_returned = base::BindOnce(
      [](base::OnceCallback<void(SuggestionGenerator::ReturnedSuggestions)>
             callback,
         FormData form, FormFieldData field, const AutofillClient& client,
         base::WeakPtr<AutocompleteSuggestionGenerator>
             autocomplete_suggestion_generator,
         std::pair<SuggestionGenerator::SuggestionDataSource,
                   std::vector<SuggestionGenerator::SuggestionData>>
             suggestion_data) {
        if (autocomplete_suggestion_generator) {
          autocomplete_suggestion_generator->GenerateSuggestions(
              std::move(form), std::move(field), /*form_structure=*/nullptr,
              /*trigger_autofill_field=*/nullptr, client,
              {std::move(suggestion_data)}, std::move(callback));
        }
      },
      std::move(on_suggestions_generated), form, trigger_field,
      std::cref(client), suggestion_generator_->GetWeakPtr());

  suggestion_generator_->FetchSuggestionData(
      form, trigger_field, form_structure, trigger_autofill_field, client,
      std::move(on_suggestion_data_returned));
}

void AutocompleteHistoryManager::OnWillSubmitFormWithFields(
    const std::vector<FormFieldData>& fields,
    const FormStructure* form,
    bool is_autocomplete_enabled) {
  if (!is_autocomplete_enabled || is_off_the_record_) {
    return;
  }
  std::vector<FormFieldData> autocomplete_saveable_fields;
  autocomplete_saveable_fields.reserve(fields.size());
  for (const FormFieldData& field : fields) {
    if (IsFieldValueSaveable(field, form)) {
      autocomplete_saveable_fields.push_back(field);
    }
  }
  if (!autocomplete_saveable_fields.empty() && profile_database_.get()) {
    profile_database_->AddFormFields(autocomplete_saveable_fields);
  }
}

void AutocompleteHistoryManager::CancelPendingQuery() {
  if (suggestion_generator_) {
    suggestion_generator_->CancelPendingQuery();
  }
}

void AutocompleteHistoryManager::OnRemoveCurrentSingleFieldSuggestion(
    const std::u16string& field_name,
    const std::u16string& value,
    SuggestionType type) {
  if (profile_database_) {
    profile_database_->RemoveFormValueForElementName(field_name, value);
  }
}

void AutocompleteHistoryManager::OnSingleFieldSuggestionSelected(
    const Suggestion& suggestion) {
  CHECK_EQ(suggestion.type, SuggestionType::kAutocompleteEntry);
  const AutocompleteEntry& entry =
      CHECK_DEREF(std::get_if<AutocompleteEntry>(&suggestion.payload));
  // The AutocompleteEntry was found, use it to log the DaysSinceLastUsed.
  base::TimeDelta time_delta = base::Time::Now() - entry.date_last_used();
  AutofillMetrics::LogAutocompleteDaysSinceLastUse(time_delta.InDays());
}

void AutocompleteHistoryManager::Init(
    scoped_refptr<AutofillWebDataService> profile_database,
    PrefService* pref_service,
    bool is_off_the_record) {
  profile_database_ = profile_database;
  pref_service_ = pref_service;
  is_off_the_record_ = is_off_the_record;

  if (!profile_database_) {
    // In some tests, there are no dbs.
    return;
  }

  // No need to run the retention policy in OTR.
  if (!is_off_the_record_) {
    // Upon successful cleanup, the last cleaned-up major version is being
    // stored in this pref.
    int last_cleaned_version = pref_service_->GetInteger(
        prefs::kAutocompleteLastVersionRetentionPolicy);
    if (version_info::GetMajorVersionNumberAsInt() > last_cleaned_version) {
      // Trigger the cleanup.
      profile_database_->RemoveExpiredAutocompleteEntries(
          base::BindOnce(&AutocompleteHistoryManager::OnAutofillCleanupReturned,
                         weak_ptr_factory_.GetWeakPtr()));
    }
  }
}

bool AutocompleteHistoryManager::IsFieldNameMeaningfulForAutocomplete(
    const std::u16string& name) {
  static constexpr char16_t kRegex[] =
      u"^(((field|input|mat-input)(_|-)?\\d+)|title|otp|tan)$|"
      // Suppress flight verification fields and other CVC fields.
      u"(csc|cvd|ccv|cvc|cvn|cvv|captcha|flight[-_]?verification|"
      u"card[-_]?verification|verification[-_]?code|"
      u"verify[-_]?(?:card|code)|security[-_]?(?:code|value|number))";
  return !MatchesRegex<kRegex>(name);
}

void AutocompleteHistoryManager::OnAutofillCleanupReturned(
    WebDataServiceBase::Handle current_handle,
    std::unique_ptr<WDTypedResult> result) {
  DCHECK(result);
  DCHECK_EQ(AUTOFILL_CLEANUP_RESULT, result->GetType());

  // Cleanup was successful, update the latest run milestone.
  pref_service_->SetInteger(prefs::kAutocompleteLastVersionRetentionPolicy,
                            version_info::GetMajorVersionNumberAsInt());
}

// We put the following restriction on stored FormFields:
//  - non-empty name
//  - neither empty nor whitespace-only value
//  - text field
//  - autocomplete is not disabled
//  - value is not a credit card number
//  - field is not a credit card verification code (CVC)
//  - field is not an autofilled loyalty card ID
//  - field has user typed input or is focusable (this is a mild criteria but
//    this way it is consistent for all platforms)
//  - not a presentation field
bool AutocompleteHistoryManager::IsFieldValueSaveable(
    const FormFieldData& field,
    const FormStructure* form) {
  // Only save values from text-like input elements that are not password
  // or number inputs.
  if (!field.IsTextInputElement() || field.IsPasswordInputElement() ||
      field.form_control_type() == FormControlType::kInputNumber) {
    return false;
  }

  // Only save values if the page allows autocomplete for the field.
  if (!field.should_autocomplete()) {
    return false;
  }

  // Reject fields with empty names or names that are not meaningful for
  // autocomplete (e.g., placeholder names generated by frameworks).
  if (!IsFieldNameMeaningfulForAutocomplete(field.name()) ||
      field.name().empty()) {
    return false;
  }

  // We don't want to save a trimmed string, but we want to make sure that the
  // value is neither empty nor only whitespaces.
  if (std::ranges::none_of(field.value(),
                           std::not_fn(base::IsUnicodeWhitespace<char16_t>))) {
    return false;
  }

  // Reject fields with types that are ineligible for autocomplete such as
  // credit card numbers, CVCs, IBANs, promo codes, or autofilled loyalty cards.
  if (!IsFieldTypeSaveable(form, field.global_id())) {
    return false;
  }

  // Do not save sensitive values like credit card numbers or Social Security
  // Numbers.
  if (IsValidCreditCardNumber(field.value()) || IsSSN(field.value())) {
    return false;
  }

  // Reject fields that the user did not type into and are not currently
  // focusable, or fields that have a presentation role (ARIA
  // role="presentation").
  if ((!(field.properties_mask() & kUserTyped) && !field.is_focusable()) ||
      field.role() == FormFieldData::RoleAttribute::kPresentation) {
    return false;
  }

  return true;
}

}  // namespace autofill
