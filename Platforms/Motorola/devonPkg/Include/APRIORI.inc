APRIORI DXE {
  INF MdeModulePkg/Core/Dxe/DxeMain.inf
  INF MdeModulePkg/Universal/PCD/Dxe/Pcd.inf

!if $(SWAP_HW) == 1
  INF devonPkg/Binaries/EnvDxe/EnvDxe.inf
!else
  INF Binaries/spes/QcomPkg/Drivers/EnvDxe/EnvDxe.inf
!endif

  INF MdeModulePkg/Universal/ReportStatusCodeRouter/RuntimeDxe/ReportStatusCodeRouterRuntimeDxe.inf
  INF MdeModulePkg/Universal/StatusCodeHandler/RuntimeDxe/StatusCodeHandlerRuntimeDxe.inf
  INF MdeModulePkg/Core/RuntimeDxe/RuntimeDxe.inf
  INF ArmPkg/Drivers/CpuDxe/CpuDxe.inf
  INF ArmPkg/Drivers/ArmGicDxe/ArmGicDxe.inf
  INF EmbeddedPkg/MetronomeDxe/MetronomeDxe.inf
  INF ArmPkg/Drivers/TimerDxe/TimerDxe.inf

!if $(SWAP_HW) == 1
  INF devonPkg/Binaries/SmemDxe/SmemDxe.inf
!else
  INF Binaries/spes/QcomPkg/Drivers/SmemDxe/SmemDxe.inf
!endif
!if $(SWAP_HW) == 1
  INF devonPkg/Binaries/DALSys/DALSys.inf
!else
  INF Binaries/spes/QcomPkg/Drivers/DALSYSDxe/DALSYSDxe.inf
!endif
!if $(SWAP_HW) == 1
  INF devonPkg/Binaries/HWIODxeDriver/HWIODxeDriver.inf
!else
  INF Binaries/spes/QcomPkg/Drivers/HWIODxe/HWIODxe.inf
!endif
!if $(SWAP_HW) == 1
  INF devonPkg/Binaries/ChipInfo/ChipInfo.inf
!else
  INF Binaries/spes/QcomPkg/Drivers/ChipInfoDxe/ChipInfoDxe.inf
!endif
!if $(SWAP_HW) == 1
  INF devonPkg/Binaries/PlatformInfoDxeDriver/PlatformInfoDxeDriver.inf
!else
  INF Binaries/spes/QcomPkg/Drivers/PlatformInfoDxe/PlatformInfoDxe.inf
!endif
!if $(SWAP_HW) == 1
  INF devonPkg/Binaries/HALIOMMU/HALIOMMU.inf
!else
  INF Binaries/spes/QcomPkg/Drivers/HALIOMMUDxe/HALIOMMUDxe.inf
!endif
!if $(SWAP_HW) == 1
  INF devonPkg/Binaries/GlinkDxe/GlinkDxe.inf
!else
  INF Binaries/spes/QcomPkg/Drivers/GLinkDxe/GLinkDxe.inf
!endif
!if $(SWAP_HW) == 1
  INF devonPkg/Binaries/ULogDxe/ULogDxe.inf
!else
  INF Binaries/spes/QcomPkg/Drivers/ULogDxe/ULogDxe.inf
!endif
!if $(SWAP_HW) == 1
  INF devonPkg/Binaries/NpaDxe/NpaDxe.inf
!else
  INF Binaries/spes/QcomPkg/Drivers/NpaDxe/NpaDxe.inf
!endif
!if $(SWAP_HW) == 1
  INF devonPkg/Binaries/VcsDxe/VcsDxe.inf
!else
  INF Binaries/spes/QcomPkg/Drivers/VcsDxe/VcsDxe.inf
!endif
!if $(SWAP_CLOCK) == 1
  INF devonPkg/Binaries/ClockDxe/ClockDxe.inf
!endif
!if $(SWAP_CLOCK) == 2
  INF devonPkg/Binaries/ClockDxeOrig/ClockDxe.inf
!endif
!if $(SWAP_CLOCK) == 0
  INF Binaries/spes/QcomPkg/Drivers/ClockDxe/ClockDxe.inf
!endif
!if $(SWAP_TZ) == 1
  INF devonPkg/Binaries/ShmBridgeDxe/ShmBridgeDxe.inf
!else
  INF Binaries/spes/QcomPkg/Drivers/ShmBridgeDxe/ShmBridgeDxeLA.inf
!endif
!if $(SWAP_TZ) == 1
  INF devonPkg/Binaries/ScmDxe/ScmDxe.inf
!else
  INF Binaries/spes/QcomPkg/Drivers/TzDxe/ScmDxeLA.inf
!endif

  # Use Binary Version of these two Drivers to Make UFS work correct.
  INF Binaries/spes/MdeModulePkg/Universal/Disk/DiskIoDxe/DiskIoDxe.inf
  INF Binaries/spes/MdeModulePkg/Universal/Disk/PartitionDxe/PartitionDxe.inf

  INF MdeModulePkg/Universal/Disk/UnicodeCollation/EnglishDxe/EnglishDxe.inf

!if $(DROP_STORAGE) == 0
  INF Binaries/spes/QcomPkg/Drivers/SdccDxe/SdccDxe.inf
!endif
!if $(DROP_STORAGE) == 0
  INF Binaries/spes/QcomPkg/Drivers/UFSDxe/UFSDxe.inf
!endif

  INF FatPkg/EnhancedFatDxe/Fat.inf

!if $(SWAP_TZ) == 1
  INF devonPkg/Binaries/TzDxe/TzDxe.inf
!else
  INF Binaries/spes/QcomPkg/Drivers/TzDxe/TzDxeLA.inf
!endif

  INF MdeModulePkg/Universal/Variable/RuntimeDxe/VariableRuntimeDxe.inf

!if $(SWAP_HW) == 1
  INF devonPkg/Binaries/DALTLMM/DALTLMM.inf
!else
  INF Binaries/spes/QcomPkg/Drivers/TLMMDxe/TLMMDxe.inf
!endif
!if $(SWAP_HW) == 1
  INF devonPkg/Binaries/SPMI/SPMI.inf
!else
  INF Binaries/spes/QcomPkg/Drivers/SPMIDxe/SPMIDxe.inf
!endif

  INF MdeModulePkg/Universal/ResetSystemRuntimeDxe/ResetSystemRuntimeDxe.inf

!if $(SWAP_PMIC) == 1
  INF devonPkg/Binaries/PmicDxe/PmicDxe.inf
!endif
!if $(SWAP_PMIC) == 0
  INF Binaries/spes/QcomPkg/Drivers/PmicDxe/PmicDxeLa.inf
!endif
  INF Binaries/spes/QcomPkg/Drivers/PILDxe/PILDxe.inf

  INF MdeModulePkg/Universal/WatchdogTimerDxe/WatchdogTimer.inf
  INF MdeModulePkg/Universal/SecurityStubDxe/SecurityStubDxe.inf
  INF EmbeddedPkg/EmbeddedMonotonicCounter/EmbeddedMonotonicCounter.inf
  INF EmbeddedPkg/RealTimeClockRuntimeDxe/RealTimeClockRuntimeDxe.inf
  INF MdeModulePkg/Universal/PrintDxe/PrintDxe.inf
  INF MdeModulePkg/Universal/DevicePathDxe/DevicePathDxe.inf
  INF MdeModulePkg/Universal/CapsuleRuntimeDxe/CapsuleRuntimeDxe.inf
  INF MdeModulePkg/Universal/HiiDatabaseDxe/HiiDatabaseDxe.inf
  INF MdeModulePkg/Universal/BdsDxe/BdsDxe.inf

!if $(SWAP_HW) == 1
  INF devonPkg/Binaries/I2C/I2C.inf
!else
  INF Binaries/spes/QcomPkg/Drivers/I2CDxe/I2CDxe.inf
!endif
!if $(SWAP_HW) == 1
  INF devonPkg/Binaries/AdcDxe/AdcDxe.inf
!else
  INF Binaries/spes/QcomPkg/Drivers/AdcDxe/AdcDxe.inf
!endif
!if $(DROP_USB) == 0
  INF Binaries/spes/QcomPkg/Drivers/UsbPwrCtrlDxe/UsbPwrCtrlDxe.inf
!endif
!if $(DROP_USB) == 0
  INF Binaries/spes/QcomPkg/Drivers/UsbfnDwc3Dxe/UsbfnDwc3Dxe.inf
!endif
!if $(ADD_XHCI) == 1
  INF Binaries/generic/QcomPkg/Drivers/XhciPciEmulationDxe/XhciPciEmulationDxe.inf
  INF Binaries/generic/QcomPkg/Drivers/XhciDxe/XhciDxe.inf
!endif

!if $(DROP_USB) == 0
  INF MdeModulePkg/Bus/Usb/UsbBusDxe/UsbBusDxe.inf
!endif
!if $(DROP_USB) == 0
  INF MdeModulePkg/Bus/Usb/UsbKbDxe/UsbKbDxe.inf
!endif
!if $(DROP_USB) == 0
  INF MdeModulePkg/Bus/Usb/UsbMassStorageDxe/UsbMassStorageDxe.inf
!endif

!if $(DROP_USB) == 0
  INF Binaries/spes/QcomPkg/Drivers/UsbMsdDxe/UsbMsdDxe.inf
!endif
!if $(DROP_USB) == 0
  INF Binaries/spes/QcomPkg/Drivers/UsbDeviceDxe/UsbDeviceDxe.inf
!endif
!if $(DROP_USB) == 0
  INF Binaries/spes/QcomPkg/Drivers/UsbConfigDxe/UsbConfigDxe.inf
!endif
!if $(DROP_BUTTONS) == 0
  INF Binaries/spes/QcomPkg/Drivers/ButtonsDxe/ButtonsDxe.inf
!endif
!if $(SWAP_HW) == 1
  INF devonPkg/Binaries/TsensDxe/TsensDxe.inf
!else
  INF Binaries/spes/QcomPkg/Drivers/TsensDxe/TsensDxe.inf
!endif

!if $(USE_CUSTOM_DISPLAY_DRIVER) == 1
  INF Binaries/spes/QcomPkg/Drivers/DisplayDxe/DisplayDxe.inf
!else
  INF SiliciumPkg/Drivers/SimpleFbDxe/SimpleFbDxe.inf
!endif

!if $(SWAP_HW) == 1
  INF devonPkg/Binaries/GpiDxe/GpiDxe.inf
!else
  INF Binaries/spes/QcomPkg/Drivers/GpiDxe/GpiDxe.inf
!endif
  INF Binaries/spes/QcomPkg/Drivers/HashDxe/HashDxe.inf
  INF Binaries/spes/QcomPkg/Drivers/CipherDxe/CipherDxe.inf
  INF Binaries/spes/QcomPkg/Drivers/MacDxe/MacDxe.inf
  INF Binaries/spes/QcomPkg/SocPkg/DivarPkg/Drivers/RNGDxe/RngDxe.inf
!if $(SWAP_HW) == 1
  INF devonPkg/Binaries/DDRInfoDxe/DDRInfoDxe.inf
!else
  INF Binaries/spes/QcomPkg/Drivers/DDRInfoDxe/DDRInfoDxe.inf
!endif

  INF EmbeddedPkg/SimpleTextInOutSerial/SimpleTextInOutSerial.inf
  INF MdeModulePkg/Universal/Console/ConPlatformDxe/ConPlatformDxe.inf
  INF MdeModulePkg/Universal/Console/ConSplitterDxe/ConSplitterDxe.inf
  INF MdeModulePkg/Universal/Console/GraphicsConsoleDxe/GraphicsConsoleDxe.inf
}
