#!/bin/bash

# Default values
EPISODES_FILE=""
INPUT_FILE=""
TEST_MODE="false"

# Show help
show_help() {
    echo "Usage: $0 [options] <episode_list_file> <episodes_mapping_file>"
    echo
    echo "Organize TV episode files into season folders and rename them with season/episode numbers"
    echo
    echo "Options:"
    echo "  --test       Test mode - show what would be done without moving files"
    echo "  -h, --help   Show this help message"
    echo
    echo "Arguments:"
    echo "  episode_list_file    File containing list of episode filenames to process"
    echo "  episodes_mapping_file File containing episode mapping with season/episode numbers"
    echo
    echo "Example:"
    echo "  $0 --test episode_list.txt episodes.txt"
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --test)
            TEST_MODE="true"
            shift
            ;;
        -h|--help)
            show_help
            exit 0
            ;;
        *)
            if [ -z "$INPUT_FILE" ]; then
                INPUT_FILE="$1"
            elif [ -z "$EPISODES_FILE" ]; then
                EPISODES_FILE="$1"
            else
                echo "Error: Too many arguments" >&2
                show_help
                exit 1
            fi
            shift
            ;;
    esac
done

# Check required arguments
if [ -z "$INPUT_FILE" ] || [ -z "$EPISODES_FILE" ]; then
    echo "Error: Missing required arguments" >&2
    show_help
    exit 1
fi

# Check if files exist
if [ ! -f "$INPUT_FILE" ]; then
    echo "Error: Episode list file not found: $INPUT_FILE" >&2
    exit 1
fi

if [ ! -f "$EPISODES_FILE" ]; then
    echo "Error: Episodes mapping file not found: $EPISODES_FILE" >&2
    exit 1
fi

# Function to format season/episode number with leading zeros
format_number() {
    printf "%02d" "$1"
}

# Function to clean filename
clean_filename() {
    local filename="$1"
    # Remove special characters and replace spaces with underscores
    echo "$filename" | sed 's/[^a-zA-Z0-9äöüÄÖÜß ]/_/g' | sed 's/__*/_/g'
}

# Function to get season and episode number for a file
get_season_episode() {
    local file="$1"
    local basename=$(basename "$file")
    local name_without_ext="${basename%.mp4}"
    local found=false
    local line_num=0
    
    echo "Debug: Looking for title: '$name_without_ext'" >&2
    
    # Read episodes.txt line by line to find the title
    while IFS= read -r line || [[ -n "$line" ]]; do
        ((line_num++))
        
        # If we find the episode title
        if [[ "$line" == "$name_without_ext" ]]; then
            # Get episode number (1 line above)
            local episode=$(sed -n "$((line_num-1))p" "$EPISODES_FILE")
            # Get season number (2 lines above) and remove the dot
            local season=$(sed -n "$((line_num-2))p" "$EPISODES_FILE" | sed 's/\.//')
            
            echo "Debug: Found match at line $line_num! Season $season Episode $episode" >&2
            
            # Pad episode number with leading zero if needed
            if [[ ${#episode} -eq 1 ]]; then
                episode="0$episode"
            fi
            echo "$season $episode"
            return 0
        fi
    done < "$EPISODES_FILE"
    
    echo "Warning: Could not find episode: $name_without_ext" >&2
    echo "0 0"
    return 1
}

# Function to process episodes
process_episodes() {
    # Array to store not found episodes
    declare -a not_found_episodes
    
    while read -r filename || [[ -n "$filename" ]]; do
        [ -z "$filename" ] && continue
        
        echo "Processing: $filename" >&2
        
        # Get season and episode numbers
        read season_num episode_num <<< $(get_season_episode "$filename")
        
        if [ "$season_num" = "0" ]; then
            echo "$filename -> NOT FOUND IN MAPPING"
            not_found_episodes+=("$filename")
            continue
        fi
        
        # Create season directory if it doesn't exist
        season_dir="Season $season_num"
        if [ "$TEST_MODE" = "false" ]; then
            mkdir -p "$season_dir"
        fi
        
        # Create new filename
        new_filename="S${season_num}E${episode_num}-${filename}"
        
        # Move file
        if [ "$TEST_MODE" = "true" ]; then
            echo "$filename -> $season_dir/$new_filename"
        else
            if [ ! -f "$filename" ]; then
                echo "Warning: Source file not found: $filename" >&2
                continue
            fi
            mv "$filename" "$season_dir/$new_filename"
        fi
    done < "$INPUT_FILE"
    
    # Print summary of not found episodes
    if [ ${#not_found_episodes[@]} -gt 0 ]; then
        echo "===============================
The following episodes were not found in the mapping:"
        printf '%s\n' "${not_found_episodes[@]}"
        echo "==============================="
    fi
}

# Main execution
if [ "$TEST_MODE" = "true" ]; then
    echo "TEST MODE: Showing what would be done"
fi

echo "===============================
Original Filename -> New Filename
-------------------------------"

process_episodes

echo "==============================="
if [ "$TEST_MODE" = "true" ]; then
    echo "Test complete. No files were moved."
fi