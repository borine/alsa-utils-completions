# Bash Completion For ALSA utilities

bash completion scripts for a selection of tools from the alsa-utils repository of the ALSA project (https://github.com/alsa-project/alsa-utils).

Includes a trivial executable `alsa-list` that lists various device names from the ALSA configuration and also card names.

## Installation

```shell
meson setup [meson-options ...] builddir
ninja -C builddir
sudo ninja -C builddir install
```

