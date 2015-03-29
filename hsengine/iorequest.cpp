#include "iorequest.h"
#include <QtDebug>
#include "event.h"
#include "actions/action.h"

IORequest::IORequest(Player* source, IORequest::RequestType type, int refCount) :
    QObject(NULL),
    mRefCount(refCount),
    m_source(source),
    m_type(type),
    m_response(NULL)
{
}

IORequest::~IORequest()
{
}

IORequest::RequestType IORequest::type() const
{
    return m_type;
}

void IORequest::putExtra(const QString& key, const QVariant& value)
{
    m_extras.insert(key, value);
}

QVariant IORequest::extra(const QString& key) const
{
    Q_ASSERT(m_extras.contains(key));
    return m_extras.value(key);
}

void IORequest::setResponse(void* r)
{
    m_response = r;
}

void* IORequest::response() const
{
    return m_response;
}

void IORequest::clearRef()
{
    if (--mRefCount <= 0)
        delete this;
}

