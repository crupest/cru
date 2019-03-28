# CruUI

[![Build Status](https://dev.azure.com/crupest/CruUI/_apis/build/status/crupest.CruUI)](https://dev.azure.com/crupest/CruUI/_build/latest?definitionId=5)

## overview

CruUI is a UI library running on *Windows* platform using *Direct2D* hardware accelaration.

It is **under construction**.

Please using VS2017 open the `CruUI.sln` file in the root directory.

## structure of repository

- `CruUI.sln` is *Visual Studio* solution file of the repository.

    - `CruUI.vcxproj` is a *Visual Studio* vc++ project file that includes the main code in `src/`.

- `src/` contains the main codes including headers and sources.

- `tools/` contains some codes of tools like code generators.

    - `tools/cppmerge` contains a *python* script program that merges all cpp headers and sources into one **single** header and source.

- `snippets/` contains useful snippets file for *Visual Studio*, you can import it.
