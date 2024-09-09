/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
 * www.dsr-zboss.com
 * www.dsr-corporation.com
 * All rights reserved.
 *
 * This is unpublished proprietary source code of DSR Corporation
 * The copyright notice does not evidence any actual or intended
 * publication of such source code.
 *
 * ZBOSS is a registered trademark of Data Storage Research LLC d/b/a DSR
 * Corporation
 *
 * Commercial Usage
 * Licensees holding valid DSR Commercial licenses may use
 * this file in accordance with the DSR Commercial License
 * Agreement provided with the Software or, alternatively, in accordance
 * with the terms contained in a written agreement between you and
 * DSR.
 */
/*  PURPOSE: Linux GPIO access declarations.
*/
#ifndef GPIO_SERVE_H
#define GPIO_SERVE_H 1

#include <stdint.h>

#include "linux_spi_internal_types.h"

#define GPIO_MAX_PIN 30
#define GPIO_MAX_FD_PATH_LEN 128

#ifndef HOST_INT_ACTIVE
#define HOST_INT_ACTIVE 0
#endif
#define HOST_INT_EDGE ((HOST_INT_ACTIVE) ? "rising\n" : "falling\n")
#define HOST_INT_VALUE ((HOST_INT_ACTIVE) ? 1 : 0)

typedef unsigned int gpio_pin_t;

typedef void (*gpio_handler_t)(void);

typedef enum gpio_direction_e
{
  GPIO_IN = 0,
  GPIO_OUT
}
gpio_direction_t;

typedef struct gpio_conf_item_s
{
  gpio_pin_t pin;
  gpio_handler_t handler;
  char path[GPIO_MAX_FD_PATH_LEN];
  int fd_value;
  uint32_t last_value;
  spi_bool_t updated;
  gpio_direction_t direction;
}
gpio_conf_item_t;

extern int ctrl_pipe_fd[2];
#define CTRL_PIPE_READ_FD   ctrl_pipe_fd[0]
#define CTRL_PIPE_WRITE_FD  ctrl_pipe_fd[1]

int gpio_in_init(gpio_pin_t pin, gpio_handler_t handler);
int gpio_out_init(gpio_pin_t pin, char initial_value);
void check_select(spi_bool_t wait);
spi_bool_t check_gpio(gpio_pin_t pin);
void free_space_has_appeared(gpio_pin_t pin);
int gpio_write(gpio_pin_t pin, char value);

#endif /* GPIO_SERVE_H */
