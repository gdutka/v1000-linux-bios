## @file
#  Describes Fixes for Build Errors when Upgrading
#
#******************************************************************************
#* Copyright (c) 2019, Insyde Software Corp. All Rights Reserved.
#*
#* You may not reproduce, distribute, publish, display, perform, modify, adapt,
#* transmit, broadcast, present, recite, release, license or otherwise exploit
#* any part of this publication in any form, by any means, without the prior
#* written permission of Insyde Software Corporation.
#*
#******************************************************************************
Tag#: 05.33.28
Sympton: Build tool show "error B5000: MODULE_TYPE is not given"
RelatedFiles: INF and source files of EDK I style driver.
Solution: EDK I style drivers are not supported anymore. Please convert them to EDK II style.
          For more EDKII style related information. Please visit
          https://github.com/tianocore/tianocore.github.io/wiki/EDK-II-Specifications
          