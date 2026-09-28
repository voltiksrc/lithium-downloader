# lithdl
Tiny c++ downloader that utilizes libcurl

# basic usage
lithdl <url>

# dependencies
c++ compiler
libcurl development headers

## install
build:
g++ main.cpp -lcurl -o lithdl
install system wide:
sudo install -m 755 lithdl /usr/local/bin/lithdl
