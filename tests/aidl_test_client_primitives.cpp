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

#include <vector>

#include <utils/String16.h>
#include <utils/String8.h>

#include "android/aidl/tests/ByteEnum.h"
#include "android/aidl/tests/INamedCallback.h"
#include "android/aidl/tests/IntEnum.h"
#include "android/aidl/tests/LongEnum.h"

#include "aidl_test_client.h"
#include "gmock/gmock.h"

using android::IBinder;
using android::sp;
using android::String16;
using android::String8;
using android::binder::Status;

// generated
using android::aidl::tests::ByteEnum;
using android::aidl::tests::INamedCallback;
using android::aidl::tests::IntEnum;
using android::aidl::tests::ITestService;
using android::aidl::tests::LongEnum;

using testing::Eq;

struct AidlPrimitiveTest : public AidlTest {
  template <typename T, typename U, typename V>
  void DoTest(Status (ITestService::*func)(T, U*), V input) {
    U repeated;
    auto status = (*service.*func)(input, &repeated);
    ASSERT_TRUE(status.isOk()) << status;
    ASSERT_THAT(repeated, Eq(input));
  }

  template <typename T>
  void DoTest(Status (ITestService::*func)(const std::vector<T>&, std::vector<T>*, std::vector<T>*),
              const std::vector<T>& input) {
    // must be preallocated for Java servers
    std::vector<T> repeated(input.size());
    std::vector<T> reversed;
    auto status = (*service.*func)(input, &repeated, &reversed);
    ASSERT_TRUE(status.isOk()) << status;
    ASSERT_THAT(repeated, Eq(input));

    std::vector<T> reversed_input(input);
    std::reverse(reversed_input.begin(), reversed_input.end());
    ASSERT_THAT(reversed, Eq(reversed_input));
  }
};

TEST_F(AidlPrimitiveTest, aBoolean) {
  DoTest(&ITestService::RepeatBoolean, true);
}

TEST_F(AidlPrimitiveTest, aByte) {
  DoTest(&ITestService::RepeatByte, int8_t{-128});
}

TEST_F(AidlPrimitiveTest, aChar) {
  DoTest(&ITestService::RepeatChar, char16_t{'A'});
}

TEST_F(AidlPrimitiveTest, aInt) {
  DoTest(&ITestService::RepeatInt, int32_t{1 << 30});
}

TEST_F(AidlPrimitiveTest, aLong) {
  DoTest(&ITestService::RepeatLong, int64_t{1LL << 60});
}

TEST_F(AidlPrimitiveTest, aFloat) {
  DoTest(&ITestService::RepeatFloat, float{1.0f / 3.0f});
}

TEST_F(AidlPrimitiveTest, aDouble) {
  DoTest(&ITestService::RepeatDouble, double{1.0 / 3.0});
}

TEST_F(AidlPrimitiveTest, byteConstants) {
  constexpr int8_t consts[] = {ITestService::BYTE_TEST_CONSTANT};
  for (auto sent : consts) {
    DoTest(&ITestService::RepeatByte, sent);
  }
}

TEST_F(AidlPrimitiveTest, intConstants) {
  constexpr int32_t consts[] = {
      ITestService::TEST_CONSTANT,   ITestService::TEST_CONSTANT2,  ITestService::TEST_CONSTANT3,
      ITestService::TEST_CONSTANT4,  ITestService::TEST_CONSTANT5,  ITestService::TEST_CONSTANT6,
      ITestService::TEST_CONSTANT7,  ITestService::TEST_CONSTANT8,  ITestService::TEST_CONSTANT9,
      ITestService::TEST_CONSTANT10, ITestService::TEST_CONSTANT11, ITestService::TEST_CONSTANT12};
  for (auto sent : consts) {
    DoTest(&ITestService::RepeatInt, sent);
  }
}

TEST_F(AidlPrimitiveTest, longConstants) {
  constexpr int64_t consts[] = {ITestService::LONG_TEST_CONSTANT};
  for (auto sent : consts) {
    DoTest(&ITestService::RepeatLong, sent);
  }
}

TEST_F(AidlPrimitiveTest, floatConstants) {
  constexpr float consts[] = {
      ITestService::FLOAT_TEST_CONSTANT,  ITestService::FLOAT_TEST_CONSTANT2,
      ITestService::FLOAT_TEST_CONSTANT3, ITestService::FLOAT_TEST_CONSTANT4,
      ITestService::FLOAT_TEST_CONSTANT5, ITestService::FLOAT_TEST_CONSTANT6,
      ITestService::FLOAT_TEST_CONSTANT7,
  };
  for (auto sent : consts) {
    DoTest(&ITestService::RepeatFloat, sent);
  }
}

TEST_F(AidlPrimitiveTest, doubleConstants) {
  constexpr double consts[] = {
      ITestService::DOUBLE_TEST_CONSTANT,  ITestService::DOUBLE_TEST_CONSTANT2,
      ITestService::DOUBLE_TEST_CONSTANT3, ITestService::DOUBLE_TEST_CONSTANT4,
      ITestService::DOUBLE_TEST_CONSTANT5, ITestService::DOUBLE_TEST_CONSTANT6,
      ITestService::DOUBLE_TEST_CONSTANT7, ITestService::DOUBLE_TEST_CONSTANT8,
      ITestService::DOUBLE_TEST_CONSTANT9,
  };
  for (auto sent : consts) {
    DoTest(&ITestService::RepeatDouble, sent);
  }
}

TEST_F(AidlPrimitiveTest, strings) {
  std::vector<String16> strings = {
      String16("Deliver us from evil."), String16(), String16("\0\0", 2),
      // This is actually two unicode code points:
      //   U+10437: The 'small letter yee' character in the deseret alphabet
      //   U+20AC: A euro sign
      String16("\xD8\x01\xDC\x37\x20\xAC"), ITestService::STRING_TEST_CONSTANT(),
      ITestService::STRING_TEST_CONSTANT2()};
  for (auto sent : strings) {
    DoTest(&ITestService::RepeatString, sent);
  }
}

TEST_F(AidlPrimitiveTest, booleanArray) {
  DoTest(&ITestService::ReverseBoolean, {true, false, false});
}

TEST_F(AidlPrimitiveTest, byteArrvay) {
  DoTest(&ITestService::ReverseByte, {uint8_t{255}, uint8_t{0}, uint8_t{127}});
}

TEST_F(AidlPrimitiveTest, charArray) {
  DoTest(&ITestService::ReverseChar, {char16_t{'A'}, char16_t{'B'}, char16_t{'C'}});
}

TEST_F(AidlPrimitiveTest, intArray) {
  DoTest(&ITestService::ReverseInt, {1, 2, 3});
}

TEST_F(AidlPrimitiveTest, longArrayr) {
  DoTest(&ITestService::ReverseLong, {-1LL, 0LL, int64_t{1LL << 60}});
}

TEST_F(AidlPrimitiveTest, floatArrays) {
  DoTest(&ITestService::ReverseFloat, {-0.3f, -0.7f, 8.0f});
}

TEST_F(AidlPrimitiveTest, doubleArray) {
  DoTest(&ITestService::ReverseDouble, {1.0 / 3.0, 1.0 / 7.0, 42.0});
}

TEST_F(AidlPrimitiveTest, stringArray) {
  DoTest(&ITestService::ReverseString, {String16{"f"}, String16{"a"}, String16{"b"}});
}

TEST_F(AidlPrimitiveTest, byteEnumArray) {
  DoTest(&ITestService::ReverseByteEnum, {ByteEnum::FOO, ByteEnum::BAR, ByteEnum::BAR});
}

TEST_F(AidlPrimitiveTest, byteEnumArray2) {
  DoTest(&ITestService::ReverseByteEnum, {std::begin(::android::enum_range<ByteEnum>()),
                                          std::end(::android::enum_range<ByteEnum>())});
}

TEST_F(AidlPrimitiveTest, intEnumArray) {
  DoTest(&ITestService::ReverseIntEnum, {IntEnum::FOO, IntEnum::BAR, IntEnum::BAR});
}

TEST_F(AidlPrimitiveTest, longEnumArray) {
  DoTest(&ITestService::ReverseLongEnum, {LongEnum::FOO, LongEnum::BAR, LongEnum::BAR});
}

TEST_F(AidlPrimitiveTest, stringList) {
  DoTest(&ITestService::ReverseStringList, {String16{"f"}, String16{"a"}, String16{"b"}});
}

TEST_F(AidlPrimitiveTest, binderArray) {
  std::vector<String16> names = {String16{"Larry"}, String16{"Curly"}, String16{"Moe"}};

  std::vector<sp<IBinder>> input;
  for (int i = 0; i < 3; i++) {
    sp<INamedCallback> got;
    auto status = service->GetOtherTestService(names[i], &got);
    ASSERT_TRUE(status.isOk());
    input.push_back(INamedCallback::asBinder(got));
  }
  {
    std::vector<sp<INamedCallback>> got;
    auto status = service->GetInterfaceArray(names, &got);
    ASSERT_TRUE(status.isOk());
    bool verified = false;
    status = service->VerifyNamesWithInterfaceArray(got, names, &verified);
    ASSERT_TRUE(status.isOk());
    ASSERT_TRUE(verified);
    for (int i = 0; i < 3; i++) {
      String16 name;
      ASSERT_TRUE(got[i]->GetName(&name).isOk());
      ASSERT_THAT(name, Eq(names[i]));
    }
  }
  {
    std::vector<std::optional<String16>> names = {String16{"Larry"}, std::nullopt, String16{"Moe"}};
    std::optional<std::vector<sp<INamedCallback>>> got;
    auto status = service->GetNullableInterfaceArray(names, &got);
    ASSERT_TRUE(status.isOk());
    bool verified = false;
    status = service->VerifyNamesWithNullableInterfaceArray(got, names, &verified);
    ASSERT_TRUE(status.isOk());
    ASSERT_TRUE(verified);
    ASSERT_TRUE(got.has_value());
    for (int i = 0; i < 3; i++) {
      if (names[i].has_value()) {
        ASSERT_NE(got->at(i).get(), nullptr);
        String16 name;
        ASSERT_TRUE(got->at(i)->GetName(&name).isOk());
        ASSERT_THAT(name, Eq(names[i].value()));
      } else {
        ASSERT_EQ(got->at(i).get(), nullptr);
      }
    }
  }
  {
    std::vector<std::optional<String16>> names = {String16{"Larry"}, std::nullopt, String16{"Moe"}};
    std::optional<std::vector<sp<INamedCallback>>> got;
    auto status = service->GetInterfaceList(names, &got);
    ASSERT_TRUE(status.isOk());
    bool verified = false;
    status = service->VerifyNamesWithInterfaceList(got, names, &verified);
    ASSERT_TRUE(status.isOk());
    ASSERT_TRUE(verified);
    ASSERT_TRUE(got.has_value());
    for (int i = 0; i < 3; i++) {
      if (names[i].has_value()) {
        ASSERT_NE(got->at(i).get(), nullptr);
        String16 name;
        ASSERT_TRUE(got->at(i)->GetName(&name).isOk());
        ASSERT_THAT(name, Eq(names[i].value()));
      } else {
        ASSERT_EQ(got->at(i).get(), nullptr);
      }
    }
  }
  if (cpp_java_tests) {
    std::vector<sp<IBinder>> output;
    std::vector<sp<IBinder>> reversed;
    auto status = cpp_java_tests->ReverseNamedCallbackList(input, &output, &reversed);
    ASSERT_TRUE(status.isOk());
    ASSERT_THAT(output.size(), Eq(3u));
    ASSERT_THAT(reversed.size(), Eq(3u));

    for (int i = 0; i < 3; i++) {
      String16 ret;
      sp<INamedCallback> named_callback = android::interface_cast<INamedCallback>(output[i]);
      auto status = named_callback->GetName(&ret);
      ASSERT_TRUE(status.isOk());
      ASSERT_THAT(ret, Eq(names[i]));
    }

    for (int i = 0; i < 3; i++) {
      String16 ret;
      sp<INamedCallback> named_callback = android::interface_cast<INamedCallback>(reversed[i]);
      auto status = named_callback->GetName(&ret);
      ASSERT_TRUE(status.isOk());
      ASSERT_THAT(ret, Eq(names[2 - i]));
    }
  }
}

TEST_F(AidlPrimitiveTest, constantExpressions) {
  EXPECT_THAT(ITestService::A1, Eq(1));
  EXPECT_THAT(ITestService::A2, Eq(1));
  EXPECT_THAT(ITestService::A3, Eq(1));
  EXPECT_THAT(ITestService::A4, Eq(1));
  EXPECT_THAT(ITestService::A5, Eq(1));
  EXPECT_THAT(ITestService::A6, Eq(1));
  EXPECT_THAT(ITestService::A7, Eq(1));
  EXPECT_THAT(ITestService::A8, Eq(1));
  EXPECT_THAT(ITestService::A9, Eq(1));
  EXPECT_THAT(ITestService::A10, Eq(1));
  EXPECT_THAT(ITestService::A11, Eq(1));
  EXPECT_THAT(ITestService::A12, Eq(1));
  EXPECT_THAT(ITestService::A13, Eq(1));
  EXPECT_THAT(ITestService::A14, Eq(1));
  EXPECT_THAT(ITestService::A15, Eq(1));
  EXPECT_THAT(ITestService::A16, Eq(1));
  EXPECT_THAT(ITestService::A17, Eq(1));
  EXPECT_THAT(ITestService::A18, Eq(1));
  EXPECT_THAT(ITestService::A19, Eq(1));
  EXPECT_THAT(ITestService::A20, Eq(1));
  EXPECT_THAT(ITestService::A21, Eq(1));
  EXPECT_THAT(ITestService::A22, Eq(1));
  EXPECT_THAT(ITestService::A23, Eq(1));
  EXPECT_THAT(ITestService::A24, Eq(1));
  EXPECT_THAT(ITestService::A25, Eq(1));
  EXPECT_THAT(ITestService::A26, Eq(1));
  EXPECT_THAT(ITestService::A27, Eq(1));
  EXPECT_THAT(ITestService::A28, Eq(1));
  EXPECT_THAT(ITestService::A29, Eq(1));
  EXPECT_THAT(ITestService::A30, Eq(1));
  EXPECT_THAT(ITestService::A31, Eq(1));
  EXPECT_THAT(ITestService::A32, Eq(1));
  EXPECT_THAT(ITestService::A33, Eq(1));
  EXPECT_THAT(ITestService::A34, Eq(1));
  EXPECT_THAT(ITestService::A35, Eq(1));
  EXPECT_THAT(ITestService::A36, Eq(1));
  EXPECT_THAT(ITestService::A37, Eq(1));
  EXPECT_THAT(ITestService::A38, Eq(1));
  EXPECT_THAT(ITestService::A39, Eq(1));
  EXPECT_THAT(ITestService::A40, Eq(1));
  EXPECT_THAT(ITestService::A41, Eq(1));
  EXPECT_THAT(ITestService::A42, Eq(1));
  EXPECT_THAT(ITestService::A43, Eq(1));
  EXPECT_THAT(ITestService::A44, Eq(1));
  EXPECT_THAT(ITestService::A45, Eq(1));
  EXPECT_THAT(ITestService::A46, Eq(1));
  EXPECT_THAT(ITestService::A47, Eq(1));
  EXPECT_THAT(ITestService::A48, Eq(1));
  EXPECT_THAT(ITestService::A49, Eq(1));
  EXPECT_THAT(ITestService::A50, Eq(1));
  EXPECT_THAT(ITestService::A51, Eq(1));
  EXPECT_THAT(ITestService::A52, Eq(1));
  EXPECT_THAT(ITestService::A53, Eq(1));
  EXPECT_THAT(ITestService::A54, Eq(1));
  EXPECT_THAT(ITestService::A55, Eq(1));
  EXPECT_THAT(ITestService::A56, Eq(1));
  EXPECT_THAT(ITestService::A57, Eq(1));
  EXPECT_THAT(ITestService::FLOAT_TEST_CONSTANT4, Eq(2.2f));
  EXPECT_THAT(ITestService::FLOAT_TEST_CONSTANT5, Eq(-2.2f));
  EXPECT_THAT(ITestService::DOUBLE_TEST_CONSTANT4, Eq(2.2));
  EXPECT_THAT(ITestService::DOUBLE_TEST_CONSTANT5, Eq(-2.2));
}
