# CLAUDE.md — PG_CP_GDTV (Crusty Pirate)

## Project Overview
2D side-scrolling action platformer prototype built with UE5.7 + PaperZD.
Following the GameDev.tv course curriculum.
Engine: Unreal Engine 5.7 | Plugin: PaperZD | Input: Enhanced Input System

## Source Files
- `Source/PG_CP_GDTV/PlayerCharacter.h/.cpp` — Player movement, jump, attack, collision
- `Source/PG_CP_GDTV/Enemy.h/.cpp` — Enemy AI, detection, chase, attack, stun, HP
- `Content/Blueprints/BP_GameMode.uasset` — Game mode
- `Content/Input/` — Input actions (Move, Jump, Attack) and IMC_CrustyPirate mapping
- `Content/Levels/Level_1.umap` — Main level

---

## Pending Tasks

### 1. Mobile On-Screen Controllers

**Target platform:** Android

**Control scheme (IMPLEMENTED on branch `mobile-inputs-and-pause-menu`):**
- Left thumb: virtual joystick → horizontal movement (`IA_Move`)
- Right thumb: virtual joystick → flick up = Jump (`IA_Jump`), flick right = Attack (`IA_Attack`)

**How it works — key notes:**
- `TI_Mobile.uasset`: two joystick controls (no images needed — axis input works without textures)
  - Left joystick: `Main = Gamepad Left Thumbstick X-Axis`, `Alt = None`, `Treat as Button = false`
  - Right joystick: `Main = Gamepad Right Thumbstick X-Axis`, `Alt = Gamepad Right Thumbstick Y-Axis`, `Treat as Button = false`
- `IMC_CrustyPirate`: right stick axes mapped with **Pressed** trigger (fires `ETriggerEvent::Started` when axis > 0.5 actuation)
  - `IA_Jump` ← `Gamepad Right Thumbstick Y-Axis` + Pressed trigger
  - `IA_Attack` ← `Gamepad Right Thumbstick X-Axis` + Pressed trigger
- `IA_Move` value type: `Axis2D (Vector2D)` — `Move()` reads `.X` only; `.Y` unused
- Do NOT use `Treat as Button = true` controls in TI_Mobile — they do not work without a texture assigned

**Still pending:**
- Add a floating pause button (top-right corner) to `WBP_PlayerHUD` → calls `OpenPauseMenu()` (BlueprintCallable already implemented in C++)
- `UPauseMenuWidget` C++ class and `WBP_PauseMenu` Blueprint are implemented; just need the HUD button wired up
- Test on actual Android device; validate safe zone padding

### 2. Theme Rework
- Replace the pirate theme with a new one (TBD — define new theme: e.g. ninja, knight, sci-fi)
- Rename `IMC_CrustyPirate` to match the new theme
- Replace placeholder sprite sheets / art assets accordingly
- Update level dressing and background to match new theme

### 3. Attack Animations (PaperZD)
- Add multiple attack animations (e.g. light attack, heavy attack, combo chain)
- Wire new `UPaperZDAnimSequence` references in `PlayerCharacter` and `Enemy`
- Implement a basic combo system (chain attacks within a time window)
- Add a heavy/charged attack input (hold attack button)
- Enemy should have its own distinct attack animation separate from the player's

### 4. Sound
- Add a `UAudioComponent` to `PlayerCharacter` for footsteps, jump, attack sounds
- Add sound cues to `Enemy` for aggro, attack, death, and taking damage
- Add background music track to `Level_1`
- Use UE5 Sound Cues or MetaSounds
- Implement basic volume control (SFX / Music) accessible from a settings widget

### 5. Polish & Game Feel
- Add screen shake on heavy hits
- Add hit-flash effect on enemies when they take damage (material parameter)
- Add particle effects for attacks (slash VFX) and death
- Add a death animation and respawn/game-over flow for the player
- Add an HP bar widget for the player (on-screen HUD)
- Replace the `UTextRenderComponent` HP display on enemies with a world-space widget or floating damage numbers
- Add a pause menu with Resume / Restart / Quit options
- Add a simple main menu level
- Tune enemy patrol behavior (idle movement when no target detected)
- Add knockback on hit for both player and enemy

### 7. Multiple Playable Characters

Three characters selectable from a character selection screen before Level_1. Each excels in one stat while being weaker in others. Stats are balanced around the current baseline (HP: 100, Attack: 25, Speed: ~600).

| Character | HP  | Attack Damage | Speed (UU/s) | Role |
|-----------|-----|---------------|--------------|------|
| Brawler   | 100 | 40            | 500          | Hard hitter, slower — rewards aggressive play |
| Scout     | 75  | 25            | 800          | Fast and fragile — rewards skillful movement |
| Tank      | 175 | 15            | 500          | Survives longer, weak hits — forgiving for new players |

**Implementation plan:**
- Add a `CharacterType` enum (Brawler, Scout, Tank) to `GameInstance`
- On `BeginPlay`, read `CharacterType` from `GameInstance` and set `HitPoints`, `AttackDamage`, and `GetCharacterMovement()->MaxWalkSpeed` accordingly
- Add a character selection screen (UMG widget) as the first level/menu
- Optionally: create a base `APlayerCharacter` C++ class with three Blueprint subclasses for distinct sprites/animations per character

---

### 6. General Code / Architecture
- Keep C++ for core gameplay logic; use Blueprints only for data/configuration
- Avoid `BlueprintReadWrite` on properties that should be read-only at runtime
- Use `UPROPERTY(EditDefaultsOnly)` for design-time config values
- Add proper `TakeDamage` override using UE's built-in damage system instead of custom function where possible
