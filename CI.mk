# Intended for gitlab CI

pwd:=$(shell pwd)
wineenv=$(WINEPREFIX) $(WINEPATH) WINEDEBUG=-all
winepath=$(wineenv) winepath
wine=$(wineenv) wine
WINEPREFIX:=WINEPREFIX="$(pwd)/wine"
WINEPATH:=WINEPATH='C:\Program Files (x86)\CMake\bin;$(shell $(winepath) -w "$(pwd)/mingw32/bin");$(shell $(winepath) -w "$(pwd)/git/bin")'
qmake_opts=QTDIR="Z:/$(pwd)/Qt/5.11.1/mingw53_32/bin/" "QMAKE_INCDIR+=Z:/$(pwd)/libgit2-0.27.2/include" "QMAKE_LIBDIR+=Z:/$(pwd)/libgit2-0.27.2/build/"

build:
	qt5-qmake . -o build/Makefile
	make -C build
	
cmake-3.12.0-rc3-win32-x86.msi:
	wget https://cmake.org/files/v3.12/cmake-3.12.0-rc3-win32-x86.msi
	
wine/drive_c/Program\ Files\ (x86)/CMake: cmake-3.12.0-rc3-win32-x86.msi
	$(wine) msiexec /quiet /i cmake-3.12.0-rc3-win32-x86.msi
	
qt-opensource-windows-x86-5.11.1.exe:
	wget http://download.qt.io/official_releases/qt/5.11/5.11.1/qt-opensource-windows-x86-5.11.1.exe
	
Qt: qt-opensource-windows-x86-5.11.1.exe
	$(wine) qt-opensource-windows-x86-5.11.1.exe --script install-qt.qs "PATH=$$($(winepath) -w "$$PWD/Qt")"
mingw.7z:
	wget https://vorboss.dl.sourceforge.net/project/mingw-w64/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/8.1.0/threads-posix/dwarf/i686-8.1.0-release-posix-dwarf-rt_v6-rev0.7z -O mingw.7z
	
mingw32/bin/gcc.exe: mingw.7z
	7z x mingw.7z
	touch -c mingw32/bin/gcc.exe
	
libgit2-v0.27.2.tar.gz:
	wget https://github.com/libgit2/libgit2/archive/v0.27.2.tar.gz -O libgit2-v0.27.2.tar.gz
	
libgit2-0.27.2/CMakeLists.txt: libgit2-v0.27.2.tar.gz
	tar -xf libgit2-v0.27.2.tar.gz
	touch -c libgit2-0.27.2/CMakeLists.txt
	
libgit2-0.27.2/build/libgit2.dll: wine/drive_c/Program\ Files\ (x86)/CMake libgit2-0.27.2/CMakeLists.txt mingw32/bin/gcc.exe
	mkdir -p libgit2-0.27.2/build
	cd libgit2-0.27.2/build && $(wine) cmake .. -DCMAKE_BINARY_DIR=libgit2-0.27.2/build -G"MinGW Makefiles" -DBUILD_CLAR=OFF
	$(wine) mingw32-make -C libgit2-0.27.2/build -j $(shell nproc)
	
PortableGit-2.18.0-32-bit.7z.exe:
	wget https://github.com/git-for-windows/git/releases/download/v2.18.0.windows.1/PortableGit-2.18.0-32-bit.7z.exe
	
git/bin/git.exe: PortableGit-2.18.0-32-bit.7z.exe
	7z -ogit x PortableGit-2.18.0-32-bit.7z.exe
	touch -c git/bin/git.exe
	
build_win32/GitAddonsManager.exe: Qt libgit2-0.27.2/build/libgit2.dll mingw32/bin/gcc.exe git/bin/git.exe
	mkdir -p build_win32 && cd build_win32 && $(wine) ../Qt/5.11.1/mingw53_32/bin/qmake.exe -spec win32-g++ ../GitAddonsManager.pro $(qmake_opts)
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
