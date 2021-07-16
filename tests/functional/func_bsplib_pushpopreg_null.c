
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
    rc = bsplib_create( lpf, pid, nprocs, 1, 0, &bsplib);
    EXPECT_EQ( "%d", BSPLIB_SUCCESS, rc );

    // register NULL once
    rc = bsplib_push_reg( bsplib, NULL, 0 );
    EXPECT_EQ( "%d", BSPLIB_SUCCESS, rc );

    rc = bsplib_sync( bsplib );
    EXPECT_EQ( "%d", BSPLIB_SUCCESS, rc );

    rc = bsplib_pop_reg( bsplib, NULL );
    EXPECT_EQ( "%d", BSPLIB_SUCCESS, rc );

    rc = bsplib_sync( bsplib );
    EXPECT_EQ( "%d", BSPLIB_SUCCESS, rc );


    // register NULL twice
    rc = bsplib_push_reg( bsplib, NULL, 0 );
    EXPECT_EQ( "%d", BSPLIB_SUCCESS, rc );
    rc = bsplib_push_reg( bsplib, NULL, 0 );
    EXPECT_EQ( "%d", BSPLIB_SUCCESS, rc );

    rc = bsplib_sync( bsplib );
    EXPECT_EQ( "%d", BSPLIB_SUCCESS, rc );

    rc = bsplib_pop_reg( bsplib, NULL );
    EXPECT_EQ( "%d", BSPLIB_SUCCESS, rc );
    rc = bsplib_pop_reg( bsplib, NULL );
    EXPECT_EQ( "%d", BSPLIB_SUCCESS, rc );
 
    rc = bsplib_sync( bsplib );
    EXPECT_EQ( "%d", BSPLIB_SUCCESS, rc );

    // use of NULL with comm primitives 
 
    char x = 'x';
    char y = 'y';
    char *p = bsplib_pid(bsplib) == 0 ? &x : NULL;

    rc = bsplib_push_reg(bsplib, p, bsplib_pid(bsplib) == 0 ? 1 : 0 );
    EXPECT_EQ( "%d", BSPLIB_SUCCESS, rc );

    rc = bsplib_sync(bsplib);
    EXPECT_EQ( "%d", BSPLIB_SUCCESS, rc );

    if ( bsplib_pid(bsplib) == 1 )
    {
       rc = bsplib_put(bsplib, 0, &y, p, 0, 1 );
       EXPECT_EQ( "%d", BSPLIB_ERR_NULL_POINTER , rc );
            
       rc = bsplib_hpput(bsplib, 0, &y, p, 0, 1 );
       EXPECT_EQ( "%d", BSPLIB_ERR_NULL_POINTER , rc );
            
       rc = bsplib_get(bsplib, 0, p, 0, &y, 1 );
       EXPECT_EQ( "%d", BSPLIB_ERR_NULL_POINTER , rc );

       rc = bsplib_hpget(bsplib, 0, p, 0, &y, 1 );
       EXPECT_EQ( "%d", BSPLIB_ERR_NULL_POINTER , rc );
    }

    if ( bsplib_pid(bsplib) == 0 )
    {
        EXPECT_EQ( "%c", 'x', *p );
        rc = bsplib_put(bsplib, 0, &y, p, 0, 1 );
        EXPECT_EQ( "%d", BSPLIB_SUCCESS, rc );
    }
    rc = bsplib_sync(bsplib);
    EXPECT_EQ( "%d", BSPLIB_SUCCESS, rc );
    if ( bsplib_pid(bsplib) == 0 )
    {
        EXPECT_EQ( "%c", 'y', *p );
    }

    rc = bsplib_destroy( bsplib);
    EXPECT_EQ( "%d", BSPLIB_SUCCESS, rc );
}

/** 
 * \test Tests BSPlib's handling of register the NULL address
 * \pre P >= 2
 * \return Exit code: 0
 */
TEST( func_bsplib_pushpopreg_null )
{
    lpf_err_t rc = lpf_exec( LPF_ROOT, LPF_MAX_P, spmd, LPF_NO_ARGS);
    EXPECT_EQ( "%d", LPF_SUCCESS, rc );
    return 0;
}

