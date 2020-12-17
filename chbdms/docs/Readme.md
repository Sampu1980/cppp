Images built from this repo: 
- sv-artifactory.infinera.com/chm6/arm64/chm6_board_ms:V1.4.41
- sv-artifactory.infinera.com/chm6/x86_64/chm6_board_ms:V1.4.41

----------------------------------------------------------------------------

Compatibility matrix for chm6_board_ms:V1.4.41

-   ARG chm6devenv=V2.20
-   ARG chm6testenv=V1.7.0
-   ARG chm6runenv=V2.8
-   ARG chm6proto=V4.2.0
-   ARG cli3pp=V0.1
-   ARG chm6common=V1.1.19
-   ARG chm6_ds_apps_adapter=V1.0.2
-   ARG epdm=v2.0.5
-   ARG milb=V4.0
-   ARG chm6_logging=V1.0.3

----------------------------------------------------------------------------

CLI commands with sim adapter in chm6_board_ms:V1.4.41

board_ms_cli> help

Commands available:
 - help
        This help message
 - exit
        Quit the session
 - manager_cli
        (menu)
 - adapter_cli
        (menu)


manager_cli> help

Commands available:
 - help
        This help message
 - exit
        Quit the session
 - log
        Dump manager log
 - status <string>
        Dump manager status
 - resetlog
        Reset manager log
 - config
        Dump board configurations
 - state
        Dump board states
 - fault
        Dump faults in manager cache
 - pm
        Dump Pms in manager cache

adapter_cli> help

Commands available:
 - help
    This help message
 - exit
    Quit the session
 - log
    Dump adapter log
 - status <string>
    Dump adapter status
 - resetlog
    Reset adapter log
 - eqpt_inv
    Dump equipment inventory
 - devices
    Dump upgradable devices
 - dump_fault <unsigned int>
    Dump faults in adapter cache
        faultId: uint32 ID of fault to dump. Use 255 or any value >= max to dump all faults
 - dump_fault <string>
    Dump faults in adapter cache
        faultName: name of fault or ALL_FAULTS to dump all faults
 - set_fault_sim <unsigned int> <bool> <bool>
    Set Fault Sim in adapter faults. When enabled the sim fault condition will override the real fault state
        faultId   - faultId from the list given in dump_fault; set to 255 or anything >= max to update all faults 
        isSimEn   - true: enable sim; false: disable sim 
        isTripped - true: sim fault state set to FAULT_SET; false: sim fault state set to FAULT_CLEAR
 - set_fault_sim <string> <bool> <bool>
    Set Fault Sim in adapter faults. When enabled the sim fault condition will override the real fault state
        faultId   - faultName: name of fault or ALL_FAULTS to update all faults 
        isSimEn   - true: enable sim; false: disable sim 
        isTripped - true: sim fault state set to FAULT_SET; false: sim fault state set to FAULT_CLEAR
 - print_max_fault_num
    Print max fault number
 - pm
    Dump Pms in adapter cache
 - led
    Dump LED states in adapter cache
 - set_eqpt_state <Equipment state:
        0: UNKNOWN
        1: BOOTING
        2: BOOTING_DONE
        3: BOOTING_DONE_WARM
        4: BOOTING_FAIL
        5: FW_UPDATING
        6: FW_UPDATE_DONE
        7: FW_UPDATE_FAIL
        8: FW_UPDATE_COMMIT
        9: HW_INITING
        10:HW_INITING_DONE
        11:HW_INITING_FAIL
        12:RESTARTING>
    Set Equipment state
 - set_inv <attribute:(
        sn:SerialNumber,
        pn:PartNumber,
        vn:VendorName,
        hv:HardwareRevision,
        md:ManufactureDate,
        fv:FirmwareVersion,
        cc:CleiCode,
        pon:Pon,
        id:InsertionDate)> <value>
    Set eqpt inventory
 - set_device <device ID: See dump of "devices"> <hwVersion> <swVersion>
    Set upgradable device
 - set_pm <value>
    Set board pm "temperature" value
 - set_fault_led <LED state:
        1: OFF
        6: RED
        7: FLASHING_RED>
    Set fault LED state
 - set_active_led <LED state:
        1: OFF
        2: YELLOW
        3: FLASHING_YELLOW
        4: GREEN
        5: FLASHING_GREEN>
    Set active LED state
 - set_port_led <portId> <ledType> <ledState>
    Set port LED state.
    Valid "portId"(0 based): 0 ~ 15
    Valid "ledType": 0 - Active LED; 1 - Los LED
    Valid Active ledState:
        1 - LED OFF
        2 - LED YELLOW
        3 - LED FLASHING_YELLOW
        4 - LED GREEN
        5 - LED FLASHING_GREEN
    Valid Los ledState: 
        1 - LED OFF
        6 - LED RED
        7 - LED FLASHING_RED

 - set_line_led <lineId> <ledType> <ledState>
    Set line LED state.
    Valid "lineId": 0 - LINE_1; 1 - LINE_2.
    Valid "ledType": 0 - Active LED; 1 - Los LED
    Valid Active ledState:
        1 - LED OFF
        2 - LED YELLOW
        3 - LED FLASHING_YELLOW
        4 - LED GREEN
        5 - LED FLASHING_GREEN
    Valid Los ledState: 
        1 - LED OFF
        6 - LED RED
        7 - LED FLASHING_RED

 - set_lamp_test <doTest(0: disable; >=1: enable)>
    Enable or disable LED lamp test

 - set_access_fault_sim <unsigned int> <bool>
    Set Access Fault Sim in adapter faults.
        faultId - faultId from the list given in dump_fault;
	          set to 255 or anything >= max to update all faults
        bSetFault - true: sets the fault; false: resets the fault.

----------------------------------------------------------------------------
CLI commands with HW driver in chm6_board_ms:V1.4.41

driver_cli> help

Commands available:
 - help
        This help message
 - exit
        Quit the session
 - log
        Dump driver log
 - status <command>
        Dump driver status
        Valid commands:
        "tmp": dump temperatures
        "tmp_all": dump all temperature related data
        "led": dump all LED states
 - resetlog
        Reset driver log
 - restart_warm
        Warm restart board
 - restart_cold
        Cold restart board
 - regif_cli
        (menu)
 - mfgeep_cli
        (menu)
 - led_cli
        (menu)
 - board_ms_cli
        (menu)
        
regif_cli> help

Commands available:
 - help
        This help message
 - exit
        Quit the session
 - read <0xOffset>
        Read FPGA register
 - write <0xoffset> <0xdata>
        Write FPGA register
 - PlI2c2IoxRead <0xoffset>
        Bottom Mezz card IO expender register read
 - PlI2c2IoxWrite <offset, data>
        Bottom Mezz card IO expender register write
 - PlI2c3IoxRead <0xoffset>
        Top Mezz card IO expender register read
 - PlI2c3IoxWrite <offset, data>
        Top Mezz card IO expender register write
 - I2c2PllRead <offset>
        Bottom Mezz card Si5394 register read
 - I2c2PllWrite <offset,data>
        Bottom Mezz card Si5394 register write
 - I2c2PllConfig
        Bottom Mezz card Si5394 configure
 - I2c3PllRead <offset>
        Top Mezz card Si5394 register read
 - I2c3PllWrite <offset,data>
        Top Mezz card Si5394 register write
 - I2c3PllConfig
        Top Mezz card Si5394 configure
 - mdioRd <bus sel> <0xMdio_addr> <0xoffset>
        Read Gearbox MDIO register
 - mdioWr <bus sel> <0xMdio_addr> <0xoffset> <0xdata>
        Write Gearbox MDIO register
 - i2cRd <bus sel> <0xphy_addr> <0xoffset>
        Read I2C register
 - i2cWr <bus sel> <0xphy_addr> <0xoffset> <0xdata>
        Write I2C register
 - driver_cli
        (menu)
        
mfgeep_cli> help

Commands available:
 - help
        This help message
 - exit
        Quit the session
 - import <filename>
        import EEPROM bin file to buffer
 - export <filename>
        export EEPROM bin buffer to file
 - read
        read from EEPROM into buffer
 - write
        write from buffer into EEPROM
 - dump_bin
        raw dump of buffer
 - dump_fields <level>
        dump parsed fields of buffer
 - buffer_name
        dump name of buffer: EEPROM or imported file
 - driver_cli
        (menu)
        
led_cli> help

Commands available:
 - help
        This help message
 - exit
        Quit the session
 - set_fru_active_led <ledType>
        Set Fru ACTIVE LED state.
        Valid Active ledState:
                1 - LED OFF
                2 - LED YELLOW
                3 - LED FLASHING_YELLOW
                4 - LED GREEN
                5 - LED FLASHING_GREEN

 - get_fru_active_led
        Get Fru ACTIVE LED state.

 - set_fru_fault_led <ledType>
        Set Fru FAULT LED state.
        Valid Los ledState:
                1 - LED OFF
                6 - LED RED
                7 - LED FLASHING_RED

 - get_fru_fault_led
        Get Fru FAULT LED state.

 - set_qsfp_led <portId> <ledType> <ledState>
        Set Mezzanine card's QSFP LED state.
        Valid "portId"(0 based): 0 ~ 15
        Valid "ledType": 0 - Active LED; 1 - Los LED
        Valid Active ledState:
                1 - LED OFF
                2 - LED YELLOW
                3 - LED FLASHING_YELLOW
                4 - LED GREEN
                5 - LED FLASHING_GREEN
        Valid Los ledState:
                1 - LED OFF
                6 - LED RED
                7 - LED FLASHING_RED

 - get_qsfp_led <portId> <ledType>
        Get Mezzanine card's QSFP LED state.
        Valid "portId"(0 based): 0 ~ 15
        Valid "ledType": 0 - Active LED; 1 - Los LED

 - set_line_led <lineId> <ledType> <ledState>
        Set Mezzanine card's LINE LED state.
        Valid "lineId": 0 - LINE_1; 1 - LINE_2.
        Valid "ledType": 0 - Active LED; 1 - Los LED
        Valid Active ledState:
                1 - LED OFF
                2 - LED YELLOW
                3 - LED FLASHING_YELLOW
                4 - LED GREEN
                5 - LED FLASHING_GREEN
        Valid Los ledState:
                1 - LED OFF
                6 - LED RED
                7 - LED FLASHING_RED

 - get_line_led <lineId> <ledType>
        Get Mezzanine card's LINE LED state.
        Valid "lineId": 0 - LINE_1; 1 - LINE_2.
        Valid "ledType": 0 - Active LED; 1 - Los LED

 - location_test <doTest>
        Start or stop LED location test.
        Valid "doTest": 0 - stop test; 1 or bigger - start test.

 - dump_led_regs
        Dump LED registers
 - dump_led
        Dump LED states
 - driver_cli
        (menu)

adm1066_cli> help

Commands available:
 - help
        This help message
 - exit
        Quit the session
 - dump_manid
        dump host board ADM1066 register MANID. Expected value: 0x41
 - dump_revid
        dump host board ADM1066 register REVID. Expected value: 0x02
 - dump_mark1
        dump host board ADM1066 register MARK1. Expected value: 0x00
 - dump_mark2
        dump host board ADM1066 register MARK2. Expected value: 0x00
 - read_byte
        read byte of host board ADM1066
 - read_byte_data <command code(hex)>
        read byte data of host board ADM1066
 - read_word_data <command code(hex)>
        read word data of host board ADM1066
 - read_block_data <command code(hex)>
        read block data of host board ADM1066
 - write_byte <value(hex)>
        write byte to host board ADM1066
 - write_byte_data <command code(hex)> <value(hex)>
        write byte data to host board ADM1066
 - write_word_data <command code(hex)> <value(hex)>
        write word data to host board ADM1066
 - dump_host_adm1066_eeprom
        dump host board ADM1066 EEPROM 64 bytes
 - dump_host_adm1066_version
        dump host board ADM1066 version
 - driver_cli
        (menu)
        
-----------------------------------------------------------
docker command to bypass "InitDone"

docker run --rm --network chm6_local_brdg --env BoardInfo=sim --env ChassisId=1 --env SlotNo=4  InitDone=bypass --hostname chm6_board_ms --name chm6_board_ms --entrypoint "/home/bin/BoardMs" -d sv-artifactory.infinera.com/chm6/x86_64/chm6_board_ms:V1.4.3
