#!/bin/bash

# run this with "sudo"

cd vnetUtils

dos2unix ./examples/* ./helper/* >/dev/null 2> /dev/null
chmod -R +x .  >/dev/null 2> /dev/null


echo "Create a network topology structure..."
cd examples
sudo bash ./makeVNet < ../../challenge/4/net.txt

echo "Run slow_perf_client and perf_server and redirect STDOUT"
echo "This test will last for 300 seconds. Please wait."
cd ../helper

nohup sudo bash ./execNS ns1 sudo  wireshark > /dev/null  2> /dev/null  &
sleep 10

sudo bash ./execNS ns2 sudo  ../../build/test/lab3/perf_server                    >  ../../challenge/4/host2.log    2> /dev/null &
sleep 5
sudo bash ./execNS ns1 sudo  ../../build/test/lab3/slow_perf_client "10.100.1.2"  >  ../../challenge/4/host1.log    2> /dev/null &

sleep 300

echo "Remove the network topology structure."
echo "Please check challenge/4/hostX.log for the result."
cd ../examples
sudo bash ./removeVNet < ../../challenge/4/net.txt
