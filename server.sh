#! /bin/bash --posix

src_file=$(find ./src/server -name "*.c")
inc_file=$(find ./include -name "*.h")

gcc -o bin/server "./src/app/mysql.c" "./src/app/operation.c" $src_file $inc_file -lmysqlclient
