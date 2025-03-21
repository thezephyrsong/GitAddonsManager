# GitAddonsManager

Git based World of Warcraft addon manager.

This application exclusively manage git repositories, addons not containing a git repositories will be ignored. All addons hosted on Gitlab, Github and likes are compatible but must be cloned through git (if you don't know what that means, you will need to redownload your addons through this app).

## Building on Linux

### Requirements:

- Qt6 Modules:
    - Core
    - Quick
    - QuickControls2
    - Concurrent
    - Network
    - Widgets
- libgit2

### build & install

```bash
cmake . -B build -DGitAddonsManager_GIT_DESCRIBE="$(git describe --tags --long)" -DCMAKE_BUILD_TYPE=Release --install-prefix="$(pwd)/release"
cmake --build build
cmake --install build
```

The executable is then found in `release/bin/`.
