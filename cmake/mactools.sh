#!/bin/bash

if [ -d "tools/curl" ]; then
  echo "Curl installed"    
else
  echo "Curl not installed"
  wget "https://github.com/stunnel/static-curl/releases/download/8.10.1/curl-macos-x86_64-dev-8.10.1.tar.xz"
  tar -xvf "curl-macos-x86_64-dev-8.10.1.tar.xz" 
  rm "curl-macos-x86_64-dev-8.10.1.tar.xz"
  mv "curl-x86_64" "curl"
  mv "curl" "tools/curl"
fi

if [ -d "tools/clang" ]; then
  echo "Clang installed"
else
  echo "Clang not installed"
  wget "https://github.com/ARM-software/LLVM-embedded-toolchain-for-Arm/releases/download/release-17.0.1/LLVMEmbeddedToolchainForArm-17.0.1-Darwin.dmg"
  hdiutil attach "LLVMEmbeddedToolchainForArm-17.0.1-Darwin.dmg"
  cp -R "/Volumes/LLVMEmbeddedToolchainForArm-17.0.1-Darwin/LLVMEmbeddedToolchainForArm-17.0.1-Darwin" "clang"
  hdiutil detach "/Volumes/LLVMEmbeddedToolchainForArm-17.0.1-Darwin"
  rm  "LLVMEmbeddedToolchainForArm-17.0.1-Darwin.dmg"
  mv "clang" "tools/clang"




fi

if [ -e "tools/linkle" ]; then
  echo "Linkle Installed"
else
  echo "Linkle not installed"
  wget "https://github.com/MegatonHammer/linkle/releases/download/v0.2.11/linkle-refs.tags.v0.2.11-x86_64-apple-darwin.zip"
  unar "linkle-refs.tags.v0.2.11-x86_64-apple-darwin.zip"
  sudo chmod a+x "linkle"
  rm "linkle-refs.tags.v0.2.11-x86_64-apple-darwin.zip"
  mv "linkle" "tools/linkle"
fi 

if [ -e "tools/npdmtool-mac" ]; then
  echo "npdmtool installed"
else
  echo "npdmtool not installed"
  git clone "https://github.com/switchbrew/switch-tools.git"
  cd "switch-tools"
  brew install autoconf automake libtool lz4 make 
  aclocal
  autoconf
  automake --add-missing
  ./configure
  make
  cd ..
  mv "switch-tools/npdmtool" "tools/npdmtool-mac"
  rm -r "switch-tools"
fi
export EXE_EXE=-mac