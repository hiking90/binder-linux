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

#include <android/aidl/fixedsizearray/FixedSizeArrayExample.h>
#include <android/aidl/tests/ParcelableForToString.h>
#include <android/aidl/tests/extension/MyExt.h>
#include <android/aidl/tests/extension/MyExt2.h>
#include <android/aidl/tests/extension/MyExtLike.h>
#include <android/aidl/tests/unions/EnumUnion.h>
#include <binder/Binder.h>
#include "aidl_test_client.h"

#include <string>
#include <vector>

using android::IInterface;
using android::sp;
using android::String16;
using android::String8;
using android::aidl::fixedsizearray::FixedSizeArrayExample;
using android::aidl::tests::BadParcelable;
using android::aidl::tests::ConstantExpressionEnum;
using android::aidl::tests::GenericStructuredParcelable;
using android::aidl::tests::INamedCallback;
using android::aidl::tests::IntEnum;
using android::aidl::tests::ITestService;
using android::aidl::tests::OtherParcelableForToString;
using android::aidl::tests::ParcelableForToString;
using android::aidl::tests::RecursiveList;
using android::aidl::tests::SimpleParcelable;
using android::aidl::tests::StructuredParcelable;
using android::aidl::tests::Union;
using android::aidl::tests::extension::ExtendableParcelable;
using android::aidl::tests::extension::MyExt;
using android::aidl::tests::extension::MyExt2;
using android::aidl::tests::extension::MyExtLike;
using android::aidl::tests::unions::EnumUnion;
using IntParcelable = android::aidl::fixedsizearray::FixedSizeArrayExample::IntParcelable;
using IRepeatFixedSizeArray =
    android::aidl::fixedsizearray::FixedSizeArrayExample::IRepeatFixedSizeArray;
using android::BBinder;
using android::IBinder;
using android::OK;
using android::binder::Status;
using android::os::PersistableBundle;
using std::string;
using std::vector;

TEST_F(AidlTest, BadParcelable) {
  if (!cpp_java_tests) GTEST_SKIP() << "Service does not support the CPP/Java-only tests.";

  BadParcelable output;
  {
    BadParcelable bad(/*bad=*/true, "Booya", 42);
    Status status = cpp_java_tests->RepeatBadParcelable(bad, &output);
    ASSERT_FALSE(status.isOk());
    EXPECT_EQ(status.exceptionCode(), Status::Exception::EX_BAD_PARCELABLE);
  }
  {
    BadParcelable not_bad(/*bad=*/false, "Booya", 42);
    Status status = cpp_java_tests->RepeatBadParcelable(not_bad, &output);
    ASSERT_TRUE(status.isOk());
    EXPECT_EQ(not_bad, output);
  }
}

TEST_F(AidlTest, RepeatSimpleParcelable) {
  if (!cpp_java_tests) GTEST_SKIP() << "Service does not support the CPP/Java-only tests.";

  SimpleParcelable input("Booya", 42);
  SimpleParcelable out_param, returned;
  Status status = cpp_java_tests->RepeatSimpleParcelable(input, &out_param, &returned);
  ASSERT_TRUE(status.isOk()) << status.toString8();
  EXPECT_EQ(input, out_param) << input.toString() << " " << out_param.toString();
  EXPECT_EQ(input, returned) << input.toString() << " " << returned.toString();
}

TEST_F(AidlTest, RepeatGenericStructureParcelable) {
  if (!cpp_java_tests) GTEST_SKIP() << "Service does not support the CPP/Java-only tests.";

  GenericStructuredParcelable<int32_t, StructuredParcelable, IntEnum> input, out_param, returned;
  input.a = 41;
  input.b = 42;
  Status status = cpp_java_tests->RepeatGenericParcelable(input, &out_param, &returned);
  ASSERT_TRUE(status.isOk()) << status.toString8();
  EXPECT_EQ(input, out_param);
  EXPECT_EQ(input, returned);
}

TEST_F(AidlTest, ReverseSimpleParcelable) {
  if (!cpp_java_tests) GTEST_SKIP() << "Service does not support the CPP/Java-only tests.";

  const vector<SimpleParcelable> original{SimpleParcelable("first", 0),
                                          SimpleParcelable("second", 1),
                                          SimpleParcelable("third", 2)};
  vector<SimpleParcelable> repeated;
  if (backend == BackendType::JAVA) {
    repeated = vector<SimpleParcelable>(original.size());
  }
  vector<SimpleParcelable> reversed;
  Status status = cpp_java_tests->ReverseSimpleParcelables(original, &repeated, &reversed);
  ASSERT_TRUE(status.isOk()) << status.toString8();

  EXPECT_EQ(repeated, original);

  std::reverse(reversed.begin(), reversed.end());
  EXPECT_EQ(reversed, original);
}

TEST_F(AidlTest, ConfirmPersistableBundles) {
  if (!cpp_java_tests) GTEST_SKIP() << "Service does not support the CPP/Java-only tests.";

  PersistableBundle empty_bundle, returned;
  Status status = cpp_java_tests->RepeatPersistableBundle(empty_bundle, &returned);
  ASSERT_TRUE(status.isOk()) << status.toString8();
  EXPECT_EQ(empty_bundle, returned);
}

TEST_F(AidlTest, ConfirmPersistableBundlesNonEmpty) {
  if (!cpp_java_tests) GTEST_SKIP() << "Service does not support the CPP/Java-only tests.";

  PersistableBundle non_empty_bundle, returned;
  non_empty_bundle.putBoolean(String16("test_bool"), false);
  non_empty_bundle.putInt(String16("test_int"), 33);
  non_empty_bundle.putLong(String16("test_long"), 34359738368L);
  non_empty_bundle.putDouble(String16("test_double"), 1.1);
  non_empty_bundle.putString(String16("test_string"), String16("Woot!"));
  non_empty_bundle.putBooleanVector(String16("test_bool_vector"),
                                    {true, false, true});
  non_empty_bundle.putIntVector(String16("test_int_vector"), {33, 44, 55, 142});
  non_empty_bundle.putLongVector(String16("test_long_vector"),
                                 {34L, 8371L, 34359738375L});
  non_empty_bundle.putDoubleVector(String16("test_double_vector"), {2.2, 5.4});
  non_empty_bundle.putStringVector(String16("test_string_vector"),
                                   {String16("hello"), String16("world!")});
  PersistableBundle nested_bundle;
  nested_bundle.putInt(String16("test_nested_int"), 345);
  non_empty_bundle.putPersistableBundle(String16("test_persistable_bundle"),
                                        nested_bundle);

  Status status = cpp_java_tests->RepeatPersistableBundle(non_empty_bundle, &returned);
  ASSERT_TRUE(status.isOk()) << status.toString8();
  EXPECT_EQ(non_empty_bundle, returned);
}

TEST_F(AidlTest, ReversePersistableBundles) {
  if (!cpp_java_tests) GTEST_SKIP() << "Service does not support the CPP/Java-only tests.";

  PersistableBundle first;
  PersistableBundle second;
  PersistableBundle third;
  first.putInt(String16("test_int"), 1231);
  second.putLong(String16("test_long"), 222222L);
  third.putDouble(String16("test_double"), 10.8);
  const vector<PersistableBundle> original{first, second, third};

  vector<PersistableBundle> repeated;
  if (backend == BackendType::JAVA) {
    repeated = vector<PersistableBundle>(original.size());
  }
  vector<PersistableBundle> reversed;
  Status status = cpp_java_tests->ReversePersistableBundles(original, &repeated, &reversed);
  ASSERT_TRUE(status.isOk()) << status.toString8();

  EXPECT_EQ(repeated, original);

  std::reverse(reversed.begin(), reversed.end());
  EXPECT_EQ(reversed, original);
}

TEST_F(AidlTest, ReverseUnion) {
  if (!cpp_java_tests) GTEST_SKIP() << "Service does not support the CPP/Java-only tests.";

  Union original = Union::make<Union::ns>({1, 2, 3});
  Union repeated, reversed;
  Status status = cpp_java_tests->ReverseUnion(original, &repeated, &reversed);
  ASSERT_TRUE(status.isOk()) << status.toString8();

  EXPECT_EQ(repeated, original);

  std::reverse(reversed.get<Union::ns>().begin(), reversed.get<Union::ns>().end());
  EXPECT_EQ(reversed, original);
}

TEST_F(AidlTest, UnionUsage) {
  // default ctor inits with first member's default value
  EXPECT_EQ(Union::make<Union::ns>(), Union());

  // make<tag>(...) to create a value for a tag.
  Union one_two_three = Union::make<Union::ns>({1, 2, 3});

  // getTag() queries the tag of the content
  EXPECT_EQ(Union::ns, one_two_three.getTag());

  // Ctor(...) works if a target tag has a unique type among fields.
  EXPECT_EQ(one_two_three, Union(std::vector{1, 2, 3}));
  EXPECT_EQ(one_two_three, std::vector<int>({1, 2, 3}));

  // Use std::in_place_index<tag> to avoid "move"
  // Note that make<tag>(...) involves "move" of the content value
  EXPECT_EQ(Union::make<Union::ns>(3, 0),
            Union(std::in_place_index<static_cast<size_t>(Union::ns)>, 3, 0));

  Union one_two = one_two_three;
  // get<tag> can be used to modify the content
  one_two.get<Union::ns>().pop_back();
  EXPECT_EQ(one_two, std::vector<int>({1, 2}));
  // get<tag> can be lvalue
  one_two.get<Union::ns>() = std::vector<int>{1, 2};
  EXPECT_EQ(one_two, std::vector<int>({1, 2}));

  // abort with a bad access
  EXPECT_DEATH(one_two.get<Union::n>(), "bad access");

  // set<tag>(...) overwrites the content with a new tag
  one_two_three.set<Union::s>("123");
  EXPECT_EQ(one_two_three, std::string("123"));

  // Or, you can simply assign a new value.
  // note that this works only if the target type is unique
  one_two_three = std::vector<std::string>{"1", "2", "3"};
  EXPECT_EQ(Union::ss, one_two_three.getTag());
}

TEST_F(AidlTest, UnionDefaultConstructorInitializeWithFirstMember) {
  EXPECT_EQ(Union::make<Union::ns>(), Union());  // int[] ns
  EXPECT_EQ(EnumUnion::make<EnumUnion::intEnum>(IntEnum::FOO),
            EnumUnion());  // IntEnum intEnum = IntEnum.FOO
}

TEST_F(AidlTest, StructuredParcelableEquality) {
  // TODO: break up equality tests, these are hard to read, because you need to
  // keep the state of the parcelables in mind
  StructuredParcelable parcelable1;
  StructuredParcelable parcelable2;

  parcelable1.f = 11;
  parcelable2.f = 11;

  service->FillOutStructuredParcelable(&parcelable1);
  service->FillOutStructuredParcelable(&parcelable2);

  sp<INamedCallback> callback1;
  sp<INamedCallback> callback2;
  service->GetOtherTestService(String16("callback1"), &callback1);
  service->GetOtherTestService(String16("callback2"), &callback2);

  parcelable1.ibinder = IInterface::asBinder(callback1);
  parcelable2.ibinder = IInterface::asBinder(callback1);

  EXPECT_EQ(parcelable1, parcelable2);

  parcelable1.f = 0;
  EXPECT_LT(parcelable1, parcelable2);
  parcelable1.f = 11;

  parcelable1.shouldBeJerry = "Jarry";
  EXPECT_LT(parcelable1, parcelable2);
  parcelable1.shouldBeJerry = "Jerry";

  parcelable2.shouldContainThreeFs = {};
  EXPECT_GT(parcelable1, parcelable2);
  parcelable2.shouldContainThreeFs = {parcelable2.f, parcelable2.f, parcelable2.f};

  parcelable2.shouldBeIntBar = IntEnum::FOO;
  EXPECT_GT(parcelable1, parcelable2);
  parcelable2.shouldBeIntBar = IntEnum::BAR;

  parcelable2.ibinder = IInterface::asBinder(callback2);
  EXPECT_NE(parcelable1, parcelable2);
}

TEST_F(AidlTest, ConfirmStructuredParcelables) {
  constexpr int kDesiredValue = 23;

  StructuredParcelable parcelable;
  parcelable.f = kDesiredValue;

  EXPECT_EQ(parcelable.stringDefaultsToFoo, String16("foo"));
  EXPECT_EQ(parcelable.byteDefaultsToFour, 4);
  EXPECT_EQ(parcelable.intDefaultsToFive, 5);
  EXPECT_EQ(parcelable.longDefaultsToNegativeSeven, -7);
  EXPECT_EQ(parcelable.booleanDefaultsToTrue, true);
  EXPECT_EQ(parcelable.charDefaultsToC, 'C');
  EXPECT_TRUE(parcelable.floatDefaultsToPi == 3.14f) << parcelable.floatDefaultsToPi;
  EXPECT_TRUE(parcelable.doubleWithDefault == -3.14e17) << parcelable.doubleWithDefault;

  EXPECT_EQ(parcelable.boolDefault, false);
  EXPECT_EQ(parcelable.byteDefault, 0);
  EXPECT_EQ(parcelable.intDefault, 0);
  EXPECT_EQ(parcelable.longDefault, 0);
  EXPECT_EQ(parcelable.floatDefault, 0.0f);
  EXPECT_EQ(parcelable.doubleDefault, 0.0);

  ASSERT_EQ(parcelable.arrayDefaultsTo123.size(), 3u);
  EXPECT_EQ(parcelable.arrayDefaultsTo123[0], 1);
  EXPECT_EQ(parcelable.arrayDefaultsTo123[1], 2);
  EXPECT_EQ(parcelable.arrayDefaultsTo123[2], 3);
  EXPECT_TRUE(parcelable.arrayDefaultsToEmpty.empty());

  EXPECT_EQ(parcelable.defaultWithFoo, IntEnum::FOO);

  service->FillOutStructuredParcelable(&parcelable);

  ASSERT_EQ(parcelable.shouldContainThreeFs.size(), 3u);
  EXPECT_EQ(parcelable.shouldContainThreeFs[0], kDesiredValue);
  EXPECT_EQ(parcelable.shouldContainThreeFs[1], kDesiredValue);
  EXPECT_EQ(parcelable.shouldContainThreeFs[2], kDesiredValue);

  EXPECT_EQ(parcelable.shouldBeJerry, "Jerry");
  EXPECT_EQ(parcelable.int32_min, INT32_MIN);
  EXPECT_EQ(parcelable.int32_max, INT32_MAX);
  EXPECT_EQ(parcelable.int64_max, INT64_MAX);
  EXPECT_EQ(parcelable.hexInt32_neg_1, -1);

  for (size_t ndx = 0; ndx < parcelable.int8_1.size(); ndx++) {
    EXPECT_EQ(parcelable.int8_1[ndx], 1) << ndx;
  }

  for (size_t ndx = 0; ndx < parcelable.int32_1.size(); ndx++) {
    EXPECT_EQ(parcelable.int32_1[ndx], 1) << ndx;
  }

  for (size_t ndx = 0; ndx < parcelable.int64_1.size(); ndx++) {
    EXPECT_EQ(parcelable.int64_1[ndx], 1) << ndx;
  }

  EXPECT_EQ(parcelable.hexInt32_pos_1, 1);
  EXPECT_EQ(parcelable.hexInt64_pos_1, 1);

  EXPECT_EQ(static_cast<int>(parcelable.const_exprs_1), 1);
  EXPECT_EQ(static_cast<int>(parcelable.const_exprs_2), 1);
  EXPECT_EQ(static_cast<int>(parcelable.const_exprs_3), 1);
  EXPECT_EQ(static_cast<int>(parcelable.const_exprs_4), 1);
  EXPECT_EQ(static_cast<int>(parcelable.const_exprs_5), 1);
  EXPECT_EQ(static_cast<int>(parcelable.const_exprs_6), 1);
  EXPECT_EQ(static_cast<int>(parcelable.const_exprs_7), 1);
  EXPECT_EQ(static_cast<int>(parcelable.const_exprs_8), 1);
  EXPECT_EQ(static_cast<int>(parcelable.const_exprs_9), 1);
  EXPECT_EQ(static_cast<int>(parcelable.const_exprs_10), 1);

  EXPECT_EQ(parcelable.addString1, "hello world!");
  EXPECT_EQ(parcelable.addString2, "The quick brown fox jumps over the lazy dog.");

  EXPECT_EQ(StructuredParcelable::BIT0 | StructuredParcelable::BIT2,
            parcelable.shouldSetBit0AndBit2);

  EXPECT_EQ(parcelable.u->get<Union::ns>(), vector<int32_t>({1, 2, 3}));
  EXPECT_EQ(parcelable.shouldBeConstS1->get<Union::s>(), Union::S1());
}

TEST_F(AidlTest, EmptyParcelableHolder) {
  using namespace android::aidl::tests::extension;
  android::Parcel parcel;
  {
    ExtendableParcelable ep;
    ep.writeToParcel(&parcel);
    std::shared_ptr<MyExt> emptyExt;
    ep.ext.getParcelable(&emptyExt);
    EXPECT_FALSE(emptyExt);
  }
  {
    parcel.setDataPosition(0);
    ExtendableParcelable ep;
    ep.readFromParcel(&parcel);
    std::shared_ptr<MyExt> emptyExt;
    ep.ext.getParcelable(&emptyExt);
    EXPECT_FALSE(emptyExt);
  }
}

TEST_F(AidlTest, ParcelableHolderEqualityOperator) {
  auto ph1 = android::os::ParcelableHolder(android::Parcelable::Stability::STABILITY_LOCAL);
  auto ph2 = android::os::ParcelableHolder(android::Parcelable::Stability::STABILITY_LOCAL);
  auto ph3 = android::os::ParcelableHolder(android::Parcelable::Stability::STABILITY_LOCAL);
  auto ptr1 = std::make_shared<MyExt>();
  auto ptr2 = std::make_shared<MyExt>();
  ptr1->a = 1;
  ptr1->b = "a";
  ptr2->a = 1;
  ptr2->b = "a";

  ph1.setParcelable(ptr1);
  ph2.setParcelable(ptr1);
  ph3.setParcelable(ptr2);

  // ParcelableHolder always uses its address as a comparison criterion.
  EXPECT_TRUE(ph1 != ph2);
  EXPECT_TRUE(ph2 != ph3);
  EXPECT_TRUE(ph1 == ph1);
  EXPECT_TRUE(ph2 == ph2);
  EXPECT_TRUE(ph3 == ph3);

  android::Parcel parcel;
  ph1.writeToParcel(&parcel);
  ph2.writeToParcel(&parcel);
  ph3.writeToParcel(&parcel);
  parcel.setDataPosition(0);

  ph1.readFromParcel(&parcel);
  ph2.readFromParcel(&parcel);
  ph3.readFromParcel(&parcel);

  // ParcelableHolder always uses its address as a comparison criterion.
  EXPECT_TRUE(ph1 != ph2);
  EXPECT_TRUE(ph2 != ph3);
  EXPECT_TRUE(ph1 == ph1);
  EXPECT_TRUE(ph2 == ph2);
  EXPECT_TRUE(ph3 == ph3);
}

TEST_F(AidlTest, NativeExtednableParcelable) {
  using namespace android::aidl::tests::extension;
  MyExt ext;
  ext.a = 42;
  ext.b = "EXT";

  MyExt2 ext2;
  ext2.a = 42;
  ext2.b.a = 24;
  ext2.b.b = "INEXT";
  ext2.c = "EXT2";
  android::Parcel parcel;
  {
    ExtendableParcelable ep;
    ep.a = 1;
    ep.b = "a";
    ep.c = 42L;

    EXPECT_TRUE(ep.ext.setParcelable(ext) == android::OK);
    EXPECT_TRUE(ep.ext2.setParcelable(ext2) == android::OK);

    std::shared_ptr<MyExtLike> extLike;
    ep.ext.getParcelable(&extLike);
    EXPECT_FALSE(extLike) << "The extension type must be MyExt, so it has to fail even though "
                             "MyExtLike has the same structure as MyExt.";

    std::shared_ptr<MyExt> actualExt;
    ep.ext.getParcelable(&actualExt);
    std::shared_ptr<MyExt2> actualExt2;
    ep.ext2.getParcelable(&actualExt2);

    EXPECT_TRUE(actualExt);
    EXPECT_TRUE(actualExt2);

    EXPECT_EQ(ext, *actualExt);
    EXPECT_EQ(ext2, *actualExt2);

    ep.writeToParcel(&parcel);
  }

  parcel.setDataPosition(0);
  {
    ExtendableParcelable ep;
    ep.readFromParcel(&parcel);

    std::shared_ptr<MyExtLike> extLike;
    ep.ext.getParcelable(&extLike);
    EXPECT_FALSE(extLike) << "The extension type must be MyExt, so it has to fail even though "
                             "MyExtLike has the same structure as MyExt.";

    std::shared_ptr<MyExt> actualExt;
    ep.ext.getParcelable(&actualExt);
    std::shared_ptr<MyExt2> actualExt2;
    ep.ext2.getParcelable(&actualExt2);

    std::shared_ptr<MyExt> emptyExt;
    ep.ext2.getParcelable(&emptyExt);
    EXPECT_FALSE(emptyExt);

    EXPECT_TRUE(actualExt);
    EXPECT_TRUE(actualExt2);

    EXPECT_EQ(ext, *actualExt);
    EXPECT_EQ(ext2, *actualExt2);
  }
}

TEST_F(AidlTest, ParcelableToString) {
  ParcelableForToString p;
  p.intValue = 10;
  p.intArray = {20, 30};
  p.longValue = 100L;
  p.longArray = {200L, 300L};
  p.doubleValue = 3.14;
  p.doubleArray = {1.1, 1.2};
  p.floatValue = 3.14f;
  p.floatArray = {1.1f, 1.2f};
  p.byteValue = 3;
  p.byteArray = {5, 6};
  p.booleanValue = true;
  p.booleanArray = {true, false};
  p.stringValue = String16("this is a string");
  p.stringArray = {String16("hello"), String16("world")};
  p.stringList = {String16("alice"), String16("bob")};
  OtherParcelableForToString op;
  op.field = String16("other");
  p.parcelableValue = op;
  p.parcelableArray = {op, op};
  p.enumValue = IntEnum::FOO;
  p.enumArray = {IntEnum::FOO, IntEnum::BAR};
  // p.nullArray = null;
  // p.nullList = null;
  GenericStructuredParcelable<int32_t, StructuredParcelable, IntEnum> gen;
  gen.a = 1;
  gen.b = 2;
  p.parcelableGeneric = gen;
  p.unionValue = Union(std::vector<std::string>{"union", "value"});

  const string expected =
      "ParcelableForToString{"
      "intValue: 10, "
      "intArray: [20, 30], "
      "longValue: 100, "
      "longArray: [200, 300], "
      "doubleValue: 3.140000, "
      "doubleArray: [1.100000, 1.200000], "
      "floatValue: 3.140000, "
      "floatArray: [1.100000, 1.200000], "
      "byteValue: 3, "
      "byteArray: [5, 6], "
      "booleanValue: true, "
      "booleanArray: [true, false], "
      "stringValue: this is a string, "
      "stringArray: [hello, world], "
      "stringList: [alice, bob], "
      "parcelableValue: OtherParcelableForToString{field: other}, "
      "parcelableArray: ["
      "OtherParcelableForToString{field: other}, "
      "OtherParcelableForToString{field: other}], "
      "enumValue: FOO, "
      "enumArray: [FOO, BAR], "
      "nullArray: [], "
      "nullList: [], "
      "parcelableGeneric: GenericStructuredParcelable{a: 1, b: 2}, "
      "unionValue: Union{ss: [union, value]}"
      "}";

  EXPECT_EQ(expected, p.toString());
}

TEST_F(AidlTest, ReverseRecursiveList) {
  std::unique_ptr<RecursiveList> head;
  for (int i = 0; i < 10; i++) {
    auto node = std::make_unique<RecursiveList>();
    node->value = i;
    node->next = std::move(head);
    head = std::move(node);
  }
  // head: [9, 8, ... 0]

  RecursiveList reversed;
  auto status = service->ReverseList(*head, &reversed);
  ASSERT_TRUE(status.isOk()) << status.toString8();

  // reversed should be [0, 1, .. 9]
  RecursiveList* cur = &reversed;
  for (int i = 0; i < 10; i++) {
    EXPECT_EQ(i, cur->value);
    cur = cur->next.get();
  }
  EXPECT_EQ(nullptr, cur);
}

TEST_F(AidlTest, GetUnionTags) {
  std::vector<Union> unions;
  std::vector<Union::Tag> tags;
  // test empty
  auto status = service->GetUnionTags(unions, &tags);
  ASSERT_TRUE(status.isOk()) << status.toString8();
  EXPECT_EQ(tags, (std::vector<Union::Tag>{}));
  // test non-empty
  unions.push_back(Union::make<Union::n>());
  unions.push_back(Union::make<Union::ns>());
  status = service->GetUnionTags(unions, &tags);
  ASSERT_TRUE(status.isOk()) << status.toString8();
  EXPECT_EQ(tags, (std::vector<Union::Tag>{Union::n, Union::ns}));
}

TEST_F(AidlTest, FixedSizeArray) {
  android::Parcel parcel;

  FixedSizeArrayExample p;
  p.byteMatrix[0][0] = 0;
  p.byteMatrix[0][1] = 1;
  p.byteMatrix[1][0] = 2;
  p.byteMatrix[1][1] = 3;
  p.floatMatrix[0][0] = 0.f;
  p.floatMatrix[0][1] = 1.f;
  p.floatMatrix[1][0] = 2.f;
  p.floatMatrix[1][1] = 3.f;
  EXPECT_EQ(OK, p.writeToParcel(&parcel));

  parcel.setDataPosition(0);

  FixedSizeArrayExample q;
  EXPECT_EQ(OK, q.readFromParcel(&parcel));
  EXPECT_EQ(p, q);
}

TEST_F(AidlTest, FixedSizeArrayWithValuesAtNullableFields) {
  android::Parcel parcel;

  FixedSizeArrayExample p;
  p.boolNullableArray = std::array<bool, 2>{true, false};
  p.byteNullableArray = std::array<uint8_t, 2>{42, 0};
  p.stringNullableArray = std::array<std::optional<std::string>, 2>{"hello", "world"};

  p.boolNullableMatrix.emplace();
  p.boolNullableMatrix->at(0) = std::array<bool, 2>{true, false};
  p.byteNullableMatrix.emplace();
  p.byteNullableMatrix->at(0) = std::array<uint8_t, 2>{42, 0};
  p.stringNullableMatrix.emplace();
  p.stringNullableMatrix->at(0) = std::array<std::optional<std::string>, 2>{"hello", "world"};

  EXPECT_EQ(OK, p.writeToParcel(&parcel));

  parcel.setDataPosition(0);

  FixedSizeArrayExample q;
  EXPECT_EQ(OK, q.readFromParcel(&parcel));
  EXPECT_EQ(p, q);
}

TEST_F(AidlTest, FixedSizeArrayOfBytesShouldBePacked) {
  android::Parcel parcel;

  std::array<std::array<uint8_t, 3>, 2> byte_array;
  byte_array[0] = {1, 2, 3};
  byte_array[1] = {4, 5, 6};
  EXPECT_EQ(OK, parcel.writeFixedArray(byte_array));

  parcel.setDataPosition(0);

  int32_t len;
  EXPECT_EQ(OK, parcel.readInt32(&len));
  EXPECT_EQ(2, len);
  std::vector<uint8_t> byte_vector;
  EXPECT_EQ(OK, parcel.readByteVector(&byte_vector));
  EXPECT_EQ(byte_vector, (std::vector<uint8_t>{1, 2, 3}));
  EXPECT_EQ(OK, parcel.readByteVector(&byte_vector));
  EXPECT_EQ(byte_vector, (std::vector<uint8_t>{4, 5, 6}));
}

template <typename Service, typename MemFn, typename Input>
void CheckRepeat(Service service, MemFn fn, Input input) {
  Input out1, out2;
  EXPECT_TRUE(std::invoke(fn, service, input, &out1, &out2).isOk());
  EXPECT_EQ(input, out1);
  EXPECT_EQ(input, out2);
}

template <typename T>
std::array<std::array<T, 3>, 2> Make2dArray(std::initializer_list<T> values) {
  std::array<std::array<T, 3>, 2> arr = {};
  auto it = std::begin(values);
  for (auto& row : arr) {
    for (auto& el : row) {
      if (it == std::end(values)) break;
      el = *it++;
    }
  }
  return arr;
}

TEST_F(AidlTest, FixedSizeArrayOverBinder) {
  sp<IRepeatFixedSizeArray> service =
      android::waitForService<IRepeatFixedSizeArray>(IRepeatFixedSizeArray::descriptor);
  ASSERT_NE(service, nullptr);

  CheckRepeat(service, &IRepeatFixedSizeArray::RepeatBytes, (std::array<uint8_t, 3>{1, 2, 3}));

  CheckRepeat(service, &IRepeatFixedSizeArray::RepeatInts, (std::array<int32_t, 3>{1, 2, 3}));

  sp<IBinder> binder1 = new BBinder();
  sp<IBinder> binder2 = new BBinder();
  sp<IBinder> binder3 = new BBinder();
  CheckRepeat(service, &IRepeatFixedSizeArray::RepeatBinders,
              (std::array<sp<IBinder>, 3>{binder1, binder2, binder3}));

  IntParcelable p1, p2, p3;
  p1.value = 1;
  p2.value = 2;
  p3.value = 3;
  CheckRepeat(service, &IRepeatFixedSizeArray::RepeatParcelables,
              (std::array<IntParcelable, 3>{p1, p2, p3}));

  CheckRepeat(service, &IRepeatFixedSizeArray::Repeat2dBytes, Make2dArray<uint8_t>({1, 2, 3}));

  CheckRepeat(service, &IRepeatFixedSizeArray::Repeat2dInts, Make2dArray<int32_t>({1, 2, 3}));

  // Not-nullable
  CheckRepeat(service, &IRepeatFixedSizeArray::Repeat2dBinders,
              Make2dArray<sp<IBinder>>({binder1, binder2, binder3, binder1, binder2, binder3}));

  CheckRepeat(service, &IRepeatFixedSizeArray::Repeat2dParcelables,
              Make2dArray<IntParcelable>({p1, p2, p3}));
}
