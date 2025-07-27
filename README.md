# Pixel Configuration Comparison Tool

This C++ program compares the "PixelConfig" sections of two JSON files and identifies which bytes have different bits and at which bit locations.

## Files

- `pixel_config_compare.cpp` - Main C++ source code
- `Makefile` - Build configuration
- `masks/maskN.srvl` - First JSON file to compare
- `masks/mask256_511_25_1.srvl` - Second JSON file to compare

## Dependencies

- C++17 compatible compiler
- nlohmann/json library (version 3.2.0 or higher)

## Building

1. Install nlohmann/json library:
   ```bash
   # Ubuntu/Debian
   sudo apt-get install nlohmann-json3-dev
   
   # Or install via vcpkg
   vcpkg install nlohmann-json
   ```

2. Build the program:
   ```bash
   make
   ```

## Usage

Run the program with two JSON files as arguments:
```bash
./pixel_config_compare <file1> <file2>
```

Example:
```bash
./pixel_config_compare masks/maskN.srvl masks/mask256_511_25_1.srvl
```

The program will:
1. Read both JSON files specified as arguments
2. Extract all "PixelConfig" values
3. Decode the base64-encoded strings to bytes
4. Compare corresponding bytes and identify bit differences
5. Output detailed information about which bytes differ and which specific bits are different

## Output Format

The program outputs:
- Number of PixelConfig entries found in each file
- For each entry: byte-by-byte comparison showing:
  - Byte index
  - Hexadecimal values of both bytes
  - Specific bit positions that differ (0-7, where 0 is least significant bit)

## Example Output

```
Found 4 PixelConfig entries in maskN.srvl
Found 4 PixelConfig entries in mask256_511_25_1.srvl

=== Comparing PixelConfig entry 1 ===
Decoded 65536 bytes from maskN.srvl
Decoded 65536 bytes from mask256_511_25_1.srvl

Differences found:
  Byte    0: 0x1f vs 0x1e (bits 0 differ)
  Byte    1: 0x13 vs 0x12 (bits 0 differ)
  Byte    2: 0x03 vs 0x02 (bits 0 differ)
  ...
```
