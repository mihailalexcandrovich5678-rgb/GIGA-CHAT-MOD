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

inline constexpr char OBJECT_HELPER_GUIDE[] =
    "<cy>OBJECT HELPER</c>\\n\\n"
    "Blocks build the main silhouette. Slopes add direction and angled transitions.\\n"
    "Use outlines to separate layers and small details to guide the eye.\\n"
    "Keep spikes, saws, portals and the playable route easy to recognize.\\n"
    "Build large forms first, then medium details, then small accents.\\n\\n"
    "This guide explains object roles; it does not place objects automatically.";

inline constexpr char LEVEL_PLANNER_GUIDE[] =
    "<cy>LEVEL PLANNER</c>\n\n"
    "Plan a complete level from its core idea to its final section.\n\n"
    "<cg>1. CHOOSE A CONCEPT</c>\n"
    "Examples: cosmic journey, purple dream, cyberpunk city, ancient temple, underwater ruins.\n\n"
    "<cg>2. BUILD THE STRUCTURE</c>\n"
    "Intro: introduce the world and teach the rhythm.\n"
    "Main part: establish the visual theme and gameplay identity.\n"
    "Drop: make the strongest visual and musical moment.\n"
    "Variation: change the scenery or gameplay to keep interest.\n"
    "Finale: resolve the theme and finish clearly.\n\n"
    "<cg>3. PLAN THE DECORATION</c>\n"
    "Choose a style, three-color palette, main structures, background layers and effects.\n"
    "Give each section a variation while keeping one recognizable visual identity.\n\n"
    "<cg>4. SYNC WITH MUSIC</c>\n"
    "Mark the intro, beat changes, drops and transitions. Use strong visual changes at important musical moments.\n\n"
    "This is an offline planning guide. It does not inspect or edit your level automatically.";

inline constexpr char MUSIC_MATCHER_GUIDE[] =
    "<cy>MUSIC MATCHER</c>\n\n"
    "Match the track to the level's mood and gameplay.\n\n"
    "CALM / DREAMY — ambient, soft electronic, gentle melodies; use flowing gameplay and gradual transitions.\n"
    "DARK / MYSTERIOUS — atmospheric, deep bass, sparse percussion; use silhouettes and controlled light.\n"
    "ENERGETIC — fast electronic, strong rhythm and clear drops; use sharper transitions and active gameplay.\n"
    "EPIC / FANTASY — orchestral or melodic electronic; build toward a clear climax.\n"
    "CYBERPUNK — synthwave, electro or bass-driven electronic; use neon accents and rhythmic structures.\n\n"
    "<cg>TRACK CHECKLIST</c>\n"
    "1. Does the mood fit the level concept?\n"
    "2. Is the rhythm comfortable for the intended gameplay?\n"
    "3. Are there clear moments for transitions and a drop?\n"
    "4. Does the track length fit the planned level?\n"
    "5. Mark important beats and section changes before decorating.\n\n"
    "This guide suggests music characteristics, not a live song catalogue. Check track availability and permissions in the game.";

inline constexpr char PERFORMANCE_HELPER_GUIDE[] =
    "<cy>PERFORMANCE HELPER</c>\\n\\n"
    "Use this checklist to keep a level readable and easier to run:\\n"
    "1. Avoid unnecessary layers of overlapping effects.\\n"
    "2. Reuse a small set of detail patterns instead of duplicating huge groups.\\n"
    "3. Keep gameplay-critical objects visually clear.\\n"
    "4. Test dense sections on the device that will play the level.\\n"
    "5. Reduce decorative particles and pulse effects if the scene feels busy.\\n\\n"
    "This helper gives optimization advice; it does not measure FPS or inspect the running level.";

inline constexpr char SCREENSHOT_ANALYZER_GUIDE[] =
    "<cy>SCREENSHOT ANALYZER</c>\\n\\n"
    "Screenshot analysis is not connected in this build.\\n"
    "For a manual review, check: gameplay readability, contrast, visual hierarchy, spacing, color balance and decoration density.\\n"
    "A real analyzer needs image input and an AI service; this button does not pretend to analyze an image.";

inline constexpr char MY_PROJECTS_GUIDE[] =
    "<cy>MY PROJECTS</c>\\n\\n"
    "Project slots are not saved in this build yet.\\n"
    "For now, keep a short project note with: level name, song, theme, palette, current section and next task.\\n"
    "Persistent project storage will be added separately; this screen does not claim to save data.";

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
