### 🚀 Quick Install

Download `Melody.zip`.
Extract the archive.

>[note]
>The included binary is a Universal Binary (compatible with Intel & Apple Silicon) for macOS.
>Linux users: Compilation is required for your architecture (see section below).

#### 🛠️ Compilation (From Source)

 **- Prerequisites**
> ##### macOS `xcode-select --install` 
  
> ##### Linux (Ubuntu/Debian) `sudo apt install g++ make`

**- Procedure**

Download and extract the source code.

Open a terminal and run the build:
```
cd /path/to/Melody-x.y.z       # navigate to the folder
make                            # compile
```
Expected output:
✅ Build OK → build/Melody  [Darwin/arm64]

The generated binary is located in the `build/` folder.

**- Finalization**

Move the binary to `Melody/build/Release/`.
Check execution permissions.
Run a `.command` script.

#### ⚙️ Build Options (Make)
| Command | Action |
| :--- | :--- |
| `make` | Compile (only modified files) |
| `make rebuild` | Recompile everything from scratch |
| `make clean` | Delete the entire build/ folder |
| `make cleanobj` | Delete .o and .d files but keep the binary |
| `make info` | Display detected configuration |

>[!Tip]
```
make 2>&1 | tee build.log   	# compile and save errors to a log
cat build.log               	# view the log
```
