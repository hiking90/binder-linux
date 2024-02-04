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

#include <android/aidl/loggable/ILoggableInterface.h>

#include "aidl_test_client.h"

#include <android/aidl/loggable/BpLoggableInterface.h>
#include <android/aidl/tests/BackendType.h>
#include <binder/IServiceManager.h>
#include <binder/ParcelFileDescriptor.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <utils/String16.h>

using android::IBinder;
using android::sp;
using android::String16;
using android::aidl::loggable::BpLoggableInterface;
using android::aidl::loggable::Data;
using android::aidl::loggable::Enum;
using android::aidl::loggable::ILoggableInterface;
using android::aidl::loggable::Union;
using android::aidl::tests::BackendType;
using android::os::ParcelFileDescriptor;
using std::optional;
using std::pair;
using std::string;
using std::vector;
using testing::Eq;

TEST_F(AidlTest, LoggableInterface) {
  BackendType backendType;
  auto status = service->getBackendType(&backendType);
  EXPECT_TRUE(status.isOk());
  if (backendType != BackendType::CPP) GTEST_SKIP();

  sp<ILoggableInterface> loggable =
      android::waitForService<ILoggableInterface>(ILoggableInterface::descriptor);
  ASSERT_NE(nullptr, loggable);

  BpLoggableInterface::TransactionLog log;
  BpLoggableInterface::logFunc = [&](const BpLoggableInterface::TransactionLog& tx) { log = tx; };

  bool boolValue = true;
  vector<bool> boolArray{false, true};
  int8_t byteValue = 41;
  vector<uint8_t> byteArray{42, 43};
  char16_t charValue = 'x';
  vector<char16_t> charArray{'a', 'b', 'c'};
  int32_t intValue{44};
  vector<int32_t> intArray{45, 46};
  int64_t longValue = 47;
  vector<int64_t> longArray{48, 49};
  float floatValue{50};
  vector<float> floatArray{51, 52};
  double doubleValue{52};
  vector<double> doubleArray{53, 54};
  String16 stringValue("def");
  vector<String16> stringArray{String16("ghi"), String16("jkl")};
  vector<String16> listValue{String16("mno")};
  Data dataValue;
  dataValue.num = 42;
  dataValue.str = "abc";
  dataValue.nestedUnion = "def";
  dataValue.nestedEnum = Enum::FOO;
  sp<IBinder> binderValue;
  optional<ParcelFileDescriptor> pfdValue;
  vector<ParcelFileDescriptor> pfdArray;
  vector<String16> _aidl_return;

  status = loggable->LogThis(boolValue, &boolArray, byteValue, &byteArray, charValue, &charArray,
                             intValue, &intArray, longValue, &longArray, floatValue, &floatArray,
                             doubleValue, &doubleArray, stringValue, &stringArray, &listValue,
                             dataValue, binderValue, &pfdValue, &pfdArray, &_aidl_return);
  EXPECT_TRUE(status.isOk());
  EXPECT_EQ(vector<String16>{String16("loggable")}, _aidl_return);

  // check the captured log
  EXPECT_EQ("[loggable]", log.result);
  EXPECT_EQ("android.aidl.loggable.ILoggableInterface", log.interface_name);
  EXPECT_EQ("LogThis", log.method_name);
  EXPECT_EQ(0, log.exception_code);
  EXPECT_EQ("", log.exception_message);
  EXPECT_EQ(0, log.transaction_error);
  EXPECT_EQ(0, log.service_specific_error_code);
  EXPECT_THAT(
      log.input_args,
      Eq(vector<pair<string, string>>{
          {"boolValue", "true"},
          {"boolArray", "[false, true]"},
          {"byteValue", "41"},
          {"byteArray", "[42, 43]"},
          {"charValue", "x"},
          {"charArray", "[a, b, c]"},
          {"intValue", "44"},
          {"intArray", "[45, 46]"},
          {"longValue", "47"},
          {"longArray", "[48, 49]"},
          {"floatValue", "50.000000"},
          {"floatArray", "[51.000000, 52.000000]"},
          {"doubleValue", "52.000000"},
          {"doubleArray", "[53.000000, 54.000000]"},
          {"stringValue", "def"},
          {"stringArray", "[ghi, jkl]"},
          {"listValue", "[mno]"},
          {"dataValue", "Data{num: 42, str: abc, nestedUnion: Union{str: def}, nestedEnum: FOO}"},
          {"binderValue", "(null)"},
          {"pfdValue", "(null)"},
          {"pfdArray", "[]"},
      }));
  EXPECT_THAT(log.output_args,
              Eq(vector<pair<string, string>>{{"boolArray", "[false, true]"},
                                              {"byteArray", "[42, 43]"},
                                              {"charArray", "[a, b, c]"},
                                              {"intArray", "[45, 46]"},
                                              {"longArray", "[48, 49]"},
                                              {"floatArray", "[51.000000, 52.000000]"},
                                              {"doubleArray", "[53.000000, 54.000000]"},
                                              {"stringArray", "[ghi, jkl]"},
                                              {"listValue", "[mno]"},
                                              {"pfdValue", "(null)"},
                                              {"pfdArray", "[]"}}));
}
