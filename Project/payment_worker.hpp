#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#include <windows.h>

#include <queue>

// Runnable-style interface: the worker thread invokes run().
class Runnable {
public:
  virtual ~Runnable() {}
  virtual void run() = 0;
};

// One background worker processes simulated payment jobs from a synchronized
// queue. The worker owns a separate SQLite connection.
class PaymentWorker : public Runnable {
public:
  PaymentWorker();
  ~PaymentWorker();

  PaymentWorker(const PaymentWorker &) = delete;
  PaymentWorker &operator=(const PaymentWorker &) = delete;

  void start();
  void enqueue(int paymentId);
  void shutdown();
  void run() override;

private:
  static unsigned __stdcall threadEntry(void *context);
  void processPayment(void *database, int paymentId);

  std::queue<int> pendingPayments_;
  CRITICAL_SECTION mutex_;
  HANDLE wakeEvent_;
  HANDLE thread_;
  bool started_;
  bool stopping_;
};
