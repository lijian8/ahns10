## TCP/IP Connection ##
All devices need to connect to the ground station 802.11 modem (Linksys WRT54GL router).
Router is preferable to ad-hoc:
  * due to full 802.11g speeds
  * ability to connect and disconnect without wifi card reconfiguration
  * inbuilt dhcp and dns servers
  * provides access to Internet through connection of wired network link

## Router Information ##
  * SSID: heliconnect10
  * Channel: 11
  * Encryption: None
  * Gateway: 192.168.1.1
  * Subnet: 255.255.255.0
  * Username: ahns
  * Password: ahns

## AHNS device IP assignment ##
| **Device** | **IP** |
|:-----------|:-------|
|Linksys WRT54GL (Server)|192.168.1.1|
|Overo Air|192.168.1.122|

## Blackfin Camera ##
The AHNS Blackfin camera will only connect to a network with a SSID of 'heliconnect10'. Full instructions for reconfiguration can be found [here.](http://www.surveyor.com/blackfin/SRV_setup_bf.html)