# Developer Playground

This directory is a local development workspace for experimenting with ATOM.

It provides a minimal ATOM application setup that can be used for quick tests and development experiments without creating a dedicated example immediately.

By default, the developer playground is **disabled from the build**. The directory is only built as an ATOM application when a `main.c` file is present. This allows the folder to exist as a workspace without affecting normal builds.

Developers can use it for:

* testing new APIs
* debugging hardware behavior
* experimenting with scheduler changes
* validating new features before creating proper examples

The playground sources are intentionally ignored by Git. Files created inside this directory (such as `main.c`, test code, generated files, or build artifacts) are not pushed to the repository. Only the shared configuration files such as this `README.md` and the `CMakeLists.txt` are tracked.

This keeps the repository clean while allowing each developer to maintain a personal scratch area for experiments.
