
/*
 *   Copyright 2021 Huawei Technologies Co., Ltd.
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

#include <lpf/core.h>
#include <limits.h>

#include "Test.h"

void spmd( lpf_t lpf, lpf_pid_t pid, lpf_pid_t nprocs, lpf_args_t args)
{
    (void) args; // ignore args parameter
    lpf_err_t rc = LPF_SUCCESS;

    EXPECT_EQ("%d", 2, nprocs);
        
    size_t maxMsgs = 1 , maxRegs = 2;
    rc = lpf_resize_message_queue( lpf, maxMsgs);
    EXPECT_EQ( "%d", LPF_SUCCESS, rc );
    rc = lpf_resize_memory_register( lpf, maxRegs );
    EXPECT_EQ( "%d", LPF_SUCCESS, rc );
    rc = lpf_sync( lpf, LPF_SYNC_DEFAULT );
    EXPECT_EQ( "%d", LPF_SUCCESS, rc );
 
    size_t huge = (size_t) 10 + INT_MAX; // if this overflows, it means that
                                         // size_t fits in 'int' and so this
                                         // test is pointless 
    int *x = calloc( huge , sizeof(int));
    int *y = calloc( huge, sizeof(int));

    EXPECT_NE( "%p", (int *) NULL, x );
    EXPECT_NE( "%p", (int *) NULL, y );
    
    size_t i;
    for (i = 0; i <huge; ++i)
    {
        x[i] = (int) (i % INT_MAX );
    }


    lpf_memslot_t xslot = LPF_INVALID_MEMSLOT;
    lpf_memslot_t yslot = LPF_INVALID_MEMSLOT;
    rc = lpf_register_local( lpf, x, sizeof(int)*huge, &xslot );
    EXPECT_EQ( "%d", LPF_SUCCESS, rc );
    rc = lpf_register_global( lpf, y, sizeof(int)*huge, &yslot );
    EXPECT_EQ( "%d", LPF_SUCCESS, rc );


    rc = lpf_sync( lpf, LPF_SYNC_DEFAULT);
    EXPECT_EQ( "%d", LPF_SUCCESS, rc );

    if ( pid == 1) {
        rc = lpf_put( lpf, xslot, 0, 0, yslot, 0, sizeof(int)*huge, LPF_MSG_DEFAULT );
        EXPECT_EQ( "%d", LPF_SUCCESS, rc );
    }

    rc = lpf_sync( lpf, LPF_SYNC_DEFAULT );
    EXPECT_EQ( "%d", LPF_SUCCESS, rc );


    if ( pid == 0) {
        for (i = 0; i <huge; ++i)
        {
            EXPECT_EQ( "%d", x[i], y[i] );
        }
    }
    else {
        for (i = 0; i <huge; ++i)
        {
            EXPECT_EQ( "%d", 0, y[i] );
        }
    }


    rc = lpf_deregister( lpf, xslot );
    EXPECT_EQ( "%d", LPF_SUCCESS, rc );
    rc = lpf_deregister( lpf, yslot );
    EXPECT_EQ( "%d", LPF_SUCCESS, rc );
}

/** 
 * \test Test lpf_put by sending a message larger than INT_MAX
 * \pre P >= 2
 * \pre P <= 2
 * \return Exit code: 0
 */
TEST( func_lpf_put_parallel_huge )
{
    lpf_err_t rc = lpf_exec( LPF_ROOT, LPF_MAX_P, spmd, LPF_NO_ARGS);
    EXPECT_EQ("%d", LPF_SUCCESS, rc );
    return 0;
}
