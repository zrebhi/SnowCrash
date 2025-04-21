#!/bin/bash

while true; do
  # Switch the symbolic link to /tmp/myfile
  ln -sf /tmp/myfile /tmp/linkfile

  # Run the level10 program
  /home/level10/level10 /tmp/linkfile 127.0.0.1

  # Switch the symbolic link to the token file
  ln -sf /home/level10/token /tmp/linkfile
done
