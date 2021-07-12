# Bash Completion For ALSA utilities

A bash completion script for a selection of tools from the alsa-utils repository of the ALSA project (https://github.com/alsa-project/alsa-utils).

Includes a trivial executable `alsa-list-ctls` that lists the mixer devices (ctl devices in the ALSA config) as there appears to be no equivalent to `aplay -L` for listing CTLs.

## Installation

```shell
meson setup [meson-options ...] builddir
ninja -C builddir
sudo ninja -C builddir install
```

