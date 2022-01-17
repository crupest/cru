# CruUI

[![CI](https://github.com/crupest/cru/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/crupest/cru/actions/workflows/ci.yml)

## overview

_cru_ is a C++ library. The biggest part of it is for UI.

It is **under construction**. It can be built with _CMake_.

After I changed to macOS, Windows feature is left behind now.

[![Open in Gitpod](https://gitpod.io/button/open-in-gitpod.svg)](https://gitpod.io/#https://github.com/crupest/cru)

## build

_cru_ uses [_vcpkg_](https://github.com/microsoft/vcpkg) with _manifests mode_ to manage dependencies. You must setup vcpkg and `VCPKG_INSTALLATION_ROOT` environment variable.

There is also a automatic python script used in CI. Use python3 to run `tool/win_build.py` to build on Windows. Run with `-h` to see options. For convenience to avoid environment setup it uses _Visual Studio_ generator.

Check the code or fire a issue if you have any problem.

## structure of repository

- `include/` contains public headers.

- `src/` contains the main codes including headers and sources.

- `test/` contains unit tests. Only targets in this directories need `gtest`.

- `demo/` contains sources of demo excutable targets. They are mostly used for integrated tests but can also be used for demonstration.

- `docs` contains docs-related things.

- `tools/` contains some codes of tools like code generators.

  - `cppmerge/` contains a _python_ script program that merges all cpp headers and sources into one **single** header and source. **Not use now**.

  - `migrate-1/` contains _python_ scripts that migrate all header and source files into CamelCase. **Not use after migration**.
