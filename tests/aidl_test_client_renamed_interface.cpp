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

#include "aidl_test_client.h"
#include "gmock/gmock.h"

#include <android/aidl/tests/INewName.h>
#include <android/aidl/tests/IOldName.h>

using android::IInterface;
using android::String16;
using android::aidl::tests::INewName;
using android::aidl::tests::IOldName;
using testing::Eq;

class RenamedInterfaceTest : public AidlTest {
 public:
  void SetUp() override {
    AidlTest::SetUp();

    ASSERT_TRUE(service->GetOldNameInterface(&oldName).isOk());
    ASSERT_TRUE(service->GetNewNameInterface(&newName).isOk());
  }

  sp<IOldName> oldName;
  sp<INewName> newName;
};

TEST_F(RenamedInterfaceTest, oldAsOld) {
  ASSERT_THAT(String16("android.aidl.tests.IOldName"), oldName->getInterfaceDescriptor());
  String16 realName;
  ASSERT_TRUE(oldName->RealName(&realName).isOk());
  ASSERT_THAT(String16("OldName"), realName);
}

TEST_F(RenamedInterfaceTest, newAsNew) {
  ASSERT_THAT(String16("android.aidl.tests.IOldName"), newName->getInterfaceDescriptor());
  String16 realName;
  ASSERT_TRUE(newName->RealName(&realName).isOk());
  ASSERT_THAT(String16("NewName"), realName);
}

TEST_F(RenamedInterfaceTest, oldAsNew) {
  sp<INewName> oldAsNew = INewName::asInterface(IInterface::asBinder(oldName));
  ASSERT_THAT(String16("android.aidl.tests.IOldName"), oldAsNew->getInterfaceDescriptor());
  String16 realName;
  ASSERT_TRUE(oldAsNew->RealName(&realName).isOk());
  ASSERT_THAT(String16("OldName"), realName);
}

TEST_F(RenamedInterfaceTest, newAsOld) {
  sp<IOldName> newAsOld = IOldName::asInterface(IInterface::asBinder(newName));
  ASSERT_THAT(String16("android.aidl.tests.IOldName"), newAsOld->getInterfaceDescriptor());
  String16 realName;
  ASSERT_TRUE(newAsOld->RealName(&realName).isOk());
  ASSERT_THAT(String16("NewName"), realName);
}
