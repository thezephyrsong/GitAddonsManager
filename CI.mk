# Intended for gitlab CI

qt7z:=6.7.2-0-202406110335£-Windows-Windows_10_22H2-Mingw-Windows-Windows_10_22H2-X86_64.7z
pwd:=$(shell pwd)
wineenv=$(WINEPREFIX) $(WINEENV) WINEDEBUG=-all
winepath=$(wineenv) winepath
wine=$(WINEPATH) $(wineenv) wine
winepwd:=$(shell $(wine) winepath -w $(pwd))
WINEPREFIX=WINEPREFIX="$(pwd)/wine"
WINEPATH=WINEPATH='C:\Program Files (x86)\CMake\bin;$(shell $(winepath) -w "$(pwd)/mingw64/bin");$(shell $(winepath) -w "$(pwd)/Qt/6.7.2/mingw_64/bin")' QTDIR="Z:$(pwd)/Qt/6.7.2/mingw_64/"

.PHONY: build winbuild windeploy linuxdeploy finalize

finalize:
	cd GitAddonsManager && echo "# DO NOT EDIT" > .installedFiles && find ! -path . >> .installedFiles
	
build:
	mkdir -p build && cd build && cmake ../ -DGIT_DESCRIBE="$(shell git describe --tags --long)" -DGAM_BUILD_NAME=Linux_x64 -G"Ninja" -DCMAKE_BUILD_TYPE=Release --install-prefix="$(pwd)/release" && ninja && ninja install
	
linuxdeploy: build
	mkdir GitAddonsManager
	cp release/bin/GitAddonsManager -f GitAddonsManager/GitAddonsManager
	
cmake-3.30.1-windows-x86_64.msi:
	wget https://github.com/Kitware/CMake/releases/download/v3.30.1/cmake-3.30.1-windows-x86_64.msi
	
wine/drive_c/Program\ Files\ (x86)/CMake: cmake-3.30.1-windows-x86_64.msi
	$(wine) msiexec /quiet /i cmake-3.30.1-windows-x86_64.msi
	
qt-opensource-windows-x86-6.7.2.exe:
	wget http://download.qt.io/official_releases/qt/6.7/6.7.2/qt-opensource-windows-x86-6.7.2.exe
	
6.7.2-0-202406110335%-Windows-Windows_10_22H2-Mingw-Windows-Windows_10_22H2-X86_64.7z:
	wget https://download.qt.io/online/qtsdkrepository/windows_x86/desktop/qt6_672/qt.qt6.672.win64_mingw/$@
	
qt-modules:=Qt/6.7.2/mingw_64/bin/qmake.exe Qt/6.7.2/mingw_64/bin/windeployqt.exe Qt/6.7.2/mingw_64/bin/Qt6Svg.dll Qt/6.7.2/mingw_64/bin/Qt6Quick.dll Qt/6.7.2/mingw_64/qml/QtQuick Qt/6.7.2/mingw_64/translations Qt/6.7.2/mingw_64/bin/Qt5Compat.dll
	
Qt/6.7.2/mingw_64/bin/qmake.exe: $(subst £,qtbase,$(qt7z))

Qt/6.7.2/mingw_64/bin/windeployqt.exe: $(subst £,qttools,$(qt7z))

Qt/6.7.2/mingw_64/bin/Qt6QuickControls2.dll: $(subst £,qtquickcontrols2,$(qt7z))

Qt/6.7.2/mingw_64/qml/QtQuick/PrivateWidgets/widgetsplugin.dll: $(subst £,qtquickcontrols,$(qt7z))

Qt/6.7.2/mingw_64/bin/Qt6Quick.dll: $(subst £,qtdeclarative,$(qt7z))

Qt/6.7.2/mingw_64/bin/Qt6Svg.dll: $(subst £,qtsvg,$(qt7z))

Qt/6.7.2/mingw_64/qml/QtGraphicalEffects: $(subst £,qtgraphicaleffects,$(qt7z))

Qt/6.7.2/mingw_64/translations: $(subst £,qttranslations,$(qt7z))

$(qt-modules):
	7zr x $< -oQt -aos
	touch -c $@

Qt/6.7.2/mingw_64/bin/Qt5Compat.dll:
	wget https://download.qt.io/online/qtsdkrepository/windows_x86/desktop/qt6_672/qt.qt6.672.qt5compat.win64_mingw/6.7.2-0-202406110335qt5compat-Windows-Windows_10_22H2-Mingw-Windows-Windows_10_22H2-X86_64.7z
	7zr x 6.7.2-0-202406110335qt5compat-Windows-Windows_10_22H2-Mingw-Windows-Windows_10_22H2-X86_64.7z -oQt -aos
	touch -c $@

Qt/6.7.2/mingw_64/bin/qt.conf: Qt/6.7.2/mingw_64/bin/qmake.exe
	echo "[paths]" > $(pwd)/Qt/6.7.2/mingw_64/bin/qt.conf
	echo "Prefix=../" >> $(pwd)/Qt/6.7.2/mingw_64/bin/qt.conf

Qt: $(qt-modules) Qt/6.7.2/mingw_64/bin/qt.conf
	sed -i.bak 's/Enterprise/OpenSource/' $(pwd)/Qt/6.7.2/mingw_64/mkspecs/qconfig.pri

mingw.7z:
	wget https://github.com/niXman/mingw-builds-binaries/releases/download/13.2.0-rt_v11-rev0/x86_64-13.2.0-release-posix-seh-msvcrt-rt_v11-rev0.7z -O mingw.7z
	
mingw64/bin/gcc.exe: mingw.7z
	7zr x mingw.7z
	touch -c $@
	
libgit2-v0.27.2.tar.gz:
	wget https://github.com/libgit2/libgit2/archive/v0.27.2.tar.gz -O libgit2-v0.27.2.tar.gz
	
libgit2-0.27.2/CMakeLists.txt: libgit2-v0.27.2.tar.gz
	tar -xf libgit2-v0.27.2.tar.gz
	touch -c $@
	
libgit2-0.27.2/release/bin/libgit2.dll: wine/drive_c/Program\ Files\ (x86)/CMake libgit2-0.27.2/CMakeLists.txt mingw64/bin/gcc.exe
	mkdir -p libgit2-0.27.2/build
	cd libgit2-0.27.2/build && $(wine) cmake .. -DCMAKE_BINARY_DIR=libgit2-0.27.2/build -G"MinGW Makefiles" -DBUILD_CLAR=OFF --install-prefix="$(winepwd)/libgit2-0.27.2/release" -DCMAKE_BUILD_TYPE=Release
	$(wine) mingw32-make -C libgit2-0.27.2/build -j $(shell nproc)
	$(wine) mingw32-make -C libgit2-0.27.2/build install
	
zlib/CMakeLists.txt:
	git clone https://github.com/madler/zlib.git
	
zlib/build/libzlib.dll: zlib/CMakeLists.txt
	mkdir -p zlib/build
	cd zlib/build && $(wine) cmake .. -DCMAKE_BINARY_DIR=zlib/build -G"MinGW Makefiles" --install-prefix="$(winepwd)/zlib/release" -DCMAKE_BUILD_TYPE=Release
	$(wine) mingw32-make -C zlib/build -j $(shell nproc)
	$(wine) mingw32-make -C zlib/build install
	
quazip/CMakeLists.txt:
	git clone https://github.com/stachenov/quazip.git
	cd quazip
	
quazip/release/bin/libquazip1-qt6.dll: quazip/CMakeLists.txt
	mkdir -p quazip/build
	cd quazip/build && $(wine) cmake ../ -G"MinGW Makefiles" -DZLIB_LIBRARY=$(pwd)/zlib/build/libzlib.dll.a -DZLIB_INCLUDE_DIR=$(pwd)/zlib -DQUAZIP_ENABLE_TESTS=OFF -DQUAZIP_BZIP2=OFF --install-prefix="$(winepwd)/quazip/release" -DCMAKE_BUILD_TYPE=Release
	$(wine) mingw32-make -C quazip/build -j $(shell nproc)
	$(wine) mingw32-make -C quazip/build install

build_win64/GitAddonsManager.exe: Qt libgit2-0.27.2/release/bin/libgit2.dll zlib/build/libzlib.dll quazip/release/bin/libquazip1-qt6.dll
	mkdir -p build_win64 && cd build_win64 && $(wine) cmake ../ -DGIT_DESCRIBE="$(shell git describe --tags --long)" -DGAM_BUILD_NAME=Win64 -G"MinGW Makefiles" -DQuaZip-Qt6_DIR=$(pwd)/quazip/release/lib/cmake/QuaZip-Qt6-1.4/ -DLIBGIT2_LIBRARY=$(pwd)/libgit2-0.27.2/release/bin/libgit2.dll -DLIBGIT2_INCLUDE_DIR=$(pwd)/libgit2-0.27.2/release/include -DZLIB_LIBRARY=$(pwd)/zlib/build/libzlib.dll.a -DZLIB_INCLUDE_DIR=$(pwd)/zlib -DCMAKE_BUILD_TYPE=Release --install-prefix="$(winepwd)/release"
	$(wine) mingw32-make -C build_win64 -j $(shell nproc)
	$(wine) mingw32-make -C build_win64 install

winbuild: build_win64/GitAddonsManager.exe

build_win64/release/GitAddonsManager.exe: winbuild

windeploy: build_win64/release/GitAddonsManager.exe libgit2-0.27.2/release/bin/libgit2.dll
	mkdir -p GitAddonsManager/
	cp libgit2-0.27.2/release/bin/libgit2.dll -f GitAddonsManager/libgit2.dll
	cp zlib/build/libzlib.dll -f GitAddonsManager/libzlib.dll
	cp quazip/release/bin/libquazip1-qt6.dll -f GitAddonsManager/libquazip1-qt6.dll
	cp Qt/6.7.2/mingw_64/bin/Qt6Core5Compat.dll GitAddonsManager/Qt6Core5Compat.dll
	cp mingw64/bin/libstdc++-6.dll GitAddonsManager/libstdc++-6.dll
	cp mingw64/bin/libgcc_s_seh-1.dll GitAddonsManager/libgcc_s_seh-1.dll
	cp mingw64/bin/libwinpthread-1.dll GitAddonsManager/libwinpthread-1.dll
	cp release/bin/GitAddonsManager.exe -f GitAddonsManager/
	mkdir -p tmpqmls && cp -f *.qml tmpqmls/
	$(wine) Qt/6.7.2/mingw_64/bin/windeployqt.exe --qmldir tmpqmls GitAddonsManager/GitAddonsManager.exe
	rm -fr tmpqmls
