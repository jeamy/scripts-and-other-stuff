#!/bin/bash

# Überprüfen, ob beide Verzeichnisse übergeben wurden
if [[ -z "$1" || -z "$2" ]]; then
    echo "Bitte Eingabe- und Ausgabeverzeichnis angeben."
    echo "Verwendung: $0 /pfad/zum/eingabeverzeichnis /pfad/zum/ausgabeverzeichnis"
    exit 1
fi

# Eingabe- und Ausgabeverzeichnisse festlegen
input_dir="$1"
output_dir="$2"

# Überprüfen, ob das Eingabeverzeichnis existiert
if [[ ! -d "$input_dir" ]]; then
    echo "Das Eingabeverzeichnis existiert nicht: $input_dir"
    exit 1
fi

# Überprüfen, ob das Ausgabeverzeichnis existiert; wenn nicht, erstellen
if [[ ! -d "$output_dir" ]]; then
    echo "Das Ausgabeverzeichnis existiert nicht. Erstelle: $output_dir"
    mkdir -p "$output_dir"
fi

# Gehe durch jede .ts-Datei im Eingabeverzeichnis
for file in "$input_dir"/*.ts; do
    # Überprüfen, ob die Datei existiert
    if [[ -f "$file" ]]; then
        # Name der Ausgabedatei im Ausgabeverzeichnis festlegen
        output="$output_dir/$(basename "${file%.ts}.mp4")"
        
        # Konvertiere die TS-Datei in MP4 mit der gewünschten Auflösung
#        ffmpeg -i "$file" -threads 8 -vf scale=1920:-2 -c:v libx264 -crf 23 -preset medium -c:a aac -b:a 192k "$output"
# HQ        ffmpeg -i "$file" -vf scale=1920:-2 -c:v libx264 -b:v 8000k -maxrate 9000k -bufsize 18000k -c:a aac -b:a 256k "$output"
#        ffmpeg -i "$file" -vf scale=1920:-2 -c:v h264_nvenc -preset p4 -b:v 5000k -maxrate 6000k -bufsize 12000k -c:a aac -b:a 256k "$output"
        ffmpeg -y -hwaccel cuda -i "$file" -vf scale=1920:-2 -c:v h264_nvenc -b:v 5000k -maxrate 6000k -bufsize 12000k -c:a aac -b:a 256k "$output"
        
        echo "Konvertiert: $file -> $output"
    fi
done

echo "Konvertierung abgeschlossen!"
