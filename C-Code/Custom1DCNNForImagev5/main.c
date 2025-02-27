
 /*
Dieser Code ist das Das Deployed Model der Bachelorarbeit. Funktionen die nicht für die Anwendung nötig sind wurden auskommentiert.
Nutzung von Chat GPT: 
Der Dataloader wurde teilweise durch chat GPT erzeugt.  
Prompt:
 Speicher jede reihe der 10 reihen des arrays mfcc_q32[10][311]  in neuen arrays die eine länge von 311 Werten haben

 */


#include <stdio.h>
#include <stdint.h>
#include "mxc.h" // Include hardware-specific functions.
#include "board.h" // Include board-specific configurations.
#include <stdlib.h>
#include <string.h>
#include "mxc.h"
#include "cnn.h"
#include <math.h>   // Für roundf
//#include "sampledata.h"
//#include "sampleoutput.h"
#include "my_mfcc.h"
#include "arm_math.h"
#include "ringbuffer.h"
#include "audio_transport.h"

#include "max9867.h" // Include MAX9867 codec-specific functions.

#define CODEC_I2C MXC_I2C1
#define CODEC_I2C_FREQ 100000
#define THRESHOLD 78
#define CODEC_MCLOCK 12288000
#define SAMPLE_RATE 48000
#define DOWNSAMPLE_SIZE 512
#define I2S_DMA_BUFFER_SIZE DOWNSAMPLE_SIZE*3

volatile int dma_flag;
uint32_t i2s_rx_buffer[I2S_DMA_BUFFER_SIZE * 2];
int dma_ch_tx, dma_ch_rx;
uint32_t *rxBufPtr = i2s_rx_buffer;

int data_ready = 0;
int32_t zwischenpuffer [I2S_DMA_BUFFER_SIZE]; // DMA callback
int16_t zwischenpuffer_resample[DOWNSAMPLE_SIZE];
int16_t mono_out[I2S_DMA_BUFFER_SIZE];
int average = 0;
uint8_t skip_average = 0;
uint8_t start_ringbuffer = 0;
int block_recording = 0;
int x = 0;
int y = 0;
int j = 0;
int calculate_cnn = 0;
int16_t data_out[DOWNSAMPLE_SIZE*2];
float32_t mfcc_in [DOWNSAMPLE_SIZE*2];
float32_t mfcc_out [40];
int8_t mfcc_q8[40][313];
int32_t mfcc_q32[10][313];


RingBuffer rb;

#define INPUT_RATE 48000
#define OUTPUT_RATE 16000
#define INPUT_SAMPLES I2S_DMA_BUFFER_SIZE
#define OUTPUT_SAMPLES ((INPUT_SAMPLES * OUTPUT_RATE) / INPUT_RATE)

#define Q15_MAXa_alt  32767
#define Q15_MIN_alt -32768
#define Q7_MAX_neu   127
#define Q7_MIN_neu  -128

#define TOGGLE_GPIO_PORT MXC_GPIO1    // GPIO Port 1
#define TOGGLE_GPIO_PIN MXC_GPIO_PIN_6 // Pin 6 des Ports



//input laden
static uint32_t input_0[311] = {0};
static uint32_t input_4[311] = {0};
static uint32_t input_8[311] = {0};
static uint32_t input_12[311] = {0};
static uint32_t input_16[311] = {0};
static uint32_t input_20[311] = {0};
static uint32_t input_24[311] = {0};
static uint32_t input_28[311] = {0};
static uint32_t input_32[311] = {0};
static uint32_t input_36[311] = {0};


float32_t inputBuffer[DOWNSAMPLE_SIZE*2];






void gpio_toggle_init(void) {
    mxc_gpio_cfg_t gpio_cfg;

    // Konfiguration von P1_6
    gpio_cfg.port = TOGGLE_GPIO_PORT;          // GPIO Port 1
    gpio_cfg.mask = TOGGLE_GPIO_PIN;          // Pin 6
    gpio_cfg.pad = MXC_GPIO_PAD_NONE;         // Kein Pull-Up/Pull-Down
    gpio_cfg.func = MXC_GPIO_FUNC_OUT;        // Ausgang
    gpio_cfg.vssel = MXC_GPIO_VSSEL_VDDIOH;   // Spannung auf 3.3V
    gpio_cfg.drvstr = MXC_GPIO_DRVSTR_0;      // Standard-Treiberstärke

    if (MXC_GPIO_Config(&gpio_cfg) != E_NO_ERROR) {
        printf("Fehler: GPIO P1_6 konnte nicht initialisiert werden.\n");
        while (1); // Stopp bei Fehler
    }

    printf("GPIO P1_6 erfolgreich initialisiert.\n");
}

// Funktion zur Berechnung des RMS-Werts eines 16-Bit Puffers
double calculate_rms(const float *buffer, int length) {
    double sum = 0.0;
    for (int i = 0; i < length; i++) {
        // Quadrieren des Samples (Explizit in double umwandeln)
        sum += (double)buffer[i] * (double)buffer[i];
    }
    return sqrt(sum / length);
}




void process_audio_data(void)
{
	//resample


for (int i =0; i<I2S_DMA_BUFFER_SIZE; i++)
	{
	mono_out[i]= stereo_to_mono(zwischenpuffer[i]);
	}

resample_linear_int16(mono_out, zwischenpuffer_resample, I2S_DMA_BUFFER_SIZE, DOWNSAMPLE_SIZE);


if(skip_average == 0)
	{
	average = calc_average(mono_out, DOWNSAMPLE_SIZE);

	if (average > THRESHOLD)
	{
	start_ringbuffer = 1;
	//printf("average : %d \n", average);
	skip_average = 1; 																							//müsste 1 sein
	}
	}
if (start_ringbuffer == 1)
	{
	ringbuffer_write(&rb,zwischenpuffer_resample, DOWNSAMPLE_SIZE);
	x++;

	if (x > 1 ) // Genug daten für calc mffcc 															//sollte x>1 sein
	{
		ringbuffer_read_overlap(&rb, data_out, DOWNSAMPLE_SIZE*2);

		for ( int i = 0; i<DOWNSAMPLE_SIZE*2; i++ )
		{
			mfcc_in[i]= data_out[i]*2/ 16384.0f;
		}

//        double rms = calculate_rms(mfcc_in, DOWNSAMPLE_SIZE*2);
//        printf("RMS: %f\n", rms);
		memcpy(inputBuffer, mfcc_in, sizeof(inputBuffer));
		computeMFCC(inputBuffer, mfcc_out );
		//quanzitieren


	    for (int i = 0; i < 40; i++) {
	        // Umwandlung: f32 -> Q7 (Normalisierung)

	        float temp = (mfcc_out[i] - 0.5f) * 256.0f;
	                // Runden (roundf rundet zur nächsten Ganzzahl)
	                int rounded = (int)roundf(temp);
	                // Clipping: auf den Bereich [-128, 127] begrenzen
	                if (rounded < -128) {
	                    rounded = -128;
	                } else if (rounded > 127) {
	                    rounded = 127;
	                }




	        mfcc_q8[i][y] = (int8_t) rounded;
	    }

		y++;
		//printf("y: %d \n", y);
		if (y == 311)
		{
		block_recording = 1;																					//müsste 1 sein
		//printf("recorded enough \n");
		calculate_cnn = 1;  																						// müsste 1 sein
		start_ringbuffer = 0;
		//müsste 0 sein
		}
	}

	}





}



void blink_halt(const char *msg) {
    if (msg && *msg) {
        puts(msg); // Print the error message.
    }
    while (1) {
        LED_On(LED1);  // Turn LED1 on.
        LED_Off(LED2); // Turn LED2 off.
        MXC_Delay(MXC_DELAY_MSEC(500)); // Delay for 500 ms.
        LED_On(LED2);  // Turn LED2 on.
        LED_Off(LED1); // Turn LED1 off.
        MXC_Delay(MXC_DELAY_MSEC(500)); // Delay for 500 ms.
    }
}

void i2c_init(void)
{
    if (MXC_I2C_Init(CODEC_I2C, 1, 0) != E_NO_ERROR)
        blink_halt("Error initializing I2C controller");
    else
        printf("I2C initialized successfully \n");

    MXC_I2C_SetFrequency(CODEC_I2C, CODEC_I2C_FREQ);
}


void codec_init(void)
{
    if (max9867_init(CODEC_I2C, CODEC_MCLOCK, 1) != E_NO_ERROR)
        blink_halt("Error initializing MAX9867 CODEC");

    if (max9867_enable_record(1) != E_NO_ERROR)
        blink_halt("Error enabling record path");

    if (max9867_adc_level(-3, -3) != E_NO_ERROR)
        blink_halt("Error setting ADC level");

    if (max9867_linein_gain(0, 0) != E_NO_ERROR)
        blink_halt("Error setting Line-In gain");
    else
        printf("Codec initialized successfully \n");
}



void i2s_init(void)
{
    mxc_i2s_req_t req;

#define I2S_CRUFT_PTR (void *)UINT32_MAX
#define I2S_CRUFT_LEN UINT32_MAX

    req.wordSize = MXC_I2S_WSIZE_WORD;
    req.sampleSize = MXC_I2S_SAMPLESIZE_TWENTYFOUR;
    req.bitsWord = 24;
    req.adjust = MXC_I2S_ADJUST_LEFT;
    req.justify = MXC_I2S_MSB_JUSTIFY;
    req.wsPolarity = MXC_I2S_POL_NORMAL;
    /* I2S Peripheral is in slave mode - no need to set clkdiv */
    req.channelMode = MXC_I2S_EXTERNAL_SCK_EXTERNAL_WS;
    req.stereoMode = MXC_I2S_STEREO;

    req.bitOrder = MXC_I2S_MSB_FIRST;

    req.rawData = NULL;
    req.txData = I2S_CRUFT_PTR;
    req.rxData = I2S_CRUFT_PTR;
    req.length = I2S_CRUFT_LEN;

    if (MXC_I2S_Init(&req) != E_NO_ERROR)
        blink_halt("Error initializing I2S");
    else
        printf("I2S initialized successfully \n");
}

void dma_handler(void)
{
    dma_flag = 1;
    MXC_DMA_Handler();
}

void dma_init(void)
{
    MXC_NVIC_SetVector(DMA0_IRQn, dma_handler);
    MXC_NVIC_SetVector(DMA1_IRQn, dma_handler);
    NVIC_EnableIRQ(DMA0_IRQn);
    NVIC_EnableIRQ(DMA1_IRQn);
}






void dma_callback(int channel, int result)
{
    if (channel == dma_ch_rx) {
    	data_ready = 1;

        // Swap buffers for next DMA transfer
        if (rxBufPtr == i2s_rx_buffer) {
            rxBufPtr = i2s_rx_buffer + I2S_DMA_BUFFER_SIZE;
        } else {
            rxBufPtr = i2s_rx_buffer;
        }

        // Reconfigure RX DMA (without TX)
        MXC_DMA_ReleaseChannel(dma_ch_rx);
        dma_ch_rx = MXC_I2S_RXDMAConfig(rxBufPtr, I2S_DMA_BUFFER_SIZE * sizeof(i2s_rx_buffer[0]));
    }
}




void setup_audio_system(void) {
    i2c_init();
 
    codec_init();

    i2s_init();


    dma_init();

    MXC_I2S_RegisterDMACallback(dma_callback);
    dma_ch_rx = MXC_I2S_RXDMAConfig(i2s_rx_buffer, I2S_DMA_BUFFER_SIZE * sizeof(i2s_rx_buffer[0]));

}



#define NUM_OUTPUTS 7


const char keywords[NUM_OUTPUTS][10] = {"Cafe", "Car", "CityCenter", "ForestPath", "Home", "office", "park"};


typedef union {
    int32_t value;
    int8_t  bytes[4];
} Int32Union;






volatile uint32_t cnn_time; // Stopwatch

void fail(void)
{
  printf("\n*** FAIL ***\n\n");
  while (1);
}

void load_input(void)
{
	
    // Für jede der 311 Reihen:
    for (int z = 0; z < 311; z++) {
        // In jeder Reihe gibt es 10 Gruppen (je 4 Spalten), also 10 Kanäle.
        for (int i = 0; i < 10; i++) {
            Int32Union u;  // Union zur Zusammenführung von 4 int8_t-Werten in einen 32-Bit-Wert

            // Zusammenschalten der 4 benachbarten Werte (Spalten i*4 bis i*4+3) dieser Reihe
            u.bytes[0] = mfcc_q8[i * 4 + 0][z];
            u.bytes[1] = mfcc_q8[i * 4 + 1][z];
            u.bytes[2] = mfcc_q8[i * 4 + 2][z];
            u.bytes[3] = mfcc_q8[i * 4 + 3][z];

            // Speicherung des zusammengefügten 32-Bit-Wertes in mfcc_q32
            mfcc_q32[i][z] = u.value;
        }
    }

    // Array von Zeigern auf die statischen Input-Arrays
    uint32_t *dest_arrays[10] = {
        input_0,   // Kanal 0
        input_4,   // Kanal 1
        input_8,   // Kanal 2
        input_12,  // Kanal 3
        input_16,  // Kanal 4
        input_20,  // Kanal 5
        input_24,  // Kanal 6
        input_28,  // Kanal 7
        input_32,  // Kanal 8
        input_36   // Kanal 9
    };

    // Kopiere für jeden Kanal die 311 32-Bit-Werte von mfcc_q32 in das jeweilige Zielarray
    for (int channel = 0; channel < 10; channel++) {
        memcpy(dest_arrays[channel], mfcc_q32[channel], 311 * sizeof(uint32_t));
    }

    // Übertrage die Daten in den entsprechenden Speicherbereichen des CNN-Beschleunigers
    memcpy32((uint32_t *) 0x50400000, input_0, 311);
    memcpy32((uint32_t *) 0x50408000, input_4, 311);
    memcpy32((uint32_t *) 0x50410000, input_8, 311);
    memcpy32((uint32_t *) 0x50418000, input_12, 311);
    memcpy32((uint32_t *) 0x50800000, input_16, 311);
    memcpy32((uint32_t *) 0x50808000, input_20, 311);
    memcpy32((uint32_t *) 0x50810000, input_24, 311);
    memcpy32((uint32_t *) 0x50818000, input_28, 311);
    memcpy32((uint32_t *) 0x50c00000, input_32, 311);
    memcpy32((uint32_t *) 0x50c08000, input_36, 311);
}




// Classification layer:
static int32_t ml_data[CNN_NUM_OUTPUTS];
static q15_t ml_softmax[CNN_NUM_OUTPUTS];




void softmax_layer(void)
{
  cnn_unload((uint32_t *) ml_data);
  softmax_q17p14_q15((const q31_t *) ml_data, CNN_NUM_OUTPUTS, ml_softmax);
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////










int main(void)
{



  MXC_ICC_Enable(MXC_ICC0); // Enable cache

  // Switch to 100 MHz clock
  MXC_SYS_Clock_Select(MXC_SYS_CLOCK_IPO);
  SystemCoreClockUpdate();

  printf("Waiting...\n");

  // DO NOT DELETE THIS LINE:
  MXC_Delay(SEC(2)); // Let debugger interrupt if needed
  gpio_toggle_init();

  // Enable peripheral, enable CNN interrupt, turn on CNN clock
  // CNN clock: APB (50 MHz) div 1
  cnn_enable(MXC_S_GCR_PCLKDIV_CNNCLKSEL_PCLK, MXC_S_GCR_PCLKDIV_CNNCLKDIV_DIV1);


  printf("\n*** CNN Inference Test FINAL NORM ***\n");
  initMFCCInstance();
  cnn_init(); // Bring state machine into consistent state
  cnn_load_weights(); // Load kernels
  cnn_load_bias();
  cnn_configure(); // Configure state machine
              // Ringbuffer-Variable erstellen
  ringbuffer_init(&rb);     // Initialisierung aufrufen

  setup_audio_system();
  printf("Loop running....................\n");
  MXC_Delay(500000); // Warte 500ms, um die CPU nicht zu überlasten.

  while (1){

while( block_recording == 0) // Wird auf 1 gesetzt sobald genug daten für CNN-Berechnung empfangen wurden
{

	  if (data_ready == 1)
	  {
			 MXC_GPIO_OutSet(TOGGLE_GPIO_PORT, TOGGLE_GPIO_PIN); // GPIO P1_6 HIGH setzen
	        // Daten werden On the fly verarbeitet
	    	for (int i =0; i < I2S_DMA_BUFFER_SIZE ; i++)
	    	{
	    		zwischenpuffer[i]= rxBufPtr[i];
	    	}

			  process_audio_data();

			  MXC_GPIO_OutClr(TOGGLE_GPIO_PORT, TOGGLE_GPIO_PIN); // GPIO P1_6 LOW Setzen
		  data_ready = 0;

	  }
}


// Dieser Teil wird nur gestartet wenn alle daten recorded sind


if (calculate_cnn== 1){

//	for (int i = 0; i<20; i++)
//	{
//
//	printf(" mfcc_q8 Number:%d = %d \n",i, mfcc_q8[i][0]);
//	}
//	printf("Calc_cnn \n");
block_recording = 1;
  load_input(); // Load data input

  cnn_start(); // Start CNN processing

  while (cnn_time == 0)
    MXC_LP_EnterSleepMode(); // Wait for CNN

 // if (check_output() != CNN_OK) fail();
  softmax_layer();

  float max = ml_data[0];
  int size = sizeof(ml_data) / sizeof(ml_data[0]); // Anzahl der Elemente im Array
  int index = 0; // Speichert die Position des größten Wertes

      for (int i = 1; i < size; i++) {
          if (ml_data[i] > max) {
              max = ml_data[i];
              index = i;
          }
      }
      printf("klasse %d \n", index);




// Kann Aktiviert werden um die Berechnungszeit anzuzeigen

//#ifdef CNN_INFERENCE_TIMER
//  printf("Approximate inference time: %u us\n\n", cnn_time);
//#endif



// Kann aktiviert werden, wenn die CNN Outputs angezeigt werden sollen
//  printf("Classification results:\n");
//  for (i = 0; i < CNN_NUM_OUTPUTS; i++) {
//    digs = (1000 * ml_softmax[i] + 0x4000) >> 15;
//    tens = digs % 10;
//    digs = digs / 10;
//    printf("[%7d] -> Class %d: %d.%d%%\n", ml_data[i], i, digs, tens);
//  }

  MXC_Delay(1000000); // Warte 500ms für neue audio
  skip_average = 0;
  calculate_cnn = 0;
  y = 0;
  ringbuffer_init(&rb);     // Buffer zurücksetzen

  block_recording = 0;
  x= 0;
}

  }
  return 0;
}

/*
  SUMMARY OF OPS
  Hardware: 27,633,984 ops (27,507,072 macc; 100,800 comp; 26,112 add; 0 mul; 0 bitwise)
    Layer 0 (conv1): 2,727,168 ops (2,713,600 macc; 13,568 comp; 0 add; 0 mul; 0 bitwise)
    Layer 1: 5,187,840 ops (5,160,960 macc; 26,880 comp; 0 add; 0 mul; 0 bitwise)
    Layer 2: 16,928,768 ops (16,875,520 macc; 53,248 comp; 0 add; 0 mul; 0 bitwise)
    Layer 3: 2,536,128 ops (2,506,752 macc; 3,264 comp; 26,112 add; 0 mul; 0 bitwise)
    Layer 4: 249,600 ops (245,760 macc; 3,840 comp; 0 add; 0 mul; 0 bitwise)
    Layer 5: 4,480 ops (4,480 macc; 0 comp; 0 add; 0 mul; 0 bitwise)

  RESOURCE USAGE
  Weight memory: 267,136 bytes out of 442,368 bytes total (60.4%)
  Bias memory:   551 bytes out of 2,048 bytes total (26.9%)
*/

