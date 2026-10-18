# Guardians of the Pharaoh – DMET 502 Project 1

Ancient Egypt tower defense: guards defend the Pharaoh's tomb from tomb raiders.

## Screen layout (matches our reference picture)

```
y 720 ┌──────────────────────────────────────────────┐
      │ [II Wave 1/3]  sky, palms, obelisks, pyramids  │
y 560 ├──────┬───────────────────────────────────────┤
      │ TOMB │ lane 4                                │
      │      │ lane 3        guards →    ← raiders   │
      │      │ lane 2                                │
      │      │ lane 1                                │
      │      │ lane 0                                │
y 120 ├──────┴───────────────────────────────────────┤
      │ [card][card][card]   help text     [coins]   │
y 0   └──────────────────────────────────────────────┘
      x 0   160                                  1280
```

The starter already has the themed look: dusk desert, tomb, palms, obelisks,
torches, navy/gold menus, guard and raider models, ankh coins.
**These are BASIC versions – each owner adds detail and all the behaviour.**

## Folder structure – who edits what

| Folder / file | Owner | Rule |
|---|---|---|
| `src/core/` | **Everyone (shared)** | Don't change without telling the group |
| `main.cpp` | Member 1 | Others shouldn't need to touch it |
| `src/member1_ui/` | Member 1 | Menus, lane grid, HUD, cards, win/lose |
| `src/member2_players/` | Member 2 | Guards |
| `src/member3_enemies/` | Member 3 | Raiders, waves, combat |
| `src/member4_world/` | Member 4 | World, tomb, cutscene, coins, sound |

**Only edit files in your own folder.** That way Git never has conflicts.
You can add new `.cpp` / `.h` files in your folder – CMake picks them up automatically
(re-run CMake configure if VS Code doesn't see a new file).

Each TODO list is at the top of your own file(s).

## How the parts talk to each other

- `src/core/interfaces.h` lists every agreed function name (e.g. `spawnPlayer`, `spawnCoin`,
  `playSound`). They all already exist as stubs, so **you can call anyone's function today**,
  even before they've written it.
- `src/core/game_state.h` holds the shared lists: `g_players`, `g_enemies`, `g_attacks`,
  `g_collectibles`, plus `g_coins` and `g_screen`.
- `src/core/entity.h` defines `Entity`, `Attack`, `Collectible`.
- `src/core/config.h` has every agreed number: screen size, lanes, costs, health, speeds, waves.

## Testing your part alone

- **Jump to your screen:** in `config.h` change `START_SCREEN` locally
  (e.g. `SCREEN_PLAYING` or `SCREEN_CUTSCENE`). Don't commit that change.
- **Test keys** (placeholders – remove before submitting):

| Key | Does |
|---|---|
| ENTER | start game / skip cutscene |
| ESC | pause (in game) |
| F1 | show lanes, columns, and mouse x/y |
| 1 / 2 / 3 | spawn soldier / archer / shield-bearer in the lane under the mouse |
| 7 / 8 / 9 | spawn slingshot / fire-pot / axe-brute raider in the lane under the mouse |
| C | drop a coin under the mouse |
| K | everyone loses 1 hp (test red tint + health bars) |
| + / - | add / remove 10 coins |
| F9 / F10 | force win / lose |

- Need a dummy opponent? Push one yourself, e.g.
  `Entity d; d.type = PLAYER_MELEE; d.lane = 2; d.x = 600; d.y = laneBottomY(2)+10; d.hp = d.maxHp = 5; g_players.push_back(d);`

## Drawing cheat sheet (`src/core/shapes.h`)

Coordinates: pixels, **(0,0) = bottom-left**, screen is 1280 × 720.

```cpp
drawRect(x, y, w, h, color);              // (x,y) = bottom-left corner
drawRoundedRect(x, y, w, h, radius, color);
drawGradientRect(x, y, w, h, bottomColor, topColor);
drawRectCentered(cx, cy, w, h, color);
drawCircle(cx, cy, r, color);
drawEllipse(cx, cy, rx, ry, color);
drawTriangle(x1,y1, x2,y2, x3,y3, color);
drawLine(x1,y1, x2,y2, thickness, color);
drawArc(cx, cy, r, startDeg, endDeg, thickness, color);
drawPolygon({{x1,y1},{x2,y2},{x3,y3},...}, color);   // convex only
drawText("Coins: 30", x, yTop, size, color);
drawTextCentered("PAUSED", cx, yTop, size, color);

pushMatrix(); translate(x, y); rotate(deg); scale(sx, sy); ... popMatrix();

setTint(entity.damageAmount());   // red damage tint, 0..1
setGrayscale(true);               // locked cards
setAlpha(0.5f);                   // fades
resetEffects();                   // ALWAYS call after using the above

drawHealthBar(e.x, e.y + e.height + 8, e.hp, e.maxHp);  // after resetEffects, outside pushMatrix
```

Shared models and UI (call them from anywhere):

```cpp
drawPlayerModel(PLAYER_MELEE, x, y, scale, animTime);   // Member 2 owns
drawEnemyModel(ENEMY_HARD, x, y, scale, animTime);      // Member 3 owns
drawPanel(x, y, w, h);                    // navy + gold panel   (src/core/ui_style.h)
drawParchmentPanel(x, y, w, h);
if (drawButton("PLAY", cx, cy, w, h)) {...}   // draws + returns true when clicked
drawCoinIcon(cx, cy, r);   drawAnkh(cx, cy, h);   drawDimOverlay(0.5f);
```

Colors: use the shared palette (`PAL_SAND`, `PAL_GOLD`, `PAL_LAPIS`, `PAL_RAIDER_RAGS`, ...)
or make your own with `rgb(255, 120, 20)`.
Team art rule: guards look upright and polished, raiders hunched and ragged.
Color alone never counts as "different".

## Input (`src/core/input.h`)

```cpp
keyPressed(GLFW_KEY_W)   // true only on the frame it was pressed
keyDown(GLFW_KEY_W)      // true while held
mouseX(), mouseY()       // already in world coordinates
mouseClicked()           // left click this frame
mouseInRect(x, y, w, h)  // hover check for buttons / cards
```

## Git routine

1. **Pull** before you start working.
2. Work only in your own folder.
3. Build and run – make sure it still compiles.
4. **Commit** with a clear message → **Push**. At least once a day.
5. Never push code that doesn't compile – it breaks everyone.

## Before submitting (Nov 2)

- [ ] Remove all TEST KEYS sections
- [ ] `START_SCREEN` is `SCREEN_MAIN_MENU`
- [ ] Fresh clone on another laptop compiles and runs
- [ ] Zip only `.h` / `.cpp` files (+ CMakeLists.txt if allowed) as `[Team]-[Name].zip`
