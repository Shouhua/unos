### 2022.02.16
- LD_SHOW_AUXV=1 ls
- pushd/popd/dirs
```shell
# pushd与cd类似，进入某个指定目录，但是pushd还额外将指定目录加入到stack的最上面
# popd则相反，弹出最上面的目录，然后进入当前指定的目录
# dirs用于显示当前stack内容
pushd ~
echo "hello, world" > helo.txt
rm helo.txt
popd
dirs -p -v

pushd +3
popd +3
pushd -n
# 以下2个命令一样，进入上次的目录，与stack无关
cd -
cd $OLDPWD
```
### 2022.02.08
- [compile opencv]https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html)
- [compile ffmpeg](https://trac.ffmpeg.org/wiki/CompilationGuide/Generic)
- -extra-cflags, -extra-ldflags需要指定额外的CFLAGS, LDFLAGS
- ```pkg-config --libs libavformat```, ```sudo ldconfig -p```
- sudo apt-get build-deps ffmpeg, 需要修改source.list，放开开头的deb-src
- ffmpeg编译-pkgconfigdir
- ```pkg-config --static --libs package_name```标识静态库需要的标识，包括package_name所有依赖库
- 验证ffmpeg是否生成pc文件(package config file)