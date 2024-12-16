tinycontainer
=============

This application example demonstrate how to use TinyContainer.

Use Cases
=========

This example supports two use cases:

* **built-in container management**, utilizing a shell UI and statically
  links containers.
* **OTA container management**, leveraging CoAP messages over the IPv6/BLE
  network stack supported by RIOT

Usage
=====

To ease all operations we provide a cli tool named `tinycontainerctl`. It can be
used to:

* Ronfigure TinyContainer
* Create containers
* Create endpoints (comming soon)
* Build the system

To list all available commands, simply execute it without any argument:

```
./tinycontainerctl
```

Below are the commands to run the default example on a dwm1001 board:

```
./tinycontainer setup init default
./tinycontainer generate all flash term
```

The example adds several TinyContainer commands to the shell prompt:

* _list_, to display built-in containers
* _load_, to load a container
* _unload_, to unload a container
* _start_, to start running it
* _stop_, to halt the container
* _status_, to check if a container is running
* _wait_, to allow the container run for a while

You can also use the _ps_ command to display a table of threads.

Supported Board
===============

The tables below show the number of containers supported in different
configurations for each supported boards.

The first characters can be:

- '1', if we have successfully tested the configuration with at least one
  container.
- '.' if we have successfully built the configuration with at least one
  container.
- 'x' if the configuration could not run on at least one container.

The second characters use the symbol '2', '.' and 'x' in a similar way. And so
on for next characters.

with network and crypto deactivated
-----------------------------------

| Board                     | WebAssembly  | rBPF         | Jerryscript  |
|---------------------------|:------------:|:------------:|:------------:|
| native                    | `........`   | `........`   | `........`   |
| dwm1001                   | `1.....xx`   | `........`   | `........`   |
| nrf52840dk                | `........`   | `........`   | `........`   |
| arduino-nano-33-ble       | `1.......`   | `........`   | `........`   |
| arduino-nano-33-ble-sense | `........`   | `........`   | `........`   |
| nrf9160dk                 | `........`   | `........`   | `........`   |
| others                    | :x:          | :x:          | :x:          |

with network deactivated and crypto activated
---------------------------------------------

| Board                     | WebAssembly  | rBPF         | Jerryscript  |
|---------------------------|:------------:|:------------:|:------------:|
| native                    | `........`   | `........`   | `........`   |
| dwm1001                   | `1.....xx`   | `........`   | `........`   |
| nrf52840dk                | `........`   | `........`   | `........`   |
| arduino-nano-33-ble       | `xxxxxxxx`   | `xxxxxxxx`   | `xxxxxxxx`   |
| arduino-nano-33-ble-sense | `xxxxxxxx`   | `xxxxxxxx`   | `xxxxxxxx`   |
| nrf9160dk                 | `........`   | `........`   | `........`   |
| others                    | :x:          | :x:          | :x:          |

with network activated and crypto deactivated
------------------------------------------------

| Board                     | WebAssembly  | rBPF         | Jerryscript  |
|---------------------------|:------------:|:------------:|:------------:|
| native                    | `xxxxxxxx`   | `xxxxxxxx`   | `xxxxxxxx`   |
| dwm1001                   | `1.....xx`   | `........`   | `........`   |
| nrf52840dk                | `........`   | `........`   | `........`   |
| arduino-nano-33-ble       | `........`   | `........`   | `........`   |
| arduino-nano-33-ble-sense | `........`   | `........`   | `........`   |
| nrf9160dk                 | `xxxxxxxx`   | `xxxxxxxx`   | `xxxxxxxx`   |
| others                    | :x:          | :x:          | :x:          |

with network and crypto activated
---------------------------------

| Board                     | WebAssembly  | rBPF         | Jerryscript  |
|---------------------------|:------------:|:------------:|:------------:|
| native                    | `xxxxxxxx`   | `xxxxxxxx`   | `xxxxxxxx`   |
| dwm1001                   | `..xxxxxx`   | `....xxxx`   | `xxxxxxxx`   |
| nrf52840dk                | `........`   | `........`   | `........`   |
| arduino-nano-33-ble       | `xxxxxxxx`   | `xxxxxxxx`   | `xxxxxxxx`   |
| arduino-nano-33-ble-sense | `xxxxxxxx`   | `xxxxxxxx`   | `xxxxxxxx`   |
| nrf9160dk                 | `xxxxxxxx`   | `xxxxxxxx`   | `xxxxxxxx`   |
| others                    | :x:          | :x:          | :x:          |

Networking
==========

By default, the network isn't activated. To build with networking capability
configure it with following command:

```
./tinycontainerctl setup set network on
```

Once activated, the device announces itself using the string
_TinyContainerNetwork_ as BLE node ID and can be remotely managed through CoAP
commands.

The supported CoAP commands are describes in the file
```TinyContainerNetwork.md```.

License
=======

Copyright (C) 2023-2024 Orange

This file is subject to the terms and conditions of the GNU Lesser
General Public License v2.1. See the file LICENSE in the top level
directory for more details.
