#!/bin/sh

cat - \
  | grep -oE "^[0-9A-Fa-f]+:\s*[0-9A-Fa-f]+\s*(\S+)(\s|$)" \
  | awk '{ print $3 }' \
  | sort | uniq -c | sort -n
