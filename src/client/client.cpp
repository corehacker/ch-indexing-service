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
 * \file   client.cpp
 *
 * \author Sandeep Prakash
 *
 * \date   Aug 20, 2017
 *
 * \brief
 *
 ******************************************************************************/

#include <ch-pal/exp_pal.h>
#include <ch-utils/exp_sock_utils.h>

#include <ch-protos/packet.pb.h>
#include <ch-protos/communication.pb.h>

using indexer::PacketHeader;
using indexer::Packet;
using indexer::Index;
using indexer::IndexEntry;


int main () {
    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // PAL_RET_E e_pal_ret = ePAL_RET_FAILURE;
    PAL_SOCK_HDL hl_sock_hdl = NULL;
    SOCK_UTIL_HOST_INFO_X x_host_info = {0};

    x_host_info.ui_bitmask |= eSOCK_UTIL_HOST_INFO_DNS_NAME_BM;
    x_host_info.ui_bitmask |= eSOCK_UTIL_HOST_INFO_HOST_PORT_BM;
    x_host_info.puc_dns_name_str = (uint8_t *) "127.0.0.1";
    x_host_info.us_host_port_ho = 8888;

    tcp_connect_sock_create (&hl_sock_hdl, &x_host_info, 1000);

    Packet packet;
    PacketHeader *header = packet.mutable_header();
    header->set_payload(0);
    Index *payload = packet.mutable_payload();

    printf ("Payload? %d\n", packet.has_payload());

    IndexEntry *entry = payload->add_entry();
    entry->set_path("file.jpg");
    entry->set_index(0);
    entry->set_key("panda");
    entry->set_probability(0.9);

    entry = payload->add_entry();
    entry->set_path("file1.jpg");
    entry->set_index(1);
    entry->set_key("lion");
    entry->set_probability(0.8);

    printf ("Num of entries: %d\n", payload->entry_size());

    int size = packet.ByteSize();
    printf ("Size before: %d\n", size);

    header->set_payload(size);

    printf ("payload? %d\n", header->has_payload());
    size = packet.ByteSize();
    printf ("Size after: %d\n", size);

    uint8_t *buffer = (uint8_t *) malloc(size);
    memset(buffer, 0x00, size);
    packet.SerializeToArray(buffer, size);

    printf ("\n\n");
    for (int i = 0; i < size; i++) {
          printf ("0x%x ", buffer[i]);
    }
    printf ("\n\n");

    pal_sock_send (hl_sock_hdl,
        (uint8_t *) buffer,
        (uint32_t *) &size,
        0);

    // Optional:  Delete all global objects allocated by libprotobuf.
    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}

/*
client:
0xa 0x2 0x8 0x36 0x12 0x30 0xa 0x16 0xa 0x8 0x66 0x69 0x6c 0x65 0x2e 0x6a 0x70 0x67 0x12 0x5 0x70 0x61 0x6e 0x64 0x61 0x1d 0x66 0x66 0x66 0x3f 0xa 0x16 0xa 0x9 0x66 0x69 0x6c 0x65 0x31 0x2e 0x6a 0x70 0x67 0x12 0x4 0x6c 0x69 0x6f 0x6e 0x1d 0xcd 0xcc 0x4c 0x3f

server:
0xa 0x2 0x8 0x36 0x12 0x30 0xa 0x16 0xa 0x8 0x66 0x69 0x6c 0x65 0x2e 0x6a 0x70 0x67 0x12 0x5 0x70 0x61 0x6e 0x64 0x61 0x1d 0x66 0x66 0x66 0x3f 0xa 0x16 0xa 0x9 0x66 0x69 0x6c 0x65 0x31 0x2e 0x6a 0x70 0x67 0x12 0x4 0x6c 0x69 0x6f 0x6e 0x1d 0xcd 0xcc 0x4c 0x3f
*/
