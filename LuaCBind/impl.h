#ifndef _IMPL_H
#define _IMPL_H

//LUACBIND_MODULE_BEGIN
class Account
{
public:
  Account(double balance)      { m_balance = balance; }
  //LUACBIND_METHOD_BEGIN
  void deposit(double amount)  { m_balance += amount; }
  void withdraw(double amount) { m_balance -= amount; }
  double balance(void)         { return m_balance; }
  //LUACBIND_METHOD_END
private:
  double m_balance;
};
//LUACBIND_MODULE_END

#endif
