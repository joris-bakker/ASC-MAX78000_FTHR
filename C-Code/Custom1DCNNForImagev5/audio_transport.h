/*
 * audio_transport.h
 *
 *  Created on: 31.01.2025
 *      Author: student1
 */

#ifndef MY_LIBS_AUDIO_TRANSPORT_H_
#define MY_LIBS_AUDIO_TRANSPORT_H_

#include <stdint.h>

#endif /* MY_LIBS_AUDIO_TRANSPORT_H_ */




int calc_average(int16_t *input, int sample_size);
int16_t stereo_to_mono (int32_t input);
void resample_linear_int16(const int16_t *input, int16_t *output, int in_len, int out_len);
