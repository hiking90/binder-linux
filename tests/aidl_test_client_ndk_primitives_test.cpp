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
};

TEST_F(AidlTest, InterfaceExchange) {
  auto service = getService<ITestService>();
  std::vector<std::string> names = {"Larry", "Curly", "Moe"};

  for (size_t i = 0; i < names.size(); i++) {
    std::shared_ptr<INamedCallback> got;
    ASSERT_TRUE(service->GetOtherTestService(names[i], &got).isOk());
    std::string name;
    ASSERT_TRUE(got->GetName(&name).isOk());
    ASSERT_THAT(name, Eq(names[i]));
  }
  {
    std::vector<std::shared_ptr<INamedCallback>> got;
    ASSERT_TRUE(service->GetInterfaceArray(names, &got).isOk());

    bool verified = false;
    ASSERT_TRUE(service->VerifyNamesWithInterfaceArray(got, names, &verified).isOk());
    ASSERT_TRUE(verified);

    for (size_t i = 0; i < names.size(); i++) {
      std::string name;
      ASSERT_TRUE(got[i]->GetName(&name).isOk());
      ASSERT_THAT(name, Eq(names[i]));
    }
  }
  {
    std::vector<std::optional<std::string>> names = {"Larry", std::nullopt, "Moe"};
    std::optional<std::vector<std::shared_ptr<INamedCallback>>> got;
    ASSERT_TRUE(service->GetNullableInterfaceArray(names, &got).isOk());
    bool verified = false;
    ASSERT_TRUE(service->VerifyNamesWithNullableInterfaceArray(got, names, &verified).isOk());
    ASSERT_TRUE(verified);
    ASSERT_TRUE(got.has_value());
    for (size_t i = 0; i < names.size(); i++) {
      if (names[i].has_value()) {
        ASSERT_NE(got->at(i).get(), nullptr);
        std::string name;
        ASSERT_TRUE(got->at(i)->GetName(&name).isOk());
        ASSERT_THAT(name, Eq(names[i].value()));
      } else {
        ASSERT_EQ(got->at(i).get(), nullptr);
      }
    }
  }
  {
    std::vector<std::optional<std::string>> names = {"Larry", std::nullopt, "Moe"};
    std::optional<std::vector<std::shared_ptr<INamedCallback>>> got;
    ASSERT_TRUE(service->GetInterfaceList(names, &got).isOk());
    bool verified = false;
    ASSERT_TRUE(service->VerifyNamesWithInterfaceList(got, names, &verified).isOk());
    ASSERT_TRUE(verified);
    ASSERT_TRUE(got.has_value());
    for (size_t i = 0; i < names.size(); i++) {
      if (names[i].has_value()) {
        ASSERT_NE(got->at(i).get(), nullptr);
        std::string name;
        ASSERT_TRUE(got->at(i)->GetName(&name).isOk());
        ASSERT_THAT(name, Eq(names[i].value()));
      } else {
        ASSERT_EQ(got->at(i).get(), nullptr);
      }
    }
  }
}
