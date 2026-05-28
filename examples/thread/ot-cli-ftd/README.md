# What is the ot-cli-ftd example?

The ot-cli-ftd example helps users get familiar with Thread FTD (Full Thread Device) using CLI commands. A Thread FTD can act as a Leader, Router, or Child in a Thread network, but it doesn’t support sleep mode, so it’s unsuitable for low-power devices.

The ot-cli-ftd can be configured to use either the 2.4GHz or Sub-GHz bands for communication and supports Thread UDP for user development.

# Quick Start

## Thread network parameter settings

* Device 1
    1. On Device 1, Generate and view new network configuration.

    ```bash
    > ot dataset init new
    Done
    > ot dataset
    Active Timestamp: 1
    Channel: 15
    Channel Mask: 0x07fff800
    Ext PAN ID: 39758ec8144b07fb
    Mesh Local Prefix: fdf1:f1ad:d079:7dc0::/64
    Network Key: f366cec7a446bab978d90d27abe38f23
    Network Name: OpenThread-5938
    PAN ID: 0x5938
    PSKc: 3ca67c969efb0d0c74a4d8ee923b576c
    Security Policy: 672 onrc 0
    Done
    ````
    2. On Device 1, Commit new dataset to the Active Operational Dataset in non-volatile storage.

    ```bash
    ot dataset commit active
    Done
    ````
    
    3. On Device 1, Enable Thread interface

    ```bash
    > ot mode rdn
    Done
    > ot ifconfig up
    Done
    > ot thread start
    Done
    ````
    
    4. On Device 1, Waiting to obtain a role.

    ```bash
    > [     51782][INFO  : app_task.c:  48] Current role       : detached
    [     51783][INFO  : app_task.c:  51] Extend Address     : 5038-3233-3245-3136
    [     51783][INFO  : app_task.c:  55] Local Prefx        : fd1e:5a75:7f08:ebac
    [     51784][INFO  : app_task.c:  59] IPv6 Address       : fe80:0000:0000:0000:5238:3233:3245:3136
    [     51785][INFO  : app_task.c:  62] Rloc16             : ec00
    [     51786][INFO  : app_task.c:  65] Rloc               : fd1e:5a75:7f08:ebac:0000:00ff:fe00:ec00
    [     86299][INFO  : app_task.c:  48] Current role       : leader
    [     86300][INFO  : app_task.c:  51] Extend Address     : 5038-3233-3245-3136
    [     86300][INFO  : app_task.c:  55] Local Prefx        : fd1e:5a75:7f08:ebac
    [     86301][INFO  : app_task.c:  59] IPv6 Address       : fe80:0000:0000:0000:5238:3233:3245:3136
    [     86302][INFO  : app_task.c:  62] Rloc16             : ec00
    [     86303][INFO  : app_task.c:  65] Rloc               : fd1e:5a75:7f08:ebac:0000:00ff:fe00:ec00
    ````

* Device 2 (Attach to Existing Network)
    1. On Device 2, Create a partial Active Operational Dataset.

    ```bash
    > ot dataset networkkey f366cec7a446bab978d90d27abe38f23
    Done
    > ot dataset commit active
    Done
    ````

    2. On Device 2, Enable Thread interface.

    ```bash
    > ot ifconfig up
    Done
    > ot thread start
    Done
    ````

    3. On Device 2, Waiting for role change after attachment completion.

    ```bash
    > [     29550][INFO  : app_task.c:  48] Current role       : detached
    [     29551][INFO  : app_task.c:  51] Extend Address     : 5038-3233-3245-4d37
    [     29551][INFO  : app_task.c:  55] Local Prefx        : fdde:ad00:beef:0000
    [     29552][INFO  : app_task.c:  59] IPv6 Address       : fe80:0000:0000:0000:5238:3233:3245:4d37
    [     29553][INFO  : app_task.c:  62] Rloc16             : ec01
    [     29554][INFO  : app_task.c:  65] Rloc               : fdde:ad00:beef:0000:0000:00ff:fe00:ec01
    [     59108][INFO  : app_task.c:  48] Current role       : child
    [     59109][INFO  : app_task.c:  51] Extend Address     : 5038-3233-3245-4d37
    [     59110][INFO  : app_task.c:  55] Local Prefx        : fd1e:5a75:7f08:ebac
    [     59111][INFO  : app_task.c:  59] IPv6 Address       : fe80:0000:0000:0000:5238:3233:3245:4d37
    [     59112][INFO  : app_task.c:  62] Rloc16             : ec01
    [     59112][INFO  : app_task.c:  65] Rloc               : fd1e:5a75:7f08:ebac:0000:00ff:fe00:ec01
    ````

## Thread UDP command transmission

* Device 1
  1. On Device 1, open and bind the example UDP socket.
  
  ```bash
  > ot udp open
  > ot udp bind :: 1234
  ````

  2. On Device 1, when it receives a UDP message will display.
  
  ```bash
  5 bytes from fd1e:5a75:7f08:ebac:5038:3233:3245:4d37 49153 hello
  ````

* Device 2
  1. On Device 2, open the example UDP socket and send a simple message.
  
  ```bash
  > ot udp open
  > ot udp send fdde:ad00:beef:0:bb1:ebd6:ad10:f33 1234 hello
  ````
