#!/bin/bash

# run this with "sudo"

cd vnetUtils

dos2unix ./examples/* ./helper/* >/dev/null 2> /dev/null
chmod -R +x .  >/dev/null 2> /dev/null

echo "Create a network topology structure..."
cd examples
sudo bash ./makeVNet < ../../checkpoints/7/net.txt

echo "Run echo_client and echo_server and redirect STDOUT"
echo "This test will last for about 60 seconds. Please wait."
cd ../helper

nohup sudo bash ./execNS ns1 sudo  wireshark > /dev/null  2> /dev/null  &
sleep 10

sudo bash ./execNS ns1 sudo  ../../build/test/lab3/echo_client "10.100.1.2" > ../../checkpoints/7/host1.log    2> /dev/null &
sudo bash ./execNS ns2 sudo  ../../build/test/lab3/echo_server              > ../../checkpoints/7/host2.log    2> /dev/null &

sleep 60

echo "Remove the network topology structure."
echo "Please check checkpoints/7/hostX.log for the result."
cd ../examples
sudo bash ./removeVNet < ../../checkpoints/7/net.txt