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

#include "tests/bad_parcelable.h"

#include <android-base/stringprintf.h>
#include <binder/Parcel.h>
#include <utils/String8.h>

using android::base::StringPrintf;

namespace android {
namespace aidl {
namespace tests {

BadParcelable::BadParcelable(bool bad, const std::string& name, int32_t number)
    : bad_(bad), name_(name.c_str(), name.length()), number_(number) {}

status_t BadParcelable::writeToParcel(Parcel* parcel) const {
  if (auto status = parcel->writeBool(bad_); status != OK) return status;
  if (auto status = parcel->writeString16(name_); status != OK) return status;
  if (auto status = parcel->writeInt32(number_); status != OK) return status;
  // BAD! write superfluous data
  if (bad_) parcel->writeInt32(42);
  return OK;
}

status_t BadParcelable::readFromParcel(const Parcel* parcel) {
  if (auto status = parcel->readBool(&bad_); status != OK) return status;
  if (auto status = parcel->readString16(&name_); status != OK) return status;
  if (auto status = parcel->readInt32(&number_); status != OK) return status;
  return OK;
}

std::string BadParcelable::toString() const {
  return StringPrintf("BadParcelable{bad=%d,name=%s,number=%d}", bad_, String8(name_).string(),
                      number_);
}

}  // namespace tests
}  // namespace aidl
}  // namespace android
