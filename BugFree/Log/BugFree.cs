2016/4/26
    1. obs 如何capture以及stream的。
    hVideoEvent

2016/4/27
    znote astyle -A1 -R -Z -z1 -n *.cpp 
    1. rtmpdump 例子
    Completed handshake with rtmp://127.0.0.1:1935/live in 3 ms.
    $ rtmpdump -r rtmp://example.com/path/to/video -o downloaded_file.mp4
    2. 看下 cp 上的 stream player
    3. 看下 rtmpdump 
    ".\..\OpenSSL-Win32\include";.\..\zlib\include;E:\rapidsvn\rtmpdump\stdint;E:\spring_git\spring\vclibs\boost\include;
    4. 使用 rtmpdump 时，至少要指定 fms 以及 app 。
    默认使用 127.0.0.1/live，可指定 app 为 demo 。 由输出时指定。
    5. 在得到 stream 之后如何分离V/A。
    分离VA后decode v，得到每一个frame。
    