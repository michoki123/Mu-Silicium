/**
  I2cProbeDxe v2 - sonda I2C del Moto G32 (devon). Solo lectura salvo que se compile con -DENABLE_OTG=1.

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

#define I2C_INSTANCE   2      // medido con la v1: aqui responden los tres chips
#define ADDR_CHARGER   0x1A   // SGM41513
#define ADDR_TYPEC     0x67   // SGM7220

STATIC QCOM_I2C_PROTOCOL  *mI2c = NULL;
STATIC VOID               *mH   = NULL;

STATIC
UINT32
RdN (
  IN  UINT8   Addr,
  IN  UINT16  Reg,
  OUT UINT8   *Buf,
  IN  UINT16  Len
  )
{
  QCOM_I2C_SLAVE_CONFIG  Cfg;
  UINT32                 Rd;

  ZeroMem (&Cfg, sizeof (Cfg));
  Cfg.BusFrequency         = 400;
  Cfg.SlaveAddress         = Addr;
  Cfg.SlaveMaxClockStretch = 500;
  Rd = 0;
  return mI2c->Read (mH, &Cfg, Reg, 1, Buf, Len, &Rd, 100);
}

#ifdef ENABLE_OTG
STATIC BOOLEAN  mOtgOn = FALSE;

STATIC
UINT32
Wr1 (
  IN UINT8   Addr,
  IN UINT16  Reg,
  IN UINT8   Val
  )
{
  QCOM_I2C_SLAVE_CONFIG  Cfg;
  UINT32                 Wr;
  UINT8                  V;

  ZeroMem (&Cfg, sizeof (Cfg));
  Cfg.BusFrequency         = 400;
  Cfg.SlaveAddress         = Addr;
  Cfg.SlaveMaxClockStretch = 500;
  Wr = 0;
  V  = Val;
  return mI2c->Write (mH, &Cfg, Reg, 1, &V, 1, &Wr, 100);
}

// Pone OTG_CONFIG=1 y CHG_CONFIG=0 en REG01 (mapa tipo BQ25601, bits 5 y 4) y
// reinicia el watchdog (bit 6). Lee antes y deja el resto de bits como estaban.
STATIC
VOID
OtgAssert (
  IN BOOLEAN  Verbose
  )
{
  UINT8   R1;
  UINT8   New;
  UINT32  St;

  St = RdN (ADDR_CHARGER, 1, &R1, 1);
  if (St != 0) {
    DEBUG ((EFI_D_ERROR, "OTG: lectura REG01 err=%u\n", St));
    return;
  }

  New = (UINT8)(((R1 | 0x20) & ~0x10 & ~0x80) | 0x40);
  St  = Wr1 (ADDR_CHARGER, 1, New);
  if (Verbose) {
    DEBUG ((EFI_D_ERROR, "OTG: REG01 %02x -> %02x (write err=%u)\n", R1, New, St));
  }
}
#endif

STATIC
VOID
DumpChg (
  IN CONST CHAR8  *Tag
  )
{
  UINT8   B[12];
  UINT32  St;

  ZeroMem (B, sizeof (B));
  St = RdN (ADDR_CHARGER, 0, B, 12);
  if (St != 0) {
    DEBUG ((EFI_D_ERROR, "%a SGM41513 err=%u\n", Tag, St));
    return;
  }

  DEBUG ((
    EFI_D_ERROR,
    "%a SGM41513: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
    Tag, B[0], B[1], B[2], B[3], B[4], B[5], B[6], B[7], B[8], B[9], B[10], B[11]
    ));
  DEBUG ((
    EFI_D_ERROR,
    "%a   (supuesto BQ25601) OTG=%u CHG=%u | VBUS_STAT=%u CHRG_STAT=%u PG=%u | FALLAS=%02x\n",
    Tag, (B[1] >> 5) & 1, (B[1] >> 4) & 1, (B[8] >> 5) & 7, (B[8] >> 3) & 3, (B[8] >> 2) & 1, B[9]
    ));
}

STATIC
VOID
DumpTypeC (
  IN CONST CHAR8  *Tag
  )
{
  UINT8   B[12];
  UINT32  St;
  UINTN   R;
  UINT32  Err;

  ZeroMem (B, sizeof (B));
  Err = 0;
  // El SGM7220 no admite lecturas de varios bytes: un registro por transaccion.
  for (R = 0; R < sizeof (B); R++) {
    St = RdN (ADDR_TYPEC, (UINT16)R, &B[R], 1);
    if (St != 0) {
      Err = St;
    }
  }

  DEBUG ((
    EFI_D_ERROR,
    "%a SGM7220: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x (err=%u)\n",
    Tag, B[0], B[1], B[2], B[3], B[4], B[5], B[6], B[7], B[8], B[9], B[10], B[11], Err
    ));
  DEBUG ((
    EFI_D_ERROR,
    "%a   (supuesto TUSB320) ESTADO_CONEXION=%u MODO=%u\n",
    Tag, (B[9] >> 6) & 3, (B[10] >> 4) & 3
    ));
}

STATIC
VOID
WaitSeconds (
  IN UINTN  Seconds
  )
{
  UINTN  S;

  for (S = 0; S < Seconds; S++) {
    gBS->Stall (1000 * 1000);
#ifdef ENABLE_OTG
    if (mOtgOn && ((S % 3) == 2)) {
      OtgAssert (FALSE);
    }
#endif
  }
}

STATIC
VOID
ProbeAll (
  VOID
  )
{
  UINT32  St;

  DEBUG ((EFI_D_ERROR, "I2cProbe v2: revision 0x%llx\n", mI2c->Revision));
#ifdef ENABLE_OTG
  DEBUG ((EFI_D_ERROR, "I2cProbe: modo CON OTG (escribe REG01 del SGM41513)\n"));
#else
  DEBUG ((EFI_D_ERROR, "I2cProbe: modo SOLO LECTURA\n"));
#endif

  mH = NULL;
  St = mI2c->Open (I2C_INSTANCE, &mH);
  if ((St != 0) || (mH == NULL)) {
    DEBUG ((EFI_D_ERROR, "I2C%u: open err=%u\n", I2C_INSTANCE, St));
    return;
  }

  DumpChg ("A");
  DumpTypeC ("A");

  DEBUG ((EFI_D_ERROR, "\n>>> TIENES 20 s: DESCONECTA el cable de la PC y CONECTA el adaptador OTG con el teclado o pendrive <<<\n\n"));
  WaitSeconds (20);

  DumpChg ("B");
  DumpTypeC ("B");

#ifdef ENABLE_OTG
  OtgAssert (TRUE);
  mOtgOn = TRUE;
  gBS->Stall (300 * 1000);
  DumpChg ("OTG");
  DumpTypeC ("OTG");
#endif

  DEBUG ((EFI_D_ERROR, "I2cProbe: fin. Pausa de 30 s para leer la pantalla...\n"));
  WaitSeconds (30);
  // mH queda abierto a proposito.
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
  mI2c  = I2c;
  ProbeAll ();
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
