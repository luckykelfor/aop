#include "multi_aspects_aop.h"
// Test Aspect.
/**
 * LogAspect with overloaded ctors
 */
class LogAspect : public aop::BaseAspect
{
public:
    LogAspect(double timestamp, const std::string &name) : m_name_(name), m_timestamp_(timestamp) {}
    LogAspect() : LogAspect(0.0, "NULL") {}
    template <typename... Args>
    void Before(Args &&...)
    {
        std::cout << "[AOP][LogAspect-" << m_name_ << "] Log before calling.  Calling time is " << m_timestamp_ << "\n";
    }
    template <typename... Args>
    void After(Args &&...)
    {
        std::cout << "[AOP][LogAspect-" << m_name_ << "] Log after calling.\n";
    }

private:
    std::string m_name_;
    double m_timestamp_;
};

/*
 * Test VerifyAspect
 */
class VerifyAspect : public aop::BaseAspect
{
public:
    void Before(int a)
    {
        std::cout << "[AOP][VerifyAspect] Verify before start. a is " << a << "\n";
    }
};

// normal funtion
void DoSomething(int a)
{
    std::cout << "Do something imoprtant. a is " << a << "\n";
}

// test class
class AAA
{
public:
    void TestA(int a) { std::cout << "This is testA. a is " << a << "\n"; }
};

int main()
{
    int a = 0;
    using namespace aop;
    Invoke<LogAspect, VerifyAspect>(LogAspect(1000.0, "LogA"), VerifyAspect(), DoSomething, 54321);
    Invoke<LogAspect>(LogAspect(1000.5, "LogB"), DoSomething, 12345);
    AAA aaa;
    Invoke<LogAspect, VerifyAspect>(LogAspect(), VerifyAspect(), &AAA::TestA, &aaa, 19);
    return 0;
}