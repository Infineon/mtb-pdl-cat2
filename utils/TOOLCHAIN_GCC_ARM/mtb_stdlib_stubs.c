/***********************************************************************************************//**
 * \file mtb_stdlib_stubs.c
 *
 * \brief
 * Implementation of the stubs for the standard library functions for the GCC_ARM compiler.
 *
 ***************************************************************************************************
 * \copyright
 * Copyright 2018-2025 Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation
 *
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
 **************************************************************************************************/
#if defined(__GNUC__) && !defined(__ARMCC_VERSION)

#include <errno.h>
#include <sys/stat.h>
#include "cy_utils.h"
#include "cmsis_compiler.h"

CY_MISRA_DEVIATE_BLOCK_START('MISRA C-2012 Directive 4.6', 8, \
                             'Directive requires use of typedef input instead of int but signature for following 8 APIs needs to match stlib one');

CY_MISRA_DEVIATE_BLOCK_START('MISRA C-2012 Rule 8.13', 3, \
                             'Rule requires const attributes for inputs for following 3 APIs but signature needs to match stlib one');
//--------------------------------------------------------------------------------------------------
// _read
//--------------------------------------------------------------------------------------------------
__WEAK int _read(int file, char* ptr, int len);
//--------------------------------------------------------------------------------------------------
// _read
//--------------------------------------------------------------------------------------------------
__WEAK int _read(int file, char* ptr, int len)
{
    CY_UNUSED_PARAMETER(file);
    CY_UNUSED_PARAMETER(ptr);
    CY_UNUSED_PARAMETER(len);
    errno = ENOSYS;
    // If we reach this point, something went wrong
    CY_ASSERT(false);
    return -1;
}


//--------------------------------------------------------------------------------------------------
// _write
//--------------------------------------------------------------------------------------------------
__WEAK int _write(int file, char* ptr, int len);
//--------------------------------------------------------------------------------------------------
// _write
//--------------------------------------------------------------------------------------------------
__WEAK int _write(int file, char* ptr, int len)
{
    CY_UNUSED_PARAMETER(file);
    CY_UNUSED_PARAMETER(ptr);
    CY_UNUSED_PARAMETER(len);
    errno = ENOSYS;
    // If we reach this point, something went wrong
    CY_ASSERT(false);
    return -1;
}


//--------------------------------------------------------------------------------------------------
// _fstat
//--------------------------------------------------------------------------------------------------
__WEAK int _fstat(int fildes, struct stat* st);

//--------------------------------------------------------------------------------------------------
// _fstat
//--------------------------------------------------------------------------------------------------
__WEAK int _fstat(int fildes, struct stat* st)
{
    CY_UNUSED_PARAMETER(fildes);
    CY_UNUSED_PARAMETER(st);
    errno = ENOSYS;
    //We don't want to assert at this point because willing or unwillingly
    //this function gets called but in a no-op way so we need to let it pass
    return -1;
}


CY_MISRA_BLOCK_END('MISRA C-2012 Rule 8.13');

//--------------------------------------------------------------------------------------------------
// _close
//--------------------------------------------------------------------------------------------------
__WEAK int _close(int fildes);
//--------------------------------------------------------------------------------------------------
// _close
//--------------------------------------------------------------------------------------------------
__WEAK int _close(int fildes)
{
    CY_UNUSED_PARAMETER(fildes);
    errno = ENOSYS;
    // If we reach this point, something went wrong
    CY_ASSERT(false);
    return -1;
}


//--------------------------------------------------------------------------------------------------
// _lseek
//--------------------------------------------------------------------------------------------------
__WEAK int _lseek(int file, int ptr, int dir);
//--------------------------------------------------------------------------------------------------
// _lseek
//--------------------------------------------------------------------------------------------------
__WEAK int _lseek(int file, int ptr, int dir)
{
    CY_UNUSED_PARAMETER(file);
    CY_UNUSED_PARAMETER(ptr);
    CY_UNUSED_PARAMETER(dir);
    errno = ENOSYS;
    // If we reach this point, something went wrong
    CY_ASSERT(false);
    return -1;
}


//--------------------------------------------------------------------------------------------------
// _getpid
//--------------------------------------------------------------------------------------------------
__WEAK int _getpid(void);
//--------------------------------------------------------------------------------------------------
// _getpid
//--------------------------------------------------------------------------------------------------
__WEAK int _getpid(void)
{
    errno = ENOSYS;
    // If we reach this point, something went wrong
    CY_ASSERT(false);
    return -1;
}


//--------------------------------------------------------------------------------------------------
// _isatty
//--------------------------------------------------------------------------------------------------
__WEAK int _isatty(int file);
//--------------------------------------------------------------------------------------------------
// _isatty
//--------------------------------------------------------------------------------------------------
__WEAK int _isatty(int file)
{
    CY_UNUSED_PARAMETER(file);
    errno = ENOSYS;
    // If we reach this point, something went wrong
    CY_ASSERT(false);
    return -1;
}


//--------------------------------------------------------------------------------------------------
// _kill
//--------------------------------------------------------------------------------------------------
__WEAK int _kill(int pid, int sig);
//--------------------------------------------------------------------------------------------------
// _kill
//--------------------------------------------------------------------------------------------------
__WEAK int _kill(int pid, int sig)
{
    CY_UNUSED_PARAMETER(pid);
    CY_UNUSED_PARAMETER(sig);
    errno = ENOSYS;
    // If we reach this point, something went wrong
    CY_ASSERT(false);
    return -1;
}


CY_MISRA_BLOCK_END('MISRA C-2012 Directive 4.6');

#endif // defined(__GNUC__) && !defined(__ARMCC_VERSION)
