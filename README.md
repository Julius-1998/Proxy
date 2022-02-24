# HTTP Caching Proxy(C++)
## By Zian Wang(zw142) and Sijie Zhou(sz232)
 
------------USAGE---------------

$ sudo docker-compose up

Then set your system's proxy address to your server address, and port to 12345.

\----------------------------------

## WARNING: We used a thread pool for concurrency so when using this proxy, please DO NOT open two many tabs in your brower at a time, you may experience a high latency when you have 20+ tabs open.


TODO:


- [X] chunked
- [X] in-cache test
- [X] log CONNECT
- [X] log overwrite
- [X] error handling/try-catch exception
- [X] seg fault
- [X] keep-alive(proxy-connection)
- [X] test CONNECT
- [X] test POST
- [X] cache response errorMsg

