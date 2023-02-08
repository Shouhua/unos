### 2022.02.08
- [compile opencv]https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html)
- [compile ffmpeg](https://trac.ffmpeg.org/wiki/CompilationGuide/Generic)
- -extra-cflags, -extra-ldflags需要指定额外的CFLAGS, LDFLAGS
- ```pkg-config --libs libavformat```, ```sudo ldconfig -p```
- sudo apt-get build-deps ffmpeg, 需要修改source.list，放开开头的deb-src
- ffmpeg编译-pkgconfigdir
- ```pkg-config --static --libs package_name```标识静态库需要的标识，包括package_name所有依赖库
- 验证ffmpeg是否生成pc文件(package config file)