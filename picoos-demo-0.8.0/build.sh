#! /bin/bash

#PICOOS=picoos-1.0.4
PICOOS=picoos-0.8.0

mkdir -p ./$PICOOS/contrib/demos/6502-c64/snakes
mkdir -p ./$PICOOS/picoos/out/6502/rel

cp noscfg.h ./$PICOOS/contrib/demos/6502-c64/snakes
cp poscfg.h ./$PICOOS/contrib/demos/6502-c64/snakes
cp snakes.c ./$PICOOS/contrib/demos/6502-c64/snakes
cp Makefile ./$PICOOS/contrib/demos/6502-c64/snakes

echo "cleaning"
make -C ./$PICOOS/picoos/ clean
make -C ./$PICOOS/contrib/demos/6502-c64/snakes clean

echo "cleaning with PORT=6502"
make -C ./$PICOOS/picoos/ PORT=6502 clean
make -C ./$PICOOS/contrib/demos/6502-c64/snakes PORT=6502 clean

echo "making picos with PORT=6502"
make -C ./$PICOOS/picoos/ PORT=6502
echo "making snakes with PORT=6502"
make -C ./$PICOOS/contrib/demos/6502-c64/snakes PORT=6502
cp ./$PICOOS/picoos/out/6502/rel/snakes.c64 .
