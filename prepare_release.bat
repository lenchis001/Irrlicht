set PATH=%PATH%;C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\

rm -rf build out

mkdir build
cd build

cmake .. -A Win32
cmake --build . --config=Release -j8
cd ..

rm -rf Release

mkdir Release\Plugins\V1
cp build/Watercolor/WelcomeWindow/Release/WelcomeWindow.exe Release/Watercolor.exe
cp build/TLauncher/Release/TLauncher.exe Release/TLauncher.exe
cp build/Irrlicht/Release/Irrlicht.dll Release/Irrlicht.dll
cp build/packages/OpenAL.Soft/runtimes/win-x86/native/OpenAL32.dll Release/OpenAL32.dll
cp build/packages/OpenAL.Soft/runtimes/win-x86/native/soft_oal.dll Release/soft_oal.dll
cp -r Watercolor/MainWindow/Resources Release/Resources
cp build/Watercolor/Plugins/GuiEditor/Release/GuiEditor.dll Release/Plugins/V1/GuiEditor.dll
