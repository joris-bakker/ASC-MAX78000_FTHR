# -*- coding: utf-8 -*-
"""
Created on Thu Feb 20 10:03:10 2025

@author: Joris Bakker

Dieses Programm ist ein Dummy input mit einem RMS von 0,7. Dies ist um den Pegel mit dem MAX78000 abzustimmen.
"""
import os
import glob
import time
import numpy as np
import sounddevice as sd
from scipy.io import wavfile



# Parameter
fs = 48000             # Abtastrate (Samples pro Sekunde)
duration = 10        # Dauer des Signals in Sekunden
f = 200                # Frequenz in Hz


t = np.linspace(0, duration, int(fs * duration), endpoint=False)

# Sinussignal erzeugen
signal = np.sin(2 * np.pi * f * t)

# RMS berechnen
rms_value = np.sqrt(np.mean(signal**2))
print("RMS des Signals:", rms_value)

    
sd.play(signal, fs)
    
sd.wait()
    
print("Playback abgeschlossen.")

