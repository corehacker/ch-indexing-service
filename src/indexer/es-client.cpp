/*
 * indexer-client.cpp
 *
 *  Created on: Oct 16, 2017
 *      Author: corehacker
 */

#include <event2/event.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include <event2/buffer.h>
#include <glog/logging.h>

#include <ch-cpp-utils/base64.h>
#include <ch-cpp-utils/thread-pool.hpp>

#include "es-client.hpp"

using ChCppUtils::base64_encode;

#define VERIFY(cond) do {                       \
   if (!(cond)) {                              \
      fprintf(stderr, "[error] %s\n", #cond); \
   }                                           \
} while (0);

#define URL_MAX 4096

EsClient::EsClient() {
   mClientPool = new ThreadPool(1, false);
   mEventBase = event_base_new();
}

EsClient::~EsClient() {

}

void EsClient::_httpDoneCb(struct evhttp_request *req, void *arg) {
   EsConnection *connection = (EsConnection *) arg;
   connection->client->httpdoneCb(req, connection);
}

void EsClient::httpdoneCb(struct evhttp_request *req, EsConnection *connection) {
   LOG(INFO) << "HTTP Done!!!";

   if (NULL == req) {
      LOG(ERROR) << "Request failed";
   } else {
      LOG(INFO) << "Request success";
      LOG(INFO) << "Response: " << req->response_code << " " << req->response_code_line;
   }
}

void EsClient::process(std::string path, uint64_t index, std::string key, float probability) {
   EsConnection *connection = new EsConnection();
   connection->client = this;
   connection->evcon = evhttp_connection_base_new(mEventBase, NULL, "localhost", 9200);
   struct evhttp_request *req = evhttp_request_new(EsClient::_httpDoneCb, connection);

   std::string authorization = "Basic ";
   std::string user = "elastic:changeme";
   authorization += base64_encode((unsigned char *) user.data(), user.length());

   LOG(INFO) << "Authorization: " << authorization;

   evhttp_add_header(req->output_headers, "Connection", "keep-alive");
   evhttp_add_header(req->output_headers, "Authorization", authorization.data());
   evhttp_add_header(req->output_headers, "Content-Type", "application/json; charset=UTF-8");

   struct evbuffer *output_buffer = evhttp_request_get_output_buffer(req);
   std::string body = "";
   body += "{";
   body += "\"path\":\"" + path + "\",";
//   body += "\"index\":\"" + index + "\",";
   body += "\"key\":\"" + key + "\"";
//   body += "\"probability\":\"" + probability + "\",";
   body += "}";
   LOG(INFO) << "Body: " << body;
   const char *json = body.data();
   evbuffer_add(output_buffer, json, body.length());

   char contentLength[URL_MAX];
   evutil_snprintf(contentLength, sizeof(contentLength) - 1, "%lu", body.length());
   evhttp_add_header(req->output_headers, "Content-Length", contentLength);


   evhttp_make_request(connection->evcon, req, EVHTTP_REQ_PUT, "/images/image/1");

   event_base_dispatch(mEventBase);


}
