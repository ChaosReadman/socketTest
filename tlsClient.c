#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include <openssl/err.h>
#include <openssl/ssl.h>

int main(int argc, char const *argv[])
{
    const int port_number = 12345;
    int sock              = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("Error: socket()\n");
        exit(EXIT_FAILURE);
    }
    // setup socket
    struct sockaddr_in server;
    server.sin_family      = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port        = htons(port_number);
    // connect to server
    if ((connect(sock, (struct sockaddr *)&server, sizeof(server))) < 0)
    {
        perror("Error: connect()\n");
        exit(EXIT_FAILURE);
    }
    // ssl
    // init ssl
    SSL_library_init();
    SSL_load_error_strings();
    // create ssl context
    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx)
    {
        perror("Error: SSL context\n");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    SSL *ssl = SSL_new(ctx);
    if (!SSL_set_fd(ssl, sock))
    {
        perror("Error: SSL_set_fd()\n");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    // SSL_connect(ssl);
    if (!SSL_connect(ssl))
    {
        perror("Error: SSL_connect()\n");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    // send data to server
    const char *hello = "Hello from client";
    SSL_write(ssl, hello, strlen(hello));
    printf("Message sent to server\n");
    // read data from server
    char buff[1024] = {0};
    SSL_read(ssl, buff, sizeof(buff));
    printf("recived message is \"%s\"\n", buff);
    // close ssl
    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    // close connecting socket
    close(sock);
    return 0;
}