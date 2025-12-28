/*-----------------------------------------------------------------------------+
|                                                                              |
| filename: main.c                                                             |
| project:  ZX Spectrum Next - ESPCMD                                          |
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

/*============================================================================*/
/*                               Includes                                     */
/*============================================================================*/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include "libzxn.h"
#include "uart.h"
#include "esp.h"
#include "espcmd.h"
#include "version.h"

/*============================================================================*/
/*                               Defines                                      */
/*============================================================================*/

/*============================================================================*/
/*                               Namespaces                                   */
/*============================================================================*/

/*============================================================================*/
/*                               Konstanten                                   */
/*============================================================================*/

/*============================================================================*/
/*                               Variablen                                    */
/*============================================================================*/
/*!
Global data of the application
*/
static appstate_t g_tState;

/*============================================================================*/
/*                               Strukturen                                   */
/*============================================================================*/

/*============================================================================*/
/*                               Typ-Definitionen                             */
/*============================================================================*/

/*============================================================================*/
/*                               Prototypen                                   */
/*============================================================================*/
/*!
This function is called once at startup of the application to reserve required
resources.
*/
void _construct(void);

/*!
This function is called once at termination of the application to release all
used resources.
*/
void _destruct(void);

/*!
Application local "printf" that handels option "-q" ("quiet") and is able to
print to stdout/stderr.
@param pStream Stream to print to ("stdout", "stderr")
@param acFmt Format string (see "printf")
@return Errorcode (see "printf"); negative values signaling errors
*/
int app_printf(FILE* pStream, char_t* acFmt, ...);

/*!
This function parses all given commandline arguments/options
@return Errorcode (EOK = no error)
*/
int parseArguments(int argc, char* argv[]);

/*!
Print help of the application
@return Errorcode (EOK = no error)
*/
int showHelp(void);

/*!
Print version information of the application
@return Errorcode (EOK = no error)
*/
int showInfo(void);

/*!
Execute AT-command
@return Errorcode (EOK = no error)
*/
int command(void);

/*============================================================================*/
/*                               Klassen                                      */
/*============================================================================*/

/*============================================================================*/
/*                               Implementierung                              */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/* _construct()                                                               */
/*----------------------------------------------------------------------------*/
void _construct(void)
{
  if (!g_tState.bInitialized)
  {
    g_tState.eAction   = ACTION_NONE;
    g_tState.bQuiet    = false;
    g_tState.acCmd[0]  = '\0';
    g_tState.uiSpeed   = zxn_getspeed();
    g_tState.iExitCode = EOK;

    zxn_setspeed(RTM_28MHZ);
    esp_open(&g_tState.tEsp);

    g_tState.bInitialized = true;
  }
}


/*----------------------------------------------------------------------------*/
/* _destruct()                                                                */
/*----------------------------------------------------------------------------*/
void _destruct(void)
{
  if (g_tState.bInitialized)
  {
    esp_close(&g_tState.tEsp);
    zxn_setspeed(g_tState.uiSpeed);
  }
}


/*----------------------------------------------------------------------------*/
/* main()                                                                     */
/*----------------------------------------------------------------------------*/
int main(int argc, char* argv[])
{
  _construct();
  atexit(_destruct);

  if (EOK == (g_tState.iExitCode = parseArguments(argc, argv)))
  {
    switch (g_tState.eAction)
    {
      case ACTION_NONE:
        g_tState.iExitCode = EOK;
        break;

      case ACTION_HELP:
        g_tState.iExitCode = showHelp();
        break;

      case ACTION_INFO:
        g_tState.iExitCode = showInfo();
        break;

      case ACTION_COMMAND:
        g_tState.iExitCode = command();
        break;
    }
  }

  return (int) (EOK == g_tState.iExitCode ? 0 : zxn_strerror(g_tState.iExitCode));
}


/*----------------------------------------------------------------------------*/
/* app_printf()                                                               */
/*----------------------------------------------------------------------------*/
int app_printf(FILE* pStream, char_t* acFmt, ...)
{
  int iReturn = EOK;

  if (pStream && acFmt) 
  {
    if (!g_tState.bQuiet)
    {
      va_list args;
      va_start(args, acFmt);

      iReturn = vfprintf(pStream, acFmt, args);

      va_end(args);
    }
  }
  else
  {
    iReturn = -EINVAL;
  }

  return iReturn;
}


/*----------------------------------------------------------------------------*/
/* parseArguments()                                                           */
/*----------------------------------------------------------------------------*/
int parseArguments(int argc, char* argv[])
{
  int iReturn = EOK;

  /* Defaults */
  g_tState.eAction = ACTION_NONE;

  int i = 1;
  while (i < argc)
  {
    const char_t* acArg = argv[i];

    if ('-' == acArg[0]) /* Options */
    {
      if ((0 == strcmp(acArg, "-h")) || (0 == stricmp(acArg, "--help")))
      {
        g_tState.eAction = ACTION_HELP;
      }
      else if ((0 == strcmp(acArg, "-v")) || (0 == stricmp(acArg, "--version")))
      {
        g_tState.eAction = ACTION_INFO;
      }
      else if ((0 == strcmp(acArg, "-q")) || (0 == stricmp(acArg, "--quiet")))
      {
        g_tState.bQuiet = true;
      }
      else
      {
        app_printf(stderr, "unknown option: %s\n", acArg);
        iReturn = EINVAL;
        break;
      }
    }
    else /* Arguments */
    {
      if ('\0' == g_tState.acCmd[0])
      {
        snprintf(g_tState.acCmd, sizeof(g_tState.acCmd), "%s", acArg);
      }
      else
      {
        app_printf(stderr, "unexpected argument: %s\n", acArg);
        iReturn = EINVAL;
        break;
      }
    }

    ++i;
  }

  if (EOK == iReturn)
  {
    if (ACTION_NONE == g_tState.eAction)
    {
      if ('\0' != g_tState.acCmd[0])
      {
        g_tState.eAction = ACTION_COMMAND;
      }
      else
      {
        app_printf(stderr, "no command specified\n");
        iReturn = EINVAL;
      }
    }
  }

  DBGPRINTF("parseargs() - action  = %d\n", g_tState.eAction);
  DBGPRINTF("parseargs() - command = %s\n", g_tState.acCmd);

  return iReturn;
}


/*----------------------------------------------------------------------------*/
/* showHelp()                                                                 */
/*----------------------------------------------------------------------------*/
int showHelp(void)
{
  char_t acAppName[0x10];
  strncpy(acAppName, VER_INTERNALNAME_STR, sizeof(acAppName));
  strupr(acAppName);

  app_printf(stdout, "%s\n\n", VER_FILEDESCRIPTION_STR);

  app_printf(stdout, "%s command [-q][-h][-v]\n\n", acAppName);
  //                  0.........1.........2.........3.
  app_printf(stdout, " command     command to execute\n");
  app_printf(stdout, " -q[uiet]    no screen output\n");
  app_printf(stdout, " -h[elp]     print this help\n");
  app_printf(stdout, " -v[ersion]  print version info\n");

  return EOK;
}


/*----------------------------------------------------------------------------*/
/* showInfo()                                                                 */
/*----------------------------------------------------------------------------*/
int showInfo(void)
{
  char_t acBuffer[0x10];
  uint16_t uiVersion;

  strncpy(acBuffer, VER_INTERNALNAME_STR, sizeof(acBuffer));
  strupr(acBuffer);

  app_printf(stdout, "%s " VER_LEGALCOPYRIGHT_STR "\n", acBuffer);

  if (ESX_DOSVERSION_NEXTOS_48K != (uiVersion = esx_m_dosversion()))
  {
    snprintf(acBuffer, sizeof(acBuffer), "NextOS %u.%02u",
             ESX_DOSVERSION_NEXTOS_MAJOR(uiVersion),
             ESX_DOSVERSION_NEXTOS_MINOR(uiVersion));
  }
  else
  {
    strncpy(acBuffer, "48K mode", sizeof(acBuffer));
  }

  //                  0.........1.........2.........3.
  app_printf(stdout, " Version %s (%s)\n", VER_FILEVERSION_STR, acBuffer);
  app_printf(stdout, " Stefan Zell (info@diezells.de)\n");

  return EOK;
}


/*----------------------------------------------------------------------------*/
/* command()                                                                  */
/*----------------------------------------------------------------------------*/
int command(void)
{
  int iReturn = EOK;

  /* Create request */
  snprintf(g_tState.esp.acTxBuffer, sizeof(g_tState.esp.acTxBuffer), "%s\r\n", g_tState.acCmd);

  app_printf(stdout, "> %s\n", g_tState.acCmd);

  /* Send request to ESP8266 */
  if (EOK == esp_transmit(&g_tState.tEsp, g_tState.esp.acTxBuffer))
  {
    for ( ; ; )
    {
      switch (esp_receive_line(&g_tState.tEsp, g_tState.esp.acRxBuffer, sizeof(g_tState.esp.acRxBuffer)))
      {
        case ESP_LINE_DATA:
          zxn_rtrim(g_tState.esp.acRxBuffer);
          if (strlen(g_tState.esp.acRxBuffer))
          {
            app_printf(stdout, "< %s\n", g_tState.esp.acRxBuffer);
          }
          break;

        case ESP_LINE_OK:
          iReturn = EOK;
          goto EXIT_LOOP;

        case ESP_LINE_ERROR:
          iReturn = ESTAT;
          goto EXIT_LOOP;

        case ESP_LINE_FAIL:
          iReturn = ERANGE;
          goto EXIT_LOOP;

        default:
          iReturn = ETIMEOUT;
          goto EXIT_LOOP;
      }
    }

    EXIT_LOOP:
  }
  else
  {
    iReturn = EBREAK;
  }

  return iReturn;
}


/*----------------------------------------------------------------------------*/
/*                                                                            */
/*----------------------------------------------------------------------------*/
