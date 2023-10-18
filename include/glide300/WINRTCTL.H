//*********************************************************************
// WinRTctl.h    Include file for WinRT Driver
//       (c)1993-94 BlueWater Systems
//
//       9Nov93 PDL  Initial code
//      18Feb94 PDL  Add Auto Increment Macros
//       2Oct94 PDL  Change device names in WinRTOpenDevice()
//                   for Windows95
//       1Apr95 PDL  Make all device names Win95 style,
//                   Add new INTERFACE_TYPES.
//***************************************************************

// Implementation macros. These macros simplify the calls to
//  Win32 file create, device control & close handle

        // WinRTOpenDevice - get handle to driver
        //     Inputs: d  - device number 0 - n
        //                  (0 opens WRTdev0 device)
        //             s  - TRUE opens device for sharing
        //                  FALSE opens device for exclusive use
        //     Outputs: returns - HANDLE to device or
        //              INVALID_HANDLE_VALUE if error.
#define WinRTOpenDevice(d,s)\
            CreateFile(\
                "\\\\.\\WRTdev" #d,\
                0,\
                ((s) ? FILE_SHARE_READ | FILE_SHARE_WRITE : 0),\
                NULL, OPEN_EXISTING, 0, NULL)  
                
        // WinRTCloseDevice - closes handle to driver
        //     Inputs:  h  - handle to device 
        //                  (returned from WinRTOpenDevice())       
        //     Outputs: returns - error if zero
        //                   (call GetLastError() to error code)

#define WinRTCloseDevice(h)\
            CloseHandle(h)

        // WinRTGetConfiguration - gets driver configuration
        //     Inputs:  h  - handle to device 
        //                  (returned from WinRTOpenDevice())       
        //     Outputs: pC  - pointer to configuration buffer
        //                  of type WINRT_CONFIGURATION 
        //              pL  - pointer to variable which contains
        //                  length of returned buffer (DWORD)
        //              returns - error if zero
        //                   (call GetLastError() to error code)                     
#define WinRTGetConfiguration(h,pC,pL)\
            DeviceIoControl(\
                h,\
                (DWORD)IOCTL_WINRT_GET_CONFIG,\
                NULL, 0,\
                pC,\
                sizeof(WINRT_CONFIGURATION),\
                pL,\
                NULL)

        // WinRTProcessIoBuffer - process an input/output buffer
        //     Inputs:  h  - handle to device 
        //                  (returned from WinRTOpenDevice())
        //              pB - pointer to input/output buffer
        //                  (array of WINRT_CONTROL_ITEM)
        //              s  - size off input/output buffer       
        //     Outputs: pL  - pointer to variable which contains
        //                  length of returned buffer (DWORD)
        //              returns - error if zero
        //                   (call GetLastError() to error code)                     
        //      notes: pB is both an input & output
#define WinRTProcessIoBuffer(h,pB,s,pL)\
            DeviceIoControl(\
                h,\
                (DWORD)IOCTL_WINRT_PROCESS_BUFFER,\
                pB, s,\
                pB, s,\
                pL,\
                NULL)

        // WinRTWaitForInterrupt - wait for interrupt on device
        //     Inputs:  h  - handle to device 
        //                  (returned from WinRTOpenDevice())
        //              pB - pointer to input/output buffer
        //                  (array of WINRT_CONTROL_ITEM)
        //              s  - size off input/output buffer       
        //     Outputs: pL  - pointer to variable which contains
        //                  length of returned buffer (DWORD)
        //              returns - error if zero
        //                   (call GetLastError() to error code)                     
        //      notes: pB is both an input & output. Buffer must
        //          include interrupt ID & interrupt Clear commands
#define WinRTWaitForInterrupt(h,pB,s, pL)\
            DeviceIoControl(\
                h,\
                (DWORD)IOCTL_WINRT_WAIT_ON_INTERRUPT,\
                pB, s,\
                pB, s,\
                pL,\
                NULL)
                                  
        // WinRTMapMemory - map physical memory into user space
        //     Inputs:  h  - handle to device 
        //                  (returned from WinRTOpenDevice())
        //              pB - pointer to input buffer
        //                  (of type WINRT_MEMORY_MAP)
        //     Outputs: ppM - pointer to variable (of PVOID) which will
        //                   receive mapped address
        //              pL  - pointer to variable which contains
        //                  length of returned buffer (DWORD)
        //              returns - error if zero
        //                   (call GetLastError() to error code)                     
#define WinRTMapMemory(h,pB,ppM,pL)\
            DeviceIoControl (\
                 h,\
                (DWORD) IOCTL_WINRT_MAP_MEMORY,\
                pB,\
                sizeof(WINRT_MEMORY_MAP),\
                ppM,\
                sizeof(PVOID),\
                pL,\
                NULL)

        // WinRTUnMapMemory - unmap physical memory from user space
        //     Inputs:  h  - handle to device 
        //                  (returned from WinRTOpenDevice())
        //              pM - pointer to address returned from
        //                  WinRTMapMemory
        //                  (of type PVOID)
        //     Outputs: pL  - pointer to variable which contains
        //                  length of returned buffer (DWORD)
        //              returns - error if zero
        //                   (call GetLastError() to error code)                     
#define WinRTUnMapMemory(h,pM,pL)\
            DeviceIoControl (\
                h,\
                (DWORD) IOCTL_WINRT_UNMAP_MEMORY,\
                &(pM),\
                sizeof(PVOID),\
                NULL, 0,\
                pL,\
                NULL)
                                           

        // WinRTAutoIncIo - process an Auto Incrementing input/output buffer
        //     Inputs:  h  - handle to device 
        //                  (returned from WinRTOpenDevice())
        //              pIn - pointer to input buffer,
        //                   buffer sent to driver.       
        //                  (of type WINRT_AUTOINC_ITEM)
        //              sIn  - size off pIn input buffer
        //              pOut - pointer to output buffer,
        //                  buffer received from driver.
        //                  (of type PVOID)        
        //              sOut  - size off output buffer       
        //     Outputs: pL  - pointer to variable which contains
        //                  length of returned buffer (DWORD)
        //              returns - error if zero
        //                   (call GetLastError() to error code)                     
#define WinRTAutoIncIo(h,pIn,sIn,pOut, sOut, pL)\
            DeviceIoControl(\
                h,\
                (DWORD)IOCTL_WINRT_AUTOINC_IO,\
                pIn, sIn,\
                pOut, sOut,\
                pL,\
                NULL)

//
//                                  
// Device type 
#define WRT_TYPE 40010

// The Device IOCTL function codes
                // Process WINRT command buffer
                //   input/output:  use array of WINRT_CONTROL_ITEMs
                //        for input & output
#define IOCTL_WINRT_PROCESS_BUFFER \
    CTL_CODE( WRT_TYPE, 0xA00, METHOD_BUFFERED, FILE_ANY_ACCESS )

                // Get driver congiuration
                //   input:  no input
                //   output: returns configuration in WINRT_CONFIGURATION
#define IOCTL_WINRT_GET_CONFIG \
    CTL_CODE( WRT_TYPE, 0xA01, METHOD_BUFFERED, FILE_ANY_ACCESS )

                // Wait on an interrupt
                //   input/output: use array of WINRT_CONTROL_ITEMs
                //      for input & output.
                //      must have INTRP_ID_nnn and INTRP_CLR_nnnn
                //      in buffer.
#define IOCTL_WINRT_WAIT_ON_INTERRUPT \
    CTL_CODE( WRT_TYPE, 0xA02, METHOD_BUFFERED, FILE_ANY_ACCESS )

                // Map memory buffer
                //   input:  use WINRT_MEMORY_MAP
                //   output: returns pointer to mapped memory
#define IOCTL_WINRT_MAP_MEMORY \
    CTL_CODE( WRT_TYPE, 0xA03, METHOD_BUFFERED, FILE_ANY_ACCESS )

                // UnMap memory buffer
                //   input:  pointer returned from IOCTL_WINRT_MAP_MEMORY
                //   output: no return
#define IOCTL_WINRT_UNMAP_MEMORY \
    CTL_CODE( WRT_TYPE, 0xA04, METHOD_BUFFERED, FILE_ANY_ACCESS )


                // Auto incrementing I/O
                //   input:  WINRT_AUTOINC_ITEM
                //   output: PVOID buffer
#define IOCTL_WINRT_AUTOINC_IO \
    CTL_CODE( WRT_TYPE, 0xA05, METHOD_BUFFERED, FILE_ANY_ACCESS )



enum WINRT_COMMAND  // commands for WINRT_CONTROL_ITEM command buffer
{                   
                        // Command naming convention:
                        //       data size
                        //     B - byte  8 bits,
                        //     W - word 16 bits,
                        //     L - long 32 bits
                        //       port or memory mapped I/O
                        //     P - port or I/O mapped
                        //         (equivalent to x86 inp() functions)
                        //     M - memory mapped
                        //       relative or absolute addressing
                        //     A - absolute addressing
                        //       - no letter is relative addressing
                        //         (relative to the 
                        //          Section0/portAddress in the
                        //          registry.)
                        //  Example:  INP_B   port I/O, 
                        //                    relative addressing,
                        //                    byte 
                        //            INM_WA  memory mapped I/O,
                        //                    absolute addressing,
                        //                    word
                        //
                        //
                        //  Input & Output commands
                        //    port I/O commands
                        //                  param1            param2
    NOP,                //  No operation    0                   0
    INP_B,              //  input byte      port rel        byte input
    INP_W,              //  input word      port rel        word input
    INP_L,              //  input long      port rel        long input
    OUTP_B,             //  output byte     port rel        byte to output
    OUTP_W,             //  output word     port rel        word to output    
    OUTP_L,             //  output long     port rel        long to output    
    INP_BA,             //  input byte      port abs        byte input        
    INP_WA,             //  input word      port abs        word input        
    INP_LA,             //  input long      port abs        long input        
    OUTP_BA,            //  output byte     port abs        byte to output    
    OUTP_WA,            //  output word     port abs        word to output    
    OUTP_LA,            //  output long     port abs        long to output    
                        //    memory mapped I/O commands
                        //                  param1            param2    
    INM_B,              //  input byte      address rel     byte input        
    INM_W,              //  input word      address rel     word input        
    INM_L,              //  input long      address rel     long input        
    OUTM_B,             //  output byte     address rel     byte to output    
    OUTM_W,             //  output word     address rel     word to output    
    OUTM_L,             //  output long     address rel     long to output    
    INM_BA,             //  input byte      address abs     byte input        
    INM_WA,             //  input word      address abs     word input        
    INM_LA,             //  input long      address abs     long input        
    OUTM_BA,            //  output byte     address abs     byte to output    
    OUTM_WA,            //  output word     address abs     word to output    
    OUTM_LA,            //  output long     address abs     long to output    
                        //
                        //  Interrupt commands 
                        //                  param1            param2
    INTRP_ID_ALWAYS,    //  identifies interrupt as always ours.
                        //                  not used    not used
    INTRP_ID_IN,        //  inputs value read by INTRP_ID_xxx commands
                        //                  not used    value read in
                        //    Interrupt commands using port I/O
                        //                  param1            param2
    INTRP_ID_IF_SET_PB, //  identifies interrupt if the port value and'ed 
                        //    with mask is non-zero (port is byte)
                        //                  port rel    mask for set bits
    INTRP_ID_IF_NSET_PB,//  identifies interrupt if the port value and'ed 
                        //    with mask is zero     (port is byte)
                        //                  port rel    mask for not set bits
    INTRP_ID_IF_SET_PW, //  identifies interrupt if the port value and'ed     
                        //    with mask is non-zero (port is word)
                        //                  port rel    mask for set bits
    INTRP_ID_IF_NSET_PW,//  identifies interrupt if the port value and'd      
                        //    with mask is zero     (port is word)
                        //                  port rel    mask for not set bits
    INTRP_ID_IF_SET_PL, //  identifies interrupt if the port value and'd      
                        //    with mask is non-zero (port is long)
                        //                  port rel    mask for set bits     
    INTRP_ID_IF_NSET_PL,//  identifies interrupt if the port value and'd      
                        //    with mask is zero     (port is long)
                        //                  port rel    mask for not set bits 
    INTRP_ID_IF_EQ_PB,  //  identifies interrupt if the port value equals value 
                        //                  port rel    value(byte)
    INTRP_ID_IF_GT_PB,  //  identifies interrupt if the port value > value 
                        //                  port rel    value(byte)
    INTRP_ID_IF_LT_PB,  //  identifies interrupt if the port value < value
                        //                  port rel    value(byte)
    INTRP_ID_IF_EQ_PW,  //  identifies interrupt if the port value equals value 
                        //                  port rel    value(word)
    INTRP_ID_IF_GT_PW,  //  identifies interrupt if the port value > value 
                        //                  port rel    value(word)
    INTRP_ID_IF_LT_PW,  //  identifies interrupt if the port value < value
                        //                  port rel    value(word)
    INTRP_ID_IF_EQ_PL,  //  identifies interrupt if the port value equals value 
                        //                  port rel    value(long)
    INTRP_ID_IF_GT_PL,  //  identifies interrupt if the port value > value 
                        //                  port rel    value(long)
    INTRP_ID_IF_LT_PL,  //  identifies interrupt if the port value < value
                        //                  port rel    value(long)
    INTRP_CLEAR_NOP,    //  clears interrupt with no operation
                        //                  not used    not used
    INTRP_CLEAR_IN,     //  inputs value read by INTRP_CLEAR_Rxxx commands
                        //                  not used    value read in
    INTRP_CLEAR_W_PB,   //  clears interrrupt by writing value to port(byte)  
                        //                  port rel    byte to output
    INTRP_CLEAR_W_PW,   //  clears interrrupt by writing value to port(word)  
                        //                  port rel    word to output
    INTRP_CLEAR_W_PL,   //  clears interrrupt by writing value to port(long)  
                        //                  port rel    long to output
    INTRP_CLEAR_R_PB,   //  clears interrrupt by reading port(byte)  
                        //                  port rel    byte input
    INTRP_CLEAR_R_PW,   //  clears interrrupt by reading port(word)  
                        //                  port rel    word input
    INTRP_CLEAR_R_PL,   //  clears interrrupt by reading port(long)  
                        //                  port rel    long input
    INTRP_CLEAR_RMW_SET_PB, //  clears interrupt with read modify write operation.  
                        //  port is input, or'd with value and result is output
                        //                  port rel    byte mask
    INTRP_CLEAR_RMW_SET_PW, //  clears interrupt with read modify write operation.  
                        //  port is input, or'd with value and result is output
                        //                  port rel    word mask
    INTRP_CLEAR_RMW_SET_PL, //  clears interrupt with read modify write operation.  
                        //  port is input, or'd with value and result is output
                        //                  port rel    long mask
    INTRP_CLEAR_RMW_NSET_PB, //  clears interrupt with read modify write operation. 
                        //  port is input, and'd with value and result is output
                        //                  port rel    byte mask
    INTRP_CLEAR_RMW_NSET_PW, //  clears interrupt with read modify write operation. 
                        //  port is input, and'd with value and result is output
                        //                  port rel    word mask
    INTRP_CLEAR_RMW_NSET_PL, //  clears interrupt with read modify write operation. 
                        //  port is input, and'd with value and result is output
                        //                  port rel    long mask
    INTRP_ID_IF_SET_PBA,    //  identifies interrupt if the port value and'ed 
                        //    with mask is non-zero (port is byte)
                        //                  port abs    mask for set bits
    INTRP_ID_IF_NSET_PBA,//  identifies interrupt if the port value and'ed 
                        //    with mask is zero     (port is byte)
                        //                  port abs    mask for not set bits
    INTRP_ID_IF_SET_PWA,    //  identifies interrupt if the port value and'ed     
                        //    with mask is non-zero (port is word)
                        //                  port abs    mask for set bits
    INTRP_ID_IF_NSET_PWA,//  identifies interrupt if the port value and'd     
                        //    with mask is zero     (port is word)
                        //                  port abs    mask for not set bits
    INTRP_ID_IF_SET_PLA,    //  identifies interrupt if the port value and'd      
                        //    with mask is non-zero (port is long)
                        //                  port abs    mask for set bits     
    INTRP_ID_IF_NSET_PLA,//  identifies interrupt if the port value and'd     
                        //    with mask is zero     (port is long)
                        //                  port abs    mask for not set bits 
    INTRP_ID_IF_EQ_PBA, //  identifies interrupt if the port value equals value 
                        //                  port abs    value(byte)
    INTRP_ID_IF_GT_PBA, //  identifies interrupt if the port value > value 
                        //                  port abs    value(byte)
    INTRP_ID_IF_LT_PBA, //  identifies interrupt if the port value < value
                        //                  port abs    value(byte)
    INTRP_ID_IF_EQ_PWA, //  identifies interrupt if the port value equals value 
                        //                  port abs    value(word)
    INTRP_ID_IF_GT_PWA, //  identifies interrupt if the port value > value 
                        //                  port abs    value(word)
    INTRP_ID_IF_LT_PWA, //  identifies interrupt if the port value < value
                        //                  port abs    value(word)
    INTRP_ID_IF_EQ_PLA, //  identifies interrupt if the port value equals value 
                        //                  port abs    value(long)
    INTRP_ID_IF_GT_PLA, //  identifies interrupt if the port value > value 
                        //                  port abs    value(long)
    INTRP_ID_IF_LT_PLA, //  identifies interrupt if the port value < value
                        //                  port abs    value(long)
    INTRP_CLEAR_W_PBA,  //  clears interrrupt by writing value to port(byte)  
                        //                  port abs    byte to output
    INTRP_CLEAR_W_PWA,  //  clears interrrupt by writing value to port(word)  
                        //                  port abs    word to output
    INTRP_CLEAR_W_PLA,  //  clears interrrupt by writing value to port(long)  
                        //                  port abs    long to output
    INTRP_CLEAR_R_PBA,  //  clears interrrupt by reading port(byte)  
                        //                  port abs    byte input
    INTRP_CLEAR_R_PWA,  //  clears interrrupt by reading port(word)  
                        //                  port abs    word input
    INTRP_CLEAR_R_PLA,  //  clears interrrupt by reading port(long)  
                        //                  port abs    long input
    INTRP_CLEAR_RMW_SET_PBA, //  clears interrupt with read modify write operation. 
                        //  port is input, or'd with value and result is output
                        //                  port abs    byte mask
    INTRP_CLEAR_RMW_SET_PWA, //  clears interrupt with read modify write operation. 
                        //  port is input, or'd with value and result is output
                        //                  port abs    word mask
    INTRP_CLEAR_RMW_SET_PLA, //  clears interrupt with read modify write operation. 
                        //  port is input, or'd with value and result is output
                        //                  port abs    long mask
    INTRP_CLEAR_RMW_NSET_PBA, //  clears interrupt with read modify write operation.    
                        //  port is input, and'd with value and result is output
                        //                  port abs    byte mask
    INTRP_CLEAR_RMW_NSET_PWA, //  clears interrupt with read modify write operation.    
                        //  port is input, and'd with value and result is output
                        //                  port abs    word mask
    INTRP_CLEAR_RMW_NSET_PLA, //  clears interrupt with read modify write operation.    
                        //  port is input, and'd with value and result is output
                        //                  port abs    long mask
                        //
                        //  Interrupt commands 
                        //    using memory mapped I/O
                        //                  param1            param2
    INTRP_ID_IF_SET_MB, //  identifies interrupt if the address value and'ed 
                        //    with mask is non-zero (address is byte)
                        //                  address rel mask for set bits
    INTRP_ID_IF_NSET_MB,//  identifies interrupt if the address value and'ed 
                        //    with mask is zero     (address is byte)
                        //                  address rel mask for not set bits
    INTRP_ID_IF_SET_MW, //  identifies interrupt if the address value and'ed      
                        //    with mask is non-zero (address is word)
                        //                  address rel mask for set bits
    INTRP_ID_IF_NSET_MW,//  identifies interrupt if the address value and'd       
                        //    with mask is zero     (address is word)
                        //                  address rel mask for not set bits
    INTRP_ID_IF_SET_ML, //  identifies interrupt if the address value and'd       
                        //    with mask is non-zero (address is long)
                        //                  address rel mask for set bits     
    INTRP_ID_IF_NSET_ML,//  identifies interrupt if the address value and'd       
                        //    with mask is zero     (address is long)
                        //                  address rel mask for not set bits 
    INTRP_ID_IF_EQ_MB,  //  identifies interrupt if the memory value equals value 
                        //                  address rel    value(byte)
    INTRP_ID_IF_GT_MB,  //  identifies interrupt if the memory value > value 
                        //                  address rel value(byte)
    INTRP_ID_IF_LT_MB,  //  identifies interrupt if the memory value < value
                        //                  address rel value(byte)
    INTRP_ID_IF_EQ_MW,  //  identifies interrupt if the memory value equals value 
                        //                  address rel    value(word)
    INTRP_ID_IF_GT_MW,  //  identifies interrupt if the memory value > value 
                        //                  address rel value(word)
    INTRP_ID_IF_LT_MW,  //  identifies interrupt if the memory value < value
                        //                  address rel value(word)
    INTRP_ID_IF_EQ_ML,  //  identifies interrupt if the memory value equals value 
                        //                  address rel    value(long)
    INTRP_ID_IF_GT_ML,  //  identifies interrupt if the memory value > value 
                        //                  address rel value(long)
    INTRP_ID_IF_LT_ML,  //  identifies interrupt if the memory value < value
                        //                  address rel value(long)
    INTRP_CLEAR_W_MB,   //  clears interrrupt by writing value to address(byte)  
                        //                  address rel byte to output
    INTRP_CLEAR_W_MW,   //  clears interrrupt by writing value to address(word)  
                        //                  address rel word to output
    INTRP_CLEAR_W_ML,   //  clears interrrupt by writing value to address(long)  
                        //                  address rel long to output
    INTRP_CLEAR_R_MB,   //  clears interrrupt by reading address(byte)  
                        //                  address rel byte input
    INTRP_CLEAR_R_MW,   //  clears interrrupt by reading address(word)  
                        //                  address rel word input
    INTRP_CLEAR_R_ML,   //  clears interrrupt by reading address(long)  
                        //                  address rel long input
    INTRP_CLEAR_RMW_SET_MB, //  clears interrupt with read modify write operation.  
                        //  address is input, or'd with value and result is output
                        //                  address rel byte mask
    INTRP_CLEAR_RMW_SET_MW, //  clears interrupt with read modify write operation.  
                        //  address is input, or'd with value and result is output
                        //                  address rel word mask
    INTRP_CLEAR_RMW_SET_ML, //  clears interrupt with read modify write operation.  
                        //  address is input, or'd with value and result is output
                        //                  address rel long mask
    INTRP_CLEAR_RMW_NSET_MB, //  clears interrupt with read modify write operation. 
                        //  address is input, and'd with value and result is output
                        //                  address rel byte mask
    INTRP_CLEAR_RMW_NSET_MW, //  clears interrupt with read modify write operation. 
                        //  address is input, and'd with value and result is output
                        //                  address rel word mask
    INTRP_CLEAR_RMW_NSET_ML, //  clears interrupt with read modify write operation. 
                        //  address is input, and'd with value and result is output
                        //                  address rel long mask
    INTRP_ID_IF_SET_MBA,    //  identifies interrupt if the address value and'ed 
                        //    with mask is non-zero (address is byte)
                        //                  address abs mask for set bits
    INTRP_ID_IF_NSET_MBA,//  identifies interrupt if the address value and'ed 
                        //    with mask is zero     (address is byte)
                        //                  address abs mask for not set bits
    INTRP_ID_IF_SET_MWA,    //  identifies interrupt if the address value and'ed      
                        //    with mask is non-zero (address is word)
                        //                  address abs mask for set bits
    INTRP_ID_IF_NSET_MWA,//  identifies interrupt if the address value and'd      
                        //    with mask is zero     (address is word)
                        //                  address abs mask for not set bits
    INTRP_ID_IF_SET_MLA,    //  identifies interrupt if the address value and'd       
                        //    with mask is non-zero (address is long)
                        //                  address abs mask for set bits     
    INTRP_ID_IF_NSET_MLA,//  identifies interrupt if the address value and'd      
                        //    with mask is zero     (address is long)
                        //                  address abs mask for not set bits 
    INTRP_ID_IF_EQ_MBA, //  identifies interrupt if the memory value equals value 
                        //                  address abs    value(byte)
    INTRP_ID_IF_GT_MBA, //  identifies interrupt if the memory value > value 
                        //                  address abs value(byte)
    INTRP_ID_IF_LT_MBA, //  identifies interrupt if the memory value < value
                        //                  address abs value(byte)
    INTRP_ID_IF_EQ_MWA, //  identifies interrupt if the memory value equals value 
                        //                  address abs    value(word)
    INTRP_ID_IF_GT_MWA, //  identifies interrupt if the memory value > value 
                        //                  address abs value(word)
    INTRP_ID_IF_LT_MWA, //  identifies interrupt if the memory value < value
                        //                  address abs value(word)
    INTRP_ID_IF_EQ_MLA, //  identifies interrupt if the memory value equals value 
                        //                  address abs    value(long)
    INTRP_ID_IF_GT_MLA, //  identifies interrupt if the memory value > value 
                        //                  address abs value(long)
    INTRP_ID_IF_LT_MLA, //  identifies interrupt if the memory value < value
                        //                  address abs value(long)
    INTRP_CLEAR_W_MBA,  //  clears interrrupt by writing value to address(byte)  
                        //                  address abs byte to output
    INTRP_CLEAR_W_MWA,  //  clears interrrupt by writing value to address(word)  
                        //                  address abs word to output
    INTRP_CLEAR_W_MLA,  //  clears interrrupt by writing value to address(long)  
                        //                  address abs long to output
    INTRP_CLEAR_R_MBA,  //  clears interrrupt by reading address(byte)  
                        //                  address abs byte input
    INTRP_CLEAR_R_MWA,  //  clears interrrupt by reading address(word)  
                        //                  address abs word input
    INTRP_CLEAR_R_MLA,  //  clears interrrupt by reading address(long)  
                        //                  address abs long input
    INTRP_CLEAR_RMW_SET_MBA, //  clears interrupt with read modify write operation. 
                        //  address is input, or'd with value and result is output
                        //                  address abs byte mask
    INTRP_CLEAR_RMW_SET_MWA, //  clears interrupt with read modify write operation. 
                        //  address is input, or'd with value and result is output
                        //                  address abs word mask
    INTRP_CLEAR_RMW_SET_MLA, //  clears interrupt with read modify write operation. 
                        //  address is input, or'd with value and result is output
                        //                  address abs long mask
    INTRP_CLEAR_RMW_NSET_MBA, //  clears interrupt with read modify write operation.    
                        //  address is input, and'd with value and result is output
                        //                  address abs byte mask
    INTRP_CLEAR_RMW_NSET_MWA, //  clears interrupt with read modify write operation.    
                        //  address is input, and'd with value and result is output
                        //                  address abs word mask
    INTRP_CLEAR_RMW_NSET_MLA, //  clears interrupt with read modify write operation.    
                        //  address is input, and'd with value and result is output
                        //                  address abs long mask
                        //
                        // Miscellaneous commands
                        //
    DELAY,              //  delay thread for count milliseconds (can NOT be used
                        //  in a wait for interrupt request)
                        //                  0           count in millisecond units
    STALL,              //  stall processor for count microseconds (to follow        
                        //  Microsoft's driver guidelines you should always
                        //  keep this value as small as possible and Never greater
                        //  than 50 microseconds.)
                        //                  0           count in microseconds
};                                         


            // Process buffer item for input to IOCTL_WINRT_PROCESS_BUFFER
            //                              and IOCTL_WINRT_WAIT_ON_INTERRUPT
typedef struct _WINRT_CONTROL_ITEM
{
    enum WINRT_COMMAND  command;    // command to perform
    ULONG               port;       // port address
    ULONG               value;      // input or output data
} WINRT_CONTROL_ITEM, *PWINRT_CONTROL_ITEM;


              // Configuration item for output from IOCTL_WINRT_GET_CONFIG
typedef struct _WINRT_CONFIGURATION
{
    USHORT  majorVer;               // driver major version number
    USHORT  minorVer;               //        minor version number
    ULONG   busType;                // type of bus,
                                    //   see enum INTERFACE_TYPE
    ULONG   busNumber;              // number of bus: 0
    ULONG   ioIncrement;            // priority boast
    ULONG   portMin;                // minimum port address
    ULONG   portMax;                // maximum port address
    ULONG   memMin;                 // minimum memory address
    ULONG   memMax;                 // maximum memory address
    ULONG   interruptVector;        // interrupt vector
    ULONG   interruptLevel;         // interrupt level
                                    // section 1 data
    ULONG   portMin1;               // minimum port address
    ULONG   portMax1;               // maximum port address
    ULONG   memMin1;                // minimum memory address
    ULONG   memMax1;                // maximum memory address
} WINRT_CONFIGURATION, *PWINRT_CONFIGURATION;


            // Map memory specification for input to IOCTL_WINRT_MAP_MEMORY
typedef struct _WINRT_MEMORY_MAP
{
    PVOID   address;                // physical address to be mapped
    ULONG   length;                 // number of bytes to be mapped (>0)
} WINRT_MEMORY_MAP, *PWINRT_MEMORY_MAP;


            // Auto Incrementing I/O buffer item for input to IOCTL_WINRT_AUTOINC_IO.
            //  The output buffer is placed in the union value.
typedef struct _WINRT_AUTOINC_ITEM
{
    enum WINRT_COMMAND  command;    // command to perform - eg OUTP_B, INP_WA
    ULONG               port;       // port address
    union
    {
      UCHAR             bytes[1];   // array of 8-bits
      USHORT            shorts[1];  // array of 16-bits
      ULONG             longs[1];   // array of 32-bits
    }                   value;      // output buffer   
} WINRT_AUTOINC_ITEM, *PWINRT_AUTOINC_ITEM;
                // size of the header portion of WINRT_AUTOINC_ITEM struct
#define AUTOINC_HEADER_SIZE\
     (sizeof(enum WINRT_COMMAND) + sizeof(ULONG)) 


#if !defined(_NTDDK_)
            // interface types returned in busType (WINRT_CONFIGURATION)
typedef enum _INTERFACE_TYPE
{
    Internal,
    Isa,
    Eisa,
    MicroChannel,
    TurboChannel,
    PCIBus,
    VMEBus,
    NuBus,
    PCMCIABus,
    CBus,
    MPIBus,
    MPSABus,
    MaximumInterfaceType
} INTERFACE_TYPE, *PINTERFACE_TYPE;
#endif
