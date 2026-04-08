# midi-visu

A real-time MIDI visualizer built as a JUCE audio plugin. Receives MIDI input and renders
animated visuals with multiple visualization modes. Runs as a standalone app or as a
VST3/AU plugin.

## Visualization Modes

- **Circles** (default) - animated wobbling circles driven by MIDI input, with SVG shape
  deformation, video backgrounds, floating/collision physics, and clock sync
- **Fluid Sim** - particle-based fluid simulation where MIDI events inject colored fluid
  from syringes around a circular arena, with cohesion forces and plastic collisions

Press `M` to toggle between modes.

## Prerequisites

- **CMake** 3.22+
- **C++17** compiler (Clang on macOS)
- **JUCE 8** - clone or download to `~/Downloads/JUCE` (the CMake build expects it at
  `../../Downloads/JUCE` relative to the project root)
- **macOS** - required for the AVFoundation video background (Objective-C++ with ARC)

## Build

```bash
# configure (first time only)
cmake -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug

# build all formats (Standalone, VST3, AU)
cmake --build cmake-build-debug --target midi-visu_All

# build & run tests
cmake --build cmake-build-debug --target midi-visu-tests
cd cmake-build-debug && ctest --output-on-failure
```

The standalone app is output to:

```
cmake-build-debug/midi-visu_artefacts/Debug/Standalone/midi-visu.app
```

## Keyboard Shortcuts

| Key | Action |
|-----|--------|
| `M` | Toggle visualization mode |
| `F` | Toggle fullscreen |
| `Esc` | Exit fullscreen |
| `O` | Toggle options panel |
| `L` | Toggle log panel |
| `I` | Debug inject (fluid sim mode) |

Circles can be repositioned by dragging them with the mouse.

## MIDI Channel Mapping

| MIDI Channel | Role | Visual Column |
|--------------|------|---------------|
| 10 | Drums (4 voices) | 0 (leftmost) |
| 2 | Melodic track 1 | 1 |
| 3 | Melodic track 2 | 2 |
| 4 | Melodic track 3 | 3 |

Channel assignments are configurable via the options panel.

For project structure, class descriptions, and implementation patterns, see
[ARCHITECTURE.md](ARCHITECTURE.md).

## License

All rights reserved.
