/*
 * my_mfcc.c
 *
 *  Created on: 03.02.2025
 *      Author: student1
 */



/*
 * my_mfcc.c
 *
 *  Created on: 31.01.2025
 *      Author: student1
 */

#include <stdio.h>
#include <stdint.h>
#include "mxc_device.h"
#include "board.h"
#include "mxc_delay.h"
#include "arm_math.h"
#include "arm_const_structs.h"


#include "my_mfcc.h"
#include "mfcc_data.h"



#define FRAME_SIZE 512              // Eingabefenstergröße
#define FFT_SIZE 1024                // FFT-Größe
#define NUM_MEL_FILTERS 40          // Anzahl der Mel-Filter
#define NUM_DCT_COEFFS 40          // Anzahl der MFCC-Koeffizienten
#define TEMP_BUFFER_SIZE (FFT_SIZE*2)

float32_t tempBufferQ31[TEMP_BUFFER_SIZE]; // Temporärer Puffer

arm_mfcc_instance_f32 mfccInstance;



// Funktion zur Initialisierung einer separaten MFCC-Instanz
void initMFCCInstance(void) {
    // Dann initialisieren:

    arm_mfcc_init_f32(&mfccInstance,
                      FFT_SIZE,
                      NUM_MEL_FILTERS,
                      NUM_DCT_COEFFS,
					  mfcc_dct_coefs_config1,
					  mfcc_filter_pos_config1,
					  mfcc_filter_len_config1,
					  mfcc_filter_coefs_config1,
					  mfcc_window_coefs_config1);
}

int computeMFCC(float32_t *inputSignal, float32_t *outputMFCC) {
    arm_mfcc_f32(&mfccInstance, inputSignal, outputMFCC, tempBufferQ31);
    return 0;
}
