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
 * \file   indexer.cpp
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
#include <string>

#include <event2/http.h>
#include <event2/http_struct.h>
#include <glog/logging.h>

#include <ch-cpp-utils/thread-pool.hpp>

#include "es-client.hpp"
#include "indexer.hpp"

using namespace std;

using indexer::PacketHeader;
using indexer::Packet;
using indexer::Index;
using indexer::IndexEntry;

/********************************* CONSTANTS **********************************/

/*********************************** MACROS ***********************************/

/******************************** ENUMERATIONS ********************************/

/************************* STRUCTURE/UNION DATA TYPES *************************/


Indexer::Indexer(INDEXER_ARGS_X *px_args) {
      server = new TcpServer (INADDR_ANY, px_args->listenPort);
      esClient = new EsClient();
}

Indexer::~Indexer(){
      // TODO: release resources.
}

void Indexer::_onNewConnection (client_ctxt *client, void *this_) {
      Indexer *this__ = (Indexer *) this_;
      this__->onNewConnection(client);
}

void Indexer::_onNewMessage (client_ctxt *client, uint8_t *message, uint32_t length, void *this_) {
      Indexer *this__ = (Indexer *) this_;
      this__->onNewMessage(client, message, length);
}

void Indexer::_onPeerDisconnect (client_ctxt *client, short what, void *this_) {
      Indexer *this__ = (Indexer *) this_;
      this__->onPeerDisconnect(client, what);
}

void Indexer::onNewConnection (client_ctxt *client) {

}

void Indexer::onNewMessage (client_ctxt *client, uint8_t *message, uint32_t length) {
   LOG(INFO) << "New Message";
      LOG(INFO) << "Read " << length << " bytes from socket";

//      LOG(INFO) << "";
//      for (uint32_t i = 0; i < length; i++) {
//            printf ("0x%x ", message[i]);
//      }
//      LOG(INFO) << "";

   Packet packet;
   packet.ParseFromArray((const void *) message, length);
   if (packet.has_header() && packet.has_payload()) {
      PacketHeader header = packet.header();
      uint64_t actual = header.payload();
      if (actual == (uint64_t) length) {
         LOG(INFO) << "Valid packet length: "<< actual << " bytes";
         Index payload = packet.payload();
         LOG(INFO) << "Index has " << payload.entry_size() << " entries";
         for (int i = 0; i < payload.entry_size(); i++) {
            const IndexEntry& entry = payload.entry(i);
            LOG(INFO) << "Entry: " << entry.path() << " (" << entry.index() << "), " << entry.key() << ", " <<
               entry.probability();
            // TODO: Push to elastic search.
            esClient->process(entry.path(), entry.index(),
               entry.key(), entry.probability());
         }
      }
   }
}

void Indexer::onPeerDisconnect (client_ctxt *client, short what) {
   LOG(INFO) << "onPeerDisconnect";
}

int Indexer::start() {
      server->OnNewMessage(Indexer::_onNewMessage, this);
      server->OnNewConnection(Indexer::_onNewConnection, this);
      server->OnPeerDisconnect(Indexer::_onPeerDisconnect, this);
      return server->start();
}
