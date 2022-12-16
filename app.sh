#! /bin/bash --posix

src_file=$(find ./src/app -name "*.c")
inc_file=$(find ./include -name "*.h")

gcc -o bin/app $src_file $inc_file -lmysqlclient
