# Loops Channel  (loops.co.ke)

VST3 channel strip (filters, 4-band EQ, compressor, expander/gate, analog drive) for FL Studio on Windows.
Source: JUCE 8 / C++17. UI: 366 x 678 px, drawn in code (no image files).

## Download the Windows installer (no Visual Studio needed)
Download the latest installer from [GitHub Releases](https://github.com/joe-calih/SSL-Channel/releases/latest).
To publish a new release, push a version tag such as `v1.0.0`; GitHub Actions builds the VST3 and installer, then attaches the installer to that release. Ordinary pushes and manual workflow runs produce a downloadable Actions artifact.

Run the installer. It installs to `C:\Program Files\Common Files\VST3` and opens https://loops.co.ke.
In FL Studio, open Options > Manage plugins > *Find installed plugins*. Loops Channel appears under Installed > Effects.

## Build locally (Windows)
    cmake -B build -G "Visual Studio 17 2022" -A x64
    cmake --build build --config Release
    (then compile installer\LoopsChannel.iss with Inno Setup 6)

## Notes
- Needs FL Studio 20.8+ (VST3). Not tested inside FL Studio yet.
- Edit the website text/licence in `installer/info.txt` and `installer/license.txt`.
