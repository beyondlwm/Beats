cmake.exe -G"MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=android.toolchain.cmake -DCMAKE_MAKE_PROGRAM="%ANDROID_NDK%\prebuilt\windows\bin\make.exe" -DANDROID_TOOLCHAIN_NAME=arm-linux-androideabi-4.9 -DLIBRARY_OUTPUT_PATH_ROOT="" -DCMAKE_BUILD_TYPE=Debug -DANDROID_NATIVE_API_LEVEL=16

cmake.exe --build .