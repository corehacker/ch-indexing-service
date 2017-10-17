/*
 * indexer-client.hpp
 *
 *  Created on: Oct 16, 2017
 *      Author: corehacker
 */

#ifndef SRC_INDEXER_ES_CLIENT_HPP_
#define SRC_INDEXER_ES_CLIENT_HPP_

using ChCppUtils::ThreadPool;

class EsClient;

typedef struct _EsConnection {
   EsClient *client;
   struct evhttp_connection *evcon;
} EsConnection;

class EsClient {
private:
   ThreadPool *mClientPool;
   struct event_base *mEventBase;

   static void _httpDoneCb(struct evhttp_request *req, void *arg);
   void httpdoneCb(struct evhttp_request *req, EsConnection *connection);
public:
   EsClient();
   ~EsClient();
   void process(std::string path, uint64_t index, std::string key, float probability);
};



#endif /* SRC_INDEXER_ES_CLIENT_HPP_ */
