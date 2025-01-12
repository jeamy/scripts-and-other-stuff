# scripts-and-other-stuff

A collection of utility scripts for file management and organization.

## Scripts

### rename-dots.sh

A bash script that replaces dots with spaces in filenames, except for the file extension.

**Usage:**
```bash
./rename-dots.sh
```

**Features:**
- Recursively processes all files in the current directory and subdirectories
- Preserves file extensions
- Logs all operations with timestamps
- Skips files that are already in the correct format
- Handles error cases gracefully

**Example:**
```
Before: The.Good.Show.S01E01.mp4
After:  The Good Show S01E01.mp4
```

### sort-episodes.sh

A bash script that organizes TV show episodes into season folders and renames them according to a provided mapping file.

**Usage:**
```bash
./sort-episodes.sh [options] <episode_list_file> <episodes_mapping_file>
```

**Options:**
- `--test`: Test mode - shows what would be done without actually moving files
- `-h, --help`: Show help message

**Required Arguments:**
1. `episode_list_file`: File containing list of episode filenames to process
2. `episodes_mapping_file`: File containing episode mapping with season/episode numbers

**Features:**
- Creates season folders automatically
- Renames episodes with proper season and episode numbers
- Test mode for safe verification before actual file operations
- Detailed error handling and validation
- Preserves original file extensions

**Example:**
```bash
# Test mode example
./sort-episodes.sh --test episode_list.txt episodes.txt

# Actual execution
./sort-episodes.sh episode_list.txt episodes.txt
```

**Input File Formats:**

*episode_list.txt* (one filename per line):
```
Show.Name.Episode.1.mp4
Show.Name.Episode.2.mp4
```

*episodes_mapping_file* (format: filename|season|episode):
```
Show.Name.Episode.1|1|1
Show.Name.Episode.2|1|2
```

### convert_ts_to_mp4.sh

A bash script that converts .ts video files to .mp4 format using NVIDIA GPU acceleration (NVENC) for high-performance encoding.

**Requirements:**
- FFmpeg with NVIDIA CUDA and NVENC support
- NVIDIA GPU with NVENC support
- NVIDIA drivers installed

**Usage:**
```bash
./convert_ts_to_mp4.sh <input_directory> <output_directory>
```

**Features:**
- Batch converts all .ts files in the input directory
- Uses NVIDIA GPU acceleration for faster encoding
- High-quality output with 1080p resolution (1920x-2)
- Optimized video bitrate settings (5000k-6000k)
- High-quality audio encoding (AAC 256k)
- Creates output directory if it doesn't exist
- Preserves original filenames (just changes extension)

**Example:**
```bash
./convert_ts_to_mp4.sh /path/to/ts/files /path/to/output/mp4s
```

**Video Encoding Settings:**
- Resolution: 1920x-2 (maintains aspect ratio)
- Video Codec: H.264 (NVENC)
- Video Bitrate: 5000k (max 6000k)
- Audio Codec: AAC
- Audio Bitrate: 256k
- Hardware Acceleration: CUDA
