#ifndef CPC_TIMER_H
#define CPC_TIMER_H

#include "FreeRTOS.h"
#include "cpc.h"

#include "sw_timer.h"

typedef sw_timer_t cpc_timer_handle_t;

/***************************************************************************/ /**
 * Typedef for the user supplied callback function which is called when
 * a timer expires.
 *
 * @param data An extra parameter for the user application.
 ******************************************************************************/
typedef void (*cpc_timer_callback_t)(void* data);

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************/ /**
 * Converts milliseconds into ticks.
 *
 * @param time_ms Number of milliseconds.
 *
 * @return Corresponding ticks number.
 *
 * @note The result is "rounded" to the superior tick number.
 *       This function is light and cannot fail so it should be privileged to
 *       perform a millisecond to tick conversion.
 ******************************************************************************/
uint32_t cpc_timer_ms_to_tick(uint16_t time_ms);

/***************************************************************************/ /**
 * Converts ticks in milliseconds.
 *
 * @param tick Number of tick.
 *
 * @return Corresponding milliseconds number.
 *
 * @note The result is rounded to the inferior millisecond.
 ******************************************************************************/
uint32_t cpc_timer_tick_to_ms(uint32_t tick);

/***************************************************************************/ /**
 * Gets current 32 bits global tick count.
 *
 * @return Current tick count.
 ******************************************************************************/
uint32_t cpc_timer_get_tick_count(void);

/***************************************************************************/ /**
 * Gets current 64 bits global tick count.
 *
 * @return Current tick count.
 ******************************************************************************/
uint64_t cpc_timer_get_tick_count64(void);

/***************************************************************************/ /**
 * Restarts a 32 bits timer.
 *
 * @param handle Pointer to handle to timer.
 * @param timeout Timer timeout, in timer ticks.
 * @param callback Callback function that will be called when
 *        initial/periodic timeout expires.
 * @param callback_data Pointer to user data that will be passed to callback.

 * @return STATUS_OK if successful.
 ******************************************************************************/
status_t cpc_timer_restart_timer(cpc_timer_handle_t** handle, uint32_t timeout,
                                 cpc_timer_callback_t callback,
                                 void* callback_data);

/***************************************************************************/ /**
 * Stops a timer.
 *
 * @param handle Pointer to handle to timer.
 *
 * @return STATUS_OK if successful.
 ******************************************************************************/
status_t cpc_timer_stop_timer(cpc_timer_handle_t* handle);

#ifdef __cplusplus
}
#endif

#endif // CPC_TIMER_H
