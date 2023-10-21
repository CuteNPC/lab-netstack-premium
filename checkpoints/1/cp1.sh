#!/bin/bash

# run this with "sudo"

cd vnetUtils

dos2unix ./examples/* ./helper/*  >/dev/null 2> /dev/null
chmod -R +x .  >/dev/null 2> /dev/null


echo "Create a network topology structure..."
cd examples
sudo bash ./makeVNet < ../../checkpoints/1/net.txt


echo "Run 'detect' and redirect STDOUT to output.log"
cd ../helper
sudo bash ./execNS ns1  ../../build/test/lab1/detect <../../checkpoints/1/input.txt  >  ../../checkpoints/1/output.log 2> /dev/null


echo "Remove the network topology structure."
cd ../examples
sudo bash ./removeVNet < ../../checkpoints/1/net.txt

echo "Please check checkpoints/1/output.log for the result."