#pragma once


class JsFunctionDescriptor
{
public:
    JsFunctionDescriptor(){ ; }

    JsFunctionDescriptor(QString guid) :m_funcGuid(guid) { ; }

    virtual ~JsFunctionDescriptor() { ; }

    virtual void invoke(QVariantList argument) { ; }

    QString funcGuid() const { return m_funcGuid; }

private:
    QString m_funcGuid;
};

Q_DECLARE_METATYPE(JsFunctionDescriptor);
