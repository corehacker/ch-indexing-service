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
      CH_IR_INDEXER_INIT_PARAMS_X x_init = {0};

      pal_memmove(&x_args, px_args, sizeof(x_args));

      x_init.ui_token_hm_table_size = 1024;
      x_init.ui_postings_hm_table_size = 1024;
      x_init.ui_max_token_len = 1024;
      x_init.ui_max_filepath_len = 16 * 1024;
      x_init.b_ignore_stopwords = false;
      x_init.b_enable_porter = false;
      ch_ir_indexer_init(&x_init, &px_indexer_ctxt);


      server = new TcpServer (INADDR_ANY, x_args.listenPort);
}

Indexer::~Indexer(){
      // TODO
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
      cout << "New Message" << endl;
      printf ("Read %d bytes from socket\n", length);

      // PacketHeader header;
      printf ("\n\n");
      for (uint32_t i = 0; i < length; i++) {
            printf ("0x%x ", message[i]);
      }
      printf ("\n\n");
      // header.ParseFromArray((const void *) message, length);
      Packet packet;
      packet.ParseFromArray((const void *) message, length);
      if (packet.has_header() && packet.has_payload()) {
            PacketHeader header = packet.header();
            uint64_t actual = header.payload();
            if (actual == (uint64_t) length) {
                  printf ("Valid packet length: %lu bytes\n", actual);
                  Index payload = packet.payload();
                  printf ("Index has %d entries\n", payload.entry_size());
                  for (int i = 0; i < payload.entry_size(); i++) {
                        const IndexEntry& entry = payload.entry(i);
                        cout << "Entry: " << entry.path() << " (" << entry.index() << "), " << entry.key() << ", " <<
                              entry.probability() << endl;
                        ch_ir_indexer_handle_token(px_indexer_ctxt,
                              (uint8_t *) entry.key().data(),
                              (uint8_t *) entry.path().data(),
                              (uint32_t) entry.index(), (entry.probability() * 100));
                  }
            }
            
      }
}

void Indexer::onPeerDisconnect (client_ctxt *client, short what) {
      cout << "onPeerDisconnect" << endl;
}

int Indexer::start() {
      server->OnNewMessage(Indexer::_onNewMessage, this);
      server->OnNewConnection(Indexer::_onNewConnection, this);
      server->OnPeerDisconnect(Indexer::_onPeerDisconnect, this);
      return server->start();
}
