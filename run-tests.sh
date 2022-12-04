#!/bin/sh

ret=0
passed=0
failed=0

green=32
red=31

color () {
  echo -e "\x1b[$2m$1\x1b[0m"
}

for file in test/*; do
  if [ -d "$file" ]; then
    if ./flog $file/main.js | grep -q 'x'; then
      ((failed=failed+1))
      echo -n "failed "
      color $file $red
      ret=1;
    else
      ((passed=passed+1))
      echo -n "passed "
      color $file $green
    fi
  fi
done

echo -e "\ntotals\n======"

echo -n "passed "
color $passed $green
echo -n "failed "
color $failed $red
echo -e "out of $((passed+failed)) tests"

exit $ret
