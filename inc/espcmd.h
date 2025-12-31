/*-----------------------------------------------------------------------------+
|                                                                              |
| filename: espcmd.h                                                           |
| project:  ZX Spectrum Next - PING                                            |
| author:   Stefan Zell                                                        |
| date:     12/26/2025                                                         |
|                                                                              |
+------------------------------------------------------------------------------+
|                                                                              |
| description:                                                                 |
|                                                                              |
| Application to send AT-commands to ESP8266 on ZX Spectrum Next               |
| (based on "espbaud" from Allen Albright)                                     |
|                                                                              |
+------------------------------------------------------------------------------+
|                                                                              |
| Copyright (c) 12/26/2025 STZ Engineering                                     |
|                                                                              |
| This software is provided  "as is",  without warranty of any kind, express   |
| or implied. In no event shall STZ or its contributors be held liable for any |
| direct, indirect, incidental, special or consequential damages arising out   |
| of the use of or inability to use this software.                             |
|                                                                              |
| Permission is granted to anyone  to use this  software for any purpose,      |
| including commercial applications,  and to alter it and redistribute it      |
| freely, subject to the following restrictions:                               |
|                                                                              |
| 1. Redistributions of source code must retain the above copyright            |
|    notice, definition, disclaimer, and this list of conditions.              |
|                                                                              |
| 2. Redistributions in binary form must reproduce the above copyright         |
|    notice, definition, disclaimer, and this list of conditions in            |
|    documentation and/or other materials provided with the distribution.      |
|                                                                          ;-) |
+-----------------------------------------------------------------------------*/

#if !defined(__ESPCMD_H__)
  #define __ESPCMD_H__

/*============================================================================*/
/*                               Includes                                     */
/*============================================================================*/

/*============================================================================*/
/*                               Defines                                      */
/*============================================================================*/
/*!
Maximum length of a AT command to ESP8266
*/
#define uiMAX_LEN_CMD (0x80)

/*============================================================================*/
/*                               Namespaces                                   */
/*============================================================================*/

/*============================================================================*/
/*                               Konstanten                                   */
/*============================================================================*/

/*============================================================================*/
/*                               Variablen                                    */
/*============================================================================*/

/*============================================================================*/
/*                               Strukturen                                   */
/*============================================================================*/

/*============================================================================*/
/*                               Typ-Definitionen                             */
/*============================================================================*/
/*!
Enumeration/list of all actions the application can execute
*/
typedef enum _action
{
  ACTION_NONE = 0,
  ACTION_HELP,
  ACTION_INFO,
  ACTION_COMMAND
} action_t;

/*!
In dieser Struktur werden alle globalen Daten der Anwendung gespeichert.
*/
typedef struct _appstate
{
  /*!
  If this flag is set, then this structure is initialized
  */
  bool bInitialized;

  /*!
  Action to execute (help, version, ping, ...)
  */
  action_t eAction;

  /*!
  If this flag is set, no messages are printed to the console while pinging.
  */
  bool bQuiet;

  /*!
  Baudrate used for communication with ESP8266 (default: 115200 bit/s)
  */
  uint32_t uiBaudrate;

  /*!
  Timeout used for communication with ESP8266 (default: ~2000 ms)
  */
  uint16_t uiTimeout;

  /*!
  Name of the host to ping
  */
  char_t acCmd[uiMAX_LEN_CMD];

  /*!
  Backup: Current speed of Z80N
  */
  uint8_t uiSpeed;

  /*!
  Device data of the ESP connection
  */
  esp_t tEsp;

  struct
  {
    /*!
    Buffer for response from ESP8266
    */
    char_t acTxBuffer[uiMAX_LEN_CMD];

    /*!
    Buffer for response from ESP8266
    */
    char_t acRxBuffer[uiMAX_LEN_CMD];
  } esp;
  
  /*!
  Exitcode of the application, that is handovered to BASIC
  */
  int iExitCode;
} appstate_t;

/*============================================================================*/
/*                               Prototypen                                   */
/*============================================================================*/

/*============================================================================*/
/*                               Klassen                                      */
/*============================================================================*/

/*============================================================================*/
/*                               Implementierung                              */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/*                                                                            */
/*----------------------------------------------------------------------------*/

#endif /* __ESPCMD_H__ */
