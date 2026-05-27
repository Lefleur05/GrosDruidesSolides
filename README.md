[French Version](https://github.com/Lefleur05/GrosDruidesSolides/blob/main/README_FR.md)

# Gros Druides Solides

Online multiplayer party game for 2 to 4 players, developed with Unreal Engine 5.  
Inspired by the games *MageQuit* and *Warlock*.

> Solo-programmed project — I am the sole programmer of the game.

[▶ Watch the demo](https://youtu.be/I_6DkkSAsvM)

[![Demo](https://img.youtube.com/vi/I_6DkkSAsvM/maxresdefault.jpg)](https://youtu.be/I_6DkkSAsvM)

---

## Quick Overview

- **Genre:** Party Game, Battle Arena  
- **Players:** 2 to 4 online  
- **Engine:** Unreal Engine 5.6.1 (C++ and Blueprint)  
- **Match Duration:** 3 to 10 minutes  

Players take on the role of eccentric Gaulish druids in a universe inspired by *Astérix*.  
The first player to reach 3 victory points wins the game.

Between each combat round, a **Carousel phase** (inspired by TFT) allows players to choose an item that modifies their spells — creating different builds every match.

---

## What I Developed

### Multiplayer

- A session system allowing players to join directly through the game using a **Find Session** menu.
- Ability to join friends through **Steam invitations**.

### Dynamic Shared Camera

- A shared camera that moves and zooms out dynamically to constantly keep all players visible.
- Dead players are ignored by the camera system.

### Modular Projectile System

- Each spell is entirely driven by an `FItem` data structure (damage, speed, range, cone angle, knockback, explosion, spell cancelation, etc.).
- Collected items directly modify this structure, meaning that as soon as a player picks up an item, it instantly changes the affected spell.

### Gameplay & Character

- WASD / Left Stick movement adapted to the shared camera orientation.
- Dash system with cooldown.
- 4 distinct spells, each with unique base behaviors, fully customizable through items.
  - Knockback that pushes back players hit by projectiles.
  - Explosion effects dealing area damage and optionally applying knockback to all players inside the zone.
- Everything can be modified through items:
  - Player speed
  - Dash cooldown
  - Spell cooldowns
  - Knockback power
  - Explosion size
  - Projectile damage
  - Explosion damage

### Carousel & Item System

- Items are divided into 4 rarity/power tiers.
- Each item has a probability of appearing in a specific tier:
  - T1: 60%
  - T2: 25%
  - T3: 10%
  - T4: 5%
- Reverse pick priority based on ranking: the leading player has to wait longer before choosing.
- Items stack and persist between rounds.

### Arenas & Environmental Gameplay Elements

- **Instant Kill Zone** → the player dies immediately upon contact.
- **Drain Zone** → deals damage every X seconds.
- **Blades** → rotating blades dealing X damage every X seconds, while their collisions physically push players away on contact.

> We kept physical collision on the blades because using a simple `Launch Character` created inconsistencies with the visual feedback.

---

## Team

**The team currently working on this project**

### Programmer

- [@Lefleur05](https://github.com/Lefleur05)

### Game Designer

- [@AlixLFL](https://github.com/AlixLFL)
