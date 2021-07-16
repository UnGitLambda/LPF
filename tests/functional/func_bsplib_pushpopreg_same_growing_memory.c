
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
    rc = bsplib_create( lpf, pid, nprocs, 1, (size_t) -1, &bsplib);
    EXPECT_EQ( "%d", BSPLIB_SUCCESS, rc );

    // first register a bit of memory
    char memory[10];
    memset( memory, 0, sizeof( memory ) );
    rc = bsplib_push_reg( bsplib, &memory[0], 3 );
    EXPECT_EQ( "%d", BSPLIB_SUCCESS, rc );

    rc = bsplib_sync( bsplib );
    EXPECT_EQ( "%d", BSPLIB_SUCCESS, rc );

    char x = 'x';

    rc = bsplib_put( bsplib, 
            ( bsplib_pid( bsplib ) + 1 ) % bsplib_nprocs( bsplib  ), 
            &x, memory, 0, sizeof( x ) );
    EXPECT_EQ( "%d", BSPLIB_SUCCESS, rc );
    EXPECT_EQ( "%c", '\0', memory[0] );
    EXPECT_EQ( "%c", 'x', x );

    rc = bsplib_sync( bsplib );
    EXPECT_EQ( "%d", BSPLIB_SUCCESS, rc );

    EXPECT_EQ( "%d", 'x', memory[0] );
    EXPECT_EQ( "%d", 'x', x );

    EXPECT_EQ( "%d", '\0', memory[4] );
        
    rc = bsplib_put( bsplib, 
            ( bsplib_pid( bsplib ) + 1 ) % bsplib_nprocs( bsplib  ), 
            &x, memory, 4, sizeof( x ) );
    EXPECT_EQ( "%d", BSPLIB_ERR_MEMORY_ACCESS_OUT_OF_RANGE, rc );
    EXPECT_EQ( "%c", '\0', memory[4] );

    // now register the memory again, but with larger extent
    rc = bsplib_push_reg( bsplib, &memory[0], 5 );
    EXPECT_EQ( "%d", BSPLIB_SUCCESS, rc );
    rc = bsplib_sync( bsplib );
    EXPECT_EQ( "%d", BSPLIB_SUCCESS, rc );

    rc = bsplib_put( bsplib, 
            ( bsplib_pid( bsplib ) + 1 ) % bsplib_nprocs( bsplib ), 
            &x, memory, 4, sizeof( x ) );
    EXPECT_EQ( "%d", BSPLIB_SUCCESS, rc );
    EXPECT_EQ( "%c", '\0', memory[4] );
    EXPECT_EQ( "%c", 'x', x );

    rc = bsplib_pop_reg( bsplib, &memory[0] );
    EXPECT_EQ( "%d", BSPLIB_SUCCESS, rc );
    rc = bsplib_pop_reg( bsplib, &memory[0] );
    EXPECT_EQ( "%d", BSPLIB_SUCCESS, rc );

    rc = bsplib_sync( bsplib );
    EXPECT_EQ( "%d", BSPLIB_SUCCESS, rc );
    EXPECT_EQ( "%c", 'x', memory[4] );
    EXPECT_EQ( "%c", 'x', x );

    rc = bsplib_destroy( bsplib);
    EXPECT_EQ( "%d", BSPLIB_SUCCESS, rc );
}

/** 
 * \test Test stack like behaviour of bsp_push_reg to confirm that last registration takes precedence.
 * \pre P >= 1
 * \return Exit code: 0
 */
TEST( func_bsplib_pushpopreg_same_growing_memory)
{
    lpf_err_t rc = lpf_exec( LPF_ROOT, LPF_MAX_P, spmd, LPF_NO_ARGS);
    EXPECT_EQ( "%d", LPF_SUCCESS, rc );
    return 0;
}

