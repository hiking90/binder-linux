/*
 * Copyright (C) 2016 The Android Open Source Project
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

#include <optional>
#include <string>
#include <vector>

#include <utils/String16.h>
#include <utils/String8.h>

#include "aidl_test_client.h"
#include "gmock/gmock.h"

using android::IBinder;
using android::sp;
using android::String16;
using android::String8;
using android::binder::Status;

using android::aidl::tests::BackendType;
using android::aidl::tests::ITestService;

using testing::Eq;

TEST_F(AidlTest, repeatUtf8String) {
  const std::vector<std::string> utf8_inputs = {
      std::string("Deliver us from evil."),
      std::string(),
      std::string("\0\0", 2),
      // Similarly, the utf8 encodings of the small letter yee and euro sign.
      std::string("\xF0\x90\x90\xB7\xE2\x82\xAC"),
      ITestService::STRING_TEST_CONSTANT_UTF8(),
  };

  for (const auto& input : utf8_inputs) {
    std::string reply;
    auto status = service->RepeatUtf8CppString(input, &reply);
    ASSERT_TRUE(status.isOk());
    ASSERT_THAT(reply, Eq(input));
  }

  std::optional<std::string> reply;
  auto status = service->RepeatNullableUtf8CppString(std::nullopt, &reply);
  ASSERT_TRUE(status.isOk());
  ASSERT_FALSE(reply.has_value());

  for (const auto& input : utf8_inputs) {
    std::optional<std::string> reply;
    auto status = service->RepeatNullableUtf8CppString(input, &reply);
    ASSERT_TRUE(status.isOk());
    ASSERT_TRUE(reply.has_value());
    ASSERT_THAT(*reply, Eq(input));
  }
}

TEST_F(AidlTest, reverseUtf8StringArray) {
  std::vector<std::string> input = {"a", "", "\xc3\xb8"};
  decltype(input) repeated;
  if (backend == BackendType::JAVA) {
    repeated = decltype(input)(input.size());
  }
  decltype(input) reversed;

  auto status = service->ReverseUtf8CppString(input, &repeated, &reversed);
  ASSERT_TRUE(status.isOk()) << status;
  ASSERT_THAT(repeated, Eq(input));

  decltype(input) reversed_input(input);
  std::reverse(reversed_input.begin(), reversed_input.end());
  ASSERT_THAT(reversed, Eq(reversed_input));
}

struct AidlStringArrayTest : public AidlTest {
  void DoTest(
      Status (ITestService::*func)(const std::optional<std::vector<std::optional<std::string>>>&,
                                   std::optional<std::vector<std::optional<std::string>>>*,
                                   std::optional<std::vector<std::optional<std::string>>>*)) {
    std::optional<std::vector<std::optional<std::string>>> input;
    decltype(input) repeated;
    decltype(input) reversed;

    auto status = (*service.*func)(input, &repeated, &reversed);
    ASSERT_TRUE(status.isOk()) << status;

    if (func == &ITestService::ReverseUtf8CppStringList && backend == BackendType::JAVA) {
      // Java cannot clear the input variable to return a null value. It can
      // only ever fill out a list.
      ASSERT_TRUE(repeated.has_value());
    } else {
      ASSERT_FALSE(repeated.has_value());
    }

    ASSERT_FALSE(reversed.has_value());

    input = std::vector<std::optional<std::string>>();
    input->push_back("Deliver us from evil.");
    input->push_back(std::nullopt);
    input->push_back("\xF0\x90\x90\xB7\xE2\x82\xAC");

    // usable size needs to be initialized for Java
    repeated = std::vector<std::optional<std::string>>(input->size());

    status = (*service.*func)(input, &repeated, &reversed);
    ASSERT_TRUE(status.isOk()) << status;
    ASSERT_TRUE(reversed.has_value());
    ASSERT_TRUE(repeated.has_value());
    ASSERT_THAT(reversed->size(), Eq(input->size()));
    ASSERT_THAT(repeated->size(), Eq(input->size()));

    for (size_t i = 0; i < input->size(); i++) {
      auto input_str = (*input)[i];
      auto repeated_str = (*repeated)[i];
      auto reversed_str = (*reversed)[(reversed->size() - 1) - i];
      if (!input_str) {
        ASSERT_FALSE(repeated_str.has_value());
        ASSERT_FALSE(reversed_str.has_value());
        // 3 nullptrs to strings.  No need to compare values.
        continue;
      }
      ASSERT_TRUE(repeated_str.has_value());
      ASSERT_TRUE(reversed_str.has_value());

      ASSERT_THAT(*repeated_str, Eq(*input_str));
      ASSERT_THAT(*reversed_str, Eq(*input_str));
    }
  }
};

TEST_F(AidlStringArrayTest, nullableList) {
  DoTest(&ITestService::ReverseUtf8CppStringList);
}

TEST_F(AidlStringArrayTest, nullableArray) {
  DoTest(&ITestService::ReverseNullableUtf8CppString);
}
