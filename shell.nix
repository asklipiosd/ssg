# shell.nix
{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  nativeBuildInputs = [
    pkgs.pkg-config
    pkgs.gnumake
    pkgs.gcc
  ];

  buildInputs = [
    pkgs.discount   # libmarkdown, mkdio.h, and the markdown binary
  ];
}
