# Spatial Hash Grid - Collision Detection Demo

**CSD2183 Data Structures - Assignment 1**  
**Singapore Institute of Technology**

Interactive demonstration of spatial hash grid collision detection with real-time performance comparison between O(n) spatial hashing and O(n²) brute-force algorithms.

---

## Team Members

- **Choi Meng Yew** (2401822@sit.singaporetech.edu.sg)
- **Alyssa Cerrero Nicole Alejandro**
- **Chan Qi Ying**

---

## Project Overview

This application visualizes how spatial partitioning using hash tables reduces collision detection complexity from O(n²) to O(n) average case. Users can interactively adjust parameters and compare performance in real-time.

### Key Features

- **Real-time collision detection** between 10-2000 moving entities
- **Interactive comparison** between Spatial Hash and Brute Force algorithms
- **Visual grid representation** showing spatial partitioning
- **Performance metrics** (FPS, collision checks, active cells)
- **ImGui control panel** for parameter adjustment
- **Batched rendering** for efficient visualization

---

## Quick Start

### Option 1: Run Pre-built Executable (Easiest)

1. Navigate to `bin/Release/`
2. Double-click `SpatialHashDemo.exe`
3. Done. No compilation needed.

### Option 2: Build from Source

**Requirements:**

- Visual Studio 2026
- Windows 10/11
- C++17 or later

**Steps:**

1. Open `spatial-hash-grid.sln` in Visual Studio
2. Select **x64** platform and **Release** configuration
3. Build → Rebuild Solution (Ctrl+Shift+B)
4. Run → Start Without Debugging (Ctrl+F5)

## Troubleshooting

### "Cannot find SFML DLLs"

**Solution:** Copy DLLs from `vendor/SFML-2.6.2/bin/` to the executable directory

### "Platform x86 not supported"

**Solution:** Change platform to **x64** in Visual Studio toolbar (not x86)

### IntelliSense errors but code compiles

**Solution:** Project → Rescan Solution

### Build fails with linker errors

**Solution:**

1. Verify platform is set to x64
2. Check that `opengl32.lib` is in Additional Dependencies
3. Clean solution and rebuild

## Acknowledgments

- **SFML Team** - Graphics library
- **Omar Cornut** - ImGui library
- **Elias Daler** - ImGui-SFML integration

**Last Updated**: February 14, 2026
