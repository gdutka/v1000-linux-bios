/* $NoKeywords:$ */
/**
 * @file
 *
 * APCB DXE Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  APCB
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
//#include "AGESA.h"
#include "Porting.h"
#include "SocLogicalId.h"
#include "Addendum/Apcb/Inc/SSP/APCB.h"
#include "ApcbCommon.h"
#include <Library/IdsLib.h>
#include <Filecode.h>

#define FILECODE PSP_APCB_TOKENSSSP_CONFIG_TOKENS_FILECODE

//
// APCB Config Token ID definitions
//
TOKEN_PAIR ConfigTokenPair[] = {
  { APCB_ID_CONFIG_CCX_MIN_SEV_ASID,                APCB_TOKEN_UID_CCX_MIN_SEV_ASID                   },
  { APCB_ID_CONFIG_CCX_SEV_ASID_COUNT,              APCB_TOKEN_UID_CCX_SEV_ASID_COUNT                 },
  { APCB_ID_CONFIG_CCX_XTRIG7_WORKAROUND,           APCB_TOKEN_UID_XTRIG7_WORKAROUND                  },
  { APCB_ID_CONFIG_DF_GMI_ENCRYPT,                  APCB_TOKEN_UID_DF_GMI_ENCRYPT                     },
  { APCB_ID_CONFIG_DF_XGMI_ENCRYPT,                 APCB_TOKEN_UID_DF_XGMI_ENCRYPT                    },
  { APCB_ID_CONFIG_DF_SAVE_RESTORE_MEM_ENCRYPT,     APCB_TOKEN_UID_DF_SAVE_RESTORE_MEM_ENCRYPT        },
  { APCB_ID_CONFIG_DF_SYS_STORAGE_AT_TOP_OF_MEM,    APCB_TOKEN_UID_DF_SYS_STORAGE_AT_TOP_OF_MEM       },
  { APCB_ID_CONFIG_DF_PROBE_FILTER_ENABLE,          APCB_TOKEN_UID_DF_PROBE_FILTER                    },
  { APCB_ID_CONFIG_DF_BOTTOMIO,                     APCB_TOKEN_UID_DF_BOTTOMIO                        },
  { APCB_ID_CONFIG_DF_MEM_INTERLEAVING,             APCB_TOKEN_UID_DF_MEM_INTERLEAVING                },
  { APCB_ID_CONFIG_DF_DRAM_INTERLEAVE_SIZE,         APCB_TOKEN_UID_DF_MEM_INTERLEAVING_SIZE           },
  { APCB_ID_CONFIG_DF_ENABLE_CHAN_INTLV_HASH,       APCB_TOKEN_UID_DF_CHAN_INTLV_HASH                 },
  { APCB_ID_CONFIG_DF_PCI_MMIO_SIZE,                APCB_TOKEN_UID_DF_PCI_MMIO_SIZE                   },
  { APCB_ID_CONFIG_DF_CAKE_CRC_THRESH_PERF_BOUNDS,  APCB_TOKEN_UID_DF_CAKE_CRC_THRESH_PERF_BOUNDS     },
  { APCB_ID_CONFIG_DF_MEM_CLEAR,                    APCB_TOKEN_UID_DF_MEM_CLEAR                       },
  { APCB_ID_CONFIG_DF3_XGMI2_LINK_CFG,              APCB_TOKEN_UID_DF3_XGMI2_LINK_CFG                 },
  { APCB_ID_CONFIG_DF_GMI2_REPLAY,                  APCB_TOKEN_UID_DF_GMI2_REPLAY                     },
  { APCB_ID_CONFIG_DF_DRAM_NPS,                     APCB_TOKEN_UID_DF_DRAM_NPS                        },
  { APCB_ID_CONFIG_DF_INVERT_DRAM_MAP,              APCB_TOKEN_UID_DF_INVERT_DRAM_MAP                 },
  { APCB_ID_CONFIG_MEMHOLEREMAPPING,                APCB_TOKEN_UID_MEMHOLEREMAPPING                   },
  { APCB_ID_CONFIG_LIMITMEMORYTOBELOW1TB,           APCB_TOKEN_UID_LIMITMEMORYTOBELOW1TB              },
  { APCB_ID_CONFIG_USERTIMINGMODE,                  APCB_TOKEN_UID_USERTIMINGMODE                     },
  { APCB_ID_CONFIG_MEMCLOCKVALUE,                   APCB_TOKEN_UID_MEMCLOCKVALUE                      },
  { APCB_ID_CONFIG_ENABLECHIPSELECTINTLV,           APCB_TOKEN_UID_ENABLECHIPSELECTINTLV              },
  { APCB_ID_CONFIG_ENABLEECCFEATURE,                APCB_TOKEN_UID_ENABLEECCFEATURE                   },
  { APCB_ID_CONFIG_ENABLEPOWERDOWN,                 APCB_TOKEN_UID_ENABLEPOWERDOWN                    },
  { APCB_ID_CONFIG_ENABLEPARITY,                    APCB_TOKEN_UID_ENABLEPARITY                       },
  { APCB_ID_CONFIG_ENABLEBANKSWIZZLE,               APCB_TOKEN_UID_ENABLEBANKSWIZZLE                  },
  { APCB_ID_CONFIG_ENABLEMEMCLR,                    APCB_TOKEN_UID_ENABLEMEMCLR                       },
  { APCB_ID_CONFIG_UMAMODE,                         APCB_TOKEN_UID_UMAMODE                            },
  { APCB_ID_CONFIG_UMASIZE,                         APCB_TOKEN_UID_UMASIZE                            },
  { APCB_ID_CONFIG_MEMRESTORECTL,                   APCB_TOKEN_UID_MEMRESTORECTL                      },
  { APCB_ID_CONFIG_ISCAPSULEMODE,                   APCB_TOKEN_UID_ISCAPSULEMODE                      },
  { APCB_ID_CONFIG_FORCETRAINMODE,                  APCB_TOKEN_UID_FORCETRAINMODE                     },
  { APCB_ID_CONFIG_DIMMTYPEUSEDINMIXEDCONFIG,       APCB_TOKEN_UID_DIMMTYPEUSEDINMIXEDCONFIG          },
  { APCB_ID_CONFIG_AMPENABLE,                       APCB_TOKEN_UID_AMPENABLE                          },
  { APCB_ID_CONFIG_DRAMDOUBLEREFRESHRATE,           APCB_TOKEN_UID_DRAMDOUBLEREFRESHRATE              },
  { APCB_ID_CONFIG_PMUTRAINMODE,                    APCB_TOKEN_UID_PMUTRAINMODE                       },
  { APCB_ID_CONFIG_ECCREDIRECTION,                  APCB_TOKEN_UID_ECCREDIRECTION                     },
  { APCB_ID_CONFIG_SCRUBDRAMRATE,                   APCB_TOKEN_UID_SCRUBDRAMRATE                      },
  { APCB_ID_CONFIG_SCRUBL2RATE,                     APCB_TOKEN_UID_SCRUBL2RATE                        },
  { APCB_ID_CONFIG_SCRUBL3RATE,                     APCB_TOKEN_UID_SCRUBL3RATE                        },
  { APCB_ID_CONFIG_SCRUBICRATE,                     APCB_TOKEN_UID_SCRUBICRATE                        },
  { APCB_ID_CONFIG_SCRUBDCRATE,                     APCB_TOKEN_UID_SCRUBDCRATE                        },
  { APCB_ID_CONFIG_ECCSYNCFLOOD,                    APCB_TOKEN_UID_ECCSYNCFLOOD                       },
  { APCB_ID_CONFIG_ECCSYMBOLSIZE,                   APCB_TOKEN_UID_ECCSYMBOLSIZE                      },
  { APCB_ID_CONFIG_DQSTRAININGCONTROL,              APCB_TOKEN_UID_DQSTRAININGCONTROL                 },
  { APCB_ID_CONFIG_UMAABOVE4G,                      APCB_TOKEN_UID_UMAABOVE4G                         },
  { APCB_ID_CONFIG_UMAALIGNMENT,                    APCB_TOKEN_UID_UMAALIGNMENT                       },
  { APCB_ID_CONFIG_MEMORYALLCLOCKSON,               APCB_TOKEN_UID_MEMORYALLCLOCKSON                  },
  { APCB_ID_CONFIG_MEMORYBUSFREQUENCYLIMIT,         APCB_TOKEN_UID_MEMORYBUSFREQUENCYLIMIT            },
  { APCB_ID_CONFIG_POWERDOWNMODE,                   APCB_TOKEN_UID_POWERDOWNMODE                      },
  { APCB_ID_CONFIG_IGNORESPDCHECKSUM,               APCB_TOKEN_UID_IGNORESPDCHECKSUM                  },
  { APCB_ID_CONFIG_MEMORYMODEUNGANGED,              APCB_TOKEN_UID_MEMORYMODEUNGANGED                 },
  { APCB_ID_CONFIG_MEMORYQUADRANKCAPABLE,           APCB_TOKEN_UID_MEMORYQUADRANKCAPABLE              },
  { APCB_ID_CONFIG_MEMORYRDIMMCAPABLE,              APCB_TOKEN_UID_MEMORYRDIMMCAPABLE                 },
  { APCB_ID_CONFIG_MEMORYLRDIMMCAPABLE,             APCB_TOKEN_UID_MEMORYLRDIMMCAPABLE                },
  { APCB_ID_CONFIG_MEMORYUDIMMCAPABLE,              APCB_TOKEN_UID_MEMORYUDIMMCAPABLE                 },
  { APCB_ID_CONFIG_MEMORYSODIMMCAPABLE,             APCB_TOKEN_UID_MEMORYSODIMMCAPABLE                },
  { APCB_ID_CONFIG_DRAMDOUBLEREFRESHRATEEN,         APCB_TOKEN_UID_DRAMDOUBLEREFRESHRATEEN            },
  { APCB_ID_CONFIG_DIMMTYPEDDDR4CAPABLE,            APCB_TOKEN_UID_DIMMTYPEDDDR4CAPABLE               },
  { APCB_ID_CONFIG_DIMMTYPEDDDR3CAPABLE,            APCB_TOKEN_UID_DIMMTYPEDDDR3CAPABLE               },
  { APCB_ID_CONFIG_DIMMTYPELPDDDR3CAPABLE,          APCB_TOKEN_UID_DIMMTYPELPDDDR3CAPABLE             },
  { APCB_ID_CONFIG_ENABLEZQRESET,                   APCB_TOKEN_UID_ENABLEZQRESET                      },
  { APCB_ID_CONFIG_ENABLEBANKGROUPSWAP,             APCB_TOKEN_UID_ENABLEBANKGROUPSWAP                },
  { APCB_ID_CONFIG_ODTSCMDTHROTEN,                  APCB_TOKEN_UID_ODTSCMDTHROTEN                     },
  { APCB_ID_CONFIG_SWCMDTHROTEN,                    APCB_TOKEN_UID_SWCMDTHROTEN                       },
  { APCB_ID_CONFIG_FORCEPWRDOWNTHROTEN,             APCB_TOKEN_UID_FORCEPWRDOWNTHROTEN                },
  { APCB_ID_CONFIG_ODTSCMDTHROTCYC,                 APCB_TOKEN_UID_ODTSCMDTHROTCYC                    },
  { APCB_ID_CONFIG_SWCMDTHROTCYC,                   APCB_TOKEN_UID_SWCMDTHROTCYC                      },
  { APCB_ID_CONFIG_DIMMSENSORCONF,                  APCB_TOKEN_UID_DIMMSENSORCONF                     },
  { APCB_ID_CONFIG_DIMMSENSORUPPER,                 APCB_TOKEN_UID_DIMMSENSORUPPER                    },
  { APCB_ID_CONFIG_DIMMSENSORLOWER,                 APCB_TOKEN_UID_DIMMSENSORLOWER                    },
  { APCB_ID_CONFIG_DIMMSENSORCRITICAL,              APCB_TOKEN_UID_DIMMSENSORCRITICAL                 },
  { APCB_ID_CONFIG_DIMMSENSORRESOLUTION,            APCB_TOKEN_UID_DIMMSENSORRESOLUTION               },
  { APCB_ID_CONFIG_AUTOREFFINEGRANMODE,             APCB_TOKEN_UID_AUTOREFFINEGRANMODE                },
  { APCB_ID_CONFIG_ENABLEMEMPSTATE,                 APCB_TOKEN_UID_ENABLEMEMPSTATE                    },
  { APCB_ID_CONFIG_SOLDERDOWNDRAM,                  APCB_TOKEN_UID_SOLDERDOWNDRAM                     },
  { APCB_ID_CONFIG_DDRROUTEBALANCEDTEE,             APCB_TOKEN_UID_DDRROUTEBALANCEDTEE                },
  { APCB_ID_CONFIG_MEM_MBIST_TEST_ENABLE,           APCB_TOKEN_UID_MEM_MBIST_TEST_ENABLE              },
  { APCB_ID_CONFIG_MEM_MBIST_SUBTEST_TYPE,          APCB_TOKEN_UID_MEM_MBIST_SUBTEST_TYPE             },
  { APCB_ID_CONFIG_MEM_MBIST_AGGRESOR_ON,           APCB_TOKEN_UID_MEM_MBIST_AGGRESOR_ON              },
  { APCB_ID_CONFIG_MEM_MBIST_HALT_ON_ERROR,         APCB_TOKEN_UID_MEM_MBIST_HALT_ON_ERROR            },
  { APCB_ID_CONFIG_MEM_CPU_VREF_RANGE,              APCB_TOKEN_UID_MEM_CPU_VREF_RANGE                 },
  { APCB_ID_CONFIG_MEM_DRAM_VREF_RANGE,             APCB_TOKEN_UID_MEM_DRAM_VREF_RANGE                },
  { APCB_ID_CONFIG_MEM_TSME_ENABLE,                 APCB_TOKEN_UID_MEM_TSME_ENABLE                    },
  { APCB_ID_CONFIG_MEM_NVDIMM_POWER_SOURCE,         APCB_TOKEN_UID_MEM_NVDIMM_POWER_SOURCE            },
  { APCB_ID_CONFIG_MEM_DATA_POISON,                 APCB_TOKEN_UID_MEM_DATA_POISON                    },
  { APCB_ID_CONFIG_MEM_DATA_SCRAMBLE,               APCB_TOKEN_UID_MEM_DATA_SCRAMBLE                  },
  { APCB_ID_CONFIG_BMC_SOCKET_NUMBER,               APCB_TOKEN_UID_BMC_SOCKET_NUMBER                  },
  { APCB_ID_CONFIG_BMC_START_LANE,                  APCB_TOKEN_UID_BMC_START_LANE                     },
  { APCB_ID_CONFIG_BMC_END_LANE,                    APCB_TOKEN_UID_BMC_END_LANE                       },
  { APCB_ID_CONFIG_BMC_DEVICE,                      APCB_TOKEN_UID_BMC_DEVICE                         },
  { APCB_ID_CONFIG_BMC_FUNCTION,                    APCB_TOKEN_UID_BMC_FUNCTION                       },
  { APCB_ID_CONFIG_FCH_CONSOLE_OUT_ENABLE,          APCB_TOKEN_UID_FCH_CONSOLE_OUT_ENABLE             },
  { APCB_ID_CONFIG_FCH_CONSOLE_OUT_SERIAL_PORT,     APCB_TOKEN_UID_FCH_CONSOLE_OUT_SERIAL_PORT        },
};

UINT16
mTranslateConfigTokenId (
  IN       UINT16             CommonId
  )
{
  UINT16 i;

  for (i = 0; i < sizeof (ConfigTokenPair) / sizeof (TOKEN_PAIR); i++) {
    if (CommonId == ConfigTokenPair[i].ApcbCommonId) {
      return ConfigTokenPair[i].ApcbToken;
    }
  }

  ASSERT (FALSE);

  return 0xFFFF;
}

