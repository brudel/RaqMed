let
  pkgsconfig = {};
in
{ pkgs ? import <nixpkgs> pkgsconfig }: with pkgs;

mkShell rec {
  name = "C++ Qt Build Shell";

  nativeBuildInputs = [
    qt6.qtbase
    qt6.qtcharts
    postgresql
    cmake
    ninja
    qtcreator
    delve
    clang-tools
    bear
  ];

  shellHook = ''
    name="${name}"
    export QT_QPA_PLATFORMTHEME=generic
  '';
}
