#!/bin/bash

for f in *; do mv "$f" `echo "$f" | sed s/\true$/true.csv/`; done
