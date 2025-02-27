import numpy as np
import os

class Normalize:
    """
    Normalisiert den Input auf den Wertebereich [-128, +127].
    Es wird angenommen, dass der Input im Bereich [0,1] liegt.
    """
    def __init__(self, act_mode_8bit=True):
        self.act_mode_8bit = act_mode_8bit

    def __call__(self, img):
        if self.act_mode_8bit:
            # Entspricht: img.sub(0.5).mul(256.).round().clamp(min=-128, max=127)
            norm_img = np.rint((img - 0.5) * 256)
            norm_img = np.clip(norm_img, -128, 127)
            return norm_img.astype(np.int8)
        else:
            norm_img = np.rint((img - 0.5) * 256)
            norm_img = np.clip(norm_img, -128, 127) / 128.
            return norm_img.astype(np.float32)

klasse = "cafe"
base_input = r"C:\Users\student1\Desktop\test_dataset"
base_output = r"C:\Users\student1\Desktop\headers_eval"

input_folder = os.path.join(base_input, klasse)
output_folder = os.path.join(base_output, klasse)
os.makedirs(output_folder, exist_ok=True)
counter = 1
normalizer = Normalize(act_mode_8bit=True)

# Iteriere über alle Dateien im input_folder
for filename in os.listdir(input_folder):
    if filename.endswith(".npy"):
        input_path = os.path.join(input_folder, filename)
        
        # --- NumPy-Array laden ---
        array = np.load(input_path)
        
        # Optional: Prüfen der Array-Größe (hier 40x311)
        if array.shape != (40, 311):
            print(f"Warnung: {filename} hat unerwartete Dimensionen {array.shape}. Diese Datei wird übersprungen.")
            continue
        
        # --- Normalisierung anwenden ---
        norm_array = normalizer(array)
        
        # --- Header-Datei erzeugen ---
        # Erzeuge einen nummerierten Header-Dateinamen (z.B. 1.h, 2.h, ...)
        header_filename = os.path.join(output_folder, f"{counter}.h")
        
        # Erzeuge einen eindeutigen Header-Guard (hier z.B. ARRAY_DATA_1_H, ARRAY_DATA_2_H, ...)
        header_guard = f"ARRAY_DATA_{counter}_H"
        
        with open(header_filename, "w") as f:
            f.write(f"#ifndef {header_guard}\n")
            f.write(f"#define {header_guard}\n\n")
            f.write("#include <stdint.h>\n\n")
            f.write(f"int8_t array{counter}[40][311] = {{\n")
            for row in norm_array:
                row_str = ", ".join(str(int(val)) for val in row)
                f.write("    {" + row_str + "},\n")
            f.write("};\n\n")
            f.write(f"#endif // {header_guard}\n")
        
        print(f"Header-Datei wurde erfolgreich erstellt: {header_filename}")
        counter += 1