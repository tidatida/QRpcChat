#pragma once
#include "ClientEmulated.h"

class QObject;
namespace qRpc
{
    class Server
    {
    public:
        Server();
        virtual ~Server(){}

    protected:
        void ListenRemoteClient(int port, QObject* realServer);
        template<typename... Targs>
        void EmitUniversalSignal(Targs... args)
        {
            if (m_remoteClient != nullptr)
            {
                m_remoteClient->EmitToRealClient(args...);
            }
            else
            {
                assert(!"Real server must to call SetClientEmulated before EmitUniversalSignal");
            }
        }

    private:
        ClientEmulated* m_remoteClient;
    };
}
