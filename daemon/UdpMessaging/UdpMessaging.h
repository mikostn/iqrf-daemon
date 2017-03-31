/**
 * Copyright 2016-2017 MICRORISC s.r.o.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "UdpMessagingTransaction.h"
#include "UdpChannel.h"
#include "IMessaging.h"
#include "TaskQueue.h"
#include "WatchDog.h"
#include <string>
#include <atomic>

class MessagingController;

class UdpMessaging : public IMessaging
{
public:
  enum class Mode {
    Operational,
    Service,
    Forwarding
  };

  UdpMessaging() = delete;
  UdpMessaging(MessagingController* messagingController);
  virtual ~UdpMessaging();

  // component
  void start() override;
  void stop() override;
  void update(const rapidjson::Value& cfg) override;
  const std::string& getName() const override { return m_name; }

  // interface
  void registerMessageHandler(MessageHandlerFunc hndl) override;
  void unregisterMessageHandler() override;
  void sendMessage(const ustring& msg) override;

  void sendDpaMessageToUdp(const DpaMessage&  dpaMessage);

  void getGwIdent(ustring& message);
  void getGwStatus(ustring& message);

  int handleMessageFromUdp(const ustring& udpMessage);
  void encodeMessageUdp(ustring& udpMessage, const ustring& message = ustring());
  void decodeMessageUdp(const ustring& udpMessage, ustring& message);

  UdpMessaging::Mode parseMode(const std::string& mode);

  Mode getMode() { return m_mode; };

private:
  void setMode(Mode mode);
  
  //void setExclusiveAccess();
  //void resetExclusiveAccess();

  //std::atomic_bool m_exclusiveAccess;
  
  WatchDog<std::function<void()>> m_watchDog;

  MessagingController *m_messagingController = nullptr;
  UdpMessagingTransaction* m_transaction = nullptr;

  UdpChannel *m_udpChannel;
  TaskQueue<ustring> *m_toUdpMessageQueue;

  // configuration
  std::string m_name;
  unsigned m_remotePort = 55000;
  unsigned m_localPort = 55300;

  std::atomic<Mode> m_mode;
  unsigned m_timeout = 30000;
};
