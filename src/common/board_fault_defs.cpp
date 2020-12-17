/*
 * Copyright (c) 2020 Infinera
 *
 * board_fault_defs.cpp
 *
 *  Created on: Aug 12, 2020
 */
#include <string>
#include <boost/format.hpp>

#include "board_fault_defs.h"
#include "board_defs.h"
#include "types.h"
#include "InfnLogger.h"

// TEMPORARY
// Uncomment to omit faults 110 and 177.
#define ERR_110_177
// Compile time message.
#ifdef ERR_110_177
#pragma message "FAULTS 110 and 177 OMITTED"
#endif

#define DBG 0
#if DBG
#define DLOG INFN_LOG(SeverityLevel::info)
#else
#define DLOG INFN_LOG(SeverityLevel::debug)
#endif

namespace boardMs
{

struct Chm6BoardFaultDefs
{
    const std::string mFaultName;
    const std::string mFaultType;
    const std::string mFaultDescr;
};

const Chm6BoardFaultDefs cBoardFaultDefs[MAX_BOARD_FAULT_ID_NUM] =
{
        //  *******    Fault ID    *******          |  *******    Fault Name    *******     |  ** Fault Type ** |  ******* Fault Description *******
        {   /* SAC_BUS_FAIL                        */  "SAC-BUS-FAIL"                       ,  "EQPTFAIL"       ,  "SAC_BUS_FAULT"                               },
        {   /* DCO_ZYNQ_PS_LINK_DOWN               */  "DCO-ZYNQ-PS-LINK-DOWN"              ,  "EQPTCOMFAIL"    ,  "DCO_ENET_PS_DOWN"                            },
        {   /* DCO_ZYNQ_PL_LINK_DOWN               */  "DCO-ZYNQ-PL-LINK-DOWN"              ,  "EQPTCOMFAIL"    ,  "DCO_ENET_PL_DOWN"                            },
        {   /* DCO_NXP_PL_LINK_DOWN                */  "DCO-NXP-PL-LINK-DOWN"               ,  "EQPTCOMFAIL"    ,  "DCO_ENET_SECURITY_PROC_DOWN"                 },
        {   /* DCO_ZYNQ_PS_LINK_CRC_ERRORS         */  "DCO-ZYNQ-PS-LINK-CRC-ERRORS"        ,  "EQPTCOMFAIL"    ,  "DCO_ENET_PS_CRC_ERRORS"                      },
        {   /* DCO_ZYNQ_PL_LINK_CRC_ERRORS         */  "DCO-ZYNQ-PL-LINK-CRC-ERRORS"        ,  "EQPTCOMFAIL"    ,  "DCO_ENET_PL_CRC_ERRORS"                      },
        {   /* DCO_NXP_PL_LINK_CRC_ERRORS          */  "DCO-NXP-PL-LINK-CRC-ERRORS"         ,  "EQPTCOMFAIL"    ,  "DCO_ENET_SECURITY_PROC_CRC_ERRORS"           },
        {   /* HOST_OCXO_FREQ_OOR                  */  "HOST-OCXO-FREQ-OOR"                 ,  "EQPTFAIL"       ,  "HOST_CLOCK_FAILURE"                          },
        {   /* HOST_HS_FAIL                        */  "HOST-HS-FAIL"                       ,  "EQPTFAIL"       ,  "HOT_SWAP_FAULT_NON_SPECIFIC"                 },
        {   /* HOST_12V_UV                         */  "HOST-12V-UV"                        ,  "EQPTFAIL"       ,  "INPUT_VOLTAGE_UV"                            },
        {   /* HOST_12V_OV                         */  "HOST-12V-OV"                        ,  "EQPTFAIL"       ,  "INPUT_VOLTAGE_OV"                            },
        {   /* HOST_5V_UV                          */  "HOST-5V-UV"                         ,  "EQPTFAIL"       ,  "POWER_5V_UV"                                 },
        {   /* HOST_5V_OV                          */  "HOST-5V-OV"                         ,  "EQPTFAIL"       ,  "POWER_5V_OV"                                 },
        {   /* HOST_3_3V_UV                        */  "HOST-3-3V-UV"                       ,  "EQPTFAIL"       ,  "POWER_3_3V_UV"                               },
        {   /* HOST_3_3V_OV                        */  "HOST-3-3V-OV"                       ,  "EQPTFAIL"       ,  "POWER_3_3V_OV"                               },
        {   /* HOST_1_8V_UV                        */  "HOST-1-8V-UV"                       ,  "EQPTFAIL"       ,  "POWER_1_8V_UV"                               },
        {   /* HOST_1_8V_OV                        */  "HOST-1-8V-OV"                       ,  "EQPTFAIL"       ,  "POWER_1_8V_OV"                               },
        {   /* HOST_1_5V_UV                        */  "HOST-1-5V-UV"                       ,  "EQPTFAIL"       ,  "POWER_1_5V_UV"                               },
        {   /* HOST_1_5V_OV                        */  "HOST-1-5V-OV"                       ,  "EQPTFAIL"       ,  "POWER_1_5V_OV"                               },
        {   /* HOST_P10V_UV                        */  "HOST-P10V-UV"                       ,  "EQPTFAIL"       ,  "POWER_10V_UV"                                },
        {   /* HOST_P10V_OV                        */  "HOST-P10V-OV"                       ,  "EQPTFAIL"       ,  "POWER_10V_OV"                                },
        {   /* HOST_P1_0V_UV                       */  "HOST-P1-0V-UV"                      ,  "EQPTFAIL"       ,  "POWER_P10V_UV"                               },
        {   /* HOST_P1_0V_OV                       */  "HOST-P1-0V-OV"                      ,  "EQPTFAIL"       ,  "POWER_P10V_OV"                               },
        {   /* HOST_P0_85V_UV                      */  "HOST-P0-85V-UV"                     ,  "EQPTFAIL"       ,  "POWER_8_5V_UV"                               },
        {   /* HOST_P0_85V_OV                      */  "HOST-P0-85V-OV"                     ,  "EQPTFAIL"       ,  "POWER_8_5V_OV"                               },
        {   /* HOST_ZYNQ_PSPLL_1_2V_UV             */  "HOST-ZYNQ-PSPLL-1-2V-UV"            ,  "EQPTFAIL"       ,  "POWER_1_2V_UV"                               },
        {   /* HOST_ZYNQ_PSPLL_1_2V_OV             */  "HOST-ZYNQ-PSPLL-1-2V-OV"            ,  "EQPTFAIL"       ,  "POWER_1_2V_OV"                               },
        {   /* HOST_ZYNQ_AVCC_UV                   */  "HOST-ZYNQ-AVCC-UV"                  ,  "EQPTFAIL"       ,  "POWER_AVCC_UV"                               },
        {   /* HOST_ZYNQ_AVCC_OV                   */  "HOST-ZYNQ-AVCC-OV"                  ,  "EQPTFAIL"       ,  "POWER_AVCC_OV"                               },
        {   /* HOST_ZYNQ_AVTT_UV                   */  "HOST-ZYNQ-AVTT-UV"                  ,  "EQPTFAIL"       ,  "POWER_AVTT_UV"                               },
        {   /* HOST_ZYNQ_AVTT_OV                   */  "HOST-ZYNQ-AVTT-OV"                  ,  "EQPTFAIL"       ,  "POWER_AVTT_OV"                               },
        {   /* EUSB_ACCESS_FAIL                    */  "EUSB-ACCESS-FAIL"                   ,  "EQPTFAIL"       ,  "eUSB_ACCESS_FAILURE"                         },
        {   /* I2C_GMCU_ACCESS_FAIL                */  "I2C-GMCU-ACCESS-FAIL"               ,  "EQPTCOMFAIL"    ,  "I2C_TO_DCO_FAIlURE"                          },
        {   /* DDR_MEMHOT_FAIL                     */  "DDR-MEMHOT-FAIL"                    ,  "EQPTFAIL"       ,  "SDRAM_FAILURE_HOT"                           },
        {   /* FPGA_LINK_DOWN                      */  "FPGA-LINK-DOWN"                     ,  "EQPTCOMFAIL"    ,  "FCP_FPGA_ENET_LINK_DOWN"                     },
        {   /* FPGA_PL_ACCESS_FAIL                 */  "FPGA-PL-ACCESS-FAIL"                ,  "EQPTFAIL"       ,  "FCP_FPGA_PL_ACCESS_DOWN"                     },
        {   /* INLET_TEMP_OOR                      */  "INLET-TEMP-OOR"                     ,  "Log only"       ,  "INLET_TEMP_OOR"                              },
        {   /* BOARD_TEMP_OORH                     */  "BOARD-TEMP-OORH"                    ,  "TEMP-OORH"      ,  "BOARD_TEMP_OORH"                              },
        {   /* HOST_PWR_SEQ_ACCESS_FAIL            */  "HOST-PWR-SEQ-ACCESS-FAIL"           ,  "EQPTFAIL"       ,  "HOST_PWR_SEQ_ACCESS_FAIL"                    },
        {   /* HOST_HS_ACCESS_FAIL                 */  "HOST-HS-ACCESS-FAIL"                ,  "EQPTFAIL"       ,  "HOST_HOT_SWAP_ACCESS_FAIL"                   },
        {   /* INLET_TEMP_ACCESS_FAIL              */  "INLET-TEMP-ACCESS-FAIL"             ,  "EQPTFAIL"       ,  "HOST_INLET_TEMP_ACCESS_FAIL"                 },
        {   /* BOARD_TEMP_ACCESS_FAIL              */  "BOARD-TEMP-ACCESS-FAIL"             ,  "EQPTFAIL"       ,  "HOST_BOARD_TEMP_ACCESS_FAIL"                 },
        {   /* MFG_EEPROM_ACCESS_FAIL              */  "MFG-EEPROM-ACCESS-FAIL"             ,  "EQPTFAIL"       ,  "HOST_MFG_EEPROM_ACCESS_FAIL"                 },
        {   /* CAL_EEPROM_ACCESS_FAIL              */  "CAL-EEPROM-ACCESS-FAIL"             ,  "EQPTFAIL"       ,  "HOST_CAL_EEPROM_ACCESS_FAIL"                 },

        {   /* TMZ_DSPLL_OOL                       */  "TMZ-DSPLL-OOL"                      ,  "EQPTFAIL"       ,  "TOP_MZ_PLL_OOL"                              },
        {   /* TMZ_OCXO_LOS                        */  "TMZ-OCXO-LOS"                       ,  "EQPTFAIL"       ,  "TOP_MZ_INPUT_CLOCK_LOS_OOF"                  },
        {   /* TMZ_I2C_CLKGEN_I2C_FAIL             */  "TMZ-I2C-CLKGEN-I2C-FAIL"            ,  "EQPTFAIL"       ,  "TOP_MZ_CLOCK_GEN_I2C_FAULT"                  },
        {   /* TMZ_TMP_OORH                        */  "TMZ-TMP-OORH"                       ,  "TEMP_OORH"      ,  "TOP_MZ_OVER_TEMP"                            },
        {   /* TMZ_TMP_OORL                        */  "TMZ-TMP-OORL"                       ,  "TEMP_OORL"      ,  "TOP_MZ_UNDER_TEMP"                           },
        {   /* TMZ_TMP_I2C_FAIL                    */  "TMZ-TMP-I2C-FAIL"                   ,  "EQPTFAIL"       ,  "TOP_MZ_TMP_ACCESS_FAIL"                      },
        {   /* TMZ_3_3V_TMP_OORH                   */  "TMZ-3-3V-TMP-OORH"                  ,  "TEMP_OORH"      ,  "TOP_MZ_3_3_OVER_TEMP"                        },
        {   /* TMZ_3_3V_TMP_OORL                   */  "TMZ-3-3V-TMP-OORL"                  ,  "EQPTFAIL"       ,  "TOP_MZ_3_3_OVER_CURRENT"                     },
        {   /* TMZ_3_3V_I2C_FAIL                   */  "TMZ-3-3V-I2C-FAIL"                  ,  "EQPTFAIL"       ,  "TOP_MZ_3_3_I2C_FAIL"                         },
        {   /* TMZ_3_3V_VIN_OORL                   */  "TMZ-3-3V-VIN-OORL"                  ,  "EQPTFAIL"       ,  "TOP_MZ_3_3_INPUT_OORL"                       },
        {   /* TMZ_3_3V_VOUT_OOR                   */  "TMZ-3-3V-VOUT-OOR"                  ,  "EQPTFAIL"       ,  "TOP_MZ_POWER_3_3V_FAILURE"                   },
        {   /* TMZ_1_8V_TMP_OORH                   */  "TMZ-1-8V-TMP-OORH"                  ,  "TEMP_OORH"      ,  "TOP_MZ_0_8V_OVER_TEMP"                       },
        {   /* TMZ_1_8V_TMP_OORL                   */  "TMZ-1-8V-TMP-OORL"                  ,  "EQPTFAIL"       ,  "TOP_MZ_0_8V_OVER_CURRENT"                    },
        {   /* TMZ_1_8V_I2C_FAIL                   */  "TMZ-1-8V-I2C-FAIL"                  ,  "EQPTFAIL"       ,  "TOP_MZ_0_8V_COMM_FAULT"                      },
        {   /* TMZ_0_8V_VIN_OORL                   */  "TMZ-0-8V-VIN-OORL"                  ,  "EQPTFAIL"       ,  "TOP_MZ_0_8_INPUT_OOR"                        },
        {   /* TMZ_0_8V_VOUT_OOR                   */  "TMZ-0-8V-VOUT-OOR"                  ,  "EQPTFAIL"       ,  "TOP_MZ_POWER_0_8V_FAILURE"                   },
        {   /* TMZ_TXDRV_VOUT_OOR                  */  "TMZ-TXDRV-VOUT-OOR"                 ,  "EQPTFAIL"       ,  "TOP_MZ_TX_DRV_VOLTAGE_OOR"                   },
        {   /* TMZ_H_TXDRV_VOUT_OOR                */  "TMZ-H-TXDRV-VOUT-OOR"               ,  "EQPTFAIL"       ,  "TOP_MZ_H_TX_DRV_VOLTAGE_OOR"                 },
        {   /* TMZ_AVS1_TMP_OORH                   */  "TMZ-AVS1-TMP-OORH"                  ,  "TEMP_OORH"      ,  "TOP_MZ_AVS1_OVER_TEMP"                       },
        {   /* TMZ_AVS2_TMP_OORH                   */  "TMZ-AVS2-TMP-OORH"                  ,  "TEMP_OORH"      ,  "TOP_MZ_AVS2_OVER_TEMP"                       },
        {   /* TMZ_AVS3_TMP_OORH                   */  "TMZ-AVS3-TMP-OORH"                  ,  "TEMP_OORH"      ,  "TOP_MZ_AVS3_OVER_TEMP"                       },
        {   /* TMZ_AVS1_OC                         */  "TMZ-AVS1-OC"                        ,  "EQPTFAIL"       ,  "TOP_MZ_AVS1_OVER_CURRENT"                    },
        {   /* TMZ_AVS2_OC                         */  "TMZ-AVS2-OC"                        ,  "EQPTFAIL"       ,  "TOP_MZ_AVS2_OVER_CURRENT"                    },
        {   /* TMZ_AVS3_OC                         */  "TMZ-AVS3-OC"                        ,  "EQPTFAIL"       ,  "TOP_MZ_AVS3_OVER_CURRENT"                    },
        {   /* TMZ_AVS1_ACCESS_FAIL                */  "TMZ-AVS1-ACCESS-FAIL"               ,  "EQPTFAIL"       ,  "TOP_MZ_AVS1_COMM_FAULT"                      },
        {   /* TMZ_AVS2_ACCESS_FAIL                */  "TMZ-AVS2-ACCESS-FAIL"               ,  "EQPTFAIL"       ,  "TOP_MZ_AVS2_COMM_FAULT"                      },
        {   /* TMZ_AVS3_ACCESS_FAIL                */  "TMZ-AVS3-ACCESS-FAIL"               ,  "EQPTFAIL"       ,  "TOP_MZ_AVS3_COMM_FAULT"                      },
        {   /* TMZ_AVS1_VIN_OORL                   */  "TMZ-AVS1-VIN-OORL"                  ,  "EQPTFAIL"       ,  "TOP_MZ_AVS1_INPUT_OOR"                       },
        {   /* TMZ_AVS2_VIN_OORL                   */  "TMZ-AVS2-VIN-OORL"                  ,  "EQPTFAIL"       ,  "TOP_MZ_AVS2_INPUT_OOR"                       },
        {   /* TMZ_AVS3_VIN_OORL                   */  "TMZ-AVS3-VIN-OORL"                  ,  "EQPTFAIL"       ,  "TOP_MZ_AVS3_INPUT_OOR"                       },
        {   /* TMZ_AVS1_VOUT_OOR                   */  "TMZ-AVS1-VOUT-OOR"                  ,  "EQPTFAIL"       ,  "TOP_MZ_POWER_AVS1_FAILURE"                   },
        {   /* TMZ_AVS2_VOUT_OOR                   */  "TMZ-AVS2-VOUT-OOR"                  ,  "EQPTFAIL"       ,  "TOP_MZ_POWER_AVS2_FAILURE"                   },
        {   /* TMZ_AVS3_VOUT_OOR                   */  "TMZ-AVS3-VOUT-OOR"                  ,  "EQPTFAIL"       ,  "TOP_MZ_POWER_AVS3_FAILURE"                   },
        {   /* TMZ_RETIMER1_FW_FAIL                */  "TMZ-RETIMER1-FW-FAIL"               ,  "EQPTFAIL"       ,  "TOP_MZ_RETIMER1_FW_FAILURE"                  },
        {   /* TMZ_RETIMER2_FW_FAIL                */  "TMZ-RETIMER2-FW-FAIL"               ,  "EQPTFAIL"       ,  "TOP_MZ_RETIMER2_FW_FAILURE"                  },
        {   /* TMZ_RETIMER3_FW_FAIL                */  "TMZ-RETIMER3-FW-FAIL"               ,  "EQPTFAIL"       ,  "TOP_MZ_RETIMER3_FW_FAILURE"                  },
        {   /* TMZ_RETIMER1_AVS_FAIL               */  "TMZ-RETIMER1-AVS-FAIL"              ,  "EQPTFAIL"       ,  "TOP_MZ_POWER_RETIMER1_AVS_FAILURE"           },
        {   /* TMZ_RETIMER2_AVS_FAIL               */  "TMZ-RETIMER2-AVS-FAIL"              ,  "EQPTFAIL"       ,  "TOP_MZ_POWER_RETIMER2_AVS_FAILURE"           },
        {   /* TMZ_RETIMER3_AVS_FAIL               */  "TMZ-RETIMER3-AVS-FAIL"              ,  "EQPTFAIL"       ,  "TOP_MZ_POWER_RETIMER3_AVS_FAILURE"           },
        {   /* TMZ_RETIMER1_INGRESS_DCO_LOS_LOL    */  "TMZ-RETIMER1-INGRESS-DCO-LOS-LOL"   ,  "EQPTFAIL"       ,  "TOP_MZ_POWER_RETIMER1_DCO_CDR_LOS_LOL"       },
        {   /* TMZ_RETIMER2_INGRESS_DCO_LOS_LOL    */  "TMZ-RETIMER2-INGRESS-DCO-LOS-LOL"   ,  "EQPTFAIL"       ,  "TOP_MZ_POWER_RETIMER2_DCO_CDR_LOS_LOL"       },
        {   /* TMZ_RETIMER3_INGRESS_DCO_LOS_LOL    */  "TMZ-RETIMER3-INGRESS-DCO-LOS-LOL"   ,  "EQPTFAIL"       ,  "TOP_MZ_POWER_RETIMER3_DCO_CDR_LOS_LOL"       },
        {   /* TMZ_RETIMER1_INGRESS_CLIENT_LOS_LOL */  "TMZ-RETIMER1-INGRESS-CLIENT-LOS-LOL",  "See Fac"        ,  "TOP_MZ_POWER_RETIMER1_CLIENT_CDR_LOS_LOL"    },
        {   /* TMZ_RETIMER2_INGRESS_CLIENT_LOS_LOL */  "TMZ-RETIMER2-INGRESS-CLIENT-LOS-LOL",  "See Fac"        ,  "TOP_MZ_POWER_RETIMER2_CLIENT_CDR_LOS_LOL"    },
        {   /* TMZ_RETIMER3_INGRESS_CLIENT_LOS_LOL */  "TMZ-RETIMER3-INGRESS-CLIENT-LOS-LOL",  "See Fac"        ,  "TOP_MZ_POWER_RETIMER3_CLIENT_CDR_LOS_LOL"    },
        {   /* TMZ_RETIMER1_INGRESS_FAULT          */  "TMZ-RETIMER1-INGRESS-FAULT"         ,  "EQPTFAIL"       ,  "TOP_MZ_POWER_RETIMER1_INGRESS_FAULT"         },
        {   /* TMZ_RETIMER2_INGRESS_FAULT          */  "TMZ-RETIMER2-INGRESS-FAULT"         ,  "EQPTFAIL"       ,  "TOP_MZ_POWER_RETIMER2_INGRESS_FAULT"         },
        {   /* TMZ_RETIMER3_INGRESS_FAULT          */  "TMZ-RETIMER3-INGRESS-FAULT"         ,  "EQPTFAIL"       ,  "TOP_MZ_POWER_RETIMER3_INGRESS_FAULT"         },
        {   /* TMZ_RETIMER1_LINK_DOWN              */  "TMZ-RETIMER1-LINK-DOWN"             ,  "EQPTFAIL"       ,  "TOP_MZ_POWER_RETIMER1_INGRESS_LINK_DOWN"     },
        {   /* TMZ_RETIMER2_LINK_DOWN              */  "TMZ-RETIMER2-LINK-DOWN"             ,  "EQPTFAIL"       ,  "TOP_MZ_POWER_RETIMER2_INGRESS_LINK_DOWN"     },
        {   /* TMZ_RETIMER3_LINK_DOWN              */  "TMZ-RETIMER3-LINK-DOWN"             ,  "EQPTFAIL"       ,  "TOP_MZ_POWER_RETIMER3_INGRESS_LINK_DOWN"     },
        {   /* TMZ_RETIMER1_EGRESS_LOS_LOL         */  "TMZ-RETIMER1-EGRESS-LOS-LOL"        ,  "EQPTFAIL"       ,  "TOP_MZ_POWER_RETIMER1_EGRESS_LOS_LOL"        },
        {   /* TMZ_RETIMER2_EGRESS_LOS_LOL         */  "TMZ-RETIMER2-EGRESS-LOS-LOL"        ,  "EQPTFAIL"       ,  "TOP_MZ_POWER_RETIMER2_EGRESS_LOS_LOL"        },
        {   /* TMZ_RETIMER3_EGRESS_LOS_LOL         */  "TMZ-RETIMER3-EGRESS-LOS-LOL"        ,  "EQPTFAIL"       ,  "TOP_MZ_POWER_RETIMER3_EGRESS_LOS_LOL"        },
        {   /* TMZ_RETIMER1_EGRESS_FAULT           */  "TMZ-RETIMER1-EGRESS-FAULT"          ,  "EQPTFAIL"       ,  "TOP_MZ_POWER_RETIMER1_EGRESS_FAULT"          },
        {   /* TMZ_RETIMER2_EGRESS_FAULT           */  "TMZ-RETIMER2-EGRESS-FAULT"          ,  "EQPTFAIL"       ,  "TOP_MZ_POWER_RETIMER2_EGRESS_FAULT"          },
        {   /* TMZ_RETIMER3_EGRESS_FAULT           */  "TMZ-RETIMER3-EGRESS-FAULT"          ,  "EQPTFAIL"       ,  "TOP_MZ_POWER_RETIMER3_EGRESS_FAULT"          },
        {   /* TMZ_RETIMER1_EGRESS_LINK_DOWN       */  "TMZ-RETIMER1-EGRESS-LINK-DOWN"      ,  "EQPTFAIL"       ,  "TOP_MZ_RETIMER1_EGRESS_LINK_DOWN"            },
        {   /* TMZ_RETIMER2_EGRESS_LINK_DOWN       */  "TMZ-RETIMER2-EGRESS-LINK-DOWN"      ,  "EQPTFAIL"       ,  "TOP_MZ_RETIMER2_EGRESS_LINK_DOWN"            },
        {   /* TMZ_RETIMER3_EGRESS_LINK_DOWN       */  "TMZ-RETIMER3-EGRESS-LINK-DOWN"      ,  "EQPTFAIL"       ,  "TOP_MZ_RETIMER3_EGRESS_LINK_DOWN"            },
        {   /* TMZ_RETIMER1_ACCESS_FAIL            */  "TMZ-RETIMER1-ACCESS-FAIL"           ,  "EQPTFAIL"       ,  "TOP_MZ_RETIMER1_I2C_FAILURE"                 },
        {   /* TMZ_RETIMER2_ACCESS_FAIL            */  "TMZ-RETIMER2-ACCESS-FAIL"           ,  "EQPTFAIL"       ,  "TOP_MZ_RETIMER2_I2C_FAILURE"                 },
        {   /* TMZ_RETIMER3_ACCESS_FAIL            */  "TMZ-RETIMER3-ACCESS-FAIL"           ,  "EQPTFAIL"       ,  "TOP_MZ_RETIMER3_I2C_FAILURE"                 },
        {   /* TMZ_IOEXP_ACCESS_FAIL               */  "TMZ-IOEXP-ACCESS-FAIL"              ,  "EQPTFAIL"       ,  "TOP_MZ_IOEXP_I2C_FAILURE"                    },
        {   /* TMZ_FPC1_ACCESS_FAIL                */  "TMZ-FPC1-ACCESS-FAIL"               ,  "EQPTFAIL"       ,  "TOP_MZ_FPC1_I2C_FAILURE"                     },
        {   /* TMZ_FPC2_ACCESS_FAIL                */  "TMZ-FPC2-ACCESS-FAIL"               ,  "EQPTFAIL"       ,  "TOP_MZ_FPC2_I2C_FAILURE"                     },
        {   /* TMZ_PWR_SEQ_ACCESS_FAIL             */  "TMZ-PWR-SEQ-ACCESS-FAIL"            ,  "EQPTFAIL"       ,  "TOP_MZ_POWER_SEQ_I2C_FAILURE"                },

        {   /* BMZ_DSPLL_OOL                       */  "BMZ-DSPLL-OOL"                      ,  "EQPTFAIL"       ,  "BOTTOM_MZ_PLL_OOL"                           },
        {   /* BMZ_OCXO_LOS                        */  "BMZ-OCXO-LOS"                       ,  "EQPTFAIL"       ,  "BOTTOM_MZ_INPUT_CLOCK_LOS_OOF"               },
        {   /* BMZ_I2C_CLKGEN_I2C_FAIL             */  "BMZ-I2C-CLKGEN-I2C-FAIL"            ,  "EQPTFAIL"       ,  "BOTTOM_MZ_CLOCK_GEN_I2C_FAULT"               },
        {   /* BMZ_TMP_OORH                        */  "BMZ-TMP-OORH"                       ,  "TEMP_OORH"      ,  "BOTTOM_MZ_OVER_TEMP"                         },
        {   /* BMZ_TMP_OORL                        */  "BMZ-TMP-OORL"                       ,  "TEMP_OORL"      ,  "BOTTOM_MZ_UNDER_TEMP"                        },
        {   /* BMZ_TMP_I2C_FAIL                    */  "BMZ-TMP-I2C-FAIL"                   ,  "EQPTFAIL"       ,  "BOTTOM_MZ_TMP_ACCESS_FAIL"                                          },
        {   /* BMZ_3_3V_TMP_OORH                   */  "BMZ-3-3V-TMP-OORH"                  ,  "TEMP_OORH"      ,  "BOTTOM_MZ_3_3_OVER_TEMP"                     },
        {   /* BMZ_3_3V_TMP_OORL                   */  "BMZ-3-3V-TMP-OORL"                  ,  "EQPTFAIL"       ,  "BOTTOM_MZ_3_3_OVER_CURRENT"                  },
        {   /* BMZ_3_3V_I2C_FAIL                   */  "BMZ-3-3V-I2C-FAIL"                  ,  "EQPTFAIL"       ,  "BOTTOM_MZ_3_3_I2C_FAIL"                      },
        {   /* BMZ_3_3V_VIN_OORL                   */  "BMZ-3-3V-VIN-OORL"                  ,  "EQPTFAIL"       ,  "BOTTOM_MZ_3_3_INPUT_OORL"                    },
        {   /* BMZ_3_3V_VOUT_OOR                   */  "BMZ-3-3V-VOUT-OOR"                  ,  "EQPTFAIL"       ,  "BOTTOM_MZ_POWER_3_3V_FAILURE"                },
        {   /* BMZ_1_8V_TMP_OORH                   */  "BMZ-1-8V-TMP-OORH"                  ,  "TEMP_OORH"      ,  "BOTTOM_MZ_0_8V_OVER_TEMP"                    },
        {   /* BMZ_1_8V_TMP_OORL                   */  "BMZ-1-8V-TMP-OORL"                  ,  "EQPTFAIL"       ,  "BOTTOM_MZ_0_8V_OVER_CURRENT"                 },
        {   /* BMZ_1_8V_I2C_FAIL                   */  "BMZ-1-8V-I2C-FAIL"                  ,  "EQPTFAIL"       ,  "BOTTOM_MZ_0_8V_COMM_FAULT"                   },
        {   /* BMZ_0_8V_VIN_OORL                   */  "BMZ-0-8V-VIN-OORL"                  ,  "EQPTFAIL"       ,  "BOTTOM_MZ_0_8_INPUT_OOR"                     },
        {   /* BMZ_0_8V_VOUT_OOR                   */  "BMZ-0-8V-VOUT-OOR"                  ,  "EQPTFAIL"       ,  "BOTTOM_MZ_POWER_0_8V_FAILURE"                },
        {   /* BMZ_TXDRV_VOUT_OOR                  */  "BMZ-TXDRV-VOUT-OOR"                 ,  "EQPTFAIL"       ,  "BOTTOM_MZ_TX_DRV_VOLTAGE_OOR"                },
        {   /* BMZ_H_TXDRV_VOUT_OOR                */  "BMZ-H-TXDRV-VOUT-OOR"               ,  "EQPTFAIL"       ,  "BOTTOM_MZ_H_TX_DRV_VOLTAGE_OOR"              },
        {   /* BMZ_AVS1_TMP_OORH                   */  "BMZ-AVS1-TMP-OORH"                  ,  "TEMP_OORH"      ,  "BOTTOM_MZ_AVS1_OVER_TEMP"                    },
        {   /* BMZ_AVS2_TMP_OORH                   */  "BMZ-AVS2-TMP-OORH"                  ,  "TEMP_OORH"      ,  "BOTTOM_MZ_AVS2_OVER_TEMP"                    },
        {   /* BMZ_AVS3_TMP_OORH                   */  "BMZ-AVS3-TMP-OORH"                  ,  "TEMP_OORH"      ,  "BOTTOM_MZ_AVS3_OVER_TEMP"                    },
        {   /* BMZ_AVS1_OC                         */  "BMZ-AVS1-OC"                        ,  "EQPTFAIL"       ,  "BOTTOM_MZ_AVS1_OVER_CURRENT"                 },
        {   /* BMZ_AVS2_OC                         */  "BMZ-AVS2-OC"                        ,  "EQPTFAIL"       ,  "BOTTOM_MZ_AVS2_OVER_CURRENT"                 },
        {   /* BMZ_AVS3_OC                         */  "BMZ-AVS3-OC"                        ,  "EQPTFAIL"       ,  "BOTTOM_MZ_AVS3_OVER_CURRENT"                 },
        {   /* BMZ_AVS1_ACCESS_FAIL                */  "BMZ-AVS1-ACCESS-FAIL"               ,  "EQPTFAIL"       ,  "BOTTOM_MZ_AVS1_COMM_FAULT"                   },
        {   /* BMZ_AVS2_ACCESS_FAIL                */  "BMZ-AVS2-ACCESS-FAIL"               ,  "EQPTFAIL"       ,  "BOTTOM_MZ_AVS2_COMM_FAULT"                   },
        {   /* BMZ_AVS3_ACCESS_FAIL                */  "BMZ-AVS3-ACCESS-FAIL"               ,  "EQPTFAIL"       ,  "BOTTOM_MZ_AVS3_COMM_FAULT"                   },
        {   /* BMZ_AVS1_VIN_OORL                   */  "BMZ-AVS1-VIN-OORL"                  ,  "EQPTFAIL"       ,  "BOTTOM_MZ_AVS1_INPUT_OOR"                    },
        {   /* BMZ_AVS2_VIN_OORL                   */  "BMZ-AVS2-VIN-OORL"                  ,  "EQPTFAIL"       ,  "BOTTOM_MZ_AVS2_INPUT_OOR"                    },
        {   /* BMZ_AVS3_VIN_OORL                   */  "BMZ-AVS3-VIN-OORL"                  ,  "EQPTFAIL"       ,  "BOTTOM_MZ_AVS3_INPUT_OOR"                    },
        {   /* BMZ_AVS1_VOUT_OOR                   */  "BMZ-AVS1-VOUT-OOR"                  ,  "EQPTFAIL"       ,  "BOTTOM_MZ_POWER_AVS1_FAILURE"                },
        {   /* BMZ_AVS2_VOUT_OOR                   */  "BMZ-AVS2-VOUT-OOR"                  ,  "EQPTFAIL"       ,  "BOTTOM_MZ_POWER_AVS2_FAILURE"                },
        {   /* BMZ-AVS3-VOUT-OOR                   */  "BMZ-AVS3-VOUT-OOR"                  ,  "EQPTFAIL"       ,  "BOTTOM_MZ_POWER_AVS3_FAILURE"                },
        {   /* BMZ-RETIMER1-FW-FAIL                */  "BMZ-RETIMER1-FW-FAIL"               ,  "EQPTFAIL"       ,  "BOTTOM_MZ_RETIMER1_FW_FAILURE"               },
        {   /* BMZ-RETIMER2-FW-FAIL                */  "BMZ-RETIMER2-FW-FAIL"               ,  "EQPTFAIL"       ,  "BOTTOM_MZ_RETIMER2_FW_FAILURE"               },
        {   /* BMZ-RETIMER3-FW-FAIL                */  "BMZ-RETIMER3-FW-FAIL"               ,  "EQPTFAIL"       ,  "BOTTOM_MZ_RETIMER3_FW_FAILURE"               },
        {   /* BMZ-RETIMER1-AVS-FAIL               */  "BMZ-RETIMER1-AVS-FAIL"              ,  "EQPTFAIL"       ,  "BOTTOM_MZ_POWER_RETIMER1_AVS_FAILURE"        },
        {   /* BMZ-RETIMER2-AVS-FAIL               */  "BMZ-RETIMER2-AVS-FAIL"              ,  "EQPTFAIL"       ,  "BOTTOM_MZ_POWER_RETIMER2_AVS_FAILURE"        },
        {   /* BMZ-RETIMER3-AVS-FAIL               */  "BMZ-RETIMER3-AVS-FAIL"              ,  "EQPTFAIL"       ,  "BOTTOM_MZ_POWER_RETIMER3_AVS_FAILURE"        },
        {   /* BMZ-RETIMER1-INGRESS-DCO-LOS-LOL    */  "BMZ-RETIMER1-INGRESS-DCO-LOS-LOL"   ,  "EQPTFAIL"       ,  "BOTTOM_MZ_POWER_RETIMER1_DCO_CDR_LOS_LOL"    },
        {   /* BMZ-RETIMER2-INGRESS-DCO-LOS-LOL    */  "BMZ-RETIMER2-INGRESS-DCO-LOS-LOL"   ,  "EQPTFAIL"       ,  "BOTTOM_MZ_POWER_RETIMER2_DCO_CDR_LOS_LOL"    },
        {   /* BMZ-RETIMER3-INGRESS-DCO-LOS-LOL    */  "BMZ-RETIMER3-INGRESS-DCO-LOS-LOL"   ,  "EQPTFAIL"       ,  "BOTTOM_MZ_POWER_RETIMER3_DCO_CDR_LOS_LOL"    },
        {   /* BMZ-RETIMER1-INGRESS-CLIENT-LOS-LOL */  "BMZ-RETIMER1-INGRESS-CLIENT-LOS-LOL",  "See Fac"        ,  "BOTTOM_MZ_POWER_RETIMER1_CLIENT_CDR_LOS_LOL" },
        {   /* BMZ-RETIMER2-INGRESS-CLIENT-LOS-LOL */  "BMZ-RETIMER2-INGRESS-CLIENT-LOS-LOL",  "See Fac"        ,  "BOTTOM_MZ_POWER_RETIMER2_CLIENT_CDR_LOS_LOL" },
        {   /* BMZ-RETIMER3-INGRESS-CLIENT-LOS-LOL */  "BMZ-RETIMER3-INGRESS-CLIENT-LOS-LOL",  "See Fac"        ,  "BOTTOM_MZ_POWER_RETIMER3_CLIENT_CDR_LOS_LOL" },
        {   /* BMZ-RETIMER1-INGRESS-FAULT          */  "BMZ-RETIMER1-INGRESS-FAULT"         ,  "EQPTFAIL"       ,  "BOTTOM_MZ_POWER_RETIMER1_INGRESS_FAULT"      },
        {   /* BMZ-RETIMER2-INGRESS-FAULT          */  "BMZ-RETIMER2-INGRESS-FAULT"         ,  "EQPTFAIL"       ,  "BOTTOM_MZ_POWER_RETIMER2_INGRESS_FAULT"      },
        {   /* BMZ-RETIMER3-INGRESS-FAULT          */  "BMZ-RETIMER3-INGRESS-FAULT"         ,  "EQPTFAIL"       ,  "BOTTOM_MZ_POWER_RETIMER3_INGRESS_FAULT"      },
        {   /* BMZ-RETIMER1-LINK-DOWN              */  "BMZ-RETIMER1-LINK-DOWN"             ,  "EQPTFAIL"       ,  "BOTTOM_MZ_POWER_RETIMER1_INGRESS_LINK_DOWN"  },
        {   /* BMZ-RETIMER2-LINK-DOWN              */  "BMZ-RETIMER2-LINK-DOWN"             ,  "EQPTFAIL"       ,  "BOTTOM_MZ_POWER_RETIMER2_INGRESS_LINK_DOWN"  },
        {   /* BMZ-RETIMER3-LINK-DOWN              */  "BMZ-RETIMER3-LINK-DOWN"             ,  "EQPTFAIL"       ,  "BOTTOM_MZ_POWER_RETIMER3_INGRESS_LINK_DOWN"  },
        {   /* BMZ-RETIMER1-EGRESS-LOS-LOL         */  "BMZ-RETIMER1-EGRESS-LOS-LOL"        ,  "EQPTFAIL"       ,  "BOTTOM_MZ_POWER_RETIMER1_EGRESS_LOS_LOL"     },
        {   /* BMZ-RETIMER2-EGRESS-LOS-LOL         */  "BMZ-RETIMER2-EGRESS-LOS-LOL"        ,  "EQPTFAIL"       ,  "BOTTOM_MZ_POWER_RETIMER2_EGRESS_LOS_LOL"     },
        {   /* BMZ-RETIMER3-EGRESS-LOS-LOL         */  "BMZ-RETIMER3-EGRESS-LOS-LOL"        ,  "EQPTFAIL"       ,  "BOTTOM_MZ_POWER_RETIMER3_EGRESS_LOS_LOL"     },
        {   /* BMZ-RETIMER1-EGRESS-FAULT           */  "BMZ-RETIMER1-EGRESS-FAULT"          ,  "EQPTFAIL"       ,  "BOTTOM_MZ_POWER_RETIMER1_EGRESS_FAULT"       },
        {   /* BMZ-RETIMER2-EGRESS-FAULT           */  "BMZ-RETIMER2-EGRESS-FAULT"          ,  "EQPTFAIL"       ,  "BOTTOM_MZ_POWER_RETIMER2_EGRESS_FAULT"       },
        {   /* BMZ-RETIMER3-EGRESS-FAULT           */  "BMZ-RETIMER3-EGRESS-FAULT"          ,  "EQPTFAIL"       ,  "BOTTOM_MZ_POWER_RETIMER3_EGRESS_FAULT"       },
        {   /* BMZ-RETIMER1-EGRESS-LINK-DOWN       */  "BMZ-RETIMER1-EGRESS-LINK-DOWN"      ,  "EQPTFAIL"       ,  "BOTTOM_MZ_RETIMER1_EGRESS_LINK_DOWN"         },
        {   /* BMZ-RETIMER2-EGRESS-LINK-DOWN       */  "BMZ-RETIMER2-EGRESS-LINK-DOWN"      ,  "EQPTFAIL"       ,  "BOTTOM_MZ_RETIMER2_EGRESS_LINK_DOWN"         },
        {   /* BMZ-RETIMER3-EGRESS-LINK-DOWN       */  "BMZ-RETIMER3-EGRESS-LINK-DOWN"      ,  "EQPTFAIL"       ,  "BOTTOM_MZ_RETIMER3_EGRESS_LINK_DOWN"         },
        {   /* BMZ-RETIMER1-ACCESS-FAIL            */  "BMZ-RETIMER1-ACCESS-FAIL"           ,  "EQPTFAIL"       ,  "BOTTOM_MZ_RETIMER1_I2C_FAILURE"              },
        {   /* BMZ-RETIMER2-ACCESS-FAIL            */  "BMZ-RETIMER2-ACCESS-FAIL"           ,  "EQPTFAIL"       ,  "BOTTOM_MZ_RETIMER2_I2C_FAILURE"              },
        {   /* BMZ-RETIMER3-ACCESS-FAIL            */  "BMZ-RETIMER3-ACCESS-FAIL"           ,  "EQPTFAIL"       ,  "BOTTOM_MZ_RETIMER3_I2C_FAILURE"              },
        {   /* BMZ-IOEXP-ACCESS-FAIL               */  "BMZ-IOEXP-ACCESS-FAIL"              ,  "EQPTFAIL"       ,  "BOTTOM_MZ_IOEXP_I2C_FAILURE"                 },
        {   /* BMZ-FPC1-ACCESS-FAIL                */  "BMZ-FPC1-ACCESS-FAIL"               ,  "EQPTFAIL"       ,  "BOTTOM_MZ_FPC1_I2C_FAILURE"                  },
        {   /* BMZ-FPC2-ACCESS-FAIL                */  "BMZ-FPC2-ACCESS-FAIL"               ,  "EQPTFAIL"       ,  "BOTTOM_MZ_FPC2_I2C_FAILURE"                  },
        {   /* BMZ-PWR-SEQ-ACCESS-FAIL             */  "BMZ-PWR-SEQ-ACCESS-FAIL"            ,  "EQPTFAIL"       ,  "BOTTOM_MZ_POWER_SEQ_I2C_FAILURE"             }
};

const DigitalInputFaultData cFpgaHostBoardFaults[] =
{
     /* ** Fault ID **      ,          ** Input Mask **     ,      **Input Polarity**        , ** IsInitFault ** */
     { HOST_OCXO_FREQ_OOR   , cFPGA_MS_OCXO_CLOCK_FAULT     ,  cFPGA_MS_OCXO_CLOCK_FAULT     ,   true     },
     { HOST_HS_FAIL         , cFPGA_MS_HSWAP_ALERT          , ~cFPGA_MS_HSWAP_ALERT          ,   true     },
     { HOST_12V_UV          , cFPGA_MS_POWER_12V_UV_ALARM   ,  cFPGA_MS_POWER_12V_UV_ALARM   ,   true     },
     { HOST_12V_OV          , cFPGA_MS_PWR_12V_OV_ALARM     ,  cFPGA_MS_PWR_12V_OV_ALARM     ,   true     },
     { HOST_5V_UV           , cFPGA_MS_POWER_SEC_UV_OV_ALARM,  cFPGA_MS_POWER_SEC_UV_OV_ALARM,   true     },
     { BOARD_TEMP_OORH      , cFPGA_MS_TEMP_ALARM           , ~cFPGA_MS_TEMP_ALARM           ,   true     }
};
const uint32 cNumFpgaHostBoardFaults = sizeof(cFpgaHostBoardFaults) / sizeof(cFpgaHostBoardFaults[0]);

const uint32 cMezzFaultIdOffset = BMZ_DSPLL_OOL - TMZ_DSPLL_OOL;

const DigitalInputFaultData cMezzIoExpFaults[] =
{
    /* ** Fault ID **      , ** Input Mask **            , **Input Polarity**       , ** IsInitFault ** */
    { TMZ_DSPLL_OOL        , cMIOEXP_PLLCLK_LOLB_L       , ~cMIOEXP_PLLCLK_LOLB_L   ,   true        },
    { TMZ_OCXO_LOS         , cMIOEXP_PLLCLK_INTRB_L      , ~cMIOEXP_PLLCLK_INTRB_L  ,   true        },
    { TMZ_3_3V_VOUT_OOR    , cMIOEXP_P3V8_PGOOD          , ~cMIOEXP_P3V8_PGOOD      ,   true        },
    { TMZ_0_8V_VOUT_OOR    , cMIOEXP_P0V8_PGOOD          , ~cMIOEXP_P0V8_PGOOD      ,   true        },
    { TMZ_AVS1_VOUT_OOR    , cMIOEXP_P0V8_AVS1_PGOOD     , ~cMIOEXP_P0V8_AVS1_PGOOD ,   true        },
    { TMZ_AVS2_VOUT_OOR    , cMIOEXP_P0V8_AVS2_PGOOD     , ~cMIOEXP_P0V8_AVS2_PGOOD ,   true        },
    { TMZ_AVS3_VOUT_OOR    , cMIOEXP_P0V8_AVS3_PGOOD     , ~cMIOEXP_P0V8_AVS3_PGOOD ,   true        }
};

const uint32 cNumMezzIoExpFaults
    = sizeof(cMezzIoExpFaults) / sizeof(cMezzIoExpFaults[0]);

Chm6BoardFault::Chm6BoardFault( BoardFaultId id, bool isSimEn,
                                faultConditionType condition)
    : mId(id)
    , mCondition(condition)
    , mSimEn(isSimEn)
    , mSimCondition(FAULT_UNKNOWN)
{
    if (id < MAX_BOARD_FAULT_ID_NUM)
    {
        mName = BoardFaultIdToName(id);
        mDescriptiveText = cBoardFaultDefs[id].mFaultDescr;
        mAlarmType = cBoardFaultDefs[id].mFaultType;
    }
    else
    {
        assert(("Fault Id: "
                + std::to_string(static_cast<uint32>(id))
                + " Beyond Max: "
                + std::to_string(
                    static_cast<uint32>(MAX_BOARD_FAULT_ID_NUM))
                        , false));
    }

    if (mSimEn)
    {
        mSimCondition = condition;
    }
}

// Check and Update Fault Condition
void Chm6BoardFault::CheckFaultCondition()
{
    DLOG << "";

    if (mSimEn)
    {
        DLOG << "Sim Enabled for alarm Id: " << mId
             << " Condition: "
             << BoardFltCondiToStr(mSimCondition);
        return;
    }

    DLOG << "Checking fault: " << static_cast<uint32>(mId);
    faultConditionType newCondition = CheckFault();

    if ((mCondition != newCondition)
        && (newCondition != FAULT_UNKNOWN))
    {
        DLOG << "Fault Id: "<< static_cast<uint32>(mId)
             << " Fault State Changed to '"
             << BoardFltCondiToStr(newCondition)
             << "' from Previous Condition: '"
             << BoardFltCondiToStr(mCondition)
             << "'";

        mCondition = newCondition;
    }
}

faultConditionType Chm6BoardFault::CheckFault()
{
    INFN_LOG(SeverityLevel::debug) << " Base Class Impl. Returning Clear";

    return FAULT_UNKNOWN;
}

std::string Chm6BoardFault::BoardFaultIdToName(BoardFaultId id)
{
    if (id < MAX_BOARD_FAULT_ID_NUM)
    {
        return (std::string(cBoardFaultDefs[id].mFaultName));
    }
    else
    {
        return std::string("");
    }
}

std::string Chm6BoardFault::BoardFltCondiToStr(faultConditionType type)
{
    switch(type)
    {
        case FAULT_UNKNOWN:
            return(std::string("Unknown"));
        case FAULT_SET:
            return(std::string("Set"));
        case FAULT_CLEAR:
            return(std::string("Clear"));
        default:
            return(std::string("Unknown"));
    }
}

void Chm6BoardFault::DumpHeader(std::ostream &os)
{
    os << boost::format(
          "%-3d : %-36s : %-9s : %-12s : %9b : %-9s")
      % "ID"
      % "Name"
      % "Condition"
      % "AlarmType"
      % "SimEnable"
      % "SimCondit"
       << std::endl;
}

void Chm6BoardFault::DumpHeaderSpecific(std::ostream &os)
{
    DumpHeader(os);
}

void Chm6BoardFault::Dump(std::ostream &os)
{
    os << boost::format(
          "%-3d : %-36s : %-9s : %-12s : %9b : %-9s")
      % static_cast<int>(mId)
      % mName
      % BoardFltCondiToStr(mCondition)
      % mAlarmType
      % mSimEn
      % BoardFltCondiToStr(mSimCondition)
       << std::endl;
}

void Chm6BoardFault::DumpSpecific(std::ostream &os)
{
    Dump(os);
}

BoardFaultFpga::BoardFaultFpga(const DigitalInputFaultData& inputFltData,
                               std::shared_ptr<BoardCommonDriver> brdDriver,
                               bool isSimEn,
                               faultConditionType condition)
    : Chm6BoardFault(inputFltData.mFaultId, isSimEn,condition)
    , mInputMsk(inputFltData.mInputMask)
    , mInputPol(inputFltData.mInputPolarity)
    , mspBrdDriver(brdDriver)
{

}

faultConditionType BoardFaultFpga::CheckFault()
{
    return GetFaultState();
}

faultConditionType BoardFaultFpga::GetFaultState()
{
    uint32 regVal;
    int errCode = GetRegValue(regVal);

    if (errCode)
    {
        INFN_LOG(SeverityLevel::error)
          << "Failure detected when reading register value."
          << " Unable to determine fault state";

        return FAULT_UNKNOWN;
    }

    INFN_LOG(SeverityLevel::debug) << "Fault Id: "
                    << static_cast<uint32>(mId)
                    << " Reg Value: 0x"
                    << std::hex << regVal << std::dec;

    uint32 maskedVal, bitAlmState;

    maskedVal   = regVal & mInputMsk;
    bitAlmState = maskedVal ^ mInputPol;

    bool isFaultSet = ((bitAlmState & mInputMsk) != mInputMsk);

    INFN_LOG(SeverityLevel::debug) << std::hex << "mInputMsk: "    << mInputMsk <<
                                   " maskedVal: "   << maskedVal <<
                                   " bitAlmState: " << bitAlmState << std::dec  <<
                                   " isFaultSet: "  << isFaultSet;

    if (isFaultSet)
    {
        return FAULT_SET;
    }
    else
    {
        return FAULT_CLEAR;
    }
}

int BoardFaultFpga::GetRegValue(const uint32 &regVal)
{
    return mspBrdDriver->GetFpgaMiscStatus(
               const_cast<uint32&>(regVal));
}

BoardFaultMezzIoExp::BoardFaultMezzIoExp(
    const DigitalInputFaultData& inputFltData,
    std::shared_ptr<BoardCommonDriver> brdDriver,
    mezzBoardIdType mezzBrdId,
    bool isSimEn,
    faultConditionType condition)
    : BoardFaultFpga(inputFltData, brdDriver, isSimEn, condition)
    , mMezzBrdId(mezzBrdId)
{
}

int BoardFaultMezzIoExp::GetRegValue(const uint32 &regVal)
{
    return mspBrdDriver->GetFpgaIoExpInput(
              mMezzBrdId, const_cast<uint32&>(regVal));
}


// Implementation of C++11 "Meyers Singleton"
// Automatically thread-safe in C++11.
//
AccessFaultsMap& AccessFaultsMap::getInstance()
{
    static AccessFaultsMap theInstance;
    return theInstance;
}

AccessFaultsMap::AccessFaultsMap()
{
    try
    {
        if( true == IsSimEnv() )
        {
            SIM_InitBoardAccessFaultsMap();
            SIM_InitBoardMezAccessFaultsMap_b();
            SIM_InitBoardMezAccessFaultsMap_t();
        }
        else
        {
            InitBoardAccessFaultsMap();
            InitBoardMezAccessFaultsMap_b();
            InitBoardMezAccessFaultsMap_t();
        }
    }
    catch(...) {}
}

const bool AccessFaultsMap::IsSimEnv()
{
    bool bRet = true;
    const char *pEnvStr = getenv( "BoardInfo" );
    if( NULL == pEnvStr
        || string( pEnvStr ) != string( "sim" ) )
    {
        bRet = false;
    }

    return bRet;
}

void AccessFaultsMap::InitBoardAccessFaultsMap()
{
    // This creates a mapping of
    // eBoardAF enums to the corresponding
    // BoardFaultId enums.
    //
    const auto& p = RegIfFactorySingleton::Instance();

    MAP_AF( HOST_PWR_SEQ_ACCESS_FAIL
        , p->CreateFpgaPsI2c0PwrSeqIf()
        , cAdm1066MANID );

    MAP_AF( HOST_HS_ACCESS_FAIL
        , p->CreateFpgaPsI2c0HotSwap()
        , cMp5023AddrBusRev );

    MAP_AF( INLET_TEMP_ACCESS_FAIL
        , p->CreateFpgaPlI2c4InletTempSensorRegIf()
        , 0 );

    MAP_AF( BOARD_TEMP_ACCESS_FAIL
        , p->CreateFpgaPlI2c4OutletTempSensorRegIf()
        , 0 );

    MAP_AF( MFG_EEPROM_ACCESS_FAIL
        , p->CreateFpgaPlI2c4MfgEepromRegIf()
        , 0 );

    MAP_AF( CAL_EEPROM_ACCESS_FAIL
        , p->CreateFpgaPlI2c4CalEepromRegIf()
        , 0);
    //
    //
    // Add more host access faults here...
}

void AccessFaultsMap::SIM_InitBoardAccessFaultsMap()
{
    MAP_AF_SIM( HOST_PWR_SEQ_ACCESS_FAIL );
    MAP_AF_SIM( HOST_HS_ACCESS_FAIL );
    MAP_AF_SIM( INLET_TEMP_ACCESS_FAIL );
    MAP_AF_SIM( BOARD_TEMP_ACCESS_FAIL );
    MAP_AF_SIM( MFG_EEPROM_ACCESS_FAIL );
    MAP_AF_SIM( CAL_EEPROM_ACCESS_FAIL );
    //
    // Add more access faults here...
}

void AccessFaultsMap::InitBoardMezAccessFaultsMap_b()
{
    // This creates a mapping of
    // eBoardAF enums to the corresponding
    // BoardFaultId enums.
    //
    const auto& p = RegIfFactorySingleton::Instance();

    // BOTTOM MEZ
    MAP_MEZ_AF_b( BMZ_I2C_CLKGEN_I2C_FAIL
       , p->CreateBottomMzSi5394RegIf()
       , 0 );

    MAP_MEZ_AF_b( BMZ_IOEXP_ACCESS_FAIL
       , p->CreateBottomMzIOExpanderRegIf()
       , 0 );

    MAP_MEZ_AF_b( BMZ_FPC1_ACCESS_FAIL
       , p->CreateBottomMzFPC402_1RegIf()
       , 0 );

    MAP_MEZ_AF_b( BMZ_FPC2_ACCESS_FAIL
       , p->CreateBottomMzFPC402_2RegIf()
       , 0 );

#ifndef ERR_110_177
    MAP_MEZ_AF_b( BMZ_PWR_SEQ_ACCESS_FAIL
       , p->CreateFpgaPlI2c0VoltageSequencerRegIf()
       , cAdm1066MANID );
#endif

    MAP_MEZ_AF_b( BMZ_TMP_I2C_FAIL
       , p->CreateFpgaPlI2c0TempSensorRegIf()
       , 0 );

    //
    // Add more access faults here...
}

void AccessFaultsMap::InitBoardMezAccessFaultsMap_t()
{
    // This creates a mapping of
    // eBoardAF enums to the corresponding
    // BoardFaultId enums.
    //
    const auto& p = RegIfFactorySingleton::Instance();

    // TOP MEZ
    MAP_MEZ_AF_t( TMZ_I2C_CLKGEN_I2C_FAIL
       , p->CreateTopMzSi5394RegIf()
       , 0 );

    MAP_MEZ_AF_t( TMZ_IOEXP_ACCESS_FAIL
       , p->CreateTopMzIOExpanderRegIf()
       , 0 );

    MAP_MEZ_AF_t( TMZ_FPC1_ACCESS_FAIL
       , p->CreateTopMzFPC402_1RegIf()
       , 0 );

    MAP_MEZ_AF_t( TMZ_FPC2_ACCESS_FAIL
       , p->CreateTopMzFPC402_2RegIf()
       , 0 );

#ifndef ERR_110_177
    MAP_MEZ_AF_t( TMZ_PWR_SEQ_ACCESS_FAIL
       , p->CreateFpgaPlI2c1VoltageSequencerRegIf()
       , cAdm1066MANID );
#endif

    MAP_MEZ_AF_t( TMZ_TMP_I2C_FAIL
       , p->CreateFpgaPlI2c1TempSensorRegIf()
       , 0 );

    //
    // Add more access faults here...
}

void AccessFaultsMap::SIM_InitBoardMezAccessFaultsMap_b()
{
    MAP_MEZ_AF_SIM_b( BMZ_I2C_CLKGEN_I2C_FAIL );
    MAP_MEZ_AF_SIM_b( BMZ_IOEXP_ACCESS_FAIL );
    MAP_MEZ_AF_SIM_b( BMZ_FPC1_ACCESS_FAIL );
    MAP_MEZ_AF_SIM_b( BMZ_FPC2_ACCESS_FAIL );
#ifndef ERR_110_177
    MAP_MEZ_AF_SIM_b( BMZ_PWR_SEQ_ACCESS_FAIL );
#endif
    MAP_MEZ_AF_SIM_b( BMZ_TMP_I2C_FAIL );
    //
    // Add more access faults here...
}

void AccessFaultsMap::SIM_InitBoardMezAccessFaultsMap_t()
{
    MAP_MEZ_AF_SIM_t( TMZ_I2C_CLKGEN_I2C_FAIL );
    MAP_MEZ_AF_SIM_t( TMZ_IOEXP_ACCESS_FAIL );
    MAP_MEZ_AF_SIM_t( TMZ_FPC1_ACCESS_FAIL );
    MAP_MEZ_AF_SIM_t( TMZ_FPC2_ACCESS_FAIL );
#ifndef ERR_110_177
    MAP_MEZ_AF_SIM_t( TMZ_PWR_SEQ_ACCESS_FAIL );
#endif
    MAP_MEZ_AF_SIM_t( TMZ_TMP_I2C_FAIL );
    //
    // Add more access faults here...
}


// Implementation of C++11 "Meyers Singleton"
// Automatically thread-safe in C++11.
//
RT_AccessFaultsMap& RT_AccessFaultsMap::getInstance()
{
    static RT_AccessFaultsMap theInstance;
    return theInstance;
}

RT_AccessFaultsMap::RT_AccessFaultsMap()
{
    try
    {
        if( true == IsSimEnv() )
        {
            SIM_InitBoardAccessFaultsMap();
            SIM_InitBoardMezAccessFaultsMap_b();
            SIM_InitBoardMezAccessFaultsMap_t();
        }
        else
        {
            InitBoardAccessFaultsMap();
            InitBoardMezAccessFaultsMap_b();
            InitBoardMezAccessFaultsMap_t();
        }
    }
    catch(...) {}
}

const bool RT_AccessFaultsMap::IsSimEnv()
{
    bool bRet = true;
    const char *pEnvStr = getenv( "BoardInfo" );
    if( NULL == pEnvStr
        || string( pEnvStr ) != string( "sim" ) )
    {
        bRet = false;
    }

    return bRet;
}

void RT_AccessFaultsMap::InitBoardAccessFaultsMap()
{
    // This creates a mapping of
    // eBoardAF enums to the corresponding
    // BoardFaultId enums.
    //
    const auto& p = RegIfFactorySingleton::Instance();

    MAP_AF( HOST_PWR_SEQ_ACCESS_FAIL
        , p->CreateFpgaPsI2c0PwrSeqIf()
        , cAdm1066MANID );

    MAP_AF( HOST_HS_ACCESS_FAIL
        , p->CreateFpgaPsI2c0HotSwap()
        , cMp5023AddrBusRev );

    MAP_AF( INLET_TEMP_ACCESS_FAIL
        , p->CreateFpgaPlI2c4InletTempSensorRegIf()
        , 0 );

    MAP_AF( BOARD_TEMP_ACCESS_FAIL
        , p->CreateFpgaPlI2c4OutletTempSensorRegIf()
        , 0 );

    MAP_AF( MFG_EEPROM_ACCESS_FAIL
        , p->CreateFpgaPlI2c4MfgEepromRegIf()
        , 0 );

    MAP_AF( CAL_EEPROM_ACCESS_FAIL
        , p->CreateFpgaPlI2c4CalEepromRegIf()
        , 0);
    //
    //
    // Add more host access faults here...
}

void RT_AccessFaultsMap::SIM_InitBoardAccessFaultsMap()
{
    MAP_AF_SIM( HOST_PWR_SEQ_ACCESS_FAIL );
    MAP_AF_SIM( HOST_HS_ACCESS_FAIL );
    MAP_AF_SIM( INLET_TEMP_ACCESS_FAIL );
    MAP_AF_SIM( BOARD_TEMP_ACCESS_FAIL );
    MAP_AF_SIM( MFG_EEPROM_ACCESS_FAIL );
    MAP_AF_SIM( CAL_EEPROM_ACCESS_FAIL );
    //
    // Add more access faults here...
}

void RT_AccessFaultsMap::InitBoardMezAccessFaultsMap_b()
{
    // This creates a mapping of
    // eBoardAF_RT enums to the corresponding
    // BoardFaultId enums.
    //
    const auto& p = RegIfFactorySingleton::Instance();

#ifndef ERR_110_177
    MAP_MEZ_AF_RT_b( BMZ_PWR_SEQ_ACCESS_FAIL
       , p->CreateFpgaPlI2c0VoltageSequencerRegIf()
       , cAdm1066MANID );
#endif

    MAP_MEZ_AF_RT_b( BMZ_TMP_I2C_FAIL
       , p->CreateFpgaPlI2c0TempSensorRegIf()
       , 0 );
    //
    //
    // Add more access faults here...
}

void RT_AccessFaultsMap::InitBoardMezAccessFaultsMap_t()
{
    // This creates a mapping of
    // eBoardAF_RT enums to the corresponding
    // BoardFaultId enums.
    //
    const auto& p = RegIfFactorySingleton::Instance();

#ifndef ERR_110_177
    MAP_MEZ_AF_RT_t( TMZ_PWR_SEQ_ACCESS_FAIL
       , p->CreateFpgaPlI2c1VoltageSequencerRegIf()
       , cAdm1066MANID );
#endif

    MAP_MEZ_AF_RT_t( TMZ_TMP_I2C_FAIL
       , p->CreateFpgaPlI2c1TempSensorRegIf()
       , 0 );
    //
    //
    // Add more access faults here...
}


void RT_AccessFaultsMap::SIM_InitBoardMezAccessFaultsMap_b()
{
#ifndef ERR_110_177
    MAP_MEZ_AF_RT_SIM_b( BMZ_PWR_SEQ_ACCESS_FAIL );
#endif
    MAP_MEZ_AF_RT_SIM_b( BMZ_TMP_I2C_FAIL );
    //
    // Add more access faults here...
}

void RT_AccessFaultsMap::SIM_InitBoardMezAccessFaultsMap_t()
{
#ifndef ERR_110_177
    MAP_MEZ_AF_RT_SIM_t( TMZ_PWR_SEQ_ACCESS_FAIL );
#endif
    MAP_MEZ_AF_RT_SIM_t( TMZ_TMP_I2C_FAIL );
    //
    // Add more access faults here...
}



BoardAccessFault::BoardAccessFault( const AFV& afv )
    : Chm6BoardFault(afv.boardFaultId
                   , afv.bSim
                   , afv.fCondition)
    , regAddr_(afv.registerAddr)
    , mspRegIfDvr(afv.devicePtr)
    , deviceAddress_(afv.devicePtr->GetDevAddr())
    , curDevAddr_(deviceAddress_)
    , mEnabled(false)
    , fidName_(afv.fidName)
    {
        assert((nullptr != mspRegIfDvr));

        DLOG << "Fault Id = " << mId;
        DLOG << "Fault Name: " << fidName_;
        DLOG << "sim = " << mSimEn;
        DLOG << "condition = " << mCondition;
        DLOG << "regAddr_ = "
             << std::hex << regAddr_;
        DLOG << "mspRegIfDvr = "
             << std::hex << mspRegIfDvr;
        DLOG << "deviceAddress_ = "
             << std::hex << deviceAddress_;
        DLOG << "mEnabled = " << mEnabled
             << endl;
    }

void BoardAccessFault::SetDevAddr(const uint32& devaddr)
{
    curDevAddr_ = devaddr;

    DLOG << fidName_ << " : " << mId
         << " : Setting DevAddr to: "
         << std::hex << curDevAddr_;

    mspRegIfDvr->SetDevAddr(curDevAddr_);
    mEnabled = true;

    DLOG << "mEnabled = " << mEnabled;
}

void BoardAccessFault::RestoreDevAddr()
{
    DLOG << fidName_ << " : " << mId
         << " : Restoring DevAddr to: "
         << std::hex << deviceAddress_;

    SetDevAddr(deviceAddress_);
    mEnabled = false;

    DLOG << "mEnabled = " << mEnabled;
}

void BoardAccessFault::TestAccess()
{
    DLOG << fidName_ << " : " << mId
         << " : reg address: " << std::hex << regAddr_
         << " @ dev address: " << std::hex << curDevAddr_;

    mspRegIfDvr->Read8(regAddr_);
}

faultConditionType BoardAccessFault::CheckFault()
{
    DLOG << fidName_ << " : " << mId;

    const int numAccessAttempts = 10;
    const int sleepMS = 10;

    // Call TestAccess a number of times with a sleep value.
    // If there are now exceptions thrown,
    // then return FAULT_CLEAR, otherwise, return a FAULT_SET.
    bool isPass = false;
    uint32 count = 0;
    std::string errLog;
    try
    {
        vector<int> v(numAccessAttempts);
        for( const auto& x : v )
        {
            TestAccess();

            ++count;

            std::this_thread::sleep_for(
                std::chrono::milliseconds(sleepMS));
        }
        isPass = true;
    }
    catch (regIf::RegIfException &ex)
    {
        errLog = "Error: Fpga Access Failed. Ex: " + std::to_string(ex.GetError());
    }
    catch(exception& ex)
    {
        std::string exStr(ex.what());
        errLog =  "Caught Exception: " + exStr;
    }
    catch( ... )
    {
        errLog = "Caught Exception";
    }

    if (!isPass)
    {
        INFN_LOG(SeverityLevel::error) << errLog;

        INFN_LOG(SeverityLevel::error) << "FAULT DETECTED for Fault: "
                        << fidName_ << " : " << mId;

        INFN_LOG(SeverityLevel::error) << " Dev Addr: " << curDevAddr_
                        << " Reg Addr: " << regAddr_
                        << " Good Read Count: " << count;

        return FAULT_SET;
    }

    DLOG << "Cleared Fault: " << fidName_ << " : " << mId;
    return FAULT_CLEAR;
}

void BoardAccessFault::DumpHeaderSpecific(std::ostream &os)
{
    os << boost::format(
          "%-3d : %-36s : %-9s : %-12s : %9b : %-9s : %9b : %-02x")
      % "ID"
      % "Name"
      % "Condition"
      % "AlarmType"
      % "SimEnable"
      % "SimCondit"
      % "Enabled"
      % "DeviceTestAddr"
       << std::endl;
}

void BoardAccessFault::DumpSpecific(std::ostream &os)
{
    os << boost::format(
          "%-3d : %-36s : %-9s : %-12s : %9b : %-9s : %9b : %-02x")
      % static_cast<int>(mId)
      % mName
      % BoardFltCondiToStr(mCondition)
      % mAlarmType
      % mSimEn
      % BoardFltCondiToStr(mSimCondition)
      % mEnabled
      % curDevAddr_
       << std::endl;
}

} //namespace boardMs

