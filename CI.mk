# Intended for gitlab CI

qt7z:=5.11.1-0-201806180838£-Windows-Windows_7-Mingw53-Windows-Windows_7-X86.7z
pwd:=$(shell pwd)
wineenv=$(WINEPREFIX) $(WINEENV) WINEDEBUG=-all
winepath=$(wineenv) winepath
wine=$(WINEPATH) $(wineenv) wine
WINEPREFIX=WINEPREFIX="$(pwd)/wine"
WINEPATH=WINEPATH='C:\Program Files (x86)\CMake\bin;$(shell $(winepath) -w "$(pwd)/mingw32/bin");$(shell $(winepath) -w "$(pwd)/Qt/5.11.1/mingw53_32/bin")' QTDIR="Z:$(pwd)/Qt/5.11.1/mingw53_32/"
qmake_opts="QMAKE_INCDIR+=Z:$(pwd)/libgit2-0.27.2/include" "QMAKE_LIBDIR+=Z:$(pwd)/libgit2-0.27.2/build/" -spec win32-g++
qmake=$(wine) $(pwd)/Qt/5.11.1/mingw53_32/bin/qmake.exe

.PHONY: build
build:
	mkdir -p build && cd build && qmake-qt5 .. -spec linux-g++ && make
	
cmake-3.12.0-rc3-win32-x86.msi:
	wget https://cmake.org/files/v3.12/cmake-3.12.0-rc3-win32-x86.msi
	
wine/drive_c/Program\ Files\ (x86)/CMake: cmake-3.12.0-rc3-win32-x86.msi
	$(wine) msiexec /quiet /i cmake-3.12.0-rc3-win32-x86.msi
	
qt-opensource-windows-x86-5.11.1.exe:
	wget http://download.qt.io/official_releases/qt/5.11/5.11.1/qt-opensource-windows-x86-5.11.1.exe
	
5.11.1-0-201806180838%-Windows-Windows_7-Mingw53-Windows-Windows_7-X86.7z:
	wget http://ftp.fau.de/qtproject/online/qtsdkrepository/windows_x86/desktop/qt5_5111/qt.qt5.5111.win32_mingw53/$@
	
qt-modules:=Qt/5.11.1/mingw53_32/bin/qmake.exe Qt/5.11.1/mingw53_32/bin/windeployqt.exe Qt/5.11.1/mingw53_32/bin/Qt5QuickControls2.dll Qt/5.11.1/mingw53_32/bin/Qt5Svg.dll Qt/5.11.1/mingw53_32/bin/Qt5Quick.dll Qt/5.11.1/mingw53_32/qml/QtQuick Qt/5.11.1/mingw53_32/qml/QtQuick/PrivateWidgets/widgetsplugin.dll Qt/5.11.1/mingw53_32/qml/QtGraphicalEffects Qt/5.11.1/mingw53_32/translations
	
Qt/5.11.1/mingw53_32/bin/qmake.exe: $(subst £,qtbase,$(qt7z))

Qt/5.11.1/mingw53_32/bin/windeployqt.exe: $(subst £,qttools,$(qt7z))

Qt/5.11.1/mingw53_32/bin/Qt5QuickControls2.dll: $(subst £,qtquickcontrols2,$(qt7z))

Qt/5.11.1/mingw53_32/qml/QtQuick/PrivateWidgets/widgetsplugin.dll: $(subst £,qtquickcontrols,$(qt7z))

Qt/5.11.1/mingw53_32/bin/Qt5Quick.dll: $(subst £,qtdeclarative,$(qt7z))

Qt/5.11.1/mingw53_32/bin/Qt5Svg.dll: $(subst £,qtsvg,$(qt7z))

Qt/5.11.1/mingw53_32/qml/QtGraphicalEffects: $(subst £,qtgraphicaleffects,$(qt7z))

Qt/5.11.1/mingw53_32/translations: $(subst £,qttranslations,$(qt7z))

$(qt-modules):
	7zr x $< -oQt -aos
	touch -c $@
	
Qt/5.11.1/mingw53_32/bin/qt.conf: Qt/5.11.1/mingw53_32/bin/qmake.exe
	echo "[paths]" > $(pwd)/Qt/5.11.1/mingw53_32/bin/qt.conf
	echo "Prefix=../" >> $(pwd)/Qt/5.11.1/mingw53_32/bin/qt.conf

Qt: $(qt-modules) Qt/5.11.1/mingw53_32/bin/qt.conf
	sed -i.bak 's/Enterprise/OpenSource/' $(pwd)/Qt/5.11.1/mingw53_32/mkspecs/qconfig.pri

mingw.7z:
	wget https://vorboss.dl.sourceforge.net/project/mingw-w64/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/8.1.0/threads-posix/dwarf/i686-8.1.0-release-posix-dwarf-rt_v6-rev0.7z -O mingw.7z
	
mingw32/bin/gcc.exe: mingw.7z
	7zr x mingw.7z
	touch -c $@
	
libgit2-v0.27.2.tar.gz:
	wget https://github.com/libgit2/libgit2/archive/v0.27.2.tar.gz -O libgit2-v0.27.2.tar.gz
	
libgit2-0.27.2/CMakeLists.txt: libgit2-v0.27.2.tar.gz
	tar -xf libgit2-v0.27.2.tar.gz
	touch -c $@
	
libgit2-0.27.2/build/libgit2.dll: wine/drive_c/Program\ Files\ (x86)/CMake libgit2-0.27.2/CMakeLists.txt mingw32/bin/gcc.exe
	mkdir -p libgit2-0.27.2/build
	cd libgit2-0.27.2/build && $(wine) cmake .. -DCMAKE_BINARY_DIR=libgit2-0.27.2/build -G"MinGW Makefiles" -DBUILD_CLAR=OFF
	$(wine) mingw32-make -C libgit2-0.27.2/build -j $(shell nproc)
	
build_win32/GitAddonsManager.exe: Qt libgit2-0.27.2/build/libgit2.dll
	mkdir -p build_win32 && cd build_win32 && $(qmake) $(qmake_opts) ../GitAddonsManager.pro GIT_DESCRIBE="$(shell git describe --tags)"
	$(wine) mingw32-make -C build_win32

winbuild: build_win32/GitAddonsManager.exe

build_win32/release/GitAddonsManager.exe: winbuild

windeploy: build_win32/release/GitAddonsManager.exe libgit2-0.27.2/build/libgit2.dll
	mkdir -p build_win32/deploy/
	cp libgit2-0.27.2/build/libgit2.dll -f build_win32/deploy/libgit2.dll
	cp build_win32/release/GitAddonsManager.exe -f build_win32/deploy/
	mkdir -p tmpqmls && cp -f *.qml tmpqmls/
	$(wine) Qt/5.11.1/mingw53_32/bin/windeployqt.exe --qmldir tmpqmls build_win32/deploy/GitAddonsManager.exe
	rm -fr tmpqmls
