/*
 * Copyright (c) 2016, 2020, Pelion and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "gtest/gtest.h"
#include "test_ns_nvm_helper.h"

class nsnvmhelper_test : public testing::Test {
protected:
    void SetUp()
    {
    }

    void TearDown()
    {
    }
};

TEST_F(nsnvmhelper_test, test_ns_nvm_helper_write)
{
    ASSERT_EQ(true, test_ns_nvm_helper_write());
}

TEST_F(nsnvmhelper_test, test_ns_nvm_helper_delete)
{
    ASSERT_EQ(true, test_ns_nvm_helper_write());
    ASSERT_EQ(true, test_ns_nvm_helper_delete());
}

TEST_F(nsnvmhelper_test, test_ns_nvm_helper_read)
{
    ASSERT_EQ(true, test_ns_nvm_helper_write());
    ASSERT_EQ(true, test_ns_nvm_helper_read());
}

TEST_F(nsnvmhelper_test, test_ns_nvm_helper_concurrent_requests)
{
    ASSERT_EQ(true, test_ns_nvm_helper_write());
    ASSERT_EQ(true, test_ns_nvm_helper_concurrent_requests());
}

TEST_F(nsnvmhelper_test, test_ns_nvm_helper_platform_error)
{
    ASSERT_EQ(true, test_ns_nvm_helper_write());
    ASSERT_EQ(true, test_ns_nvm_helper_platform_error());
}

TEST_F(nsnvmhelper_test, test_ns_nvm_helper_platform_error_in_write)
{
    ASSERT_EQ(true, test_ns_nvm_helper_write());
    ASSERT_EQ(true, test_ns_nvm_helper_platform_error_in_write());
}


