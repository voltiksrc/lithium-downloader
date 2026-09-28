# lithdl

tiny c++ downloader that uses libcurl

## basic usage

```sh
lithdl https://example.com/file.zip
```

## dependencies

c++ compiler

libcurl development headers

## install

build:

```sh
g++ main.cpp -lcurl -o lithdl
```

install system wide:

```sh
sudo install -m 755 lithdl /usr/local/bin/lithdl
```
