# VPinballX 10.8.1 — configuration

🇬🇧 [English](#-english) | 🇫🇷 [Français](#-français)

---

## 🇬🇧 English

10.8.1 is in development, and several subsystems were rewritten rather than
extended. Settings that used to work no longer exist, and the ones that replaced
them are not documented anywhere — which currently leaves cabinet owners
reverse-engineering their own `VPinballX.ini`.

These pages document what was verified in the source, with the files and
functions to check against.

### Chapters

- **[Input, nudge and tilt](input_eng.md)** — the sensor mapping schema and its
  six fields, unit scales, the three nudge modes, the hardcoded intent
  threshold, and the plumb.
- **[View and cabinet fitting](view_eng.md)** — the Window projection, the two
  autofit modes, and the glass heights the frame is built from.

### Start here if…

- **your plunger barely moves the ball** — the mapping's `scale` is a unit
  conversion, and it defaults to 1.0:
  [Input](input_eng.md#the-scale-is-a-unit-not-a-sensitivity)
- **your nudge does nothing, or everything** — sensitivity is `Strength`, not
  `scale`, and Intent mode discards anything under a hardcoded 1 m/s²:
  [Input](input_eng.md#the-three-nudge-modes)
- **your table never tilts however hard you shake** — past a certain threshold
  the shove needed exceeds what your sensor can report: [Input](input_eng.md#tilt)
- **your table sits badly on a cabinet screen** — the frame is built from the
  table's glass heights, which VPX guesses when the table does not declare them:
  [View](view_eng.md#what-autofit-needs-from-the-table)
- **your settings survive an upgrade but stop working** — the default mappings
  carrying the correct unit scales are only installed on auto-detection, so an
  existing config keeps its old values: [Input](input_eng.md#devices)

### Scope

Written from the 10.8.1 development branch. Anything here can still change before
release; the source references are there so a reader can check whether it has.

Corrections and additions welcome — particularly for the subsystems not covered
yet (audio, plugins, PinMAME, PUP).

---

## 🇫🇷 Français

La 10.8.1 est en développement, et plusieurs sous-systèmes ont été réécrits
plutôt qu'étendus. Des réglages qui fonctionnaient n'existent plus, et ceux qui
les remplacent ne sont documentés nulle part — ce qui laisse aujourd'hui les
propriétaires de cabinets faire de la rétro-ingénierie sur leur propre
`VPinballX.ini`.

Ces pages documentent ce qui a été vérifié dans le code source, avec les fichiers
et fonctions à consulter pour recouper.

### Chapitres

- **[Entrées, nudge et tilt](input_fra.md)** — le schéma de mapping des capteurs
  et ses six champs, les échelles d'unité, les trois modes de nudge, le seuil
  d'intention codé en dur, et le pendule de tilt.
- **[Vue et ajustement cabinet](view_fra.md)** — la projection Window, les deux
  modes d'ajustement automatique, et les hauteurs de vitre dont dépend le
  cadrage.

### Commencez ici si…

- **votre plongeur bouge à peine la bille** — le `scale` d'un mapping est une
  conversion d'unité, et il vaut 1.0 par défaut :
  [Entrées](input_fra.md#léchelle-est-une-unité-pas-une-sensibilité)
- **votre nudge ne fait rien, ou tout** — la sensibilité, c'est `Strength`, pas
  `scale`, et le mode intention écarte tout ce qui passe sous 1 m/s² codé en
  dur : [Entrées](input_fra.md#les-trois-modes-de-nudge)
- **votre table ne tilte jamais, même secouée violemment** — au-delà d'un certain
  seuil, la secousse nécessaire dépasse ce que votre capteur sait mesurer :
  [Entrées](input_fra.md#tilt)
- **votre table tombe mal sur un écran de cabinet** — le cadrage se construit sur
  les hauteurs de vitre de la table, que VPX devine quand elle ne les déclare
  pas : [Vue](view_fra.md#ce-que-lajustement-attend-de-la-table)
- **vos réglages survivent à une mise à jour mais cessent de fonctionner** — les
  mappings par défaut, qui portent les bonnes échelles d'unité, ne sont installés
  qu'à l'auto-détection : une configuration existante conserve ses anciennes
  valeurs : [Entrées](input_fra.md#périphériques)

### Portée

Écrit à partir de la branche de développement 10.8.1. Tout ce qui figure ici peut
encore changer avant la sortie ; les références au code source sont là pour qu'un
lecteur puisse vérifier si c'est le cas.

Corrections et ajouts bienvenus — en particulier pour les sous-systèmes non
encore couverts (audio, plugins, PinMAME, PUP).
