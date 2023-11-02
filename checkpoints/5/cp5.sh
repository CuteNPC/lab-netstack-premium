#!/bin/bash

# run this with "sudo"

cd vnetUtils

dos2unix ./examples/* ./helper/* >/dev/null 2> /dev/null
chmod -R +x .  >/dev/null 2> /dev/null


echo "Create a network topology structure..."
cd examples
sudo bash ./makeVNet < ../../checkpoints/5/net.txt > /dev/null 2> /dev/null

echo "Run six 'looptest' and redirect STDOUT"
echo "This test will last for 40 seconds. Please wait."
cd ../helper

sudo bash ./execNS ns1 sudo  ../../build/test/lab2/looptest flag1    >  ../../checkpoints/5/host1.log    2> /dev/null &
sudo bash ./execNS ns2 sudo  ../../build/test/lab2/looptest flag2    >  ../../checkpoints/5/host2.log    2> /dev/null &
sudo bash ./execNS ns3 sudo  ../../build/test/lab2/looptest flag3    >  ../../checkpoints/5/host3.log    2> /dev/null &
sudo bash ./execNS ns4 sudo  ../../build/test/lab2/looptest flag4    >  ../../checkpoints/5/host4.log    2> /dev/null &
sudo bash ./execNS ns5 sudo  ../../build/test/lab2/looptest flag5    >  ../../checkpoints/5/host5.log    2> /dev/null &
sudo bash ./execNS ns6 sudo  ../../build/test/lab2/looptest flag6    >  ../../checkpoints/5/host6.log    2> /dev/null &

sleep 20

pid=$(pgrep -f "looptest flag5")
kill $pid

sleep 20


echo "Remove the network topology structure."
echo "Please check checkpoints/5/hostXXX.log for the result."
cd ../examples
sudo bash ./removeVNet < ../../checkpoints/5/net.txt
