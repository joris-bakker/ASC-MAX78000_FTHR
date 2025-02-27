/*
 * my_mfcc.h
 *
 *  Created on: 03.02.2025
 *      Author: student1
 */

#ifndef MY_MFCC_H_
#define MY_MFCC_H_

#include "arm_math.h"

void initMFCCInstance(void);
int computeMFCC(float32_t *inputSignal, float32_t *outputMFCC);


#endif /* MY_MFCC_H_ */
