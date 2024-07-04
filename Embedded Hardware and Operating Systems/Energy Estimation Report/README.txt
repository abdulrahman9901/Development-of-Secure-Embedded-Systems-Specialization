DISCLAIMER:
1. It seems Contiki development moved forward since this task was created:
Some RDC drivers mentioned in the assignment are no longer available. According to Contiki WIKI there are 3
RDC drivers available: contikimac_driver, cxmac_driver, nullrdc_driver.
Please see https://github.com/contiki-os/contiki/wiki/Change-mac-or-radio-duty-cycling-protocols#user-content-About_MAC_Drivers

Therefore, I adjusted task scope accordingly

Task 2:
Obviously null_rdc consumes more energy then xmac. It does not have any optimizations to reduce energy consumption.