# CruUI

[![Build Status](https://dev.azure.com/crupest/CruUI/_apis/build/status/crupest.CruUI)](https://dev.azure.com/crupest/CruUI/_build/latest?definitionId=5)

## overview

CruUI is a UI library using [*skia*](https://skia.org/).

It is **under construction**.

## build

Set python**2** on path to allow skia build tool work. Use python**3** to run `tool/build.py`. Run with `-h` to see the options.

Notice: x86 is **not** currently supported!

Tips: You can run the build script with `--only-skia` to build the skia dependency fisrt. Then feel free to open an ide and edit the source code of cruui and build it with command in ide.

## structure of repository

- `src/` contains the main codes including headers and sources.

- `tools/` contains some codes of tools like code generators.

    - `tools/cppmerge` contains a *python* script program that merges all cpp headers and sources into one **single** header and source. **Not use now**.

    - `tools/build.py` is a python3 script to build the source code.

- `snippets/` contains useful snippets file for *Visual Studio*, you can import it.
