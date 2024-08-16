
CoAP specification for TinyContainerNetwork
============================================

TinyContainer IoT devices that advertising the ble name _TinyContainerNetwork_ can be managed using CoAP.

CoAP URI
--------

All TinyContainer actions on the IoT are made using the following CoAP Request URI:

    coap://[ipv6_address]/container/

IoT nodes of TinyContainerNetwork also support an URI to request or update an IoT device identifier.

    coap://[ipv6_address]/device-id/

CoAP Request Method
-------------------

This paragraph defines the set of CoAP Method that is used for TinyContainerNetwork.

    GET     = 0.01, as stated in RFC 7252
    POST    = 0.02, as stated in RFC 7252
    PUT     = 0.03, as stated in RFC 7252
    DELETE  = 0.04, as stated in RFC 7252

The section [CoAP Messages](#coap-messages-for-tinycontainernetwork) details which CoAP method shall be used for each supported messages.

CoAP Response Code
------------------

This paragraph defines the set of CoAP Response Code that is used for TinyContainerNetwork.

    COAP_CODE_CREATE                   = 2.01, as stated in RFC 7252
    COAP_CODE_CHANGED                  = 2.04, as stated in RFC 7252
    COAP_CODE_CONTENT                  = 2.05, as stated in RFC 7252
    COAP_CODE_BAD_OPTION               = 4.02, as stated in RFC 7252
    COAP_CODE_PATH_NOT_FOUND           = 4.04, as stated in RFC 7252
    COAP_CODE_METHOD_NOT_ALLOWED       = 4.05, as stated in RFC 7252
    COAP_CODE_REQUEST_ENTITY_TOO_LARGE = 4.13, as stated in RFC 7252
    COAP_CODE_INTERNAL_SERVER_ERROR    = 5.00, as stated in RFC 7252

The section [CoAP Messages](#coap-messages-for-tinycontainernetwork) details which CoAP Response Code shall be used for each supported messages.

CoAP Options
------------

This paragraph defines the set of CoAP Option that is used for TinyContainerNetwork.

    COAP_OPT_SIZE1 = 60, as stated in RFC 7252

We have defined some new CoAP Option. To conform with RFC7254 their number have been chosen in the reserved range for experiments.

    OPTION_ACTION   = 65001, with a value of uint type
    OPTION_METADATA = 65002, with a value of opaque type
    OPTION_DATA     = 65003, with a value of opaque type
    OPTION_CODE     = 65004, with a value of opaque type
    OPTION_UID      = 65005, with a value of opaque type
    OPTION_STATUS   = 65006, with a value of uint type

The section [CoAP Messages](#coap-messages-for-tinycontainernetwork) details which CoAP Options shall be used for each supported messages.

When the type of value of a CoAP Option is _opaque_, the IoT parses it as an array of bytes.
When the type of value of a CoAP Option is _uint_, the defined values are described below.

### Values for ```OPTION_ACTION```

The possible values for ```OPTION_ACTION``` are encoded as:

    ACTION_LOAD       = 1, for a container loading
    ACTION_START      = 2, for starting a container
    ACTION_IS_RUNNING = 3, for checking if a container run
    ACTION_STOP       = 4, for stopping a container
    ACTION_DELETE     = 5, for deleting a container

Other values are reserved and shall not be used.

### Values for ```OPTION_STATUS```

The possible values for ```OPTION_STATUS``` are encoded as:

    STATUS_NOT_RUN = 0, when the container is not running
    STATUS_RUN     = 1, when the container is running

Other values are reserved and shall not be used.

CoAP Messages for TinyContainerNetwork
--------------------------------------

This section details the format of CoAP request/response pair for each actions.

### Retrieve the IoT device identifier

#### CoAP Request method:

* ```GET```

#### CoAP Request URI:

* ```/device-id/```

#### CoAP Options in request:

_none_

#### CoAP Response:

* ```COAP_CODE_CONTENT```, the payload contents the IoT device identifier
* ```COAP_CODE_PATH_NOT_FOUND```, no identifier has been assigned yet
* ```COAP_CODE_INTERNAL_SERVER_ERROR```, if the device is out of memory

####  CoAP Options in response:

_none_

### Update of the IoT device identifier

#### CoAP Request method:

* ```POST```, with the payload set to the new identifier of the IoT device

#### CoAP Request URI:

* ```/device-id/```

#### CoAP Options in request:

_none_

#### CoAP Response:

* ```COAP_CODE_CHANGED```, if identifier has been updated
* ```COAP_CODE_BAD_REQUEST```, if payload is empty
* ```COAP_CODE_REQUEST_ENTITY_TOO_LARGE```, the provided identifier is too long and the CoAP Option ```COAP_OPT_SIZE1``` is set in response

####  CoAP Options in response:

* ```COAP_OPT_SIZE1```, used in response to provide the maximal size of the identifier supported by the IoT device

### Load a container

#### CoAP Request method:

* ```POST```

#### CoAP Request URI:

* ```/container/```

##### CoAP Options in request:

* ```OPTION_ACTION```, set to ```ACTION_LOAD```
* ```OPTION_METADATA```, container metadata, as CBOR encoded
* ```OPTION_DATA```, container data as raw binary
* ```OPTION_CODE```, container code as raw binary

#### CoAP Response:

* ```COAP_CODE_CREATE```, the container has been correctly loaded
* ```COAP_CODE_METHOD_NOT_ALLOWED```, if method is not ```POST```
* ```COAP_CODE_BAD_OPTION```, if metadata, data or code are missing
* ```COAP_CODE_INTERNAL_SERVER_ERROR```, if container can't be loaded

####  CoAP Options in response:

_none_

### Start a container

##### CoAP Request method:

* ```PUT```

#### CoAP Request URI:

* ```/container/```

##### CoAP Options in request:

* ```OPTION_ACTION```, set to ```ACTION_START```
* ```OPTION_UID```, container UID, as raw binary

##### CoAP Response:

* ```COAP_CODE_CHANGED```, the container has been started
* ```COAP_CODE_METHOD_NOT_ALLOWED```, if method is not ```PUT```
* ```COAP_CODE_BAD_OPTION```, if the container uid is missing
* ```COAP_CODE_INTERNAL_SERVER_ERROR```, if container can't be started

####  CoAP Options in response:

_none_

### Check if a container is running

#### CoAP Request method:

* ```GET```

#### CoAP Request CoAP Request URI:

* ```/container/```

#### CoAP Options in request:

* ```OPTION_ACTION```, set to ```ACTION_IS_RUNNING```
* ```OPTION_UID```, container UID, as raw binary
* ```OPTION_STATUS```, in response, set to the container status

#### CoAP Response:

* ```COAP_CODE_CONTENT```, the container status is set in the option OPTION_STATUS
* ```COAP_CODE_METHOD_NOT_ALLOWED```, if method is not ```GET```
* ```COAP_CODE_BAD_OPTION```, if the container uid is missing

####  CoAP Options in response:

_none_

### Stop a container

#### CoAP Request method:

* ```PUT```

#### CoAP Request URI:

* ```/container/```

#### CoAP Options in request:

* ```OPTION_ACTION```, set to ```ACTION_STOP```

#### CoAP Response:

* ```COAP_CODE_METHOD_NOT_ALLOWED```, if method is not ```PUT```
* ```COAP_CODE_NOT_IMPLEMENTED```, this method is not yet implemented

####  CoAP Options in response:

_none_

### Delete a container

#### CoAP Request method:

* ```DELETE```

#### CoAP Request URI:

* ```/container/```

####  CoAP Options in request:

* ```OPTION_ACTION```, set to ```ACTION_DELETE```

#### CoAP Response:

* ```COAP_CODE_METHOD_NOT_ALLOWED```, if method is not ```DELETE```
* ```COAP_CODE_NOT_IMPLEMENTED```, this method is not yet implemented

####  CoAP Options in response:

_none_

### Any other value for OPTION_ACTION

#### CoAP Request URI:

* ```/container/```

#### CoAP Response:

* ```COAP_CODE_BAD_OPTION```

####  CoAP Options in response:

_none_

---
