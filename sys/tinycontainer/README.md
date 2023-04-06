# TinyContainer

TinyContainer implements a containerization framework that includes:

- a Service, in charge of running containers
- a Controller, to load and manage containers lifecycle
- a Memory Manager, to store containers
- a Firewall, to enforce security of containers

Two type of container is currently implemented: Web Assembly and JerryScript.
Currently the Memory Manager implements a RAM backend.
And, the Firewall, is not fully implemented.

## usage

To use this module, add

```
USEMODULE += tinycontainer
TINYCONTAINER_CONTAINER ?= wamr
```

to your makefile to use WAMR containers. All requires sub-modules will be included.
If you prefer using JerryScript containers add

```
USEMODULE += tinycontainer
TINYCONTAINER_CONTAINER ?= jerryscript
```

to your makefile.

## License

Please refer to the file LICENSE.
