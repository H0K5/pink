// Copyright (c) 2015-present, Qihoo, Inc.  All rights reserved.
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree. An additional grant
// of patent rights can be found in the PATENTS file in the same directory.

#ifndef INCLUDE_HOLY_THREAD_H_
#define INCLUDE_HOLY_THREAD_H_

#include <map>
#include <set>
#include <string>
#include <atomic>

#include "slash/include/xdebug.h"
#include "slash/include/slash_mutex.h"
#include "pink/include/server_thread.h"
#include "pink/include/pink_conn.h"

namespace pink {
class PinkConn;

class HolyThread: public ServerThread {
 public:
  // This type thread thread will listen and work self list redis thread
  HolyThread(int port, ConnFactory* conn_factory,
             int cron_interval = 0, const ServerHandle* handle = nullptr);
  HolyThread(const std::string& bind_ip, int port,
             ConnFactory* conn_factory,
             int cron_interval = 0, const ServerHandle* handle = nullptr);
  HolyThread(const std::set<std::string>& bind_ips, int port, 
             ConnFactory* conn_factory,
             int cron_interval = 0, const ServerHandle* handle = nullptr);
  virtual ~HolyThread();

  virtual int StartThread() override;

  virtual int StopThread() override;

  virtual void set_keepalive_timeout(int timeout) override {
    keepalive_timeout_ = timeout;
  }

  virtual int conn_num() override {
    slash::ReadLock l(&rwlock_);
    return conns_.size();
  }

  virtual void KillAllConns() override;

  virtual void KillConn(const std::string& ip_port) override;

 private:
  /*
   *  For external statistics
   */
  slash::RWMutex rwlock_;
  std::map<int, PinkConn*> conns_;

  ConnFactory *conn_factory_;
  void* private_data_;

  std::atomic<int> keepalive_timeout_; // keepalive second

  void DoCronTask() override;

  void HandleNewConn(int connfd, const std::string &ip_port) override;
  void HandleConnEvent(PinkFiredEvent *pfe) override;
};  // class HolyThread


}  // namespace pink

#endif  // INCLUDE_HOLY_THREAD_H_