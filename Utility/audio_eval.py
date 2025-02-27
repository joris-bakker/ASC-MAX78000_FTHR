"""
Dieses Programm dient der Evaluation des MAX78000 mit Audiodateien. 
Die Evaluierung geschieht klasse für klasse. Die Outputs  des MAX78000 werden über die Serielle Schnittstelle ausgegeben. Baudrate 115200

"""



import os
import glob
import time
import numpy as np

import sounddevice as sd
from scipy.io import wavfile
counter = 1

folder_path = r"D:\KerasTUT2017_16khz_FULL\Testing\city_center"


wav_files = glob.glob(os.path.join(folder_path, "*.wav"))

# Für jede gefundene Datei:
for file_path in wav_files:
    print(f"Verarbeite Datei: {file_path}")
 
    sample_rate, data = wavfile.read(file_path)

    mean_val = np.mean(abs(data))
    print("RMS des Signals:", mean_val)
    scale = 2
    data = data*scale


    print(f"Spiele das normalisierte  Audio Nr: {counter} ")
    sd.play(data, sample_rate)
    sd.wait()  

    counter = counter +1

    time.sleep(2)

print("Klasse abgeschlossen.\n")