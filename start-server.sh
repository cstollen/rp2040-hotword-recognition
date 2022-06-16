#! /bin/bash

if [[ ! -f "ssl-server/build/ssl_server" ]]; then
  echo "Building project"
  build-all.sh
fi

ssl-server/build/ssl_server

