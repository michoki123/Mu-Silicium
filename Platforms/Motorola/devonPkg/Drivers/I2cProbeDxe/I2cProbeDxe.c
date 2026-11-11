/**
  I2cProbeDxe - sonda de SOLO LECTURA para el bus I2C del Moto G32 (devon).

  Usa el protocolo I2C de Qualcomm (mismo diseno que QcomPkg/Include/Protocol/EFII2C.h).
  Para cada instancia de bus 0..15 intenta leer los registros 0x00.. de tres chips conocidos:
    0x1A  SGM41513  (cargador, modo OTG)
    0x67  SGM7220   (controlador USB-C)
    0x66  BQ25970   (bomba de carga)
  y muestra el resultado por la consola de depuracion (pantalla). NO ESCRIBE nada.
**/

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>

#define QCOM_I2C_PROTOCOL_GUID \
  { 0xb27ae8b1, 0x3e10, 0x4d07, { 0xab, 0x5c, 0xeb, 0x9a, 0x6d, 0xc6, 0xfa, 0x8f } }

// Estructuras con anchos explicitos (los enum de Qualcomm son de 4 bytes).
typedef struct {
  UINT32  BusFrequency;          // kHz
  UINT32  SlaveAddress;
  UINT32  Mode;                  // 0 = I2C
  UINT32  SlaveMaxClockStretch;  // us
  UINT32  CoreConfiguration1;
  UINT32  CoreConfiguration2;
} QCOM_I2C_SLAVE_CONFIG;

typedef struct _QCOM_I2C_PROTOCOL QCOM_I2C_PROTOCOL;

typedef UINT32 (EFIAPI *QCOM_I2C_OPEN) (IN UINT32 Instance, OUT VOID **I2cHandle);

typedef UINT32 (EFIAPI *QCOM_I2C_READ) (
  IN  VOID                   *I2cHandle,
  IN  QCOM_I2C_SLAVE_CONFIG  *Config,
  IN  UINT16                 Offset,
  IN  UINT16                 OffsetLength,
  OUT UINT8                  *Buffer,
  IN  UINT16                 BufferLength,
  OUT UINT32                 *Read,
  IN  UINT32                 TimeoutMs
  );

typedef UINT32 (EFIAPI *QCOM_I2C_WRITE) (
  IN  VOID                   *I2cHandle,
  IN  QCOM_I2C_SLAVE_CONFIG  *Config,
  IN  UINT16                 Offset,
  IN  UINT16                 OffsetLength,
  IN  UINT8                  *Buffer,
  IN  UINT16                 BufferLength,
  OUT UINT32                 *Written,
  IN  UINT32                 TimeoutMs
  );

typedef UINT32 (EFIAPI *QCOM_I2C_TRANSFER) (
  IN  VOID   *I2cHandle,
  IN  VOID   *Config,
  IN  VOID   *Descriptor,
  IN  UINT16 NumDescriptors,
  IN  VOID   *Callback,
  IN  VOID   *Context OPTIONAL,
  IN  UINT32 DelayUs,
  OUT UINT32 *Transferred
  );

typedef UINT32 (EFIAPI *QCOM_I2C_CLOSE) (IN VOID *I2cHandle);

struct _QCOM_I2C_PROTOCOL {
  UINT64             Revision;
  QCOM_I2C_OPEN      Open;
  QCOM_I2C_READ      Read;
  QCOM_I2C_WRITE     Write;
  QCOM_I2C_TRANSFER  Transfer;
  QCOM_I2C_CLOSE     Close;
};

STATIC EFI_GUID  mQcomI2cGuid = QCOM_I2C_PROTOCOL_GUID;
STATIC BOOLEAN   mDone        = FALSE;

STATIC CONST UINT8  mAddrs[] = { 0x1A, 0x67, 0x66 };

STATIC
VOID
ProbeAll (
  IN QCOM_I2C_PROTOCOL  *I2c
  )
{
  UINT32                 Inst;
  UINTN                  A;
  UINT32                 St;
  UINT32                 Rd;
  VOID                   *H;
  UINT8                  Buf[12];
  QCOM_I2C_SLAVE_CONFIG  Cfg;

  DEBUG ((EFI_D_ERROR, "I2cProbe: revision 0x%llx (SOLO LECTURA)\n", I2c->Revision));

  for (Inst = 0; Inst < 16; Inst++) {
    H  = NULL;
    St = I2c->Open (Inst, &H);
    if ((St != 0) || (H == NULL)) {
      DEBUG ((EFI_D_ERROR, "I2C%u: open err=%u\n", Inst, St));
      continue;
    }

    for (A = 0; A < sizeof (mAddrs); A++) {
      ZeroMem (Buf, sizeof (Buf));
      Rd = 0;
      ZeroMem (&Cfg, sizeof (Cfg));
      Cfg.BusFrequency         = 400;
      Cfg.SlaveAddress         = mAddrs[A];
      Cfg.Mode                 = 0;
      Cfg.SlaveMaxClockStretch = 500;

      St = I2c->Read (H, &Cfg, 0x00, 1, Buf, (UINT16)sizeof (Buf), &Rd, 100);
      if (St == 0) {
        DEBUG ((
          EFI_D_ERROR,
          "I2C%u @%02x OK n=%u: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
          Inst, mAddrs[A], Rd,
          Buf[0], Buf[1], Buf[2], Buf[3], Buf[4], Buf[5], Buf[6], Buf[7], Buf[8], Buf[9], Buf[10], Buf[11]
          ));
      } else {
        DEBUG ((EFI_D_ERROR, "I2C%u @%02x err=%u\n", Inst, mAddrs[A], St));
      }
    }

    I2c->Close (H);
  }

  DEBUG ((EFI_D_ERROR, "I2cProbe: fin. Pausa de 30 s para leer la pantalla...\n"));
  gBS->Stall (30 * 1000 * 1000);
}

STATIC
VOID
EFIAPI
OnI2cInstalled (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  QCOM_I2C_PROTOCOL  *I2c;

  if (mDone) {
    return;
  }

  I2c = NULL;
  if (EFI_ERROR (gBS->LocateProtocol (&mQcomI2cGuid, NULL, (VOID **)&I2c)) || (I2c == NULL)) {
    return;
  }

  mDone = TRUE;
  ProbeAll (I2c);
}

EFI_STATUS
EFIAPI
I2cProbeEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_EVENT  Ev;
  VOID       *Reg;

  Ev  = NULL;
  Reg = NULL;

  if (!EFI_ERROR (gBS->CreateEvent (EVT_NOTIFY_SIGNAL, TPL_CALLBACK, OnI2cInstalled, NULL, &Ev))) {
    gBS->RegisterProtocolNotify (&mQcomI2cGuid, Ev, &Reg);
  }

  // Por si el protocolo ya estaba instalado cuando cargo este driver.
  OnI2cInstalled (NULL, NULL);
  return EFI_SUCCESS;
}
