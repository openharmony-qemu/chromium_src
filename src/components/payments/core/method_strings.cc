// Copyright 2019 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "arkweb/build/features/features.h"
#include "components/payments/core/method_strings.h"

namespace payments {
namespace methods {

// Please keep the list alphabetized.

#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
const char kAndroidPay[] = "https://x.x.x";
const char kBasicCard[] = "basic-card";
const char kGooglePay[] = "https://x.x.x";
const char kGooglePayAuthentication[] = "https://x.x.x";
const char kGooglePlayBilling[] = "https://x.x.x";
const char kSecurePaymentConfirmation[] = "secure-payment-confirmation";
#else
const char kAndroidPay[] = "https://android.com/pay";
const char kGooglePay[] = "https://google.com/pay";
const char kGooglePayAuthentication[] = "https://pay.google.com/authentication";
const char kGooglePlayBilling[] = "https://play.google.com/billing";
const char kSecurePaymentConfirmation[] = "secure-payment-confirmation";
#endif

}  // namespace methods
}  // namespace payments
