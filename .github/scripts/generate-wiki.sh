#!/bin/bash
# Generate wiki pages from the codebase.
# Usage: generate-wiki.sh <output-dir> <version>
set -euo pipefail

OUT="$1"
VERSION="${2:-dev}"
mkdir -p "$OUT"

# ── Home page (from README) ─────────────────────────────────────────────────
cp README.md "$OUT/Home.md"

# ── Architecture page ────────────────────────────────────────────────────────
cp ARCHITECTURE.md "$OUT/Architecture.md"

# ── Per-class API pages from headers ─────────────────────────────────────────
for header in src/*.h; do
    filename=$(basename "$header")
    classname="${filename%.h}"

    # Skip header-only constant files that aren't classes
    case "$classname" in
        AppConstants|StyleTokens) continue ;;
    esac

    # Extract the doc comment block at the top (between first /* and */)
    doc=$(sed -n '/^\/\*/,/\*\//{ /^\/\*/d; /\*\//d; s/^[ *]*/  /; p; }' "$header" | sed 's/^  =*$//' | sed '/^$/d')

    # Extract public method signatures
    methods=$(sed -n '/^public:/,/^private:\|^protected:\|^};/{
        /^\s*\/\//d
        /^public:/d
        /^private:/d
        /^protected:/d
        /^};/d
        /^$/d
        /^\s*$/d
        /friend /d
        /JUCE_DECLARE/d
        /using /d
        /^[[:space:]]*class /d
        p
    }' "$header" | sed 's/^ *//')

    # Extract class declaration line
    classline=$(grep -E '^\s*class\s+'"$classname"'\b' "$header" | head -1 | sed 's/^ *//')

    {
        echo "# $classname"
        echo ""
        echo "_Version: $VERSION"
        echo ""
        echo "**Header:** \`$header\`"
        echo ""

        if [ -n "$doc" ]; then
            echo "$doc" | sed 's/^  //'
            echo ""
        fi

        if [ -n "$classline" ]; then
            echo "## Declaration"
            echo ""
            echo '```cpp'
            echo "$classline"
            echo '```'
            echo ""
        fi

        if [ -n "$methods" ]; then
            echo "## Public API"
            echo ""
            echo '```cpp'
            echo "$methods"
            echo '```'
            echo ""
        fi
    } > "$OUT/$classname.md"
done

# ── Sidebar ──────────────────────────────────────────────────────────────────
{
    echo "**midi-visu $VERSION**"
    echo ""
    echo "- [[Home]]"
    echo "- [[Architecture]]"
    echo ""
    echo "**Audio Thread**"
    echo "- [[PluginProcessor|PluginProcessor]]"
    echo "- [[MidiManager|MidiManager]]"
    echo "- [[VoiceManager|VoiceManager]]"
    echo ""
    echo "**UI Thread**"
    echo "- [[PluginEditor|PluginEditor]]"
    echo "- [[UiManager|UiManager]]"
    echo "- [[InteractionManager|InteractionManager]]"
    echo "- [[StyleManager|StyleManager]]"
    echo ""
    echo "**Video**"
    echo "- [[VideoBackground|VideoBackground]]"
    echo "- [[VideoListManager|VideoListManager]]"
    echo ""
    echo "**Widgets**"
    echo "- [[RangeSlider|RangeSlider]]"
    echo "- [[RangeSliderLogic|RangeSliderLogic]]"
    echo "- [[SeekBar|SeekBar]]"
    echo "- [[MultiHandleSliderLogic|MultiHandleSliderLogic]]"
    echo "- [[OptionsPanelLayout|OptionsPanelLayout]]"
    echo ""
    echo "**Rendering**"
    echo "- [[SvgShapeManager|SvgShapeManager]]"
    echo "- [[SvgWobbleLogic|SvgWobbleLogic]]"
} > "$OUT/_Sidebar.md"

echo "Wiki generated: $(ls "$OUT"/*.md | wc -l) pages in $OUT"
