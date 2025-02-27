# ASC-MAX78000_FTHR  
Repository zur Implementierung von Acoustic Scene Detection auf dem MAX78000 FTHR  

## Inhalt des Repositories  

Dieses Repository enthält alle notwendigen Dateien und Skripte zur Umsetzung des Projekts, einschließlich:  

- PyTorch-Modell  
- Keras-Modell  
- Generierte Checkpoint-Dateien  
- Skripte für die **Quantisierung** und **ai8x-Synthese** (C-Code-Generator für den MAX78000)  
- Vollständiger C-Code 
- Zusätzlicher Utility-Code, der im Projekt verwendet wurde 

---


## **Workflow**  

Der in dieser Arbeit verwendete Workflow wird hier Schritt für Schritt erläutert. Dabei werden verschiedene bestehende **Tutorials** und Repositories genutzt.  

### **Übersicht**  

1. **Training des neuronalen Netzes in Python**  
2. **Quantisierung des Modells**  
3. **Transformation in C-Code für den MAX78000**  

---

## **1. Training des CNN**  

### **Schritte**  

1. **Laden und Einrichten der Umgebung**  
   - Klonen des GitHub-Repositories **MAX7800x-Jupyter-training**  
   - Erstellen eines Environments mit den benötigten Abhängigkeiten  
   - Repository: [MAX7800x-Jupyter-training](https://github.com/InES-HPMM/MAX7800x-Jupyter-training)  

2. **Training des CNN**  
   - Das Training erfolgt in einem **Jupyter Notebook** innerhalb des geklonten Repositories.  
   - Das Modell im Ordner `/PyTorch-Models` wird mit diesem Workflow trainiert.  
   - Spezielle Funktionen zur Generierung von **Checkpoint-Dateien** sind enthalten, welche für die **ai8x-Synthese** (C-Code-Generierung) notwendig sind.  

3. **Wichtige Hinweise zur Normalisierung**  
   - Der Datensatz wird mit der Funktion `ai8x.normalize()` normalisiert.  
   - Die Argumente von `ai8x.normalize()` können mit `ai8x.set_device()` angepasst werden.    

4. **Kompatibilität mit MAX78000**  
   - Alle PyTorch-Layer sollten in **ai8x-Layer** umgewandelt werden, z. B. `ai8x.Conv1D()`.
   - ai8x.py unterstützt die Fusion von Operationen wie zum Beispiel ai8x.FusedConv1dReLu(), und hilft bei der effizienten Nutzung der 	Hardware (KI-Beschleuniger) 

5. **Speicherung des Modells**  
   - Nach dem Training wird das Modell als **Checkpoint-Datei** gespeichert.  
   - Diese Datei ist spezifisch für den MAX78000 und unterscheidet sich von einem regulären PyTorch-Modell.  

6. **Erstellung eines Test-Samples**  
   - Ein Datensatz-Sample wird als **NumPy-Array** gespeichert.  
   - Dieses Sample wird für die C-Code-Generierung benötigt und muss im Verzeichnis `ai8x-synthesis/tests/` gespeichert werden.  

Nach diesen Schritten kann das Modell **quantisiert** werden.  

---

## **2. Quantisierung**  

### **Schritte**  

1. **Laden und Einrichten des Environments**  
   - Klonen des GitHub-Repositories **ai8x-synthesis**  
   - Erstellen eines Environments mit den benötigten Abhängigkeiten  
   - Repository: [ai8x-synthesis](https://github.com/analogdevicesinc/ai8x-synthesis)  

2. **Durchführen der Quantisierung**  
   - Nach dem Aktivieren des Environments kann das Skript zur Quantisierung ausgeführt werden.  
   - Das notwendige Skript befindet sich in diesem Repository unter `Scripts/quantization_script.txt`.  
   - Die **Checkpoint-Datei** ist erforderlich und muss im Ordner `ai8x-synthesis` abgelegt sein.  

Nach der Quantisierung kann die **C-Code-Generierung** durchgeführt werden.  

---

## **3. C-Code-Generierung**  

Die Umwandlung des quantisierten Modells in **C-Code** erfolgt ebenfalls mit dem **ai8x-synthesis** Repository.  

### **Notwendige Komponenten**  

1. **NumPy-Testsample**  
2. **Quantisierte Checkpoint-Datei**  
3. **YAML-Netzwerkbeschreibung**  

Eine detaillierte Erklärung zur YAML-Netzwerkbeschreibung findet sich hier:  
[YAML Quickstart Guide](https://github.com/analogdevicesinc/MaximAI_Documentation/blob/main/Guides/YAML%20Quickstart.md)  

### **C-Code Generierungsskript**  

Ein Beispiel-Skript zur C-Code-Generierung:  

```bash
python ai8xize.py --test-dir CNN_example --prefix ASC_CNN --checkpoint-file Generated-Checkpoint-Files/1dcnn_final_checkpoint_q8.pth.tar --config-file Yaml-Network-Description/classifier_final.yaml --device MAX78000 --timer 0 --display-checkpoint --verbose --softmax --device MAX78000 --board-name FTHR_RevA --overwrite
```

⚠ **Hinweis:**  
Dieses Skript dient nur als Beispiel! Die genauen Befehle können je nach Modell variieren.  

Für eine detaillierte Beschreibung der **C-Code-Generierung** siehe das README des **ai8x-synthesis** Repositories, Abschnitt **"Network Loader (AI8Xize)"**.  

---

## **Zusätzliche Ressourcen**  

- [MAX7800x-Jupyter-training Repository](https://github.com/InES-HPMM/MAX7800x-Jupyter-training)  
- [ai8x-synthesis Repository](https://github.com/analogdevicesinc/ai8x-synthesis)  
- [MaximAI Dokumentation](https://github.com/analogdevicesinc/MaximAI_Documentation/blob/main/Guides/YAML%20Quickstart.md)  

---
