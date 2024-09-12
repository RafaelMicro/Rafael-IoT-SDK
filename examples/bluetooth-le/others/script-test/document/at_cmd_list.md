# AT cmd Command
- [AT cmd Command](#at-cmd-command)
- [Revision Map](#revision-map)
- [Address](#address)
  - [+DADDR](#daddr)
  - [+DEVADDR](#devaddr)
  - [+DEVADDRTYPE](#devaddrtype)
- [Advertising](#advertising)
  - [+ADVCHMAP](#advchmap)
  - [+ADVDATA](#advdata)
  - [+ADVFP](#advfp)
  - [+ADVINT](#advint)
  - [+ADVTYPE](#advtype)
  - [+DISADV](#disadv)
  - [+ENADV](#enadv)
  - [+SCANRSP](#scanrsp)
- [Connect](#connect)
  - [+CONINT](#conint)
  - [+CONLAT](#conlat)
  - [+CONPARAM](#conparam)
  - [+CONSUPTMO](#consuptmo)
  - [+DISCON](#discon)
  - [+READCONINT](#readconint)
  - [+READCONLAT](#readconlat)
  - [+READCONSUPTMO](#readconsuptmo)
- [Connect_Other](#connect_other)
  - [+SBOND](#sbond)
  - [+PLDATALEN](#pldatalen)
  - [+IBOND](#ibond)
  - [+PHY](#phy)
  - [+PREPLDATALEN](#prepldatalen)
  - [+PFMTUS](#pfmtus)
  - [+RMTUS](#rmtus)
  - [+READPHY](#readphy)
  - [+RSCCCD](#rscccd)
  - [+READRSSI](#readrssi)
  - [+SEC](#sec)
- [Other](#other)
  - [+ALLPARAM](#allparam)
  - [+RFRST](#rfrst)
  - [+ROLE](#role)
  - [+STSTART](#ststart)
- [Scanning](#scanning)
  - [+DISSCAN](#disscan)
  - [+ENSCAN](#enscan)
  - [+PARSADV](#parsadv)
  - [+PARSSCAN](#parsscan)
  - [+SCANFP](#scanfp)
  - [+SCANINT](#scanint)
  - [+SCANTYPE](#scantype)
  - [+SCANWIN](#scanwin)
- [Master](#master)
  - [+CHECKERRORRSP](#checkerrorrsp)
  - [+EXMTUS](#exmtus)
  - [+READ](#read)
  - [+READCCCD](#readcccd)
  - [+READDEVAP](#readdevap)
  - [+READDEVNAME](#readdevname)
  - [+READPCONPAR](#readpconpar)
  - [+WRITE](#write)
  - [+WRITEAUTHE](#writeauthe)
  - [+WRITEAUTHO](#writeautho)
  - [+WRITEENC](#writeenc)
  - [+WRITENRSP](#writenrsp)
- [create_connect](#create_connect)
  - [+CCCON](#cccon)
  - [+CRCON](#crcon)
  - [+CCONINT](#cconint)
  - [+CCONLAT](#cconlat)
  - [+CCONSUPTMO](#cconsuptmo)
- [Slave](#slave)
  - [+DEVAP](#devap)
  - [+DEVNAME](#devname)
  - [+PCONPAR](#pconpar)
  - [+IND](#ind)
  - [+NFY](#nfy)
  - [+SETERRORCODE](#seterrorcode)
  - [+SETREADVAL](#setreadval)

# Revision Map
- This document is mapping to AT_Cmd module revision 5808

# Address

## +DADDR

- +DADDR?  
  get the default address for +CRCON, +ENSCAN  
- +DADDR = `<addr>`  
    - `<addr>` : the default address  
- +DADDR = `<num>`,`<addr>`  
  set the default address for +CRCON, +ENSCAN  
    - `<num>` : the type of default address  
    - `<addr>` : the default address  
      - format : XX:XX:XX:XX:XX:XX  
      - ex. 01:02:03:04:05:FF, addr[0] = 0xFF, addr[5] = 0x01  
    - notice  
      If BLE Address Type is set to ramdom address, addr[5] >= 0xC0 (the two most significant bits of the address shall be equal to 1).  

## +DEVADDR

- +DEVADDR?  
  get the address of device  
- +DEVADDR = `<addr>`  
  set the address of device  
  - `<addr>` : the address of device  
    - format : XX:XX:XX:XX:XX:XX  
      - ex.01:02:03:04:05:FF, addr[0] = 0xFF, addr[5] = 0x01  
  - notice  
    If BLE Address Type is set to ramdom address,  
    addr[5] >= 0xC0(the two most significant bits  
    of the address shall be equal to 1).  

## +DEVADDRTYPE

- +DEVADDRTYPE?  
  get the address type of device  
- +DEVADDRTYPE = `<num>`  
  set the address type of device  
  - `<num>` : the address type of device  
    0: public address  
    1: ramdom address  
  - notice  
    If BLE Address Type is set to ramdom address,  
    addr[5] >= 0xC0(the two most significant bits  
    of the address shall be equal to 1).  

# Advertising

## +ADVCHMAP

- +ADVCHMAP?  
  get the advertising channel  
- +ADVCHMAP = `<num>`  
  set the advertising channel  
    - `<num>` : the advertising channel  
      range : 0-7  
      bit1: channel 37  
      bit2: channel 38  
      bit3: channel 39  

## +ADVDATA

- +ADVDATA?  
  get the advertising data  
- +ADVDATA = `<string>`  
  set the advertising data  
    - `<string>` : the advertising data  
      - format : XX:XX:XX:XX:XX:XX  
      - notice  
        XX : hex  
        It should follow adv_data format  
        The first XX means the length of value,  
        first XX : 00-1F  
      - ex. 07:02:01:05:03:FF:12:34  
        It has 7 length of value  
        It has two AD structures  
          the first AD structure has 2 length  
            type_flages(01) which value is 05  
          the second AD structure has 3 length  
            manufacturer_specific(FF) which value is 3412  

## +ADVFP

- +ADVFP?  
  get the advertising filter policy  
- +ADVFP = `<num>`  
  set the advertising filter policy  
    - `<num>` : the advertising filter policy  
    - notice:  
      not support now  

## +ADVINT

- +ADVINT?  
  get the advertising interval  
- +ADVINT = `<num1>`, `<num2>`  
  set the advertising interval  
    - `<num1>` : the minimum advertising interval  
      range : 32-16384  
      interval = `<num>` * 0.625ms  
    - `<num2>` : the maximum advertising interval  
      range : 32-16384  
      interval = `<num>` * 0.625ms  
    - notice  
       - `<num1>` must smaller `<num2>`  

## +ADVTYPE

- +ADVTYPE?  
  get advertising type  
- +ADVTYPE = `<num>`  
  set advertising type  
    - `<num>` : the advertising type  
       0: Connectable and scannable undirected advertising  
       1: Connectable directed advertising  
       2: Scanable undirected advertising  
       3: Non-Connectable undirected advertising  
- +ADVTYPE = 1,`<num>`,`<addr>`  
  when set advertising type to 1(direct address mode), it has to provide an address as a target  
  - `<num>` : the address type of device  
    0: public address  
    1: ramdom address  
  - `<addr>` : the address of device  
    - format : XX:XX:XX:XX:XX:XX  
      - ex.01:02:03:04:05:FF, addr[0] = 0xFF, addr[5] = 0x01  

## +DISADV

- +DISADV  
  disable advertising  

## +ENADV

- +ENADV = `<num>`  
  enable advertising of specific host ID  
    - `<num>` : host ID  
      range : 0-0  

## +SCANRSP

- +SCANRSP?  
  get the scan response data  
- +SCANRSP = `<string>`  
  set the scan response data  
    - `<string>` : the scan response data  
      - format : XX:XX:XX:XX:XX:XX  
      - notice  
        XX : hex  
        It should follow adv_data format  
        The first XX means the length of value, first XX : 00-1F  
      - ex. 09:08:09:61:62:63:64:65:66:67  
        It has 9 length of value  
        It has one AD structures  
          the first AD structure has 8 length  
            local_name_complete(09) which value is 61:62:63:64:65:66:67 = \"abcdefg\"  

# Connect

## +CONINT

- +CONINT?  
  get the connection interval of host ID 0  
- +CONINT = `<num1>`, `<num2>`, `<num3>`  
  set the connection interval of specific host ID  
    - `<num1>` : host ID  
      range : 0-0  
    - `<num2>` : the minimum connection interval  
      range : 6-3200  
      interval = `<num2>` * 1.25ms  
    - `<num3>` : the maximum connection interval  
      range : 6-3200  
      interval = `<num3>` * 1.25ms  
    - notice  
       - `<num2>` must smaller `<num3>`  
       must match this formula : timeout * 4 > interval_max * (1+latency)  

## +CONLAT

- +CONLAT?  
  get the connection latency  
- +CONLAT = `<num1>`, `<num2>`  
  set the connection latency  
    - `<num1>` : host ID  
      range : 0-0  
    - `<num2>` : the connection latency  
      range : 0-499  
      interval = `<num2>` * 1.25ms  
    - notice  
       must match this formula : timeout * 4 > interval_max * (1+latency)  

## +CONPARAM

- +CONPARAM = `<num1>`, `<num2>`, `<num3>`, `<num4>`, `<num5>`  
  set the connection interval of specific host ID  
    - `<num1>` : host ID  
      range : 0-0  
    - `<num2>` : the minimum connection interval  
      range : 6-3200  
      interval = `<num2>` * 1.25ms  
    - `<num3>` : the maximum connection interval  
      range : 6-3200  
      interval = `<num3>` * 1.25ms  
    - `<num4>` : the connection latency  
      range : 0-499  
      interval = `<num4>` * 1.25ms  
    - `<num5>` : the connection supervision timeout  
      range : 10-3200  
      supervision timeout = `<num5>` * 10ms  
    - notice  
       - `<num2>` must smaller `<num3>`  
       must match this formula : timeout * 4 > interval_max * (1+latency)  

## +CONSUPTMO

- +CONSUPTMO?  
  get the connection supervision timeout of host ID 0  
- +CONSUPTMO = `<num1>`, `<num2>`  
  set the connection supervision timeout of specific host ID  
    - `<num1>` : host ID  
      range : 0-0  
    - `<num2>` : the connection supervision timeout  
      range : 10-3200  
      supervision timeout = `<num2>` * 10ms  
    - notice  
       must match this formula : timeout * 4 > interval_max * (1+latency)  

## +DISCON

- +DISCON = `<num>`  
  disable connection of specific host ID  
    - `<num>` : host ID  
      range : 0-0  

## +READCONINT

- +READCONINT = `<num>`  
  read the connection interval of specific host ID  
    - `<num>` : host ID  
      range : 0-0  

## +READCONLAT

- +READCONLAT = `<num>`  
  read the connection latency of specific host ID  
    - `<num>` : host ID  
      range : 0-0  

## +READCONSUPTMO

- +READCONSUPTMO = `<num>`  
  read the connection supervision timeout of specific host ID  
    - `<num>` : host ID  
      range : 0-0  

# Connect_Other

## +SBOND

- +SBOND  
  set BLE Bonding Flag  
- +SBOND = `<num>`  
  - `<num>`: flag  

## +PLDATALEN

- +PKGDATALEN = `<num1>`,`<num2>`  
  set the tx max payload octets of specific host ID  
    - `<num1>` : host ID  
      range : 0-0  
    - `<num2>` : the tx max payload octets  
      range : 27-251  

## +IBOND

- +IBOND  
  initial BLE Bonding information  

## +PHY

- +PHY?  
  read PHY rate of host ID 0  
- +PHY =  `<num1>`,`<num2>`,`<num3>`,`<num3>`  
  set PHY rate of specific host ID  
    - `<num1>` : host ID  
      range : 0-0  
    - `<num2>` : TX PHY  
      1:BLE_PHY_1M  
      2:BLE_PHY_2M  
      4:BLE_PHY_CODED  
    - `<num3>` : RX PHY  
      1:BLE_PHY_1M  
      2:BLE_PHY_2M  
      4:BLE_PHY_CODED  
    - `<num4>` : phy option when TX/RX choose BLE_PHY_CODED(4)  
    - notice  
      TX PHY must equal to RX PHY  

## +PREPLDATALEN

- +PREPLDATALEN?  
  get the preferred tx payload octets  
- +PREPLDATALEN = `<num1>`  
  set the preferred tx max payload octets  
    - `<num1>` : the tx max payload octets  
      range : 27-251  

## +PFMTUS

- +PFMTUS = `<num1>`, `<num2>`  
set preferred MTU size of specific host ID  
  - `<num1>`: host ID  
    range : 0-0  
  - `<num2>`: preferred rx mtu size  
    range : 23-247  

## +RMTUS

- +RMTUS = `<num>`  
  read MTU size of specific host ID  
    - `<num>` : host ID  
      range : 0-0  

## +READPHY

- +READPHY =  `<num>`  
  read PHY rate of specific host ID  
    - `<num>` : host ID  
      range : 0-0  

## +RSCCCD

- +RSCCCD = `<num>`  
  restore last bond CCCD  
    - `<num>` : host ID  
      range : 0-0  
    - notice: this cmd is only valid in following situation  
      A connection bond with bond_flag=1(bonding)  
      Doing some CCCD operate  
      Discon and reconnect the connection  
      Do the +RSCCCD command which can retrieve CCCD state before disconnection  

## +READRSSI

- +READRSSI = `<num>`  
  read RSSI value of specific host ID  

## +SEC

- +SEC  
request security to specific host ID  
- +SEC = `<num>`  
  - `<num>`: host ID  

# Other

## +ALLPARAM

- +ALLPARAM?  
  read the all param  

## +RFRST

- +RFRST  
  reset RF IC  

## +ROLE

- +ROLE?  
  read the role(master/slave)  

## +STSTART

- +STSTART  
  stress test start  

# Scanning

## +DISSCAN

- +DISSCAN  
  disable scan  

## +ENSCAN

- +ENSCAN = `<addr>`  
  
  any scan response, then print  
  - `<addr>` : the address of device which be scanned  
    - format : XX:XX:XX:XX:XX:XX  

## +PARSADV

- +PARSADV = `<num>`  
get adv data by BLE_GAP_AD_TYPE  
  - `<num>` =  BLE_GAP_AD_TYPE  

## +PARSSCAN

- +PARSSCAN?  
get scan response data by BLE_GAP_AD_TYPE  
- +PARSSCAN = `<num>`  
  - `<num>` = BLE_GAP_AD_TYPE  

## +SCANFP

- +SCANFP?  
  get the scan filter policy  
- +SCANFP = `<num>`  
  set the scan filter policy  
  - `<num>` : the filter policy  
    0 : accept all  
    1 : accept white list  

## +SCANINT

- +SCANINT?  
  get the scan interval  
- +SCANINT = `<num>`  
  set the scan interval  
  - `<num>` : the scan interval  
    range : 4-16384  
    interval = `<num>` * 0.625ms  

## +SCANTYPE

- +SCANTYPE?  
  get scan type  
- +SCANTYPE = `<num>`  
  set scan type  
  - `<num>` : the scan type  
    0: passive  
    1: active  

## +SCANWIN

- +SCANWIN?  
  get the scan window  
- +SCANWIN = `<num>`  
  set scan window  
  - `<num>` : the scan window  
    range : 4-16384  
    window = `<num>` * 0.625ms  

# Master

## +CHECKERRORRSP

- +CHECKERRORRSP = `<num>`  
  check error response for specific host ID  
    - `<num>` : host ID  
      range : 0-0  

## +EXMTUS

- +EXMTUS = `<num1>`, `<num2>`  
  - exchange MTU size of specific host ID with server  
    - `<num1>` : host ID  
      range : 0-0  
    - `<num2>` : Client rx mtu size  
      range : 23-247  

## +READ

- +READ = `<num>`  
  read the specified characteristic value for specific host ID  
    - `<num>` : host ID  
      range : 0-0  

## +READCCCD

- +READCCCD = `<num>`  
  read CCCD value for specific host ID  
    - `<num>` : host ID  
      range : 0-0  
    - notice :  
      0:disable notify & disable indicate  
      1:enable notify & disable indicate  
      2:disable notify & enable indicate  
      3:enable notify & enable indicate  

## +READDEVAP

- +READDEVAP = `<num>`  
  read GAP appearance for specific host ID  
    - `<num>` : host ID  
      range : 0-0  

## +READDEVNAME

- +READDEVNAME = `<num>`  
  read GAP device name for specific host ID  
    - `<num>` : host ID  
      range : 0-0  

## +READPCONPAR

- +READDEVAP = `<num>`  
  read GAP appearance for specific host ID  
    - `<num>` : host ID  
      range : 0-0  

## +WRITE

- +WRITE = `<num1>`, `<num2>`  
  send write value for specific host ID  
    - `<num1>` : host ID  
      range : 0-0  
    - `<num2>` : the num of write length  
      range : 0-244  
        - notice: the write value will be set by the num of write length.  
          The write value = 0023456789012345678902234567890323456789042345678905234567890623456789072345678908234567890923456789102345678911234567891223456789132345678914234567891523456789162345678917234567891823456789192345678920234567892123456789222345678923234567892423  
          - ex. `<num2>` = 8  
            the write value = 00234567  
    - notice: the num of write value length should lower than (mtu size - 3)  

## +WRITEAUTHE

- +WRITEAUTHE = `<num1>`, `<num2>`  
  send write value for specific host ID with authentication  
    - `<num1>` : host ID  
      range : 0-0  
    - `<num2>` : the num of write length  
      range : 0-244  
        - notice: the write value will be set by the num of write length.  
          The write value = 0023456789012345678902234567890323456789042345678905234567890623456789072345678908234567890923456789102345678911234567891223456789132345678914234567891523456789162345678917234567891823456789192345678920234567892123456789222345678923234567892423  
          - ex. `<num2>` = 8  
            the write value = 00234567  
    - notice: the num of write value length should lower than (mtu size - 3)  

## +WRITEAUTHO

- +WRITEAUTHO = `<num1>`, `<num2>`  
  send write value for specific host ID with authorisation  
    - `<num1>` : host ID  
      range : 0-0  
    - `<num2>` : the num of write length  
      range : 0-244  
        - notice: the write value will be set by the num of write length.  
          The write value = 0023456789012345678902234567890323456789042345678905234567890623456789072345678908234567890923456789102345678911234567891223456789132345678914234567891523456789162345678917234567891823456789192345678920234567892123456789222345678923234567892423  
          - ex. `<num2>` = 8  
            the write value = 00234567  
    - notice: the num of write value length should lower than (mtu size - 3)  

## +WRITEENC

- +WRITEENC = `<num1>`, `<num2>`  
  send write value for specific host ID with encryption  
    - `<num1>` : host ID  
      range : 0-0  
    - `<num2>` : the num of write length  
      range : 0-244  
        - notice: the write value will be set by the num of write length.  
          The write value = 0023456789012345678902234567890323456789042345678905234567890623456789072345678908234567890923456789102345678911234567891223456789132345678914234567891523456789162345678917234567891823456789192345678920234567892123456789222345678923234567892423  
          - ex. `<num2>` = 8  
            the write value = 00234567  
    - notice: the num of write value length should lower than (mtu size - 3)  

## +WRITENRSP

- +WRITENRSP = `<num1>`, `<num2>`  
  send write value for specific host ID, which will not get response  
    - `<num1>` : host ID  
      range : 0-0  
    - `<num2>` : the num of write length  
      range : 0-244  
        - notice: the write value will be set by the num of write length.  
          The write value = 0023456789012345678902234567890323456789042345678905234567890623456789072345678908234567890923456789102345678911234567891223456789132345678914234567891523456789162345678917234567891823456789192345678920234567892123456789222345678923234567892423  
          - ex. `<num2>` = 8  
            the write value = 00234567  
    - notice: the num of write value length should lower than (mtu size - 3)  

# create_connect

## +CCCON

- +CCCON  
  cancel create connection  

## +CRCON

- +CRCON  
  create connection with default value  
- +CRCON = `<num1>`,`<num2>`,`<addr>`  
  create connection of specific host ID  
    - `<num1>` : host ID  
      range : 0-0  
    - `<num2>` : address type  
    - `<addr>` : the address of device  
      - format : XX:XX:XX:XX:XX:XX  
  - ex.AT+CRCON=0,1,11:12:13:BB:BB:C4, host id 0, random address 11:12:13:BB:BB:C4  

## +CCONINT

- +CCONINT?  
  get the create connection interval  
- +CCONINT = `<num1>`, `<num2>`  
  set the create connection interval  
  - `<num1>` : the minimum create connection interval  
    range : 6-3200  
    interval = `<num>` * 1.25ms  
  - `<num2>` : the maximum create connection interval  
    range : 6-3200  
    interval = `<num>` * 1.25ms  
  - notice  
    - `<num1>` must smaller `<num2>`  
    must match this formula : timeout * 4 > interval_max * (1+latency)  

## +CCONLAT

- +CCONLAT?  
  get the create connection latency  
- +CCONLAT = `<num>`  
  set the create connection latency  
  - `<num>` : the create connection latency  
    range : 0-499  
    interval = `<num>` * 1.25ms  
  - notice  
    must match this formula : timeout * 4 > interval_max * (1+latency)  

## +CCONSUPTMO

- +CCONSUPTMO?  
  get the create connection supervision timeout  
- +CCONSUPTMO = `<num>`  
  set the create connection supervision timeout  
  - `<num>` : the create connection supervision timeout  
    range : 10-3200  
    supervision timeout = `<num>` * 10ms  
  - notice  
    must match this formula : timeout * 4 > interval_max * (1+latency)  

# Slave

## +DEVAP

- +DEVAP = `<num>`  
  set GAP appearance  
    - `<num>` : appearance  
  

## +DEVNAME

- +DEVNAME = `<string>`  
  set GAP device name  
    - `<string>` : the GAP device name data  
  

## +PCONPAR

- +PCONPAR = `<num1>`, `<num2>`, `<num3>`, `<num4>`  
  set GAP perfer connection parameter  
    - `<num1>` : connIntervalMin  
  
    - `<num2>` : connIntervalMax  
  
    - `<num3>` : connLatency  
  
    - `<num4>` : connSupervisionTimeout  
  

## +IND

- +IND = `<num1>`, `<num2>`  
  send indication for specific host ID  
    - `<num1>` : host ID  
      range : 0-0  
    - `<num2>` : the num of indication length  
      range : 0-244  
        - notice: the indication value will be set by the num of indication length.  
          The indication value = 0023456789012345678902234567890323456789042345678905234567890623456789072345678908234567890923456789102345678911234567891223456789132345678914234567891523456789162345678917234567891823456789192345678920234567892123456789222345678923234567892423  
          - ex. `<num2>` = 8  
            the indication value = 00234567  
    - notice: the num of indication value length should lower than (mtu size - 3)  

## +NFY

- +NFY = `<num1>`, `<num2>`  
  send notification for specific host ID  
    - `<num1>` : host ID  
      range : 0-0  
    - `<num2>` : the num of notification length  
      range : 0-244  
        - notice: the notification value will be set by the num of notification length.  
          The notification value = 0023456789012345678902234567890323456789042345678905234567890623456789072345678908234567890923456789102345678911234567891223456789132345678914234567891523456789162345678917234567891823456789192345678920234567892123456789222345678923234567892423  
          - ex. `<num2>` = 8  
            the notification value = 00234567  
    - notice: the num of notification value length should lower than (mtu size - 3)  

## +SETERRORCODE

- +SETERRORCODE = `<num1>`, `<num2>`  
  set error code for specific host ID  
    - `<num1>` : host ID  
      range : 0-0  
    - `<num2>` : the error code  
      range : 0-255  

## +SETREADVAL

- +SETREADVAL = `<num1>`, `<num2>`  
  set read value for specific host ID  
    - `<num1>` : host ID  
      range : 0-0  
    - `<num2>` : the num of read value length  
      range : 0-244  
        - notice: the read value will be set by the num of read value length.  
          The read value = 0023456789012345678902234567890323456789042345678905234567890623456789072345678908234567890923456789102345678911234567891223456789132345678914234567891523456789162345678917234567891823456789192345678920234567892123456789222345678923234567892423  
          - ex. `<num2>` = 8  
            the read value = 00234567  
    - notice: the num of read value length should lower than (mtu size - 3)  

