#include <QtGlobal>

#ifdef Q_OS_SYMBIAN

extern int __aeabi_uidivmod(unsigned int a, unsigned int b);
extern int __aeabi_idivmod(int a, int b);
int __aeabi_idiv(int a, int b)
{
    return __aeabi_idivmod(a, b);
}

int __aeabi_uidiv(unsigned int a, unsigned int b)
{
    return __aeabi_uidivmod(a, b);
}

#endif
