#ifndef _MFRC522_HPP
#define _MFRC522_HPP

#include <cstdint>
#include <array>
#include "../Controllers/Common/Compiler/Compiler.h"

namespace device{

template<typename interface, typename pinNSS, typename pinIRQ = void>
class MFRC522{

  enum registers{
    /*! @brief starts and stops command execution*/
    CommandReg = 0x02,
    /*! @brief enable and disable interrupt request control bits*/
    ComIEnReg = 0x04,
    /*! @brief enable and disable interrupt request control bits*/
    DivIEnReg = 0x06,
    /*! @brief interrupt request bits*/
    ComIrqReg = 0x08,
    /*! @brief interrupt request bits*/
    DivIrqReg = 0x0A,
    /*! @brief error bits showing the error status of the last command executed*/
    ErrorReg = 0x0C,
    /*! @brief communication status bits*/
    Status1Reg = 0x0E,
    /*! @brief receiver and transmitter status bits*/
    Status2Reg = 0x10,
    /*! @brief input and output of 64 byte FIFO buffer*/
    FIFODataReg = 0x12,
    /*! @brief number of bytes stored in the FIFO buffer*/
    FIFOLevelReg = 0x14,
    /*! @brief level for FIFO underflow and overflow warning*/
    WaterLevelReg = 0x16,
    /*! @brief miscellaneous control registers*/
    ControlReg = 0x18,
    /*! @brief adjustments for bit-oriented frames*/
    BitFramingReg = 0x1A,
    /*! @brief bit position of the first bit-collision detected on the RF interface*/
    CollReg = 0x1C,
    /*! @brief defines general modes for transmitting and receiving*/
    ModeReg = 0x22,
    /*! @brief defines transmission data rate and framing*/
    TxModeReg = 0x24,
    /*! @brief defines reception data rate and framing*/
    RxModeReg = 0x26,
    /*! @brief controls the logical behavior of the antenna driver pins TX1 and TX2*/
    TxControlReg = 0x28,
    /*! @brief controls the setting of the transmission modulation*/
    TxASKReg = 0x2A,
    /*! @brief selects the internal sources for the antenna driver*/
    TxSelReg = 0x2C,
    /*! @brief selects internal receiver settings*/
    RxSelReg = 0x2E,
    /*! @brief selects thresholds for the bit decoder*/
    RxThresholdReg = 0x30,
    /*! @brief defines demodulator settings*/
    DemodReg = 0x32,
    /*! @brief controls some MIFARE communication transmit parameters*/
    MfTxReg = 0x38,
    /*! @brief controls some MIFARE communication receive parameters*/
    MfRxReg = 0x3A,
    /*! @brief selects the speed of the serial UART interface*/
    SerialSpeedReg = 0x3E,
    /*! @brief shows the MSB value of the CRC calculation*/
    CRCResultRegH = 0x42,
    /*! @brief shows the LSB value of the CRC calculation*/
    CRCResultRegL = 0x44,
    /*! @brief controls the ModWidth setting*/
    ModWidthReg = 0x48,
    /*! @brief configures the receiver gain*/
    RFCfgReg = 0x4C,
    /*! @brief selects the conductance of the antenna driver pins TX1 and TX2 for modulation*/
    GsNReg = 0x4E,
    /*! @brief defines the conductance of the p-driver output during periods of no modulation*/
    CWGsPReg = 0x50,
    /*! @brief  defines the conductance of the p-driver output during periods of modulation*/
    ModGsPReg = 0x52,
    /*! @brief defines settings for the internal timer*/
    TModeReg = 0x54,
    /*! @brief defines settings for the internal timer*/
    TPrescalerReg = 0x56,
    /*! @brief defines the 16-bit timer reload value (High)*/
    TReloadRegH = 0x58,
    /*! @brief defines the 16-bit timer reload value (Low)*/
    TReloadRegL = 0x5A,
    /*! @brief shows the 16-bit timer value*/
    TCounterValRegH = 0x5C,
    /*! @brief shows the 16-bit timer value*/
    TCounterValRegL = 0x5E
  };

struct command{

  enum chip{
    /*! @brief no action, cancels current command execution*/
    Idle = 0,
    /*! @brief stores 25 bytes into the internal buffer*/
    Mem,
    /*! @brief generates a 10-byte random ID number*/
    GenerateRandomID,
    /*! @brief activates the CRC coprocessor or performs a self test*/
    CalcCRC,
    /*! @brief transmits data from the FIFO buffer*/
    Transmit,
    /*! @brief no action, cancels current command execution*/
    NoCmdChange = 7,
    /*! @brief activates the receiver circuits*/
    Receive = 8,
    /*! @brief transmits data from FIFO buffer to antenna and automatically
               activates the receiver after transmission*/
    Transeive = 0x0C,
    /*! @brief performs the MIFARE standard authentication as a reader*/
    MFAuthent = 0x0E,
    /*! @brief resets the MFRC522*/
    SoftReset = 0x0F
  };

  enum picc{
    /*! @brief REQuest command, Type A. Invites PICCs in state IDLE to go to READY and prepare for anticollision or selection. 7 bit frame*/
    REQA      = 0x26,
    /*! @brief Wake-UP command, Type A. Invites PICCs in state IDLE and HALT to go to READY(*) and prepare for anticollision or selection. 7 bit frame*/
    WUPA      = 0x52,
    /*! @brief Cascade Tag. Not really a command, but used during anti collision*/
    CT        = 0x88,
    /*! @brief Anti collision/Select, Cascade Level 1*/
    SEL_CL1    = 0x93,
    /*! @brief Anti collision/Select, Cascade Level 2*/
    SEL_CL2    = 0x95,
    /*! @brief Anti collision/Select, Cascade Level 3*/
    SEL_CL3    = 0x97,
    /*! @brief HaLT command, Type A. Instructs an ACTIVE PICC to go to state HALT*/
    HLTA  = 0x50,
    /*! @brief Perform authentication with Key A*/
    MF_AUTH_KEY_A  = 0x60,
    /*! @brief Perform authentication with Key B*/
    MF_AUTH_KEY_B  = 0x61,
    /*! @brief Reads one 16 byte block from the authenticated sector of the PICC. Also used for MIFARE Ultralight*/
    MF_READ = 0x30,
    /*! @brief Writes one 16 byte block to the authenticated sector of the PICC. Called "COMPATIBILITY WRITE" for MIFARE Ultralight*/
    MF_WRITE  = 0xA0,
    /*! @brief Decrements the contents of a block and stores the result in the internal data register*/
    MF_DECREMENT  = 0xC0,
    /*! @brief Increments the contents of a block and stores the result in the internal data register*/
    MF_INCREMENT  = 0xC1,
    /*! @brief Reads the contents of a block into the internal data register*/
    MF_RESTORE  = 0xC2,
    /*! @brief Writes the contents of the internal data register to a block*/
    MF_TRANSFER  = 0xB0,
    /*! @brief Writes one 4 byte page to the PICC*/
    UL_WRITE  = 0xA2
  };

};


  static constexpr size_t sizeFIFO = 64;
  static inline size_t sizeUsedFIFO = 0;
  // BitFramingReg: used for transmission of bit oriented frames: defines the number of bits of the last byte that will be transmitted
  static inline uint8_t TxLastBits = 0;

  static inline auto command = command::chip::Idle;

public:
static inline size_t sizeToRead = 0;
  static inline std::array<uint8_t, sizeFIFO> bufferFIFO;

  static void Init(){
    static constexpr std::array<uint8_t, 14> arrayInit { 
      registers::CommandReg, command::SoftReset,
      registers::TPrescalerReg, 0xA9, // A9 f_timer=40kHz, ie a timer period of 25us.
      registers::TReloadRegH, 0x03, // 0x3E8 = 1000 => 25ms before timeout
      registers::TReloadRegL, 0xE8,
      registers::TxControlReg, 0x83, // Enable the antenna driver pins TX1 and TX2
      registers::TxASKReg, 0x40, // forces a 100 % ASK modulation
      registers::ModeReg, 0x3D}; //set the preset value for the CRC coprocessor for the CalcCRC command to 0x6363
    WriteData(arrayInit.data(), arrayInit.size(), 2);
  }

  static void CalculateCRC1(size_t size){
    static constexpr std::array<uint8_t, 14> arrayConfig{
      registers::CommandReg, command::chip::Idle, // Stop Command
      registers::ComIEnReg, 0x81,
      registers::DivIEnReg, 0x84, // Enable CRC Interrupt
      registers::ComIrqReg, 0x7F, // Clear interrupts
      registers::DivIrqReg, 0x7F, // Clear Interrupts
      registers::FIFOLevelReg, 0x80, // Flush fifo buffer
      registers::ControlReg, 0x40}; //the CalcCRC command is active and all data is processed
    static constexpr std::array<uint8_t, 2> arrayCommandCRC{registers::CommandReg, command::chip::CalcCRC};
    static constexpr std::array<uint8_t, 2> arrayReadCRC{registers::CRCResultRegL | 0x80, registers::CRCResultRegH | 0x80};

    static auto _size = size;

    WriteData(arrayConfig.data(), arrayConfig.size() - 2, 2);
    CallbackEndTransmission = [](){
      interface::Push(registers::FIFODataReg);
      WriteData(bufferFIFO.data(), _size, 0);
      CallbackEndTransmission = [](){
        CallbackEndTransmission = nullptr;
        WriteData(arrayCommandCRC.data(), arrayCommandCRC.size(), 2);
        pinIRQ::CallbackEvent = [](){
          pinIRQ::CallbackEvent = nullptr;
          interface::FlushRX();
          ReadData(arrayReadCRC.data(), arrayReadCRC.size());
          CallbackEndTransmission = [](){
            CallbackEndTransmission = nullptr;
            (void) interface::Read();
            bufferFIFO[2] = interface::Read();
            bufferFIFO[3] = interface::Read();
            if (CallbackCommandComplete){
              CallbackCommandComplete();
              CallbackCommandComplete = nullptr;
            }
          };
        };
      };
    };

  }

static void Cm_PICC(){
  static constexpr std::array<uint8_t, 10> config{
    registers::ComIEnReg, 0xA0, // Enable RxIEn
    registers::DivIEnReg, 0x80, // Enable CRC Interrupt
    registers::ComIrqReg, 0x7F, // Clear interrupts
    registers::DivIrqReg, 0x7F, // Clear Interrupts
    registers::FIFOLevelReg, 0x80}; // Flush fifo buffer
  static std::array<uint8_t, 4> commandSend{
    registers::CommandReg, command::chip::Transeive,
    registers::BitFramingReg, 0x80};
  commandSend[1] = command;
  commandSend[3] = uint8_t(0x80 | TxLastBits);

  static size_t commandSendSize = 0;
  commandSendSize = commandSend.size() - (command == command::chip::Transeive ? 0 : 2);

  WriteData(config.data(), config.size(), 2);
  CallbackEndTransmission = [](){
    interface::Push(registers::FIFODataReg);
    WriteData(bufferFIFO.data(), sizeUsedFIFO, 0);
    CallbackEndTransmission = [](){
      CallbackEndTransmission = nullptr;
      WriteData(commandSend.data(), commandSendSize, 2);
      pinIRQ::CallbackEvent = [](){
        pinIRQ::CallbackEvent = nullptr;
        if (CallbackInternalComplete) CallbackInternalComplete();
      };
    };
  };

}

static void SetPICCtoReady(){
  bufferFIFO[0] = command::picc::WUPA;
  sizeUsedFIFO = 1;
  TxLastBits = 7;
  sizeToRead = 2;
  command = command::chip::Transeive;
  CallbackInternalComplete = ReadFIFO;
  Cm_PICC();
}

static void GetUID(){
  bufferFIFO[0] = command::picc::SEL_CL1;
  bufferFIFO[1] = 0x20;
  sizeUsedFIFO = 2;
  TxLastBits = 0;
  sizeToRead = 5;
  command = command::chip::Transeive;
  CallbackInternalComplete = ReadFIFO;
  Cm_PICC();
}

static void Select(){
  bufferFIFO[0] = command::picc::SEL_CL1;
  bufferFIFO[1] = 0x70;
  // UID
  bufferFIFO[2] = 0x06;
  bufferFIFO[3] = 0x87;
  bufferFIFO[4] = 0xdb;
  bufferFIFO[5] = 0x48;
  // end of UID
  // BCC - BCC count check
  bufferFIFO[6] = 0x12;
  sizeUsedFIFO = 7;
  TxLastBits = 0;
  sizeToRead = 3;
  command = command::chip::Transeive;
  CallbackInternalComplete = ReadFIFO;
  CalculateCRC();
  Cm_PICC();
}

static void Authentication(uint8_t numberBlock){
  bufferFIFO[0] = command::picc::MF_AUTH_KEY_A;
  bufferFIFO[1] = numberBlock;
  for(size_t i = 0; i < 6; ++i)
    bufferFIFO[i + 2] = 0xFF;
  bufferFIFO[8] = 0x06;
  bufferFIFO[9] = 0x87;
  bufferFIFO[10] = 0xdb;
  bufferFIFO[11] = 0x48;
  sizeUsedFIFO = 12;
  command = command::chip::MFAuthent;
  Cm_PICC();
}

static void AuthenticationHack(uint8_t number){
  bufferFIFO[0] = command::picc::MF_AUTH_KEY_A;
  bufferFIFO[1] = 0;
  
  bufferFIFO[2] = 0xFF;
  bufferFIFO[3] = 0xFF;
  bufferFIFO[4] = 0xFF;
  bufferFIFO[5] = 0xFF;
  bufferFIFO[6] = 0xFF;
  bufferFIFO[7] = number;

  bufferFIFO[8] = 0x06;
  bufferFIFO[9] = 0x87;
  bufferFIFO[10] = 0xdb;
  bufferFIFO[11] = 0x48;
  sizeUsedFIFO = 12;
  command = command::chip::MFAuthent;
  Cm_PICC();
}

static void Read(uint8_t numberBlock){
  bufferFIFO[0] = command::picc::MF_READ;
  bufferFIFO[1] = numberBlock;
  sizeUsedFIFO = 2;
  sizeToRead = 16;
  TxLastBits = 0;
  command = command::chip::Transeive;
  CallbackInternalComplete = ReadFIFO;
  CalculateCRC();
  Cm_PICC();
}

static void ReadFIFO(){
  CallbackInternalComplete = nullptr;
  interface::FlushRX();
  interface::Push(registers::FIFODataReg | 0x80, sizeToRead);
  ReadData();
  CallbackEndTransmission = [](){
    CallbackEndTransmission = nullptr;
    (uint8_t)interface::Read();
    for(size_t i = 0; i < sizeToRead; ++i)
      bufferFIFO[i] = interface::Read();
    if (CallbackCommandComplete) CallbackCommandComplete();
  };
}

static void callbackWriteData(){
  pinNSS::High();
  if(interface::IsTxEmpty()){
    interface::SetCountSendAtOnce(0);
    interface::CallbackRxNotEmpty = nullptr;
    if (CallbackEndTransmission) CallbackEndTransmission();
  }
  else pinNSS::Low();
}

static void WriteData(const uint8_t * data, size_t size, size_t countAtOnce){
  pinNSS::Low();
  interface::CallbackRxNotEmpty = callbackWriteData;
  interface::SetCountSendAtOnce(countAtOnce);
  interface::Write(data, size);
}

static void ReadData(const uint8_t * data, size_t size){
  pinNSS::Low();
  interface::CallbackRxNotEmpty = callbackWriteData;
  interface::Push(data, size);
  interface::Write(0);
}

static void ReadData(){
  pinNSS::Low();
  interface::CallbackRxNotEmpty = callbackWriteData;
  interface::Write(0);
}

static void CalculateCRC(){
  uint32_t crc = 0x6363;
  for(size_t i = 0; i < sizeUsedFIFO; ++i){
    uint8_t  tmp;
    tmp = bufferFIFO[i];
    tmp = (tmp ^ (uint8_t)(crc & 0x00FF));
    tmp = (tmp ^ (tmp << 4));
    crc = (crc >> 8) ^ ((uint32_t) tmp << 8) ^ ((uint32_t) tmp << 3) ^ ((uint32_t) tmp >> 4);
  }
  bufferFIFO[sizeUsedFIFO] = (uint8_t)(crc & 0xFF);
  bufferFIFO[sizeUsedFIFO + 1] = (uint8_t)((crc >> 8) & 0xFF);
  sizeUsedFIFO += 2;
}

static inline void (*CallbackEndTransmission)() = nullptr; 

static inline void (*CallbackCommandComplete)() = nullptr;

static inline void (*CallbackInternalComplete)() = nullptr;

};
 

}

#endif // !_MFRC522_HPP