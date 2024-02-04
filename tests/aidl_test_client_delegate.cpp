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
#include "gmock/gmock.h"

#include "android/aidl/tests/BnNamedCallback.h"
#include "android/aidl/tests/BnTestService.h"

using android::binder::Status;

using android::String16;
using android::aidl::tests::INamedCallback;
using android::aidl::tests::ITestService;
using android::aidl::tests::ITestServiceDelegator;

static constexpr int8_t kCustomByte = 8;

static_assert(std::is_same<ITestService::DefaultDelegator, ITestServiceDelegator>::value);

struct CustomDelegator : public ITestServiceDelegator {
 public:
  CustomDelegator(sp<ITestService>& impl) : ITestServiceDelegator(impl) {}

  // Change RepeatByte to always return the same byte.
  Status RepeatByte(int8_t /* token */, int8_t* _aidl_return) override {
    *_aidl_return = kCustomByte;
    return Status::ok();
  }
};

TEST_F(AidlTest, Delegator) {
  auto delegator = sp<ITestServiceDelegator>::make(service);

  int8_t returned_value;
  auto status = delegator->RepeatByte(12, &returned_value);
  ASSERT_TRUE(status.isOk()) << status;
  EXPECT_EQ(12, returned_value);
}

TEST_F(AidlTest, CustomDelegator) {
  auto delegator = sp<CustomDelegator>::make(service);

  int8_t returned_value;
  auto status = delegator->RepeatByte(12, &returned_value);
  ASSERT_TRUE(status.isOk()) << status;
  EXPECT_EQ(kCustomByte, returned_value);
}

TEST_F(AidlTest, DelegatorWithBinders) {
  auto delegator = sp<ITestServiceDelegator>::make(service);

  sp<INamedCallback> callback;
  auto status = delegator->GetOtherTestService(String16("callback1"), &callback);
  ASSERT_TRUE(status.isOk()) << status;

  bool verified = false;
  status = delegator->VerifyName(callback, String16("callback1"), &verified);
  ASSERT_TRUE(status.isOk()) << status;
  EXPECT_TRUE(verified);
}

TEST_F(AidlTest, DelegatorSimpl) {
  auto delegator = sp<ITestServiceDelegator>::make(service);
  sp<ITestService> impl = delegator->getImpl();
}

TEST_F(AidlTest, DelegateWrapAndGet) {
  auto delegator = delegate(service);
  auto delegator2 = delegate(service);
  EXPECT_EQ(delegator, delegator2);
}

TEST_F(AidlTest, DelegateWrapAndUnwrap) {
  sp<ITestServiceDelegator> delegator = sp<ITestServiceDelegator>::cast(delegate(service));
  EXPECT_NE(service, delegator);

  sp<ITestService> service2 = delegator->getImpl();
  EXPECT_EQ(service, service2);

  auto delegator2 = delegate(sp<ITestService>::cast(delegator));
  EXPECT_EQ(service, delegator2);
}

TEST_F(AidlTest, DelegatorSetAndGetBinder) {
  auto delegator = sp<ITestServiceDelegator>::make(service);

  sp<INamedCallback> callback;
  auto status = delegator->GetCallback(false, &callback);
  ASSERT_TRUE(status.isOk()) << status;

  // callback will be wrapped for first time and the delegator will be sent
  bool already_existing;
  status = delegator->SetOtherTestService(String16("same_one"), callback, &already_existing);
  ASSERT_TRUE(status.isOk()) << status;
  ASSERT_FALSE(already_existing);

  // unwrap the delegator here and get the original binder
  sp<INamedCallback> callback2;
  status = delegator->GetOtherTestService(String16("same_one"), &callback2);
  ASSERT_TRUE(status.isOk()) << status;
  EXPECT_EQ(callback, callback2);
}

TEST_F(AidlTest, DelegatorSettingSameBinders) {
  auto delegator = sp<ITestServiceDelegator>::make(service);

  sp<INamedCallback> callback;
  auto status = delegator->GetCallback(false, &callback);
  ASSERT_TRUE(status.isOk()) << status;

  bool already_existing = false;
  status = delegator->SetOtherTestService(String16("same_two"), callback, &already_existing);
  ASSERT_TRUE(status.isOk()) << status;
  ASSERT_FALSE(already_existing);

  status = delegator->SetOtherTestService(String16("same_two"), callback, &already_existing);
  ASSERT_TRUE(status.isOk()) << status;
  EXPECT_TRUE(already_existing);
}

TEST_F(AidlTest, DelegatorSameBinders) {
  auto delegator = sp<ITestServiceDelegator>::make(service);

  sp<INamedCallback> callback1;
  auto status = delegator->GetOtherTestService(String16("callback1"), &callback1);
  ASSERT_TRUE(status.isOk()) << status;

  sp<INamedCallback> callback1Copy;
  status = delegator->GetOtherTestService(String16("callback1"), &callback1Copy);
  ASSERT_TRUE(status.isOk()) << status;
  EXPECT_EQ(callback1, callback1Copy);
}

TEST_F(AidlTest, DelegatorWithCallback) {
  auto delegator = sp<ITestServiceDelegator>::make(service);

  sp<INamedCallback> callback;
  auto status = delegator->GetCallback(false, &callback);
  ASSERT_TRUE(status.isOk()) << status;

  String16 name;
  status = callback->GetName(&name);
  ASSERT_TRUE(status.isOk()) << status;
  EXPECT_TRUE(name.size() != 0);
}

TEST_F(AidlTest, DelegatorWithNullCallback) {
  auto delegator = sp<ITestServiceDelegator>::make(service);

  sp<INamedCallback> callback;
  auto status = delegator->GetCallback(true, &callback);
  ASSERT_TRUE(status.isOk()) << status;
  EXPECT_TRUE(callback == nullptr);
}
