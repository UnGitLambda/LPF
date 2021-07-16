
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
#include <lpf/bsplib.h>
#include "Test.h"

void spmd( lpf_t lpf, lpf_pid_t pid, lpf_pid_t nprocs, lpf_args_t args)
{
    (void) args; // ignore any arguments passed through call to lpf_exec

    bsplib_err_t rc = BSPLIB_SUCCESS;
    bsplib_t bsplib;
    rc = bsplib_create( lpf, pid, nprocs, 1, 1, &bsplib);
    EXPECT_EQ( "%d", BSPLIB_SUCCESS, rc );
 
    char a = 'a';
    char b = 'b';
    rc = bsplib_push_reg( bsplib, &a, sizeof( a ) );
    EXPECT_EQ( "%d", BSPLIB_SUCCESS, rc );
    rc = bsplib_sync( bsplib  );
    EXPECT_EQ( "%d", BSPLIB_SUCCESS, rc );

    rc =bsplib_put( bsplib, 0, &b, &a, 1, sizeof( a ) );
    EXPECT_EQ( "%d", BSPLIB_ERR_MEMORY_ACCESS_OUT_OF_RANGE, rc );
    rc =bsplib_put( bsplib, 0, &b, &a, 0, sizeof( a ) * 2 );
    EXPECT_EQ( "%d", BSPLIB_ERR_MEMORY_ACCESS_OUT_OF_RANGE, rc );

    rc = bsplib_put( bsplib, 
            bsplib_nprocs( bsplib ), &b, &a, 0, sizeof( a ) );
    EXPECT_EQ( "%d", BSPLIB_ERR_PID_OUT_OF_RANGE, rc );

    rc = bsplib_destroy( bsplib);
    EXPECT_EQ( "%d", BSPLIB_SUCCESS, rc );
}

/** 
 * \test Tests some common illegal cases with lpf_put. 
 * \pre P >= 2
 * \return Exit code: 0
 */
TEST( func_bsplib_put_exceptions )
{
    lpf_err_t rc = lpf_exec( LPF_ROOT, LPF_MAX_P, spmd, LPF_NO_ARGS);
    EXPECT_EQ( "%d", LPF_SUCCESS, rc );
    return 0;
}

