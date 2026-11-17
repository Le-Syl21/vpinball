# Packs AltSound et `new_sound_commands`

[← Index](README.md#-français) · [🇬🇧 English](altsound_eng.md) · 🇫🇷 Français

Pour ceux qui font des packs AltSound. Dans la 10.8.1 (PinMAME 3.7 et suivantes), PinMAME
journalise autrement les commandes son de certaines machines anciennes, et un pack fait avant
peut ne plus retrouver ses sons. Une clé dans l'`altsound.ini` du pack permet de garder les
deux cas.

## La clé

```ini
[system]
new_sound_commands = 0
```

| Valeur | Ce que lit AltSound | Pour |
|---|---|---|
| `0`, ou pas de clé (le défaut) | le flux de commandes et l'appariement des octets en commandes de 16 bits, tels que les anciens packs ont été faits | tout pack fait avant la 3.7 |
| `1` | seulement les vraies commandes son du jeu, un octet chacune, comme les journalise PinMAME 3.7 et suivantes | les packs faits pour ce flux, comme ceux que rom2altsound écrit |

Personne n'a à toucher un ancien pack : sans clé, il se comporte comme avant. Un nouveau pack
dit `1`, et il reçoit alors le flux pour lequel il a été fait.

## Les machines concernées

La clé change la lecture des commandes pour Bally MPU-17/35 et pour toute machine dont
libaltsound ne traite pas la génération à part. WPC (DCS, security, 95, alpha, DMD,
Fliptronics), System 11, Data East, Whitestar et Gottlieb System 80A lisent leurs commandes de
la même façon quelle que soit la valeur.

Certaines machines ne journalisent leurs commandes que depuis le pilote : un pack pour elles
doit dire `1` (tant que libaltsound ne reçoit pas l'origine de chaque commande) : Capcom,
Pinball 2000, Mr. Game, Hankin, Atari, Playmatic Zira, Spinball et Inder, Zaccaria 1311.

## Pour quel flux mon pack est-il fait ?

Demandez à AltSound d'enregistrer ce que le jeu envoie :

```ini
[system]
record_sound_cmds = 1
```

Jouez une minute. Un fichier `cmdlog.txt` apparaît dans le dossier du pack
(`altsound/<jeu>/`), avec la génération matérielle dans son en-tête et une ligne par
identifiant de commande, avec le nombre de fois où il a été vu. Comparez ces identifiants avec la première
colonne de votre CSV :

- vos identifiants sont des paires de deux octets qui ne ressemblent pas à des sons
  (`0x0f05`, `0x060f`, `0x0f0f`) : le pack est fait pour l'ancien flux, laissez
  `new_sound_commands` à `0` (ou absent) ;
- vos identifiants sont des octets seuls, les vrais numéros de son du jeu : mettez `1`.

Remettez `record_sound_cmds` à `0` ensuite : c'est un enregistreur de débogage, inutile pour
jouer un pack.

## Ce que vous verrez si c'est faux

AltSound écrit `No sample(s) found for ID: …` dans son journal pour chaque commande qu'il ne
trouve pas. Un pack fait pour un octet par commande, lu comme un ancien pack, ne trouve presque
rien (vu sur Fathom : 1 consultation sur 22). Dans l'autre sens, attendez-vous aussi à peu
de correspondances, voire aucune. Réglez le niveau du journal avec
`logging_level` dans le même fichier pour le voir.

## Avec rom2altsound

[rom2altsound](https://github.com/Le-Syl21/rom2altsound) écrit toujours
`new_sound_commands = 1` (et le fusionne dans un `altsound.ini` déjà présent dans le dossier,
en gardant les autres clés et les commentaires), parce que les identifiants qu'il écrit sont les
vraies commandes d'un octet.
