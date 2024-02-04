/*
 * Copyright (C) 2020 The Android Open Source Project
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

#pragma once

#include <android/aidl/tests/ICppJavaTests.h>
#include <android/aidl/tests/ITestService.h>
#include <binder/IServiceManager.h>
#include <binder/ProcessState.h>
#include <gtest/gtest.h>
#include <utils/String16.h>

using android::sp;
using android::aidl::tests::BackendType;
using android::aidl::tests::ICppJavaTests;
using android::aidl::tests::ITestService;

class AidlTest : public testing::Test {
 public:
  void SetUp() override {
    using android::OK;
    using android::String16;
    using android::waitForService;

    android::ProcessState::self()->setThreadPoolMaxThreadCount(1);
    android::ProcessState::self()->startThreadPool();
    service = waitForService<ITestService>(ITestService::descriptor);
    ASSERT_NE(nullptr, service);

    sp<android::IBinder> ibinder;
    auto status = service->GetCppJavaTests(&ibinder);
    ASSERT_TRUE(status.isOk());
    cpp_java_tests = android::interface_cast<ICppJavaTests>(ibinder);

    status = service->getBackendType(&backend);
    ASSERT_TRUE(status.isOk()) << status;

    if (backend != BackendType::RUST) {
      ASSERT_NE(cpp_java_tests, nullptr);
    }
  }

  BackendType backend;
  sp<ITestService> service;
  sp<ICppJavaTests> cpp_java_tests;
};
