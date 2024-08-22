@echo off

cd /d %~dp0

magick icon_24.png icon_24.tga
magick icon_48.png icon_48.tga

%CTRSDK_ROOT%/tools/CommandLineTools/ctr_TexturePackager32.exe -l "icon_24.tga" -dsl
%CTRSDK_ROOT%/tools/CommandLineTools/ctr_TexturePackager32.exe -l "icon_48.tga" -dsl
