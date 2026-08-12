# VPinballX 10.8.1 — what changed, and how to configure it

10.8.1 is in development, and several subsystems were rewritten rather than
extended. Settings that used to work no longer exist, and the ones that replaced
them are not documented anywhere — which currently leaves cabinet owners
reverse-engineering their own `VPinballX.ini`.

These pages document what was verified in the source, with the files and
functions to check against.

## Chapters

| | 🇬🇧 English | 🇫🇷 Français |
|---|---|---|
| **Input, nudge, tilt** — the sensor mapping schema, unit scales, the three nudge modes, the plumb | [input_eng.md](input_eng.md) | [input_fra.md](input_fra.md) |
| **View and cabinet fitting** — the Window projection, autofit, glass heights | [view_eng.md](view_eng.md) | [view_fra.md](view_fra.md) |

## Start here if…

**your plunger barely moves the ball.** The mapping's `scale` is a unit
conversion, and it defaults to 1.0 — see *Input*, "The scale is a unit, not a
sensitivity".

**your nudge does nothing, or everything.** Sensitivity is `Strength`, not
`scale`, and the Intent mode discards anything under a hardcoded 1 m/s² — see
*Input*, "The three nudge modes".

**your table never tilts however hard you shake.** Past a certain threshold the
shove needed exceeds what your sensor can report — see *Input*, "Tilt".

**your table sits badly on a cabinet screen.** The frame is built from the
table's glass heights, which VPX guesses when the table does not declare them —
see *View*, "What autofit needs from the table".

**your settings survive an upgrade but stop working.** The default mappings that
carry the correct unit scales are only installed on auto-detection, so an
existing config keeps its old values — see *Input*, "Devices".

## Scope

Written from the 10.8.1 development branch. Anything here can still change before
release; the source references are there so a reader can check whether it has.

Corrections and additions welcome — particularly for the subsystems not covered
yet (audio, plugins, PinMAME, PUP).
