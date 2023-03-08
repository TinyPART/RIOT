tinycontainer
=============

This is a simple example of the usage of TinyContainer.

Usage
=====

To build the code, just select your board and l as usual:

```
BOARD=<YOUR_BOARD_NAME> make
```

You can use ```TINYCONTAINER_CONTAINER``` directive to select another runtime. By default the WebAssembly runtime is used.

```
TINYCONTAINER_CONTAINER=jerryscript make
```

The example adds some TinyContainer commands to the shell prompt:

* _load_, to load a container
* _start_, to start running it
* _stop_, to stop the container
* _status_, to check if the container is running or not
* _wait_, to let the container run a while

You can also use the _ps_ command to display a threads table.

The container is prebuilt and displays some messages to the console when running:

> WASM: start()
> WASM: loop n=1 of 100
> WASM: loop n=2 of 100
> ...
> WASM: loop n=100 of 100
> WASM: stop()

Supported Board
===============

| Board               | WebAssembly  | Jerryscript  |
|---------------------|:------------:|:------------:|
| native              | build & test | build only   |
| dwm1001             | build & test | build only   |
| arduino-nano-33-ble | build only   | build only   |
| nrf52840dk          | build & test | build only   |
| others              | :x:          | :x:          |


