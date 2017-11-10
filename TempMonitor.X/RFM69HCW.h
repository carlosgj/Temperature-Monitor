#ifndef RFM69HCW_H
#define	RFM69HCW_H

#include <xc.h> // include processor files - each processor file is guarded.  

typedef union{
    unsigned char all;
    struct{
        unsigned SequencerOff :1;
        unsigned ListenOn :1;
        unsigned ListenAbort :1;
        unsigned Mode :3;
        unsigned RESERVED0 :2;
    };
} RegOpMode_t;

typedef union{
    unsigned char all;
    struct{
        unsigned RESERVED7 :1;
        unsigned DataMode :2;
        unsigned ModulationType :2;
        unsigned RESERVED2 :1;
        unsigned ModulationShaping :2;
    };
} RegDataModul_t;

typedef union{
    unsigned char all;
    struct{
        unsigned RcCalStart :1;
        unsigned RcCalDone :1;
        unsigned RESERVED :6;
    };
}RegOsc1_t;

typedef union{
    unsigned char all;
    struct{
        unsigned Pa0On :1;
        unsigned Pa1On :1;
        unsigned Pa2On :1;
        unsigned OutputPower :5;
    };
}RegPaLevel_t;

typedef union{
    unsigned char all;
    struct{
        unsigned RESERVED :3;
        unsigned OcpOn :1;
        unsigned OcpTrim :4;
    };
}RegOcp_t;

typedef union{
    unsigned char all;
    struct{
        unsigned LnaZin :1;
        unsigned RESERVED;
        unsigned LnaCurrentGain :3;
        unsigned LnaGainSelect :3;
    };
} RegLna_t;

typedef union{
    unsigned char all;
    struct{
        unsigned DccFreq :3;
        unsigned RxBwMant :2;
        unsigned RxBwExp :3;
    };
}RegRxBw_t;

typedef union{
    unsigned char all;
    struct{
        unsigned DccFreqAfc :3;
        unsigned RxBwMantAfc :2;
        unsigned RxBwExpAfc :3;
    };
}RegAfcBw_t;

typedef union{
    unsigned char all;
    struct{
        unsigned OokThreshType :2;
        unsigned OokPeakThreshStep :3;
        unsigned OokPeakThreshDec :3;
    };
}RegOokPeak_t;

typedef union{
    unsigned char all;
    struct{
        unsigned OokAverageThreshFit :2;
        unsigned RESERVED :6;
    };
}RegOokAvg_t;

typedef union{
    unsigned char all;
    struct{
        unsigned RESERVED :1;
        unsigned FeiDone :1;
        unsigned FeiStart :1;
        unsigned AfcDone :1;
        unsigned AfcAutoclearOn :1;
        unsigned AfcAutoOn :1;
        unsigned AfcClear :1;
        unsigned AfcStart :1;
    };
}RegAfcFei_t;

typedef union{
    unsigned char all;
    struct{
        unsigned Dio0Mapping :2;
        unsigned Dio1Mapping :2;
        unsigned Dio2Mapping :2;
        unsigned Dio3Mapping :2;
    };
}RegDioMapping1_t;

typedef union{
    unsigned char all;
    struct{
        unsigned Dio4Mapping :2;
        unsigned Dio5Mapping :2;
        unsigned RESERVED :1;
        unsigned ClkOut :3;
    };
}RegDioMapping2_t;

typedef union{
    unsigned char all;
    struct{
        unsigned ModeReady :1;
        unsigned RxReady :1;
        unsigned TxReady :1;
        unsigned PllLock :1;
        unsigned Rssi :1;
        unsigned Timeout :1;
        unsigned AutoMode :1;
        unsigned SyncAddressMatch :1;
    };
}RegIrqFlags1_t;

typedef union{
    unsigned char all;
    struct{
        unsigned FifoFull :1;
        unsigned FifoNotEmpty :1;
        unsigned FifoLevel :1;
        unsigned FifoOverrun :1;
        unsigned PacketSent :1;
        unsigned PayloadReady :1;
        unsigned CrcOk :1;
        unsigned RESERVED :1;
    };
}RegIrqFlags2_t;

typedef union{
    unsigned char all;
    struct{
        unsigned SyncOn :1;
        unsigned FifoFillCondition :1;
        unsigned SyncSize :3;
        unsigned SyncTol :3;
    };
}RegSyncConfig_t;

typedef union{
    unsigned char all;
    struct{
        unsigned PacketFormat :1;
        unsigned DcFree :2;
        unsigned CrcOn :1;
        unsigned CrcAutoClearOff :1;
        unsigned AddressFiltering :2;
        unsigned RESERVED :1;
    };
}RegPacketConfig1_t;

typedef union{
    unsigned char all;
    struct{
        unsigned EnterCondition :3;
        unsigned ExitCondition :3;
        unsigned IntermediateMode :2;
    };
}RegAutoModes_t;

#endif	/* XC_HEADER_TEMPLATE_H */

