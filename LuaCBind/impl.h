#ifndef _IMPL_H
#define _IMPL_H

class Account
{
public:
  Account(double balance)      { m_balance = balance; }
  void deposit(double amount)  { m_balance += amount; }
  void withdraw(double amount) { m_balance -= amount; }
  double balance(void)         { return m_balance; }
private:
  double m_balance;
};

#endif