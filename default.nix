{ pkgs ? import <nixpkgs> { } }:

{
  desktop = pkgs.callPackage ./desktop/package.nix { };
}
