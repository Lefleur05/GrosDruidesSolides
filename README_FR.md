[English Version](https://github.com/Lefleur05/GrosDruidesSolides/blob/main/README.md)
# Gros Druides Solides
Party Game multijoueur en ligne de 2 à 4 joueurs, développé sur Unreal Engine 5.
Inspiré des jeux MageQuit et Warlock.
> Projet programmé en solo - je suis l'unique programmeur du jeu.

[▶ Voir la démonstration](https://youtu.be/I_6DkkSAsvM)
[![Démonstration](https://img.youtube.com/vi/I_6DkkSAsvM/maxresdefault.jpg)](https://youtu.be/I_6DkkSAsvM)


## Présentation rapide
-  Genre: Party Game, Battle Arena
-  Joueurs: 2 à 4 en ligne
-  Moteur: Unreal Engine 5.6.1 (C++ et Blueprint)
-  Durée d'une partie: 3 à 10 minutes
Les joueurs incarnent des druides gaulois farfelus dans un univers inspiré d'Astérix. Le premier à atteindre 3 points de victoire gagne.
Entre chaque manche de combat, une phase de Carrousel (inspirée de TFT) permet à chaque joueur de choisir un objet qui modifie ses sorts
— générant des builds différents à chaque partie.
## Ce que j'ai développé
### Multijoueur
- Un système de session permettant de rejoindre des joueurs directement par le jeu via un menu Find Session.
- Mais aussi la possibilité de rejoindre via des amis Steam grâce aux invitations Steam.
### Caméra partagée dynamique
- Caméra commune qui bouge et s'éloigne pour constamment garder tous les joueurs en vue.
- Ignore les joueurs morts.
### Système de projectiles modulaire
- Chaque sort est entièrement piloté par une structure de données `FItem` (dégâts, vitesse, portée, cône de tir, knockback, explosion, annulation de sorts…)
- Les objets ramassés s'additionnent directement à cette structure afin que dès que le joueur récupère un item il modifie directement le sort sur lequel il fait effet.
### Gameplay et Character
- Déplacement ZQSD/Joystick gauche qui s'adapte à la caméra commune.
- Dash avec un cooldown.
- 4 sorts distincts avec des comportements de base uniques à chacun, entièrement modifiables grâce aux items.
  - Knockback qui repousse le joueur touché par le projectile.
  - Explosion qui fait des dégâts au joueur dans la zone et si activée knockback tous les joueurs dans la zone.
- Le tout est modifiable par les items : vitesse du joueur, cooldown du dash, des sorts, puissance du knockback, taille de l'explosion, dégâts du projectile, dégâts de l'explosion...
### Système de Carrousel et d'objets
- Les items ont 4 tiers de rareté/puissance.
- Chaque item a une probabilité d'être un certain tier :
  - T1 : 60%
  - T2 : 25%
  - T3 : 10%
  - T4 : 5%
- Ordre de priorité inversé au classement : le leader attend plus longtemps pour choisir.
- Les objets se cumulent et persistent entre les manches.
### Arènes et GPE
- Instant Kill Zone -> le joueur meurt immédiatement quand il entre en contact avec.
- Drain Zone -> inflige des dégâts toutes les X secondes.
- Blades -> Lames rotatives infligeant X dégâts toutes les X secondes, mais avec ces lames qui ont des collisions repoussent les joueurs qui entrent en contact.
  > Nous avons gardé la collision des lames pour repousser le joueur car si nous faisions un simple `Launch Character` cela rendait incohérent avec le visuel.
## Team
**The team currently working on this project**  
**Programmeur**
- [@Lefleur05](https://github.com/Lefleur05)

**Game Designer**
- [@AlixLFL](https://github.com/AlixLFL)
