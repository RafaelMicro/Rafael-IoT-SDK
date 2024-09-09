/**
 * @file status.h
 * @author Rex Huang (rex.huang@rafaelmicro.com)
 * @brief 
 * @version 0.1
 * @date 2023-08-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef CPC_STATUS_H
#define CPC_STATUS_H

#include "stdint.h"

typedef uint32_t status_t;

enum status_t_enum {
    /* No error. */
    CPC_STATUS_OK = 0x0000,
    /* Generic error. */
    CPC_STATUS_FAIL = 0x0001,

    /* ============ */
    /* State Errors */
    /* ============ */
    /* Generic invalid state error. */
    CPC_STATUS_INVALID_STATE = 0x0002,
    /* Module is not ready for requested operation. */
    CPC_STATUS_NOT_READY = 0x0003,
    /* Module is busy and cannot carry out requested operation. */
    CPC_STATUS_BUSY = 0x0004,
    /* Operation is in progress and not yet complete (pass or fail). */
    CPC_STATUS_IN_PROGRESS = 0x0005,
    /* Operation aborted. */
    CPC_STATUS_ABORT = 0x0006,
    /* Operation timed out. */
    CPC_STATUS_TIMEOUT = 0x0007,
    /* Operation not allowed per permissions. */
    CPC_STATUS_PERMISSION = 0x0008,
    /* Non-blocking operation would block. */
    CPC_STATUS_WOULD_BLOCK = 0x0009,
    /* Operation/module is Idle, cannot carry requested operation. */
    CPC_STATUS_IDLE = 0x000A,
    /* Operation cannot be done while construct is waiting. */
    CPC_STATUS_IS_WAITING = 0x000B,
    /* No task/construct waiting/pending for that action/event. */
    CPC_STATUS_NONE_WAITING = 0x000C,
    /* Operation cannot be done while construct is suspended. */
    CPC_STATUS_SUSPENDED = 0x000D,
    /* Feature not available due to software configuration. */
    CPC_STATUS_NOT_AVAILABLE = 0x000E,
    /* Feature not supported. */
    CPC_STATUS_NOT_SUPPORTED = 0x000F,
    /* Initialization failed. */
    CPC_STATUS_INITIALIZATION = 0x0010,
    /* Module has not been initialized. */
    CPC_STATUS_NOT_INITIALIZED = 0x0011,
    /* Module has already been initialized. */
    CPC_STATUS_ALREADY_INITIALIZED = 0x0012,
    /* Object/construct has been deleted. */
    CPC_STATUS_DELETED = 0x0013,
    /* Illegal call from ISR. */
    CPC_STATUS_ISR = 0x0014,
    /* Illegal call because network is up. */
    CPC_STATUS_NETWORK_UP = 0x0015,
    /* Illegal call because network is down. */
    CPC_STATUS_NETWORK_DOWN = 0x0016,
    /* Failure due to not being joined in a network. */
    CPC_STATUS_NOT_JOINED = 0x0017,
    /* Invalid operation as there are no beacons. */
    CPC_STATUS_NO_BEACONS = 0x0018,

    /* =========================== */
    /* Allocation/ownership Errors */
    /* =========================== */
    /* Generic allocation error. */
    CPC_STATUS_ALLOCATION_FAILED = 0x0019,
    /* No more resource available to perform the operation. */
    CPC_STATUS_NO_MORE_RESOURCE = 0x001A,
    /* Item/list/queue is empty. */
    CPC_STATUS_EMPTY = 0x001B,
    /* Item/list/queue is full. */
    CPC_STATUS_FULL = 0x001C,
    /* Item would overflow. */
    CPC_STATUS_WOULD_OVERFLOW = 0x001D,
    /* Item/list/queue has been overflowed. */
    CPC_STATUS_HAS_OVERFLOWED = 0x001E,
    /* Generic ownership error. */
    CPC_STATUS_OWNERSHIP = 0x001F,
    /* Already/still owning resource. */
    CPC_STATUS_IS_OWNER = 0x0020,

    /* ========================= */
    /* Invalid Parameters Errors */
    /* ========================= */
    /* Generic invalid argument or consequence of invalid argument. */
    CPC_STATUS_INVALID_PARAMETER = 0x0021,
    /* Invalid null pointer received as argument. */
    CPC_STATUS_NULL_POINTER = 0x0022,
    /* Invalid configuration provided. */
    CPC_STATUS_INVALID_CONFIGURATION = 0x0023,
    /* Invalid mode. */
    CPC_STATUS_INVALID_MODE = 0x0024,
    /* Invalid handle. */
    CPC_STATUS_INVALID_HANDLE = 0x0025,
    /* Invalid type for operation. */
    CPC_STATUS_INVALID_TYPE = 0x0026,
    /* Invalid index. */
    CPC_STATUS_INVALID_INDEX = 0x0027,
    /* Invalid range. */
    CPC_STATUS_INVALID_RANGE = 0x0028,
    /* Invalid key. */
    CPC_STATUS_INVALID_KEY = 0x0029,
    /* Invalid credentials. */
    CPC_STATUS_INVALID_CREDENTIALS = 0x002A,
    /* Invalid count. */
    CPC_STATUS_INVALID_COUNT = 0x002B,
    /* Invalid signature / verification failed. */
    CPC_STATUS_INVALID_SIGNATURE = 0x002C,
    /* Item could not be found. */
    CPC_STATUS_NOT_FOUND = 0x002D,
    /* Item already exists. */
    CPC_STATUS_ALREADY_EXISTS = 0x002E,

    /* ======================= */
    /* IO/Communication Errors */
    /* ======================= */
    /* Generic I/O failure. */
    CPC_STATUS_IO = 0x002F,
    /* I/O failure due to timeout. */
    CPC_STATUS_IO_TIMEOUT = 0x0030,
    /* Generic transmission error. */
    CPC_STATUS_TRANSMIT = 0x0031,
    /* Transmit underflowed. */
    CPC_STATUS_TRANSMIT_UNDERFLOW = 0x0032,
    /* Transmit is incomplete. */
    CPC_STATUS_TRANSMIT_INCOMPLETE = 0x0033,
    /* Transmit is busy. */
    CPC_STATUS_TRANSMIT_BUSY = 0x0034,
    /* Generic reception error. */
    CPC_STATUS_RECEIVE = 0x0035,
    /* Failed to read on/via given object. */
    CPC_STATUS_OBJECT_READ = 0x0036,
    /* Failed to write on/via given object. */
    CPC_STATUS_OBJECT_WRITE = 0x0037,
    /* Message is too long. */
    CPC_STATUS_MESSAGE_TOO_LONG = 0x0038,
};

#endif