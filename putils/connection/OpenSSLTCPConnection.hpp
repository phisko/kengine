#pragma once

#include <regex>

#include "putils/Directory.hpp"
#include "putils/RAII.hpp"
#include "openssl/ssl.h"
#include "openssl/err.h"

#include "ATCPConnection.hpp"

namespace putils
{
    class OpenSSLTCPConnection final : public ATCPConnection
    {
    private:
        int doWrite(int, const char *data, int length) noexcept { return SSL_write(_ssl, data, length); }

        int doRead(int, char *dest, int length) noexcept { return SSL_read(_ssl, dest, length); }

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
        OpenSSLTCPConnection(const std::string &host, short port,
                             const std::string &certificate = getCertificate(".pem"),
                             const std::string &key = getCertificate(".key"),
                             bool verbose = false)
                : ATCPConnection(host, port, verbose)
        {
            SSL_library_init();
            SSL_load_error_strings();
            OpenSSL_add_all_algorithms();

            _ssl_ctx = SSL_CTX_new(SSLv23_client_method());
            if (_ssl_ctx == nullptr)
                throw std::runtime_error("Failed to initialize SSL context");

            // Specify certificate to OpenSSL
            if (SSL_CTX_use_certificate_file(_ssl_ctx.get(), certificate.c_str(), SSL_FILETYPE_PEM) != 1)
                throw std::runtime_error("Failed to use certificate " + certificate);
            if (SSL_CTX_use_PrivateKey_file(_ssl_ctx.get(), key.c_str(), SSL_FILETYPE_PEM) != 1)
                throw std::runtime_error("Failed to use private key " + key);

            _ssl = SSL_new(_ssl_ctx);
            SSL_set_fd(_ssl, this->getSocket());

            if (SSL_connect(_ssl) == -1)
                throw std::runtime_error("Failed to SSL connect");
        }

    private:
        putils::RAII<SSL_CTX *> _ssl_ctx{nullptr, [](SSL_CTX *ctx) { SSL_CTX_free(ctx); }};
        putils::RAII<SSL *> _ssl{nullptr, [](SSL *ssl) { SSL_free(ssl); }};
    };
}
