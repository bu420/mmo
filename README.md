# ⋆˖⁺‧₊☽◯☾₊‧⁺˖⋆
Text-based MMORPG.

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
