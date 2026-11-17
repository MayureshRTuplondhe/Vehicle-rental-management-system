#include "payment_worker.hpp"

#include <process.h>
#include <sqlite3.h>

#include <iostream>
#include <stdexcept>
#include <string>

PaymentWorker::PaymentWorker()
    : wakeEvent_(NULL), thread_(NULL), started_(false), stopping_(false) {
  InitializeCriticalSection(&mutex_);
  wakeEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
  if (!wakeEvent_) {
    DeleteCriticalSection(&mutex_);
    throw std::runtime_error("Could not create the payment worker event.");
  }
}

PaymentWorker::~PaymentWorker() {
  shutdown();
  CloseHandle(wakeEvent_);
  DeleteCriticalSection(&mutex_);
}

void PaymentWorker::start() {
  EnterCriticalSection(&mutex_);
  if (started_) {
    LeaveCriticalSection(&mutex_);
    return;
  }
  stopping_ = false;
  // _beginthreadex starts a Windows worker whose entry point can use the C++
  // runtime.
  uintptr_t handle = _beginthreadex(NULL, 0, threadEntry, this, 0, NULL);
  if (handle == 0) {
    LeaveCriticalSection(&mutex_);
    throw std::runtime_error("Could not start the payment worker thread.");
  }
  thread_ = reinterpret_cast<HANDLE>(handle);
  started_ = true;
  LeaveCriticalSection(&mutex_);
}

void PaymentWorker::enqueue(int paymentId) {
  EnterCriticalSection(&mutex_);
  if (!started_ || stopping_) {
    LeaveCriticalSection(&mutex_);
    throw std::runtime_error("Payment processor is not accepting requests.");
  }
  pendingPayments_.push(paymentId);
  LeaveCriticalSection(&mutex_);
  SetEvent(wakeEvent_);
}

void PaymentWorker::shutdown() {
  EnterCriticalSection(&mutex_);
  if (!started_) {
    LeaveCriticalSection(&mutex_);
    return;
  }
  stopping_ = true;
  HANDLE thread = thread_;
  LeaveCriticalSection(&mutex_);
  SetEvent(wakeEvent_);

  // WaitForSingleObject is the Windows equivalent of joining this worker.
  WaitForSingleObject(thread, INFINITE);
  CloseHandle(thread);

  EnterCriticalSection(&mutex_);
  thread_ = NULL;
  started_ = false;
  LeaveCriticalSection(&mutex_);
}

unsigned __stdcall PaymentWorker::threadEntry(void *context) {
  static_cast<PaymentWorker *>(context)->run();
  return 0;
}

void PaymentWorker::run() {
  sqlite3 *database = NULL;
  if (sqlite3_open("rental.db", &database) != SQLITE_OK) {
    std::cerr << "Payment worker could not open rental.db.\n";
    if (database)
      sqlite3_close(database);
    return;
  }
  sqlite3_busy_timeout(database, 5000);
  sqlite3_exec(database, "PRAGMA foreign_keys = ON;", NULL, NULL, NULL);

  while (true) {
    int paymentId = 0;
    EnterCriticalSection(&mutex_);
    while (pendingPayments_.empty() && !stopping_) {
      LeaveCriticalSection(&mutex_);
      WaitForSingleObject(wakeEvent_, INFINITE);
      EnterCriticalSection(&mutex_);
    }
    if (pendingPayments_.empty() && stopping_) {
      LeaveCriticalSection(&mutex_);
      break;
    }
    paymentId = pendingPayments_.front();
    pendingPayments_.pop();
    LeaveCriticalSection(&mutex_);

    processPayment(database, paymentId);
  }
  sqlite3_close(database);
}

void PaymentWorker::processPayment(void *databaseHandle, int paymentId) {
  sqlite3 *database = static_cast<sqlite3 *>(databaseHandle);
  // Simulates waiting for a payment provider without blocking the HTTP request.
  Sleep(1200);

  sqlite3_stmt *statement = NULL;
  const char *sql = "UPDATE payments SET status='PAID', transaction_ref=? "
                    "WHERE id=? AND status='PENDING'";
  int result = sqlite3_prepare_v2(database, sql, -1, &statement, NULL);
  if (result == SQLITE_OK) {
    const std::string reference = "DEMO-PAY-" + std::to_string(paymentId);
    sqlite3_bind_text(statement, 1, reference.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(statement, 2, paymentId);
    result = sqlite3_step(statement);
  }
  sqlite3_finalize(statement);

  if (result != SQLITE_DONE || sqlite3_changes(database) == 0) {
    sqlite3_stmt *failed = NULL;
    if (sqlite3_prepare_v2(database,
                           "UPDATE payments SET status='FAILED' WHERE id=? "
                           "AND status='PENDING'",
                           -1, &failed, NULL) == SQLITE_OK) {
      sqlite3_bind_int(failed, 1, paymentId);
      sqlite3_step(failed);
    }
    sqlite3_finalize(failed);
  }
}
