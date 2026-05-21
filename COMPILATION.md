### 🚀 Quick Install & Compilation

To run **Melody**, you need the official application package (**`Melody.zip`**), which contains the folder structure, assets, and data.

> [!IMPORTANT]
> * **macOS:** A pre-compiled Universal Binary (Intel/Apple Silicon) is already included in `Melody.zip`.
> * **Linux:** You **must** compile the binary from source and place it into the application folder (see instructions below).
#### 💻 System Requirements

This project requires modern processor architectures with hardware support for SIMD and CRC instructions:

| Architecture | Required Extensions (Minimum) | Notes |
| :--- | :--- | :--- |
| **ARM** | **ARMv8-A** (with NEON & CRC32)  | Apple Silicon (M1/M2/M3...), Raspberry Pi 4+ |
| **x86-64** | **AVX2** | Intel Haswell (2013), AMD Excavator (2015) or newer. |

> ⚠️ **Note:** Older processors lacking these extensions are not supported.

---

#### 🛠️ Compilation (From Source)

**1. Prerequisites**
> ##### macOS 
> `xcode-select --install` 
  
> ##### Linux (Ubuntu/Debian) 
> `sudo apt update && sudo apt install g++ make`

**2. Procedure**

1. Download and extract the **Source Code** archive.
2. Open a terminal and navigate to your extracted source folder:
    ```bash
    cd /path/to/Melody-source-x.y.z
    ```
3. Run the build command:
    ```bash
    make
    ```
    *Expected output ending with:* `✅ Build OK → build/Melody [OS/ARCH]`

**3. Finalization**

Once compiled, you must move the newly generated binary into the official Melody application folder (extracted from `Melody.zip`) so it can access the required data directories.

1. Move the binary to the destination release folder:
    ```bash
    cp build/Melody /path/to/your/extracted/Melody/build/Release/
    ```
2. Ensure the binary has proper execution permissions:
    ```bash
    chmod +x /path/to/your/extracted/Melody/build/Release/Melody
    ```
3. Run the application using the startup script located inside the main `Melody/` folder:
    * **macOS:** Double-click `*.command`.
    * **Linux:** Run `./*.sh` in your terminal.

---

#### ⚙️ Build Options (Make)

| Command | Action |
| :--- | :--- |
| `make` | Compile (only modified files) |
| `make rebuild` | Recompile everything from scratch |
| `make clean` | Delete the entire build/ folder |
| `make cleanobj` | Delete .o and .d files but keep the binary |
| `make info` | Display detected configuration |

>[!TIP]
> To compile while saving logs and errors to a file for debugging, run:
> ```bash
> make 2>&1 | tee build.log   # compile and save errors to a log
> cat build.log               # view the log
> ```