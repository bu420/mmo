# ⋆˖⁺‧₊☽◯☾₊‧⁺˖⋆
Text-based MMORPG.

## Download and play (Ubuntu)
```sh
curl -OL https://github.com/bu420/mmo/releases/download/0.1.0/mmo-client-v0.1.0-ubuntu
mmo-client-v0.1.0-ubuntu
```

## Download and run server
```sh
git clone https://github.com/bu420/mmo
cd mmo
make
build/release/mmo
```

## Debug (Visual Studio Code)
- Open Command Palette (Ctrl+Shift+P).
- Enter `Debug: Select and Start Debugging`.
- Select `Debug`.

To debug macros:
- Enter `make expand` in your terminal.
- ./build/debug_expanded/src will contain the source files after the preprocessor stage. Set breakpoints in these files.
- Open Command Palette (Ctrl+Shift+P).
- Enter `Debug: Select and Start Debugging`.
- Select `Debug with expanded macros`.
