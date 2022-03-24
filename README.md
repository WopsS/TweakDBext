# TweakDBext

**Important**: This mod is not supported anymore! Use [TweakXL](https://github.com/psiberx/cp2077-tweak-xl) instead.

An plugin that load custom TweakDBs for REDengine 4 ([Cyberpunk 2077](https://www.cyberpunk.net)).

## How to install

1. Get the latest [RED4ext](https://github.com/WopsS/RED4ext).
2. Compile the poject or download the latest release / artifacts.
3. Copy the build artifacts inside `<game_root>/red4ext/plugins/tweakdbext` (or `<game_root>/red4ext/plugins`) directory.
4. (Optional) Add TweakDB files inside `<game_root>/r6/tweakdbs` directory.
5. Run the game.

## Frequently asked question

- **How to create a new TweakDB file?**
  - A: To create a new TweakDB file you need [WolvenKit](https://github.com/WolvenKit/WolvenKit).

- **Where should I place the TweakDB files?**
  - A: The TweakDB file should be placed in `game_root/r6/tweakdbs`. You can place them in the directory tweakdbs (e.g. `r6/tweakdbs/my_db.bin`) or create a sub-directory for you mod (e.g. `r6/tweakdbs/wopss_dbs/my_db.bin`).

## Build instructions

1. Download and install [Visual Studio 2019 Community Edition](https://www.visualstudio.com/) or a higher version.
2. Download and extract the content of [Premake 5](https://github.com/premake/premake-core/releases) in `premake` directory.
3. Clone this repository.
4. Go to the `premake` directory and run `generate_projects.bat`.
5. Open the solution (`TweakDBext.sln`) located in `projects` directory.
