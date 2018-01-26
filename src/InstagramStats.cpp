/*

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

Written By Brian Lough
https://www.youtube.com/user/witnessmenow
https://github.com/witnessmenow
https://twitter.com/witnessmenow

*/

#include "InstagramStats.h"

String currentKey = "";
String currentParent = "";
InstagramUserStats userStatsResponse;

//**************************************************************************//
// This code is the JSON Parser code written by squix78 as part of his example,
// modified for this application //
// https://github.com/squix78/json-streaming-parser //

class InstagramUserListener : public JsonListener {
 public:
  virtual void whitespace(char c);

  virtual void startDocument();

  virtual void key(String key);

  virtual void value(String value);

  virtual void endArray();

  virtual void endObject();

  virtual void endDocument();

  virtual void startArray();

  virtual void startObject();
};

void InstagramUserListener::whitespace(char c) {
  // Serial.println("whitespace");
}

void InstagramUserListener::startDocument() {
  // Serial.println("start document");
}

void InstagramUserListener::key(String key) {
  currentKey = key;
}

void InstagramUserListener::value(String value) {
  if (currentParent == "followed_by") {
    if (currentKey == "count") {
      userStatsResponse.followedByCount = value.toInt();
    }
  }
}

void InstagramUserListener::endArray() {
  // Serial.println("end array. ");
}

void InstagramUserListener::endObject() {
  currentParent = "";
  // Serial.println("end object. ");
}

void InstagramUserListener::endDocument() {
  // Serial.println("end document. ");
}

void InstagramUserListener::startArray() {
  // Serial.println("start array. ");
}

void InstagramUserListener::startObject() {
  currentParent = currentKey;
  // Serial.println("start object. ");
}
//*********** END of Json Parser code **************//


InstagramStats::InstagramStats(Client& client) {
  this->client = &client;
}

InstagramUserStats InstagramStats::getUserStats(String user) {
  Serial.println("starting getUserStats function");
  JsonStreamingParser parser;
  InstagramUserListener listener;
  userStatsResponse = InstagramUserStats();
  parser.setListener(&listener);

  long now;
  // Connect to Instagram over ssl
  if (connectClient()) {
    Serial.println(".... connected to server");

    client->println("GET /" + user + "/?__a=1 HTTP/1.1");
    client->print("Host:"); client->println(INSTA_HOST);
    client->println();

    now = millis();
    while (millis() - now < 3000) {
      while (client->available()) {
        char c = client->read();
        // parsing code:
        // most of the work happens in the header code
        // at the top of this file
        parser.parse(c);

        // This is the check to see if you have everything you connected
        // as the library is expanded to include more info this will connected
        // to change.
        if (userStatsResponse.followedByCount > 0) {
          // Serial.println("finished");
          closeClient();
          return userStatsResponse;
        }
      }
    }
  }
  closeClient();
  return userStatsResponse;
}

bool InstagramStats::connectClient() {
  if(!client->connected()){
    return client->connect(INSTA_HOST, INSTA_SSL_PORT);
  } else {
    // Already Connected
    return true;
  }
}

void InstagramStats::closeClient() {
  if(client->connected()){
    client->stop();
  }
}