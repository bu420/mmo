# mmo
## About
A [TUI](https://en.wikipedia.org/wiki/Text-based_user_interface) [MMORPG](https://en.wikipedia.org/wiki/Massively_multiplayer_online_role-playing_game) for [POSIX](https://en.wikipedia.org/wiki/POSIX)-compliant systems.
Building from source requires C23 and CMake >=3.31.

*Tip: Use cURL or Wget to download the client:*
```sh
curl -OL https://github.com/bu420/mmo/releases/download/0.1.1/mmo-client-v0.1.1
mmo-client-v0.1.1
```

## Technical details
If you see `[[nodiscard]] int` it means the function will return either 0 (success) or -1 (failure).
A failure means something horrible happened. Example: malloc() returned NULL.

Anything that ends with `_view_t` must not be written to, even though it is possible.

All non-null pointer parameters must be checked for NULL in an assert().

Keep preprocessor macros to the minimum. Exception: generic type code generation.
