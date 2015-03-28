#ifndef VPTR_H
#define VPTR_H

#include <QVariant>

template<class T> class VPtr
{
public:
    static T* AsPtr(const QVariant& v);
    static QVariant AsQVariant(T* ptr);
    static QVariant AsQVariant(const T* ptr);

    ~VPtr();

private:
    VPtr();
};


template<class T> T* VPtr<T>::AsPtr(const QVariant& v)
{
    return (T*) v.value<void*>();
}

template<class T> QVariant VPtr<T>::AsQVariant(T* ptr)
{
    return qVariantFromValue((void*) ptr);
}

template<class T> QVariant VPtr<T>::AsQVariant(const T* ptr)
{
    return qVariantFromValue((void*) ptr);
}

template<class T> VPtr<T>::~VPtr() { }

template<class T> VPtr<T>::VPtr() { }

#endif // VPTR_H
