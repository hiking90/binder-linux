/*
 * Copyright (C) 2015 The Android Open Source Project
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
#include "gmock/gmock.h"

TEST_F(AidlTest, onewayNoError) {
  // oneway servers try to return an error
  auto status = service->TestOneway();
  EXPECT_TRUE(status.isOk()) << status;
}

TEST_F(AidlTest, serviceSpecificException) {
  using testing::Eq;

  for (int32_t i = -1; i < 2; ++i) {
    auto status = service->ThrowServiceException(i);
    EXPECT_THAT(status.exceptionCode(), Eq(android::binder::Status::EX_SERVICE_SPECIFIC))
        << status << " for " << i;
    EXPECT_THAT(status.serviceSpecificErrorCode(), Eq(i)) << status << " for " << i;
  }
}
