// Copyright 2014 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/push_messaging/push_messaging_constants.h"

const char kPushMessagingGcmEndpoint[] =
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
    "https://x.x.x";
#else
    "https://fcm.googleapis.com/fcm/send/";
#endif

const char kPushMessagingStagingGcmEndpoint[] =
    "https://jmt17.google.com/fcm/send/";

const char kPushMessagingWebpushEndpoint[] =
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
    "https://***/";
#else
    "https://fcm.googleapis.com/wp/";
#endif

const char kPushMessagingStagingWebpushEndpoint[] =
#if BUILDFLAG(ARKWEB_PRIVACY_COMPLIANCE)
    "https://***/";
#else
    "https://fcm.googleapis.com/preprod/wp/";
#endif

const char kPushMessagingForcedNotificationTag[] =
    "user_visible_auto_notification";
