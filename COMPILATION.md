### 🚀 Quick Install & Compilation

To run **Melody**, you need the official application package (**`Melody.zip`**), which contains the folder structure, assets, and data.

> [!IMPORTANT]
> * **macOS:** A pre-compiled Universal Binary (Intel/Apple Silicon) is already included in `Melody.zip`.
> * **Linux:** You **must** compile the binary from source and place it into the application folder (see instructions below).
> * **Windows:** You **must** compile the binary from source (MSVC) and place it into the application folder (see instructions below).

#### 💻 System Requirements

This project requires modern processor architectures with hardware support for SIMD and CRC instructions:

| Architecture | Required Extensions (Minimum) | Notes |
| :--- | :--- | :--- |
| **ARM** | **ARMv8-A** (with NEON & CRC32)  | Apple Silicon (M1/M2/M3...), Raspberry Pi 4+, Windows on ARM |
| **x86-64** | **AVX2** | Intel Haswell (2013), AMD Excavator (2015) or newer. |

> ⚠️ **Note:** Older processors lacking these extensions are not supported.

---

#### 🛠️ Compilation (From Source)

**1. Prerequisites**
> ##### macOS
> `xcode-select --install`

> ##### Linux (Ubuntu/Debian)
> `sudo apt update && sudo apt install g++ make`

> ##### Windows
> Install **Visual Studio Build Tools** (or full Visual Studio) with the **"Desktop development with C++"** workload — this provides `cl.exe`, `nmake`, and the Windows SDK (needed for Winsock).
> Download: https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio
>
> All Windows build commands below must be run from a **"Developer Command Prompt for VS"** or **"x64 Native Tools Command Prompt for VS"** (available from the Start Menu after installing Build Tools) — a regular `cmd.exe`/PowerShell window won't have `cl`/`nmake` on the `PATH`.

**2. Procedure**

1. Download and extract the **Source Code** archive.
2. Open a terminal and navigate to your extracted source folder:

    **macOS / Linux:**
    ```bash
    cd /path/to/Melody-source-x.y.z
    ```

    **Windows (Developer Command Prompt):**
    ```bat
    cd \path\to\Melody-source-x.y.z
    ```

3. Run the build command:

    **macOS / Linux:**
    ```bash
    make
    ```
    *Expected output ending with:* `✅ Build OK → build/Melody [OS/ARCH]`

    **Windows:**
    ```bat
    nmake /f Makefile.win
    ```
    *Expected output ending with:* `Build OK -> build\Melody.exe [Windows/MSVC]`

**3. Finalization**

Once compiled, you must move the newly generated binary into the official Melody application folder (extracted from `Melody.zip`) so it can access the required data directories.

1. Move the binary to the destination release folder:

    **macOS / Linux:**
    ```bash
    cp build/Melody /path/to/your/extracted/Melody/build/Release/
    ```

    **Windows:**
    ```bat
    copy build\Melody.exe \path\to\your\extracted\Melody\build\Release\
    ```

2. Ensure the binary has proper execution permissions:

    **macOS / Linux:**
    ```bash
    chmod +x /path/to/your/extracted/Melody/build/Release/Melody
    ```

    **Windows:** no action needed — `.exe` files are executable by default.

3. Run the application using the startup script located inside the main `Melody/` folder:
    * **macOS:** Double-click `*.command`.
    * **Linux:** Run `./*.sh` in your terminal.
    * **Windows:** Double-click `*.bat` (or `*.cmd`).

---

#### ⚙️ Build Options

**macOS / Linux (Make)**

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

**Windows (NMake)**

| Command | Action |
| :--- | :--- |
| `nmake /f Makefile.win` | Compile (only modified files) |
| `nmake /f Makefile.win rebuild` | Recompile everything from scratch |
| `nmake /f Makefile.win clean` | Delete the entire build\ folder |
| `nmake /f Makefile.win info` | Display detected configuration |

>[!TIP]
> To compile while saving logs and errors to a file for debugging, run:
> ```bat
> nmake /f Makefile.win > build.log 2>&1
> type build.log
> ```
