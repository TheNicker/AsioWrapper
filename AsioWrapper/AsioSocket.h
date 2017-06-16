#pragma once
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

namespace AsioWrapper
{
    class AsioSocket
    {
    public:
        AsioSocket(boost::asio::io_service& aIoService) :
            mIoService(aIoService)
        {
            
        }

        AsioSocket(boost::asio::io_service& aIoService, boost::asio::ip::tcp::socket* socket) :
              mIoService(aIoService)
            , mSocket(socket)
        {

        }

        ~AsioSocket()
        {
            if (mSocket != nullptr)
            {
                mSocket->close();
                delete mSocket;
            }
        }

        void Close()
        {
            mSocket->close();
        }

        int32_t Connect(const std::string& host, uint16_t port)
        {
            boost::asio::ip::tcp::resolver mResolver(mIoService);
            boost::asio::ip::tcp::resolver::query query(host, std::to_string(port));
            boost::asio::ip::tcp::resolver::iterator endpoint_iterator = mResolver.resolve(query);

            boost::system::error_code ec = boost::asio::error::would_block;
            uint32_t attemptNo = 0;
            if (mSocket == nullptr)
                mSocket = new boost::asio::ip::tcp::socket(mIoService);

            while (ec)
            {
                connect(*mSocket, endpoint_iterator, ec);

                if (ec.value() == boost::asio::error::connection_refused)
                {
                    // connection refused, can not access port in server.
                    return -1;

                }

                attemptNo++;
            }
        }


        int32_t GetAvailableBytesToRead()
        {
            try
            {
                return mSocket->available();
            }
            catch (boost::system::system_error exc)
            {
                return -1;
            }
        }

        int32_t Read(void* buffer, int32_t size)
        {
            try
            {
                return boost::asio::read(*mSocket, boost::asio::buffer(buffer, size));

            }
            catch (boost::system::system_error exc)
            {
                return -1;
            }
        }
        int32_t Write(void* buffer, int32_t size)
        {
            try
            {
                return boost::asio::write(*mSocket, boost::asio::buffer(buffer, size));
            }
            catch (boost::system::system_error exc)
            {
                return -1;
            }
        }

        void SetSocketDelay(bool delay)
        {
            mSocket->set_option(boost::asio::ip::tcp::no_delay(!delay));
        }

        void SetReuseAddress(bool reuse)
        {
            mSocket->set_option(boost::asio::ip::tcp::socket::reuse_address(reuse));
        }

        bool Isopen() const
        {
            return mSocket != nullptr && mSocket->is_open();
        }

        
    private:
        boost::asio::ip::tcp::socket* mSocket;
        boost::asio::io_service& mIoService;
    };

    using AsioSocketPtr = AsioSocket*;
}