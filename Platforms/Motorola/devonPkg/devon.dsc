##
#  Copyright (c) 2011 - 2022, ARM Limited. All rights reserved.
#  Copyright (c) 2014, Linaro Limited. All rights reserved.
#  Copyright (c) 2015 - 2020, Intel Corporation. All rights reserved.
#  Copyright (c) 2018, Bingxing Wang. All rights reserved.
#  Copyright (c) Microsoft Corporation.
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
##

################################################################################
#
# Defines Section - statements that will be processed to create a Makefile.
#
################################################################################
[Defines]
  PLATFORM_NAME                  = devon
  PLATFORM_GUID                  = 725802F3-C9CC-4596-9083-54B2B438866D
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010005
  OUTPUT_DIRECTORY               = Build/devonPkg
  SUPPORTED_ARCHITECTURES        = AARCH64
  BUILD_TARGETS                  = RELEASE|DEBUG
  SKUID_IDENTIFIER               = DEFAULT
  FLASH_DEFINITION               = devonPkg/devon.fdf
  USE_CUSTOM_DISPLAY_DRIVER      = 0
  SWAP_PMIC                      = 0
  SWAP_CLOCK                     = 0
  SWAP_HW                        = 0
  SWAP_TZ                        = 0
  DROP_STORAGE                   = 0
  DROP_USB                       = 0
  DROP_BUTTONS                   = 0
  ADD_WDOG                       = 0
  ADD_MOT                        = 0
  ADD_DEVCHG                     = 0
  I2C_MODE                       = 0
  ADD_XHCI                       = 0
  PAUSE_LOG                      = 0
  MOT_EARLY                      = 0

  #
  # 0 = SM6225
  # 1 = SM6225-AD
  #
  SOC_TYPE                       = 0

!include DivarPkg/DivarPkg.dsc.inc

[PcdsFixedAtBuild]
  #
  # DDR Memory
  #
  gArmTokenSpaceGuid.PcdSystemMemoryBase|0x40000000

  #
  # UEFI Stack
  #
  gArmPlatformTokenSpaceGuid.PcdCPUCoresStackBase|0x5FF90000
  gArmPlatformTokenSpaceGuid.PcdCPUCorePrimaryStackSize|0x40000

  #
  # SMBIOS
  #
  gSiliciumPkgTokenSpaceGuid.PcdSmbiosSystemManufacturer|"Motorola"
  gSiliciumPkgTokenSpaceGuid.PcdSmbiosSystemModel|"Moto G32"
  gSiliciumPkgTokenSpaceGuid.PcdSmbiosSystemRetailModel|"devon"
  gSiliciumPkgTokenSpaceGuid.PcdSmbiosSystemRetailSku|"Moto_G32_devon"
  gSiliciumPkgTokenSpaceGuid.PcdSmbiosSystemBoardModel|"Moto G32"

  #
  # Simple Frame Buffer
  #
  gSiliciumPkgTokenSpaceGuid.PcdFrameBufferWidth|1080
  gSiliciumPkgTokenSpaceGuid.PcdFrameBufferHeight|2400
  gSiliciumPkgTokenSpaceGuid.PcdFrameBufferColorDepth|32

  #
  # Platform PEI
  #
  gQcomPkgTokenSpaceGuid.PcdPlatformType|"LA"

[LibraryClasses]
  #
  # Memory Libraries
  #
  MemoryMapLib|devonPkg/Library/MemoryMapLib/MemoryMapLib.inf

  #
  # QCOM Libraries
  #
  ConfigurationMapLib|devonPkg/Library/ConfigurationMapLib/ConfigurationMapLib.inf

[Components]
  #
  # ACPI Tables
  #
  spes/AcpiTables.inf

!if $(I2C_MODE) == 1
  devonPkg/Drivers/I2cProbeDxe/I2cProbeDxe.inf
!endif
!if $(I2C_MODE) == 2
  devonPkg/Drivers/I2cProbeDxe/I2cProbeDxe.inf {
    <BuildOptions>
      *_*_*_CC_FLAGS = -DENABLE_OTG=1
  }
!endif

!if $(PAUSE_LOG) == 1
[BuildOptions]
  *_*_*_CC_FLAGS = -DPAUSE_ON_ERROR
!endif
