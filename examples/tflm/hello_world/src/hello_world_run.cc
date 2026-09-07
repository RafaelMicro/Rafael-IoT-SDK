/*
 * Copyright (c) 2026 Rafael Microelectronics Inc. All rights reserved.
 *
 * SPDX-License-Identifier: LicenseRef-RafaelMicro-Proprietary-1.0
 *
 * TensorFlow Lite for Microcontrollers smoke test: the classic "hello_world"
 * sine model (y = sin(x)) quantized to int8. Exercises the full TFLM pipeline
 * end to end - model load, tensor allocation, and inference - on Cortex-M33 with
 * CMSIS-NN as the optimized kernel backend. Output is printed via the SDK UART
 * stdio (printf), so no %f float-printf or libm dependency is used.
 *
 * Entry point tflm_hello_world_run() is called from the FreeRTOS task in main.c.
 */

#include <cstdint>
#include <cstdio>

#include "hello_world_int8_model_data.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"

namespace {

// Static tensor arena (no dynamic allocation - TF_LITE_STATIC_MEMORY). 8 KiB is
// comfortably above what the tiny hello_world model needs; arena_used_bytes()
// below reports the real figure.
constexpr int kTensorArenaSize = 8 * 1024;
alignas(16) uint8_t g_tensor_arena[kTensorArenaSize];

// Print a float as fixed-point (sign + 3 fractional digits) using only integer
// printf - newlib-nano printf does not carry %f by default.
void PrintFixed(float v) {
  int milli = static_cast<int>(v * 1000.0f + (v >= 0.0f ? 0.5f : -0.5f));
  const char* sign = "";
  if (milli < 0) {
    sign = "-";
    milli = -milli;
  }
  printf("%s%d.%03d", sign, milli / 1000, milli % 1000);
}

// A few sample angles across [0, 2*pi] with the reference sin(x), so we can show
// prediction vs. ground truth without pulling in libm's sinf().
struct Sample {
  float x;
  float sin_x;
};
const Sample kSamples[] = {
    {0.000f, 0.000f},  {0.785f, 0.707f},   {1.571f, 1.000f},
    {2.356f, 0.707f},  {3.142f, 0.000f},   {3.927f, -0.707f},
    {4.712f, -1.000f}, {5.498f, -0.707f},  {6.283f, 0.000f},
};

}  // namespace

extern "C" void tflm_hello_world_run(void) {
  tflite::InitializeTarget();

  const tflite::Model* model = tflite::GetModel(g_hello_world_int8_model_data);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    printf("[TFLM] model schema %u != supported %d\r\n",
           static_cast<unsigned>(model->version()), TFLITE_SCHEMA_VERSION);
    return;
  }

  // hello_world is a stack of fully-connected layers -> a single op registered.
  static tflite::MicroMutableOpResolver<1> resolver;
  if (resolver.AddFullyConnected() != kTfLiteOk) {
    printf("[TFLM] AddFullyConnected failed\r\n");
    return;
  }

  static tflite::MicroInterpreter interpreter(model, resolver, g_tensor_arena,
                                              kTensorArenaSize);
  if (interpreter.AllocateTensors() != kTfLiteOk) {
    printf("[TFLM] AllocateTensors failed\r\n");
    return;
  }
  printf("[TFLM] AllocateTensors OK, arena used = %u bytes\r\n",
         static_cast<unsigned>(interpreter.arena_used_bytes()));

  TfLiteTensor* input = interpreter.input(0);
  TfLiteTensor* output = interpreter.output(0);
  const float in_scale = input->params.scale;
  const int in_zero_point = input->params.zero_point;
  const float out_scale = output->params.scale;
  const int out_zero_point = output->params.zero_point;

  printf("[TFLM] y = sin(x) inference (int8, CMSIS-NN backend):\r\n");
  for (const Sample& s : kSamples) {
    // Quantize the float input to int8 for the model.
    int q = static_cast<int>(s.x / in_scale + in_zero_point +
                             (s.x >= 0.0f ? 0.5f : -0.5f));
    if (q < -128) q = -128;
    if (q > 127) q = 127;
    input->data.int8[0] = static_cast<int8_t>(q);

    if (interpreter.Invoke() != kTfLiteOk) {
      printf("[TFLM] Invoke failed\r\n");
      return;
    }

    // Dequantize the int8 output back to a float prediction.
    float y = (output->data.int8[0] - out_zero_point) * out_scale;

    printf("  x=");
    PrintFixed(s.x);
    printf("  y_pred=");
    PrintFixed(y);
    printf("  y_true=");
    PrintFixed(s.sin_x);
    printf("\r\n");
  }
  printf("[TFLM] inference done.\r\n");
}
