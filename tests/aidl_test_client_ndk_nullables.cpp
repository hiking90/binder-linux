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

#include <android/binder_auto_utils.h>
#include <android/binder_manager.h>
#include <binder/ProcessState.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <aidl/android/aidl/tests/ITestService.h>

using aidl::android::aidl::tests::BackendType;
using aidl::android::aidl::tests::INamedCallback;
using aidl::android::aidl::tests::ITestService;
using testing::Eq;

struct AidlTest : testing::Test {
  template <typename T>
  std::shared_ptr<T> getService() {
    android::ProcessState::self()->setThreadPoolMaxThreadCount(1);
    android::ProcessState::self()->startThreadPool();
    ndk::SpAIBinder binder = ndk::SpAIBinder(AServiceManager_waitForService(T::descriptor));
    return T::fromBinder(binder);
  }
  void SetUp() override {
    service = getService<ITestService>();
    auto status = service->getBackendType(&backend);
    ASSERT_TRUE(status.isOk()) << status.getDescription();
  }
  std::shared_ptr<ITestService> service;
  BackendType backend;

  template <typename T>
  void DoTest(ndk::ScopedAStatus (ITestService::*func)(const std::optional<T>&, std::optional<T>*),
              std::optional<T> input) {
    std::optional<T> output;
    auto status = (*service.*func)(input, &output);
    ASSERT_TRUE(status.isOk());
    ASSERT_TRUE(output.has_value());
    ASSERT_THAT(*output, Eq(*input));

    input.reset();
    status = (*service.*func)(input, &output);
    ASSERT_TRUE(status.isOk());
    ASSERT_FALSE(output.has_value());
  }
};

TEST_F(AidlTest, parcelableArray) {
  std::vector<std::optional<ITestService::Empty>> input;
  input.push_back(ITestService::Empty());
  input.push_back(std::nullopt);
  DoTest(&ITestService::RepeatNullableParcelableArray, std::make_optional(input));
}

TEST_F(AidlTest, parcelableList) {
  std::vector<std::optional<ITestService::Empty>> input;
  input.push_back(ITestService::Empty());
  input.push_back(std::nullopt);
  DoTest(&ITestService::RepeatNullableParcelableList, std::make_optional(input));
}

TEST_F(AidlTest, nullBinder) {
  auto status = service->TakesAnIBinder(nullptr);
  ASSERT_THAT(status.getStatus(), Eq(STATUS_UNEXPECTED_NULL)) << status.getDescription();
  // Note that NDK backend checks null before transaction while C++ backends doesn't.
}

TEST_F(AidlTest, binderListWithNull) {
  std::vector<ndk::SpAIBinder> input{service->asBinder(), nullptr};
  auto status = service->TakesAnIBinderList(input);
  ASSERT_THAT(status.getStatus(), Eq(STATUS_UNEXPECTED_NULL));
  // Note that NDK backend checks null before transaction while C++ backends doesn't.
}

TEST_F(AidlTest, nonNullBinder) {
  auto status = service->TakesAnIBinder(service->asBinder());
  ASSERT_TRUE(status.isOk());
}

TEST_F(AidlTest, binderListWithoutNull) {
  std::vector<ndk::SpAIBinder> input{service->asBinder()};
  auto status = service->TakesAnIBinderList(input);
  ASSERT_TRUE(status.isOk());
}

TEST_F(AidlTest, nullBinderToAnnotatedMethod) {
  auto status = service->TakesANullableIBinder(nullptr);
  ASSERT_TRUE(status.isOk());
}

TEST_F(AidlTest, binderListWithNullToAnnotatedMethod) {
  std::vector<ndk::SpAIBinder> input{service->asBinder(), nullptr};
  auto status = service->TakesANullableIBinderList(input);
  ASSERT_TRUE(status.isOk());
}

TEST_F(AidlTest, binderArray) {
  std::vector<ndk::SpAIBinder> repeated;
  if (backend == BackendType::JAVA) {
    // Java can only modify out-argument arrays in-place
    repeated.resize(2);
  }
  // get INamedCallback for "SpAIBinder" object
  std::shared_ptr<INamedCallback> callback;
  auto status = service->GetCallback(false, &callback);
  ASSERT_TRUE(status.isOk()) << status.getDescription();

  std::vector<ndk::SpAIBinder> reversed;
  std::vector<ndk::SpAIBinder> input{service->asBinder(), callback->asBinder()};
  status = service->ReverseIBinderArray(input, &repeated, &reversed);
  ASSERT_TRUE(status.isOk()) << status.getDescription();

  EXPECT_THAT(input, Eq(repeated));
  std::reverse(std::begin(reversed), std::end(reversed));
  EXPECT_THAT(input, Eq(reversed));
}

TEST_F(AidlTest, nullableBinderArray) {
  std::optional<std::vector<ndk::SpAIBinder>> repeated;
  if (backend == BackendType::JAVA) {
    // Java can only modify out-argument arrays in-place
    repeated = std::vector<ndk::SpAIBinder>(2);
  }

  std::optional<std::vector<ndk::SpAIBinder>> reversed;
  std::optional<std::vector<ndk::SpAIBinder>> input =
      std::vector<ndk::SpAIBinder>{service->asBinder(), service->asBinder()};
  auto status = service->ReverseNullableIBinderArray(input, &repeated, &reversed);
  ASSERT_TRUE(status.isOk()) << status.getDescription();

  EXPECT_THAT(input, Eq(repeated));
  ASSERT_TRUE(reversed);
  std::reverse(std::begin(*reversed), std::end(*reversed));
  EXPECT_THAT(input, Eq(reversed));
}
