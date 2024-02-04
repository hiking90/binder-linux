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

#include <binder/IInterface.h>

using android::IInterface;
using android::OK;
using android::String16;

TEST_F(AidlTest, Ping) {
  EXPECT_EQ(OK, IInterface::asBinder(service)->pingBinder());
}

TEST_F(AidlTest, GetInterfaceDescriptor) {
  EXPECT_EQ(String16("android.aidl.tests.ITestService"),
            IInterface::asBinder(service)->getInterfaceDescriptor());
}
