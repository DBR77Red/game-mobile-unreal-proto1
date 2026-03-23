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

**Control scheme:**
- Left thumb: Android virtual joystick (UE5 built-in Touch Interface / `UVirtualJoystick`)
  - Left/Right axis → horizontal movement (maps to `IA_Move`)
  - **Up on joystick → triggers Jump** (`IA_Jump`) — no separate jump button
- Right thumb: single on-screen Attack button (maps to `IA_Attack`)

**Implementation notes:**
- Configure the Touch Interface asset (`Content/Input/TI_Mobile.uasset`) with two zones:
  - Left zone: joystick with `CaptureRadius` covering ~35% of screen width; bind up-axis threshold (e.g. Y > 0.5) to fire `IA_Jump`
  - Right zone: single tap button for `IA_Attack`
- To map joystick-up to Jump via Enhanced Input, add a `UInputTriggerThreshold` on the vertical axis binding, or handle it in `PlayerCharacter::Tick` / `SetupPlayerInputComponent` by reading the axis value and calling `Jump()` when Y exceeds a threshold
- Make sure Enhanced Input works with touch events (set `bSupportsTouch=true` in project settings)
- Ensure safe zone padding is respected (`UUserWidget` anchors to safe zone, or use `USafeZone` slot)
- Properly size joystick and button for thumbs (~120–150 dp diameter)
- Test on Android (target platform); validate with different screen aspect ratios

**In-game exit / pause menu:**
- Add a floating pause/menu button (top-right corner) visible during gameplay
- Tapping it opens a UMG pause widget with:
  - **Resume** — dismisses menu, unpauses
  - **Restart Level** — reloads current level via `UGameplayStatics::OpenLevel`
  - **Quit to Main Menu** — loads main menu level
  - **Quit Game** — calls `UKismetSystemLibrary::QuitGame` (or `APlayerController::ConsoleCommand("quit")`)
- Pause state: call `UGameplayStatics::SetGamePaused(this, true/false)` on open/close
- Widget should be added to the viewport in `PlayerCharacter::BeginPlay` or via the HUD class (`UPlayerHUD`)

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
