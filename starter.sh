#!/usr/bin/env bash

exec="cmake-build-debug/eglue_client"

$exec --load /home/matteo/Documents/eGlue/eglue-server/try.ini
$exec --get Section1.Value1
$exec --set Section1.Value1 15
