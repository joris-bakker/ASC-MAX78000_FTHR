#ifndef _MFCC_DATA_H_
#define _MFCC_DATA_H_ 

#include "arm_math_types.h"


#ifdef   __cplusplus
extern "C"
{
#endif


/*****

 DCT COEFFICIENTS FOR THE MFCC

*****/


#define NB_MFCC_DCT_COEFS_CONFIG1 1600
extern const float32_t mfcc_dct_coefs_config1[NB_MFCC_DCT_COEFS_CONFIG1];



/*****

 WINDOW COEFFICIENTS

*****/


#define NB_MFCC_WIN_COEFS_CONFIG1 1024
extern const float32_t mfcc_window_coefs_config1[NB_MFCC_WIN_COEFS_CONFIG1];



/*****

 MEL FILTER COEFFICIENTS FOR THE MFCC

*****/

#define NB_MFCC_NB_FILTER_CONFIG1 40
extern const uint32_t mfcc_filter_pos_config1[NB_MFCC_NB_FILTER_CONFIG1];
extern const uint32_t mfcc_filter_len_config1[NB_MFCC_NB_FILTER_CONFIG1];





#define NB_MFCC_FILTER_COEFS_CONFIG1 987
extern const float32_t mfcc_filter_coefs_config1[NB_MFCC_FILTER_COEFS_CONFIG1];



#ifdef   __cplusplus
}
#endif

#endif

