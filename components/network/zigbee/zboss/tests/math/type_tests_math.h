#ifndef TYPE_TESTS_MATH_H
#define TYPE_TESTS_MATH_H

#include <zboss_api.h>

typedef struct
{
  zb_int64_t param1;
  zb_int64_t param2;
  zb_int64_t param3;
} test_param_t;

typedef int (*test_func)(test_param_t);

typedef struct
{
  test_func func;
  int expected_ret;
  test_param_t params;
} test_t;

#define TEST_ZB_MATH(TYPETEST, FUNC_TO_TYPETEST, FUNC_TO_INT, MATH_FUNC, RET_VAL) \
do                                                                                \
{                                                                                 \
  TYPETEST type_param1, type_param2, type_result;                                 \
                                                                                  \
  FUNC_TO_TYPETEST(param.param1, &type_param1);                                   \
  FUNC_TO_TYPETEST(param.param2, &type_param2);                                   \
                                                                                  \
  RET_VAL = (int)MATH_FUNC(&type_param1, &type_param2, &type_result);             \
  if (RET_VAL == ZB_MATH_OK)                                                      \
  {                                                                               \
    if (param.param3 != FUNC_TO_INT(&type_result))                                \
    {                                                                             \
      RET_VAL = -1;                                                               \
    }                                                                             \
  }                                                                               \
} while (0)

#endif /* TYPE_TESTS_MATH_H */
