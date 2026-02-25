#ifndef HAL_OPAMP_H_
#define HAL_OPAMP_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32h7xx.h"
#include "stm32h7xx_ll_opamp.h"
#include "stm32h7xx_ll_rcc.h"

void OpAmp4Init();
void OpAmp5Init();

#ifdef __cplusplus
}
#endif

#endif /* HAL_OPAMP_H_ */
