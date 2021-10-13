#!/usr/bin/env bash

#set -e

eglue_client="cmake-build-debug/eglue_client"
eglue_server="../eglue-server/cmake-build-debug/eglue_server"
eglue_config="/home/matteo/Documents/eGlue/eglue-server/try.ini"

pid_server=0
server_port=12345

fun run_server() {
  $eglue_server &
  error=$?
  echo server error $?
  pid_server=$!
  echo "Server up and running with pid " $pid_server " on port " $server_port
}

func server_error() {

  kill $pid_server
}

fun test1() {
  run_server
  ss -ltupn | grep $server_port
  kill -SIGINT $pid_server
  ss -ltupn | grep $server_port
}

fun test2() {
  tmp_config="/tmp/config.ini"
  cp $eglue_config $tmp_config

  run_server

  a="$($eglue_client --load ${tmp_config} 2>&1)"
  a="$($eglue_client --get Section1.Value1 2>&1)"
  a="$($eglue_client --get Section2.Value1 2>&1)"
  a="$($eglue_client --set Section1.Value1 15 2>&1)"
  echo client error ${a}

  kill -SIGINT $pid_server
}

test1

test2
