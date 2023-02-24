# SimpleServerTLS
## certとkeyを作る

`openssl req -newkey rsa:2048 -new -nodes -x509 -days 3650 -keyout key.pem -out cert.pem`

この後、次のように入れた

`Country Name (2 letter code) [AU]:JP`
`State or Province Name (full name) [Some-State]:Chiba`
`Locality Name (eg, city) []:Funabashi`
`Organization Name (eg, company) [Internet Widgits Pty Ltd]:test corp`
`Organizational Unit Name (eg, section) []:test org`
`Common Name (e.g. server FQDN or YOUR name) []:T.OOO`   
`Email Address []:aaa@bbb.com`

## コンパイルオプション
`gcc simpleServer.c -o simpleServer -ldl  -lresolv -lssl -lcrypto`

## OSI階層
OpenSSL は Layer 6(Presentation) で、WinSock は layer 5(Session) だ
ということは、TransparentProxyとしてOpenSSLを動かして、WinsockにAppendLayer的なことをすればいいのか
てことは、TransparentProxyをOpenSSLで動かして、
CAyncSockを改造してプロキシ経由にすればいいのか？
