/*
 * Copyright (C) 2018 The Android Open Source Project
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

using android::binder::Status;

using android::aidl::tests::ITestService;
using android::aidl::tests::ITestServiceDefault;

using testing::Eq;

static constexpr int32_t kExpectedArgValue = 100;
static constexpr int32_t kExpectedReturnValue = 200;

static int numCalled = 0;
static int32_t gotArgument = 0;

struct Def : public ITestServiceDefault {
  Status UnimplementedMethod(int32_t arg, int32_t* _aidl_return) override {
    numCalled++;
    gotArgument = arg;
    *_aidl_return = kExpectedReturnValue;
    return android::binder::Status::ok();
  }
};

/* Make sure the unimplementedMethod behaves as expected before testing the
 * default impl
 */
TEST_F(AidlTest, unimplementedMethodImpl) {
  int32_t returned_value;
  auto status = service->UnimplementedMethod(kExpectedArgValue, &returned_value);
  ASSERT_FALSE(status.isOk()) << status;
  ASSERT_EQ(status.exceptionCode(), android::binder::Status::EX_TRANSACTION_FAILED);
  EXPECT_EQ(status.transactionError(), android::UNKNOWN_TRANSACTION);
}

TEST_F(AidlTest, defaultImpl) {
  android::sp<ITestService> defImpl = android::sp<Def>::make();
  auto ret = ITestService::setDefaultImpl(std::move(defImpl));
  ASSERT_TRUE(ret);

  int32_t returned_value;
  auto status = service->UnimplementedMethod(kExpectedArgValue, &returned_value);
  ASSERT_TRUE(status.isOk()) << status;
  ASSERT_THAT(numCalled, Eq(1));
  ASSERT_THAT(gotArgument, Eq(kExpectedArgValue));
  ASSERT_THAT(returned_value, Eq(kExpectedReturnValue));
}
