# VPinballX 10.8.1 — what changed, and how to configure it

🇬🇧 English · [🇫🇷 Français](README_fra.md)

10.8.1 is in development, and several subsystems were rewritten rather than
extended. Settings that used to work no longer exist, and the ones that replaced
them are not documented anywhere — which currently leaves cabinet owners
reverse-engineering their own `VPinballX.ini`.

These pages document what was verified in the source, with the files and
functions to check against.

## Chapters

- **[Input, nudge and tilt](input_eng.md)** — the sensor mapping schema and its
  six fields, unit scales, the three nudge modes, the hardcoded intent
  threshold, and the plumb.
- **[View and cabinet fitting](view_eng.md)** — the Window projection, the two
  autofit modes, and the glass heights the frame is built from.

## Start here if…

**your plunger barely moves the ball.** The mapping's `scale` is a unit
conversion, and it defaults to 1.0 — see [Input](input_eng.md#the-scale-is-a-unit-not-a-sensitivity).

**your nudge does nothing, or everything.** Sensitivity is `Strength`, not
`scale`, and Intent mode discards anything under a hardcoded 1 m/s² — see
[Input](input_eng.md#the-three-nudge-modes).

**your table never tilts however hard you shake.** Past a certain threshold the
shove needed exceeds what your sensor can report — see [Input](input_eng.md#tilt).

**your table sits badly on a cabinet screen.** The frame is built from the
table's glass heights, which VPX guesses when the table does not declare them —
see [View](view_eng.md#what-autofit-needs-from-the-table).

**your settings survive an upgrade but stop working.** The default mappings that
carry the correct unit scales are only installed on auto-detection, so an
existing config keeps its old values — see [Input](input_eng.md#devices).

## Scope

Written from the 10.8.1 development branch. Anything here can still change before
release; the source references are there so a reader can check whether it has.

Corrections and additions welcome — particularly for the subsystems not covered
yet (audio, plugins, PinMAME, PUP).
