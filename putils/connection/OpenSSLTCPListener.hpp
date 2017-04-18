#pragma once

#include <regex>
#include <iostream>

#include "Directory.hpp"

#include "openssl/ssl.h"
#include "openssl/err.h"

#include "ATCPListener.hpp"
#include "RAII.hpp"

namespace putils
{
    class OpenSSLTCPListener final : public ATCPListener
    {
    private:
        static std::string getCertificate(std::string &&extension)
        {
            Directory dir("certificates");
            std::regex reg("^.*\\" + extension);
            std::string ret = "";

            dir.for_each([&reg, &ret](const putils::Directory::File &f)
            {
                if (std::regex_match(f.fullPath, reg))
                    ret = f.fullPath;
            });

            return ret;
        }

    public:
        OpenSSLTCPListener(short port, const std::string &host = "127.0.0.1",
                           const std::string &certificate = getCertificate(".pem"),
                           const std::string &key = getCertificate(".key"),
                           bool verbose = false)
                : ATCPListener(port, host, verbose)
        {
            // Initialize OpenSSL
            SSL_library_init();
            SSL_load_error_strings();
            OpenSSL_add_all_algorithms();

            _ssl_ctx = SSL_CTX_new(SSLv23_server_method());
            if (_ssl_ctx == nullptr)
                throw std::runtime_error("Failed to initialize SSL context");

            // Specify certificate to OpenSSL
            if (SSL_CTX_use_certificate_file(_ssl_ctx.get(), certificate.c_str(), SSL_FILETYPE_PEM) != 1)
                throw std::runtime_error("Failed to use certificate " + certificate);
            if (SSL_CTX_use_PrivateKey_file(_ssl_ctx.get(), key.c_str(), SSL_FILETYPE_PEM) != 1)
                throw std::runtime_error("Failed to use private key " + key);
        }

        ~OpenSSLTCPListener() { EVP_cleanup(); }

    private:
        static SSL *init_ssl(SSL_CTX *ctx, int fd)
        {
            SSL *ssl = SSL_new(ctx);
            if (ssl == nullptr)
            {
                std::cerr << "[OpenSSL] Failed to SSL_new" << std::endl;
                return nullptr;
            }
            SSL_set_fd(ssl, fd);
            return ssl;
        }

        bool doAccept(int fd) noexcept
        {
            auto ssl = init_ssl(_ssl_ctx, fd);
            if (ssl == nullptr)
            {
                std::cerr << "[OpenSSL] Failed to SSL init" << std::endl;
                return false;
            }

            _ssls.emplace(fd, putils::RAII<SSL *>(ssl, [](SSL *ssl) { SSL_free(ssl); }));
            if (SSL_accept(ssl) <= 0)
            {
                std::cerr << "[OpenSSL] Failed to SSL accept" << std::endl;
                ERR_print_errors_fp(stderr);
                _ssls.erase(fd);
                return false;
            }

            return true;
        }

    private:
        int doWrite(int fd, const char *data, size_t length) noexcept { return SSL_write(_ssls.at(fd), data, length); }

        int doRead(int fd, char *buff, size_t length) noexcept { return SSL_read(_ssls.at(fd), buff, length); }

        void doRemove(int fd) noexcept
        {
            _ssls.erase(fd);
            if (this->isVerbose() == true)
                std::cout << "[OpenSSL] Client disconnected" << std::endl;
        }

    private:
        putils::RAII<SSL_CTX *> _ssl_ctx{nullptr, [](SSL_CTX *ctx) { SSL_CTX_free(ctx); }};
        std::unordered_map<int, putils::RAII<SSL *>> _ssls;      // Maps sockets to their SSL object. SSL object closes the socket
    };
}
