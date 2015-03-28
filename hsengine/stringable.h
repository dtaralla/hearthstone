#ifndef STRINGABLE_H
#define STRINGABLE_H

#include <QString>

class Stringable
{
public:
    operator QString() const;
    virtual QString toString() const = 0;
};

#endif // STRINGABLE_H
