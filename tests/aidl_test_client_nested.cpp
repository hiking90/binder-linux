/*
 * Copyright (C) 2021 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "aidl_test_client.h"

#include <android/aidl/tests/nested/INestedService.h>
#include <binder/IServiceManager.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <utils/String16.h>

using android::IBinder;
using android::sp;
using android::String16;
using android::aidl::tests::nested::INestedService;
using android::aidl::tests::nested::ParcelableWithNested;
using android::binder::Status;
using NestedResult = android::aidl::tests::nested::INestedService::Result;
using NestedStatus = android::aidl::tests::nested::ParcelableWithNested::Status;
using std::optional;
using std::pair;
using std::string;
using std::vector;
using testing::Eq;
using testing::Optional;

TEST_F(AidlTest, NestedService) {
  sp<INestedService> nestedService =
      android::waitForService<INestedService>(INestedService::descriptor);
  ASSERT_NE(nullptr, nestedService);

  ParcelableWithNested p;
  p.status = NestedStatus::OK;
  NestedResult r;
  // OK -> NOT_OK
  EXPECT_TRUE(nestedService->flipStatus(p, &r).isOk());
  EXPECT_EQ(r.status, NestedStatus::NOT_OK);

  // NOT_OK -> OK with callback nested interface
  struct Callback : INestedService::BnCallback {
    optional<NestedStatus> result;
    Status done(NestedStatus st) override {
      result = st;
      return Status::ok();
    }
  };
  sp<Callback> cb = new Callback;
  EXPECT_TRUE(nestedService->flipStatusWithCallback(r.status, cb).isOk());
  EXPECT_THAT(cb->result, Optional(NestedStatus::OK));

  // android::enum_ranges<>
  vector<NestedStatus> values{android::enum_range<NestedStatus>().begin(),
                              android::enum_range<NestedStatus>().end()};
  EXPECT_EQ(values, vector<NestedStatus>({NestedStatus::OK, NestedStatus::NOT_OK}));

  // toString()
  EXPECT_EQ(toString(NestedStatus::NOT_OK), "NOT_OK");
}
