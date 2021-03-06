// Copyright (c) 2009 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Tests of CancellationFlag class.

#include "base/cancellation_flag.h"

#include "base/logging.h"
#include "base/spin_wait.h"
#include "base/time.h"
#include "base/thread.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/platform_test.h"

using base::CancellationFlag;
using base::TimeDelta;
using base::Thread;

namespace {

//------------------------------------------------------------------------------
// Define our test class.
//------------------------------------------------------------------------------

class CancelTask : public Task {
 public:
  explicit CancelTask(CancellationFlag* flag) : flag_(flag) {}
  virtual void Run() {
    ASSERT_DEBUG_DEATH(flag_->Set(), "");
  }
 private:
  CancellationFlag* flag_;
};

TEST(CancellationFlagTest, SimpleSingleThreadedTest) {
  CancellationFlag flag;
  ASSERT_FALSE(flag.IsSet());
  flag.Set();
  ASSERT_TRUE(flag.IsSet());
}

TEST(CancellationFlagTest, DoubleSetTest) {
  CancellationFlag flag;
  ASSERT_FALSE(flag.IsSet());
  flag.Set();
  ASSERT_TRUE(flag.IsSet());
  flag.Set();
  ASSERT_TRUE(flag.IsSet());
}

TEST(CancellationFlagTest, SetOnDifferentThreadDeathTest) {
  // Checks that Set() can't be called from any other thread.
  // CancellationFlag should die on a DCHECK if Set() is called from
  // other thread.
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  Thread t("CancellationFlagTest.SetOnDifferentThreadDeathTest");
  ASSERT_TRUE(t.Start());
  ASSERT_TRUE(t.message_loop());
  ASSERT_TRUE(t.IsRunning());

  CancellationFlag flag;
  t.message_loop()->PostTask(FROM_HERE, new CancelTask(&flag));
}

}  // namespace
