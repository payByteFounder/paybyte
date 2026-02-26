# Building PayByte Core

This document describes how to build the PayByte Core reference implementation from source.

PayByte Core is derived from Bitcoin Core and Litecoin Core and follows a similar build process.

---

## System Requirements

### Minimum
- 2 GB RAM
- 5 GB disk space
- Linux, macOS, or Windows

### Recommended
- 4+ GB RAM
- SSD storage

---

## Linux Build Instructions (Debian/Ubuntu)

### 1. Install Dependencies

```bash
sudo apt update
sudo apt install -y \
  build-essential \
  libtool \
  autotools-dev \
  automake \
  pkg-config \
  bsdmainutils \
  python3 \
  libevent-dev \
  libboost-system-dev \
  libboost-filesystem-dev \
  libboost-test-dev \
  libboost-thread-dev \
  libminiupnpc-dev \
  libzmq3-dev
```

For wallet support (Berkeley DB):

```bash
sudo apt install -y libdb-dev libdb++-dev
```

For Qt GUI:

```bash
sudo apt install -y qtbase5-dev qttools5-dev qttools5-dev-tools
```

---

### 2. Build

From the repository root:

```bash
./autogen.sh
./configure
make -j$(nproc)
```

---

## Building With Qt GUI

```bash
./configure --with-gui=qt5
make -j$(nproc)
```

The GUI binary will be located at:

```
src/qt/paybyte-qt
```

---

## Windows Cross-Compilation (from Linux)

Install MinGW toolchain:

```bash
sudo apt install g++-mingw-w64-x86-64
```

Then build:

```bash
./configure --host=x86_64-w64-mingw32
make -j$(nproc)
```

The Windows executables will be located in:

```
src/paybyted.exe
src/qt/paybyte-qt.exe
```

---

## Running PayByte

Start daemon:

```bash
./src/paybyted -daemon
```

Check status:

```bash
./src/paybyte-cli getblockchaininfo
```

Start GUI:

```bash
./src/qt/paybyte-qt
```

---

## Data Directory

Default data directory locations:

- Linux: `~/.paybyte/`
- Windows: `%APPDATA%\PayByte\`
- macOS: `~/Library/Application Support/PayByte/`

---

## Troubleshooting

### Berkeley DB Errors

If you see:

```
libdb_cxx headers missing
```

Install:

```bash
sudo apt install libdb-dev libdb++-dev
```

### Missing Qt5Core

Install:

```bash
sudo apt install qtbase5-dev
```

---

## Notes

- The `master` branch may contain experimental code.
- Always verify consensus-related changes carefully.
- Do not run with `sudo` to avoid permission issues in the data directory.

For additional build details, refer to documentation in the `doc/` directory.