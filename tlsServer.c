#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <openssl/err.h>
#include <openssl/ssl.h>

int main(int argc, char const *argv[])
{
    const int port_number = 12345;
    // create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error: socket()\n");
        exit(EXIT_FAILURE);
    }
    // setup socket
    struct sockaddr_in server;
    int addrlen            = sizeof(server);
    server.sin_family      = AF_INET;
    server.sin_addr.s_addr = inet_addr("0.0.0.0");  // or INADDR_ANY
    server.sin_port        = htons(port_number);
    int opt                = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("Error: setsockopt()");
        exit(EXIT_FAILURE);
    }
    if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Error: bind()");
        exit(EXIT_FAILURE);
    }
    // start to listen requests
    if (listen(sockfd, SOMAXCONN) < 0)
    {
        perror("Error: listen");
        exit(EXIT_FAILURE);
    }

    // ssl
    // init ssl
    SSL_library_init();
    SSL_load_error_strings();
    // create ssl context
    SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());
    if (!ctx)
    {
        perror("Error: SSL context\n");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    // load crt
    if (!SSL_CTX_use_certificate_file(ctx, "cert.pem", SSL_FILETYPE_PEM))
    {
        perror("Error: SSL_CTX_use_certificate_file()\n");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    // load private key
    if (!SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM))
    {
        perror("Error: SSL_CTX_use_PrivateKey_file()\n");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    // accept request from client
    struct sockaddr_in client;
    int client_len = sizeof(client);
    int sock       = accept(sockfd, (struct sockaddr *)&client, (socklen_t *)&client_len);
    if (sock < 0)
    {
        perror("Error: accept()");
        exit(EXIT_FAILURE);
    }
    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);
    if (!SSL_accept(ssl))
    {
        perror("Error: SSL_accept()");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    // read data from client
    char buff[1024] = {0};
    SSL_read(ssl, buff, sizeof(buff));
    printf("recived message is \"%s\"\n", buff);
    // send data to client
    char *hello = "Hello from server";
    SSL_write(ssl, hello, strlen(hello));
    printf("Message sent to client\n");
    // close ssl
    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    // close connecting socket
    close(sock);
    // close listening socket
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
    return 0;
}