# 🆕 Underground Trigger Creator - New Features Guide

**Compatibility**: DayZ 1.29+ (some features require specific versions)

---

## 📖 Table of Contents

1. [What's New](#whats-new)
2. [Breadcrumb Visual Scaling](#-breadcrumb-visual-scaling)
3. [LinePointFade Mode](#-LinePointFade-Mode)
4. [LightLerp (Lighting Transitions)](#-lightlerp-lighting-transitions)
5. [Adding Sound Properties](#-adding-sound-properties)
6. [Quick Reference](#-quick-reference)

---

## 🎉 What's New

The Underground Trigger Creator mod now supports advanced underground trigger features that give you precise control over player experience:

### New Features Overview

| Feature | Type | Purpose | DayZ Version |
|---------|------|---------|--------------|
| **Breadcrumb Visual Scaling** | Visual | Breadcrumbs now scale based on their radius | All versions |
| **LinePointFade** | Trigger Property | Linear path-based eye accommodation | All versions |
| **LightLerp** | Breadcrumb Property | Smooth lighting transitions | All versions |
| **AmbientSoundType** | Trigger Property | 23* engine sound controllers | All versions |
| **AmbientSoundSet** | Trigger Property | Custom looping audio playback | **1.29+ ONLY** |

*
---

## 🎨 Breadcrumb Visual Scaling

### What Changed?

Breadcrumbs now **visually scale** in the editor based on their **Radius** property, making it much easier to see their area of influence!

### How It Works

- **Default Radius (-1.0)**: Breadcrumb displays at 5m scale
- **Custom Radius**: Breadcrumb scales to match the radius value
- **Real-time Updates**: Scale updates automatically when you change the radius

### Visual Comparison

```
Before: All breadcrumbs were same size (hard to see coverage)
   🔵 BC1 (Radius: 5m)
   🔵 BC2 (Radius: 15m)  ← Same visual size as BC1
   🔵 BC3 (Radius: 25m)  ← Same visual size as BC1

After: Breadcrumbs scale to radius (easy to see coverage)
   🔵 BC1 (Radius: 5m)
   🔵🔵🔵 BC2 (Radius: 15m)  ← 3x larger visually
   🔵🔵🔵🔵🔵 BC3 (Radius: 25m)  ← 5x larger visually
```

### Why This Matters

- **Better Planning**: See exactly how far each breadcrumb's influence extends
- **Easier Layout**: No more guessing if breadcrumbs overlap or have gaps
- **Visual Feedback**: Immediately see the effect of radius changes

### Usage Tips

1. Place breadcrumbs in your trigger
2. Adjust the **Radius** property in the Properties panel
3. Watch the breadcrumb scale in real-time
4. Use overlapping radii for smooth transitions
5. Use non-overlapping radii for distinct zones

### ⚠️ Important: When Radius Matters

**Radius IS used** (UseLinePointFade = false):
- Standard calculation mode (default)
- Open cave systems, multi-room areas
- Radius defines area of influence
- Overlapping radii create blended zones

**Radius IS NOT used** (UseLinePointFade = true):
- Linear path mode
- Tunnels, corridors, hallways
- Only Position, EyeAccommodation, and LightLerp matter
- Distance along path determines blending

**Visual Scaling**: Breadcrumbs ALWAYS scale visually based on Radius in the editor, regardless of UseLinePointFade mode. This helps with planning, even if Radius isn't used in the calculation.

---

## 🛤️ LinePointFade Mode

### What Is It?

`UseLinePointFade` is a **trigger property** that changes how eye accommodation is calculated from breadcrumbs. It's perfect for **tunnels, hallways, and linear paths**.

### Two Calculation Modes

#### Mode 1: Standard (UseLinePointFade = false) - DEFAULT
**Best for**: Open caves, multi-room areas, complex layouts

**How it works**:
- All nearby breadcrumbs influence the player at once
- Uses weighted distance calculation (closer = stronger influence)
- Player can approach from any direction
- Works with 1+ breadcrumbs

**Example**: Large underground chamber with multiple entry points
```
         [Entrance BC]
            /    \
           /      \
    [Room A]    [Room B]
           \      /
            \    /
         [Center BC]
```

---

#### Mode 2: LinePointFade (UseLinePointFade = true)
**Best for**: Tunnels, corridors, mine shafts, linear paths

**How it works**:
- Breadcrumbs form a path
- Eye accommodation smoothly interpolates between the two closest breadcrumbs
- Creates guided progression along the path
- Requires 2+ breadcrumbs

**Example**: Mine shaft entrance to deep underground
```
[BC1: 1.0] →→→ [BC2: 0.75] →→→ [BC3: 0.5] →→→ [BC4: 0.0]
  Bright        Transitioning      Darker       Complete Dark
```

### When to Use Each Mode

| Scenario | UseLinePointFade | Reasoning |
|----------|------------------|-----------|
| **Cave system** (multiple rooms) | `false` | Open area, multiple paths |
| **Mine shaft** (straight tunnel) | `true` | Linear progression |
| **Underground base** (complex) | `false` | Non-linear navigation |
| **Hallway** (one direction) | `true` | Controlled experience |
| **Sewer** (branching) | `false` | Multiple intersections |
| **Bunker corridor** (linear) | `true` | Smooth guided transitions |

### How to Enable

1. Select your **UGTriggerObject**
2. Open the **Properties** panel
3. Find **UseLinePointFade** checkbox
4. Check it to enable linear path mode
5. **IMPORTANT**: Place at least 2 breadcrumbs along your intended path (required!)
6. Space breadcrumbs 10-15m apart for smooth transitions

### ⚠️ Critical Requirement

**UseLinePointFade = true requires MINIMUM 2 breadcrumbs**:
- With 0-1 breadcrumbs: Falls back to standard mode (UseLinePointFade is ignored)
- With 2+ breadcrumbs: Linear path mode activates as expected

**Don't enable UseLinePointFade on triggers without breadcrumbs** - it has no effect and is misleading.

---

## 💡 LightLerp (Lighting Transitions)

### What Is It?

`LightLerp` is a **breadcrumb property** that triggers smooth **lighting darkening animations** as players move through underground areas.

### ⚠️ CRITICAL Requirements

- **ONLY works with UseLinePointFade = true**
- **Requires minimum 2 breadcrumbs** (same requirement as UseLinePointFade)
- Only affects **lighting**, NOT eye accommodation
- Breadcrumbs need **alternating values** for transitions to occur

**Note**: Since LightLerp requires UseLinePointFade, and UseLinePointFade requires 2+ breadcrumbs, **LightLerp is useless on triggers with 0-1 breadcrumbs**.

### How It Works

```
LightLerp Values:
• false (0) = Dark lighting state (deeper underground)
• true (1) = Light lighting state (approaching surface)

When player moves between breadcrumbs with DIFFERENT LightLerp values:
→ Lighting smoothly transitions with animation (~2 seconds)
→ Uses smooth easing for natural feel
→ Triggers only when value changes (1→0 or 0→1)
```

### Example Setup: Cave Entrance

```json
"Breadcrumbs": [
  {
    "Position": [100, 50, 100],
    "EyeAccommodation": 1.0,
    "LightLerp": 1     ← Bright (surface)
  },
  {
    "Position": [110, 48, 100],
    "EyeAccommodation": 0.7,
    "LightLerp": 1     ← Still bright
  },
  {
    "Position": [120, 45, 100],
    "EyeAccommodation": 0.3,
    "LightLerp": 0     ← TRANSITION! (1→0)
  },
  {
    "Position": [130, 42, 100],
    "EyeAccommodation": 0.0,
    "LightLerp": 0     ← Dark (deep cave)
  }
]
```

**⚠️ Note**: When using LinePointFade mode, **Radius is NOT used**. Only Position, EyeAccommodation, and LightLerp matter.

### What Players Experience

```
Walking from BC1 → BC2 → BC3 → BC4:

BC1 to BC2: Eyes adjust (1.0→0.7), lighting stays bright
BC2 to BC3: Eyes adjust (0.7→0.3), LIGHTING DARKENS SMOOTHLY ✨
BC3 to BC4: Eyes adjust (0.3→0.0), lighting stays dark
```

### Best Practices

**✅ DO:**
- Enable `UseLinePointFade = true` on the trigger
- Use LightLerp to create distinct lighting zones
- Place transition at entrance/exit points
- Test the transition by walking through slowly

**❌ DON'T:**
- Use LightLerp without UseLinePointFade (won't work)
- Set same LightLerp value on all breadcrumbs (no transitions)
- Forget to place enough breadcrumbs (need 2+ minimum)

### Lighting Zone Patterns

**Pattern 1: Surface to Deep**
```
Surface → Entrance → Deep Cave
LightLerp:   1    →    1    →    0
            (Bright)  (Bright) (TRANSITION to Dark)
```

**Pattern 2: Multiple Zones**
```
Exit → Transition → Cave → Transition → Deep Pit
  1        0          0         0          0
```

**Pattern 3: In and Out**
```
Entrance → Deep → Exit
    1    →  0  →  1
  (Light → Dark → Light again)
```

---

## 🔊 Adding Sound Properties

### Overview

Underground triggers support **layered audio** with two property types:

1. **AmbientSoundType**: Engine sound controllers (audio processing)
2. **AmbientSoundSet**: Custom looping audio files (**1.29+ ONLY**)

### AmbientSoundType (All Versions)

**What it does**: Activates DayZ's built-in audio processing controllers that modify how ALL sounds are played in the area. These are NOT ambient sounds themselves, but audio processors that affect reverb, echo, muffling, and environmental audio characteristics.

**Sound Controller List:**

These controllers modify the engine's internal audio processing parameters. The exact engine values and effects they modify are not publicly documented by Bohemia (or I can't find them).

**Sound Controllers (22):**
From `P:\scripts\3_Game\Sound.c` line 34:

| # | Controller Name | # | Controller Name |
|---|----------------|---|----------------|
| 1 | `rain` | 12 | `altitudeSea` |
| 2 | `night` | 13 | `altitudeSurface` |
| 3 | `meadow` | 14 | `daytime` |
| 4 | `trees` | 15 | `shooting` |
| 5 | `hills` | 16 | `coast` |
| 6 | `houses` | 17 | `waterDepth` |
| 7 | `windy` | 18 | `overcast` |
| 8 | `deadBody` | 19 | `fog` |
| 9 | `sea` | 20 | `snowfall` |
| 10 | `forest` | 21 | `caveSmall` |
| 11 | `altitudeGround` | 22 | `caveBig` |


### How to Use AmbientSoundType

1. Select your **UGTriggerObject**
2. Open **Properties** panel
3. Find **AmbientSoundType** dropdown
4. Select appropriate controller based on acoustics you want:
   - `caveBig` for large cave echo/reverb
   - `caveSmall` for tight space acoustics
   - `waterDepth` for underwater muffling effect
   - `houses` for indoor/building acoustics
5. Test by making sounds (footsteps, voice) in the trigger

---

### AmbientSoundSet (1.29+ ONLY) 🆕

**What it does**: Plays specific looping audio files for atmospheric sound

**⚠️ COMPATIBILITY WARNING**: This feature **requires DayZ 1.29 or later**. On older versions, the property will be ignored.

### Example SoundSets

| SoundSet |
|----------|
| `Geyser_bubbling_loop_SoundSet` |
| `pour_Water_Pot_SoundSet` |
| `powerGeneratorLoop_SoundSet` |
| `Bunker_Lamp_Hum_SoundSet` |
| `Geyser_eruption_loop_SoundSet` |
| `lockpicker_loop_SoundSet` |
| `Flag_lower_loop_SoundSet` |
| `AlarmClock_Ring_Loop_SoundSet` |


### How to Use AmbientSoundSet

1. Select your **UGTriggerObject**
2. Open **Properties** panel
3. Find **AmbientSoundSet** text field
4. Enter exact SoundSet name
5. Example: `Geyser_bubbling_loop_SoundSet`

---

## 📋 Quick Reference

### Decision Matrix

| I Want To... | UseLinePointFade | LightLerp | AmbientSoundType | AmbientSoundSet (1.29+) |
|-------------|------------------|-----------|------------------|------------------------|
| **Linear tunnel with smooth lighting** | ✅ true | ✅ Alternating 0/1 | `caveSmall` | Optional |
| **Open cave, any direction** | ❌ false | N/A (ignored) | `caveBig` | Optional |
| **Underwater area** | Either | If linear: alternate | `waterDepth` | Optional |
| **Military bunker** | ✅ true | ✅ Alternating 0/1 | `houses` | `powerGeneratorLoop_SoundSet` |
| **Mine shaft** | ✅ true | ✅ Alternating 0/1 | `caveSmall` | Optional |
| **Complex cave system** | ❌ false | N/A (ignored) | `caveBig` | `Geyser_bubbling_loop_SoundSet` |

### Property Quick Check

**Trigger-Level Properties:**
- ✅ UseLinePointFade (true/false) - Changes calculation mode
- ✅ AmbientSoundType (dropdown) - 22 engine sound controllers
- ✅ AmbientSoundSet (text, 1.29+) - Custom audio loops

**Breadcrumb-Level Properties:**
- ✅ Radius (number) - Visual scale & influence area (only used when UseLinePointFade = false)
- ✅ LightLerp (true/false) - Lighting transitions (only with UseLinePointFade = true)
- ✅ EyeAccommodation (0.0-1.0) - Darkness level
- ✅ UseRaycast (true/false) - Block influence through walls (only used when UseLinePointFade = false)

### Common Mistakes to Avoid

❌ **Setting LightLerp without enabling UseLinePointFade**
→ LightLerp only works with LinePointFade mode!

❌ **Using same LightLerp value on all breadcrumbs**
→ No transitions will occur! Use alternating 1/0 values.

❌ **Not enough breadcrumbs for LinePointFade**
→ Need minimum 2 breadcrumbs for linear path mode. With 0-1 breadcrumbs, it falls back to standard mode.

❌ **Enabling UseLinePointFade on triggers without breadcrumbs**
→ Has no effect - just leave it disabled (false).

❌ **Using UseLinePointFade for open areas**
→ Standard mode (false) is better for multi-directional exploration.

---

## 📚 Additional Resources

- **All 6,190 SoundSets**: Browse `P:\DZ\sounds\hpp\config.cpp` (It's possible that some do not work if they are not intended to loop)
- **Original Bohemia Wiki**: [Underground Areas Configuration](https://community.bistudio.com/wiki/DayZ:Underground_Areas_Configuration)

---

## 💬 Getting Help

If you encounter issues or have questions:

**Discord:** **@bauvdel** & **@JinieJ**

**GitHub Issues**: [Create an issue](https://github.com/bauvdel/EditorUGTriggers/issues)

---

## 🎓 Tips for Success

1. **Start Simple**: Use standard mode (UseLinePointFade: false) until comfortable
2. **Test Progression**: Walk through your triggers slowly to test transitions
3. **Watch the Scale**: Use breadcrumb visual scaling to plan coverage
4. **Layer Audio**: Combine SoundType + SoundSet for rich environments (1.29+)
5. **Plan Lighting Zones**: Use LightLerp to create distinct entrance/deep areas
6. **Experiment**: Try different combinations to find what works best

---

**Happy Underground Triggering! ⛏️**

*Created with ❤️ by bauvdel & JinieJ*
