#include "stringable.h"


Stringable::operator QString() const
{
    return toString();
}
