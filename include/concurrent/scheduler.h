#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup concurrent Concurrent
 * @{
 */

/**
 * @defgroup scheduler Scheduler
 * @brief Scheduler management functions.
 *
 * @ingroup concurrent
 * @{
 */

/**

* @brief Start the scheduler on the secondary processor.
*
* Starts the scheduler on the secondary processor, enabling it to participate
* in concurrent execution of application threads.
*
* The secondary scheduler is not started automatically. The application must
* call this function explicitly when dual-processor scheduling is desired.
*
* @note This function must only be called once.
  */
void scheduler_start_secondary(void);

/** @} */ /* end of scheduler */
/** @} */ /* end of concurrent */

#ifdef __cplusplus
}
#endif
