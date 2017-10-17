/*******************************************************************************
 *
 *  BSD 2-Clause License
 *
 *  Copyright (c) 2017, Sandeep Prakash
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

/*******************************************************************************
 * Copyright (c) 2017, Sandeep Prakash <123sandy@gmail.com>
 *
 * \file   main.cpp
 *
 * \author Sandeep Prakash
 *
 * \date   Aug 18, 2017
 *
 * \brief
 *
 ******************************************************************************/

/********************************** INCLUDES **********************************/
#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/time.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <err.h>

#include <event2/event.h>
#include <event2/util.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include <iostream>
#include <thread>
#include <chrono>

#include <event2/http.h>
#include <event2/http_struct.h>
#include <glog/logging.h>

#include <ch-cpp-utils/thread-pool.hpp>

#include "es-client.hpp"

#include "indexer.hpp"

using namespace std;

/********************************* CONSTANTS **********************************/

/*********************************** MACROS ***********************************/
#define INDEXER_DEFAULT_LISTEN_PORT (8888)

/******************************** ENUMERATIONS ********************************/

/************************* STRUCTURE/UNION DATA TYPES *************************/

/************************ STATIC FUNCTION PROTOTYPES **************************/
static void indexer_get_opts_from_args (int argc, char **argv,
      INDEXER_ARGS_X *px_indexer_args);

static void event_log_cbk(int severity, const char *msg);

static void indexer_print_usage (struct option *long_opt,
                              const char **long_opt_description,
                              uint32_t ui_opts_count,
                              char **argv);

static bool indexer_is_opts_from_args_valid (INDEXER_ARGS_X *px_indexer_args);

/****************************** LOCAL FUNCTIONS *******************************/
using namespace std;

static void event_log_cbk(int severity, const char *msg) {

}

static void indexer_print_usage (struct option *long_opt,
                              const char **long_opt_description,
                              uint32_t ui_opts_count,
                              char **argv) {
   uint32_t ui_opts_idx = 0;
   printf("Usage: %s [OPTIONS]\n", argv[0]);
   for (ui_opts_idx = 0; ui_opts_idx < ui_opts_count; ui_opts_idx++)
   {
      if (NULL == long_opt[ui_opts_idx].name)
      {
         break;
      }
      char temp[20] = {0};
      strcat(temp, "--");
      strcat(temp, long_opt[ui_opts_idx].name);

      printf("  -%c %-20s (OR %22s=%-20s) - %s\n",
            long_opt[ui_opts_idx].val,
            long_opt[ui_opts_idx].name,
            temp,
            long_opt[ui_opts_idx].name,
            long_opt_description[ui_opts_idx]);
   }
   printf("\n");
}

static bool indexer_is_opts_from_args_valid (INDEXER_ARGS_X *px_indexer_args) {
   printf ("Entered Peer Ports: %d\n", px_indexer_args->numPorts);
   return true;
}

static void indexer_get_opts_from_args (int argc, char **argv,
      INDEXER_ARGS_X *px_indexer_args)
{
   uint32_t ui_opts_count = 0;
   int             c;
   const char    * short_opt = "ha:b:c:w:s:p:l:";
   struct option   long_opt[] =
   {
      {"help",          no_argument,       NULL, 'h'},
      {"listen-port",   optional_argument, NULL, 'l'},
      {NULL,            0,                 NULL, 0  }
   };
   const char *long_opt_description[] =
   {
         "Print this help and exit.",
         "(default=8888) Listen Port.",
   };

   px_indexer_args->listenPort = INDEXER_DEFAULT_LISTEN_PORT;

   ui_opts_count = sizeof(long_opt) / sizeof (struct option);
   while ((c = getopt_long(argc, argv, short_opt, long_opt, NULL)) != -1) {
      switch (c) {
      case -1: /* no more arguments */
      case 0: /* long options toggles */
         break;

      case 'h':
         indexer_print_usage(long_opt, long_opt_description, ui_opts_count, argv);
         exit (0);
         break;
      case 'l':
         printf("you entered \"%s\"\n", optarg);
         px_indexer_args->listenPort = atoi (optarg);
         if (px_indexer_args->listenPort < 1024 ||
               px_indexer_args->listenPort > 65535) {
            px_indexer_args->listenPort = INDEXER_DEFAULT_LISTEN_PORT;
         }
         break;
      default:
         fprintf(stderr, "%s: invalid option -- %c\n", argv[0], c);
         fprintf(stderr, "Try `%s --help' for more information.\n", argv[0]);
      };
   };

   if (!indexer_is_opts_from_args_valid (px_indexer_args)) {
      indexer_print_usage(long_opt, long_opt_description, ui_opts_count, argv);
      exit (0);
   }
}

int main (int argc, char **argv)
{
      PAL_LOGGER_INIT_PARAMS_X x_init_params = {false};
      pal_env_init ();

      x_init_params.e_level = eLOG_LEVEL_HIGH;
      x_init_params.b_enable_console_logging = true;
      pal_logger_env_init(&x_init_params);

   INDEXER_ARGS_X *px_indexer_args = (INDEXER_ARGS_X *) malloc (sizeof (INDEXER_ARGS_X));
   memset (px_indexer_args, 0x00, sizeof (*px_indexer_args));
   indexer_get_opts_from_args (argc, argv, px_indexer_args);

   px_indexer_args->listenPort = INDEXER_DEFAULT_LISTEN_PORT;

   event_set_log_callback(event_log_cbk);

   Indexer *indexer = new Indexer (px_indexer_args);
   indexer->start();

   std::chrono::milliseconds ms(1000);
   while (true) {
      std::this_thread::sleep_for(ms);
   }

   return 0;
}
