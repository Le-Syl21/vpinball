# AltSound packs and `new_sound_commands`

[← Index](README.md#-english) · 🇬🇧 English · [🇫🇷 Français](altsound_fra.md)

For people who make AltSound packs. In 10.9.0 (PinMAME 3.7 and later) PinMAME logs the
sound commands of some older machines differently, and a pack made before that can stop
finding its sounds. One key in the pack's `altsound.ini` keeps both worlds working.

## The key

```ini
[system]
new_sound_commands = 0
```

| Value | What AltSound reads | For |
|---|---|---|
| `0`, or no key (the default) | the command stream and the pairing of bytes into 16-bit commands that older packs were made with | every pack made before 3.7 |
| `1` | only the real sound commands of the game, one byte each, as PinMAME 3.7 and later logs them | packs made for that stream, such as the ones rom2altsound writes |

Nobody has to touch an old pack: with no key it behaves as it always did. A new pack says
`1`, and the stream it was made for is then the one it gets.

## Which machines it concerns

The key changes how commands are read for Bally MPU-17/35 and for every machine whose
generation libaltsound does not handle on its own. WPC (DCS, security, 95, alpha, DMD,
Fliptronics), System 11, Data East, Whitestar and Gottlieb System 80A read their commands the
same way whatever the value.

Some machines only log their commands from the driver, so a pack for them must say `1`
(until libaltsound passes each command's source on): Capcom, Pinball 2000, Mr. Game, Hankin,
Atari, Playmatic Zira, Spinball and Inder, Zaccaria 1311.

## Which stream is my pack made for?

Ask AltSound to record what the game sends:

```ini
[system]
record_sound_cmds = 1
```

Play the game for a minute. A `cmdlog.txt` appears in the pack's folder
(`altsound/<game>/`), with the hardware generation in its header and one line per command
id, with how many times it was seen. Compare those ids with the first column of your
CSV:

- your ids are two-byte pairs that do not look like sounds (`0x0f05`, `0x060f`, `0x0f0f`):
  the pack was made for the older stream, leave `new_sound_commands` at `0` (or absent);
- your ids are single bytes, the real sound numbers of the game: set it to `1`.

Turn `record_sound_cmds` back to `0` when you are done: it is a recorder for debugging, not
needed to play a pack.

## What you will see if it is wrong

AltSound writes `No sample(s) found for ID: …` to its log for each command it cannot find.
A pack made for one byte per command, read as an old pack, finds almost nothing (seen on
Fathom, 1 lookup out of 22). The other way round, expect few or no matches too. Set the level of the log with `logging_level` in the same
file to see it.

## With rom2altsound

[rom2altsound](https://github.com/Le-Syl21/rom2altsound) always writes `new_sound_commands = 1`
(and merges it into an `altsound.ini` already in the folder, keeping the other keys and the
comments), because the ids it writes are the real one-byte commands.
