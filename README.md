# CruUI

[![Build Status](https://dev.azure.com/crupest/CruUI/_apis/build/status/crupest.CruUI)](https://dev.azure.com/crupest/CruUI/_build/latest?definitionId=5)

## overview

CruUI is a UI library running on *Windows* platform using *Direct2D* hardware accelaration.

It is **under construction**.

Please using VS2017 open the `CruUI.sln` file in the root directory.

## structure of repository

- `CruUI.sln` is *Visual Studio* solution file of the repository which including two vc++ projects.

    - `CruUI.vcxproj` is a *Visual Studio* vc++ project file that includes the main code in `src/`.

    - `CruUI-Generate/CruUI-Generate.vcxproj` is a *Visual Studio* vc++ project file that includes a header and a source generated from tools(see below). It is a project used to test that generated codes work well.

- `src/` contains the main codes including headers and sources.

- `tools/` contains some codes of tools like code generators.

    - `tools/cppmerge` contains a *python* script program that merges all cpp headers and sources into one **single** header and source.

- `scripts/` contains some useful scripts.

    - `scripts/merge.py` is a *python* script that invoke the *cppmerge* tool to merge code in `src\` to `CruUI-Generate\cru_ui.h` and `CruUI-Generate\cru_ui.cpp`.

- `CruUI-Generate/` contains codes and project file that used to test merged codes.

