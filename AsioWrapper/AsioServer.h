#pragma once
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "AsioSocket.h"

namespace AsioWrapper
{
    class ASIOServer
    {
    public:
        ASIOServer(uint16_t port) :
              mPort(port)
            , mEndPoint(boost::asio::ip::tcp::v4(), port)
            , mAcceptor(mIoService,mEndPoint)
            , mDeadline(mIoService)

        {
            
        }


        void CloselistenSocket()
        {
            mAcceptor.close();
        }

        AsioSocket* AcceptSyncBlocking()
        {
            boost::asio::ip::tcp::socket* socket = new boost::asio::ip::tcp::socket(mIoService);

            boost::system::error_code ec = boost::asio::error::would_block;
            mAcceptor.accept(*socket,ec);
            return new AsioSocket(mIoService, socket);
        }

        void OnAccept(const boost::system::error_code error)
        {
            mDeadline.cancel();
            mIoService.run_one();
            mLastError = error;
        }

        void OnDoneWaiting(const boost::system::error_code error)
        {
            //if timeout has reached - assign timeout error code.
            if (!error)
                mLastError = boost::asio::error::timed_out;
        }

        AsioSocketPtr AcceptAsyncWithDeadLine(uint32_t timeOut = 0)
        {
            mDeadline.expires_from_now(boost::posix_time::millisec(timeOut));
            boost::asio::ip::tcp::socket* socket = new boost::asio::ip::tcp::socket(mIoService);

            mAcceptor.async_accept( 
                *socket
                , boost::bind(&ASIOServer::OnAccept, this, _1));
            
            mDeadline.async_wait(boost::bind(&ASIOServer::OnDoneWaiting, this, _1));
            mIoService.run_one();
            mIoService.reset();

            if (mLastError)
            {
                boost::system::error_code ignored_ec;
                socket->close(ignored_ec);
                delete socket;
                socket = nullptr;
            }
            return (AsioSocketPtr)(new AsioSocket(mIoService, socket));
        }


        AsioSocketPtr AcceptSync(uint32_t timeOut = 0)
        {
            return timeOut == 0 ? AcceptSyncBlocking() : AcceptAsyncWithDeadLine(timeOut);
        }

    private:
        // warning!, Order of member fields does matter.
        boost::asio::io_service mIoService;
        uint16_t mPort;
        boost::asio::ip::tcp::endpoint mEndPoint;
        boost::asio::ip::tcp::acceptor mAcceptor;
        boost::asio::deadline_timer mDeadline;
        boost::system::error_code mLastError;;
    };
}
