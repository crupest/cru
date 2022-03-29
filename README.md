# CruUI

[![CI](https://github.com/crupest/cru/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/crupest/cru/actions/workflows/ci.yml)

## overview

_cru_ is a C++ library. The biggest part of it is for UI.

It is **under heavy construction**.

Check the code or fire a issue if you have any problem.

[![Open in Gitpod](https://gitpod.io/button/open-in-gitpod.svg)](https://gitpod.io/#https://github.com/crupest/cru)

## build

_cru_ is built with [_CMake_](https://cmake.org/) and [_vcpkg_](https://github.com/microsoft/vcpkg). You must setup vcpkg first. There are two ways.

1. Set environment variable `VCPKG_INSTALLATION_ROOT` to your vcpkg installation directory.

2. Symlink the vcpkg installation directory to `vcpkg` of project root directory. Or just put real vcpkg there.

## structure of repository

- `include/` contains public headers.

- `src/` contains the main codes including headers and sources.

- `test/` contains unit tests. Only targets in this directories need `gtest`.

- `demo/` contains sources of demo excutable targets. They are mostly used for integrated tests but can also be used for demonstration.

- `docs/` contains docs-related things.

- `tools/` contains some codes of tools I wrote for development. However most of them are not used now.
