/** ============================================================================
 *  @file   ldrv_smm.h
 *
 *  @path   $(DSPLINK)/gpp/src/ldrv/SMM/
 *
 *  @desc   Defines interfaces exposed by the LDRV_SMM (Shared Memory Manager)
 *          sub-component.
 *          The SMM component provides services for management of shared memory
 *          within the system.
 *          These services are used by the LDRV sub-components to allocate and
 *          free their shared memory requirements.
 *
 *  @ver    1.64
 *  ============================================================================
 *  Copyright (C) 2002-2009, Texas Instruments Incorporated -
 *  http://www.ti.com/
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  *  Neither the name of Texas Instruments Incorporated nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *  ============================================================================
 */


#if !defined (LDRV_SMM_H)
#define LDRV_SMM_H


/*  ----------------------------------- DSP/BIOS Link               */
#include <dsplink.h>


#if defined (__cplusplus)
EXTERN "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  @func   LDRV_SMM_moduleInit
 *
 *  @desc   This function initializes the LDRV_SMM module.
 *
 *  @arg    None.
 *
 *  @ret    DSP_SOK
 *              Operation completed successfully.
 *          DSP_ECONFIG
 *              Incorrect configuration.
 *          DSP_EMEMORY
 *              Out of memory
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    LDRV_SMM_moduleExit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_SMM_moduleInit (Void) ;


/** ============================================================================
 *  @func   LDRV_SMM_moduleExit
 *
 *  @desc   This function finalizes the LDRV_SMM module.
 *
 *  @arg    None.
 *
 *  @ret    DSP_SOK
 *              Operation completed successfully.
 *          DSP_ECONFIG
 *              Incorrect configuration.
 *          DSP_EMEMORY
 *              Out of memory
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  None.
 *
 *  @leave  None.
 *
 *  @see    LDRV_SMM_moduleInit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_SMM_moduleExit (Void) ;


/** ============================================================================
 *  @func   LDRV_SMM_init
 *
 *  @desc   This function initializes the Shared Memory Manger (SMM) component.
 *
 *  @arg    dspId
 *              ID of the DSP for which the SMM component is to be initialized.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be valid.
 *
 *  @leave  None.
 *
 *  @see    LDRV_SMM_exit ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_SMM_init (IN ProcessorId dspId) ;


/** ============================================================================
 *  @func   LDRV_SMM_exit
 *
 *  @desc   This function finalizes the Shared Memory Manger (SMM) component.
 *
 *  @arg    dspId
 *              ID of the DSP for which the SMM component is to be finalized.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be valid.
 *
 *  @leave  None.
 *
 *  @see    LDRV_SMM_init ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_SMM_exit (IN ProcessorId dspId) ;


/** ============================================================================
 *  @func   LDRV_SMM_alloc
 *
 *  @desc   This function allocates a chunk of memory of the requested size from
 *          the specified shared memory region.
 *
 *  @arg    dspId
 *              ID of the DSP with which the memory region is shared.
 *  @arg    memEntryId
 *              ID of the memory entry from which memory is to be allocated.
 *  @arg    physAddr
 *              Location to receive the physical address of the allocated memory
 *              chunk. If NULL, the address is not returned.
 *  @arg    dspVirtAddr
 *              Location to receive the DSP virtual address of the allocated
 *              memory chunk. If NULL, the address is not returned.
 *  @arg    gppVirtAddr
 *              Location to receive the GPP kernel virtual address of the
 *              allocated memory chunk. If NULL, the address is not returned.
 *  @arg    size
 *              Size of the memory chunk to be allocated.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EVALUE
 *              Memory entry ID does not correspond to a shared region.
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be valid.
 *
 *  @leave  None.
 *
 *  @see    LDRV_SMM_free ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_SMM_alloc (IN  ProcessorId dspId,
                IN  Uint32      memEntryId,
                OUT Uint32 *    physAddr,
                OUT Uint32 *    dspVirtAddr,
                OUT Uint32 *    gppVirtAddr,
                IN  Uint32      size) ;


/** ============================================================================
 *  @func   LDRV_SMM_free
 *
 *  @desc   This function frees the chunk of memory of the requested size into
 *          the specified shared memory region.
 *
 *  @arg    dspId
 *              ID of the DSP with which the memory region is shared.
 *  @arg    memEntryId
 *              ID of the memory entry into which memory is to be freed.
 *  @arg    size
 *              Size of the memory chunk to be freed.
 *
 *  @ret    DSP_SOK
 *              Operation successfully completed.
 *          DSP_EVALUE
 *              Memory entry ID does not correspond to a shared region.
 *          DSP_EMEMORY
 *              Operation failed due to a memory error.
 *          DSP_EFAIL
 *              General failure.
 *
 *  @enter  dspId must be valid.
 *
 *  @leave  None.
 *
 *  @see    LDRV_SMM_alloc ()
 *  ============================================================================
 */
NORMAL_API
DSP_STATUS
LDRV_SMM_free (IN  ProcessorId dspId,
               IN  Uint32      memEntryId,
               IN  Uint32      size) ;


#if defined (DDSP_DEBUG)
/** ============================================================================
 *  @func   LDRV_SMM_debug
 *
 *  @desc   Prints the current status of this subcomponent.
 *
 *  @arg    dspId
 *              Processor Identifier.
 *
 *  @ret    None.
 *
 *  @enter  dspId shall be valid.
 *
 *  @leave  None.
 *
 *  @see    None.
 *  ============================================================================
 */
NORMAL_API
Void
LDRV_SMM_debug (IN  ProcessorId  dspId) ;
#endif /* defined (DDSP_DEBUG) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (LDRV_SMM_H) */
