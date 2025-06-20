/***************************************************************************//**
* \file cy_scb_spi.c
* \version 4.70
*
* Provides SPI API implementation of the SCB driver.
*
********************************************************************************
* \copyright
* (c) (2016-2025), Cypress Semiconductor Corporation (an Infineon company) or
* an affiliate of Cypress Semiconductor Corporation.
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
*******************************************************************************/

#include "cy_scb_spi.h"

#if defined (CY_IP_M0S8SCB) || defined (CY_IP_MXSCB)

#if defined(__cplusplus)
extern "C" {
#endif

/* Static functions */
static void HandleTransmit(CySCB_Type *base, cy_stc_scb_spi_context_t *context);
static void HandleReceive (CySCB_Type *base, cy_stc_scb_spi_context_t *context);
static void DiscardArrayNoCheck(CySCB_Type const *base, uint32_t size);

/*******************************************************************************
* Function Name: Cy_SCB_SPI_Init
****************************************************************************//**
*
* Initializes the SCB for SPI operation.
*
* \param base
* The pointer to the SPI SCB instance.
*
* \param config
* The pointer to the configuration structure \ref cy_stc_scb_spi_config_t.
*
* \param context
* The pointer to the context structure \ref cy_stc_scb_spi_context_t allocated
* by the user. The structure is used during the SPI operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
* If only SPI \ref group_scb_spi_ll will be used pass NULL as pointer to
* context.
*
* \return
* \ref cy_en_scb_spi_status_t
*
* \note
* Ensure that the SCB block is disabled before calling this function.
*
*******************************************************************************/
cy_en_scb_spi_status_t Cy_SCB_SPI_Init(CySCB_Type *base, cy_stc_scb_spi_config_t const *config, cy_stc_scb_spi_context_t *context)
{
    cy_en_scb_spi_status_t status;

    /* Input parameters verification */
    if ((NULL == base) || (NULL == config))
    {
        status = CY_SCB_SPI_BAD_PARAM;
    }
    else
    {
        CY_ASSERT_L3(CY_SCB_SPI_IS_MODE_VALID     (config->spiMode));
        CY_ASSERT_L3(CY_SCB_SPI_IS_SUB_MODE_VALID (config->subMode));
        CY_ASSERT_L3(CY_SCB_SPI_IS_SCLK_MODE_VALID(config->sclkMode));

        CY_ASSERT_L2(CY_SCB_SPI_IS_OVERSAMPLE_VALID (config->oversample, config->spiMode));
        CY_ASSERT_L2(CY_SCB_SPI_IS_SS_POLARITY_VALID(config->ssPolarity));
        CY_ASSERT_L2(CY_SCB_SPI_IS_DATA_WIDTH_VALID (config->rxDataWidth));
        CY_ASSERT_L2(CY_SCB_SPI_IS_DATA_WIDTH_VALID (config->txDataWidth));
        CY_ASSERT_L2(CY_SCB_SPI_IS_BOTH_DATA_WIDTH_VALID(config->subMode, config->rxDataWidth, config->txDataWidth));

        CY_ASSERT_L2(CY_SCB_IS_INTR_VALID(config->rxFifoIntEnableMask, CY_SCB_SPI_RX_INTR_MASK));
        CY_ASSERT_L2(CY_SCB_IS_INTR_VALID(config->txFifoIntEnableMask, CY_SCB_SPI_TX_INTR_MASK));
        CY_ASSERT_L2(CY_SCB_IS_INTR_VALID(config->masterSlaveIntEnableMask, CY_SCB_SPI_MASTER_SLAVE_INTR_MASK));

        uint32_t locSclkMode = CY_SCB_SPI_GetSclkMode(config->subMode, config->sclkMode);

        /* Configure an SPI interface */
        SCB_CTRL(base) = _BOOL2FLD(SCB_CTRL_EC_AM_MODE, config->enableWakeFromSleep) |
                         _VAL2FLD(SCB_CTRL_OVS, (config->oversample - 1UL))          |
                         _VAL2FLD(SCB_CTRL_MODE, CY_SCB_CTRL_MODE_SPI);

        SCB_SPI_CTRL(base) = _BOOL2FLD(SCB_SPI_CTRL_SELECT_PRECEDE,  (CY_SCB_SPI_TI_PRECEDES == config->subMode)) |
                             _BOOL2FLD(SCB_SPI_CTRL_LATE_MISO_SAMPLE, config->enableMisoLateSample)               |
                             _BOOL2FLD(SCB_SPI_CTRL_SCLK_CONTINUOUS,  config->enableFreeRunSclk)                  |
                             _BOOL2FLD(SCB_SPI_CTRL_MASTER_MODE,     (CY_SCB_SPI_MASTER == config->spiMode))      |
                             _VAL2FLD(CY_SCB_SPI_CTRL_CLK_MODE,      locSclkMode)                                 |
                             _VAL2FLD(CY_SCB_SPI_CTRL_SSEL_POLARITY, config->ssPolarity)                          |
                             _VAL2FLD(SCB_SPI_CTRL_MODE,  (uint32_t) config->subMode);

#ifdef CY_IP_M0S8SCB
        bool byteMode = (config->rxDataWidth <= CY_SCB_BYTE_WIDTH) && (config->txDataWidth <= CY_SCB_BYTE_WIDTH);

        SCB_CTRL(base) |= _BOOL2FLD(SCB_CTRL_BYTE_MODE, byteMode);

        SCB_SPI_CTRL(base) |= _BOOL2FLD(SCB_SPI_CTRL_CONTINUOUS, (!config->enableTransferSeperation));
#else /* CY_IP_MXSCB */
        bool memWidth = (config->rxDataWidth > CY_SCB_BYTE_WIDTH) || (config->txDataWidth > CY_SCB_BYTE_WIDTH);

        /* Configure an SPI interface */
        CY_REG32_CLR_SET(SCB_CTRL(base), SCB_CTRL_MEM_WIDTH, memWidth);

        SCB_SPI_CTRL(base) |= _BOOL2FLD(SCB_SPI_CTRL_SSEL_CONTINUOUS, (!config->enableTransferSeperation)) |
                              _BOOL2FLD(SCB_SPI_CTRL_SSEL_SETUP_DEL, config->ssSetupDelay)                 |
                              _BOOL2FLD(SCB_SPI_CTRL_SSEL_HOLD_DEL, config->ssHoldDelay)                   |
                              _BOOL2FLD(SCB_SPI_CTRL_SSEL_INTER_FRAME_DEL, config->ssInterDataframeDelay);

        CY_ASSERT_L2(CY_SCB_SPI_IS_PARITY_VALID(config->parity));

        /* Configure the RX direction */
        SCB_SPI_RX_CTRL(base) = _VAL2FLD(CY_SCB_SPI_RX_CTRL_SET_PARITY, (uint32_t) config->parity) |
                                _BOOL2FLD(SCB_SPI_RX_CTRL_DROP_ON_PARITY_ERROR, config->dropOnParityError);

        /* Configure the TX direction */
        SCB_SPI_TX_CTRL(base) = _VAL2FLD(CY_SCB_SPI_TX_CTRL_SET_PARITY, (uint32_t) config->parity) |
                                _BOOL2FLD(SCB_SPI_TX_CTRL_MOSI_IDLE_HIGH, config->mosiIdleHigh);
#endif /* CY_IP_M0S8SCB */

        /* Configure SCB_CTRL.BYTE_MODE/SCB_CTRL.MEM_WIDTH then verify levels */
        CY_ASSERT_L2(CY_SCB_IS_TRIGGER_LEVEL_VALID(base, config->rxFifoTriggerLevel));
        CY_ASSERT_L2(CY_SCB_IS_TRIGGER_LEVEL_VALID(base, config->txFifoTriggerLevel));

        /* Configure the RX direction */
        SCB_RX_CTRL(base) = _BOOL2FLD(SCB_RX_CTRL_MSB_FIRST, config->enableMsbFirst) |
                            _BOOL2FLD(SCB_RX_CTRL_MEDIAN, config->enableInputFilter) |
                            _VAL2FLD(SCB_RX_CTRL_DATA_WIDTH, (config->rxDataWidth - 1UL));

        SCB_RX_FIFO_CTRL(base) = _VAL2FLD(SCB_RX_FIFO_CTRL_TRIGGER_LEVEL, config->rxFifoTriggerLevel);

        /* Configure the TX direction */
        SCB_TX_CTRL(base) = _BOOL2FLD(SCB_TX_CTRL_MSB_FIRST, config->enableMsbFirst) |
                            _VAL2FLD(SCB_TX_CTRL_DATA_WIDTH, (config->txDataWidth - 1UL));

        SCB_TX_FIFO_CTRL(base) = _VAL2FLD(SCB_TX_FIFO_CTRL_TRIGGER_LEVEL, config->txFifoTriggerLevel);

        /* Set up interrupt sources */
        SCB_INTR_RX_MASK(base) = (config->rxFifoIntEnableMask & CY_SCB_SPI_RX_INTR_MASK);
        SCB_INTR_TX_MASK(base) = (config->txFifoIntEnableMask & CY_SCB_SPI_TX_INTR_MASK);
        SCB_INTR_M_MASK(base)  = (config->masterSlaveIntEnableMask & CY_SCB_SPI_MASTER_DONE);
        SCB_INTR_S_MASK(base)  = (config->masterSlaveIntEnableMask & CY_SCB_SPI_SLAVE_ERR);
        SCB_INTR_SPI_EC_MASK(base) = 0UL;

        /* Initialize the context */
        if (NULL != context)
        {
            context->status    = 0UL;

            context->txBufIdx  = 0UL;
            context->rxBufIdx  = 0UL;

            context->cbEvents = NULL;
            context->txDefaultValue = CY_SCB_SPI_DEFAULT_TX;

        #ifdef CY_IP_M0S8SCB
            context->cbDeepSleep = NULL;
        #endif /* CY_IP_M0S8SCB */

        #ifdef CY_IP_MXSCB
            context->parityEnable = (config->parity != CY_SCB_SPI_PARITY_NONE) ? true : false;
        #endif /* CY_IP_MXSCB */

        #if !defined(NDEBUG)
            /* Put an initialization key into the initKey variable to verify
            * context initialization in the transfer API.
            */
            context->initKey = CY_SCB_SPI_INIT_KEY;
        #endif /* !(NDEBUG) */
        }
        status = CY_SCB_SPI_SUCCESS;
    }

    return status;
}


/*******************************************************************************
* Function Name: Cy_SCB_SPI_DeInit
****************************************************************************//**
*
* De-initializes the SCB block; returns the register values to default.
*
* \param base
* The pointer to the SPI SCB instance.
*
* \note
* Ensure that the SCB block is disabled before calling this function.
*
*******************************************************************************/
void Cy_SCB_SPI_DeInit(CySCB_Type *base)
{
    /* SPI interface */
    SCB_CTRL(base)         = CY_SCB_CTRL_DEF_VAL;
    SCB_SPI_CTRL(base)     = CY_SCB_SPI_CTRL_DEF_VAL;

#ifdef CY_IP_MXSCB
    /* RX and TX direction */
    SCB_SPI_RX_CTRL(base)  = 0UL;
    SCB_SPI_TX_CTRL(base)  = 0UL;
#endif /* CY_IP_MXSCB */

    /* RX direction */
    SCB_RX_CTRL(base)      = CY_SCB_RX_CTRL_DEF_VAL;
    SCB_RX_FIFO_CTRL(base) = 0UL;

    /* TX direction */
    SCB_TX_CTRL(base)      = CY_SCB_TX_CTRL_DEF_VAL;
    SCB_TX_FIFO_CTRL(base) = 0UL;

    /* Disable all interrupt sources */
    SCB_INTR_SPI_EC_MASK(base) = 0UL;
    SCB_INTR_I2C_EC_MASK(base) = 0UL;
    SCB_INTR_RX_MASK(base)     = 0UL;
    SCB_INTR_TX_MASK(base)     = 0UL;
    SCB_INTR_M_MASK(base)      = 0UL;
    SCB_INTR_S_MASK(base)      = 0UL;
}


/*******************************************************************************
* Function Name: Cy_SCB_SPI_Disable
****************************************************************************//**
*
* Disables the SCB block, clears context statuses, and disables
* TX and RX interrupt sources.
* Note that after the block is disabled, the TX and RX FIFOs and
* hardware statuses are cleared. Also, the hardware stops driving the output
* and ignores the input. Refer to section \ref group_scb_spi_lp for more
* information about SPI pins when SCB disabled.
*
* \param base
* The pointer to the SPI SCB instance.
*
* \param context
* The pointer to the context structure \ref cy_stc_scb_spi_context_t allocated
* by the user. The structure is used during the SPI operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
* If only SPI functions that do not require context will be used to pass NULL
* as pointer to context.
*
* \note
* Calling this function when the SPI is busy (master preforms data transfer or
* slave communicates with the master) may cause transfer corruption because the
* hardware stops driving the outputs and ignores the inputs.
* Ensure that the SPI is not busy before calling this function.
*
*******************************************************************************/
void Cy_SCB_SPI_Disable(CySCB_Type *base, cy_stc_scb_spi_context_t *context)
{
    SCB_CTRL(base) &= (uint32_t) ~SCB_CTRL_ENABLED_Msk;

    if (NULL != context)
    {
        context->status    = 0UL;

        context->rxBufIdx  = 0UL;
        context->txBufIdx  = 0UL;
    }

    /* Disable RX and TX interrupt sources for the slave because
    * RX overflow and TX underflow are kept enabled after the 1st call of
    * Cy_SCB_SPI_Transfer().
    */
    if (!_FLD2BOOL(SCB_SPI_CTRL_MASTER_MODE, SCB_SPI_CTRL(base)))
    {
        Cy_SCB_SetRxInterruptMask(base, CY_SCB_CLEAR_ALL_INTR_SRC);
        Cy_SCB_SetTxInterruptMask(base, CY_SCB_CLEAR_ALL_INTR_SRC);
    }
}


/*******************************************************************************
* Function Name: Cy_SCB_SPI_DeepSleepCallback
****************************************************************************//**
*
* This function handles the transition of the SCB SPI into and out of
* Deep Sleep mode. It prevents the device from entering Deep Sleep mode
* if the SPI slave or master is actively communicating, or there is any data
* in the TX or RX FIFOs.
* The following behavior of the SPI SCB depends on whether the SCB block is
* configured as enable-wakeup or not:
* * <b>enable-wakeup</b>: any transfer intended to the slave wakes up
*   the device from Deep Sleep mode. The slave responds with 0xFF to the
*   transfer and incoming data is ignored.
*   If the transfer occurs before the device enters Deep Sleep mode, the device
*   will not enter Deep Sleep mode and incoming data is stored in the RX FIFO.
*   The SCB clock is disabled before entering Deep Sleep and enabled after the
*   device exits Deep Sleep mode. The SCB clock disabling may lead to
*   corrupted data in the RX FIFO. Clear the RX FIFO after this callback is
*   executed.
*   Note that for proper SPI operation after Deep Sleep the source of
*   HFCLK must be stable, this includes the PLL.
*   Only the SPI slave can be configured to be a wakeup source from Deep Sleep
*   mode.
* * <b>Not enable-wakeup</b>: the SPI is disabled. It is enabled when
*   the device fails to enter Deep Sleep mode or it is awakened from Deep Sleep
*   mode. While the SPI is disabled, it stops driving the outputs and ignores
*   the inputs. Any incoming data is ignored.
*   Refer to section \ref group_scb_spi_lp for more information about SPI pins
*   when SCB disabled.
*
* This function must be called during execution of \ref Cy_SysPm_CpuEnterDeepSleep.
* To do it, register this function as a callback before calling
* \ref Cy_SysPm_CpuEnterDeepSleep : specify \ref CY_SYSPM_DEEPSLEEP as the callback
* type and call \ref Cy_SysPm_RegisterCallback.
*
* \param callbackParams
* The pointer to the callback parameters structure
* \ref cy_stc_syspm_callback_params_t.
*
* \param mode
* Callback mode, see \ref cy_en_syspm_callback_mode_t
*
* \return
* \ref cy_en_syspm_status_t
*
* \note
*   For proper operation, when the SPI slave is configured to be a wakeup source
*   from Deep Sleep mode, need to register callback function by \ref
*   Cy_SCB_SPI_RegisterDSClockConfig. Callback function must disable or
*   enable the clock divider depending on the event \ref
*   group_scb_spi_macros_deep_sleep_callback_events.
*   Not applicable for PSOC 4100S Max and PSOC 4000T.
*
*******************************************************************************/
cy_en_syspm_status_t Cy_SCB_SPI_DeepSleepCallback(cy_stc_syspm_callback_params_t *callbackParams, cy_en_syspm_callback_mode_t mode)
{
    CySCB_Type *locBase = (CySCB_Type *) callbackParams->base;
    cy_stc_scb_spi_context_t *locContext = (cy_stc_scb_spi_context_t *) callbackParams->context;

    cy_en_syspm_status_t retStatus = CY_SYSPM_FAIL;

    switch(mode)
    {
        case CY_SYSPM_CHECK_READY:
        {
            /* Check whether the High-level API is not busy executing the transfer
            * operation.
            */
            if (0UL == (CY_SCB_SPI_TRANSFER_ACTIVE & Cy_SCB_SPI_GetTransferStatus(locBase, locContext)))
            {
                /* If the SPI bus is not busy, all data elements are transferred
                * on the bus from the TX FIFO and shifter and the RX FIFOs are
                * empty - the SPI is ready to enter Deep Sleep mode.
                */
                if (!Cy_SCB_SPI_IsBusBusy(locBase))
                {
                    if (Cy_SCB_SPI_IsTxComplete(locBase))
                    {
                        if (0UL == Cy_SCB_SPI_GetNumInRxFifo(locBase))
                        {
                            if (_FLD2BOOL(SCB_CTRL_EC_AM_MODE, SCB_CTRL(locBase)))
                            {
                                /* The SCB is configured as enable-wakeup: clear the SPI
                                * wakeup interrupt source because it triggers
                                * during Active mode communication and make
                                * sure that a new transfer is not started after
                                * clearing.
                                */
                                Cy_SCB_ClearSpiInterrupt(locBase, CY_SCB_SPI_INTR_WAKEUP);

                                if (!Cy_SCB_SPI_IsBusBusy(locBase))
                                {
                                    retStatus = CY_SYSPM_SUCCESS;
                                }
                            }
                            else
                            {
                                /* The SCB is NOT configured as enable-wakeup: disable the
                                * SPI. The master and slave stop driving the
                                * bus until the SPI is enabled. This happens
                                * when the device fails to enter Deep Sleep
                                * mode or it is awakened from Deep Sleep mode.
                                */
                                Cy_SCB_SPI_Disable(locBase, locContext);

                                retStatus = CY_SYSPM_SUCCESS;
                            }
                        }
                    }
                }
            }
        }
        break;

        case CY_SYSPM_CHECK_FAIL:
        {
            /* The other driver is not ready for Deep Sleep mode. Restore
            * Active mode configuration.
            */

            if (!_FLD2BOOL(SCB_CTRL_EC_AM_MODE, SCB_CTRL(locBase)))
            {
                /* The SCB is NOT configured as enable-wakeup: enable the SPI to operate */
                Cy_SCB_SPI_Enable(locBase);
            }

            retStatus = CY_SYSPM_SUCCESS;
        }
        break;

        case CY_SYSPM_BEFORE_TRANSITION:
        {
            /* This code executes inside the critical section and enabling the
            * active interrupt source makes the interrupt pending in the NVIC.
            * However, the interrupt processing is delayed until the code exits
            * the critical section. The pending interrupt force WFI instruction
            * does nothing and the device remains in Active mode.
            */

            if (_FLD2BOOL(SCB_CTRL_EC_AM_MODE, SCB_CTRL(locBase)))
            {
                /* The SCB is configured as enable-wakeup: enable the SPI wakeup interrupt
                * source. If any transaction happens, the wakeup interrupt
                * becomes pending and prevents entering Deep Sleep mode.
                */
                Cy_SCB_SetSpiInterruptMask(locBase, CY_SCB_I2C_INTR_WAKEUP);

#ifdef CY_IP_M0S8SCB
                if (NULL != locContext->cbDeepSleep)
                {
                    locContext->cbDeepSleep(CY_SCB_SPI_SLAVE_DEEP_SLEEP_ENTER_EVENT);
                }
#else /* CY_IP_MXSCB */
                /* Disable SCB clock */
                SCB_CTRL(locBase) |= (uint32_t) SCB_CTRL_EC_ACCESS_Msk;
#endif /* CY_IP_M0S8SCB */
            }

            retStatus = CY_SYSPM_SUCCESS;
        }
        break;

        case CY_SYSPM_AFTER_TRANSITION:
        {
            if (_FLD2BOOL(SCB_CTRL_EC_AM_MODE, SCB_CTRL(locBase)))
            {
#ifdef CY_IP_M0S8SCB
                if (NULL != locContext->cbDeepSleep)
                {
                    locContext->cbDeepSleep(CY_SCB_SPI_SLAVE_DEEP_SLEEP_EXIT_EVENT);
                }
#else /* CY_IP_MXSCB */
                /* Enable SCB clock */
                SCB_CTRL(locBase) &= (uint32_t) ~SCB_CTRL_EC_ACCESS_Msk;
#endif /* CY_IP_M0S8SCB */
                /* The SCB is configured as enable-wakeup: disable the SPI wakeup interrupt
                * source
                */
                Cy_SCB_SetSpiInterruptMask(locBase, CY_SCB_CLEAR_ALL_INTR_SRC);
            }
            else
            {
                /* The SCB is NOT configured as enable-wakeup: enable the SPI to operate */
                Cy_SCB_SPI_Enable(locBase);
            }

            retStatus = CY_SYSPM_SUCCESS;
        }
        break;

        default:
            /* Unknown SysPm mode */
        break;
    }

    return (retStatus);
}


/************************* High-Level Functions ********************************
* The following functions are considered high-level. They provide the layer of
* intelligence to the SCB. These functions require interrupts.
* Low-level and high-level functions must not be mixed because low-level API
* can adversely affect the operation of high-level functions.
*******************************************************************************/


/*******************************************************************************
* Function Name: Cy_SCB_SPI_Transfer
****************************************************************************//**
*
* This function starts an SPI transfer operation.
* It configures transmit and receive buffers for an SPI transfer.
* If the data that will be received is not important, pass NULL as rxBuffer.
* If the data that will be transmitted is not important, pass NULL as txBuffer
* and then the default TX value is sent out as each data element.
* To define the default TX value, specify txDefVal by \ref Cy_SCB_SPI_SetTxDefaultValue
* and verify the default TX value by \ref Cy_SCB_SPI_GetTxDefaultValue.
* Note that passing NULL as rxBuffer and txBuffer are considered invalid cases.
*
* After the function configures TX and RX interrupt sources, it returns and
* \ref Cy_SCB_SPI_Interrupt manages further data transfer.
*
* * In the master mode, the transfer operation starts after calling this
*   function
* * In the slave mode, the transfer registers and will start when
*   the master request arrives.
*
* When the transfer operation is completed (requested number of data elements
* sent and received), the \ref CY_SCB_SPI_TRANSFER_ACTIVE status is cleared
* and the \ref CY_SCB_SPI_TRANSFER_CMPLT_EVENT event is generated.
*
* \param base
* The pointer to the SPI SCB instance.
*
* \param txBuffer
* The pointer of the buffer with data to transmit.
* The element size is defined by the data type that depends on the configured
* TX data width.
*
* \param rxBuffer
* The pointer to the buffer to store received data.
* The element size is defined by the data type that depends on the configured
* RX data width.
*
* \param size
* The number of data elements to transmit and receive.
*
* \param context
* The pointer to the context structure \ref cy_stc_scb_spi_context_t allocated
* by the user. The structure is used during the SPI operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \return
* \ref cy_en_scb_spi_status_t
*
* \note
* * The buffers must not be modified and must stay allocated until the end of the
*   transfer.
* * This function overrides all RX and TX FIFO interrupt sources and changes
*   the RX and TX FIFO level.
* * This function clears RX FIFO buffer before start of the transfer.
*
*******************************************************************************/
cy_en_scb_spi_status_t Cy_SCB_SPI_Transfer(CySCB_Type *base, void *txBuffer, void *rxBuffer, uint32_t size,
                                           cy_stc_scb_spi_context_t *context)
{
    CY_ASSERT_L1(NULL != context);
    CY_ASSERT_L1(CY_SCB_SPI_INIT_KEY == context->initKey);
    CY_ASSERT_L1(CY_SCB_SPI_IS_BUFFER_VALID(txBuffer, rxBuffer, size));

    cy_en_scb_spi_status_t retStatus = CY_SCB_SPI_TRANSFER_BUSY;

    /* Check whether there are no active transfer requests */
    if (0UL == (CY_SCB_SPI_TRANSFER_ACTIVE & context->status))
    {
        uint32_t fifoSize = Cy_SCB_GetFifoSize(base);

        /* Set up the context */
        context->status    = CY_SCB_SPI_TRANSFER_ACTIVE;

        context->txBuf     = txBuffer;
        context->txBufSize = size;
        context->txBufIdx =  0UL;

        context->rxBuf     = rxBuffer;
        context->rxBufSize = size;
        context->rxBufIdx  = 0UL;

        /* Clear RX Fifo buffer */
        Cy_SCB_SPI_ClearRxFifo(base);

        /* Set the TX interrupt when half of FIFO was transmitted */
        Cy_SCB_SetTxFifoLevel(base, fifoSize / 2UL);

        if (_FLD2BOOL(SCB_SPI_CTRL_MASTER_MODE, SCB_SPI_CTRL(base)))
        {
            /* Trigger an RX interrupt:
            * - If the transfer size is equal to or less than FIFO, trigger at the end of the transfer.
            * - If the transfer size is greater than FIFO, trigger 1 byte earlier than the TX interrupt.
            */
            Cy_SCB_SetRxFifoLevel(base, (size > fifoSize) ? ((fifoSize / 2UL) - 2UL) : (size - 1UL));

            Cy_SCB_SetMasterInterruptMask(base, CY_SCB_CLEAR_ALL_INTR_SRC);

            /* Enable interrupt sources to perform a transfer */
#ifdef CY_IP_M0S8SCB
            Cy_SCB_SetRxInterruptMask(base, CY_SCB_RX_INTR_LEVEL);
#else /* CY_IP_MXSCB */
            if (context->parityEnable == true)
            {
                Cy_SCB_SetRxInterruptMask(base, CY_SCB_RX_INTR_LEVEL | CY_SCB_RX_INTR_SPI_PARITY_ERROR);
            }
            else
            {
                Cy_SCB_SetRxInterruptMask(base, CY_SCB_RX_INTR_LEVEL);
            }
#endif /* CY_IP_M0S8SCB */
            Cy_SCB_SetTxInterruptMask(base, CY_SCB_TX_INTR_LEVEL);
        }
        else
        {
            /* Trigger an RX interrupt:
            * - If the transfer size is equal to or less than half of FIFO, trigger at the end of the transfer.
            * - If the transfer size is greater than half of FIFO, trigger 1 byte earlier than a TX interrupt.
            */
            Cy_SCB_SetRxFifoLevel(base, (size > (fifoSize / 2UL)) ? ((fifoSize / 2UL) - 2UL) : (size - 1UL));

            Cy_SCB_SetSlaveInterruptMask(base, CY_SCB_SLAVE_INTR_SPI_BUS_ERROR);

            /* Enable interrupt sources to perform a transfer */
#ifdef CY_IP_M0S8SCB
            Cy_SCB_SetRxInterruptMask(base, CY_SCB_RX_INTR_LEVEL | CY_SCB_RX_INTR_OVERFLOW);
#else /* CY_IP_MXSCB */
            if (context->parityEnable == true)
            {
                Cy_SCB_SetRxInterruptMask(base, CY_SCB_RX_INTR_LEVEL | CY_SCB_RX_INTR_OVERFLOW | CY_SCB_RX_INTR_SPI_PARITY_ERROR);
            }
            else
            {
                Cy_SCB_SetRxInterruptMask(base, CY_SCB_RX_INTR_LEVEL | CY_SCB_RX_INTR_OVERFLOW);
            }
#endif /* CY_IP_M0S8SCB */
            Cy_SCB_SetTxInterruptMask(base, CY_SCB_TX_INTR_LEVEL | CY_SCB_TX_INTR_UNDERFLOW);
        }

        retStatus = CY_SCB_SPI_SUCCESS;
    }

    return (retStatus);
}


/*******************************************************************************
* Function Name: Cy_SCB_SPI_AbortTransfer
****************************************************************************//**
*
* Aborts the current SPI transfer.
* It disables the TX and RX interrupt sources, clears the TX
* and RX FIFOs and the status.
*
* \param base
* The pointer to the SPI SCB instance.
*
* \param context
* The pointer to the context structure \ref cy_stc_scb_spi_context_t allocated
* by the user. The structure is used during the SPI operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \note
* If slave aborts transfer and the master is still transferring data,
* that data will be placed in the RX FIFO, and the TX underflow will be set.
* To drop data received into the RX FIFO, RX FIFO must be cleared when
* the transfer is complete. Otherwise, received data will be kept and
* copied to the buffer when \ref Cy_SCB_SPI_Transfer is called.
*
* \sideeffect
* The transmit FIFO clear operation also clears the shift register, so that
* the shifter can be cleared in the middle of a data element transfer,
* corrupting it. The data element corruption means that all bits that have
* not been transmitted are transmitted as "ones" on the bus.
*
*******************************************************************************/
void Cy_SCB_SPI_AbortTransfer(CySCB_Type *base, cy_stc_scb_spi_context_t *context)
{
    /* Disable interrupt sources */
    Cy_SCB_SetSlaveInterruptMask(base, CY_SCB_CLEAR_ALL_INTR_SRC);

    if (_FLD2BOOL(SCB_SPI_CTRL_MASTER_MODE, SCB_SPI_CTRL(base)))
    {
        Cy_SCB_SetRxInterruptMask(base, CY_SCB_CLEAR_ALL_INTR_SRC);
        Cy_SCB_SetTxInterruptMask(base, CY_SCB_CLEAR_ALL_INTR_SRC);
    }
    else
    {
        Cy_SCB_SetRxInterruptMask(base, CY_SCB_RX_INTR_OVERFLOW);
        Cy_SCB_SetTxInterruptMask(base, CY_SCB_TX_INTR_UNDERFLOW);
    }

    /* Clear FIFOs */
    Cy_SCB_SPI_ClearTxFifo(base);
    Cy_SCB_SPI_ClearRxFifo(base);

    /* Clear the status to allow a new transfer */
    context->status = 0UL;
}


/*******************************************************************************
* Function Name: Cy_SCB_SPI_GetNumTransfered
****************************************************************************//**
*
* Returns the number of data elements transferred since the last call to \ref
* Cy_SCB_SPI_Transfer.
*
* \param base
* The pointer to the SPI SCB instance.
*
* \param context
* The pointer to the context structure \ref cy_stc_scb_spi_context_t allocated
* by the user. The structure is used during the SPI operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \return
* The number of data elements transferred.
*
*******************************************************************************/
uint32_t Cy_SCB_SPI_GetNumTransfered(CySCB_Type const *base, cy_stc_scb_spi_context_t const *context)
{
    /* Suppress a compiler warning about unused variables */
    CY_UNUSED_PARAMETER(base);

    return (context->rxBufIdx);
}


/*******************************************************************************
* Function Name: Cy_SCB_SPI_GetTransferStatus
****************************************************************************//**
*
* Returns the status of the transfer operation started by
* \ref Cy_SCB_SPI_Transfer.
* This status is a bit mask and the value returned may have a multiple-bit set.
*
* \param base
* The pointer to the SPI SCB instance.
*
* \param context
* The pointer to the context structure \ref cy_stc_scb_spi_context_t allocated
* by the user. The structure is used during the SPI operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
* \return
* \ref group_scb_spi_macros_xfer_status.
*
* \note
* The status is cleared by calling \ref Cy_SCB_SPI_Transfer or
* \ref Cy_SCB_SPI_AbortTransfer.
*
*******************************************************************************/
uint32_t Cy_SCB_SPI_GetTransferStatus(CySCB_Type const *base, cy_stc_scb_spi_context_t const *context)
{
    /* Suppress a compiler warning about unused variables */
    CY_UNUSED_PARAMETER(base);

    return (context->status);
}


/*******************************************************************************
* Function Name: Cy_SCB_SPI_Interrupt
****************************************************************************//**
*
* This is the interrupt function for the SCB configured in the SPI mode.
* This function must be called inside the  user-defined interrupt service
* routine for \ref Cy_SCB_SPI_Transfer to work.
*
* \param base
* The pointer to the SPI SCB instance.
*
* \param context
* The pointer to the context structure \ref cy_stc_scb_spi_context_t allocated
* by the user. The structure is used during the SPI operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
*******************************************************************************/
void Cy_SCB_SPI_Interrupt(CySCB_Type *base, cy_stc_scb_spi_context_t *context)
{
    bool locXferErr = false;

    /* Wake up on the slave select condition */
    if (0UL != (CY_SCB_SPI_INTR_WAKEUP & Cy_SCB_GetSpiInterruptStatusMasked(base)))
    {
        Cy_SCB_ClearSpiInterrupt(base, CY_SCB_SPI_INTR_WAKEUP);
    }

    /* The slave error condition */
    if (0UL != (CY_SCB_SLAVE_INTR_SPI_BUS_ERROR & Cy_SCB_GetSlaveInterruptStatusMasked(base)))
    {
        locXferErr       = true;
        context->status |= CY_SCB_SPI_SLAVE_TRANSFER_ERR;

        Cy_SCB_ClearSlaveInterrupt(base, CY_SCB_SLAVE_INTR_SPI_BUS_ERROR);
    }

#ifdef CY_IP_MXSCB
    /* The parity error condition*/
    if ((0UL != (CY_SCB_RX_INTR_SPI_PARITY_ERROR & Cy_SCB_GetRxInterruptStatusMasked(base))))
    {
        locXferErr = true;
        context->status |= CY_SCB_SPI_TRANSFER_PARITY_ERR;

        Cy_SCB_ClearRxInterrupt(base, CY_SCB_RX_INTR_SPI_PARITY_ERROR);
    }
#endif /* CY_IP_MXSCB */

    /* The RX overflow error condition */
    if (0UL != (CY_SCB_RX_INTR_OVERFLOW & Cy_SCB_GetRxInterruptStatusMasked(base)))
    {
        locXferErr       = true;
        context->status |= CY_SCB_SPI_TRANSFER_OVERFLOW;

        Cy_SCB_ClearRxInterrupt(base, CY_SCB_RX_INTR_OVERFLOW);
    }

    /* The TX underflow error condition or slave complete data transfer */
    if (0UL != (CY_SCB_TX_INTR_UNDERFLOW & Cy_SCB_GetTxInterruptStatusMasked(base)))
    {
        locXferErr       = true;
        context->status |= CY_SCB_SPI_TRANSFER_UNDERFLOW;

        Cy_SCB_ClearTxInterrupt(base, CY_SCB_TX_INTR_UNDERFLOW);
    }

    /* Report an error, use a callback */
    if (locXferErr)
    {
        if (NULL != context->cbEvents)
        {
            context->cbEvents(CY_SCB_SPI_TRANSFER_ERR_EVENT);
        }
    }

    /* RX direction */
    if (0UL != (CY_SCB_RX_INTR_LEVEL & Cy_SCB_GetRxInterruptStatusMasked(base)))
    {
        HandleReceive(base, context);

        Cy_SCB_ClearRxInterrupt(base, CY_SCB_RX_INTR_LEVEL);
    }

    /* TX direction */
    if (0UL != (CY_SCB_TX_INTR_LEVEL & Cy_SCB_GetTxInterruptStatusMasked(base)))
    {
        HandleTransmit(base, context);

        Cy_SCB_ClearTxInterrupt(base, CY_SCB_TX_INTR_LEVEL);
    }

    /* The transfer is complete: all data is loaded in the TX FIFO
    * and all data is read from the RX FIFO
    */
    if ((0UL != (context->status & CY_SCB_SPI_TRANSFER_ACTIVE)) &&
        (0UL == context->rxBufSize) && (0UL == context->txBufSize))
    {
        /* The transfer is complete */
        context->status &= (uint32_t) ~CY_SCB_SPI_TRANSFER_ACTIVE;

        if (NULL != context->cbEvents)
        {
            context->cbEvents(CY_SCB_SPI_TRANSFER_CMPLT_EVENT);
        }
    }
}


/*******************************************************************************
* Function Name: HandleReceive
****************************************************************************//**
*
* Reads data from RX FIFO into the buffer provided by \ref Cy_SCB_SPI_Transfer.
*
* \param base
* The pointer to the SPI SCB instance.
*
* \param context
* The pointer to the context structure \ref cy_stc_scb_spi_context_t allocated
* by the user. The structure is used during the SPI operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
*******************************************************************************/
static void HandleReceive(CySCB_Type *base, cy_stc_scb_spi_context_t *context)
{
    /* Get data in RX FIFO */
    uint32_t numToCopy = Cy_SCB_GetNumInRxFifo(base);

    /* Adjust the number to read */
    if (numToCopy > context->rxBufSize)
    {
        numToCopy = context->rxBufSize;
    }

    /* Move the buffer */
    context->rxBufIdx  += numToCopy;
    context->rxBufSize -= numToCopy;

    /* Read data from RX FIFO */
    if (NULL != context->rxBuf)
    {
        uint8_t *buf = (uint8_t *) context->rxBuf;

        Cy_SCB_ReadArrayNoCheck(base, context->rxBuf, numToCopy);

        buf = &buf[(Cy_SCB_IsRxDataWidthByte(base) ? (numToCopy) : (2UL * numToCopy))];
        context->rxBuf = (void *) buf;
    }
    else
    {
        /* Discard read data. */
        DiscardArrayNoCheck(base, numToCopy);
    }

    if (0UL == context->rxBufSize)
    {
        /* Disable the RX level interrupt */
        Cy_SCB_SetRxInterruptMask(base, (Cy_SCB_GetRxInterruptMask(base) & (uint32_t) ~CY_SCB_RX_INTR_LEVEL));
    }
    else
    {
        uint32_t level = (_FLD2BOOL(SCB_SPI_CTRL_MASTER_MODE, SCB_SPI_CTRL(base))) ?
                                    Cy_SCB_GetFifoSize(base) : (Cy_SCB_GetFifoSize(base) / 2UL);

        if (context->rxBufSize < level)
        {
            Cy_SCB_SetRxFifoLevel(base, (context->rxBufSize - 1UL));
        }
    }
}


/*******************************************************************************
* Function Name: HandleTransmit
****************************************************************************//**
*
* Loads TX FIFO with data provided by \ref Cy_SCB_SPI_Transfer.
*
* \param base
* The pointer to the SPI SCB instance.
*
* \param context
* The pointer to the context structure \ref cy_stc_scb_spi_context_t allocated
* by the user. The structure is used during the SPI operation for internal
* configuration and data retention. The user must not modify anything
* in this structure.
*
*******************************************************************************/
static void HandleTransmit(CySCB_Type *base, cy_stc_scb_spi_context_t *context)
{
    uint32_t numToCopy;
    uint32_t fifoSize = Cy_SCB_GetFifoSize(base);

    if (_FLD2BOOL(SCB_SPI_CTRL_MASTER_MODE, SCB_SPI_CTRL(base)))
    {
        /* Do not allow lose data in RX buffer. In case if in TX buffer load more bytes
        * than free space in RX buffer.
        */
        numToCopy = fifoSize - Cy_SCB_GetNumInTxFifo(base) - Cy_SCB_GetNumInRxFifo(base);
    }
    else
    {
        numToCopy = fifoSize - Cy_SCB_GetNumInTxFifo(base);
    }

    /* Adjust the number to load */
    if (numToCopy > context->txBufSize)
    {
        numToCopy = context->txBufSize;
    }

    /* Move the buffer */
    context->txBufIdx  += numToCopy;
    context->txBufSize -= numToCopy;

    /* Load TX FIFO with data */
    if (NULL != context->txBuf)
    {
        uint8_t *buf = (uint8_t *) context->txBuf;

        Cy_SCB_WriteArrayNoCheck(base, context->txBuf, numToCopy);

        buf = &buf[(Cy_SCB_IsTxDataWidthByte(base) ? (numToCopy) : (2UL * numToCopy))];
        context->txBuf = (void *) buf;
    }
    else
    {
        Cy_SCB_WriteDefaultArrayNoCheck(base, context->txDefaultValue, numToCopy);
    }

    if (0UL == context->txBufSize)
    {
        /* Data is transferred into TX FIFO */
        context->status |= CY_SCB_SPI_TRANSFER_IN_FIFO;

        /* Disable the TX level interrupt */
        Cy_SCB_SetTxInterruptMask(base, (Cy_SCB_GetTxInterruptMask(base) & (uint32_t) ~CY_SCB_TX_INTR_LEVEL));

        if (NULL != context->cbEvents)
        {
            context->cbEvents(CY_SCB_SPI_TRANSFER_IN_FIFO_EVENT);
        }
    }
}


/*******************************************************************************
* Function Name: DiscardArrayNoCheck
****************************************************************************//**
*
* Reads the number of data elements from the SPI RX FIFO. The read data is
* discarded. Before calling this function, make sure that RX FIFO has
* enough data elements to read.
*
* \param base
* The pointer to the SPI SCB instance.
*
* \param size
* The number of data elements to read.
*
*******************************************************************************/
static void DiscardArrayNoCheck(CySCB_Type const *base, uint32_t size)
{
    uint32_t locSize = size;

    while (locSize > 0UL)
    {
        (void) Cy_SCB_SPI_Read(base);
        --locSize;
    }
}

#if defined(__cplusplus)
}
#endif

#endif /* defined (CY_IP_M0S8SCB) || defined (CY_IP_MXSCB) */

/* [] END OF FILE */
