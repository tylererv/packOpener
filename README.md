# packOpener

`packOpener` is a pure C++ desktop card-pack opening game built with CMake, raylib, and raylib-cpp.

The first version focuses on the core loop required for the C++ OOP final assignment: open fantasy card packs, collect cards, sell cards for cash, buy stronger packs, configure auto-sell rules, and save progress locally.

## Build

```sh
cmake -S . -B build
cmake --build build
./build/packOpener
```

CMake downloads raylib and raylib-cpp through `FetchContent` during configuration if they are not already available in the build cache.

## Controls

- Use the buttons at the bottom to switch between Shop, Open, Inventory, and Settings.
- Open free starter packs to earn cards and cash.
- Sell individual cards or sell duplicates from the Inventory screen.
- Configure rarity/value auto-sell rules from Settings.

## Assignment Concepts

The implementation intentionally demonstrates inheritance, constructors, destructors, static class data, const class data, virtual methods, overloading, operators, non-inheritance class relationships, standalone functions with default arguments, dynamic memory management, static arrays, dynamic arrays, recursion, linked lists, typecasting, enumerations, streams/files, visualization, and multifile organization.

Card and pack content lives in simple pipe-delimited text files under `data/`. Progress is saved with raylib's cross-platform storage API.

See `docs/contentGuide.md` for instructions on adding new cards, creating packs, and controlling pull chances.

For a deeper explanation of the codebase, read:

- `docs/fileReference.md`
- `docs/runtimeFlow.md`
- `docs/classArchitecture.md`
- `docs/dataAndPersistence.md`
