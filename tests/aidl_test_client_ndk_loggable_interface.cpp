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

#include <aidl/android/aidl/loggable/ILoggableInterface.h>

#include <android/binder_auto_utils.h>
#include <android/binder_manager.h>
#include <binder/ProcessState.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <aidl/android/aidl/loggable/BpLoggableInterface.h>
#include <aidl/android/aidl/tests/BackendType.h>
#include <aidl/android/aidl/tests/ITestService.h>

using aidl::android::aidl::loggable::BpLoggableInterface;
using aidl::android::aidl::loggable::Data;
using aidl::android::aidl::loggable::Enum;
using aidl::android::aidl::loggable::ILoggableInterface;
using aidl::android::aidl::tests::BackendType;
using aidl::android::aidl::tests::ITestService;
using std::optional;
using std::pair;
using std::shared_ptr;
using std::string;
using std::vector;
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

TEST_F(AidlTest, LoggableInterface) {
  std::shared_ptr<ITestService> service = getService<ITestService>();
  ASSERT_NE(nullptr, service.get());

  BackendType backendType;
  ndk::ScopedAStatus status = service->getBackendType(&backendType);
  EXPECT_TRUE(status.isOk()) << status.getDescription();
  if (backendType != BackendType::CPP) GTEST_SKIP();

  shared_ptr<ILoggableInterface> loggable = getService<ILoggableInterface>();
  ASSERT_NE(nullptr, loggable.get());

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
  string stringValue("def");
  vector<string> stringArray{string("ghi"), string("jkl")};
  vector<string> listValue{string("mno")};
  Data dataValue;
  dataValue.num = 42;
  dataValue.str = "abc";
  dataValue.nestedUnion = "def";
  dataValue.nestedEnum = Enum::FOO;
  ndk::SpAIBinder binderValue;
  ndk::ScopedFileDescriptor pfdValue;
  vector<ndk::ScopedFileDescriptor> pfdArray;
  vector<string> _aidl_return;
  status = loggable->LogThis(boolValue, &boolArray, byteValue, &byteArray, charValue, &charArray,
                             intValue, &intArray, longValue, &longArray, floatValue, &floatArray,
                             doubleValue, &doubleArray, stringValue, &stringArray, &listValue,
                             dataValue, binderValue, &pfdValue, &pfdArray, &_aidl_return);
  EXPECT_TRUE(status.isOk());
  EXPECT_EQ(vector<string>{string("loggable")}, _aidl_return);

  // check the captured log
  EXPECT_EQ("[loggable]", log.result);
  EXPECT_EQ("android.aidl.loggable.ILoggableInterface", log.interface_name);
  EXPECT_EQ("LogThis", log.method_name);
  EXPECT_EQ(0, log.exception_code);
  EXPECT_EQ("", log.exception_message);
  EXPECT_EQ(0, log.transaction_error);
  EXPECT_EQ(0, log.service_specific_error_code);
  EXPECT_THAT(log.input_args,
              Eq(vector<pair<string, string>>{
                  {"in_boolValue", "true"},
                  {"in_boolArray", "[false, true]"},
                  {"in_byteValue", "41"},
                  {"in_byteArray", "[42, 43]"},
                  {"in_charValue", "x"},
                  {"in_charArray", "[a, b, c]"},
                  {"in_intValue", "44"},
                  {"in_intArray", "[45, 46]"},
                  {"in_longValue", "47"},
                  {"in_longArray", "[48, 49]"},
                  {"in_floatValue", "50.000000"},
                  {"in_floatArray", "[51.000000, 52.000000]"},
                  {"in_doubleValue", "52.000000"},
                  {"in_doubleArray", "[53.000000, 54.000000]"},
                  {"in_stringValue", "def"},
                  {"in_stringArray", "[ghi, jkl]"},
                  {"in_listValue", "[mno]"},
                  {"in_dataValue",
                   "Data{num: 42, str: abc, nestedUnion: Union{str: def}, nestedEnum: FOO}"},
                  {"in_binderValue", "binder:0x0"},
                  {"in_pfdValue", "fd:-1"},
                  {"in_pfdArray", "[]"},
              }));
  EXPECT_THAT(log.output_args,
              Eq(vector<pair<string, string>>{{"in_boolArray", "[false, true]"},
                                              {"in_byteArray", "[42, 43]"},
                                              {"in_charArray", "[a, b, c]"},
                                              {"in_intArray", "[45, 46]"},
                                              {"in_longArray", "[48, 49]"},
                                              {"in_floatArray", "[51.000000, 52.000000]"},
                                              {"in_doubleArray", "[53.000000, 54.000000]"},
                                              {"in_stringArray", "[ghi, jkl]"},
                                              {"in_listValue", "[mno]"},
                                              {"in_pfdValue", "fd:-1"},
                                              {"in_pfdArray", "[]"}}));
}
