#pragma once

namespace voidlight {
inline constexpr char DECOR_AI_GUIDE[] =
    "<cg>DECOR AI — DECORATION ASSISTANT</c>\n\n"
    "Choose a style, then build from the main silhouette to larger forms and small details.\n"
    "Keep the gameplay path readable. This version uses an offline guide; live GPT is not connected yet.\n\n"
    "<cy>QUICK WORKFLOW</c>\n"
    "1. Pick a style in STYLES.\n"
    "2. Test light and color in GLOW and COLORS.\n"
    "3. Use AUTO for an experimental decoration plan.";

inline constexpr char STYLE_BOOK[] =
    "<cy>STYLE BOOK</c>\n\n"
    "GLOW — bright edges, darker base, soft light behind the shape.\n"
    "NEON — dark background, clean geometry, two bright colors.\n"
    "MODERN — panels, frames, even spacing, restrained colors.\n"
    "CYBERPUNK — layered panels, cables, cyan or purple light, warm accents.\n"
    "FANTASY — arches, crystals, ornaments and magical highlights.\n"
    "ATMOSPHERIC — distant silhouettes, subtle particles and controlled contrast.\n"
    "MINIMAL — one clear structure and a few accents.\n"
    "HARD DECOR — silhouette, large forms, medium details, micro-details.";

inline constexpr char GLOW_GUIDE[] =
    "<cy>GLOW LAB</c>\n\n"
    "1. Place the main solid blocks.\n"
    "2. Add a thin brighter edge or small highlight details.\n"
    "3. Put a larger, softer glow behind the structure.\n"
    "4. Lower the glow opacity until the blocks remain readable.\n"
    "5. Keep the brightest light near focal points, not everywhere.\n\n"
    "Tip: test the structure against the actual gameplay background.";

inline constexpr char COLOR_GUIDE[] =
    "<cy>COLOR LAB</c>\n\n"
    "PURPLE VOID — deep purple base, lavender details, pale violet highlights.\n"
    "ICE — dark blue base, cyan details, near-white highlights.\n"
    "EMBER — dark brown base, orange details, warm yellow highlights.\n"
    "FOREST — deep green base, mint details, pale green highlights.\n\n"
    "Keep one main color, one supporting color and one highlight color. Check contrast against the background.";

inline constexpr char AUTO_DECOR_GUIDE[] =
    "<cy>AUTO DECOR — EXPERIMENTAL PROTOTYPE</c>\n\n"
    "This first prototype creates a decoration plan, not edits to your level.\n\n"
    "<cg>EXAMPLE: PURPLE GLOW GATE</c>\n"
    "1. Main shape: a simple gate made from solid blocks.\n"
    "2. Frame: a thin lavender outline.\n"
    "3. Light: soft violet glow behind the gate.\n"
    "4. Details: a few small crystals near the corners.\n"
    "5. Finish: darker background accents and clear gameplay space.\n\n"
    "Automatic object placement is not enabled yet. This is a safe first test of the planning feature.";
}
