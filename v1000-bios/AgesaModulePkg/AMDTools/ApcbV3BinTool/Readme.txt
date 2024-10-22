1. Run with GUI tool
  I. Run tool in PI package: double click ApcbV3BinTool.exe to run GUI tool
  II. Make MemoryTunningKit release package: double click Generate_MemoryTuningKit.bat to generate package
  
2. Run with Command line
  I. Read SBIOS command:
    ApcbV3Editor.bat -r <SOCInfo> <InputBinary> <InputHeaderFile>
    e.g. ApcbV3Edit.bat -r RV MandolinInt.FD D:\ApcbV3\Apcb.h

  II. Write SBIOS command:
    ApcbV3Editor.bat -w <InputBinary> <OutputBinary> <InputHeaderFile>
    e.g. ApcbV3Edit.bat -w MandolinInt.FD out.rom D:\ApcbV3\Apcb.h
  PS: Multiple input header file is supported. Please split header file path with ";".
  
  III. Quick R/W SBIOS:
    Select both SBIOS and header file folder(SSP folder), drag the two selected file and folder onto ApcbV3Read_SSP.bat/ApcbV3Write_SSP.bat
  
3. Key Words of header files
  I. Header file start flag:
    //
    // @APCB_START
    //
  II. Section flag:
    //
    // SECTION_NAME                         <=== The name can be @MACRO/@CONSTANT/@APCB_ENUM/@APCB_STRUCT/@APCB_UNION/@APCB_TOKEN
    //
    PS:
      a. Only @MACRO/@APCB_TOKEN/@CONSTANT is necessary. @APCB_ENUM/@APCB_STRUCT/@APCB_UNION will be auto-detected.
      b. If no section flag is given, script will consider current section as @CONSTANT.
      c. All the name of token in @APCB_TOKEN section should start with "APCB_TOKEN_UID". Other wise, the definition will be considered as constant instead of token.

  III. Macro/Constant section key words:
    a. "///<" => Define description
    b. @VALUE: define value for macro defined constant.
      e.g.
      #define XGMI_TX_EQ_SPEED_53    XGMI_BIT_POS(0)    ///< @VALUE:1  Instance ID of 5.3G              <=== Value will be "1", Description will be "Instance ID of 5.3G"

  IV. Enum section key words:
    a. "/**...*/" block => Define short name(optional), description (optional)
      e.g.
      /** SHORT_NAME
      *    Desc line 1
      *    Desc line 2
      */
    b. "///<" => used to define description of each element
    c. Enum type value. Enum type constant value is defined as <Type name>::<Field name>. Which can be used as constant value in comments.
       e.g.
        typedef union {
          PTO_VALIDITY_DRAM_MFG_ID_ENTRY  PtoValDramMfgId; ///< @SELECTION: PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_VALIDITY_DRAM_MFG_ID              <========= "PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_VALIDITY_DRAM_MFG_ID" is enum type constant.
          PTO_VALIDITY_DRAM_STEPPING_ENTRY   PtoValDramStepping;  ///< @SELECTION:  PTEType = PLATFORM_TUNING_ENTRY::PLATFORM_TUNING_VALIDITY_DRAM_STEPPING
        } MEM_PLATFORM_TUNING;

  V. Struct section key words:
    a. "/**...*/" block => Define short name(optional), description (optional), groupId, typeId, alignment (optional)
    b. "@GROUPID:" => Define group ID. Define with constant is suggested. So that constant name will be displayed in output xml.
      e.g. @GROUPID: APCB_GROUP_MEMORY
    c. "@TYPEID:" => Define type ID.
      e.g. @TYPEID: APCB_MEM_TYPE_ERROR_OUT_EVENT_CONTROL
    d. "@ALIGNMENT" => Define the forced alignment. (Optional)
      e.g. @ALIGNMENT: 1      <=== Force 1 byte alignment
    e. "///<" => Define description of fields in structure
    f. Multiple group/type ID support:
       Only 1 GroupID can be given in each "@GROUPID" key word. If multiple groupID is needed, they should be defined in multiple "@GROUPID" key word.
       Multiple typeID can be defined in each "@TYPEID" key word and splitted by ",".
    e.g.
      /** SHORT_NAME         <=== Must write in the first line, after "/**"
      *    @GROUPID: 0x1701  <=== Group ID is defined as 0x1701
      *    @TYPEID: 0x1,0x2  <=== Two Group/Type ID pair is defined. 0x1701,0x1; 0x1701,0x2
      *    @ALIGNMENT: 1     <=== Alignment these two Group/Type ID pair is forced as 1 Byte.
      *    Desc line 1       <=== Description for these two Group/Type ID pair
      *    Desc line 2
      *    @GROUPID: 0x1702  <=== GroupID is defined as 0x1702
      *    @TYPEID: 0x1      <=== Group/Type ID pair is defined as 0x1702,0x1
      *    Desc line1        <=== Description for Group/Type ID 0x1702,0x1. Do not force alignment as no "@ALIGNMENT" is defined.
      *    Desc line2
      *    Desc line3
      */

  VI. Union section key words:
    a. "/**...*/" block, "@GROUPID:", "@TYPEID:", "///<" => Same as struct section
    b. Union supports both typdef struct and internal defined struct.
	   e.g. 
      typedef union {
        struct {                       ///< PSCFG_MAXFREQ_ENTRY
          _1BYTE_ALIGN UINT8 DimmPerCh;             ///< Dimm slot per chanel
          _2BYTE_ALIGN UINT16 Dimms;              ///< Number of Dimms on a channel
          _2BYTE_ALIGN UINT16 SR;                 ///< Number of single-rank Dimm
          _2BYTE_ALIGN UINT16 DR;                 ///< Number of dual-rank Dimm
          _2BYTE_ALIGN UINT16 QR;                 ///< Number of quad-rank Dimm
          _2BYTE_ALIGN UINT16 Speed1_5V;            ///< Speed limit with voltage 1.5V
          _2BYTE_ALIGN UINT16 Speed1_35V;           ///< Speed limit with voltage 1.35V
          _2BYTE_ALIGN UINT16 Speed1_25V;           ///< Speed limit with voltage 1.25V
        } _MAXFREQ_ENTRY;                          <==========  Internal defined struct
        MAXFREQ_ENTRY     maxFreqEntry;            <==========  Typedef struct
      } PSCFG_MAXFREQ_ENTRY;

	c. "@SELECTION:" => Define the switch case of each struct in union. Script will use the first struct as default if no "@SELECTION" key word is given.
    e.g.
      typedef union {
        struct {                       ///< PSCFG_MAXFREQ_ENTRY
          _1BYTE_ALIGN UINT8 DimmPerCh;             ///< Dimm slot per chanel
                                                    ///< @SELECTION: Dimms = 0x3      <==========  For internal defined struct, @SELECTION Key word is placed in description of first field. Format: @SELECTION: <Field name> = <Value>
          _2BYTE_ALIGN UINT16 Dimms;              ///< Number of Dimms on a channel
          _2BYTE_ALIGN UINT16 SR;                 ///< Number of single-rank Dimm
          _2BYTE_ALIGN UINT16 DR;                 ///< Number of dual-rank Dimm
          _2BYTE_ALIGN UINT16 QR;                 ///< Number of quad-rank Dimm
          _2BYTE_ALIGN UINT16 Speed1_5V;            ///< Speed limit with voltage 1.5V
          _2BYTE_ALIGN UINT16 Speed1_35V;           ///< Speed limit with voltage 1.35V
          _2BYTE_ALIGN UINT16 Speed1_25V;           ///< Speed limit with voltage 1.25V
        } _MAXFREQ_ENTRY;
        MAXFREQ_ENTRY     maxFreqEntry;            ///< @SELECTION: DimmSlotPerCh = 0x1 <==========  For typedef struct, @SELECTION Key word is placed in description. Format: @SELECTION: <Field name> = <Value>
      } PSCFG_MAXFREQ_ENTRY;

  VII. Token section key words:
    a. "///" => Define group ID, type ID, size, description, range
    b. "@GROUPID:" => key word of group ID, define all the following tokens until next @GROUPID/@APCB_TOKEN key word
    c. "@TYPEID:" => key word of type ID, define all the following tokens until next @TYPEID/@GROUPID/@APCB_TOKEN key word
    d. "@SIZE:" => key word of size, define all the following tokens until next @SIZE/@TYPEID/@GROUPID/@APCB_TOKEN key word
    e. "@DESC:" = > key word of description, only define the next following token.
    f. "@RANGE:" => key word of range, enum is supported when defining valid range, only define the next following token.
    PS: Each key word can exist independently.
      e.g.
      /// @GROUPID: 0x3000
      /// @TYPEID: 0x0
      /// @SIZE: 1
      #define APCB_TOKEN_UID_TYPE0
      /// @TYPEID: 0x1
      #define APCB_TOKEN_UID_TYPE1
      /// @TYPEID: 0x2
      /// @SIZE: 2
      #define APCB_TOKEN_UID_TYPE2_0
      /// @DESC: This is description
      /// @RANGE: 0x0~0x1F
      #define APCB_TOKEN_UID_TYPE2_1

      Redefinition of token section will clear the groupID/typeID/size.
      e.g.
      /// @GROUPID: 0x3000
      /// @TYPEID: 0x0
      /// @SIZE: 1
      #define APCB_TOKEN_UID_TYPE0
      /// @TYPEID: 0x1
      #define APCB_TOKEN_UID_TYPE1             <=== Valid


      /// @GROUPID: 0x3000
      /// @TYPEID: 0x0
      /// @SIZE: 1
      #define APCB_TOKEN_UID_TYPE0
      //
      // @APCB_TOKEN
      //
      /// @TYPEID: 0x1
      #define APCB_TOKEN_UID_TYPE1              <=== Invalid, groupID should be defined again after "@APCB_TOKEN"

    
4. Limitation
  I. Tool support header files include. The base path of include header file is defined as dir of input header file.
    e.g. 
    Input header file: D:\ApcbV3\APCB.h
    Included header file: #include "Internal\APCB_UID.h"
    Target file to parse: D:\ApcbV3\Internal\APCB_UID.h

  II. Tool can not handle macro yet. For those macro defined constants, value must be given in definition by @VALUE.
    e.g. #define XGMI_TX_EQ_SPEED_53    XGMI_BIT_POS(0)    ///< @VALUE:1

  III. Section flag is necessary for macro, token and constant to make tool know what kind of "#define" is being handled. And it is optional for struct, enum and union

  IV. Constant should be defined before it is used in comment.

  V. Internal union define in struct is not supported. Using union type defined outside the struct is accepted.

5. Error Log
  I. "Warning: Unknown Token UID 0x..."
    Group/Type ID is defined; Token UID is not defined in header file.
  II. "Warning: Unknown potential token in binary ... Group 0x..., Type 0x..., UID 0x..."
    GroupId/TypeId/UID for this token is not defined in header file.
  III. "Error: /**..*/ block incorrect in file ... line ..."
    Existing line that is not start by "/*" in /**..*/ block
