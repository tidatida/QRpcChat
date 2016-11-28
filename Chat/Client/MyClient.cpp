#include "stdafx.h"
#include "MyClient.h"
#include "ServerEmulated.h"
#include "Message.h"

MyClient::MyClient(QObject* parent)
    : Client(parent) //
    , m_serverEmulated(nullptr)
{
}

void MyClient::ConnectToServer(const QString& host, int port)
{
    m_serverEmulated = GetServerEmulated(port, host, this, this);
    connect(this, SIGNAL(SendToServer(int, const QTime&, const QString&)), m_serverEmulated, SLOT(OnNewMsg(int, const QTime&, const QString&)), Qt::AutoConnection);
    connect(this, SIGNAL(Registrate(const QString&)), m_serverEmulated, SLOT(OnRegistration(const QString&)), Qt::AutoConnection);
    connect(m_serverEmulated, SIGNAL(OnRegistration_signal(QString)), this, SLOT(OnRegistrationResponse(int, const QString&)), Qt::AutoConnection);
    connect(this, SIGNAL(Update()), m_serverEmulated, SLOT(OnUpdate()), Qt::AutoConnection);
    connect(m_serverEmulated, SIGNAL(OnUpdate_signal()), this, SLOT(OnReceivedMsgHistory(QByteArray)), Qt::AutoConnection);
}

void MyClient::OnSendMsg(const QString& msg)
{
    QTime time(QTime::currentTime());
    emit SendToServer(m_id, time, msg);
}

void MyClient::OnRegistrationResponse(int id, const QString& name)
{
    if (id != 0 && id != -1)
    {
        m_id = id;
        emit RegistrationSuccessful(id, name);
    }
    else
    {
        emit RegistrationFailed();
    }
}

void MyClient::OnReceivedMsgHistory(QByteArray history) //CustomContainer<Message> msgs
{
    QDataStream inStream(&history, QIODevice::ReadOnly);
    inStream.setVersion(QDataStream::Qt_5_7);

    int size;
    inStream >> size;
    m_history.clear();
    for (int i = 0; i < size; ++i)
    {
        Message msg;
        inStream >> msg;
        m_history.push_back(msg);
    }
    UpdateHistory(m_history);
}

void MyClient::OnUpdateHistory()
{
    emit Update();
}