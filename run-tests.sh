#!/bin/sh

for file in test/*; do
  if [ -d "$file" ]; then
    ./flog $file/main.js
  fi
done
