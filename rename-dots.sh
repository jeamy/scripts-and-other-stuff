#!/bin/bash

# Function to log messages
log() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1"
}

find . -type f | while read -r file; do
    # Skip if it's a directory
    if [ -d "$file" ]; then
        log "Skipping directory: $file"
        continue
    fi
    
    # Get directory path and filename separately
    dirpath=$(dirname "$file")
    basename=$(basename "$file")
    
    # Get the file extension (everything after the last dot)
    extension="${basename##*.}"
    # Get the filename without the extension
    filename="${basename%.*}"
    
    # Replace all dots with spaces except the last one (before extension)
    new_filename=$(echo "$filename" | sed 's/\./ /g')
    
    # Combine new filename with extension
    new_basename="${new_filename}.${extension}"
    new_full_path="${dirpath}/${new_basename}"
    
    # Skip if the filename is already in desired format
    if [ "$file" = "$new_full_path" ]; then
        log "File already in correct format: $file"
        continue
    fi
    
    # Attempt to rename the file
    if mv "$file" "$new_full_path"; then
        log "Renamed: '$file' -> '$new_full_path'"
    else
        log "Error: Failed to rename '$file'"
    fi
done

log "Processing complete"