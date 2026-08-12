# VPinballX 10.8.1 — ce qui a changé, et comment le configurer

[🇬🇧 English](README.md) · 🇫🇷 Français

La 10.8.1 est en développement, et plusieurs sous-systèmes ont été réécrits
plutôt qu'étendus. Des réglages qui fonctionnaient n'existent plus, et ceux qui
les remplacent ne sont documentés nulle part — ce qui laisse aujourd'hui les
propriétaires de cabinets faire de la rétro-ingénierie sur leur propre
`VPinballX.ini`.

Ces pages documentent ce qui a été vérifié dans le code source, avec les fichiers
et fonctions à consulter pour recouper.

## Chapitres

- **[Entrées, nudge et tilt](input_fra.md)** — le schéma de mapping des capteurs
  et ses six champs, les échelles d'unité, les trois modes de nudge, le seuil
  d'intention codé en dur, et le pendule de tilt.
- **[Vue et ajustement cabinet](view_fra.md)** — la projection Window, les deux
  modes d'ajustement automatique, et les hauteurs de vitre dont dépend le
  cadrage.

## Commencez ici si…

**votre plongeur bouge à peine la bille.** Le `scale` d'un mapping est une
conversion d'unité, et il vaut 1.0 par défaut — voir
[Entrées](input_fra.md#léchelle-est-une-unité-pas-une-sensibilité).

**votre nudge ne fait rien, ou tout.** La sensibilité, c'est `Strength`, pas
`scale`, et le mode intention écarte tout ce qui passe sous 1 m/s² codé en dur —
voir [Entrées](input_fra.md#les-trois-modes-de-nudge).

**votre table ne tilte jamais, même secouée violemment.** Au-delà d'un certain
seuil, la secousse nécessaire dépasse ce que votre capteur sait mesurer — voir
[Entrées](input_fra.md#tilt).

**votre table tombe mal sur un écran de cabinet.** Le cadrage se construit sur
les hauteurs de vitre de la table, que VPX devine quand elle ne les déclare pas —
voir [Vue](view_fra.md#ce-que-lajustement-attend-de-la-table).

**vos réglages survivent à une mise à jour mais cessent de fonctionner.** Les
mappings par défaut, qui portent les bonnes échelles d'unité, ne sont installés
qu'à l'auto-détection : une configuration existante conserve ses anciennes
valeurs — voir [Entrées](input_fra.md#périphériques).

## Portée

Écrit à partir de la branche de développement 10.8.1. Tout ce qui figure ici peut
encore changer avant la sortie ; les références au code source sont là pour
qu'un lecteur puisse vérifier si c'est le cas.

Corrections et ajouts bienvenus — en particulier pour les sous-systèmes non
encore couverts (audio, plugins, PinMAME, PUP).
