 Quantization und ai8x-synthesis (C-Code Generator for MAX78000)
 
## Script_Remarks
 
 Die Verschiedenen Skripte werden mit Tools aus dem Repository ai8x-synthesis
  durchgeführt. Dafür muss ein virtual python environment mit den verschiedenen Requirements eingerichtet werden. 
 
 
# Quantization

Für das Skript werden benötigt:
- Checkpoint File, welches im Ordner ai8x-synthesis eingefügt werden muss
- man muss sich im ai8x-synthesis Repository befinden


# Ai8x-synthesis (C-Code Generator)

Benötigt für die Synthese:
- Quantisierter Checkpoint file
- Testsample für input des CNN, generiert aus PyTorch Model (muss im ai8x-synthese Ordner "Test eingefügt werden")
- Yaml Description file (Model Name muss mit dem Modellname im Checkpoint file übereinstimmen)


