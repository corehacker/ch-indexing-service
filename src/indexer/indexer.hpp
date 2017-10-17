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
 * \file   indexer.hpp
 *
 * \author Sandeep Prakash
 *
 * \date   Aug 21, 2017
 *
 * \brief
 *
 ******************************************************************************/

#include <ch-pal/exp_pal.h>
#include <ch-utils/exp_list.h>
#include <ch-utils/exp_hashmap.h>
#include <ch-utils/exp_sock_utils.h>
#include <ch-cpp-utils/thread-pool.hpp>
#include <ch-cpp-utils/tcp-listener.hpp>
#include <ch-cpp-utils/tcp-server.hpp>

#include <ch-protos/packet.pb.h>
#include <ch-protos/communication.pb.h>


#ifndef __SRC_INDEXER_INDEXER_HPP__
#define __SRC_INDEXER_INDEXER_HPP__

using ChCppUtils::TcpServer;
using ChCppUtils::client_ctxt;

typedef struct _INDEXER_ARGS_X
{
   in_port_t listenPort;

   uint32_t numPorts;
} INDEXER_ARGS_X;

class Indexer {
private:
      TcpServer *server;
      INDEXER_ARGS_X x_args;
      EsClient *esClient;

      static void _onNewConnection (client_ctxt *client, void *this_);
      static void _onNewMessage (client_ctxt *client, uint8_t *message, uint32_t length, void *this_);
      static void _onPeerDisconnect (client_ctxt *client, short what, void *this_);

      void onNewConnection (client_ctxt *client);
      void onNewMessage (client_ctxt *client, uint8_t *message, uint32_t length);
      void onPeerDisconnect (client_ctxt *client, short what);


public:
      Indexer(INDEXER_ARGS_X *px_args);
      ~Indexer();
      int start ();
};

#endif /* __SRC_INDEXER_INDEXER_HPP__ */
