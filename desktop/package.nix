{
  lib,
  stdenv,
  cmake,
  qt6,
  libpq,
}:

stdenv.mkDerivation {
  pname = "raqmed";
  version = "1.3";

  src = lib.fileset.toSource {
    root = ../.;
    fileset = lib.fileset.unions [
      ./CMakeLists.txt
      ./src
      ./assets
      ../LICENSE
    ];
  };
  sourceRoot = "source/desktop";

  nativeBuildInputs = [
    cmake
    qt6.wrapQtAppsHook
  ];

  qtWrapperArgs = [ "--set QT_QPA_PLATFORMTHEME generic" ];

  buildInputs = [
    qt6.qtbase
    qt6.qtcharts
    qt6.qtwayland
    libpq
  ];

  meta = {
    description = "Patient and appointment manager for a pediatric clinic";
    license = lib.licenses.eupl12;
    mainProgram = "raqmed";
    platforms = lib.platforms.linux;
  };
}
