cmake.exe -G"MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=android.toolchain.cmake -DCMAKE_MAKE_PROGRAM="%ANDROID_NDK%\prebuilt\windows\bin\make.exe" -DCMAKE_BUILD_TYPE=Release -DANDROID_NATIVE_API_LEVEL=16

cmake.exe --build .