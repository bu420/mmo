# ⋆˖⁺‧₊☽◯☾₊‧⁺˖⋆
Text-based MMORPG.

```sh
git clone https://github.com/bu420/mmo
cd mmo
make
bin/mmo
```

## How to Play
Download the client from Releases or build from source.

### Build client
```sh
git clone https://github.com/bu420/mmo
cd mmo
make client
bin/client
```

## Debug server with VSCode
To debug open the command palette and select `Debug: Select and Start Debugging` and select `(gdb) Debug`.

To debug macros, first run `make expand-macros` in the terminal.
Then go into build and set breakpoints in the new source files.
Open the command palette and select `Debug: Select and Start Debugging` and
select `(gdb) Debug Macros`.