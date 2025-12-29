#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Audio conversion tool - Convert various audio formats to OGG/Opus"""

import os
import sys
import subprocess
from pathlib import Path

# Configuration
INPUT_DIR = "music"
OUTPUT_DIR = "output"
BITRATE = "32k"  # Lower bitrate for ESP32 memory constraints
SUPPORTED_FORMATS = ['.mp3', '.wav', '.flac', '.m4a', '.aac', '.wma', '.ogg']

def get_ffmpeg_path():
    """Get FFmpeg path, prefer local portable version"""
    local_ffmpeg = Path("ffmpeg/bin/ffmpeg.exe")
    if local_ffmpeg.exists():
        return str(local_ffmpeg)
    return "ffmpeg"

def check_ffmpeg():
    """Check if FFmpeg is available"""
    ffmpeg_path = get_ffmpeg_path()
    
    try:
        subprocess.run([ffmpeg_path, '-version'],
                      capture_output=True,
                      check=True)
        
        if "bin" in ffmpeg_path.lower():
            print("[OK] Using local FFmpeg (portable)")
        else:
            print("[OK] Using system FFmpeg")
        return True
    except (subprocess.CalledProcessError, FileNotFoundError):
        print("[ERROR] FFmpeg not found!")
        print("\nSolution:")
        print("1. Run 'download-ffmpeg.bat' to auto-download (recommended)")
        print("2. Or manually install FFmpeg")
        return False

def convert_audio(input_file, output_file):
    """Convert audio to OGG/Opus format using FFmpeg"""
    ffmpeg_path = get_ffmpeg_path()
    
    cmd = [
        ffmpeg_path,
        '-i', str(input_file),
        '-c:a', 'libopus',
        '-b:a', BITRATE,
        '-ac', '1',  # Force mono (single channel)
        '-ar', '24000',  # Force 24kHz sample rate to match ESP32 output
        '-vbr', 'on',
        '-compression_level', '10',
        '-y',
        str(output_file)
    ]
    
    try:
        # Don't capture output to avoid encoding issues
        result = subprocess.run(cmd,
                              stdout=subprocess.DEVNULL,
                              stderr=subprocess.DEVNULL,
                              check=True)
        return True
    except subprocess.CalledProcessError:
        return False

def get_audio_files(directory):
    """Get all supported audio files in directory"""
    audio_files = []
    input_path = Path(directory)
    
    if not input_path.exists():
        return audio_files
    
    for file in input_path.iterdir():
        if file.is_file() and file.suffix.lower() in SUPPORTED_FORMATS:
            audio_files.append(file)
    
    return sorted(audio_files)

def format_size(size_bytes):
    """Format file size for display"""
    for unit in ['B', 'KB', 'MB', 'GB']:
        if size_bytes < 1024.0:
            return f"{size_bytes:.2f} {unit}"
        size_bytes /= 1024.0
    return f"{size_bytes:.2f} TB"

def main():
    """Main function"""
    print("=" * 60)
    print("  Audio Converter - MP3/WAV to OGG/Opus")
    print("=" * 60)
    print()
    
    # Check FFmpeg
    if not check_ffmpeg():
        print("\nProgram terminated")
        return 1
    
    # Create directories
    input_path = Path(INPUT_DIR)
    output_path = Path(OUTPUT_DIR)
    
    if not input_path.exists():
        input_path.mkdir(parents=True, exist_ok=True)
        print(f"[OK] Created input directory: {INPUT_DIR}")
    
    if not output_path.exists():
        output_path.mkdir(parents=True, exist_ok=True)
        print(f"[OK] Created output directory: {OUTPUT_DIR}")
    
    # Get audio files
    audio_files = get_audio_files(input_path)
    
    if not audio_files:
        print(f"\n[ERROR] No audio files found in '{INPUT_DIR}' directory")
        print(f"Supported formats: {', '.join(SUPPORTED_FORMATS)}")
        print("\nUsage:")
        print(f"  1. Put audio files into '{INPUT_DIR}' folder")
        print(f"  2. Run this script again")
        return 1
    
    print(f"\nFound {len(audio_files)} audio file(s)")
    print("-" * 60)
    
    # Convert all files
    success_count = 0
    failed_count = 0
    total_input_size = 0
    total_output_size = 0
    
    for i, input_file in enumerate(audio_files, 1):
        output_file = output_path / f"{input_file.stem}.ogg"
        
        print(f"\n[{i}/{len(audio_files)}] {input_file.name}")
        input_size = input_file.stat().st_size
        print(f"  Input:  {format_size(input_size)}")
        
        print(f"  Converting...", end='', flush=True)
        if convert_audio(input_file, output_file):
            output_size = output_file.stat().st_size
            print(f" Done!")
            print(f"  Output: {format_size(output_size)}")
            print(f"  [OK] -> {output_file.name}")
            
            success_count += 1
            total_input_size += input_size
            total_output_size += output_size
        else:
            print(f" Failed!")
            print(f"  [ERROR] Conversion failed")
            failed_count += 1
    
    # Show statistics
    print("\n" + "=" * 60)
    print("Conversion Complete!")
    print("=" * 60)
    print(f"Success: {success_count} file(s)")
    if failed_count > 0:
        print(f"Failed:  {failed_count} file(s)")
    
    if success_count > 0:
        print(f"\nTotal input size:  {format_size(total_input_size)}")
        print(f"Total output size: {format_size(total_output_size)}")
        compression_ratio = (1 - total_output_size / total_input_size) * 100
        print(f"Compression ratio: {compression_ratio:.1f}%")
        
        print(f"\n[OK] Converted files are in: {OUTPUT_DIR}/")
        print("\nNext steps:")
        print("  1. Copy .ogg files from 'output' folder")
        print("  2. To the root directory of SD card")
        print("  3. Insert SD card into ESP32 device")
        print("  4. Use voice commands:")
        print("     - 'List music on SD card'")
        print("     - 'Play xxx.ogg'")
        print("     - 'Stop playing music'")
    
    return 0 if failed_count == 0 else 1

if __name__ == "__main__":
    try:
        sys.exit(main())
    except KeyboardInterrupt:
        print("\n\nInterrupted by user")
        sys.exit(1)
    except Exception as e:
        print(f"\n[ERROR] {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
