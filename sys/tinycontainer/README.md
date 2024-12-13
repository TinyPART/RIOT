# TinyContainer

TinyContainer implements a containerization framework that includes:

- a Service, in charge of running containers
- a Controller, to load and drive the lifecycle of containers
- a Memory Manager, to store containers
- some Security modules, to enforce security of containers

Three types of container runtimes are currently implemented: Web Assembly,
rBPF and JerryScript. Nevertheless, we are now focusing on Web Assembly and not
all feature are supported for the other two.

Currently the Memory Manager is implemented using a in-RAM backend.

And it worth nothing that implementation of Security modules are ongoing.

## usage

To use this module, add

```
USEMODULE += tinycontainer
TINYCONTAINER_RUNTIME ?= wamr
```

to your makefile. That activates the support for Web Assembly containers and the
RIOT build system will automatically includes all required sub-modules.

If you prefer using rBPF or JerryScript runtimes just adapt the
`TINYCONTAINER_RUNTIME` directive with a `rbpf` or `jerryscript` value.

## Security

TinyContainer support two kinds of crypto backend: *None* and *PSA*. The default
one is *None*. Please use the directive `TINYCONTAINER_CRYPTO` to select which
crypto backend to use with a `none` or `psa` value.

## License

Copyright (C) 2023-2024 Orange

This file is subject to the terms and conditions of the GNU Lesser
General Public License v2.1. See the file LICENSE in the top level
directory for more details.
