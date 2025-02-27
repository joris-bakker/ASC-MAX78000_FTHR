

/*
 * In diesem Ordner werden alle initialisierungen für den Codec festgelegt, und der Datentransport!
 *
 * Die Funktionen wurden teilweise mit CHATGPT generiert, und dann angepasst.
 */

#include "audio_transport.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "mxc.h" // Include hardware-specific functions.



int16_t stereo_to_mono (int32_t input){

	int16_t channel_1 = (int16_t)(input & 0x0000FFFF); //unterer Teil
	int16_t channel_2 = (int16_t)(input >>16);			// oberer Teil

	return (channel_1 / 2) + (channel_2 / 2);
	};

int calc_average(int16_t *input, int sample_size){
	int ergebnis = 0;
	int sum = 0;

	for (int i = 0; i < sample_size; i++)
	{
		sum += abs(input[i]);
	}

	ergebnis = (int)sum/sample_size;


	return ergebnis;
}


void resample_linear_int16(const int16_t *input, int16_t *output, int in_len, int out_len) {
    for (int i = 0; i < out_len; i++) {
        float pos = (float)i * (in_len - 1) / (out_len - 1);
        int index = (int)pos;
        float frac = pos - index;

        if (index + 1 < in_len) {
            output[i] = (int16_t)((1.0f - frac) * input[index] + frac * input[index + 1]);
        } else {
            output[i] = input[index];  // Letzter Punkt
        }
    }
}
